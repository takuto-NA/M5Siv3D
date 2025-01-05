# M5Siv3D

A Siv3D-inspired graphics library for M5Stack devices.

## Description

M5Siv3D provides an OpenSiv3D-inspired API for M5Stack devices, offering familiar and intuitive graphics and input handling interfaces while maintaining full compatibility with M5Unified's powerful features.

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


## License

This library is licensed under the MIT License.

## Dependencies

- M5Unified