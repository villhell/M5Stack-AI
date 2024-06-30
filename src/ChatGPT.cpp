#include "ChatGPT.h"

ChatGPT::ChatGPT(const char* apiKey) : apiKey(apiKey) {}

String ChatGPT::getResponse(const String& prompt) {
    HTTPClient http;
    String payload;
    String url = "https://api.openai.com/v1/engines/davinci-codex/completions";

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));
    String requestBody = "{\"prompt\": \"" + prompt + "\", \"max_tokens\": 50}";
    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
        payload = http.getString();
    }
    http.end();

    return payload;
}
