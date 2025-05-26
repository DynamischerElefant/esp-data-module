#include <WiFi.h>
#define ESPNOW_WIFI_CHANNEL 6
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.println("ESP-NOW Example - Broadcast Master");
  Serial.println("Wi-Fi parameters:");
  Serial.println("  Mode: STA");
  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("  MAC Address: " + WiFi.macAddress());
}
