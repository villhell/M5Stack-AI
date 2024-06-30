#include "WavPlayer.h"

WavPlayer::WavPlayer() {}

void WavPlayer::begin() {
    out = new AudioOutputI2S();
    out->begin();
    
    if (!SD.begin()) {
        Serial.println("SD Card initialization failed!");
        return;
    }
    Serial.println("SD Card initialized.");
}

void WavPlayer::playWav(const char* filename) {
    file = new AudioFileSourceSD(filename);
    wav = new AudioGeneratorWAV();
    wav->begin(file, out);
    
    while (wav->isRunning()) {
        if (!wav->loop()) {
            wav->stop();
        }
    }

    delete file;
    delete wav;
}
