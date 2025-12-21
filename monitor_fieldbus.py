import serial
import time

# 設定
SERIAL_PORT = 'COM3'  # U2D2のポート
BAUD_RATE = 115200   # 1Mbpsに戻してテスト

def main():
    try:
        # RS485モードであることを確認してください（U2D2のスイッチ）
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
        print(f"Monitoring {SERIAL_PORT} at {BAUD_RATE}...")
        print("Waiting for data from Teensy...")

        last_print_time = time.time()
        byte_count = 0

        while True:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                byte_count += len(data)
                
                # 1秒ごとに受信状況を表示
                if time.time() - last_print_time > 1.0:
                    print(f"[{time.strftime('%H:%M:%S')}] Received: {byte_count} bytes total. Last chunk size: {len(data)} bytes.")
                    # 生データの一部をヘキサ表示（データの内容確認）
                    print(f"Raw hex: {data[:20].hex(' ')}")
                    last_print_time = time.time()
            
            time.sleep(0.01)

    except Exception as e:
        print(f"\nError: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    main()

