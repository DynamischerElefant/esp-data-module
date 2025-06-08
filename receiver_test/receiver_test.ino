#include <esp_now.h>
#include <WiFi.h>

// === STRUCT DEFINITION ===
#define MAX_MEASUREMENTS 10
#define LABEL_LENGTH 5

typedef struct {
  uint8_t senderID;
  uint8_t numMeasurements;
  char labels[MAX_MEASUREMENTS][LABEL_LENGTH];
  float values[MAX_MEASUREMENTS];
} MeasurementPacket;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
  // Passive receiver
}

void onDataReceived(const esp_now_recv_info_t* recv_info, const uint8_t *incomingData, int len) {
  MeasurementPacket pkt;
  memcpy(&pkt, incomingData, sizeof(pkt));

  Serial.print("Received from sender ID: ");
  Serial.println(pkt.senderID);

  for (int i = 0; i < pkt.numMeasurements; i++) {
    Serial.print("  ");
    Serial.print(pkt.labels[i]);
    Serial.print(": ");
    Serial.println(pkt.values[i]);
  }

  Serial.println("------");
}
