import serial
import time
import argparse

DEFAULT_SERIAL_PORT = "COM3"
DEFAULT_BAUD_RATE = 115200

def main():
    try:
        parser = argparse.ArgumentParser(description="Monitor raw serial traffic (byte count + hex dump).")
        parser.add_argument("--port", default=DEFAULT_SERIAL_PORT, help="Serial port (e.g. COM3).")
        parser.add_argument("--baud", type=int, default=DEFAULT_BAUD_RATE, help="Baud rate (e.g. 115200).")
        args = parser.parse_args()

        # RS485モードであることを確認してください（U2D2のスイッチ）
        ser = serial.Serial(args.port, args.baud, timeout=0.1)
        print(f"Monitoring {args.port} at {args.baud}...")
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

