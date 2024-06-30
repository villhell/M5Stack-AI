#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <WiFi.h>

class WiFiConnection {
public:
    WiFiConnection(const char* ssid, const char* password);
    void connect();

private:
    const char* ssid;
    const char* password;
};

#endif // WIFI_CONNECTION_H
