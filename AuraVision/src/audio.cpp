#include "audio.h"

// ===== GLOBAL PRIORITY VARIABLES =====
int currentAudioPriority = 0;
unsigned long lastAudioTime = 0;
const unsigned long AUDIO_RESET_COOLDOWN = 2000; // 2 seconds

// ===========================================
//              AUDIO INITIALIZATION
// ===========================================
bool AudioPlayer::begin(int rx_pin, int tx_pin) {
    delay(1500); // Required for DFPlayer boot

    mp3Serial = new HardwareSerial(2);
    mp3Serial->begin(9600, SERIAL_8N1, rx_pin, tx_pin);

    Serial.println("Initializing DFPlayer...");

    if (!myDFPlayer.begin(*mp3Serial)) {
        Serial.println("DFPlayer init FAILED!");
        return false;
    }

    myDFPlayer.volume(30);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    delay(200);

    Serial.println("DFPlayer READY (Volume 30)");
    return true;
}

// ===========================================
//           PRIORITY PLAY FUNCTION
// ===========================================
void AudioPlayer::playTrackPriority(int trackNumber, int priority) {
    unsigned long now = millis();

    // Reset priority after cooldown
    if (now - lastAudioTime >= AUDIO_RESET_COOLDOWN) {
        currentAudioPriority = 0;
    }

    // Ignore if incoming sound is lower priority
    if (priority < currentAudioPriority) {
        Serial.println("AUDIO BLOCKED (lower priority)");
        return;
    }

    // Accept the sound
    currentAudioPriority = priority;
    lastAudioTime = now;

    myDFPlayer.play(trackNumber);

    Serial.print("AUDIO PRIORITY PLAY: ");
    Serial.print(trackNumber);
    Serial.print(" (priority ");
    Serial.print(priority);
    Serial.println(")");
}

// ===========================================
//      NORMAL PLAY — LEGACY (NO PRIORITY)
// ===========================================
void AudioPlayer::playTrack(int trackNumber) {
    if (millis() - lastPlayTime < PLAY_COOLDOWN) return;

    myDFPlayer.play(trackNumber);
    lastPlayTime = millis();

    switch (trackNumber) {
        case TRACK_BOOTING: Serial.println("AUDIO: Booting system"); break;
        case TRACK_OBJECT_DETECTED: Serial.println("AUDIO: Object Detected"); break;
        case TRACK_FALL_DETECTED: Serial.println("AUDIO: Fall Detected"); break;
        case TRACK_SENDING_ALERTS: Serial.println("AUDIO: Sending Alerts"); break;
        case TRACK_PERSON_AHEAD: Serial.println("AUDIO: Person Ahead"); break;
        case TRACK_CHAIR_AHEAD: Serial.println("AUDIO: Chair Ahead"); break;
        case TRACK_COMPUTER_DETECTED: Serial.println("AUDIO: Computer Detected"); break;
        case TRACK_UNIDENTIFIED_OBJECT: Serial.println("AUDIO: Unknown Object"); break;
        case TRACK_OBSTACLE: Serial.println("AUDIO: Obstacle"); break;
        case TRACK_CROWD: Serial.println("AUDIO: Crowd Ahead"); break;
        case TRACK_ARE_YOU_OK: Serial.println("AUDIO: Are You Ok"); break;
        case TRACK_SCAN_SURROUNDINGS: Serial.println("AUDIO: Scanning Area"); break;
        case TRACK_THANK_YOU: Serial.println("AUDIO: Thank you"); break;
        case TRACK_DOOR_DETECTED: Serial.println("AUDIO: Door ahead"); break;
        case TRACK_STAIRCASE: Serial.println("AUDIO: Staircase"); break;
    }
}
