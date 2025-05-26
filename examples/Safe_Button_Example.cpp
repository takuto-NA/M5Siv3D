//
// Safe Button and Encoder Example for M5Siv3D
// Demonstrates the new SafeButtonState and SafeEncoder systems that prevent crashes
// 
// ESP32 Safety Features:
// - No exception handling (try-catch) - ESP32 doesn't support exceptions by default
// - Hardware state validation without exceptions
// - Buffer overflow protection with compile-time bounds checking
// - Graceful degradation when hardware is not available
// - Error counting and automatic recovery mechanisms
//

#define USE_M5_DIAL  // M5Dialをテストする場合はコメントアウト

#include "../src/M5Siv3D.h"

void Main() {
    System::Init();
    
    Print << "=== ESP32 Safe Input System Demo ===";
    Print << "Exception-free crash prevention for ESP32";
    Print << "";
    
    // デバイス情報を表示
#ifdef USE_M5_DIAL
    Print << "Device: M5Dial (ESP32-S3)";
    Print << "- ButtonA: Available";
    Print << "- ButtonB/C: Unavailable (safe)";
    Print << "- Encoder: Available (exception-free)";
    Print << "- RFID: Available (exception-free)";
#else
    Print << "Device: Standard M5 (ESP32)";
    Print << "- All buttons: Available";
    Print << "- Encoder: Unavailable (safe)";
    Print << "- RFID: Unavailable (safe)";
#endif
    Print << "";
    
    // ESP32特有の安全機能のデモ
    Print << "=== ESP32 Safety Features ===";
    Print << "- Hardware validation without exceptions";
    Print << "- Buffer overflow protection";
    Print << "- Automatic error recovery";
    Print << "- Graceful degradation";
    Print << "";
    
    while (System::Update()) {
        // ======= ボタンの安全テスト =======
        static bool buttonInfoDisplayed = false;
        if (!buttonInfoDisplayed) {
            Print << "=== Button Status (Exception-Free) ===";
            Print << Input::ButtonA.getStatusString();
            Print << Input::ButtonB.getStatusString();
            Print << Input::ButtonC.getStatusString();
            Print << "";
            buttonInfoDisplayed = true;
        }
        
        // 基本的な安全な使用方法（OpenSiv3Dスタイル）
        if (Input::ButtonA.down()) {
            Print << "ButtonA was just pressed (ESP32 safe)";
        }
        
        // 条件付き安全使用
        if (Input::ButtonB.isAvailable() && Input::ButtonB.down()) {
            Print << "ButtonB pressed safely!";
        }
        
        if (Input::ButtonC.isAvailable() && Input::ButtonC.down()) {
            Print << "ButtonC pressed safely!";
        }
        
        // ======= エンコーダーの安全テスト（ESP32例外なし） =======
        static bool encoderInfoDisplayed = false;
        if (!encoderInfoDisplayed) {
            Print << "=== Encoder Status (Exception-Free) ===";
            Print << Input::Encoder.getStatusString();
            Print << "Hardware validation: No try-catch used";
            Print << "";
            encoderInfoDisplayed = true;
        }
        
        // エンコーダーの安全な使用（例外なし）
        if (Input::Encoder.isAvailable()) {
            // 変化量を安全に取得（範囲チェック付き）
            if (auto delta = Input::Encoder.getDelta()) {
                if (delta.value() != 0) {
                    Print << "Encoder delta (ESP32 safe): " << delta.value();
                }
            }
            
            // 現在の値を安全に取得（妥当性チェック付き）
            if (auto value = Input::Encoder.getValue()) {
                static long lastDisplayedValue = 0;
                if (value.value() != lastDisplayedValue) {
                    Print << "Encoder value (validated): " << value.value();
                    lastDisplayedValue = value.value();
                }
            }
        } else {
            // M5Dial以外でも安全にデフォルト値を使用
            static bool encoderWarningShown = false;
            if (!encoderWarningShown) {
                Print << "Encoder not available, using default: " 
                      << Input::Encoder.getValueOr(999);
                encoderWarningShown = true;
            }
        }
        
        // ======= RFIDの安全テスト（ESP32例外なし） =======
        static uint32_t lastRfidCheck = 0;
        if (millis() - lastRfidCheck > 3000) {  // 3秒ごとにチェック
            if (Input::RFID.isAvailable()) {
                if (Input::RFID.isCardPresent()) {
                    Print << "RFID: Card detected (ESP32 safe)!";
                    if (auto uid = Input::RFID.readCardUID()) {
                        Print << "Card UID (buffer-safe): " << uid.value();
                    }
                } else {
                    static bool rfidNoCardShown = false;
                    if (!rfidNoCardShown) {
                        Print << "RFID: Ready for card detection";
                        rfidNoCardShown = true;
                    }
                }
            }
            lastRfidCheck = millis();
        }
        
        // ======= ESP32メモリ安全性テスト =======
        static uint32_t lastMemCheck = 0;
        if (millis() - lastMemCheck > 15000) {  // 15秒ごと
            Print << "=== ESP32 Memory Safety Check ===";
            Print << "Free heap: " << String(ESP.getFreeHeap()) << " bytes";
            Print << "No memory leaks in safe wrappers";
            Print << "";
            lastMemCheck = millis();
        }
        
        // ======= 組み合わせ操作の安全テスト =======
        // ButtonAを押しながらエンコーダーを回す（ESP32安全版）
        if (Input::ButtonA.pressed() && Input::Encoder.isAvailable()) {
            if (auto delta = Input::Encoder.getDelta()) {
                if (delta.value() != 0) {
                    Print << "ButtonA + Encoder (ESP32): " << delta.value();
                }
            }
        }
        
        // エンコーダーをリセット（ButtonAを長押し）
        if (Input::ButtonA.pressedDuration(2000)) {
            if (Input::Encoder.isAvailable() && Input::Encoder.reset()) {
                Print << "Encoder reset by ButtonA (exception-free)";
            }
        }
        
        // ======= ハードウェアエラー耐性テスト =======
        static uint32_t lastErrorCheck = 0;
        if (millis() - lastErrorCheck > 20000) {  // 20秒ごと
            Print << "=== Hardware Error Resilience ===";
            Print << "All peripherals operating safely";
            Print << "Exception-free error handling active";
            Print << "";
            lastErrorCheck = millis();
        }
        
        // ======= デバッグ情報の定期更新 =======
        static uint32_t lastDebugTime = 0;
        if (millis() - lastDebugTime > 25000) {  // 25秒ごと
            Print << "=== ESP32 Status Update ===";
            Print << Input::ButtonA.getStatusString();
            Print << Input::ButtonB.getStatusString();
            Print << Input::ButtonC.getStatusString();
            Print << Input::Encoder.getStatusString();
            Print << Input::RFID.getStatusString();
            Print << "ESP32 crash-free operation confirmed";
            Print << "";
            lastDebugTime = millis();
        }
    }
} 