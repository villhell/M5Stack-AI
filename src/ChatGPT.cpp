#include "ChatGPT.h"
#include <ArduinoJson.h>

ChatGPT::ChatGPT(const char* apiKey) : apiKey(apiKey) {}

String ChatGPT::getResponse(const String& prompt) {
    HTTPClient http;
    String payload;
    String url = "https://api.openai.com/v1/chat/completions";  // URLを最新のエンドポイントに更新

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));

    // リクエストボディをJSONで構築
    DynamicJsonDocument doc(1024);
    doc["model"] = "gpt-3.5-turbo";  // 使用するモデルを指定
    JsonArray messages = doc.createNestedArray("messages");
    JsonObject message = messages.createNestedObject();
    message["role"] = "user";
    message["content"] = prompt;
    doc["max_tokens"] = 150;  // 応答の最大トークン数

    String requestBody;
    serializeJson(doc, requestBody);

    Serial.println("Sending request to ChatGPT API...");
    Serial.println("Request Body: " + requestBody);

    int httpResponseCode = http.POST(requestBody);
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
        payload = http.getString();
        Serial.println("Response payload: " + payload);

        // レスポンスのJSONをパース
        DynamicJsonDocument responseDoc(1024);
        DeserializationError error = deserializeJson(responseDoc, payload);

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return "Error parsing JSON response";
        }

        // 応答テキストを抽出
        String responseText = responseDoc["choices"][0]["message"]["content"].as<String>();
        Serial.println("Extracted response: " + responseText);
        return responseText;
    } else if (httpResponseCode == 429){
        
        Serial.println("Error: API rate limit exceeded or insufficient quota");
        return "Error: API rate limit exceeded or insufficient quota. Please check your OpenAI account.";
    
    } else if (httpResponseCode != 200) {
        
        Serial.println("Error: Unexpected response from ChatGPT API");
        return "Error: Unexpected response from ChatGPT API. Response code: " + String(httpResponseCode);

    } else {
        
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
        return "Error: " + String(httpResponseCode);
        
    }

    http.end();
}