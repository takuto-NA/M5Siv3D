#pragma once

#include "Math.h"

// Color構造体の定義
struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}

    // RGB565形式の整数からColorを生成するコンストラクタ
    Color(uint16_t rgb565)
    {
        r = (rgb565 >> 8) & 0xF8; // 赤色成分を抽出し、5ビットから8ビットに拡張
        g = (rgb565 >> 3) & 0xFC; // 緑色成分を抽出し、6ビットから8ビットに拡張
        b = (rgb565 << 3) & 0xF8; // 青色成分を抽出し、5ビットから8ビットに拡張

        r |= r >> 5; // 下位ビットを埋める
        g |= g >> 6;
        b |= b >> 5;
    }

    // RGB888形式の整数からColorを設定するメソッド
    void setFromRGB888(uint32_t rgb888)
    {
        r = (rgb888 >> 16) & 0xFF; // 赤色成分
        g = (rgb888 >> 8) & 0xFF;  // 緑色成分
        b = rgb888 & 0xFF;         // 青色成分
    }

    // RGB565形式への変換
    uint16_t toRGB565() const
    {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // OpenSiv3D風の色操作メソッドを追加
    Color lerp(const Color &other, float t) const
    {
        return Color(
            r + (other.r - r) * t,
            g + (other.g - g) * t,
            b + (other.b - b) * t);
    }

    Color operator+(const Color &other) const
    {
        return Color(
            std::min(255, int(r) + int(other.r)),
            std::min(255, int(g) + int(other.g)),
            std::min(255, int(b) + int(other.b)));
    }

    // HSVからRGBを生成する静的メソッド
    static Color FromHSV(float h, float s, float v)
    {
        h = Math::fmod(h, 360.0f);
        s = std::min(1.0f, std::max(0.0f, s));
        v = std::min(1.0f, std::max(0.0f, v));

        float c = v * s;
        float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
        float m = v - c;

        float r = 0, g = 0, b = 0;

        if (h >= 0 && h < 60)
        {
            r = c;
            g = x;
            b = 0;
        }
        else if (h >= 60 && h < 120)
        {
            r = x;
            g = c;
            b = 0;
        }
        else if (h >= 120 && h < 180)
        {
            r = 0;
            g = c;
            b = x;
        }
        else if (h >= 180 && h < 240)
        {
            r = 0;
            g = x;
            b = c;
        }
        else if (h >= 240 && h < 300)
        {
            r = x;
            g = 0;
            b = c;
        }
        else
        {
            r = c;
            g = 0;
            b = x;
        }

        return Color(
            uint8_t((r + m) * 255),
            uint8_t((g + m) * 255),
            uint8_t((b + m) * 255));
    }

    // RGBからHSVに変換するメソッド
    void toHSV(float &h, float &s, float &v) const
    {
        float r_norm = r / 255.0f;
        float g_norm = g / 255.0f;
        float b_norm = b / 255.0f;

        float cmax = std::max({r_norm, g_norm, b_norm});
        float cmin = std::min({r_norm, g_norm, b_norm});
        float diff = cmax - cmin;

        // Hue calculation
        if (diff == 0)
            h = 0;
        else if (cmax == r_norm)
            h = fmod(60 * ((g_norm - b_norm) / diff) + 360, 360);
        else if (cmax == g_norm)
            h = 60 * ((b_norm - r_norm) / diff) + 120;
        else if (cmax == b_norm)
            h = 60 * ((r_norm - g_norm) / diff) + 240;

        // Saturation calculation
        s = (cmax == 0) ? 0 : (diff / cmax);

        // Value calculation
        v = cmax;
    }
}; 