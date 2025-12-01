// =================== AURAVISION - UPDATED MAIN (ARE YOU OK → EMERGENCY COUNTDOWN) ===================

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ArduinoJson.h>

#include "firebaseUploader.h"
#include "mpu_fall.h"
#include "audio.h"
#include "gps.h"
#include "camera.h"
#include "tof.h"
#include "oled.h"
#include "io_expander.h"

// ======================================================
// WiFi SETTINGS
// ======================================================
const char* WIFI_SSID     = "Sisimanu";
const char* WIFI_PASSWORD = "1234567890";

// ======================================================
// GLOBAL MODULE OBJECTS
// ======================================================
MPUFallDetector fallDetector;
AudioPlayer     audio;
GPSModule       gps;
CameraModule    camera;
ToFModule       tof;
OLED            oled;
IOExpander      ioExp;

// ======================================================
// SYSTEM STATES
// ======================================================
enum SystemState {
    STATE_NORMAL,
    STATE_EMERGENCY_COUNTDOWN,
    STATE_HELP_MODE
};

SystemState systemState = STATE_NORMAL;

// ======================================================
// EMERGENCY VARIABLES
// ======================================================
unsigned long emergencyStartTime = 0;
const unsigned long EMERGENCY_DURATION = 60000;
unsigned long lastEmergencyVibration = 0;  // <-- Added for pulsed vibration

// ======================================================
// TOF + CAMERA SETTINGS
// ======================================================
const int TOF_MIN_TRIGGER = 1000;
const int TOF_MAX_TRIGGER = 2000;

unsigned long lastCameraTrigger = 0;
const unsigned long CAMERA_COOLDOWN = 5000; // Added cooldown for camera
bool cameraProcessing = false;

unsigned long lastTofPrint = 0;
const unsigned long TOF_PRINT_INTERVAL = 1000;

unsigned long lastVibrationTime = 0;
const unsigned long VIBE_PULSE_MS = 150;

unsigned long lastObstacleAudioTime = 0;
const unsigned long OBSTACLE_AUDIO_COOLDOWN = 7000;

// ======================================================
// BUTTON VARIABLES
// ======================================================
bool btnLast = false;
unsigned long btnPressStart = 0;
bool longHandled = false;

// ======================================================
// RUNTIME DATA
// ======================================================
String lastDetectedObject = "Person";
String lastKnownAddress = "1750 Finch Ave E, North York, ON";

// ======================================================
// FIREBASE PERIODIC UPLOAD
// ======================================================
unsigned long lastFirebasePush = 0;
const unsigned long FIREBASE_PUSH_INTERVAL = 70000;

// ======================================================
// WIFI RECONNECT
// ======================================================
unsigned long lastWifiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000;

// ======================================================
// FIREBASE STATUS CHECK POLLING (5s)
// ======================================================
unsigned long lastStatusCheckPoll = 0;
const unsigned long STATUS_CHECK_POLL_INTERVAL = 5000;

// ======================================================
// FUNCTION DECLARATIONS
// ======================================================
void startEmergencyCountdown();
void updateEmergencyCountdown();
void enterHelpMode();
void cancelEmergencyByUser();
void ensureWiFiConnected();
void checkFirebaseStatusRequest();

// ======================================================
// WiFi helper
// ======================================================
void ensureWiFiConnected() {
    if (WiFi.status() == WL_CONNECTED) return;

    Serial.println("⚠ WiFi lost, reconnecting...");

    WiFi.disconnect(true);
    delay(300);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
        delay(300);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
        Serial.println("\n✅ WiFi Reconnected");
    else
        Serial.println("\n❌ WiFi Failed");
}

// ======================================================
// FIREBASE "ARE YOU OK?" CHECK
// ======================================================
void checkFirebaseStatusRequest() {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin("https://firestore.googleapis.com/v1/projects/auravision-d0c06/databases/(default)/documents/deviceData/live");
    int code = http.GET();

    if (code != 200) {
        Serial.print("StatusCheck GET failed, code = ");
        Serial.println(code);
        http.end();
        return;
    }

    String payload = http.getString();
    http.end();

    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, payload)) return;

    bool flag = doc["fields"]["checkStatusRequest"]["booleanValue"] | false;

    if (!flag) return;

    Serial.println("🔔 Caregiver status request detected!");

    // Play the audio
    audio.playTrack(TRACK_ARE_YOU_OK);

    // Haptic alert
    ioExp.vibrateTOF(true);
    ioExp.vibrateFall(true);
    delay(180);
    ioExp.vibrateTOF(false);
    ioExp.vibrateFall(false);

    // ⭐ IMPORTANT: Enter 60-second emergency countdown
    startEmergencyCountdown();

    // Reset flag in Firestore
    HTTPClient post;
    post.begin(
        "https://firestore.googleapis.com/v1/projects/auravision-d0c06/"
        "databases/(default)/documents/deviceData/live?updateMask.fieldPaths=checkStatusRequest"
    );
    post.addHeader("Content-Type", "application/json");

    StaticJsonDocument<128> outDoc;
    outDoc["fields"]["checkStatusRequest"]["booleanValue"] = false;

    String outJson;
    serializeJson(outDoc, outJson);
    post.PATCH(outJson);
    post.end();
}

// ======================================================
// SETUP
// ======================================================
void setup() {
    Serial.begin(115200);
    delay(1200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) delay(200);

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Wire.begin(32, 33);
    Wire.setClock(400000);

    audio.begin(13, 12);
    fallDetector.begin(32, 33);
    gps.begin();
    camera.begin();
    tof.begin(32, 33);
    ioExp.begin(0x20);

    oled.begin();
    oled.showNormalMode();
    audio.playTrack(TRACK_BOOTING);
    delay(1200);
    audio.playTrack(TRACK_USE_STICK);
    delay(2000);
}

// ======================================================
// MAIN LOOP
// ======================================================
void loop() {
    unsigned long now = millis();

    if (now - lastWifiCheck >= WIFI_CHECK_INTERVAL) {
        lastWifiCheck = now;
        ensureWiFiConnected();
    }

    if (now - lastStatusCheckPoll >= STATUS_CHECK_POLL_INTERVAL) {
        lastStatusCheckPoll = now;
        checkFirebaseStatusRequest();
    }

    // ========= BUTTON LOGIC =========
    bool pressed = ioExp.isButtonPressed();

    if (pressed && !btnLast) {
        btnPressStart = now;
        longHandled = false;
    }

    if (pressed && btnLast && !longHandled) {
        if (now - btnPressStart >= 3000) {
            longHandled = true;
            enterHelpMode();
        }
    }

    if (!pressed && btnLast) {
        unsigned long dur = now - btnPressStart;

        if (!longHandled && dur >= 50 && dur < 1000 &&
            systemState == STATE_NORMAL)
        {
            ioExp.vibrateTOF(true);
            ioExp.vibrateFall(true);
            delay(120);
            ioExp.vibrateTOF(false);
            ioExp.vibrateFall(false);

            oled.showScanning();
            audio.playTrack(TRACK_SCAN_SURROUNDINGS);

            String objects;
            if (camera.captureAndDetect(objects)) {
                lastDetectedObject = objects;

                delay(1500);
                String low = objects;
                low.toLowerCase();

                int personCount = 0;
                int index = low.indexOf("person");
                while (index != -1) {
                    personCount++;
                    index = low.indexOf("person", index + 1);
                }

                if (personCount >= 2)
                    audio.playTrack(TRACK_CROWD);
                else if (personCount == 1)
                    audio.playTrack(TRACK_PERSON_AHEAD);
                else if (low.indexOf("chair") != -1)
                    audio.playTrack(TRACK_CHAIR_AHEAD);
                else if (low.indexOf("computer") != -1)
                    audio.playTrack(TRACK_COMPUTER_DETECTED);
                else if (low.indexOf("stairs") != -1)
                    audio.playTrack(TRACK_STAIRCASE);
                else if (low.indexOf("door") != -1)
                    audio.playTrack(TRACK_DOOR_DETECTED);
                else
                    audio.playTrack(TRACK_UNIDENTIFIED_OBJECT);

                ioExp.vibrateTOF(true);
                ioExp.vibrateFall(true);
                delay(120);
                ioExp.vibrateTOF(false);
                ioExp.vibrateFall(false);

                oled.showNormalMode();

                firebaseUpload(
                    "NORMAL",
                    lastKnownAddress,
                    lastDetectedObject,
                    gps.hasFix() ? "YES" : "NO",
                    (WiFi.status() == WL_CONNECTED) ? "ONLINE" : "OFFLINE"
                );
            }
        }

        if (!longHandled && dur >= 50 && dur < 1000 &&
            systemState != STATE_NORMAL)
        {
            audio.playTrack(TRACK_THANK_YOU);
            delay(1200);
            cancelEmergencyByUser();
        }
    }

    btnLast = pressed;

    gps.update();
    if (gps.hasFix()) {
        String addr = gps.getCurrentAddress();
        if (addr.length() > 0) lastKnownAddress = addr;
    }

    if (now - lastFirebasePush >= FIREBASE_PUSH_INTERVAL) {
        lastFirebasePush = now;

        firebaseUpload(
            (systemState == STATE_NORMAL) ? "NORMAL" :
            (systemState == STATE_EMERGENCY_COUNTDOWN) ? "EMERGENCY_COUNTDOWN" :
                                                         "HELP_MODE",
            lastKnownAddress,
            lastDetectedObject,
            gps.hasFix() ? "YES" : "NO",
            (WiFi.status() == WL_CONNECTED) ? "ONLINE" : "OFFLINE"
        );
    }

    // ======================================================
    // STATE MACHINE
    // ======================================================
    switch (systemState) {

        case STATE_NORMAL: {
            if (fallDetector.checkFall()) {
                startEmergencyCountdown();
                return;
            }

            int distance = tof.getDistance();

            if (distance > 0 && distance < 500) {

                if (now - lastObstacleAudioTime >= OBSTACLE_AUDIO_COOLDOWN) {
                    lastObstacleAudioTime = now;
                    audio.playTrack(TRACK_OBSTACLE);
                }

                if (now - lastVibrationTime >= 2000) {
                    lastVibrationTime = now;
                    ioExp.vibrateTOF(true);
                    ioExp.vibrateFall(true);
                    delay(VIBE_PULSE_MS);
                    ioExp.vibrateTOF(false);
                    ioExp.vibrateFall(false);
                }
            }

            else if (distance >= 500 && distance <= 1000) {
                if (now - lastVibrationTime >= 3000) {
                    lastVibrationTime = now;
                    ioExp.vibrateTOF(true);
                    ioExp.vibrateFall(true);
                    delay(VIBE_PULSE_MS);
                    ioExp.vibrateTOF(false);
                    ioExp.vibrateFall(false);
                }
            }

            else {
                ioExp.vibrateTOF(false);
                ioExp.vibrateFall(false);
            }

            if (now - lastTofPrint >= TOF_PRINT_INTERVAL) {
                lastTofPrint = now;
                Serial.printf("TOF: %d mm\n", distance);
            }

            // ================= CAMERA OBJECT DETECTION =================
            if (!cameraProcessing &&
                distance > TOF_MIN_TRIGGER &&
                distance < TOF_MAX_TRIGGER &&
                now - lastCameraTrigger >= CAMERA_COOLDOWN)
            {
                cameraProcessing = true;
                lastCameraTrigger = now;

                audio.playTrack(TRACK_OBJECT_DETECTED);

                String objects;
                String apiJson; // <-- JSON response from API
                if (camera.captureAndDetect(objects, apiJson)) { // modified function
                    lastDetectedObject = objects;

                    Serial.println("📸 Google Vision API Response:");
                    Serial.println(apiJson); // <-- print API response JSON

                    delay(1500);

                    String low = objects;
                    low.toLowerCase();

                    int personCount = 0;
                    int index = low.indexOf("person");
                    while (index != -1) {
                        personCount++;
                        index = low.indexOf("person", index + 1);
                    }

                    if (personCount >= 2)
                        audio.playTrack(TRACK_CROWD);
                    else if (personCount == 1)
                        audio.playTrack(TRACK_PERSON_AHEAD);
                    else if (low.indexOf("chair") != -1)
                        audio.playTrack(TRACK_CHAIR_AHEAD);
                    else if (low.indexOf("computer") != -1)
                        audio.playTrack(TRACK_COMPUTER_DETECTED);
                    else if (low.indexOf("stairs") != -1)
                        audio.playTrack(TRACK_STAIRCASE);
                    else if (low.indexOf("door") != -1)
                        audio.playTrack(TRACK_DOOR_DETECTED);
                    else
                        audio.playTrack(TRACK_UNIDENTIFIED_OBJECT);

                    firebaseUpload(
                        "NORMAL",
                        lastKnownAddress,
                        lastDetectedObject,
                        gps.hasFix() ? "YES" : "NO",
                        (WiFi.status() == WL_CONNECTED) ? "ONLINE" : "OFFLINE"
                    );
                }

                cameraProcessing = false;
            }

        }
        break;

        case STATE_EMERGENCY_COUNTDOWN:
            updateEmergencyCountdown();
            break;

        case STATE_HELP_MODE:
            break;
    }

    delay(30);
}

// ======================================================
// EMERGENCY COUNTDOWN
// ======================================================
void startEmergencyCountdown() {
    systemState = STATE_EMERGENCY_COUNTDOWN;
    emergencyStartTime = millis();

    audio.playTrack(TRACK_FALL_DETECTED);

    // Remove constant vibration
    lastEmergencyVibration = 0; // reset timer for pulsing vibration
    oled.showAlertMode(60);
}

void updateEmergencyCountdown() {
    unsigned long elapsed = millis() - emergencyStartTime;
    int remaining = (60000 - elapsed) / 1000;

    if (remaining < 0) remaining = 0;

    oled.showAlertMode(remaining);

    // Vibrate once every 3 seconds
    if (millis() - lastEmergencyVibration >= 3000) {
        lastEmergencyVibration = millis();
        ioExp.vibrateTOF(true);
        ioExp.vibrateFall(true);
        delay(150); // pulse duration
        ioExp.vibrateTOF(false);
        ioExp.vibrateFall(false);
    }

    if (elapsed >= 60000) enterHelpMode();
}

// ======================================================
// HELP MODE
// ======================================================
void enterHelpMode() {
    systemState = STATE_HELP_MODE;

    ioExp.vibrateTOF(false);
    ioExp.vibrateFall(false);

    oled.showEmergencyInfo();

    String addr = gps.getCurrentAddress();
    if (addr.length() > 0) lastKnownAddress = addr;

    audio.playTrack(TRACK_SENDING_ALERTS);
    gps.sendLocationToCloud();

    firebaseUpload(
        "HELP_MODE",
        lastKnownAddress,
        lastDetectedObject,
        gps.hasFix() ? "YES" : "NO",
        (WiFi.status() == WL_CONNECTED) ? "ONLINE" : "OFFLINE"
    );
}

// ======================================================
// CANCEL EMERGENCY
// ======================================================
void cancelEmergencyByUser() {
    if (systemState == STATE_NORMAL) return;

    ioExp.vibrateTOF(false);
    ioExp.vibrateFall(false);

    systemState = STATE_NORMAL;
    oled.showNormalMode();

    firebaseUpload(
        "NORMAL",
        lastKnownAddress,
        lastDetectedObject,
        gps.hasFix() ? "YES" : "NO",
        (WiFi.status() == WL_CONNECTED) ? "ONLINE" : "OFFLINE"
    );
}
