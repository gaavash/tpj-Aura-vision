#ifndef TOF_H
#define TOF_H

#include <Arduino.h>
#include <VL53L1X.h>

class ToFModule {
public:
    ToFModule();
    bool begin(int sda_pin = 32, int scl_pin = 33);
    int getDistance();

private:
    VL53L1X sensor;
};

#endif
