#pragma once

#include "M5Siv3D.h"
#include "SafeKiboSTL.hpp"
#include <MsgPacketizer.h>

namespace robot {

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
    
    const RobotState& getState() const { return m_state; }
    const kstd::vector<ActionItem>& getActions() const { return m_actions; }

private:
    CommsManager() {
        // モーターリストの初期化
        m_state.motors = {
            {"HipL"}, {"HipR"}, {"HipTwist"},
            {"NeckL"}, {"NeckR"}, {"NeckTwist"}, {"Eyelid"},
            {"LeftArm01"}, {"LeftArm02"}, {"LeftArm03"}, {"LeftArm04"}, {"LeftArm05"}, {"LeftArm06"}, {"LeftArm07"},
            {"RightArm01"}, {"RightArm02"}, {"RightArm03"}, {"RightArm04"}, {"RightArm05"}, {"RightArm06"}, {"RightArm07"}
        };

        // アクションの初期化
        m_actions = {
            {"TORQUE ON", "TORQUE_ON", Palette::Cyan},
            {"TORQUE OFF", "TORQUE_OFF", Palette::Yellow},
            {"HIP ON", "TORQUE_ON:HIP", Palette::Deepskyblue},
            {"HEAD ON", "TORQUE_ON:HEAD", Palette::Deepskyblue},
            {"L-ARM ON", "TORQUE_ON:ARM_L", Palette::Deepskyblue},
            {"R-ARM ON", "TORQUE_ON:ARM_R", Palette::Deepskyblue},
            {"ARMS ON", "TORQUE_ON:ARMS", Palette::Deepskyblue},
            {"CLEAR FAULTS", "CLEAR_FAULTS", Palette::Orange},
            {"RESET CRASH", "CLEAR_CRASH_LATCH", Palette::Orangered},
            {"REBOOT SYSTEM", "REBOOT", Palette::Red},
            {"ARMS REC", "REC_PRESET:ARMS_REC", Palette::Magenta},
            {"EYELID REC", "REC_PRESET:EYELID_REC", Palette::Purple},
            {"ALL PLAY", "REC_PRESET:ALL_PLAY", Palette::Green}
        };
    }
    
    ~CommsManager() = default;
    CommsManager(const CommsManager&) = delete;
    CommsManager& operator=(const CommsManager&) = delete;

    void setupSubscriptions() {
        // 1. モーターデータ (Index 10)
        MsgPacketizer::subscribe(Serial2, kIndexMotorTelemetry, [this](const std::vector<float>& packedData) {
            m_state.diagnostics.lastTelemetryMs = millis();
            m_state.diagnostics.msgCount++;

            const size_t valuesPerMotor = 4;
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
                
                if (angle > -900.0f) {
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

            m_state.crashLatch = healthData[0];
            m_state.persistentArmState = healthData[1];

            // ポート障害フラグの更新 (2..5)
            for (size_t i = 2; i < 6 && (i - 2) < m_state.portFaults.size(); ++i) {
                m_state.portFaults[i - 2] = healthData[i];
            }

            // 録画エンジンとグループモードのパース (7..12)
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
    static constexpr uint8_t kIndexMotorTelemetry = 10;
    static constexpr uint8_t kIndexSystemHealth = 11;
    static constexpr uint8_t kIndexCommand = 0;
};

} // namespace robot
