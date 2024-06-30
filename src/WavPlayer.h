#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>

class WavPlayer {
public:
    WavPlayer();
    void begin();
    void playWav(const char* filename);

private:
    AudioOutputI2S* out;
    AudioGeneratorWAV* wav;
    AudioFileSourceSD* file;
};

#endif // WAV_PLAYER_H
