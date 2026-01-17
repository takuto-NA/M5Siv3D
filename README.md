# M5Siv3D

A Siv3D-inspired graphics library for M5Stack devices.

> **For future me / our team:** This repository contains both **the library** and a **PlatformIO demo app** (telemetry viewer) under `src/main.cpp`.

## Description

M5Siv3D provides an OpenSiv3D-inspired API for M5Stack devices, offering familiar and intuitive graphics and input handling interfaces while maintaining full compatibility with M5Unified's powerful features.

## What’s in this repository

- **Library**
  - Public include entry: `M5Siv3D.h` (root) → includes `src/M5Siv3D.h`
  - Device selection:
    - Default: M5Unified (`#include <M5Unified.h>`)
    - M5Dial mode: define `USE_M5_DIAL` to use `#include "M5Dial.h"` and enable encoder/RFID helpers
- **PlatformIO demo app (Telemetry Viewer)**
  - `src/main.cpp` uses `src/CommsManager.hpp` to receive telemetry via `MsgPacketizer` and render a UI on M5Dial.
  - This is meant as an **app/example**, not the core library API reference.
- **Docs**
  - Troubleshooting: `docs/Troubleshooting.md`
  - Memory (fonts): `docs/MemoryOptimization.md`
  - Fonts: `docs/FontSystem.md`

## Quickstart (PlatformIO, recommended)

This repo is already a PlatformIO project (`platformio.ini`).

1. Install PlatformIO (VSCode or CLI)
2. Open this repository as a PlatformIO project
3. Update serial port settings in `platformio.ini`:
   - `upload_port = COM9`
   - `monitor_port = COM9`
4. Build/Upload:

```bash
pio run -t upload
```

5. Monitor:

```bash
pio device monitor
```

If upload fails with `Write timeout`, close any serial monitor and re-plug USB. See `docs/Troubleshooting.md`.

## Features

- Simple and intuitive graphics API
- Vector mathematics support
- Color manipulation with RGB and HSV
- Text rendering with multiple fonts
- Input handling for buttons
- IMU support
- Compatible with all M5Stack devices

## Installation

### Using Arduino Library Manager
*Coming soon*

### Manual Installation
1. Download this repository
2. Extract to your Arduino libraries folder
3. Restart Arduino IDE

## Using as an Arduino library

Include the public header:

```cpp
#include <M5Siv3D.h>
```

For M5Dial-specific features, define `USE_M5_DIAL` **before** including:

```cpp
#define USE_M5_DIAL
#include <M5Siv3D.h>
```

## Usage

```cpp
#include <M5Siv3D.h>
void setup() {
System::getInstance().setBackgroundColor(Palette::White);
}
void loop() {
System::getInstance().Update();
Circle(160, 120, 30).draw(Palette::Blue);
}
```

## Troubleshooting

- See `docs/Troubleshooting.md`

## Additional documentation

- `docs/Quickstart.md` (PlatformIO/Arduino quickstart)
- `docs/DevicesAndBuildMatrix.md` (device differences, build notes)
- `docs/DebugScripts.md` (python scripts for debugging serial/comms)
- `docs/CommsProtocol.md` (MsgPacketizer index & payload layout used by the demo app)
- `README_M5Dial.md` (M5Dial features: encoder/RFID/audio)
- `README_Creative_Demo.md` (M5Dial creative demo overview)
- `docs/FontSystem.md` (font API)
- `docs/MemoryOptimization.md` (reduce flash usage by disabling fonts)


## License

This library is licensed under the MIT License.

## Dependencies

- M5Unified