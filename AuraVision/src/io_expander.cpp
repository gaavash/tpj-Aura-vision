#include "io_expander.h"

IOExpander::IOExpander() {}

bool IOExpander::begin(uint8_t addr) {
    if (!mcp.begin_I2C(addr)) {
        Serial.println("❌ MCP23017 init failed!");
        return false;
    }

    Serial.println("MCP23017 detected. Initializing pins...");

    // Button Input (wired to VCC → PA3)
    mcp.pinMode(BTN_PIN, INPUT);

    // Vibration Motors Output
    mcp.pinMode(VIB_TOF_PIN, OUTPUT);
    mcp.pinMode(VIB_FALL_PIN, OUTPUT);

    mcp.digitalWrite(VIB_TOF_PIN, LOW);
    mcp.digitalWrite(VIB_FALL_PIN, LOW);

    Serial.println("MCP23017 ready (Button + Vibration)");
    return true;
}

bool IOExpander::isButtonPressed() {
    // Button connected to VCC → reads HIGH when pressed
    return mcp.digitalRead(BTN_PIN) == HIGH;
}

void IOExpander::vibrateTOF(bool state) {
    mcp.digitalWrite(VIB_TOF_PIN, state ? HIGH : LOW);
}

void IOExpander::vibrateFall(bool state) {
    mcp.digitalWrite(VIB_FALL_PIN, state ? HIGH : LOW);
}
