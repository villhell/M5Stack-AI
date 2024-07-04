#include "AudioInput.h"
#include <M5Unified.h>

void AudioInput::begin() {
    M5.Mic.begin();
}

String AudioInput::getAudioInput() {
    const int recordDuration = 5000;  // 5秒間録音
    const int sampleRate = 16000;
    const int bufferSize = (sampleRate * recordDuration / 1000) * 2;  // 16-bitサンプル用に2倍

    int16_t* audioBuffer = (int16_t*)malloc(bufferSize);
    if (!audioBuffer) {
        Serial.println("Failed to allocate audio buffer");
        return "";
    }

    M5.Mic.record(audioBuffer, bufferSize, sampleRate);

    // ここで audioBuffer を Base64 エンコードするか、
    // Google Speech-to-Text API に送信するための適切な形式に変換する必要があります
    String encodedAudio = encodeAudioToBase64(audioBuffer, bufferSize);

    free(audioBuffer);

    return encodedAudio;
}

String AudioInput::encodeAudioToBase64(const int16_t* audioData, size_t dataLength) {
    // Base64エンコーディングの実装
    // この実装は簡略化されています。実際にはより効率的なエンコーディングが必要です。
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    String result;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    for (size_t i = 0; i < dataLength; i++) {
        char_array_3[j++] = (audioData[i] >> 8) & 0xFF;
        char_array_3[j++] = audioData[i] & 0xFF;
        if (j == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (int i = 0; i < 4; i++)
                result += base64_chars[char_array_4[i]];
            j = 0;
        }
    }

    if (j) {
        for (int i = j; i < 3; i++)
            char_array_3[i] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (int i = 0; i < j + 1; i++)
            result += base64_chars[char_array_4[i]];

        while (j++ < 3)
            result += '=';
    }

    return result;
}