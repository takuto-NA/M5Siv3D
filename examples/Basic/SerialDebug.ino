#include <M5Siv3D.h>

void setup() {
    System::SetBackgroundColor(Palette::Blue);
    Serial.begin(115200);   // デバッグ用シリアル
    Serial2.begin(115200);  // 受信用シリアル
}

void loop() {
    if (System::Update()) {
        auto& canvas = System::getInstance().getCanvas();
        
        // Serial2からデータを読み取る
        static String receivedData;
        static String displayText;
        
        while (Serial2.available()) {
            char c = Serial2.read();
            receivedData += c;
            
            // 改行文字を検出したら、表示用テキストを更新
            if (c == '\n') {
                displayText = receivedData;
                receivedData = "";
                
                // デバッグ用にSerial出力
                Serial.print("Received: ");
                Serial.println(displayText);
            }
        }
        
        // 受信したテキストを画面に表示
        Font font;
        font.setSize(2);
        font.setAlign(Font::TextAlign::Left);
        font("Received Data:", Font::Pos(10, 10), Palette::White);
        font(displayText, Font::Pos(10, 40), Palette::Yellow);
    }
} 