#ifndef VOICEBOX_API_H
#define VOICEBOX_API_H

#include <HTTPClient.h>

class VoiceBoxAPI {
public:
    VoiceBoxAPI(const char* apiKey);
    String getAudio(const String& text, int speaker);

private:
    const char* apiKey;
};

#endif // VOICEBOX_API_H
