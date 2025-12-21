import serial
import msgpack
import time
from cobs import cobs

# 設定
SERIAL_PORT = 'COM7'  # CH340のポート
BAUD_RATE = 115200

# MsgPacketizer インデックス (Teensy側の定義に合わせる)
INDEX_COMMAND = 0
INDEX_MOTOR_TELEMETRY = 1
INDEX_SYSTEM_HEALTH = 2

def send_packet(ser, index, data):
    """
    MsgPacketizer形式 [COBS(index + msgpack_payload) + 0x00] で送信
    """
    # 1. MsgPackでデータをシリアライズ
    payload = msgpack.packb(data)
    
    # 2. インデックス(1byte) + ペイロード を結合
    raw_data = bytes([index]) + payload
    
    # 3. COBSエンコード
    encoded_data = cobs.encode(raw_data)
    
    # 4. デリミタ(0x00)を付与して送信
    packet = encoded_data + b'\x00'
    ser.write(packet)

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE}")
        print("Sending dummy telemetry data to M5Dial...")

        phase = 0.0
        while True:
            # 1. モーター角度 (21個)
            angles = [float(0.5 * 3.14 * (phase + i*0.1)) for i in range(21)]
            send_packet(ser, INDEX_MOTOR_TELEMETRY, angles)

            # 2. 健康状態 (0: OK, 1: FAULT)
            health = 0 if (int(phase) % 10 < 8) else 1
            send_packet(ser, INDEX_SYSTEM_HEALTH, health)

            phase += 0.2
            print(f"Sent: Motor data & Health {health}") # 改行するように変更
            time.sleep(0.2)  # 5FPS程度に落として安定性確認

    except Exception as e:
        print(f"\nError: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    main()

