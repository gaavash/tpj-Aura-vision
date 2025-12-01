#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

class IOExpander {
public:
    IOExpander();

    bool begin(uint8_t addr = 0x20);

    bool isButtonPressed();
    void vibrateTOF(bool state);
    void vibrateFall(bool state);

private:
    Adafruit_MCP23X17 mcp;

    // Pins on MCP23017
    static const uint8_t BTN_PIN = 3;        // PA3
    static const uint8_t VIB_TOF_PIN = 8;    // PB0
    static const uint8_t VIB_FALL_PIN = 9;   // PB1
};

#endif
