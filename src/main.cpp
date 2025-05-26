#include <M5Siv3D.h>

void Main() {
    // 基本的なオーディオアセットのテスト
    RegisterAudioAsset(String("TestBeep"), 1000.0f, 200);
    RegisterAudioAsset(String("HighBeep"), 2000.0f, 100);
    RegisterAudioAsset(String("LowBeep"), 500.0f, 300);
    
    // プリセットオーディオ（自動登録される）
    AudioAsset(String("Beep")).playOneShot();
    
    while (System::Update()) {
        if (Input::ButtonA.pressed()) {
            AudioAsset(String("TestBeep")).playOneShot();
        }
        
        if (Input::Touch.down()) {
            AudioAsset(String("Click")).playOneShot();
        }
    }
} 