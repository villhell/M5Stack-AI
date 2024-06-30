#include "AudioInput.h"

AudioInput::AudioInput() {}

void AudioInput::begin() {
    // 必要な初期化処理（現在は特に無し）
}

int16_t AudioInput::read() {
    return analogRead(36); // Pin 36は内蔵マイクの入力ピン
}
