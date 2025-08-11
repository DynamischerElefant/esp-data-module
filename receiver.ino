#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <InfluxDbClient.h> //for DB conenction
#include <InfluxDbCloud.h>
#include <map>
#include <string>

// === STRUCT DEFINITION ===
#define MAX_MEASUREMENTS 10
#define LABEL_LENGTH 5

typedef struct {
  uint8_t senderID;
  uint8_t numMeasurements;
  char labels[MAX_MEASUREMENTS][LABEL_LENGTH];
  float values[MAX_MEASUREMENTS];
} MeasurementPacket;
MeasurementPacket pkt;

std::map<std::string, float> measurements; //contains all measurements (use distinct names!)


// === InfluxDB Definition ===
// WiFi AP SSID
#define WIFI_SSID "SSID" 
// WiFi password
#define WIFI_PASSWORD "password" 
#define WIFI_CHANNEL 1 

#define INFLUXDB_URL "INFLUXURL"
#define INFLUXDB_TOKEN "INFLUX_TOKEN"
#define INFLUXDB_ORG "INFLUX_ORG"
#define INFLUXDB_BUCKET "INFLUX_BUCKET"

// Time zone info
#define TZ_INFO "UTC2"
#define DEVICE "ESP32"
// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("sensor point name");

bool new_vals = false;


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  // === ESPNOW-Setup ===
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceived);

  Serial.print("WiFi Channel: ");
  Serial.println(WiFi.channel());

  // === DB-Setup ===
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());
}

void loop() {
  if (new_vals) {
      // Clear fields for reusing the point. Tags will remain the same as set above.
    sensor.clearFields();

    // Store measured value into point
    for (const auto& pair : measurements) {
      Serial.print("Key: ");
      Serial.print(pair.first.c_str());
      Serial.print(", Value: ");
      Serial.println(pair.second);
      sensor.addField(pair.first.c_str(), pair.second);
    }
    

    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    // Check WiFi connection and reconnect if needed
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.println("Wifi connection lost");
    }
    // Write point
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
    new_vals = false;
  }
  delay(10);
}

void onDataReceived(const esp_now_recv_info_t* recv_info, const uint8_t *incomingData, int len) {
  memcpy(&pkt, incomingData, sizeof(pkt));

  Serial.print("Received from sender ID: ");
  Serial.println(pkt.senderID);

  for (int i = 0; i < pkt.numMeasurements; i++) {
    //Serial.print("  ");
    //Serial.print(pkt.labels[i]);
    //Serial.print(": ");
    //Serial.println(pkt.values[i]);

    std::string label = std::string(pkt.labels[i]);
    measurements[label] = pkt.values[i];
  }

  Serial.println("------");
  new_vals = true;
}
