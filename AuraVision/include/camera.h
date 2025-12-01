#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include <HTTPClient.h>

class CameraModule {
public:
    CameraModule();
    bool begin();

    // Capture and detect objects (original)
    bool captureAndDetect(String &objectsOut);

    // NEW: Capture, detect and return raw API JSON
    bool captureAndDetect(String &objectsOut, String &apiJsonOut);

private:
    bool captureImage(String &base64ImageOut);
    bool detectObjects(String base64Image, String &resultText, String &apiJsonOut);
    String extractObjectsFromJSON(const String &json);
};

#endif
