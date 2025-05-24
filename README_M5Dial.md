# M5Siv3D with M5Dial Support

M5Siv3DがM5Dialに対応しました！エンコーダーやRFID機能を含む、M5Dialのすべての機能をSiv3D風のAPIで使用できます。

## 🌟 新機能

### M5Dial固有の機能
- **エンコーダー**: 回転入力による直感的な操作
- **RFID**: NFCカードの読み書き
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
│   └── M5Dial_Example.cpp # M5Dial使用例
└── M5Dial.h               # M5Dialライブラリ（別途必要）
```

## 📖 基本的な使い方

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
        }
        
        // ボタンAでリセット
        if (Input::ButtonA.pressed()) {
            Input::Encoder.reset();
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
    // 両方の機能を有効化
    auto cfg = M5.config();
    M5RealUnified::begin(cfg, true, true);  // encoder=true, rfid=true
    
    int mode = 0;  // 0: エンコーダー, 1: RFID
    
    while (System::Update()) {
        // モード切り替え
        if (Input::ButtonA.pressed()) {
            mode = (mode + 1) % 2;
        }
        
        if (mode == 0) {
            // エンコーダーモード
            long value = Input::Encoder.getValue();
            
            // 値に応じて色を変更
            float hue = Math::fmod(value * 5.0f, 360.0f);
            Color color = Color::FromHSV(hue, 1.0f, 1.0f);
            
            // 円を描画
            Circle(System::Width()/2, System::Height()/2, 50).draw(color);
            
        } else {
            // RFIDモード
            if (Input::RFID.isCardPresent()) {
                Rect(50, 50, 100, 50).draw(Palette::Green);
                Print << "Card OK";
            } else {
                Rect(50, 50, 100, 50).draw(Palette::Red);
                Print << "No Card";
            }
        }
    }
}
```

## 🎯 APIリファレンス

### Input::Encoder クラス

#### 基本メソッド
```cpp
long read()                    // 現在の値を取得
void write(long value)         // 値を設定
long readAndReset()           // 値を取得してリセット
```

#### Siv3D風拡張メソッド
```cpp
long getValue()               // read()のエイリアス
void setValue(long value)     // write()のエイリアス
void reset()                  // readAndReset()のエイリアス
long getDelta()              // 前回からの変化量
bool changed()               // 値が変化したかどうか
```

### Input::RFID クラス

#### カード検出
```cpp
bool isCardPresent()          // カードが存在するかチェック
String readCardUID()          // カードのUIDを読み取り
```

#### データ操作
```cpp
bool readCardData(uint8_t blockAddr, uint8_t* buffer, uint8_t bufferSize)
bool writeCardData(uint8_t blockAddr, uint8_t* buffer, uint8_t bufferSize)
```

### M5RealUnified 名前空間

#### デバイス統一API
```cpp
M5RealUnified::begin(config, enableEncoder, enableRFID)  // 初期化
M5RealUnified::update()                                  // 状態更新
M5RealUnified::getDevice()                              // デバイス取得
M5RealUnified::getDisplay()                             // ディスプレイ取得
M5RealUnified::getEncoder()                             // エンコーダー取得
M5RealUnified::getRfid()                                // RFID取得
```

## 🔧 設定オプション

### 条件コンパイル
```cpp
// M5Dialを使用
#define USE_M5_DIAL
#include "M5Siv3D.h"

// 通常のM5Unified（デフォルト）
#include "M5Siv3D.h"
```

### 初期化パラメータ
```cpp
// 完全機能版
M5RealUnified::begin(cfg, true, true);   // エンコーダー + RFID

// エンコーダーのみ
M5RealUnified::begin(cfg, true, false);  // エンコーダーのみ

// RFIDのみ
M5RealUnified::begin(cfg, false, true);  // RFIDのみ

// 基本機能のみ
M5RealUnified::begin(cfg, false, false); // M5Unified互換
```

## 💡 設計思想

### 1. 被害最小化原則
- 既存コードへの影響を最小限に抑制
- 条件コンパイルによる自動切り替え
- 後方互換性の完全保証

### 2. 統一API設計
- M5RealUnified名前空間による抽象化
- デバイス固有機能の透明な提供
- シンプルなマクロベース設定

### 3. 拡張性重視
- 新しいM5デバイスへの対応を容易に
- 機能追加時の影響範囲を限定
- モジュラー設計による保守性向上

## 🚀 サンプルプロジェクト

完全な動作例は `examples/M5Dial_Example.cpp` を参照してください。このサンプルでは以下の機能を実装しています：

- **エンコーダー値表示**: リアルタイムで値と変化量を表示
- **円形インジケーター**: エンコーダー値を針で視覚化
- **RFID読み取り**: カード検出とUID表示
- **動的色変更**: エンコーダー値に応じたHSVカラー変更
- **モード切り替え**: ボタンによる機能切り替え

## 🔍 トラブルシューティング

### コンパイルエラー
```cpp
// エラー: USE_M5_DIAL が定義されていない
#define USE_M5_DIAL  // この行を追加
#include "M5Siv3D.h"
```

### 機能が動作しない
```cpp
// M5Dialの機能を使用する前に適切に初期化
M5RealUnified::begin(cfg, true, true);  // エンコーダーとRFIDを有効化
```

### 既存コードの移植
```cpp
// 変更前（M5Unified直接使用）
M5.begin(cfg);
M5.update();

// 変更後（M5RealUnified使用）
M5RealUnified::begin(cfg);
M5RealUnified::update();
```

---

M5Siv3DのM5Dial対応により、より豊富な入力方法と機能を活用したクリエイティブなプロジェクトが可能になります！ 