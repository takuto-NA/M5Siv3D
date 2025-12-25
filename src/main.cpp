//
// M5Dial MsgPacketizer Telemetry Viewer (M5Siv3D)
// - Teensy 4.1 からのモーター角度(21個)と健康状態を受信し可視化する
//

#include "M5Siv3D.h"
#include "SafeKiboSTL.hpp"
#include "CommsManager.hpp"

namespace {

// 通信設定 (Port B: Grove 黒)
constexpr int32_t kUartBaudRate = 115200; // 安定した115200bpsを使用
constexpr int8_t kPortBRxPin = 1; // G2
constexpr int8_t kPortBTxPin = 2; // G1

// システム状態
int32_t g_scrollIndex = 0;   // エンコーダーで操作するスクロール位置
constexpr int32_t kMaxVisibleMotors = 5;

enum class DisplayMode {
    BodyDashboard, // 概要（Pip-boy風）
    MotorList,      // 詳細リスト
    ActionMenu     // コマンドメニュー
};
DisplayMode g_displayMode = DisplayMode::BodyDashboard;

int32_t g_menuIndex = 0;
uint32_t g_flashTimer = 0; // 送信時のフィードバック用

// ボタン操作管理 (UIUX: 誤操作防止とホールドメーター用)
uint32_t g_pressStartTime = 0;
bool g_commandExecuted = false;
constexpr uint32_t kHoldThresholdMs = 800;

// レイアウト定数
constexpr int32_t kHeaderHeight = 45;
constexpr int32_t kRowHeight = 30;

// ボディダッシュボード用座標 (身体の部位ごとの配置)
struct BodyPos {
    int16_t x, y;
};
const std::array<BodyPos, 21> g_bodyCoords = {{
    {135, 180}, {105, 180}, {120, 160}, // 0:HipL(右), 1:HipR(左), 2:HipTwist(上)
    {130, 85},  {110, 85},  {120, 70}, {120, 55}, // 3:NeckL, 4:NeckR, 5:NeckTwist, 6:Eyelid(最上部)
    {90, 110}, {80, 120}, {70, 130}, {60, 140}, {50, 150}, {40, 160}, {30, 170}, // LeftArm 01-07 (左)
    {150, 110}, {160, 120}, {170, 130}, {180, 140}, {190, 150}, {200, 160}, {210, 170} // RightArm 01-07 (右)
}};

void drawHeader(int32_t centerX) {
    const Color pipGreen = Color(30, 255, 30);
    const auto& state = robot::CommsManager::getInstance().getState();
    
    // 背景の薄いグリッド (透過の代わりに暗い色で)
    for (int i = 0; i < 240; i += 40) {
        Line(i, 0, i, 240).draw(Color(0, 30, 0));
        Line(0, i, 240, i).draw(Color(0, 30, 0));
    }

    // 接続状態インジケータ (左端)
    if (!state.diagnostics.isConnected()) {
        Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
              ("DISCONNECTED", Font::Pos(10, 5), Palette::Red);
    } else if (state.diagnostics.errorCount > 0) {
        Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
              ("COMM ERR", Font::Pos(10, 5), Palette::Orange);
    }

    // システムクラッシュ状態 (左上)
    const Color crashColor = (state.crashLatch == 0) ? pipGreen : Palette::Red;
    Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
          ((state.crashLatch == 0 ? "SYS:OK" : "SYS:CRASH"), Font::Pos(20, 20), crashColor);

    // ポート障害インジケータ (中央)
    const char* portLabels[] = {"S", "H", "L", "R"};
    for (size_t i = 0; i < state.portFaults.size(); ++i) {
        Color c = (state.portFaults[i] == 0) ? pipGreen : Palette::Red;
        Font().setHorizontalAlign(Font::HorizontalAlign::Center).setSize(1)
              (portLabels[i], Font::Pos(95 + i * 15, 20), c);
    }

    // システム全体のトルク設定 (右上)
    const Color armColor = (state.persistentArmState == 1) ? Palette::Yellow : Palette::Gray;
    Font().setHorizontalAlign(Font::HorizontalAlign::Right).setSize(1)
          ((state.persistentArmState == 1 ? "ARM" : "DISARM"), Font::Pos(220, 20), armColor);
    
    String modeName;
    if (g_displayMode == DisplayMode::BodyDashboard) modeName = "-- STATUS OVERVIEW --";
    else if (g_displayMode == DisplayMode::MotorList) modeName = "-- SENSOR DETAIL --";
    else modeName = "-- COMMAND MENU --";

    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(1)
          (modeName, Font::Pos(centerX, 45), pipGreen);
}

void drawActionMenu(int32_t centerX) {
    const int32_t startY = 70;
    const int32_t itemHeight = 35;
    const auto& actions = robot::CommsManager::getInstance().getActions();

    for (int32_t i = 0; i < (int32_t)actions.size(); ++i) {
        const auto& action = actions[i];
        int32_t y = startY + (i * itemHeight);
        bool isSelected = (i == g_menuIndex);

        // 背景ハイライト (UIUX: 選択状態を明確に)
        if (isSelected) {
            Rect(20, y - 5, 200, 30).drawFrame(action.color); // 枠線でハイライト
            Rect(20, y - 5, 4, 30).draw(action.color);      // 左側にアクセントバー
        }

        Color textColor = isSelected ? Palette::White : Palette::Gray;
        Font().setHorizontalAlign(Font::HorizontalAlign::Left)
              .setSize(1)
              (action.label.c_str(), Font::Pos(35, y), textColor);

        if (isSelected) {
            // 決定ガイド
            Font().setHorizontalAlign(Font::HorizontalAlign::Right)
                  .setSize(1)
                  ("HOLD >", Font::Pos(210, y), action.color);
        }
    }

    // 操作ガイド
    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(1)
          ("Dial: Select / Btn Hold: Execute", Font::Pos(centerX, 200), Palette::Darkgray);
}

void drawBodyDashboard() {
    const int32_t cx = 120;
    const Color pipDarkGreen = Color(0, 100, 0);
    const Color pipBrightGreen = Color(30, 255, 30);
    const auto& state = robot::CommsManager::getInstance().getState();

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

    for (size_t i = 0; i < state.motors.size(); ++i) {
        const auto& motor = state.motors[i];
        const auto& pos = g_bodyCoords[i];
        
        Color dotColor;
        if (!motor.updated) dotColor = Color(80, 20, 20); 
        else if (state.crashLatch != 0) dotColor = Palette::Red;
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
    const auto& state = robot::CommsManager::getInstance().getState();

    for (int32_t i = 0; i < kMaxVisibleMotors; ++i) {
        int32_t motorIdx = g_scrollIndex + i;
        if (motorIdx >= (int32_t)state.motors.size()) break;

        const auto& motor = state.motors[motorIdx];
        int32_t y = kHeaderHeight + 20 + (i * kRowHeight);

        // 1. ラベル (左端)
        font(motor.label.c_str(), Font::Pos(25, y), Palette::White);

        // 2. 角度数値 (右端)
        float degrees = motor.angleRadians * 180.0f / Math::Pi;
        // 角度数値の色をトルク状態に合わせて変える
        Color valColor = motor.updated ? (motor.isTorqueEnabled ? Palette::Cyan : Palette::Orange) : Palette::Darkgray;
        String valText = motor.updated ? String(degrees, 1) + (motor.isTorqueEnabled ? "T" : "_") : "---";
        
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
    float progress = (float)g_scrollIndex / (state.motors.size() - kMaxVisibleMotors);
    Circle(System::Width() - 10, barY + (progress * barH), 3).draw(Palette::Gray);
}

} // namespace

void Main() {
    System::SetBackgroundColor(Palette::Black);

    // 通信初期化
    robot::CommsManager::getInstance().init(kUartBaudRate, kPortBRxPin, kPortBTxPin);

    while (System::Update()) {
        // 通信更新
        robot::CommsManager::getInstance().update();
        const auto& state = robot::CommsManager::getInstance().getState();
        const auto& actions = robot::CommsManager::getInstance().getActions();

        // --- ボタンAの押下状態管理 ---
        if (Input::getButtonA().pressed()) {
            g_pressStartTime = millis();
            g_commandExecuted = false;
        }

        uint32_t holdTime = 0;
        if (Input::getButtonA().down()) {
            holdTime = millis() - g_pressStartTime;
            
            // 長押し中かつ未実行の場合
            if (holdTime >= kHoldThresholdMs && !g_commandExecuted) {
                if (g_displayMode == DisplayMode::ActionMenu) {
                    robot::CommsManager::getInstance().sendCommand(actions[g_menuIndex].command);
                    g_flashTimer = 15;
                } else {
                    // ActionMenu以外でも長押しでCLEAR_FAULTSを実行
                    robot::CommsManager::getInstance().sendCommand("CLEAR_FAULTS");
                    g_flashTimer = 10;
                }
                g_commandExecuted = true;
            }
        }

        // モード切り替え（ボタンAを離した瞬間）
        if (Input::getButtonA().released()) {
            // 長押しコマンドが実行されておらず、かつ押下時間が短かった場合のみ切り替え
            if (!g_commandExecuted && holdTime < kHoldThresholdMs) {
                if (g_displayMode == DisplayMode::BodyDashboard) g_displayMode = DisplayMode::MotorList;
                else if (g_displayMode == DisplayMode::MotorList) g_displayMode = DisplayMode::ActionMenu;
                else g_displayMode = DisplayMode::BodyDashboard;
            }
            // リセット
            g_commandExecuted = false;
            g_pressStartTime = 0;
        }

        // エンコーダーで操作
        const long encoderDelta = Input::getDialEncoder().getDelta().value_or(0);
        if (encoderDelta != 0) {
            if (g_displayMode == DisplayMode::MotorList) {
                // 詳細リスト時はスクロール
                g_scrollIndex = Math::clamp((int32_t)(g_scrollIndex + encoderDelta), 0, (int32_t)state.motors.size() - kMaxVisibleMotors);
            } else if (g_displayMode == DisplayMode::ActionMenu) {
                // メニュー時は項目選択
                g_menuIndex = (g_menuIndex + (int32_t)encoderDelta + (int32_t)actions.size()) % (int32_t)actions.size();
            }
        }

        // 描画
        const int32_t centerX = System::Width() / 2;
        drawHeader(centerX);
        
        if (g_displayMode == DisplayMode::BodyDashboard) {
            drawBodyDashboard();
        } else if (g_displayMode == DisplayMode::MotorList) {
            drawMotorList(centerX);
        } else {
            drawActionMenu(centerX);
        }

        // ホールドメーターの描画 (UIUX: 進行状況の可視化)
        if (Input::getButtonA().down() && !g_commandExecuted) {
            float progress = Math::clamp((float)holdTime / kHoldThresholdMs, 0.0f, 1.0f);
            // 中央ボタンの周囲に円形のプログレスバーを表示 (0~360度)
            Circle(120, 120, 42).drawArc(4, 0, (int32_t)(progress * 360), Palette::Cyan);
        }

        // 送信フィードバック（UIUX: 実行したことが一目でわかるフラッシュ）
        if (g_flashTimer > 0) {
            Circle(System::Width() / 2, System::Height() / 2, 118).drawFrame(Palette::Orange);
            g_flashTimer--;
        }

        // ヘルプ
        String helpText;
        if (g_displayMode == DisplayMode::BodyDashboard) helpText = "BtnA: Detail Mode";
        else if (g_displayMode == DisplayMode::MotorList) helpText = "BtnA: Menu / Dial: Scroll";
        else helpText = "BtnA: Overview / Dial: Select";

        Font().setHorizontalAlign(Font::HorizontalAlign::Center)
              .setSize(1)
              (helpText, Font::Pos(centerX, System::Height() - 15), Palette::Gray);

        ClearPrint();
        drawPrint();
    }
}
