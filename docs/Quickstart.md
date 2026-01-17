# Quickstart (運用/再現性重視)

このリポジトリは **ライブラリ (M5Siv3D)** と **PlatformIOデモアプリ** を同梱しています。

- ライブラリの公開ヘッダ: `M5Siv3D.h`（root）→ 実体は `src/M5Siv3D.h`
- PlatformIOデモアプリ: `src/main.cpp`（M5Dial想定のテレメトリビューア）

## 1) PlatformIO（このリポジトリをそのままビルド/書き込み）

### 前提

- Windowsの場合、まずは `docs/Troubleshooting.md` の「COMポート確認」「Upload Write timeout」も併せて参照してください。

### 手順

1. PlatformIOをインストール（VSCode拡張 or CLI）
2. このリポジトリを PlatformIO プロジェクトとして開く
3. `platformio.ini` のポート設定を **自分の環境に合わせて変更**
   - `upload_port`
   - `monitor_port`
4. ビルド＆書き込み

```bash
pio run -t upload
```

5. シリアルモニタ

```bash
pio device monitor
```

### よくある落とし穴

- **Uploadが `Write timeout`**: ほぼ「別アプリがCOMポートを占有」か「USBの一時不調」です。`docs/Troubleshooting.md` の手順（モニタを閉じる→USB抜き差し）に従ってください。
- **Windows cp932で `pio pkg search` が落ちる**: `docs/Troubleshooting.md` のUTF-8強制手順を参照してください。

## 2) Arduino IDE（ライブラリとして使う）

### 手順（手動導入）

1. このリポジトリをArduinoライブラリフォルダへ配置
2. Arduino IDEを再起動
3. スケッチでインクルード:

```cpp
#include <M5Siv3D.h>
```

### M5Dial固有機能を使う

`USE_M5_DIAL` を **インクルード前に** 定義します。

```cpp
#define USE_M5_DIAL
#include <M5Siv3D.h>
```

詳細は `README_M5Dial.md` を参照してください。

