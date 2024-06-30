#include "VoiceBoxAPI.h"

VoiceBoxAPI::VoiceBoxAPI(const char* apiKey) : apiKey(apiKey) {}

String VoiceBoxAPI::getAudio(const String& text, int speaker) {
    HTTPClient http;
    String url = "https://deprecatedapis.tts.quest/v2/voicevox/audio/?text=" + text + "&key=" + String(apiKey) + "&speaker=" + String(speaker);
    String payload = "";

    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode == HTTP_CODE_OK) {
        payload = http.getString();
    } else {
        Serial.println("Failed to get audio data");
    }
    http.end();

    return payload;
}
