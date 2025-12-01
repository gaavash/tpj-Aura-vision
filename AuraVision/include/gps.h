#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>

class GPSModule {
private:
    HardwareSerial* gpsSerial;
    TinyGPSPlus gps;

    // WiFi + Cloud
    const char* ssid = "Sisimanu";
    const char* password = "1234567890";
    const char* cloudURL = "http://api.thingspeak.com/update";
    const char* apiKey = "MESVKQQQ095KGR2S";

    // NEW — Indoor fallback location
    const float fallbackLat = 43.800590;
    const float fallbackLng = -79.344650;
    const char* fallbackAddress = "1750 Finch Avenue E, North York, ON, M2J 2X5";

    void setupWiFi();
    String getAddressFromCoordinates(float latitude, float longitude);

public:
    GPSModule(int rx = 15, int tx = 14);
    void begin();
    void update();
    bool hasFix();
    float getLatitude();
    float getLongitude();
    String getCurrentAddress();
    void sendLocationToCloud();
};

#endif
