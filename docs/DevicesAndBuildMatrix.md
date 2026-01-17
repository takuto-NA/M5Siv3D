# Devices & Build Matrix

このドキュメントは、M5Siv3D の **デバイス差分** と **ビルド設定の要点** を固定化します。

## ビルド設定（要点）

- **C++17が必要**: `platformio.ini` では `-std=gnu++17` を有効化しています。
- **M5Dialモード**: `USE_M5_DIAL` を定義すると `M5Dial.h` を使用し、エンコーダー/RFID 等のAPIが有効になります。

## デバイス差分（概要）

| 項目 | M5Dial（`USE_M5_DIAL`） | それ以外（M5Unified） |
|---|---|---|
| 初期化 | `M5Dial.begin(...)` | `M5.begin(...)` |
| 入力 | Dialエンコーダー / RFID / Touch / BtnA | Touch / BtnA など（機種依存） |
| オーディオ | 圧電ブザー相当（短音推奨・単音前提） | I2Sスピーカー等（機種依存・比較的高機能） |

## 代表的なコンパイルマクロ

- `USE_M5_DIAL`
  - 目的: M5Dial向けのデバイス層を有効化
  - 定義場所:
    - PlatformIO: `platformio.ini` の `build_flags`
    - Arduino: スケッチで `#define USE_M5_DIAL` を `#include <M5Siv3D.h>` より前に書く

## 依存関係（目安）

PlatformIOの実運用は `platformio.ini` の `lib_deps` を正としてください。

- `m5stack/M5Unified`
- `m5stack/M5Dial`（M5Dialモードの場合）
- `hideakitai/MsgPacketizer`（通信デモで使用）
- `Densaugeo/base64`（環境により `base64.hpp` が必要）

