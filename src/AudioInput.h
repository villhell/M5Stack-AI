#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include <M5Core2.h>

class AudioInput {
public:
    AudioInput();
    void begin();
    int16_t read();
};

#endif // AUDIO_INPUT_H