//
// M5Dial Creative Demo for M5Siv3D - SAFE INPUT TESTING
// 入力系を段階的に追加してクラッシュ原因を特定
//

#define USE_M5_DIAL
#include "../src/M5Siv3D.h"

void Main() {
    Font font;
    int counter = 0;
    
    while (System::Update()) {
        System::SetBackgroundColor(Palette::Black);
        
        Circle(System::Width()/2, System::Height()/2, 30).draw(Palette::Red);
        
        font.setSize(2)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
        
        font("M5Dial Safe Test: " + String(counter), 
             Font::Pos(System::Width()/2, System::Height()/2 + 50), 
             Palette::White);
        
        // === 段階的入力テスト（ESP32では例外使用不可） ===
        
        // Step 1: タッチ入力テスト（最も安全）
        if (Input::Touch.pressed()) {
            counter++;
            font("Touch Detected!", 
                 Font::Pos(System::Width()/2, System::Height()/2 + 80), 
                 Palette::Green);
        }
        
        // Step 2: ボタンAテスト
        if (Input::ButtonA.pressed()) {
            counter += 10;
            font("Button A Pressed!", 
                 Font::Pos(System::Width()/2, System::Height()/2 + 110), 
                 Palette::Yellow);
        }
        
        // Step 3: エンコーダーテスト（初期化後は安全のはず）
        static long lastEncoderValue = 0;
        long encoderValue = Input::Encoder.getValue();
        if (encoderValue != lastEncoderValue) {
            counter += (encoderValue - lastEncoderValue);
            lastEncoderValue = encoderValue;
            font("Encoder: " + String(encoderValue), 
                 Font::Pos(System::Width()/2, System::Height()/2 + 140), 
                 Palette::Cyan);
        }
        
        // 自動増加（バックアップ）
        static uint32_t lastTime = 0;
        if (millis() - lastTime > 1000) {
            counter++;
            if (counter > 9999) counter = 0;
            lastTime = millis();
        }
        
        delay(50);
    }
} 