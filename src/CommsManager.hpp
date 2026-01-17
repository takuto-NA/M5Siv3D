#pragma once

#include "M5Siv3D.h"
#include "SafeKiboSTL.hpp"
#include <MsgPacketizer.h>

namespace robot {

// =============================================================================
// Comms Protocol Notes
// - This module subscribes/sends MsgPacketizer packets via Serial2.
// - Index / payload layout is documented in: docs/CommsProtocol.md
// =============================================================================

// モーター情報
struct MotorInfo {
    kstd::string label;
    float angleRadians = 0.0f;
    bool isTorqueEnabled = false;
    uint8_t errorStatus = 0;    // Register 70 (Hardware Error Status)
    uint8_t shutdownStatus = 0; // Register 63 (Shutdown)
    bool updated = false;
    uint32_t lastUpdateMs = 0;
};

// アクションアイテム
struct ActionItem {
    kstd::string label;
    kstd::string command;
    Color color;
    int32_t groupIdx; // -1: Global/System, 0:HIP, 1:HEAD, 2:EYE, 3:ARM_L, 4:ARM_R
    kstd::string category;
};

// 診断データ
struct DiagnosticData {
    uint32_t lastTelemetryMs = 0;
    uint32_t lastHealthMs = 0;
    uint32_t msgCount = 0;
    uint32_t errorCount = 0;
    kstd::string lastError;
    
    bool isConnected() const {
        return (millis() - lastTelemetryMs < 2000) || (millis() - lastHealthMs < 2000);
    }
};

// ロボットの全体状態
struct RobotState {
    kstd::vector<MotorInfo> motors;
    int32_t crashLatch = 0;
    int32_t persistentArmState = 0;
    kstd::vector<int32_t> portFaults;
    int32_t engineRunning = 0;   // 1: 録画エンジン動作中
    int32_t groupModes[5] = {0}; // 0:OFF, 1:PLAY, 2:REC (Hip, Head, Eyelid, ArmL, ArmR)
    DiagnosticData diagnostics;

    RobotState() {
        portFaults = {0, 0, 0, 0};
    }
};

class CommsManager {
public:
    static CommsManager& getInstance() {
        static CommsManager instance;
        return instance;
    }

    void init(int32_t baudRate, int8_t rxPin, int8_t txPin) {
        Serial2.begin(baudRate, SERIAL_8N1, rxPin, txPin);
        setupSubscriptions();
    }

    void update() {
        MsgPacketizer::update();
    }
    
    void sendCommand(const kstd::string& command) {
        // デバッグログ
        Serial.printf("[COMMS] Sending: %s\n", command.c_str());
        // char* として渡す (MsgPackのString型として適切に変換される)
        MsgPacketizer::send(Serial2, kIndexCommand, command.c_str());
        // 物理的にシリアルバッファを空にする
        Serial2.flush();
        // 送信処理を完了させる
        MsgPacketizer::update();
    }
    
    // スロットを指定して切り替えるヘルパー
    void selectSlot(int32_t slot) {
        kstd::string cmd = "REC_SLOT:" + kstd::to_string(slot);
        sendCommand(cmd);
    }

    // 名前指定でモーションをロードするヘルパー
    void loadMotion(const kstd::string& name) {
        kstd::string cmd = "LOAD_MOTION:" + name;
        sendCommand(cmd);
    }

    const RobotState& getState() const { return m_state; }
    const kstd::vector<ActionItem>& getActions() const { return m_actions; }

private:
    CommsManager() {
        // モーターリストの初期化
        m_state.motors.clear();
        m_state.motors.push_back({"HipL"});
        m_state.motors.push_back({"HipR"});
        m_state.motors.push_back({"HipTwist"});
        m_state.motors.push_back({"NeckL"});
        m_state.motors.push_back({"NeckR"});
        m_state.motors.push_back({"NeckTwist"});
        m_state.motors.push_back({"Eyelid"});
        m_state.motors.push_back({"LeftArm01"});
        m_state.motors.push_back({"LeftArm02"});
        m_state.motors.push_back({"LeftArm03"});
        m_state.motors.push_back({"LeftArm04"});
        m_state.motors.push_back({"LeftArm05"});
        m_state.motors.push_back({"LeftArm06"});
        m_state.motors.push_back({"LeftArm07"});
        m_state.motors.push_back({"RightArm01"});
        m_state.motors.push_back({"RightArm02"});
        m_state.motors.push_back({"RightArm03"});
        m_state.motors.push_back({"RightArm04"});
        m_state.motors.push_back({"RightArm05"});
        m_state.motors.push_back({"RightArm06"});
        m_state.motors.push_back({"RightArm07"});

        // アクションの初期化
        m_actions.clear();
        // --- SYSTEM ---
        m_actions.push_back({"CLEAR FAULTS", "CLEAR_FAULTS", Palette::Orange, -1, "SYS"});
        m_actions.push_back({"RESET CRASH", "CLEAR_CRASH_LATCH", Palette::Orangered, -1, "SYS"});
        m_actions.push_back({"PLAY START", "PLAY_START", Palette::Limegreen, -1, "SYS"});
        m_actions.push_back({"REC START", "REC_START", Palette::Red, -1, "SYS"});
        m_actions.push_back({"MOTION STOP", "MOTION_STOP", Palette::Lightgray, -1, "SYS"});
        m_actions.push_back({"MOTION SAVE", "MOTION_SAVE", Palette::Deepskyblue, -1, "SYS"});
        m_actions.push_back({"LOAD LAST", "LOAD_LAST", Palette::Yellow, -1, "SYS"});
        m_actions.push_back({"REBOOT", "REBOOT", Palette::Red, -1, "SYS"});

        // --- SLOTS ---
        m_actions.push_back({"SLOT 0", "REC_SLOT:0", Palette::Cyan, -1, "SLOTS"});
        m_actions.push_back({"SLOT 1", "REC_SLOT:1", Palette::Cyan, -1, "SLOTS"});
        m_actions.push_back({"SLOT 2", "REC_SLOT:2", Palette::Cyan, -1, "SLOTS"});
        m_actions.push_back({"SLOT 3", "REC_SLOT:3", Palette::Cyan, -1, "SLOTS"});
        m_actions.push_back({"SLOT 4", "REC_SLOT:4", Palette::Cyan, -1, "SLOTS"});

        // --- GLOBAL ---
        m_actions.push_back({"ALL ON", "TORQUE_ON", Palette::Cyan, -1, "ALL"});
        m_actions.push_back({"ALL OFF", "TORQUE_OFF", Palette::Lightgray, -1, "ALL"});
        m_actions.push_back({"ALL STIFFEN", "STIFFEN:ALL", Palette::White, -1, "ALL"});
        m_actions.push_back({"ALL PLAY", "REC_PRESET:ALL_PLAY", Palette::Deepskyblue, -1, "ALL"});
        m_actions.push_back({"ALL REC", "REC_PRESET:ALL_REC", Palette::Red, -1, "ALL"});
        m_actions.push_back({"ALL OFF", "REC_PRESET:ALL_OFF", Palette::Gray, -1, "ALL"});

        // --- L-ARM ---
        m_actions.push_back({"L-ARM ON", "TORQUE_ON:ARM_L", Palette::Cyan, 3, "L-ARM"});
        m_actions.push_back({"L-ARM STIFFEN", "STIFFEN:ARM_L", Palette::White, 3, "L-ARM"});
        m_actions.push_back({"L-ARM REC", "REC_PRESET:ARM_L_REC", Palette::Red, 3, "L-ARM"});
        m_actions.push_back({"L-ARM OFF", "TORQUE_OFF:ARM_L", Palette::Gray, 3, "L-ARM"});

        // --- R-ARM ---
        m_actions.push_back({"R-ARM ON", "TORQUE_ON:ARM_R", Palette::Cyan, 4, "R-ARM"});
        m_actions.push_back({"R-ARM STIFFEN", "STIFFEN:ARM_R", Palette::White, 4, "R-ARM"});
        m_actions.push_back({"R-ARM REC", "REC_PRESET:ARM_R_REC", Palette::Red, 4, "R-ARM"});
        m_actions.push_back({"R-ARM OFF", "TORQUE_OFF:ARM_R", Palette::Gray, 4, "R-ARM"});

        // --- ARMS ---
        m_actions.push_back({"ARMS REC", "REC_PRESET:ARMS_REC", Palette::Red, -1, "BODY"});

        // --- HIP / HEAD ---
        m_actions.push_back({"HIP ON", "TORQUE_ON:HIP", Palette::Cyan, 0, "BODY"});
        m_actions.push_back({"HIP STIFFEN", "STIFFEN:HIP", Palette::White, 0, "BODY"});
        m_actions.push_back({"HIP REC", "REC_PRESET:HIP_REC", Palette::Red, 0, "BODY"});
        m_actions.push_back({"HIP OFF", "TORQUE_OFF:HIP", Palette::Gray, 0, "BODY"});

        m_actions.push_back({"HEAD ON", "TORQUE_ON:HEAD", Palette::Cyan, 1, "BODY"});
        m_actions.push_back({"HEAD STIFFEN", "STIFFEN:HEAD", Palette::White, 1, "BODY"});
        m_actions.push_back({"HEAD REC", "REC_PRESET:HEAD_REC", Palette::Red, 1, "BODY"});
        m_actions.push_back({"HEAD OFF", "TORQUE_OFF:HEAD", Palette::Gray, 1, "BODY"});

        m_actions.push_back({"EYE ON", "TORQUE_ON:EYELID", Palette::Cyan, 2, "BODY"});
        m_actions.push_back({"EYE REC", "REC_PRESET:EYELID_REC", Palette::Red, 2, "BODY"});
        m_actions.push_back({"EYE OFF", "TORQUE_OFF:EYELID", Palette::Gray, 2, "BODY"});
    }
    
    ~CommsManager() = default;
    CommsManager(const CommsManager&) = delete;
    CommsManager& operator=(const CommsManager&) = delete;

    void setupSubscriptions() {
        // 1. モーターデータ (Index 10)
        MsgPacketizer::subscribe(Serial2, kIndexMotorTelemetry, [this](const std::vector<float>& packedData) {
            m_state.diagnostics.lastTelemetryMs = millis();
            m_state.diagnostics.msgCount++;

            // Payload layout (per motor):
            // [ angleRad, torqueFlag, errorStatus, shutdownStatus ]
            // - angleRad <= -900.0f is treated as "invalid / not updated"
            constexpr size_t valuesPerMotor = 4;
            constexpr float kInvalidAngleSentinel = -900.0f;
            size_t motorCount = packedData.size() / valuesPerMotor;
            if (motorCount > m_state.motors.size()) {
                m_state.diagnostics.errorCount++;
                m_state.diagnostics.lastError = "Motor data size mismatch";
                motorCount = m_state.motors.size();
            }

            for (size_t i = 0; i < motorCount; ++i) {
                size_t baseIdx = i * valuesPerMotor;
                float angle = packedData[baseIdx];
                float torque = packedData[baseIdx + 1];
                float error = packedData[baseIdx + 2];
                float shutdown = packedData[baseIdx + 3];
                
                if (angle > kInvalidAngleSentinel) {
                    m_state.motors[i].angleRadians = angle;
                    m_state.motors[i].isTorqueEnabled = (torque > 0.5f);
                    
                    if (error >= 0.0f) m_state.motors[i].errorStatus = static_cast<uint8_t>(error);
                    if (shutdown >= 0.0f) m_state.motors[i].shutdownStatus = static_cast<uint8_t>(shutdown);
                    
                    m_state.motors[i].updated = true;
                    m_state.motors[i].lastUpdateMs = millis();
                } else {
                    m_state.motors[i].updated = false;
                }
            }
        });

        // 2. システム健康状態 (Index 11)
        MsgPacketizer::subscribe(Serial2, kIndexSystemHealth, [this](const std::vector<int32_t>& healthData) {
            m_state.diagnostics.lastHealthMs = millis();
            m_state.diagnostics.msgCount++;

            if (healthData.size() < 7) {
                m_state.diagnostics.errorCount++;
                m_state.diagnostics.lastError = "Health data too short";
                return;
            }

            // Layout (minimum):
            // [0] crashLatch, [1] persistentArmState, [2..5] portFaults(4)
            m_state.crashLatch = healthData[0];
            m_state.persistentArmState = healthData[1];

            // ポート障害フラグの更新 (2..5)
            for (size_t i = 2; i < 6 && (i - 2) < m_state.portFaults.size(); ++i) {
                m_state.portFaults[i - 2] = healthData[i];
            }

            // Extended layout (requires size >= 13):
            // [7] engineRunning, [8..12] groupModes(5)
            if (healthData.size() >= 13) {
                m_state.engineRunning = healthData[7];
                for (int i = 0; i < 5; ++i) {
                    m_state.groupModes[i] = healthData[8 + i];
                }
            }
        });
    }

    RobotState m_state;
    kstd::vector<ActionItem> m_actions;
    
    // インデックス定義
    // See docs/CommsProtocol.md for payload definitions.
    static constexpr uint8_t kIndexMotorTelemetry = 10;
    static constexpr uint8_t kIndexSystemHealth = 11;
    static constexpr uint8_t kIndexCommand = 0;
};

} // namespace robot
