# Debug Scripts (Python)

このリポジトリには、シリアル疎通やテレメトリ送受信を切り分けるための Python スクリプトが同梱されています。

## 依存パッケージ

最低限:

```bash
python -m pip install pyserial
```

MsgPacketizer形式の送信テストをする場合:

```bash
python -m pip install msgpack cobs
```

## `test_serial_com9.py`（M5Dialへ文字列を書いてログを読む）

- **用途**: PlatformIO monitor と切り分け、COMポート占有やボーレート違いの確認。
- **特徴**: 引数でポート/ボーレート/書き込み有無/読取時間を変更可能。

例（読取のみ）:

```bash
python .\test_serial_com9.py --port COM9 --baud 115200 --no-write --read-seconds 10
```

例（`ping`送信）:

```bash
python .\test_serial_com9.py --port COM9 --baud 115200 --message ping --read-seconds 3
```

## `test_fieldbus.py`（MsgPacketizer形式でダミー送信）

- **用途**: 外部送信側（例: Teensy）なしで、M5側UI/受信処理の動作確認。
- **送信内容**: `docs/CommsProtocol.md` の Index/ペイロードに合わせて、Index10/11 を送信します。

例:

```bash
python .\test_fieldbus.py --port COM9 --baud 115200 --fps 5
```

## `monitor_fieldbus.py`（生データの受信量/ヘキサ確認）

- **用途**: 受信データが“出ている/出ていない”をまず確認したいとき。
- **出力**: 受信バイト数と先頭20byte程度のhexダンプ。

例:

```bash
python .\monitor_fieldbus.py --port COM3 --baud 115200
```

## まず迷ったら

- COMポート特定や `Write timeout` は `docs/Troubleshooting.md` を優先して参照してください。

