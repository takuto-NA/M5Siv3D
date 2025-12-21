## トラブルシュート（再発防止メモ）

このドキュメントは、M5Dial + PlatformIO + M5Siv3D で実際に発生した問題と、再発防止のための手順をまとめたものです。

### 1) 画面が点滅する（リセットループ）

- **症状**: 画面が点滅し続ける / シリアルに `Guru Meditation Error` が出る
- **原因候補**:
  - 何らかのクラッシュ（例: 不整合なファーム、未初期化の参照、API不一致）
  - シリアル入力で例外系コードパスに入ってクラッシュ（実際に `ping` 送信で再現）
- **切り分け**:
  - まず **書き込み無しでログを見る**（再起動原因の確認）
  - 書き込みすると再起動するなら、受信側パーサ/コマンド処理を疑う
- **実行例**:

```bash
python .\test_serial_com9.py --port COM9 --baud 115200 --no-write --read-seconds 10
```

### 2) COMポートが分からない（COM9/COM6が混在する）

- **症状**: 接続しているはずのデバイスにログが出ない / 反応がない
- **対策**: Pythonで **VID/PID/SN** を見て確定する（ESP32系は `VID:PID=303A:xxxx` が多い）

```bash
python -c "from serial.tools import list_ports as lp; import json; ports=[{'device':p.device,'hwid':p.hwid,'desc':p.description,'sn':getattr(p,'serial_number',None)} for p in lp.comports()]; print(json.dumps(ports, ensure_ascii=False, indent=2))"
```

### 3) COM9へ書き込みできない / Uploadが `Write timeout`

- **症状**:
  - `pio run -t upload` が `A serial exception error occurred: Write timeout`
  - `pyserial` での書き込みも `Write timeout`
- **原因候補**:
  - **他のアプリがCOMを掴んでいる**（PlatformIO monitor / Arduino IDE / VSCode拡張など）
  - USB接続が不安定 / ドライバやモード（USB-Serial/JTAG）が変な状態
- **再発防止**:
  - Upload前に **シリアルモニタを必ず閉じる**
  - 詰まったら **USB抜き差し**（ほぼ確実に復帰する）
  - `upload_port` / `monitor_port` を `platformio.ini` で固定（ポートが変わった場合は更新）

### 4) PlatformIOの `pkg search` が文字化け/Unicodeで落ちる（Windows cp932）

- **症状**: `pio pkg search ...` が `UnicodeEncodeError: 'cp932' codec can't encode ...`
- **対策**: コマンド実行時だけ **UTF-8** を強制する

```bash
cmd /c "chcp 65001>nul & set PYTHONIOENCODING=utf-8 & python -m platformio pkg search m5stack/M5Dial"
```

### 5) `M5Dial.h` / `base64.hpp` が無い（依存不足）

- **症状**:
  - `fatal error: M5Dial.h: No such file or directory`
  - `fatal error: base64.hpp: No such file or directory`
- **対策**: `platformio.ini` の `lib_deps` に追加する
  - `m5stack/M5Dial`
  - `Densaugeo/base64`（`<base64.hpp>` を提供）

### 6) M5Unifiedのバージョン不整合（APIが合わない）

- **症状**: 依存ライブラリの中で `getButton` が無いなどのビルドエラー
- **原因**: 古い `M5Unified` と `M5Dial` の組み合わせ
- **対策**: `m5stack/M5Unified` を **新しめに揃える**
  - 今回は `m5stack/M5Unified@^0.2.11` で安定

### 7) C++17が必要（`if constexpr` など）

- **症状**: `if constexpr` が使えない等の警告/エラー
- **対策**:
  - `platformio.ini` で **C++17を有効**にする
  - 併せて `build_unflags = -std=gnu++11` で古い標準指定を外す

### 8) `M5Siv3D.h` が空で include できない

- **症状**: `#include <M5Siv3D.h>` が期待通り動かない
- **対策**: ルートの `M5Siv3D.h` を **公開ヘッダ（src/M5Siv3D.h を include）**にする

### 9) フォント周りでビルドが落ちる（重複定義 / experimentalブロック）

- **症状**: `FontInfo` の再定義などでコンパイルエラー
- **対策**:
  - `FontInfo` の **二重定義を解消**
  - 実装が未整理な experimental なフォント管理ブロックは **一旦無効化**し、既存の `FontRegistry` を使う

### 10) まず動かすための推奨手順（最短）

- **手順**:
  - `platformio.ini` を「M5Dial + M5Unified + base64 + C++17」構成にする
  - `src/main.cpp` はまず `examples/M5Dial_Example.cpp` を基に、M5Siv3D APIで動く最小例にする
  - Uploadが詰まったら、モニタを閉じてUSB抜き差し→再アップロード

