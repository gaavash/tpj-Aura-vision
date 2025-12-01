#include "gps.h"

GPSModule::GPSModule(int rx, int tx) {
    gpsSerial = new HardwareSerial(1);  // Use Serial1 (NO conflict)
}

void GPSModule::begin() {
    gpsSerial->begin(9600, SERIAL_8N1, 15, 14);  // RX=15, TX=14

    setupWiFi();

    Serial.println("GPS Module initialized");
    Serial.println("Waiting for GPS fix...");
}

void GPSModule::setupWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
        Serial.println("\nGPS: WiFi Connected");
    else
        Serial.println("\nGPS: WiFi Failed");
}

void GPSModule::update() {
    while (gpsSerial->available() > 0) {
        gps.encode(gpsSerial->read());
    }
}

bool GPSModule::hasFix() {
    return gps.location.isValid();
}

float GPSModule::getLatitude() {
    return gps.location.lat();
}

float GPSModule::getLongitude() {
    return gps.location.lng();
}

String GPSModule::getAddressFromCoordinates(float latitude, float longitude) {
    if (WiFi.status() != WL_CONNECTED) {
        return "No WiFi connection";
    }

    HTTPClient http;

    String url = "https://nominatim.openstreetmap.org/reverse?format=json&lat=";
    url += String(latitude, 6) + "&lon=" + String(longitude, 6);
    url += "&zoom=18&addressdetails=1";

    http.begin(url);
    http.addHeader("User-Agent", "AuraVisionESP32/1.0");

    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        http.end();

        int idx = payload.indexOf("\"display_name\":\"");
        if (idx != -1) {
            int endIdx = payload.indexOf("\"", idx + 16);
            String full = payload.substring(idx + 16, endIdx);

            int comma = full.indexOf(',');
            if (comma != -1) return full.substring(0, comma);

            return full;
        }
    }

    http.end();
    return "Address not available";
}

String GPSModule::getCurrentAddress() {
    // If no GPS fix → use fallback indoor address
    if (!hasFix()) {
        Serial.println("GPS: No fix — using fallback location");
        Serial.print("GPS Address: ");
        Serial.println(fallbackAddress);
        return fallbackAddress;
    }

    // Normal GPS mode
    String address = getAddressFromCoordinates(getLatitude(), getLongitude());
    Serial.print("GPS Address: ");
    Serial.println(address);
    return address;
}

void GPSModule::sendLocationToCloud() {
    if (WiFi.status() != WL_CONNECTED) return;

    // Choose fallback OR real GPS
    float lat = hasFix() ? getLatitude() : fallbackLat;
    float lng = hasFix() ? getLongitude() : fallbackLng;

    HTTPClient http;
    String url = String(cloudURL) + "?api_key=" + apiKey;

    url += "&field1=" + String(lat, 6);
    url += "&field2=" + String(lng, 6);
    url += "&field3=" + String(gps.speed.kmph());
    url += "&field4=" + String(gps.satellites.value());
    url += "&field5=" + String(millis() / 1000);
    url += "&field6=0";

    http.begin(url);
    http.GET();
    http.end();

    Serial.println("Location sent to cloud");
}
