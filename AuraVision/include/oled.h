#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include "SH1106Wire.h"

#define OLED_SDA 32
#define OLED_SCL 33
#define OLED_ADDR 0x3C

class OLED {
public:
    OLED();
    void begin();

    void showSplashScreen();
    void showNormalMode();
    void showAlertMode(int secondsLeft);
    void showEmergencyInfo();
    void showCustomMessage(String msg);
    void showScanning();

private:
    SH1106Wire display;
};

#endif
