#include "camera.h"
#include "esp_camera.h"
#include <mbedtls/base64.h>

// =================== GOOGLE VISION API KEY ===================
const char* GOOGLE_API_KEY = "AIzaSyDfoVcQ9AvDPPaDj1RJhjDsh49odwDYEWI"; // replace with your key

// =================== Camera Pins ===================
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM      5
#define Y2_GPIO_NUM      4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

CameraModule::CameraModule() {}

bool CameraModule::begin() {
    Serial.println("📷 Initializing OV2640 Camera...");

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size   = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count     = 1;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("❌ Camera init failed!");
        return false;
    }

    Serial.println("✅ Camera initialized!");
    return true;
}

// ============================================================
// CAPTURE IMAGE AND CONVERT TO BASE64
// ============================================================
bool CameraModule::captureImage(String &base64ImageOut) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("❌ Camera capture failed!");
        return false;
    }

    size_t out_len;
    mbedtls_base64_encode(NULL, 0, &out_len, fb->buf, fb->len);

    unsigned char* out_buf = (unsigned char*)malloc(out_len + 1);
    if (!out_buf) {
        Serial.println("❌ Base64 malloc failed!");
        esp_camera_fb_return(fb);
        return false;
    }

    mbedtls_base64_encode(out_buf, out_len, &out_len, fb->buf, fb->len);
    out_buf[out_len] = '\0';

    base64ImageOut = String((char*)out_buf);

    free(out_buf);
    esp_camera_fb_return(fb);

    Serial.println("📸 Image captured & encoded!");
    return true;
}

// ============================================================
// DETECT OBJECTS VIA GOOGLE VISION
// ============================================================
bool CameraModule::detectObjects(String base64Image, String &resultText, String &apiJsonOut) {
    Serial.println("🌐 Sending to Google Vision API...");

    String payload = "{\"requests\": [{\"image\": {\"content\": \"" +
                     base64Image +
                     "\"},\"features\": [{\"type\": \"OBJECT_LOCALIZATION\",\"maxResults\": 10}]}]}";

    String endpoint =
        "https://vision.googleapis.com/v1/images:annotate?key=" + String(GOOGLE_API_KEY);

    HTTPClient http;
    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(payload);
    if (code <= 0) {
        resultText = "HTTP Error: " + String(code);
        apiJsonOut = resultText;
        http.end();
        return false;
    }

    String response = http.getString();
    http.end();

    apiJsonOut = response;
    resultText = extractObjectsFromJSON(response);

    return true;
}

// ============================================================
// PARSE JSON FOR OBJECT NAMES + CONFIDENCE
// ============================================================
String CameraModule::extractObjectsFromJSON(const String &json) {
    String output = "";
    int pos = 0;
    int found = 0;

    while (true) {
        int start = json.indexOf("\"name\": \"", pos);
        if (start == -1) break;
        start += 9;

        int end = json.indexOf("\"", start);
        if (end == -1) break;

        String objectName = json.substring(start, end);

        int scoreStart = json.indexOf("\"score\": ", end);
        if (scoreStart == -1) break;
        scoreStart += 9;

        int scoreEnd = json.indexOf(",", scoreStart);
        float confidence = json.substring(scoreStart, scoreEnd).toFloat() * 100;

        output += objectName + " (" + String(confidence, 1) + "%)\n";

        found++;
        pos = scoreEnd;
    }

    if (found == 0)
        return "No objects detected.";

    return output;
}

// ============================================================
// ONE-SHOT FUNCTION (ORIGINAL)
bool CameraModule::captureAndDetect(String &objectsOut) {
    String img;
    String dummyJson;
    if (!captureImage(img)) return false;
    if (!detectObjects(img, objectsOut, dummyJson)) return false;

    return true;
}

// ============================================================
// NEW ONE-SHOT FUNCTION WITH RAW JSON
bool CameraModule::captureAndDetect(String &objectsOut, String &apiJsonOut) {
    String img;
    if (!captureImage(img)) return false;
    if (!detectObjects(img, objectsOut, apiJsonOut)) return false;

    return true;
}
