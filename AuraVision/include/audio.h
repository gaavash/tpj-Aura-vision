#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

// GLOBAL AUDIO PRIORITY SYSTEM
extern int currentAudioPriority;
extern unsigned long lastAudioTime;
extern const unsigned long AUDIO_RESET_COOLDOWN;

class AudioPlayer {
private:
    DFRobotDFPlayerMini myDFPlayer;
    HardwareSerial* mp3Serial;
    unsigned long lastPlayTime = 0;
    const unsigned long PLAY_COOLDOWN = 300;

public:
    bool begin(int rx_pin = 13, int tx_pin = 12);

    // Priority-based audio
    void playTrackPriority(int trackNumber, int priority);

    // Normal audio (legacy)
    void playTrack(int trackNumber);
};

enum AudioTracks {
    TRACK_BOOTING = 1,
    TRACK_OBJECT_DETECTED = 2,
    TRACK_FALL_DETECTED = 3,
    TRACK_SENDING_ALERTS = 4,
    TRACK_PERSON_AHEAD = 5,
    TRACK_CHAIR_AHEAD = 6,
    TRACK_COMPUTER_DETECTED = 7,
    TRACK_UNIDENTIFIED_OBJECT = 8, 
    TRACK_ARE_YOU_OK = 9,
    TRACK_USE_STICK = 10,
    TRACK_THANK_YOU = 11,
    TRACK_CROWD = 12,
    TRACK_DOOR_DETECTED = 13,
    TRACK_STAIRCASE = 14,
    TRACK_OBSTACLE = 15,
    TRACK_SCAN_SURROUNDINGS = 16
};

#endif
