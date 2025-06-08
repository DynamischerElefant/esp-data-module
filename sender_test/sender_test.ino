#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define WIFI_CHANNEL 1

// === STRUCT DEFINITION ===
#define MAX_MEASUREMENTS 10
#define LABEL_LENGTH 5  // 4 characters + null terminator

typedef struct {
  uint8_t senderID;
  uint8_t numMeasurements;
  char labels[MAX_MEASUREMENTS][LABEL_LENGTH];
  float values[MAX_MEASUREMENTS];
} MeasurementPacket;

// === CONFIGURATION ===
const uint8_t SENDER_ID = 1;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const int numMeasure = 3;
const char* dataLabels[numMeasure] = {"temp", "hum", "pres"};
float data[numMeasure] = {22.5, 60.0, 1012.8};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

void loop() {
  sendMeasurements();
  delay(10000);  // Send every 10 seconds
}

void sendMeasurements() {
  MeasurementPacket pkt;
  pkt.senderID = SENDER_ID;
  pkt.numMeasurements = numMeasure;
  for (int i = 0; i < numMeasure; i++) {
    strcpy(pkt.labels[i], dataLabels[i]);
    pkt.values[i] = data[i];
  }

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&pkt, sizeof(pkt));
  Serial.println(result == ESP_OK ? "Packet sent" : "Send error");
}
