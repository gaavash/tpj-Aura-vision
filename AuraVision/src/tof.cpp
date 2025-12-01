#include "tof.h"

ToFModule::ToFModule() { }

bool ToFModule::begin(int sda_pin, int scl_pin) {

    // ❗ I2C already started in main.cpp → DO NOT CALL Wire.begin()
    sensor.setTimeout(500);

    if (!sensor.init()) {
        Serial.println("❌ VL53L1X init failed!");
        return false;
    }

    // Short mode → best for <2m detection
    sensor.setDistanceMode(VL53L1X::Short);
    sensor.setMeasurementTimingBudget(20000); // 20ms
    sensor.startContinuous(50); // ~20 fps

    Serial.println("ToF ready.");
    return true;
}

int ToFModule::getDistance() {
    uint16_t d = sensor.read();
    if (sensor.timeoutOccurred()) return -1;
    return d;
}
