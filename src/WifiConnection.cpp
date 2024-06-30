#include "WiFiConnection.h"

WiFiConnection::WiFiConnection(const char* ssid, const char* password) : ssid(ssid), password(password) {}

void WiFiConnection::connect() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}
