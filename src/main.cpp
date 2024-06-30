#include <M5Unified.h>
#include "WiFiConnection.h"
#include "AudioInput.h"
#include "ChatGPT.h"
#include "VoiceBoxAPI.h"
#include "WavPlayer.h"
#include "ConfigReader.h"
#include "GoogleSpeechAPI.h"

// 設定ファイル名
const char* configFilename = "/config.txt";

// Speech To TextのAPIキー
const char* sttApiKey = "/speech_to_text_api_key.json";

// 各種設定変数
String ssid;
String password;
String openaiApiKey;
String voiceboxApiKey;
int speakerId;

// 各クラスのインスタンス
WiFiConnection* wifi;
AudioInput audioInput;
ChatGPT* chatGpt;
VoiceBoxAPI* voicebox;
WavPlayer wavPlayer;
ConfigReader configReader(configFilename);
GoogleSpeechAPI speechAPI;

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.println("Hello World!");

    // SDカードから設定を読み取る
    if (!configReader.begin()) {
        Serial.println("Failed to initialize SD card!");
        M5.Lcd.println("Failed to initialize SD card!");
        return;
    }
    
    Serial.println("SDカード読み取り成功");

    std::map<String, String> config = configReader.readConfig();
    if (config.empty()) {
        Serial.println("Failed to read config file!");
        M5.Lcd.println("Failed to read config file!");
        return;
    }
    Serial.println("config読み取り成功");

    // 設定を変数に代入
    ssid = config["ssid"];
    password = config["password"];
    openaiApiKey = config["openai_api_key"];
    voiceboxApiKey = config["voicebox_api_key"];
    speakerId = config["speaker_id"].toInt();

    Serial.println("設定を変数に代入");

    // 設定値をシリアルモニタとLCDに表示
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + password);
    Serial.println("OpenAI API Key: " + openaiApiKey);
    Serial.println("VoiceBox API Key: " + voiceboxApiKey);
    Serial.println("Speaker ID: " + String(speakerId));

    // 設定値の表示
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.printf("SSID: %s\n", ssid.c_str());
    M5.Lcd.printf("Password: %s\n", password.c_str());
    M5.Lcd.printf("OpenAI API Key: %s\n", openaiApiKey.c_str());
    M5.Lcd.printf("VoiceBox API Key: %s\n", voiceboxApiKey.c_str());
    M5.Lcd.printf("Speaker ID: %d\n", speakerId);

    // 各クラスのインスタンスを初期化
    wifi = new WiFiConnection(ssid.c_str(), password.c_str());
    chatGpt = new ChatGPT(openaiApiKey.c_str());
    voicebox = new VoiceBoxAPI(voiceboxApiKey.c_str());

    // WiFi接続
    wifi->connect();

    M5.Lcd.println("IP: " + WiFi.localIP().toString());
    Serial.println("WiFi接続完了");

    wavPlayer.begin();
    Serial.println("wavPlayer起動完了");

    // AudioInputの初期化
    audioInput.begin();
    Serial.println("AudioInputの初期化");

    if (speechAPI.init(sttApiKey)) {
        M5.Lcd.println("Speech API initialized");
        Serial.println("Speech API initialized");
    } else {
        M5.Lcd.println("Failed to initialize Speech API");
        Serial.println("Failed to initialize Speech API");
    }

    Serial.println("Setup completed");
    M5.Lcd.println("Setup completed");
    M5.Lcd.println("Touch screen to start recording");
}

void loop() {
    M5.update();
    
    if (M5.Touch.getCount()) {
        M5.Lcd.clear();
        M5.Lcd.println("Recording...");
        
        String result = speechAPI.recognizeSpeech();
        
        M5.Lcd.clear();
        M5.Lcd.println("Speech Result:");
        M5.Lcd.println(result);

        // ChatGPT APIに送信して応答を取得
        String gptResponse = chatGpt->getResponse(result);

        M5.Lcd.println("\nGPT Response:");
        M5.Lcd.println(gptResponse);

        // VoiceBoxAPIを使ってWAVデータを取得
        String wavData = voicebox->getAudio(gptResponse, speakerId);

        // WavPlayerを使ってWAVデータを再生
        wavPlayer.playWav(wavData.c_str());

        delay(5000);  // 結果を5秒間表示
        
        M5.Lcd.clear();
        M5.Lcd.println("Touch screen to start recording");
    }
}