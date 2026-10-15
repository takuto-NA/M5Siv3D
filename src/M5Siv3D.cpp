#include "M5Siv3D.h"

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