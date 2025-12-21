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
    {120, 140}, {120, 150}, {120, 130}, // HipL, HipR, HipTwist (Spine)
    {120, 80},  {120, 70},  {120, 90}, {120, 60}, // NeckL, NeckR, NeckTwist, Eyelid (Head)
    {100, 110}, {90, 120}, {80, 130}, {70, 140}, {60, 150}, {50, 160}, {40, 170}, // LeftArm 01-07
    {140, 110}, {150, 120}, {160, 130}, {170, 140}, {180, 150}, {190, 160}, {200, 170} // RightArm 01-07
};

void drawHeader(int32_t centerX) {
    const Color healthColor = (g_systemHealth == 0) ? Palette::Green : Palette::Red;
    const String healthText = (g_systemHealth == 0) ? "SYSTEM: OK" : "SYSTEM: FAULT";

    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(2)
          (healthText, Font::Pos(centerX, 25), healthColor);
    
    String modeName = (g_displayMode == DisplayMode::BodyDashboard) ? "[OVERVIEW]" : "[DETAIL]";
    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(1)
          (modeName, Font::Pos(centerX, 45), Palette::Gray);
}

void drawBodyDashboard() {
    // ボディダッシュボード（Pip-boy風）の描画
    const int32_t cx = System::Width() / 2;

    // 身体のメインライン（Spine）
    Line(cx, 70, cx, 150).draw(Color(0, 100, 0));
    // 肩ライン
    Line(cx - 40, 110, cx + 40, 110).draw(Color(0, 100, 0));

    for (size_t i = 0; i < g_motors.size(); ++i) {
        const auto& motor = g_motors[i];
        const auto& pos = g_bodyCoords[i];
        
        // モーターの状態色
        Color dotColor;
        if (!motor.updated) dotColor = Color(60, 0, 0); // スタール
        else if (g_systemHealth != 0) dotColor = Palette::Red; // システム異常
        else dotColor = Palette::Green; // 正常
        
        // モーターを矩形で表示
        Rect(pos.x - 3, pos.y - 3, 6, 6).draw(dotColor);
        if (motor.updated) {
            // アクティブなモーターは白い枠で強調
            Rect(pos.x - 3, pos.y - 3, 6, 6).drawFrame(Palette::White);
        }
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

        // モード切り替え（ボタンA長押し、またはダブルクリックの代わりに「特定の秒数」で判定も可能ですが、
        // 今回はボタンAの「離した瞬間」で切り替えるようにします）
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

        // 長押しでリセットコマンド送信 (ここでは簡易的にpressedDurationを使用)
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
