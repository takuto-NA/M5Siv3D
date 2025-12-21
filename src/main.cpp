//
// M5Dial MsgPacketizer Telemetry Viewer (M5Siv3D)
// - Teensy 4.1 からのモーター角度(21個)と健康状態を受信し可視化する
//

#include "M5Siv3D.h"
#include <MsgPacketizer.h>
#include <vector>

namespace {

// 通信設定 (Port B: Grove 黒)
constexpr int32_t kUartBaudRate = 115200; // 安定した115200bpsを使用
constexpr int8_t kPortBRxPin = 1; // G2
constexpr int8_t kPortBTxPin = 2; // G1

// MsgPacketizer インデックス (Teensy側の最新定義に合わせる)
constexpr uint8_t kIndexMotorTelemetry = 10;
constexpr uint8_t kIndexSystemHealth = 11;
constexpr uint8_t kIndexCommand = 0;

// モーター構成
struct MotorInfo {
    String label;
    float angleRadians = 0.0f;
    bool updated = false;
};

// TeensyのProjectConfigに基づいたモーターリスト
std::vector<MotorInfo> g_motors = {
    {"HipL"}, {"HipR"}, {"HipTwist"},
    {"NeckL"}, {"NeckR"}, {"NeckTwist"}, {"Eyelid"},
    {"LeftArm01"}, {"LeftArm02"}, {"LeftArm03"}, {"LeftArm04"}, {"LeftArm05"}, {"LeftArm06"}, {"LeftArm07"},
    {"RightArm01"}, {"RightArm02"}, {"RightArm03"}, {"RightArm04"}, {"RightArm05"}, {"RightArm06"}, {"RightArm07"}
};

// システム状態
int32_t g_systemHealth = 0; // 0: OK, 1: FAULT
int32_t g_scrollIndex = 0;   // エンコーダーで操作するスクロール位置
constexpr int32_t kMaxVisibleMotors = 5;

enum class DisplayMode {
    BodyDashboard, // 概要（Pip-boy風）
    MotorList      // 詳細リスト
};
DisplayMode g_displayMode = DisplayMode::BodyDashboard;

// レイアウト定数
constexpr int32_t kHeaderHeight = 45;
constexpr int32_t kRowHeight = 30;

// ボディダッシュボード用座標 (身体の部位ごとの配置)
struct BodyPos {
    int16_t x, y;
};
const BodyPos g_bodyCoords[] = {
    {135, 180}, {105, 180}, {120, 160}, // 0:HipL(右), 1:HipR(左), 2:HipTwist(上)
    {130, 85},  {110, 85},  {120, 70}, {120, 55}, // 3:NeckL, 4:NeckR, 5:NeckTwist, 6:Eyelid(最上部)
    {90, 110}, {80, 120}, {70, 130}, {60, 140}, {50, 150}, {40, 160}, {30, 170}, // LeftArm 01-07 (左)
    {150, 110}, {160, 120}, {170, 130}, {180, 140}, {190, 150}, {200, 160}, {210, 170} // RightArm 01-07 (右)
};

void drawHeader(int32_t centerX) {
    const Color pipGreen = Color(30, 255, 30);
    const Color healthColor = (g_systemHealth == 0) ? pipGreen : Palette::Red;
    const String healthText = (g_systemHealth == 0) ? "SYSTEM: OK" : "SYSTEM: FAULT";

    // 背景の薄いグリッド (透過の代わりに暗い色で)
    for (int i = 0; i < 240; i += 40) {
        Line(i, 0, i, 240).draw(Color(0, 30, 0));
        Line(0, i, 240, i).draw(Color(0, 30, 0));
    }

    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(2)
          (healthText, Font::Pos(centerX, 25), healthColor);
    
    String modeName = (g_displayMode == DisplayMode::BodyDashboard) ? "-- STATUS OVERVIEW --" : "-- SENSOR DETAIL --";
    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(1)
          (modeName, Font::Pos(centerX, 45), pipGreen);
}

void drawBodyDashboard() {
    const int32_t cx = 120;
    const Color pipDarkGreen = Color(0, 100, 0);
    const Color pipBrightGreen = Color(30, 255, 30);

    // 骨格ライン（Pip-boy風）
    Line(cx, 70, cx, 160).draw(pipDarkGreen); 
    Line(cx-30, 110, cx+30, 110).draw(pipDarkGreen); 
    Line(cx, 160, g_bodyCoords[0].x, g_bodyCoords[0].y).draw(pipDarkGreen);
    Line(cx, 160, g_bodyCoords[1].x, g_bodyCoords[1].y).draw(pipDarkGreen);

    // コーナーデコレーション
    int32_t d = 15;
    Line(20, 20, 20+d, 20).draw(pipBrightGreen);
    Line(20, 20, 20, 20+d).draw(pipBrightGreen);
    Line(220, 220, 220-d, 220).draw(pipBrightGreen);
    Line(220, 220, 220, 220-d).draw(pipBrightGreen);

    for (size_t i = 0; i < g_motors.size(); ++i) {
        const auto& motor = g_motors[i];
        const auto& pos = g_bodyCoords[i];
        
        Color dotColor;
        if (!motor.updated) dotColor = Color(80, 20, 20); 
        else if (g_systemHealth != 0) dotColor = Palette::Red;
        else dotColor = pipBrightGreen;
        
        // ジョイントの描画
        if (motor.updated) {
            float pulse = (float)sin(millis() * 0.005f) * 2.0f;
            Circle(pos.x, pos.y, 4 + (int32_t)abs(pulse)).drawFrame(pipBrightGreen);
        }
        Rect(pos.x - 2, pos.y - 2, 4, 4).draw(dotColor);
    }
}

void drawMotorList(int32_t centerX) {
    Font font;
    font.setHorizontalAlign(Font::HorizontalAlign::Left)
        .setSize(1);

    for (int32_t i = 0; i < kMaxVisibleMotors; ++i) {
        int32_t motorIdx = g_scrollIndex + i;
        if (motorIdx >= (int32_t)g_motors.size()) break;

        const auto& motor = g_motors[motorIdx];
        int32_t y = kHeaderHeight + 20 + (i * kRowHeight);

        // 1. ラベル (左端)
        font(motor.label, Font::Pos(25, y), Palette::White);

        // 2. 角度数値 (右端)
        float degrees = motor.angleRadians * 180.0f / Math::Pi;
        String valText = motor.updated ? String(degrees, 1) + "d" : "---"; // degの代わりにd
        Color valColor = motor.updated ? Palette::Cyan : Palette::Darkgray;
        
        Font().setHorizontalAlign(Font::HorizontalAlign::Right)
              .setSize(1)
              (valText, Font::Pos(System::Width() - 25, y), valColor);
        
        // 3. 視覚的インジケータ（中央付近）
        // 範囲を -PI/4 ~ PI/4 (-45deg ~ 45deg) に設定
        constexpr float kRangeDeg = 45.0f;
        int32_t barX = 110;
        int32_t barWidth = 60;
        int32_t barY = y + 6;
        
        // 背景（溝）
        Rect(barX, barY, barWidth, 6).draw(Color(40, 40, 40));
        
        // 正規化 (0.0 ~ 1.0)
        float normalized = (degrees + kRangeDeg) / (kRangeDeg * 2.0f);
        float progress = Math::clamp(normalized, 0.0f, 1.0f);
        
        // アクティブなバー（更新されていれば黄色、いなければ暗い赤）
        Color barColor = motor.updated ? Palette::Yellow : Color(100, 0, 0);
        Rect(barX, barY, (int32_t)(progress * barWidth), 6).draw(barColor);
        
        // センターマーカー (0度位置)
        Rect(barX + barWidth/2 - 1, barY - 2, 2, 10).draw(Palette::White);
    }

    // スクロールバーの代わり
    int32_t barY = kHeaderHeight + 10;
    int32_t barH = (System::Height() - kHeaderHeight - 20);
    float progress = (float)g_scrollIndex / (g_motors.size() - kMaxVisibleMotors);
    Circle(System::Width() - 10, barY + (progress * barH), 3).draw(Palette::Gray);
}

} // namespace

void Main() {
    System::SetBackgroundColor(Palette::Black);

    // Serial2 (Port B) 初期化
    Serial2.begin(kUartBaudRate, SERIAL_8N1, kPortBRxPin, kPortBTxPin);

    // MsgPacketizer 購読設定
    // 1. モーター角度配列 (Index 10)
    MsgPacketizer::subscribe(Serial2, kIndexMotorTelemetry, [](const std::vector<float>& angles) {
        for (size_t i = 0; i < angles.size() && i < g_motors.size(); ++i) {
            if (angles[i] > -900.0f) {
                g_motors[i].angleRadians = angles[i];
                g_motors[i].updated = true;
            } else {
                g_motors[i].updated = false;
            }
        }
    });

    // 2. システム健康状態 (Index 11)
    MsgPacketizer::subscribe(Serial2, kIndexSystemHealth, [](int32_t health) {
        g_systemHealth = health;
    });

    while (System::Update()) {
        // MsgPacketizer の更新
        MsgPacketizer::update();

        // モード切り替え（ボタンAを離した瞬間）
        if (Input::getButtonA().released()) {
            if (g_displayMode == DisplayMode::BodyDashboard) g_displayMode = DisplayMode::MotorList;
            else g_displayMode = DisplayMode::BodyDashboard;
        }

        // エンコーダーで操作
        const long encoderDelta = Input::getDialEncoder().getDelta().value_or(0);
        if (encoderDelta != 0) {
            if (g_displayMode == DisplayMode::MotorList) {
                // 詳細リスト時はスクロール
                g_scrollIndex = Math::clamp((int32_t)(g_scrollIndex + encoderDelta), 0, (int32_t)g_motors.size() - kMaxVisibleMotors);
            }
        }

        // 長押しでリセットコマンド送信
        if (Input::getButtonA().pressedDuration(1000)) {
            MsgPacketizer::send(Serial2, kIndexCommand, String("CLEAR_FAULTS"));
            Circle(System::Width()/2, System::Height()/2, 20).draw(Palette::Orange);
        }

        // 描画
        const int32_t centerX = System::Width() / 2;
        drawHeader(centerX);
        
        if (g_displayMode == DisplayMode::BodyDashboard) {
            drawBodyDashboard();
        } else {
            drawMotorList(centerX);
        }

        // ヘルプ
        String helpText = (g_displayMode == DisplayMode::BodyDashboard) ? "BtnA: Detail Mode" : "BtnA: Overview / Dial: Scroll";
        Font().setHorizontalAlign(Font::HorizontalAlign::Center)
              .setSize(1)
              (helpText, Font::Pos(centerX, System::Height() - 15), Palette::Gray);

        ClearPrint();
        drawPrint();
    }
}
