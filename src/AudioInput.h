#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include <Arduino.h>
#include <M5Unified.h>

class AudioInput {
public:
    void begin();
    String getAudioInput();

private:
    String encodeAudioToBase64(const int16_t* audioData, size_t dataLength);
};

#endif // AUDIO_INPUT_H