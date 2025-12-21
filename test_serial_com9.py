"""
M5Dial (ESP32系) のシリアルポートに対して、テスト文字列を書き込み/受信ログを表示する。

用途:
- COM9 へ ping を送って応答/ログが流れるか確認する
- PlatformIO の monitor と切り分ける（ポート占有/ボーレート違いの確認）
"""

from __future__ import annotations

import argparse
import sys
import time
from dataclasses import dataclass

import serial


DEFAULT_PORT = "COM9"
DEFAULT_BAUDRATE = 115200
DEFAULT_MESSAGE = "ping"
DEFAULT_APPEND_NEWLINE = True
DEFAULT_READ_SECONDS = 2.0
DEFAULT_IO_TIMEOUT_SECONDS = 0.2
READ_CHUNK_SIZE_BYTES = 1024
TEXT_ENCODING = "utf-8"
NEWLINE = "\n"

EXIT_CODE_OK = 0
EXIT_CODE_SERIAL_ERROR = 1
EXIT_CODE_INVALID_ARGUMENT = 2


@dataclass(frozen=True)
class SerialTestConfig:
    port: str
    baudrate: int
    message: str
    append_newline: bool
    read_seconds: float
    io_timeout_seconds: float
    do_write: bool


def parse_args(argv: list[str]) -> SerialTestConfig:
    parser = argparse.ArgumentParser(description="Write/read a short message via serial port.")
    parser.add_argument("--port", default=DEFAULT_PORT, help="Serial port name (e.g. COM9).")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUDRATE, help="Baud rate (e.g. 115200).")
    parser.add_argument(
        "--message",
        default=DEFAULT_MESSAGE,
        help="Message to write (text). Ignored with --no-write.",
    )
    parser.add_argument(
        "--no-newline",
        dest="append_newline",
        action="store_false",
        default=DEFAULT_APPEND_NEWLINE,
        help="Do not append a trailing newline to the message.",
    )
    parser.add_argument(
        "--read-seconds",
        type=float,
        default=DEFAULT_READ_SECONDS,
        help="How long to read after opening (and writing).",
    )
    parser.add_argument(
        "--io-timeout-seconds",
        type=float,
        default=DEFAULT_IO_TIMEOUT_SECONDS,
        help="Read/Write timeout for the serial port.",
    )
    parser.add_argument("--no-write", action="store_true", help="Do not write; read only.")

    args = parser.parse_args(argv)
    return SerialTestConfig(
        port=str(args.port),
        baudrate=int(args.baud),
        message=str(args.message),
        append_newline=bool(args.append_newline),
        read_seconds=float(args.read_seconds),
        io_timeout_seconds=float(args.io_timeout_seconds),
        do_write=not bool(args.no_write),
    )


def open_serial_port(config: SerialTestConfig) -> serial.Serial:
    serial_port = serial.Serial(
        port=config.port,
        baudrate=config.baudrate,
        timeout=config.io_timeout_seconds,
        write_timeout=config.io_timeout_seconds,
    )

    # USB-Serial の制御線トグルでリセットが掛かるボードがあるため、明示的に落とす。
    serial_port.dtr = False
    serial_port.rts = False
    return serial_port


def write_message(serial_port: serial.Serial, message: str) -> None:
    message_bytes = message.encode(TEXT_ENCODING, errors="replace")
    written_bytes = serial_port.write(message_bytes)
    serial_port.flush()

    print(f"[write] bytes={written_bytes} message={message!r}")


def read_for_duration(serial_port: serial.Serial, read_seconds: float) -> bytes:
    deadline = time.monotonic() + read_seconds
    received = bytearray()

    while time.monotonic() < deadline:
        chunk = serial_port.read(READ_CHUNK_SIZE_BYTES)
        if not chunk:
            continue

        received.extend(chunk)
        text = chunk.decode(TEXT_ENCODING, errors="replace")
        print(f"[read] {text}", end="")

    return bytes(received)


def main(argv: list[str]) -> int:
    config = parse_args(argv)

    if config.read_seconds <= 0:
        print("ERROR: --read-seconds must be > 0", file=sys.stderr)
        return EXIT_CODE_INVALID_ARGUMENT

    try:
        with open_serial_port(config) as serial_port:
            print(f"[open] port={config.port} baud={config.baudrate}")
            if config.do_write:
                message_to_send = config.message + (NEWLINE if config.append_newline else "")
                write_message(serial_port, message_to_send)
            read_for_duration(serial_port, config.read_seconds)
            print("\n[done]")
            return EXIT_CODE_OK
    except serial.SerialException as exception:
        print(f"ERROR: failed to open/use serial port: {exception}", file=sys.stderr)
        print(
            "HINT: PlatformIO monitor/other app may be using the port, or the port name is wrong.",
            file=sys.stderr,
        )
        return EXIT_CODE_SERIAL_ERROR


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

