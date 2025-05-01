/*
 * M5Siv3D
 * https://github.com/chobby/M5Siv3D
 * Copyright (c) 2024 chobby
 * Licensed under MIT License
 *
 * M5Siv3D - A wrapper library for M5Unified
 *
 * This library provides an OpenSiv3D-inspired API for M5Stack devices,
 * offering familiar and intuitive graphics and input handling interfaces
 * while maintaining full compatibility with M5Unified's powerful features.
 *
 * Designed to make creative coding on M5Stack devices more accessible
 * through a simplified, yet powerful programming interface.
 */

#pragma once

#define M5SIV3D_INCLUDED

//////////////////////////////////////////////////
//
//	Core Dependencies
//
//////////////////////////////////////////////////

#include <M5Unified.h>
#include <base64.hpp>
#include <SPI.h>
#include <sstream>

//////////////////////////////////////////////////
//
//	Math Utilities
//
//////////////////////////////////////////////////

#include "M5Siv3D/Math.h"

//////////////////////////////////////////////////
//
//	Core Components
//
//////////////////////////////////////////////////

#include "M5Siv3D/Color.h"
#include "M5Siv3D/Palette.h"
#include "M5Siv3D/Input.h"
#include "M5Siv3D/System.h"
#include "M5Siv3D/Print.h"

//////////////////////////////////////////////////
//
//	Graphics
//
//////////////////////////////////////////////////

#include "M5Siv3D/Shapes.h"
#include "M5Siv3D/Font.h"
#include "M5Siv3D/Image.h"

//////////////////////////////////////////////////
//
//	GUI
//
//////////////////////////////////////////////////

#include "M5Siv3D/SimpleGUI.h"

//////////////////////////////////////////////////
//
//	Main Function
//
//////////////////////////////////////////////////

#ifndef M5SIV3D_LEGACY_STYLE

// These functions are defined in M5Siv3D.cpp
void setup();
void loop();

#endif

class M5Siv3D {
public:
    inline M5Siv3D() {
        // コンストラクタの実装
    }
    
    inline ~M5Siv3D() {
        // デストラクタの実装
    }
}; 