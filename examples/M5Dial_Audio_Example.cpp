//
// M5Dial Audio Example for M5Siv3D
// Demonstrates OpenSiv3D-style audio functionality using M5Dial's onboard buzzer
// Shows proper AudioAsset usage following OpenSiv3D conventions
//

// M5Dialを使用する場合はこのマクロを定義
#define USE_M5_DIAL

#include "../src/M5Siv3D.h"

// グローバル変数
int currentMode = 0;  // 0: Device Info, 1: Basic sounds, 2: Musical notes, 3: Interactive
float currentFrequency = 440.0f;  // A4
bool isPlaying = false;

void drawHeader() {
    // ヘッダー背景
    Rect(0, 0, System::Width(), 25).draw(Palette::Darkblue);
    
    // タイトル
    Font titleFont;
    titleFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Center)
            .setVerticalAlign(Font::VerticalAlign::Center);
    
    String title = "M5Siv3D Audio Demo - Mode " + String(currentMode + 1) + "/4";
    titleFont(title, Font::Pos(System::Width()/2, 12), Palette::White);
}

void drawDeviceInfo() {
    // 背景をクリア
    Rect(0, 30, System::Width(), 210).draw(Palette::Black);
    
    Font font;
    font.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);
    
    font("Device Audio Info", Font::Pos(System::Width()/2, 60), Palette::White);
    
    // デバイス情報を表示
    Font infoFont;
    infoFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Left);
    
    int yPos = 90;
    const int lineHeight = 15;
    
#ifdef USE_M5_DIAL
    infoFont("Hardware: M5Dial Buzzer", Font::Pos(10, yPos), Palette::Yellow);
    yPos += lineHeight;
    infoFont("Type: Piezo Buzzer", Font::Pos(10, yPos), Palette::White);
    yPos += lineHeight;
    infoFont("Volume Control: No (Fixed)", Font::Pos(10, yPos), Palette::Orange);
    yPos += lineHeight;
    infoFont("Frequency: 100Hz - 5kHz", Font::Pos(10, yPos), Palette::White);
    yPos += lineHeight;
    infoFont("Duration: Max 500ms", Font::Pos(10, yPos), Palette::White);
    yPos += lineHeight;
    infoFont("Optimized: Short clear tones", Font::Pos(10, yPos), Palette::Green);
#else
    infoFont("Hardware: M5Stack Speaker", Font::Pos(10, yPos), Palette::Yellow);
    yPos += lineHeight;
    infoFont("Type: I2S Speaker", Font::Pos(10, yPos), Palette::White);
    yPos += lineHeight;
    infoFont("Polyphony: Yes (Up to 8 tones)", Font::Pos(10, yPos), Palette::Green);
    yPos += lineHeight;
    infoFont("Frequency: 20Hz - 20kHz", Font::Pos(10, yPos), Palette::White);
    yPos += lineHeight;
    infoFont("Duration: Max 60 seconds", Font::Pos(10, yPos), Palette::White);
    yPos += lineHeight;
    infoFont("Features: Waveforms, Chords", Font::Pos(10, yPos), Palette::Green);
#endif
    
    yPos += lineHeight + 5;
    
    // OpenSiv3D風のアセット情報表示
    infoFont("AudioAsset Status:", Font::Pos(10, yPos), Palette::Cyan);
    yPos += lineHeight;
    
    // 登録されているアセットの確認
    if (Audio::AudioAsset::IsReady(String("Beep"))) {
        infoFont("✓ Beep Asset Ready", Font::Pos(10, yPos), Palette::Green);
    } else {
        infoFont("✗ Beep Asset Not Ready", Font::Pos(10, yPos), Palette::Red);
    }
    yPos += lineHeight;
    
    if (Audio::AudioAsset::IsReady(String("Click"))) {
        infoFont("✓ Click Asset Ready", Font::Pos(10, yPos), Palette::Green);
    } else {
        infoFont("✗ Click Asset Not Ready", Font::Pos(10, yPos), Palette::Red);
    }
    
    // 操作説明
    Font helpFont;
    helpFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    helpFont("Button A: Next mode", Font::Pos(System::Width()/2, 205), Palette::Gray);
    helpFont("Encoder: Toggle mute/unmute", Font::Pos(System::Width()/2, 220), Palette::Gray);
}

void drawBasicSounds() {
    // 背景をクリア
    Rect(0, 30, System::Width(), 210).draw(Palette::Black);
    
    Font font;
    font.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);
    
    font("Basic Sound Effects", Font::Pos(System::Width()/2, 60), Palette::White);
    
    // 効果音ボタンの配置
    const int buttonWidth = 80;
    const int buttonHeight = 30;
    const int spacing = 10;
    const int startY = 90;
    
    // Beep ボタン
    Rect beepBtn(20, startY, buttonWidth, buttonHeight);
    beepBtn.drawRound(5, Palette::Blue);
    beepBtn.drawRoundFrame(5, Palette::White);
    
    Font btnFont;
    btnFont.setSize(1)
          .setHorizontalAlign(Font::HorizontalAlign::Center)
          .setVerticalAlign(Font::VerticalAlign::Center);
    btnFont("BEEP", Font::Pos(beepBtn.m_x + buttonWidth/2, beepBtn.m_y + buttonHeight/2), Palette::White);
    
    // Click ボタン
    Rect clickBtn(120, startY, buttonWidth, buttonHeight);
    clickBtn.drawRound(5, Palette::Green);
    clickBtn.drawRoundFrame(5, Palette::White);
    btnFont("CLICK", Font::Pos(clickBtn.m_x + buttonWidth/2, clickBtn.m_y + buttonHeight/2), Palette::White);
    
    // Success ボタン
    Rect successBtn(20, startY + buttonHeight + spacing, buttonWidth, buttonHeight);
    successBtn.drawRound(5, Palette::Orange);
    successBtn.drawRoundFrame(5, Palette::White);
    btnFont("SUCCESS", Font::Pos(successBtn.m_x + buttonWidth/2, successBtn.m_y + buttonHeight/2), Palette::White);
    
    // Error ボタン
    Rect errorBtn(120, startY + buttonHeight + spacing, buttonWidth, buttonHeight);
    errorBtn.drawRound(5, Palette::Red);
    errorBtn.drawRoundFrame(5, Palette::White);
    btnFont("ERROR", Font::Pos(errorBtn.m_x + buttonWidth/2, errorBtn.m_y + buttonHeight/2), Palette::White);
    
    // ボタンのタッチ処理（OpenSiv3D風のAudioAsset使用）
    if (beepBtn.touched()) {
        AudioAsset(String("Beep")).playOneShot();
    }
    if (clickBtn.touched()) {
        AudioAsset(String("Click")).playOneShot();
    }
    if (successBtn.touched()) {
        AudioAsset(String("Success")).playOneShot();
    }
    if (errorBtn.touched()) {
        AudioAsset(String("Error")).playOneShot();
    }
    
    // 現在の音声状態表示（M5Dialではミュート状態のみ）
    Font volumeFont;
    volumeFont.setSize(1)
             .setHorizontalAlign(Font::HorizontalAlign::Center);
    bool isMuted = Audio::GlobalAudio::IsMuted();
    String statusText = isMuted ? "Audio: MUTED" : "Audio: ON";
    Color statusColor = isMuted ? Palette::Red : Palette::Green;
    volumeFont(statusText, Font::Pos(System::Width()/2, 180), statusColor);
    
    // 操作説明
    Font helpFont;
    helpFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    helpFont("Touch buttons to play sounds", Font::Pos(System::Width()/2, 210), Palette::Gray);
    helpFont("Encoder: Toggle mute/unmute", Font::Pos(System::Width()/2, 225), Palette::Gray);
}

void drawMusicalNotes() {
    // 背景をクリア
    Rect(0, 30, System::Width(), 210).draw(Palette::Black);
    
    Font font;
    font.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);
    
    font("Musical Notes", Font::Pos(System::Width()/2, 60), Palette::White);
    
    // 音階の配置（円形）
    const float centerX = System::Width() / 2.0f;
    const float centerY = 140.0f;
    const float radius = 60.0f;
    
    // 音階の定義
    const float noteFreqs[] = {
        Audio::Note::C4, Audio::Note::D4, Audio::Note::E4, Audio::Note::F4,
        Audio::Note::G4, Audio::Note::A4, Audio::Note::B4, Audio::Note::C5
    };
    const String noteNames[] = {"C", "D", "E", "F", "G", "A", "B", "C5"};
    const int noteCount = 8;
    
    for (int i = 0; i < noteCount; i++) {
        float angle = (i * 2.0f * Math::Pi) / noteCount - Math::Pi / 2.0f;
        float x = centerX + Math::cos(angle) * radius;
        float y = centerY + Math::sin(angle) * radius;
        
        Circle noteBtn(x, y, 15);
        
        // 現在再生中の音符をハイライト
        Color btnColor = (Math::abs(currentFrequency - noteFreqs[i]) < 1.0f && isPlaying) ? 
                        Palette::Yellow : Palette::Cyan;
        
        noteBtn.draw(btnColor);
        noteBtn.drawFrame(Palette::White);
        
        // 音符名を描画
        Font noteFont;
        noteFont.setSize(1)
               .setHorizontalAlign(Font::HorizontalAlign::Center)
               .setVerticalAlign(Font::VerticalAlign::Center);
        noteFont(noteNames[i], Font::Pos(x, y), Palette::Black);
        
        // タッチ処理（OpenSiv3D風の動的アセット作成）
        if (noteBtn.touched()) {
            currentFrequency = noteFreqs[i];
            
            // 動的にアセットを作成して再生
            String noteName = String("Note_") + noteNames[i];
            Audio::AudioAsset::Register(noteName, noteFreqs[i], 500);
            AudioAsset(noteName).playOneShot();
            
            isPlaying = true;
        }
    }
    
    // 現在の周波数を表示
    Font freqFont;
    freqFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    freqFont("Frequency: " + String(currentFrequency, 1) + " Hz", 
            Font::Pos(System::Width()/2, 210), Palette::White);
    
    // 操作説明
    Font helpFont;
    helpFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    helpFont("Touch notes to play", Font::Pos(System::Width()/2, 225), Palette::Gray);
}

void drawInteractive() {
    // 背景をクリア
    Rect(0, 30, System::Width(), 210).draw(Palette::Black);
    
    Font font;
    font.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);
    
    font("Interactive Mode", Font::Pos(System::Width()/2, 60), Palette::White);
    
    // エンコーダー値を取得
    long encoderValue = Input::Encoder.getValueOr(0);
    
    // エンコーダー値を周波数にマッピング（200Hz - 2000Hz）
    float mappedFreq = 200.0f + (encoderValue % 1800);
    if (mappedFreq < 200.0f) mappedFreq = 200.0f;
    if (mappedFreq > 2000.0f) mappedFreq = 2000.0f;
    
    // 周波数表示
    Font freqFont;
    freqFont.setSize(2)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    freqFont("Frequency: " + String(mappedFreq, 0) + " Hz", 
            Font::Pos(System::Width()/2, 100), Palette::Cyan);
    
    // タッチエリア
    Circle touchArea(System::Width()/2, 150, 40);
    Color areaColor = Input::Touch.pressed() ? Palette::Yellow : Palette::Blue;
    touchArea.draw(areaColor);
    touchArea.drawFrame(Palette::White);
    
    Font touchFont;
    touchFont.setSize(1)
            .setHorizontalAlign(Font::HorizontalAlign::Center)
            .setVerticalAlign(Font::VerticalAlign::Center);
    touchFont("TOUCH", Font::Pos(System::Width()/2, 150), Palette::White);
    
    // タッチで音を再生（OpenSiv3D風の動的アセット）
    if (touchArea.touched()) {
        Audio::AudioAsset::Register(String("Interactive"), mappedFreq, 100);
        AudioAsset(String("Interactive")).playOneShot();
    }
    
    // 操作説明
    Font helpFont;
    helpFont.setSize(1)
           .setHorizontalAlign(Font::HorizontalAlign::Center);
    helpFont("Encoder: Change frequency", Font::Pos(System::Width()/2, 200), Palette::Gray);
    helpFont("Touch circle: Play sound", Font::Pos(System::Width()/2, 215), Palette::Gray);
}

void Main() {
    // OpenSiv3D風の初期化（アセットは自動登録される）
    
    while (System::Update()) {
        // ヘッダーを描画
        drawHeader();
        
        // エンコーダーでボリューム調整（M5Dialではミュート/アンミュート切り替え）
        if (auto delta = Input::Encoder.getDelta()) {
            if (delta.value() != 0) {
                // M5Dialブザーはボリューム制御ができないため、ミュート切り替えとして動作
                bool currentMuted = Audio::GlobalAudio::IsMuted();
                Audio::GlobalAudio::SetMute(!currentMuted);
                
                // フィードバック音
                if (!Audio::GlobalAudio::IsMuted()) {
                    AudioAsset(String("Click")).playOneShot();
                }
            }
        }
        
        // ボタンAでモード切り替え
        if (Input::ButtonA.pressed()) {
            currentMode = (currentMode + 1) % 4;  // 4モードに変更
            AudioAsset(String("Click")).playOneShot();  // OpenSiv3D風のアセット使用
        }
        
        // モードに応じた描画
        switch (currentMode) {
            case 0:
                drawDeviceInfo();
                break;
            case 1:
                drawBasicSounds();
                break;
            case 2:
                drawMusicalNotes();
                break;
            case 3:
                drawInteractive();
                break;
        }
        
        // 再生状態の更新
        if (isPlaying) {
            // 簡単な再生状態管理
            static uint32_t lastPlayTime = 0;
            if (millis() - lastPlayTime > 500) {
                isPlaying = false;
            }
        }
    }
} 