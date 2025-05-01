#include "M5Siv3D.h"

// This file is intentionally empty as this is a header-only library.
// It exists only to satisfy PlatformIO's build system requirements.

// Explicitly instantiate the M5Siv3D class to ensure it's included in the build
static M5Siv3D m5siv3d;

// Required Arduino framework functions
void setup() {
    // Initialize the library
    System::Init();
}

void loop() {
    // Main loop is handled by System::Update()
    System::Update();
} 