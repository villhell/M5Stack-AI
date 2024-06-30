#include "GoogleSpeechAPI.h"

GoogleSpeechAPI::GoogleSpeechAPI() {}

bool GoogleSpeechAPI::init(const char* credentials_file) {
    if (!SD.begin()) {
        Serial.println("Failed to initialize SD card");
        return false;
    }

    File file = SD.open(credentials_file);
    if (!file) {
        Serial.println("Failed to open credentials file from SD card");
        return false;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse credentials file");
        return false;
    }

    api_key = doc["api_key"].as<String>();

    M5.begin();
    return true;
}

String GoogleSpeechAPI::recordAudio() {
    M5.Mic.begin();
    
    const int bufferSize = SAMPLE_RATE * RECORD_TIME * 2;
    int16_t* audioBuffer = new int16_t[bufferSize / 2];
    
    Serial.println("Started recording...");
    size_t bytesRead = M5.Mic.record(audioBuffer, bufferSize, SAMPLE_RATE);
    Serial.printf("Recorded %d bytes of audio data\n", bytesRead);

    // Check if any non-zero samples were recorded
    bool hasNonZeroSamples = false;
    int16_t maxSample = 0;
    for (int i = 0; i < bytesRead / 2; i++) {
        if (audioBuffer[i] != 0) {
            hasNonZeroSamples = true;
            if (abs(audioBuffer[i]) > maxSample) {
                maxSample = abs(audioBuffer[i]);
            }
        }
    }

    if (!hasNonZeroSamples) {
        Serial.println("Warning: All recorded samples are zero!");
    } else {
        Serial.printf("Max sample value: %d\n", maxSample);
    }

    String base64Audio = encodeAudioToBase64((uint8_t*)audioBuffer, bytesRead);
    delete[] audioBuffer;

    Serial.println("Base64 encoded audio length: " + String(base64Audio.length()));

    return base64Audio;
}

String GoogleSpeechAPI::encodeAudioToBase64(const uint8_t* audioData, size_t dataLength) {
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    size_t outputLength = 4 * ((dataLength + 2) / 3);
    String result;
    result.reserve(outputLength);

    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    for (size_t i = 0; i < dataLength; i++) {
        char_array_3[j++] = audioData[i];
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

String GoogleSpeechAPI::recognizeSpeech() {
    String audioData = recordAudio();
    
    if (audioData.length() == 0) {
        Serial.println("Error: No audio data recorded");
        return "Error: No audio data recorded";
    }

    HTTPClient http;
    client.setInsecure(); // SSL証明書の検証をスキップ
    http.begin(client, "https://speech.googleapis.com/v1/speech:recognize?key=" + api_key);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"ja-JP\"},\"audio\":{\"content\":\"" + audioData + "\"}}";

    Serial.println("Sending request to Google Speech-to-Text API...");
    Serial.println("Payload length: " + String(payload.length()));

    int httpResponseCode = http.POST(payload);
    String response = "";

    if (httpResponseCode > 0) {
        response = http.getString();
        Serial.println("Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
        
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, response);
        
        if (error) {
            return "JSON parsing error: " + String(error.c_str());
        }
        
        if (doc.containsKey("results") && doc["results"].size() > 0) {
            if (doc["results"][0].containsKey("alternatives") && doc["results"][0]["alternatives"].size() > 0) {
                return doc["results"][0]["alternatives"][0]["transcript"].as<String>();
            }
        }
        return "No transcription available. Response: " + response;
    } else {
        return "HTTP Error: " + String(httpResponseCode) + " - " + http.errorToString(httpResponseCode);
    }
}

GoogleSpeechAPI::~GoogleSpeechAPI() {
    M5.Mic.end();
}