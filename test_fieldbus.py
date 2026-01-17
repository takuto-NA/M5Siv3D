from __future__ import annotations

import serial
import msgpack
import time
from cobs import cobs
import argparse

DEFAULT_SERIAL_PORT = "COM7"
DEFAULT_BAUD_RATE = 115200
DEFAULT_FPS = 5.0

# MsgPacketizer インデックス（M5Siv3D側: src/CommsManager.hpp に合わせる）
INDEX_COMMAND = 0
INDEX_MOTOR_TELEMETRY = 10
INDEX_SYSTEM_HEALTH = 11

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

def build_motor_telemetry_packet(phase: float, motor_count: int = 21) -> list[float]:
    """
    Index 10: std::vector<float>
    1モータあたり 4要素:
      [angleRad, torqueFlag, errorStatus, shutdownStatus]
    """
    packed: list[float] = []
    for i in range(motor_count):
        angle_rad = float(0.5 * 3.1415926535 * (phase + i * 0.1))
        torque_flag = 1.0
        error_status = -1.0     # 負値は「未送信/不明」扱い（M5側は更新しない）
        shutdown_status = -1.0  # 同上
        packed.extend([angle_rad, torque_flag, error_status, shutdown_status])
    return packed

def build_system_health_packet(phase: float) -> list[int]:
    """
    Index 11: std::vector<int32_t>
    期待サイズ:
      - 最低7要素（不足するとM5側でエラー）
      - 13要素以上で拡張情報（engineRunning / groupModes）を使用
    レイアウト（M5側実装に合わせる）:
      [0] crashLatch
      [1] persistentArmState
      [2..5] portFaults(4)
      [6] (unused, keep 0)
      [7] engineRunning
      [8..12] groupModes(5) 0:OFF 1:PLAY 2:REC
    """
    crash_latch = 0
    persistent_arm_state = 0
    port_faults = [0, 0, 0, 0]
    unused = 0
    engine_running = 1

    # なんとなく動きが見えるようにモードを周期で変化
    t = int(phase) % 30
    if t < 10:
        modes = [0, 0, 0, 0, 0]
    elif t < 20:
        modes = [1, 0, 0, 1, 0]
    else:
        modes = [2, 0, 0, 2, 0]

    return [
        crash_latch,
        persistent_arm_state,
        *port_faults,
        unused,
        engine_running,
        *modes,
    ]

def main():
    try:
        parser = argparse.ArgumentParser(description="Send dummy MsgPacketizer packets to M5Siv3D.")
        parser.add_argument("--port", default=DEFAULT_SERIAL_PORT, help="Serial port (e.g. COM9).")
        parser.add_argument("--baud", type=int, default=DEFAULT_BAUD_RATE, help="Baud rate (e.g. 115200).")
        parser.add_argument("--fps", type=float, default=DEFAULT_FPS, help="Send rate (Hz).")
        args = parser.parse_args()

        ser = serial.Serial(args.port, args.baud, timeout=0.1)
        print(f"Connected to {args.port} at {args.baud}")
        print("Sending dummy telemetry (Index10) + health (Index11) ...")

        phase = 0.0
        while True:
            # 1) モータテレメトリ（21モータ * 4値）
            motor_packet = build_motor_telemetry_packet(phase, motor_count=21)
            send_packet(ser, INDEX_MOTOR_TELEMETRY, motor_packet)

            # 2) システム健康状態（最低7要素、推奨13要素）
            health_packet = build_system_health_packet(phase)
            send_packet(ser, INDEX_SYSTEM_HEALTH, health_packet)

            phase += 0.2
            print("Sent: motor telemetry + system health")

            sleep_sec = 1.0 / max(0.1, float(args.fps))
            time.sleep(sleep_sec)

    except Exception as e:
        print(f"\nError: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    main()

