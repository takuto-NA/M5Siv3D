//
// M5Dial Example for M5Siv3D
// Demonstrates encoder rotation and RFID card detection
//

// M5Dialを使用する場合はこのマクロを定義
#define USE_M5_DIAL

#include "../src/M5Siv3D.h"

// グローバル変数
long lastEncoderValue = 0;
int displayMode = 0;  // 0: Encoder value, 1: Circular display, 2: RFID status
Color currentColor = Palette::White;
float hue = 0.0f;

void drawEncoderValue(long value) {
    // 背景をクリア
    Rect(0, 30, System::Width(), 180).draw(Palette::Black);
    
    // エンコーダー値を大きく表示
    Font largeFont;
    largeFont.setSize(3)
            .setHorizontalAlign(Font::HorizontalAlign::Center)
            .setVerticalAlign(Font::VerticalAlign::Center);
    
    largeFont(String(value), Font::Pos(System::Width()/2, 120), currentColor);
    
    // 変化量も表示
    long delta = Input::Encoder.getDelta();
    if (delta != 0) {
        Font smallFont;
        smallFont.setSize(1)
                .setHorizontalAlign(Font::HorizontalAlign::Center);
        
        String deltaText = "Δ" + String(delta);
        smallFont(deltaText, Font::Pos(System::Width()/2, 160), 
                 delta > 0 ? Palette::Green : Palette::Red);
    }
    
    // 操作説明
    Font helpFont;
    helpFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    helpFont("Rotate: Change value", Font::Pos(System::Width()/2, 210), Palette::Gray);
    helpFont("Button A: Reset / Change mode", Font::Pos(System::Width()/2, 225), Palette::Gray);
}

void drawCircularDisplay(long value) {
    // 背景をクリア
    Rect(0, 30, System::Width(), 180).draw(Palette::Black);
    
    int centerX = System::Width() / 2;
    int centerY = 120;
    int radius = 60;
    
    // 外枠を描画
    Circle(centerX, centerY, radius).drawFrame(Palette::White);
    Circle(centerX, centerY, radius - 10).drawFrame(Palette::Darkgray);
    
    // 角度計算（0-360度）
    float angle = Math::fmod(value, 360) * Math::Pi / 180.0f;
    
    // 針の座標計算
    int needleX = centerX + (radius - 15) * cos(angle - Math::HalfPi);
    int needleY = centerY + (radius - 15) * sin(angle - Math::HalfPi);
    
    // 針を描画
    Line(centerX, centerY, needleX, needleY).draw(currentColor);
    Circle(centerX, centerY, 5).draw(currentColor);
    
    // 目盛りを描画
    for (int i = 0; i < 12; i++) {
        float tickAngle = i * Math::Pi / 6;
        int tick1X = centerX + (radius - 5) * cos(tickAngle - Math::HalfPi);
        int tick1Y = centerY + (radius - 5) * sin(tickAngle - Math::HalfPi);
        int tick2X = centerX + radius * cos(tickAngle - Math::HalfPi);
        int tick2Y = centerY + radius * sin(tickAngle - Math::HalfPi);
        
        Line(tick1X, tick1Y, tick2X, tick2Y).draw(Palette::White);
    }
    
    // 数値表示
    Font valueFont;
    valueFont.setSize(2)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
    valueFont(String(value), Font::Pos(centerX, 200), Palette::Green);
    
    Font angleFont;
    angleFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
    angleFont("Angle: " + String((int)Math::fmod(value, 360)) + "°", 
             Font::Pos(centerX, 45), Palette::White);
}

void drawRFIDStatus() {
    // 背景をクリア
    Rect(0, 30, System::Width(), 180).draw(Palette::Black);
    
    Font titleFont;
    titleFont.setSize(2)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
    titleFont("RFID Reader", Font::Pos(System::Width()/2, 60), Palette::Cyan);
    
    Font statusFont;
    statusFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    // カード検出状態を表示
    if (Input::RFID.isCardPresent()) {
        statusFont("Card Detected!", Font::Pos(System::Width()/2, 100), Palette::Green);
        
        // UID表示
        String uid = Input::RFID.readCardUID();
        if (uid.length() > 0) {
            statusFont("UID: " + uid, Font::Pos(System::Width()/2, 120), Palette::Yellow);
        }
        
        // インジケーター
        Circle(System::Width()/2, 150, 20).draw(Palette::Green);
        
    } else {
        statusFont("No Card", Font::Pos(System::Width()/2, 100), Palette::Red);
        statusFont("Place card near device", Font::Pos(System::Width()/2, 120), Palette::Gray);
        
        // インジケーター
        Circle(System::Width()/2, 150, 20).drawFrame(Palette::Red);
    }
    
    statusFont("RFID Mode", Font::Pos(System::Width()/2, 200), Palette::White);
}

void drawHeader() {
    // ヘッダー背景
    Rect(0, 0, System::Width(), 30).draw(Palette::Darkblue);
    
    Font headerFont;
    headerFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Left)
            .setVerticalAlign(Font::VerticalAlign::Center);
    
    String modeText;
    switch(displayMode) {
        case 0: modeText = "Encoder Value"; break;
        case 1: modeText = "Circular Display"; break;
        case 2: modeText = "RFID Reader"; break;
    }
    
    headerFont("M5Dial: " + modeText, Font::Pos(5, 15), Palette::White);
}

void Main() {
    // M5Dialの初期化（エンコーダーとRFIDを有効化）
    auto cfg = M5.config();
    M5RealUnified::begin(cfg, true, true);  // encoder=true, rfid=true
    
    Print << "M5Siv3D with M5Dial Example";
    Print << "Encoder and RFID Demo";
    Print << "Rotate encoder to change values";
    Print << "Press Button A to change modes";
    
    while (System::Update()) {
        // 現在のエンコーダー値を取得
        long currentValue = Input::Encoder.getValue();
        
        // エンコーダーが変化した場合の処理
        if (currentValue != lastEncoderValue) {
            lastEncoderValue = currentValue;
            
            // 色相を変更（HSVカラー）
            hue = Math::fmod(currentValue * 5.0f, 360.0f);
            currentColor = Color::FromHSV(hue, 1.0f, 1.0f);
        }
        
        // ボタンAが押されたときの処理
        if (Input::ButtonA.pressed()) {
            switch(displayMode) {
                case 0:
                case 1:
                    // エンコーダーをリセット
                    Input::Encoder.reset();
                    lastEncoderValue = 0;
                    // モード切り替え
                    displayMode = (displayMode + 1) % 3;
                    break;
                case 2:
                    // RFIDモードから戻る
                    displayMode = 0;
                    break;
            }
        }
        
        // ヘッダーを描画
        drawHeader();
        
        // 現在のモードに応じて表示を切り替え
        switch(displayMode) {
            case 0:
                drawEncoderValue(currentValue);
                break;
            case 1:
                drawCircularDisplay(currentValue);
                break;
            case 2:
                drawRFIDStatus();
                break;
        }
        
        // デバッグ情報を画面下部に表示
        ClearPrint();
        drawPrint();
    }
} 