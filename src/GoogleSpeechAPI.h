#ifndef GOOGLE_SPEECH_API_H
#define GOOGLE_SPEECH_API_H

#include <M5Unified.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

class GoogleSpeechAPI {
private:
    String api_key;
    WiFiClientSecure client;

    static const int SAMPLE_RATE = 16000;
    static const int RECORD_TIME = 5; // 録音時間（秒）

    String recordAudio();
    String encodeAudioToBase64(const uint8_t* audioData, size_t dataLength);

public:
    GoogleSpeechAPI();
    bool init(const char* credentials_file);
    String recognizeSpeech();
    ~GoogleSpeechAPI();
};

#endif // GOOGLE_SPEECH_API_H