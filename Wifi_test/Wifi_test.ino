#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "SSID";      // Your WiFi SSID
const char* password = "PASSWORD";    // Your WiFi Password
const char* server = "http://www.google.com"; // Server to request
const int LED_PIN = 2;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    delay(1000);
    
    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // Print ESP32 IP address
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { // Check if connected
        HTTPClient http;
        http.begin(server);  // Start HTTP request
        int httpCode = http.GET(); // Send GET request

        if (httpCode > 0) { // Success
            Serial.print("HTTP Response Code: ");
            Serial.println(httpCode);
            String payload = http.getString();
            Serial.println("Response:");
            Serial.println(payload); // Print the webpage content
            digitalWrite(LED_PIN, HIGH);
        } else {
            Serial.print("Error on HTTP request: ");
            Serial.println(httpCode);
            digitalWrite(LED_PIN, LOW);
        }

        http.end(); // Close connection
    } else {
        Serial.println("WiFi not connected...");
    }

    delay(10000); // Wait 10 seconds before next request
}
