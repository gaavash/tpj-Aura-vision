#include "mpu_fall.h"

bool MPUFallDetector::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);

    if (!mpu.begin()) {
        Serial.println("❌ MPU6050 not detected!");
        return false;
    }

    Serial.println("✅ MPU6050 initialized");
    calibrate();
    return true;
}

void MPUFallDetector::calibrate() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    ax_filtered = a.acceleration.x;
    ay_filtered = a.acceleration.y;
    az_filtered = a.acceleration.z;

    Serial.println("MPU calibrated");
}

bool MPUFallDetector::checkFall() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Apply simple low-pass filter
    ax_filtered = FILTER_ALPHA * ax_filtered + (1 - FILTER_ALPHA) * a.acceleration.x;
    ay_filtered = FILTER_ALPHA * ay_filtered + (1 - FILTER_ALPHA) * a.acceleration.y;
    az_filtered = FILTER_ALPHA * az_filtered + (1 - FILTER_ALPHA) * a.acceleration.z;

    float totalAcc = sqrt(ax_filtered * ax_filtered +
                          ay_filtered * ay_filtered +
                          az_filtered * az_filtered);

    unsigned long now = millis();
    if (totalAcc > FALL_THRESHOLD) {
        // debounce to ignore short motor spikes
        if (now - lastFallTime > DEBOUNCE_MS) {
            lastFallTime = now;
            Serial.printf("⚠ Fall detected! Acc=%.2f\n", totalAcc);
            return true;
        }
    }

    return false;
}
