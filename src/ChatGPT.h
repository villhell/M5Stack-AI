#ifndef CHATGPT_H
#define CHATGPT_H

#include <Arduino.h>
#include <HTTPClient.h>

class ChatGPT {
public:
    ChatGPT(const char* apiKey);
    String getResponse(const String& prompt);

private:
    const char* apiKey;
};

#endif // CHATGPT_H