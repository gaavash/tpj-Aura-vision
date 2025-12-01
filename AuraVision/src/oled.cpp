#include "oled.h"

OLED::OLED() : display(OLED_ADDR, OLED_SDA, OLED_SCL) {}

void OLED::begin() {
    display.init();
    display.flipScreenVertically();
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.display();
}

void OLED::showSplashScreen() {
    display.clear();
    display.drawString(0, 0, "AuraVision");
    display.drawString(0, 15, "Booting...");
    display.display();
}

void OLED::showNormalMode() {
    display.clear();
    display.drawString(0, 0, "NORMAL MODE");
    display.drawString(0, 15, "System Active");
    display.display();
}

void OLED::showAlertMode(int secondsLeft) {
    display.clear();
    display.drawString(0, 0, "⚠ FALL DETECTED");
    display.drawString(0, 15, "Help in:");
    display.drawString(0, 30, String(secondsLeft) + " sec");
    display.display();
}

void OLED::showEmergencyInfo() {
    display.clear();
    display.drawString(0, 0, "HELP MODE ACTIVE");
    display.drawString(0, 15, "Sending Location...");
    display.drawString(0, 25, "Aayush S");
    display.drawString(0, 35, "6477016003");
    display.drawString(0, 45, "338 Steeles Ave E");
    display.display();
}

void OLED::showCustomMessage(String msg) {
    display.clear();
    display.drawString(0, 0, "MESSAGE:");
    display.drawStringMaxWidth(0, 15, 128, msg);
    display.display();
}

void OLED::showScanning() {
    display.clear();
    display.drawString(0, 0, "SCANNING...");
    display.drawString(0, 15, "Please wait");
    display.display();
}

