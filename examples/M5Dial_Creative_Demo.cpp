//
// M5Dial Creative Demo for M5Siv3D - SAFE INPUT TESTING
// 入力系を段階的に追加してクラッシュ原因を特定
//

#define USE_M5_DIAL
#include "../src/M5Siv3D.h"

// デモモード列挙
enum class DemoMode {
    ColorPicker = 0,    // HSVカラーピッカー
    AnalogClock,        // アナログ時計
    SpeedMeter,         // スピードメーター
    RFIDReader,         // RFID読み取り
    DrawingMode,        // お絵描きモード
    COUNT
};

// グローバル変数
DemoMode currentMode = DemoMode::ColorPicker;
long lastEncoderValue = 0;
float hue = 0.0f;
float saturation = 1.0f;
float brightness = 1.0f;
Color currentColor = Palette::Red;
uint32_t lastModeSwitch = 0;

// 時計用変数
float clockRotation = 0.0f;

// スピードメーター用変数
float speed = 0.0f;
float targetSpeed = 0.0f;

// お絵描き用変数
struct DrawPoint {
    Math::Vec2i pos;
    Color color;
    int size;
};
std::vector<DrawPoint> drawPoints;
int brushSize = 3;

// =============================================================================
// ユーティリティ関数
// =============================================================================

void drawModeIndicator() {
    // 上部にモード名を表示
    Rect(0, 0, System::Width(), 25).draw(Palette::Darkblue);
    
    Font modeFont;
    modeFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center)
           .setVerticalAlign(Font::VerticalAlign::Center);
    
    String modeName;
    switch(currentMode) {
        case DemoMode::ColorPicker: modeName = "HSV Color Picker"; break;
        case DemoMode::AnalogClock: modeName = "Analog Clock"; break;
        case DemoMode::SpeedMeter: modeName = "Speed Meter"; break;
        case DemoMode::RFIDReader: modeName = "RFID Reader"; break;
        case DemoMode::DrawingMode: modeName = "Drawing Mode"; break;
    }
    
    modeFont(modeName, Font::Pos(System::Width()/2, 12), Palette::White);
}

void drawNavigationHelp() {
    // 下部に操作説明
    Font helpFont;
    helpFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    helpFont("Rotate: Adjust | Touch: Select | ButtonA: Mode", 
             Font::Pos(System::Width()/2, System::Height() - 10), 
             Palette::Gray);
}

// =============================================================================
// カラーピッカーモード
// =============================================================================

void updateColorPicker(long encoderDelta) {
    if (abs(encoderDelta) > 0) {
        if (Input::Touch.pressed()) {
            // タッチ中は彩度と明度を調整
            Math::Vec2i touchPos = Input::Touch.pos();
            int centerX = System::Width() / 2;
            int centerY = System::Height() / 2;
            
            float distance = Math::Vec2f(touchPos.x - centerX, touchPos.y - centerY).length();
            saturation = Math::clamp(distance / 80.0f, 0.0f, 1.0f);
            
            float angle = atan2(touchPos.y - centerY, touchPos.x - centerX);
            brightness = (Math::sin(angle) + 1.0f) * 0.5f;
        } else {
            // エンコーダーで色相を調整
            hue = Math::fmod(hue + encoderDelta * 2.0f, 360.0f);
            if (hue < 0) hue += 360.0f;
        }
        
        currentColor = Color::FromHSV(hue, saturation, brightness);
    }
}

void drawColorPicker() {
    int centerX = System::Width() / 2;
    int centerY = System::Height() / 2 + 10;
    int radius = 80;
    
    // HSVカラーホイールを描画
    for (int angle = 0; angle < 360; angle += 3) {
        Color wheelColor = Color::FromHSV(angle, 1.0f, 1.0f);
        float rad = Math::ToRadians(angle);
        
        int x1 = centerX + Math::cos(rad) * (radius - 15);
        int y1 = centerY + Math::sin(rad) * (radius - 15);
        int x2 = centerX + Math::cos(rad) * radius;
        int y2 = centerY + Math::sin(rad) * radius;
        
        Line(x1, y1, x2, y2).draw(wheelColor);
    }
    
    // 現在の色相位置にマーカーを描画
    float currentRad = Math::ToRadians(hue);
    int markerX = centerX + Math::cos(currentRad) * (radius + 10);
    int markerY = centerY + Math::sin(currentRad) * (radius + 10);
    Circle(markerX, markerY, 5).draw(Palette::White);
    Circle(markerX, markerY, 3).draw(currentColor);
    
    // 彩度・明度の可視化（中央の円）
    Circle(centerX, centerY, 30).draw(currentColor);
    
    // 彩度リング
    for (int i = 0; i < 20; i++) {
        float sat = i / 20.0f;
        Color satColor = Color::FromHSV(hue, sat, brightness);
        Circle(centerX, centerY, 35 + i).drawFrame(satColor);
    }
    
    // HSV値の表示
    Font valueFont;
    valueFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    valueFont("H:" + String((int)hue) + "°", Font::Pos(centerX, centerY + 50), Palette::White);
    valueFont("S:" + String((int)(saturation * 100)) + "%", Font::Pos(centerX - 40, centerY + 65), Palette::White);
    valueFont("V:" + String((int)(brightness * 100)) + "%", Font::Pos(centerX + 40, centerY + 65), Palette::White);
}

// =============================================================================
// アナログ時計モード
// =============================================================================

void updateAnalogClock(long encoderDelta) {
    // エンコーダーで時計の回転速度を調整
    clockRotation += encoderDelta * 0.1f;
}

void drawAnalogClock() {
    int centerX = System::Width() / 2;
    int centerY = System::Height() / 2 + 10;
    int radius = 85;
    
    // 時計の外枠
    Circle(centerX, centerY, radius).drawFrame(Palette::White);
    Circle(centerX, centerY, radius - 5).drawFrame(Palette::Gray);
    
    // 時刻表示（実際の時刻 + エンコーダー調整）
    uint32_t currentTime = millis();
    float seconds = (currentTime / 1000.0f) + clockRotation;
    float minutes = seconds / 60.0f;
    float hours = minutes / 60.0f;
    
    // 12時間表示用の目盛り
    for (int i = 0; i < 12; i++) {
        float angle = i * Math::Pi / 6 - Math::HalfPi;
        int x1 = centerX + Math::cos(angle) * (radius - 10);
        int y1 = centerY + Math::sin(angle) * (radius - 10);
        int x2 = centerX + Math::cos(angle) * (radius - 20);
        int y2 = centerY + Math::sin(angle) * (radius - 20);
        
        Line(x1, y1, x2, y2).draw(Palette::White);
        
        // 数字表示
        Font numFont;
        numFont.setSize(1)
              .setHorizontalAlign(Font::HorizontalAlign::Center)
              .setVerticalAlign(Font::VerticalAlign::Center);
        
        int numX = centerX + Math::cos(angle) * (radius - 30);
        int numY = centerY + Math::sin(angle) * (radius - 30);
        numFont(String(i == 0 ? 12 : i), Font::Pos(numX, numY), Palette::White);
    }
    
    // 針の描画
    // 時針
    float hourAngle = Math::fmod(hours, 12) * Math::Pi / 6 - Math::HalfPi;
    int hourX = centerX + Math::cos(hourAngle) * 35;
    int hourY = centerY + Math::sin(hourAngle) * 35;
    Line(centerX, centerY, hourX, hourY).draw(Color(255, 100, 100));
    
    // 分針
    float minuteAngle = Math::fmod(minutes, 60) * Math::Pi / 30 - Math::HalfPi;
    int minuteX = centerX + Math::cos(minuteAngle) * 55;
    int minuteY = centerY + Math::sin(minuteAngle) * 55;
    Line(centerX, centerY, minuteX, minuteY).draw(Color(100, 255, 100));
    
    // 秒針
    float secondAngle = Math::fmod(seconds, 60) * Math::Pi / 30 - Math::HalfPi;
    int secondX = centerX + Math::cos(secondAngle) * 70;
    int secondY = centerY + Math::sin(secondAngle) * 70;
    Line(centerX, centerY, secondX, secondY).draw(Palette::Red);
    
    // 中心点
    Circle(centerX, centerY, 4).draw(Palette::White);
    
    // デジタル時刻表示
    Font timeFont;
    timeFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    int displayHours = ((int)hours % 12);
    if (displayHours == 0) displayHours = 12;
    
    String timeStr = String(displayHours) + ":" + 
                    String((int)minutes % 60) + ":" + 
                    String((int)seconds % 60);
    
    timeFont(timeStr, Font::Pos(centerX, centerY + 110), currentColor);
}

// =============================================================================
// スピードメーターモード
// =============================================================================

void updateSpeedMeter(long encoderDelta) {
    // エンコーダーで目標速度を調整
    targetSpeed = Math::clamp(targetSpeed + encoderDelta * 2.0f, 0.0f, 200.0f);
    
    // スムーズに速度を変化させる
    speed = Math::lerp(speed, targetSpeed, 0.05f);
}

void drawSpeedMeter() {
    int centerX = System::Width() / 2;
    int centerY = System::Height() / 2 + 20;
    int radius = 80;
    
    // スピードメーターの弧を描画（180度）
    Circle(centerX, centerY, radius).drawFrame(Palette::White);
    
    // 速度目盛り（0-200km/h）
    for (int i = 0; i <= 20; i++) {
        float angle = Math::Pi + i * Math::Pi / 20;
        int x1 = centerX + Math::cos(angle) * (radius - 10);
        int y1 = centerY + Math::sin(angle) * (radius - 10);
        int x2 = centerX + Math::cos(angle) * (radius - (i % 5 == 0 ? 20 : 15));
        int y2 = centerY + Math::sin(angle) * (radius - (i % 5 == 0 ? 20 : 15));
        
        Color tickColor = (i * 10 > 120) ? Palette::Red : Palette::White;
        Line(x1, y1, x2, y2).draw(tickColor);
        
        // 数値表示（10km/h単位）
        if (i % 2 == 0) {
            Font speedFont;
            speedFont.setSize(1)
                    .setHorizontalAlign(Font::HorizontalAlign::Center);
            
            int numX = centerX + Math::cos(angle) * (radius - 35);
            int numY = centerY + Math::sin(angle) * (radius - 35);
            speedFont(String(i * 10), Font::Pos(numX, numY), tickColor);
        }
    }
    
    // スピード針
    float speedAngle = Math::Pi + (speed / 200.0f) * Math::Pi;
    int needleX = centerX + Math::cos(speedAngle) * (radius - 25);
    int needleY = centerY + Math::sin(speedAngle) * (radius - 25);
    
    Color needleColor = (speed > 120) ? Palette::Red : Palette::Green;
    Line(centerX, centerY, needleX, needleY).draw(needleColor);
    Circle(centerX, centerY, 5).draw(needleColor);
    
    // 速度値のデジタル表示
    Font speedValueFont;
    speedValueFont.setSize(3)
                 .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    speedValueFont(String((int)speed), Font::Pos(centerX, centerY + 25), needleColor);
    
    Font unitFont;
    unitFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    unitFont("km/h", Font::Pos(centerX, centerY + 45), Palette::White);
    
    // 警告表示
    if (speed > 120) {
        Font warningFont;
        warningFont.setSize(1)
                  .setHorizontalAlign(Font::HorizontalAlign::Center);
        
        warningFont("SPEED LIMIT!", Font::Pos(centerX, centerY - 40), Palette::Red);
    }
}

// =============================================================================
// RFID読み取りモード
// =============================================================================

void updateRFIDReader(long encoderDelta) {
    // RFIDモードでは特に何もしない
}

void drawRFIDReader() {
    int centerX = System::Width() / 2;
    int centerY = System::Height() / 2 + 10;
    
    // RFID読み取り状態の表示
    Font titleFont;
    titleFont.setSize(2)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
    
    titleFont("RFID Reader", Font::Pos(centerX, centerY - 50), Palette::Cyan);
    
    // カード検出インジケーター（アニメーション）
    uint32_t time = millis();
    float pulseScale = 1.0f + Math::sin(time * 0.005f) * 0.2f;
    
    if (Input::RFID.isCardPresent()) {
        // カード検出時
        Circle(centerX, centerY, 40 * pulseScale).draw(Palette::Green);
        
        Font statusFont;
        statusFont.setSize(1)
                 .setHorizontalAlign(Font::HorizontalAlign::Center);
        
        statusFont("Card Detected!", Font::Pos(centerX, centerY + 60), Palette::Green);
        
        if (auto uid = Input::RFID.readCardUID()) {
            statusFont("UID: " + uid.value(), Font::Pos(centerX, centerY + 75), Palette::Yellow);
        }
        
        // アクセスインジケーター
        for (int i = 0; i < 8; i++) {
            float angle = i * Math::TwoPi / 8 + time * 0.01f;
            int x = centerX + Math::cos(angle) * 60;
            int y = centerY + Math::sin(angle) * 60;
            Circle(x, y, 3).draw(Color::FromHSV(i * 45, 1.0f, 1.0f));
        }
        
    } else {
        // カード未検出時
        Circle(centerX, centerY, 40 * pulseScale).drawFrame(Palette::Red);
        
        Font statusFont;
        statusFont.setSize(1)
                 .setHorizontalAlign(Font::HorizontalAlign::Center);
        
        statusFont("Place Card Near Device", Font::Pos(centerX, centerY + 60), Palette::Gray);
        
        // 待機アニメーション
        float waitAngle = time * 0.01f;
        int dotX = centerX + Math::cos(waitAngle) * 50;
        int dotY = centerY + Math::sin(waitAngle) * 50;
        Circle(dotX, dotY, 5).draw(Palette::Red);
    }
}

// =============================================================================
// お絵描きモード
// =============================================================================

void updateDrawingMode(long encoderDelta) {
    // エンコーダーでブラシサイズを調整
    if (abs(encoderDelta) > 0) {
        brushSize = Math::clamp(brushSize + static_cast<int>(encoderDelta), 1, 20);
    }
    
    // タッチで描画
    if (Input::Touch.pressed()) {
        DrawPoint point;
        point.pos = Input::Touch.pos();
        point.color = currentColor;
        point.size = brushSize;
        
        // 画面上部のUI領域を避ける
        if (point.pos.y > 30) {
            drawPoints.push_back(point);
        }
    }
}

void drawDrawingMode() {
    // 描画された点を表示
    for (const auto& point : drawPoints) {
        Circle(point.pos.x, point.pos.y, point.size).draw(point.color);
    }
    
    // ブラシサイズ表示
    int centerX = System::Width() / 2;
    
    Font brushFont;
    brushFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Left);
    
    brushFont("Brush: " + String(brushSize), Font::Pos(10, 30), Palette::White);
    
    // ブラシプレビュー
    Circle(centerX + 50, 35, brushSize).draw(currentColor);
    
    // カラーパレット（小さい）
    for (int i = 0; i < 6; i++) {
        Color paletteColor = Color::FromHSV(i * 60, 1.0f, 1.0f);
        Circle(10 + i * 15, 50, 5).draw(paletteColor);
        
        if (Input::Touch.down()) {
            Math::Vec2i touchPos = Input::Touch.pos();
            if (Math::Vec2f(touchPos.x - (10 + i * 15), touchPos.y - 50).length() < 10) {
                currentColor = paletteColor;
            }
        }
    }
    
    // クリアボタン
    Rect clearButton(System::Width() - 50, 30, 40, 20);
    clearButton.draw(Palette::Red);
    
    Font clearFont;
    clearFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Center)
            .setVerticalAlign(Font::VerticalAlign::Center);
    
    clearFont("Clear", Font::Pos(System::Width() - 30, 40), Palette::White);
    
    if (clearButton.touched()) {
        drawPoints.clear();
    }
}

// =============================================================================
// メイン関数
// =============================================================================

void Main() {
    Print << "M5Dial Creative Demo";
    Print << "Multiple interactive modes demonstration";
    
    while (System::Update()) {
        // エンコーダーの変化量を取得
        long currentEncoderValue = Input::Encoder.getValueOr(0);
        long encoderDelta = currentEncoderValue - lastEncoderValue;
        lastEncoderValue = currentEncoderValue;
        
        // ボタンAでモード切り替え（デバウンス処理）
        if (Input::ButtonA.pressed() && (millis() - lastModeSwitch > 300)) {
            currentMode = static_cast<DemoMode>((static_cast<int>(currentMode) + 1) % static_cast<int>(DemoMode::COUNT));
            lastModeSwitch = millis();
            
            // モード切り替え時のリセット
            if (currentMode == DemoMode::SpeedMeter) {
                speed = 0.0f;
                targetSpeed = 0.0f;
            }
        }
        
        // 背景クリア
        System::SetBackgroundColor(Palette::Black);
        
        // 現在のモードに応じた更新と描画
        switch (currentMode) {
            case DemoMode::ColorPicker:
                updateColorPicker(encoderDelta);
                drawColorPicker();
                break;
                
            case DemoMode::AnalogClock:
                updateAnalogClock(encoderDelta);
                drawAnalogClock();
                break;
                
            case DemoMode::SpeedMeter:
                updateSpeedMeter(encoderDelta);
                drawSpeedMeter();
                break;
                
            case DemoMode::RFIDReader:
                updateRFIDReader(encoderDelta);
                drawRFIDReader();
                break;
                
            case DemoMode::DrawingMode:
                updateDrawingMode(encoderDelta);
                drawDrawingMode();
                break;
        }
        
        // 共通UI要素
        drawModeIndicator();
        drawNavigationHelp();
        
        // デバッグ情報
        ClearPrint();
        drawPrint();
    }
} 