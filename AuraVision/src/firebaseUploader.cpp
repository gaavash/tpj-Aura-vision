#include "firebaseUploader.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

void firebaseUpload(String mode,
                    String address,
                    String lastObject,
                    String gpsSignal,
                    String wifiStatus)
{
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ Firebase Upload Skipped — WiFi offline");
        return;
    }

    HTTPClient http;

    // Firestore REST API (PATCH to update document)
    String url =
        "https://firestore.googleapis.com/v1/projects/auravision-d0c06/"
        "databases/(default)/documents/deviceData/live"
        "?updateMask.fieldPaths=mode"
        "&updateMask.fieldPaths=address"
        "&updateMask.fieldPaths=lastObject"
        "&updateMask.fieldPaths=timestamp"
        "&updateMask.fieldPaths=gpsSignal"
        "&updateMask.fieldPaths=wifi";

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Use ESP32 NTP time only (seconds since 1970)
    unsigned long unixTime = time(nullptr);
    if (unixTime < 100000) {
        // NTP not synced yet, fall back to 0
        unixTime = 0;
    }

    StaticJsonDocument<512> doc;

    doc["fields"]["mode"]["stringValue"]       = mode;
    doc["fields"]["address"]["stringValue"]    = address;
    doc["fields"]["lastObject"]["stringValue"] = lastObject;
    // Firestore expects integerValue as a string
    doc["fields"]["timestamp"]["integerValue"] = String(unixTime);
    doc["fields"]["gpsSignal"]["stringValue"]  = gpsSignal;
    doc["fields"]["wifi"]["stringValue"]       = wifiStatus;

    String json;
    serializeJson(doc, json);

    int code = http.PATCH(json);
    http.end();

    Serial.print("📡 Firestore Upload → Status: ");
    Serial.println(code);
}
