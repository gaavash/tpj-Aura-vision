#ifndef MPU_FALL_H
#define MPU_FALL_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MPUFallDetector {
public:
    bool begin(int sda_pin = 32, int scl_pin = 33);
    void calibrate();
    bool checkFall();

private:
    Adafruit_MPU6050 mpu;

    float ax_filtered = 0.0;
    float ay_filtered = 0.0;
    float az_filtered = 0.0;

    const float FALL_THRESHOLD = 35.0; // adjust 30-40 if needed
    const float FILTER_ALPHA = 0.7;    // low-pass filter factor
    const unsigned long DEBOUNCE_MS = 200; // ignore spikes shorter than 200ms
    unsigned long lastFallTime = 0;
};

#endif
