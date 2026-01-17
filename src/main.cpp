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

// モーターインデックスからグループインデックスへのマッピング
int32_t getGroupIndex(int32_t motorIdx) {
    if (motorIdx >= 0 && motorIdx <= 2) return 0;  // HIP
    if (motorIdx >= 3 && motorIdx <= 5) return 1;  // HEAD
    if (motorIdx == 6) return 2;                   // EYELID
    if (motorIdx >= 7 && motorIdx <= 13) return 3; // ARM_L
    if (motorIdx >= 14 && motorIdx <= 20) return 4;// ARM_R
    return -1;
}

const char* getGroupName(int32_t groupIdx) {
    static const char* names[] = {"HIP", "HEAD", "EYE", "ARM_L", "ARM_R"};
    if (groupIdx >= 0 && groupIdx < 5) return names[groupIdx];
    return "";
}

void drawHeader(int32_t centerX) {
    const Color pipGreen = Color(30, 255, 30);
    const auto& state = robot::CommsManager::getInstance().getState();
    
    // 背景の薄いグリッド
    for (int i = 0; i < 240; i += 40) {
        Line(i, 0, i, 240).draw(Color(0, 30, 0));
        Line(0, i, 240, i).draw(Color(0, 30, 0));
    }

    // 接続状態インジケータ (最上部中央)
    if (!state.diagnostics.isConnected()) {
        Font().setHorizontalAlign(Font::HorizontalAlign::Center).setSize(1)
              ("!! DISCONNECTED !!", Font::Pos(centerX, 15), Palette::Red);
    } else {
        // グローバル録画ステータスの表示
        bool anyRec = false;
        bool anyPlay = false;
        for (int i = 0; i < 5; ++i) {
            if (state.groupModes[i] == 2) anyRec = true;
            if (state.groupModes[i] == 1) anyPlay = true;
        }

        String recText = "■ STOP";
        Color recColor = Palette::Gray;
        if (anyRec) {
            recText = "● REC";
            recColor = Palette::Red;
        } else if (anyPlay || state.engineRunning) {
            recText = "▶ PLAY";
            recColor = Palette::Deepskyblue;
        }

        Font().setHorizontalAlign(Font::HorizontalAlign::Center).setSize(1)
              (recText, Font::Pos(centerX, 15), recColor);

        if (state.diagnostics.errorCount > 0) {
            Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
                  ("ERR", Font::Pos(30, 15), Palette::Orange);
        }
    }

    // システムクラッシュ状態 (上部左寄り)
    const Color crashColor = (state.crashLatch == 0) ? pipGreen : Palette::Red;
    Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
          ((state.crashLatch == 0 ? "SYS:OK" : "CRASH"), Font::Pos(55, 30), crashColor);

    // ポート障害インジケータ (中央やや上)
    const char* portLabels[] = {"S", "H", "L", "R"};
    for (size_t i = 0; i < state.portFaults.size(); ++i) {
        Color c = (state.portFaults[i] == 0) ? pipGreen : Palette::Red;
        Font().setHorizontalAlign(Font::HorizontalAlign::Center).setSize(1)
              (portLabels[i], Font::Pos(95 + i * 15, 30), c);
    }

    // システム全体のトルク設定 (上部右寄り)
    const Color armColor = (state.persistentArmState == 1) ? Palette::Yellow : Palette::Gray;
    Font().setHorizontalAlign(Font::HorizontalAlign::Right).setSize(1)
          ((state.persistentArmState == 1 ? "ARM" : "DISARM"), Font::Pos(185, 30), armColor);
    
    String modeName;
    if (g_displayMode == DisplayMode::BodyDashboard) modeName = "- STATUS -";
    else if (g_displayMode == DisplayMode::MotorList) modeName = "- SENSORS -";
    else modeName = "- COMMAND -";

    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(1)
          (modeName, Font::Pos(centerX, 50), pipGreen);
}

void drawMiniSkeleton(int32_t x, int32_t y, int32_t highlightGroup) {
    const Color pipDarkGreen = Color(0, 80, 0);
    const Color pipBrightGreen = Color(30, 255, 30);
    const auto& state = robot::CommsManager::getInstance().getState();

    // スケールを小さく (0.4倍程度)
    auto toMini = [&](int16_t bx, int16_t by) {
        return Math::Vec2i(x + (bx - 120) * 4 / 10, y + (by - 120) * 4 / 10);
    };

    // 骨格ライン
    Line(toMini(120, 70), toMini(120, 160)).draw(pipDarkGreen);
    Line(toMini(90, 110), toMini(150, 110)).draw(pipDarkGreen);
    Line(toMini(120, 160), toMini(105, 180)).draw(pipDarkGreen);
    Line(toMini(120, 160), toMini(135, 180)).draw(pipDarkGreen);

    // 各グループの代表点
    struct GroupPoint { int32_t idx; int16_t bx, by; };
    const GroupPoint groupPoints[] = {
        {0, 120, 170}, // HIP
        {1, 120, 80},  // HEAD
        {2, 120, 55},  // EYE
        {3, 60, 130},  // ARM_L
        {4, 180, 130}  // ARM_R
    };

    for (const auto& gp : groupPoints) {
        Math::Vec2i p = toMini(gp.bx, gp.by);
        bool isHighlighted = (gp.idx == highlightGroup);
        int32_t mode = state.groupModes[gp.idx];
        
        Color c = pipDarkGreen;
        if (mode == 2) c = Palette::Red;       // REC
        else if (mode == 1) c = Palette::Cyan; // PLAY
        else if (isHighlighted) c = pipBrightGreen;

        if (isHighlighted) {
            float pulse = (float)sin(millis() * 0.01f) * 3.0f;
            Circle(p, 5 + (int32_t)abs(pulse)).drawFrame(pipBrightGreen);
        }

        Circle(p, 3).draw(c);
    }
}

void drawActionMenu(int32_t centerX) {
    const int32_t startY = 85;
    const int32_t itemHeight = 28;
    const int32_t maxVisible = 5;
    const auto& actions = robot::CommsManager::getInstance().getActions();
    const auto& state = robot::CommsManager::getInstance().getState();

    // ミニスケルトンの表示 (右上)
    drawMiniSkeleton(200, 70, actions[g_menuIndex].groupIdx);

    // カテゴリとステータスの表示 (左上)
    const robot::ActionItem& currentAction = actions[g_menuIndex];
    String catText = "CAT: " + String(currentAction.category.c_str());
    Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
          (catText, Font::Pos(30, 65), Palette::Gray);

    // スクロール位置の計算
    int32_t scrollOffset = 0;
    if ((int32_t)actions.size() > maxVisible) {
        scrollOffset = g_menuIndex - (maxVisible / 2);
        scrollOffset = Math::clamp(scrollOffset, 0, (int32_t)actions.size() - maxVisible);
    }

    for (int32_t i = 0; i < maxVisible && (i + scrollOffset) < (int32_t)actions.size(); ++i) {
        int32_t idx = i + scrollOffset;
        const auto& action = actions[idx];
        int32_t y = startY + (i * itemHeight);
        bool isSelected = (idx == g_menuIndex);

        // カテゴリの変わり目にライン
        if (idx > 0 && actions[idx].category != actions[idx-1].category) {
            Line(30, y - 4, 160, y - 4).draw(Color(40, 40, 40));
        }

        // 背景ハイライト
        if (isSelected) {
            Rect(25, y - 4, 165, 24).drawFrame(action.color); 
            Rect(25, y - 4, 3, 24).draw(action.color);
        }

        Color textColor = isSelected ? Palette::White : Palette::Gray;
        
        // アイコン的表示
        if (action.command.find("REC") != kstd::string::npos) {
            Circle(32, y + 8, 3).draw(isSelected ? Palette::Red : Palette::Darkred);
        } else if (action.command.find("TORQUE_ON") != kstd::string::npos) {
            Rect(29, y + 5, 6, 6).draw(isSelected ? Palette::Cyan : Palette::Gray);
        }

        Font().setHorizontalAlign(Font::HorizontalAlign::Left)
              .setSize(1)
              (action.label.c_str(), Font::Pos(42, y), textColor);

        if (isSelected) {
            // 決定ガイドを短く
            Font().setHorizontalAlign(Font::HorizontalAlign::Right)
                  .setSize(1)
                  (">>", Font::Pos(185, y), action.color);
        }
    }

    // スクロールインジケータ (中央右寄り)
    if ((int32_t)actions.size() > maxVisible) {
        float barH = (maxVisible * itemHeight);
        float progress = (float)scrollOffset / (actions.size() - maxVisible);
        Rect(195, startY, 2, barH).draw(Color(20, 20, 20));
        Rect(195, startY + (int32_t)(progress * (barH - 10)), 2, 10).draw(Palette::Gray);
    }

    // 操作ガイド
    Font().setHorizontalAlign(Font::HorizontalAlign::Center)
          .setSize(1)
          ("Dial: Select / Hold: Execute", Font::Pos(centerX, 220), Palette::Darkgray);
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
        int32_t groupIdx = getGroupIndex((int32_t)i);
        int32_t mode = (groupIdx != -1) ? state.groupModes[groupIdx] : 0;
        
        Color dotColor;
        bool hasError = (motor.errorStatus != 0);
        
        if (!motor.updated) dotColor = Color(80, 20, 20); 
        else if (state.crashLatch != 0 || hasError) dotColor = Palette::Red;
        else dotColor = pipBrightGreen;
        
        // モードに応じた「オーラ」の描画
        if (motor.updated) {
            // REC: 赤い太いパルス, PLAY: 水色の細い光, OFF: なし
            if (mode == 2) { // REC
                float pulse = (float)sin(millis() * 0.015f) * 4.0f;
                Circle(pos.x, pos.y, 6 + (int32_t)abs(pulse)).drawFrame(Palette::Red);
            } else if (mode == 1) { // PLAY
                float pulse = (float)sin(millis() * 0.005f) * 2.0f;
                Circle(pos.x, pos.y, 5 + (int32_t)abs(pulse)).drawFrame(Palette::Deepskyblue);
            }

            // 標準のジョイントパルス（エラー時は赤）
            float pulseScale = hasError ? 3.0f : 1.5f;
            float pulseSpeed = hasError ? 0.015f : 0.005f;
            float pulse = (float)sin(millis() * pulseSpeed) * pulseScale;
            
            Color ringColor = hasError ? Palette::Red : pipBrightGreen;
            Circle(pos.x, pos.y, 4 + (int32_t)abs(pulse)).drawFrame(ringColor);

            // トルク状態
            if (motor.isTorqueEnabled) {
                Rect(pos.x - 2, pos.y - 2, 4, 4).draw(dotColor);
            } else {
                Rect(pos.x - 2, pos.y - 2, 4, 4).drawFrame(dotColor);
            }
        } else {
            Rect(pos.x - 2, pos.y - 2, 4, 4).draw(dotColor);
        }
    }
}

void drawMotorList(int32_t centerX) {
    Font font;
    font.setHorizontalAlign(Font::HorizontalAlign::Left)
        .setSize(1);
    const auto& state = robot::CommsManager::getInstance().getState();

    int32_t lastGroupIdx = -1;

    for (int32_t i = 0; i < kMaxVisibleMotors; ++i) {
        int32_t motorIdx = g_scrollIndex + i;
        if (motorIdx >= (int32_t)state.motors.size()) break;

        const auto& motor = state.motors[motorIdx];
        int32_t groupIdx = getGroupIndex(motorIdx);
        int32_t y = kHeaderHeight + 25 + (i * kRowHeight);
        bool hasError = (motor.errorStatus != 0);

        // 部位の変わり目にインジケータ（グループ名の略称を表示）
        if (groupIdx != lastGroupIdx) {
            Color grpColor = Palette::Gray;
            String modeStr = "";
            if (state.groupModes[groupIdx] == 2) { grpColor = Palette::Red; modeStr = " (REC)"; }
            else if (state.groupModes[groupIdx] == 1) { grpColor = Palette::Deepskyblue; modeStr = " (PLY)"; }
            
            Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
                  (String(getGroupName(groupIdx)) + modeStr, Font::Pos(25, y - 12), grpColor);
            lastGroupIdx = groupIdx;
        }

        // 1. ラベル (エラー時は赤色)
        Color labelColor = hasError ? Palette::Red : Palette::White;
        font(motor.label.c_str(), Font::Pos(40, y), labelColor);

        // 2. 角度数値
        float degrees = motor.angleRadians * 180.0f / Math::Pi;
        Color valColor;
        if (!motor.updated) valColor = Palette::Darkgray;
        else if (hasError) valColor = Palette::Red;
        else if (motor.isTorqueEnabled) valColor = Palette::Cyan;
        else valColor = Palette::Orange;

        String valText;
        if (!motor.updated) valText = "---";
        else {
            valText = String(degrees, 0); 
            if (hasError) valText += "!";
            else {
                // グループモードの略称を付与
                if (state.groupModes[groupIdx] == 2) valText += "R";
                else if (state.groupModes[groupIdx] == 1) valText += "P";
                else valText += (motor.isTorqueEnabled ? "T" : "f");
            }
        }
        
        Font().setHorizontalAlign(Font::HorizontalAlign::Right)
              .setSize(1)
              (valText, Font::Pos(centerX + 85, y), valColor);
        
        // 3. 視覚的インジケータ
        constexpr float kRangeDeg = 45.0f;
        int32_t barX = 110;
        int32_t barWidth = 35;
        int32_t barY = y + 6;
        
        Rect(barX, barY, barWidth, 6).draw(Color(40, 40, 40));
        float normalized = (degrees + kRangeDeg) / (kRangeDeg * 2.0f);
        float progress = Math::clamp(normalized, 0.0f, 1.0f);
        
        Color barColor;
        if (!motor.updated) barColor = Color(100, 0, 0);
        else if (hasError) barColor = Palette::Red;
        else barColor = Palette::Yellow;

        Rect(barX, barY, (int32_t)(progress * barWidth), 6).draw(barColor);
        Rect(barX + barWidth/2 - 1, barY - 2, 2, 10).draw(Palette::White);

        // 4. ステータスアイコン (右端付近)
        if (motor.updated && motor.shutdownStatus != 0) {
            Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
                  ("SD", Font::Pos(centerX + 90, y), Palette::Orange);
        }

        // 5. エラー詳細 (エラーがある場合のみ、ラベルの下に小さく表示)
        if (hasError) {
            String errDetail = "";
            if (motor.errorStatus & 0x01) errDetail += "VOLT ";
            if (motor.errorStatus & 0x04) errDetail += "TEMP ";
            if (motor.errorStatus & 0x08) errDetail += "ENC ";
            if (motor.errorStatus & 0x10) errDetail += "SHOCK ";
            if (motor.errorStatus & 0x20) errDetail += "LOAD ";

            Font().setHorizontalAlign(Font::HorizontalAlign::Left).setSize(1)
                  (errDetail, Font::Pos(45, y + 10), Palette::Red);
        }
    }

    // スクロールバー
    int32_t barY = kHeaderHeight + 15;
    int32_t barH = (System::Height() - kHeaderHeight - 40);
    float progress = (float)g_scrollIndex / (state.motors.size() - kMaxVisibleMotors);
    Circle(System::Width() - 25, barY + (progress * barH), 3).draw(Palette::Gray);
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
                    // ActionMenu以外での長押しは「一括復旧」とする
                    // FAULTクリアとCRASHラッチクリアを同時に送信
                    robot::CommsManager::getInstance().sendCommand("CLEAR_FAULTS");
                    delay(50); // 送信間隔を空けて確実性を高める
                    robot::CommsManager::getInstance().sendCommand("CLEAR_CRASH_LATCH");
                    g_flashTimer = 20; 
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

        // ヘルプ (円形に合わせて位置調整)
        String helpText;
        if (g_displayMode == DisplayMode::BodyDashboard) helpText = "BtnA: Detail";
        else if (g_displayMode == DisplayMode::MotorList) helpText = "BtnA: Menu / Dial: Scroll";
        else helpText = "BtnA: Overview / Dial: Select";

        Font().setHorizontalAlign(Font::HorizontalAlign::Center)
              .setSize(1)
              (helpText, Font::Pos(centerX, System::Height() - 25), Palette::Gray);

        ClearPrint();
        drawPrint();
    }
}
