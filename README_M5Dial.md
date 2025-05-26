# M5Siv3D with M5Dial Support

M5Siv3DがM5Dialに対応しました！エンコーダーやRFID機能、そして**オンボードブザーによるサウンド機能**を含む、M5Dialのすべての機能をSiv3D風のAPIで使用できます。

## 🌟 新機能

### M5Dial固有の機能
- **エンコーダー**: 回転入力による直感的な操作
- **RFID**: NFCカードの読み書き
- **🔊 オーディオ**: オンボードブザーによるOpenSiv3D風サウンド機能
- **完全な後方互換性**: 既存のM5Siv3Dコードがそのまま動作

### 統一API設計
- M5UnifiedとM5Dialの両方に対応
- デバイス固有機能は条件コンパイルで自動切り替え
- シンプルなマクロによる設定

## 🛠 セットアップ

### 必要なライブラリ
```cpp
// M5Dialを使用する場合
#define USE_M5_DIAL
#include "M5Siv3D.h"

// 通常のM5Unified使用時（既存コード）
#include "M5Siv3D.h"
```

### プロジェクト構成
```
your_project/
├── src/
│   └── M5Siv3D.h          # メインライブラリ
├── examples/
│   ├── M5Dial_Example.cpp      # M5Dial基本使用例
│   └── M5Dial_Audio_Example.cpp # M5Dialオーディオ使用例
└── M5Dial.h               # M5Dialライブラリ（別途必要）
```

## 📖 基本的な使い方

### 🔊 オーディオ機能（NEW!）

M5Dialのオンボードブザーを使って、OpenSiv3D風のサウンド機能を実現できます。

#### OpenSiv3D風のAudioAsset使用

```cpp
#define USE_M5_DIAL
#include "M5Siv3D.h"

void Main() {
    // OpenSiv3D Style: AudioAssetの登録
    AudioAsset::Register(U"Beep", 1000.0f, 200);
    AudioAsset::Register(U"Click", 2000.0f, 50);
    AudioAsset::Register(U"Success", 800.0f, 300);
    AudioAsset::Register(U"Error", 200.0f, 500);
    
    // 音階アセットの登録
    AudioAsset::Register(U"C4", Audio::Note::C4, 500);
    AudioAsset::Register(U"D4", Audio::Note::D4, 500);
    
    while (System::Update()) {
        // OpenSiv3D Style: AudioAssetを使った再生
        if (Input::ButtonA.pressed()) {
            AudioAsset(U"Click").playOneShot();
        }
        
        // 音階の再生
        if (Input::Touch.pressed()) {
            AudioAsset(U"C4").playOneShot();
        }
    }
}
```

#### 音階定数とアセット登録

```cpp
// OpenSiv3D Style: 音階アセットの一括登録
void RegisterNoteAssets() {
    // オクターブ4の音階（基準）
    AudioAsset::Register(U"C4", Audio::Note::C4, 500);   // ド
    AudioAsset::Register(U"D4", Audio::Note::D4, 500);   // レ
    AudioAsset::Register(U"E4", Audio::Note::E4, 500);   // ミ
    AudioAsset::Register(U"F4", Audio::Note::F4, 500);   // ファ
    AudioAsset::Register(U"G4", Audio::Note::G4, 500);   // ソ
    AudioAsset::Register(U"A4", Audio::Note::A4, 500);   // ラ（440Hz）
    AudioAsset::Register(U"B4", Audio::Note::B4, 500);   // シ

    // 他のオクターブ
    AudioAsset::Register(U"C3", Audio::Note::C3, 500);   // 低いド
    AudioAsset::Register(U"C5", Audio::Note::C5, 500);   // 高いド

    // シャープ音
    AudioAsset::Register(U"CS4", Audio::Note::CS4, 500); // ド#
    AudioAsset::Register(U"FS4", Audio::Note::FS4, 500); // ファ#
}

void Main() {
    RegisterNoteAssets();
    
    while (System::Update()) {
        // OpenSiv3D Style: 登録したアセットを使用
        if (Input::ButtonA.pressed()) {
            AudioAsset(U"C4").playOneShot();
        }
    }
}
```

#### OpenSiv3D風のメロディー再生

```cpp
void Main() {
    // OpenSiv3D Style: プリセットメロディーは自動登録済み
    // MelodyAsset(U"Scale"), MelodyAsset(U"Startup"), MelodyAsset(U"Shutdown")
    
    // プリセットメロディーの再生
    MelodyAsset(U"Scale").playOneShot();
    MelodyAsset(U"Startup").playOneShot();
    MelodyAsset(U"Shutdown").playOneShot();
    
    // カスタムメロディー（きらきら星）をアセットとして登録
    std::vector<Audio::MelodyNote> twinkle = {
        {Audio::Note::C4, 300}, {Audio::Note::C4, 300},
        {Audio::Note::G4, 300}, {Audio::Note::G4, 300},
        {Audio::Note::A4, 300}, {Audio::Note::A4, 300},
        {Audio::Note::G4, 600},
        {Audio::Note::F4, 300}, {Audio::Note::F4, 300},
        {Audio::Note::E4, 300}, {Audio::Note::E4, 300},
        {Audio::Note::D4, 300}, {Audio::Note::D4, 300},
        {Audio::Note::C4, 600}
    };
    
    MelodyAsset::Register(U"Twinkle", twinkle);
    MelodyAsset(U"Twinkle").playOneShot();
    
    while (System::Update()) {
        // 全ての音を停止
        if (Input::ButtonA.pressed()) {
            GlobalAudio::StopAll();
        }
    }
}
```

#### OpenSiv3D風のサウンド制御

```cpp
void Main() {
    // OpenSiv3D Style: グローバルオーディオ制御
    GlobalAudio::SetVolume(0.7f);  // マスターボリューム設定（0.0 - 1.0）
    
    // ミュート制御
    GlobalAudio::SetMute(true);   // ミュート
    GlobalAudio::SetMute(false);  // ミュート解除
    
    // カスタムサウンドをアセットとして登録
    AudioAsset::Register(U"CustomSound", 800.0f, 500);
    
    while (System::Update()) {
        // OpenSiv3D Style: アセットを使った再生
        if (Input::ButtonA.pressed()) {
            AudioAsset(U"CustomSound").playOneShot(0.8f);  // ボリューム指定
        }
        
        // エンコーダーでボリューム調整
        if (auto delta = Input::Encoder.getDelta()) {
            float currentVol = GlobalAudio::GetVolume();
            float newVol = Math::clamp(currentVol + delta.value() * 0.05f, 0.0f, 1.0f);
            GlobalAudio::SetVolume(newVol);
        }
        
        // 現在のボリューム表示
        Print << "Volume: " << static_cast<int>(GlobalAudio::GetVolume() * 100) << "%";
    }
}
```

#### OpenSiv3D風のインタラクティブサウンド

```cpp
void Main() {
    while (System::Update()) {
        // エンコーダー値を周波数にマッピング
        long encoderValue = Input::Encoder.getValueOr(0);
        float frequency = 200.0f + (encoderValue % 1800);  // 200Hz - 2000Hz
        
        // タッチで音を再生（OpenSiv3D風の動的アセット作成）
        if (Input::Touch.pressed()) {
            AudioAsset::Register(U"Interactive", frequency, 100);
            AudioAsset(U"Interactive").playOneShot();
        }
        
        // 周波数を表示
        Print << "Frequency: " << frequency << " Hz";
    }
}
```

### エンコーダー機能

```cpp
#define USE_M5_DIAL
#include "M5Siv3D.h"

void Main() {
    // M5Dialの初期化（エンコーダー有効）
    auto cfg = M5.config();
    M5RealUnified::begin(cfg, true, false);  // encoder=true, rfid=false
    
    long lastValue = 0;
    
    while (System::Update()) {
        // エンコーダー値の取得
        long currentValue = Input::Encoder.getValue();
        
        // 値が変化した場合
        if (currentValue != lastValue) {
            lastValue = currentValue;
            
            // 値を画面に表示
            Print << "Encoder: " << currentValue;
            
            // 変化量も取得可能
            long delta = Input::Encoder.getDelta();
            Print << "Delta: " << delta;
            
            // 回転に合わせて音を再生
            PlayClick();
        }
        
        // ボタンAでリセット
        if (Input::ButtonA.pressed()) {
            Input::Encoder.reset();
            PlayBeep();
        }
    }
}
```

### RFID機能

```cpp
#define USE_M5_DIAL
#include "M5Siv3D.h"

void Main() {
    // M5Dialの初期化（RFID有効）
    auto cfg = M5.config();
    M5RealUnified::begin(cfg, false, true);  // encoder=false, rfid=true
    
    while (System::Update()) {
        // カードの検出
        if (Input::RFID.isCardPresent()) {
            Print << "Card detected!";
            PlaySuccess();  // カード検出音
            
            // UIDの読み取り
            String uid = Input::RFID.readCardUID();
            if (uid.length() > 0) {
                Print << "UID: " << uid;
            }
            
            // カードデータの読み取り例
            uint8_t buffer[16];
            if (Input::RFID.readCardData(4, buffer, 16)) {
                Print << "Data read successfully";
            }
        } else {
            Print << "No card detected";
        }
    }
}
```

### 複合機能の例

```cpp
#define USE_M5_DIAL
#include "M5Siv3D.h"

void Main() {
    // 全機能を有効化
    auto cfg = M5.config();
    M5RealUnified::begin(cfg, true, true);  // encoder=true, rfid=true
    
    // 起動音を再生
    PlayStartup();
    
    int mode = 0;
    
    while (System::Update()) {
        // エンコーダーでモード切り替え
        if (auto delta = Input::Encoder.getDelta()) {
            if (delta.value() != 0) {
                mode = (mode + delta.value()) % 3;
                PlayClick();
            }
        }
        
        // モードに応じた処理
        switch (mode) {
            case 0:
                Print << "Mode: Audio Test";
                if (Input::ButtonA.pressed()) {
                    PlayNote(Audio::Note::C4 + (rand() % 12) * 50, 200);
                }
                break;
                
            case 1:
                Print << "Mode: RFID Reader";
                if (Input::RFID.isCardPresent()) {
                    PlayBeep();
                }
                break;
                
            case 2:
                Print << "Mode: Interactive Sound";
                long encoderValue = Input::Encoder.getValueOr(0);
                float freq = 200.0f + (encoderValue % 1000);
                if (Input::Touch.pressed()) {
                    PlaySound(freq, 100);
                }
                break;
        }
        
        // ボタンAでメロディー再生
        if (Input::ButtonA.pressedDuration(1000)) {
            PlayScale();
        }
    }
}
```

## 🎵 オーディオ機能の詳細

### サポートする機能
- **基本音再生**: 指定周波数での音の再生
- **音階定数**: C3-C5の音階定数を提供
- **効果音**: ビープ、クリック、成功、エラー音
- **メロディー**: 複数音符の連続再生
- **ボリューム制御**: マスターボリューム、ミュート機能
- **再生状態管理**: 音の再生状態の確認

### 音階定数一覧
```cpp
// オクターブ3（低音）
Audio::Note::C3, D3, E3, F3, G3, A3, B3

// オクターブ4（基準音）
Audio::Note::C4, D4, E4, F4, G4, A4, B4

// オクターブ5（高音）
Audio::Note::C5, D5, E5, F5, G5, A5, B5

// シャープ音
Audio::Note::CS4, DS4, FS4, GS4, AS4  // C#, D#, F#, G#, A#
```

### 効果音の種類
```cpp
PlayBeep();     // 1000Hz, 200ms - 一般的なビープ音
PlayClick();    // 2000Hz, 50ms  - UI操作音
PlaySuccess();  // 800Hz, 300ms  - 成功通知音
PlayError();    // 200Hz, 500ms  - エラー通知音
```

## 📁 サンプルコード

完全な動作例は以下のファイルを参照してください：

- `examples/M5Dial_Example.cpp` - エンコーダーとRFIDの基本使用例
- `examples/M5Dial_Audio_Example.cpp` - **オーディオ機能の完全デモ**

オーディオサンプルでは以下の機能を実装しています：

- **基本効果音モード**: ボタンタッチで各種効果音を再生
- **音階モード**: 円形に配置された音符ボタンで音階を演奏
- **メロディーモード**: プリセットメロディーとカスタムメロディーの再生
- **インタラクティブモード**: エンコーダーで周波数を変更、タッチで音を再生
- **ボリューム制御**: エンコーダーでリアルタイムボリューム調整

## 🔧 デバイス間の違いと最適化

### M5Dialと他のM5Stackデバイスの音響機能比較

| 機能 | M5Dial | 他のM5Stackデバイス |
|------|--------|-------------------|
| **ハードウェア** | 圧電ブザー | I2Sスピーカー |
| **周波数範囲** | 50Hz - 10kHz | 20Hz - 20kHz |
| **最大再生時間** | 10秒（推奨2秒以下） | 60秒 |
| **和音対応** | ❌ 単音のみ | ✅ 最大8音同時 |
| **波形選択** | ❌ 矩形波のみ | ✅ 複数波形対応 |
| **音質** | シンプルなビープ音 | 高品質オーディオ |

### 自動最適化機能

M5Siv3Dは自動的にデバイスを検出し、最適な音響設定を適用します：

```cpp
void Main() {
    // デバイス情報を取得
    Print << GetAudioDeviceInfo();  // "Audio: Buzzer" または "Audio: Speaker Polyphony"
    
    // 機能チェック
    if (SupportsPolyphony()) {
        Print << "和音機能が利用可能です";
        
        // M5Unified固有の和音機能
        PlayMajorChord(Audio::Note::C4);  // Cメジャーコード
        PlayMinorChord(Audio::Note::A4);  // Aマイナーコード
        
        // カスタム和音
        std::vector<float> chord = {440.0f, 554.37f, 659.25f};  // A-C#-E
        PlayChord(chord, 1000);
    } else {
        Print << "単音のみ対応（M5Dial）";
        PlayNote(Audio::Note::C4);  // 単音再生
    }
    
    if (HasAdvancedSpeaker()) {
        Print << "高度なスピーカー機能が利用可能";
    } else {
        Print << "基本的なブザー機能";
    }
}
```

### デバイス固有の最適化

#### M5Dial最適化
- **短い音の推奨**: 2秒以下の音が最適
- **周波数制限**: 50Hz-10kHzに自動制限
- **安定性重視**: エラー処理を強化

#### M5Unified最適化
- **長時間再生**: 最大60秒まで対応
- **広帯域**: 20Hz-20kHzの全範囲
- **高機能**: 和音、波形選択、高音質

## 🔍 トラブルシューティング

### コンパイルエラー
```cpp
// エラー: USE_M5_DIAL が定義されていない
#define USE_M5_DIAL  // この行を追加
#include "M5Siv3D.h"
```

### 音が出ない場合
```cpp
// ボリュームが0になっていないかチェック
SetMasterVolume(0.5f);  // 50%に設定

// ミュートされていないかチェック
SetMuted(false);

// M5Dialが適切に初期化されているかチェック
M5RealUnified::begin(cfg, true, true);  // エンコーダーとRFIDを有効化

// デバイス情報を確認
Print << GetAudioDeviceInfo();
```

### M5Dialで音が途切れる場合
```cpp
// M5Dialでは短い音を推奨
PlaySound(440.0f, 500);   // ✅ 0.5秒 - 推奨
PlaySound(440.0f, 5000);  // ⚠️ 5秒 - 自動的に2秒に制限される

// メロディーでも短い音符を使用
Audio::MelodyNote shortNote(Audio::Note::C4, 300);  // ✅ 0.3秒
Audio::MelodyNote longNote(Audio::Note::C4, 3000);  // ⚠️ 自動的に1秒に制限
```

### 機能が動作しない
```cpp
// M5Dialの機能を使用する前に適切に初期化
M5RealUnified::begin(cfg, true, true);  // エンコーダーとRFIDを有効化

// 機能の可用性をチェック
if (SupportsPolyphony()) {
    // 和音機能を使用
    PlayChord({440.0f, 554.37f, 659.25f});
} else {
    // 単音のみ使用
    PlayNote(440.0f);
}
```

### 既存コードの移植
```cpp
// 変更前（M5Unified直接使用）
M5.begin(cfg);
M5.update();
M5.Speaker.tone(440, 1000);

// 変更後（M5RealUnified使用 - 自動最適化）
M5RealUnified::begin(cfg);
M5RealUnified::update();
PlaySound(440.0f, 1000);  // デバイスに応じて自動最適化
```

### パフォーマンス最適化
```cpp
void Main() {
    // デバイス固有の設定を一度だけ取得
    bool hasPolyphony = SupportsPolyphony();
    bool hasAdvanced = HasAdvancedSpeaker();
    
    while (System::Update()) {
        if (hasPolyphony) {
            // 和音機能を活用
        } else {
            // 単音機能のみ使用
        }
    }
}
```

---

M5Siv3DのM5Dial対応により、エンコーダー、RFID、そして**リッチなオーディオ機能**を活用したより豊富で表現力のあるクリエイティブなプロジェクトが可能になります！🎵 