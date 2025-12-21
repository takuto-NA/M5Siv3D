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

// =============================================================================
// メモリ最適化設定 - 必要なフォントのみ有効化
// =============================================================================

// 基本フォントセット（常に有効）
#define M5SIV3D_ENABLE_BASIC_FONTS 1

// 日本語フォント（大容量）
#ifndef M5SIV3D_ENABLE_JAPANESE_FONTS
#define M5SIV3D_ENABLE_JAPANESE_FONTS 1  // デフォルトで有効
#endif

// 装飾フォント（中容量）
#ifndef M5SIV3D_ENABLE_DECORATIVE_FONTS
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 1  // デフォルトで有効
#endif

// 大きなフォント（32px以上）
#ifndef M5SIV3D_ENABLE_LARGE_FONTS
#define M5SIV3D_ENABLE_LARGE_FONTS 1  // デフォルトで有効
#endif

// 太字フォント
#ifndef M5SIV3D_ENABLE_BOLD_FONTS
#define M5SIV3D_ENABLE_BOLD_FONTS 1  // デフォルトで有効
#endif

// =============================================================================
// M5 Device Selection - M5Dial and M5Unified Support
// =============================================================================

#ifdef USE_M5_DIAL
    #include "M5Dial.h"
    #define M5_DEVICE M5Dial
    
    // M5Dial compatibility layer - 完全にM5Dialのみを使用
    namespace M5RealUnified {
        // M5Dial専用の実装
        m5::M5_DIAL& getDevice() { return M5Dial; }
        M5GFX& getDisplay() { return M5Dial.Display; }
        m5::Touch_Class& getTouch() { return M5Dial.Touch; }
        m5::Power_Class& getPower() { return M5Dial.Power; }
        m5::Speaker_Class& getSpeaker() { return M5Dial.Speaker; }
        
        // M5Dial specific features
        ENCODER& getEncoder() { return M5Dial.Encoder; }
        MFRC522& getRfid() { return M5Dial.Rfid; }
        
        // ボタン：M5DialのBtnAのみ使用
        m5::Button_Class& getButtonA() { return M5Dial.BtnA; }
        
        // M5Dial用の設定とDelay
        m5::M5Unified::config_t config() { 
            m5::M5Unified::config_t cfg;
            return cfg; 
        }
        void delay(uint32_t ms) { ::delay(ms); }
        
        // Update function - M5Dialのみ
        void update() { 
            M5Dial.update();
        }
        
        // Begin function - M5Dialのみ
        template<typename... Args>
        void begin(Args&&... args) { M5Dial.begin(std::forward<Args>(args)...); }
        
        // =============================================================================
        // Audio Device Capability Detection for M5Dial
        // =============================================================================
        
        // M5Dialのオーディオ機能情報
        struct AudioCapabilities {
            bool hasBasicBuzzer = true;      // 基本的なブザー機能
            bool hasAdvancedSpeaker = false; // 高度なスピーカー機能（I2S等）
            bool supportsPolyphony = false;  // 和音対応
            bool supportsWaveforms = false;  // 波形選択対応
            bool supportsVolumeControl = false; // ボリューム制御（M5Dialブザーは非対応）
            uint8_t maxVolume = 255;         // 最大音量（実際は固定）
            float minFrequency = 100.0f;     // 最小周波数（ブザー特性に合わせて調整）
            float maxFrequency = 5000.0f;    // 最大周波数（ブザー特性に合わせて調整）
            uint32_t maxDuration = 500;      // 最大再生時間（ms）- 短く制限
            
            // M5Dial固有の制限
            bool requiresShortTones = true;   // 短い音の推奨
            bool limitedFrequencyRange = true; // 周波数範囲の制限
        };
        
        // M5Dialのオーディオ機能を取得
        const AudioCapabilities& getAudioCapabilities() {
            static AudioCapabilities caps;
            return caps;
        }
        
        // C++17 constexpr if を使った安全な音再生（M5Dial最適化版）
        [[nodiscard]] bool playToneSafe(float frequency, uint32_t duration, uint8_t volume = 128) noexcept {
            const auto& caps = getAudioCapabilities();
            
            // 範囲制限（Math名前空間が定義される前なので直接実装）
            const auto safeFreq = (frequency < caps.minFrequency) ? caps.minFrequency : 
                       (frequency > caps.maxFrequency) ? caps.maxFrequency : frequency;
            const auto safeDuration = (duration < 10u) ? 10u : (duration > 500u) ? 500u : duration;
            
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = getSpeaker();
                
                // ボリューム0の場合は無音として成功扱い
                if (volume == 0) {
                    return true;
                }
                
                // M5Dialブザーは固定ボリュームで動作
                speaker.setVolume(255);
                speaker.tone(safeFreq, safeDuration);
                return true;
            }
        }
        
        // C++17 noexcept指定のM5Dial用音停止
        void stopAudioSafe() noexcept {
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = getSpeaker();
                speaker.stop();
            }
        }
    }
    
#else
    #include <M5Unified.h>
    #define M5_DEVICE M5
    
    // Standard M5Unified compatibility layer
    namespace M5RealUnified {
        m5::M5Unified& getDevice() { return M5; }
        M5GFX& getDisplay() { return M5.Display; }
        m5::Touch_Class& getTouch() { return M5.Touch; }
        m5::Power_Class& getPower() { return M5.Power; }
        m5::Speaker_Class& getSpeaker() { return M5.Speaker; }
        m5::Button_Class& getButtonA() { return M5.BtnA; }
        
        // Update and begin functions
        void update() { M5.update(); }
        
        template<typename... Args>
        void begin(Args&&... args) { M5.begin(std::forward<Args>(args)...); }
        
        // =============================================================================
        // Audio Device Capability Detection for M5Unified Devices
        // =============================================================================
        
        // 他のM5Stackデバイスのオーディオ機能情報
        struct AudioCapabilities {
            bool hasBasicBuzzer = false;     // 基本的なブザー機能
            bool hasAdvancedSpeaker = true;  // 高度なスピーカー機能（I2S等）
            bool supportsPolyphony = true;   // 和音対応（最大8音）
            bool supportsWaveforms = true;   // 波形選択対応
            uint8_t maxVolume = 255;         // 最大音量
            float minFrequency = 20.0f;      // 最小周波数
            float maxFrequency = 20000.0f;   // 最大周波数
            uint32_t maxDuration = 60000;    // 最大再生時間（ms）
            
            // M5Unified固有の機能
            bool requiresShortTones = false;   // 長時間再生可能
            bool limitedFrequencyRange = false; // 広い周波数範囲
        };
        
        // M5Unifiedデバイスのオーディオ機能を取得
        const AudioCapabilities& getAudioCapabilities() {
            static AudioCapabilities caps;
            return caps;
        }
        
        // C++17 constexpr if を使った安全な音再生（M5Unified最適化版）
        [[nodiscard]] bool playToneSafe(float frequency, uint32_t duration, uint8_t volume = 128) noexcept {
            const auto& caps = getAudioCapabilities();
            
            // 範囲制限（Math名前空間が定義される前なので直接実装）
            const auto safeFreq = (frequency < caps.minFrequency) ? caps.minFrequency : 
                       (frequency > caps.maxFrequency) ? caps.maxFrequency : frequency;
            const auto safeDuration = (duration < 10u) ? 10u : 
                      (duration > caps.maxDuration) ? caps.maxDuration : duration;
            const auto safeVolume = (static_cast<int>(volume) < 0) ? 0 : 
                                   (static_cast<int>(volume) > static_cast<int>(caps.maxVolume)) ? 
                                   static_cast<int>(caps.maxVolume) : static_cast<int>(volume);
            
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = getSpeaker();
                speaker.setVolume(static_cast<uint8_t>(safeVolume));
                speaker.tone(safeFreq, safeDuration);
                return true;
            }
        }
        
        // C++17 noexcept指定のM5Unified用音停止
        void stopAudioSafe() noexcept {
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = getSpeaker();
                speaker.stop();
            }
        }
        
        // C++17 constexpr if を使ったM5Unified固有の高度な機能
        [[nodiscard]] bool playChord(const std::vector<float>& frequencies, uint32_t duration, uint8_t volume = 128) noexcept {
            if constexpr (true) {  // ESP32では例外を使わない
            if (!getAudioCapabilities().supportsPolyphony) return false;
            
                auto& speaker = getSpeaker();
                speaker.setVolume(volume);
                
                                 // C++17 constexpr値を使った最大音数制限
                 constexpr size_t maxPolyphony = 8;
                 const auto maxNotes = (frequencies.size() < maxPolyphony) ? frequencies.size() : maxPolyphony;
                
                for (size_t i = 0; i < maxNotes; ++i) {
                    speaker.tone(frequencies[i], duration, i);
                }
                return true;
            }
        }
    }
    
#endif

#include <base64.hpp>  // Densaugeoのライブラリ
#include <SPI.h>
#include <sstream>
#include <optional>
#include <functional>
#include <mutex>      // C++17 std::call_once用
#include <type_traits> // C++17 型特性用
#include <string_view> // C++17 string_view
#include <array>       // C++17 std::array

// C++17 using宣言
using namespace std::string_view_literals;

// 数学ユーティリティを格納する名前空間
namespace Math
{
    template <typename T>
    struct Vec2
    {
        T x, y;

        Vec2(T _x = 0, T _y = 0) : x(_x), y(_y) {}

        // 基本的な演算子のオーバーロード
        Vec2 operator+(const Vec2 &other) const { return Vec2(x + other.x, y + other.y); }
        Vec2 operator-(const Vec2 &other) const { return Vec2(x - other.x, y - other.y); }
        Vec2 operator*(T scalar) const { return Vec2(x * scalar, y * scalar); }
        Vec2 operator/(T scalar) const { return Vec2(x / scalar, y / scalar); }

        // ドット積
        T dot(const Vec2 &other) const { return x * other.x + y * other.y; }

        // 長さ
        T length() const { return std::sqrt(lengthSquared()); }
        T lengthSquared() const { return x * x + y * y; }

        // 正規化
        Vec2 normalized() const
        {
            T len = length();
            return len ? (*this / len) : Vec2();
        }
    };

    template <typename T>
    struct Vec3
    {
        T x, y, z;

        Vec3(T _x = 0, T _y = 0, T _z = 0) : x(_x), y(_y), z(_z) {}

        // 基本的な演算子のオーバーロード
        Vec3 operator+(const Vec3 &other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3 &other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(T scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
        Vec3 operator/(T scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

        // ドット積
        T dot(const Vec3 &other) const { return x * other.x + y * other.y + z * other.z; }

        // クロス積
        Vec3 cross(const Vec3 &other) const
        {
            return Vec3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x);
        }

        // 長さ
        T length() const { return std::sqrt(lengthSquared()); }
        T lengthSquared() const { return x * x + y * y + z * z; }

        // 正規化
        Vec3 normalized() const
        {
            T len = length();
            return len ? (*this / len) : Vec3();
        }
    };

    // clamp関数の実装を追加
    template <typename T>
    T clamp(T value, T min, T max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    // C++17 using宣言による型エイリアス
    using Vec2f = Vec2<float>;
    using Vec2d = Vec2<double>;
    using Vec2i = Vec2<int32_t>;
    using Vec3f = Vec3<float>;
    using Vec3d = Vec3<double>;
    using Vec3i = Vec3<int32_t>;

    // C++17 変数テンプレート対応の型特性
    template<typename T>
    inline constexpr bool is_vec2_v = false;
    template<typename T>
    inline constexpr bool is_vec2_v<Vec2<T>> = true;
    template<typename T>
    inline constexpr bool is_vec3_v = false;
    template<typename T>
    inline constexpr bool is_vec3_v<Vec3<T>> = true;

    // C++17 inline constexpr 数学定数
    inline constexpr float Pi = 3.14159265358979323846f;
    inline constexpr float TwoPi = Pi * 2.0f;
    inline constexpr float HalfPi = Pi / 2.0f;
    inline constexpr float QuarterPi = Pi / 4.0f;
    inline constexpr float E = 2.71828182845904523536f;

    // C++17 追加の数学定数
    inline constexpr float Sqrt2 = 1.41421356237309504880f;
    inline constexpr float Sqrt3 = 1.73205080756887729353f;
    inline constexpr float InvPi = 1.0f / Pi;
    inline constexpr float Deg2Rad = Pi / 180.0f;
    inline constexpr float Rad2Deg = 180.0f / Pi;

    // C++17 constexpr 数学関数
    template <typename T>
    [[nodiscard]] constexpr T abs(T x) noexcept { return x < 0 ? -x : x; }

    template <typename T>
    [[nodiscard]] constexpr T min(T a, T b) noexcept { return a < b ? a : b; }

    template <typename T>
    [[nodiscard]] constexpr T max(T a, T b) noexcept { return a > b ? a : b; }

            // C++17 可変長テンプレート版（constexpr制約を緩和）
        template <typename T, typename... Args>
        [[nodiscard]] T min(T first, Args... args) noexcept {
            if constexpr (sizeof...(args) == 0) {
                return first;
            } else {
                return min(first, min(args...));
            }
        }
        
        template <typename T, typename... Args>
        [[nodiscard]] T max(T first, Args... args) noexcept {
            if constexpr (sizeof...(args) == 0) {
                return first;
            } else {
                return max(first, max(args...));
            }
        }

    // C++17 nodiscard属性付き三角関数（ラジアン）
    [[nodiscard]] inline float sin(float x) noexcept { return ::sinf(x); }
    [[nodiscard]] inline float cos(float x) noexcept { return ::cosf(x); }
    [[nodiscard]] inline float tan(float x) noexcept { return ::tanf(x); }
    
    // C++17 nodiscard属性付き逆三角関数
    [[nodiscard]] inline float asin(float x) noexcept { return ::asinf(x); }
    [[nodiscard]] inline float acos(float x) noexcept { return ::acosf(x); }
    [[nodiscard]] inline float atan(float x) noexcept { return ::atanf(x); }
    [[nodiscard]] inline float atan2(float y, float x) noexcept { return ::atan2f(y, x); }

    // 双曲線関数
    inline float sinh(float x) { return ::sinhf(x); }
    inline float cosh(float x) { return ::coshf(x); }
    inline float tanh(float x) { return ::tanhf(x); }

    // 指数・対数関数
    inline float exp(float x) { return ::expf(x); }
    inline float log(float x) { return ::logf(x); }
    inline float log10(float x) { return ::log10f(x); }
    inline float pow(float x, float y) { return ::powf(x, y); }
    inline float sqrt(float x) { return ::sqrtf(x); }

    // C++17 constexpr 角度変換
    [[nodiscard]] constexpr float ToRadians(float degrees) noexcept { return degrees * Deg2Rad; }
    [[nodiscard]] constexpr float ToDegrees(float radians) noexcept { return radians * Rad2Deg; }

    // C++17 constexpr 線形補間
    template <typename T>
    [[nodiscard]] constexpr T lerp(T a, T b, float t) noexcept { return a + (b - a) * t; }

    // 値の符号を返す
    template <typename T>
    int sign(T x) { return (x > 0) - (x < 0); }

    // 2つの値の差の絶対値
    template <typename T>
    T distance(T a, T b) { return abs(a - b); }

    // 値が範囲内にあるかチェック
    template <typename T>
    bool inRange(T x, T min, T max) { return min <= x && x <= max; }

    // 値を0.0から1.0の範囲に正規化
    template <typename T>
    float normalize(T x, T min, T max) { return static_cast<float>(x - min) / (max - min); }

    // 剰余演算
    inline float fmod(float x, float y) { return ::fmodf(x, y); }
    inline double fmod(double x, double y) { return ::fmod(x, y); }

    // 浮動小数点数の整数部と小数部を分離
    inline float modf(float x, float* intpart) { return ::modff(x, intpart); }
    inline double modf(double x, double* intpart) { return ::modf(x, intpart); }

    // 切り上げ・切り捨て・四捨五入
    inline float ceil(float x) { return ::ceilf(x); }
    inline float floor(float x) { return ::floorf(x); }
    inline float round(float x) { return ::roundf(x); }
    
    // 整数への変換（切り捨て）
    inline int32_t trunc(float x) { return static_cast<int32_t>(x); }
    
    // 小数部分の取得
    inline float fract(float x) { return x - floor(x); }

    // 値を指定された範囲内に収める（循環）
    template <typename T>
    T wrap(T value, T min, T max)
    {
        const T range = max - min;
        if (range == 0) return min;
        
        value = fmod(value - min, range);
        if (value < 0) value += range;
        
        return value + min;
    }

    // 2つの角度の間の最短の差を計算（ラジアン）
    inline float angleDiff(float a, float b)
    {
        float diff = fmod(b - a + Pi, TwoPi) - Pi;
        return diff < -Pi ? diff + TwoPi : diff;
    }
}

// グローバル名前空間でも使えるように using 宣言を追加
using Math::clamp;
using Math::abs;
using Math::min;
using Math::max;
using Math::lerp;
using Math::Pi;
using Math::TwoPi;
using Math::ToRadians;
using Math::ToDegrees;
using Math::ceil;
using Math::floor;
using Math::round;
using Math::trunc;
using Math::fract;
using Math::wrap;

// C++17対応のColor構造体
struct Color
{
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};

    // C++17 デフォルト初期化とconstexpr対応
    constexpr Color() noexcept = default;
    constexpr Color(uint8_t red, uint8_t green, uint8_t blue) noexcept : r(red), g(green), b(blue) {}

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

    // RGB888形式の整数からColorを生成するコンストラクタ
    void setFromRGB888(uint32_t rgb888)
    {
        r = (rgb888 >> 16) & 0xFF; // 赤色成分
        g = (rgb888 >> 8) & 0xFF;  // 緑色成分
        b = rgb888 & 0xFF;         // 青色成分
    }

    // C++17 constexpr RGB565変換
    [[nodiscard]] constexpr uint16_t toRGB565() const noexcept {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // C++17 constexpr 色操作メソッド
    [[nodiscard]] constexpr Color lerp(const Color &other, float t) const noexcept {
        return {
            static_cast<uint8_t>(r + (other.r - r) * t),
            static_cast<uint8_t>(g + (other.g - g) * t),
            static_cast<uint8_t>(b + (other.b - b) * t)
        };
    }

    [[nodiscard]] constexpr Color operator+(const Color &other) const noexcept {
        return {
            static_cast<uint8_t>(Math::min(255, static_cast<int>(r) + static_cast<int>(other.r))),
            static_cast<uint8_t>(Math::min(255, static_cast<int>(g) + static_cast<int>(other.g))),
            static_cast<uint8_t>(Math::min(255, static_cast<int>(b) + static_cast<int>(other.b)))
        };
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

// C++17 inline constexpr 色定数
namespace Palette
{
    // Basic colors
    inline constexpr Color Black{0, 0, 0};
    inline constexpr Color White{255, 255, 255};
    inline constexpr Color Red{255, 0, 0};
    inline constexpr Color Green{0, 128, 0};
    inline constexpr Color Blue{0, 0, 255};
    inline constexpr Color Yellow{255, 255, 0};
    inline constexpr Color Magenta{255, 0, 255};
    inline constexpr Color Cyan{0, 255, 255};

    // Gray shades
    inline constexpr Color Dimgray{105, 105, 105};
    inline constexpr Color Gray{128, 128, 128};
    inline constexpr Color Darkgray{169, 169, 169};
    inline constexpr Color Silver{192, 192, 192};
    inline constexpr Color Lightgray{211, 211, 211};
    inline constexpr Color Gainsboro{220, 220, 220};
    inline constexpr Color Whitesmoke{245, 245, 245};

    // Warm colors
    const Color Orange(255, 165, 0);
    const Color Darkorange(255, 140, 0);
    const Color Coral(255, 127, 80);
    const Color Tomato(255, 99, 71);
    const Color Orangered(255, 69, 0);
    const Color Crimson(220, 20, 60);
    const Color Firebrick(178, 34, 34);
    const Color Darkred(139, 0, 0);
    const Color Maroon(128, 0, 0);

    // Cool colors
    const Color Navy(0, 0, 128);
    const Color Darkblue(0, 0, 139);
    const Color Mediumblue(0, 0, 205);
    const Color Royalblue(65, 105, 225);
    const Color Steelblue(70, 130, 180);
    const Color Deepskyblue(0, 191, 255);
    const Color Dodgerblue(30, 144, 255);
    const Color Cornflowerblue(100, 149, 237);

    // Green shades
    const Color Darkgreen(0, 100, 0);
    const Color Forestgreen(34, 139, 34);
    const Color Seagreen(46, 139, 87);
    const Color Limegreen(50, 205, 50);
    const Color Springgreen(0, 255, 127);
    const Color Lime(0, 255, 0);

    // Purple shades
    const Color Indigo(75, 0, 130);
    const Color Purple(128, 0, 128);
    const Color Darkmagenta(139, 0, 139);
    const Color Darkviolet(148, 0, 211);
    const Color Darkorchid(153, 50, 204);
    const Color Blueviolet(138, 43, 226);

    // Special colors
    inline constexpr Color DefaultLetterbox{1, 2, 3};
    inline constexpr Color DefaultBackground{11, 22, 33};
}

namespace Input
{
    // C++17対応の安全なボタン状態管理クラス
    class SafeButtonState
    {
    private:
        std::optional<std::reference_wrapper<m5::Button_Class>> m_button;

        // C++17 constexpr if を使った型安全なヘルパー
        template<typename Func>
        constexpr auto invokeIfAvailable(Func&& func) const noexcept {
            if constexpr (std::is_void_v<std::invoke_result_t<Func, m5::Button_Class&>>) {
                if (m_button) {
                    std::invoke(std::forward<Func>(func), m_button->get());
                }
            } else {
                using ReturnType = std::invoke_result_t<Func, m5::Button_Class&>;
                if (m_button) {
                    return std::invoke(std::forward<Func>(func), m_button->get());
                }
                return ReturnType{};
            }
        }

    public:
        // C++17 デダクションガイド対応コンストラクタ（constexpr制約を緩和）
        SafeButtonState() noexcept = default;
        SafeButtonState(m5::Button_Class& btn) noexcept : m_button{std::ref(btn)} {}
        SafeButtonState(m5::Button_Class* btn) noexcept 
            : m_button{btn ? std::optional{std::ref(*btn)} : std::nullopt} {}

        // C++17 nodiscard属性付きの状態取得メソッド（constexpr制約を緩和）
        [[nodiscard]] bool down() const noexcept { 
            return m_button ? m_button->get().isPressed() : false; 
        }
        
        [[nodiscard]] bool up() const noexcept { 
            return m_button ? m_button->get().isReleased() : false; 
        }
        
        [[nodiscard]] bool pressed() const noexcept { 
            return m_button ? m_button->get().wasPressed() : false; 
        }
        
        [[nodiscard]] bool released() const noexcept { 
            return m_button ? m_button->get().wasReleased() : false; 
        }
        
        [[nodiscard]] bool pressedDuration(uint32_t ms) const noexcept { 
            return m_button ? m_button->get().pressedFor(ms) : false; 
        }
        
        [[nodiscard]] bool releasedDuration(uint32_t ms) const noexcept { 
            return m_button ? m_button->get().releasedFor(ms) : false; 
        }

        [[nodiscard]] bool isAvailable() const noexcept {
            return m_button.has_value();
        }

        // C++17 perfect forwarding と constexpr if を使った安全な操作（constexpr制約を緩和）
        template<typename Func>
        [[nodiscard]] auto whenAvailable(Func&& func) const noexcept 
            -> std::invoke_result_t<Func, m5::Button_Class&> {
            return invokeIfAvailable(std::forward<Func>(func));
        }

        // C++17 string_view対応のデバッグ情報
        [[nodiscard]] String getStatusString() const {
            return m_button ? 
                String("Available - Pressed: ") + (m_button->get().isPressed() ? "Yes" : "No") :
                String("Unavailable");
        }
    };

         /*
      * 使用例（OpenSiv3Dスタイル準拠）:
      * 
      * // 基本的な使用方法
      * if (Input::ButtonA.pressed()) {
      *     Print << "Button A is being held down";
      * }
      * 
      * if (Input::ButtonA.down()) {
      *     Print << "Button A was just pressed";
      * }
      * 
      * if (Input::ButtonA.up()) {
      *     Print << "Button A was just released";
      * }
      * 
      * // 長押し検出
      * if (Input::ButtonA.pressedDuration(1000)) {
      *     Print << "Button A held for 1 second";
      * }
      * 
      * // M5DialでButtonB/Cを安全に使用
      * if (Input::ButtonB.isAvailable()) {
      *     if (Input::ButtonB.pressed()) {
      *         Print << "Button B is available and pressed";
      *     }
      * } else {
      *     Print << "Button B is not available on this device";
      * }
      * 
      * // ラムダを使った安全な操作
      * Input::ButtonC.whenAvailable([](auto& btn) {
      *     if (btn.wasPressed()) {
      *         Print << "Button C pressed safely!";
      *     }
      * });
      * 
      * // デバッグ情報表示
      * Print << "Button A: " << Input::ButtonA.getStatusString();
      * Print << "Button B: " << Input::ButtonB.getStatusString();
      * Print << "Button C: " << Input::ButtonC.getStatusString();
      */

    // 後方互換性のため、旧ButtonStateも残す（deprecated）
    using ButtonState = SafeButtonState;

    // C++17 constexpr if を使ったエレガントなボタンステート管理
    template<int ButtonId>
    [[nodiscard]] auto& getButton() noexcept {
        if constexpr (ButtonId == 0) {
            static SafeButtonState instance{M5RealUnified::getButtonA()};
        return instance; 
        } else if constexpr (ButtonId == 1) {
#ifdef USE_M5_DIAL
            static SafeButtonState instance{}; // M5Dialでは無効
#else
            static SafeButtonState instance{M5.BtnB};
#endif
        return instance; 
        } else if constexpr (ButtonId == 2) {
#ifdef USE_M5_DIAL
            static SafeButtonState instance{}; // M5Dialでは無効
#else
            static SafeButtonState instance{M5.BtnC};
#endif
        return instance; 
        }
    }
    
    // 後方互換性のための関数
    [[nodiscard]] inline auto& getButtonA() noexcept { return getButton<0>(); }
    [[nodiscard]] inline auto& getButtonB() noexcept { return getButton<1>(); }
    [[nodiscard]] inline auto& getButtonC() noexcept { return getButton<2>(); }
    
    #define ButtonA getButtonA()
    #define ButtonB getButtonB() 
    #define ButtonC getButtonC()

    class IMU
    {
    public:
        // C++17 構造化束縛対応の角度構造体
        struct EulerAngles {
            float roll{0.0f};    // X軸周りの回転（横回転）
            float pitch{0.0f};   // Y軸周りの回転（縦回転）
            float yaw{0.0f};     // Z軸周りの回転（水平回転）
            
            // C++17 構造化束縛サポート
            template<std::size_t N>
            [[nodiscard]] constexpr auto& get() const noexcept {
                if constexpr (N == 0) return roll;
                else if constexpr (N == 1) return pitch;
                else if constexpr (N == 2) return yaw;
            }
        };

        [[nodiscard]] static IMU& getInstance() noexcept {
            static IMU instance;
            return instance;
        }

        // C++17 nodiscard属性付きの角度取得
        [[nodiscard]] const EulerAngles& getAngles(float deltaTime) noexcept {
            updateAttitude(deltaTime);
            return m_currentAngles;
        }

        // C++17 デフォルト引数とconstexpr値を使った姿勢更新
        void updateAttitude(float deltaTime, float alpha = 0.96f, float gyroScale = 1.0f) noexcept {
            const auto [accel_x, accel_y, accel_z] = getAccel();  // 構造化束縛
            const auto [gyro_x, gyro_y, gyro_z] = getGyro();     // 構造化束縛

            // C++17 constexpr 数学定数を使用
            constexpr float rad_to_deg = 180.0f / Math::Pi;
            
            // 加速度からの角度計算（構造化束縛を活用）
            const float accelPitch = atan2f(-accel_x, sqrtf(accel_y * accel_y + accel_z * accel_z)) * rad_to_deg;
            const float accelRoll = atan2f(accel_y, accel_z) * rad_to_deg;
            const float accelYaw = atan2f(accel_x, accel_y) * rad_to_deg;

            // 相補フィルタを各軸に適用（構造化束縛で代入）
            auto& [roll, pitch, yaw] = m_currentAngles;
            roll = complementaryFilter(accelRoll, gyro_x, deltaTime, roll, alpha, gyroScale);
            pitch = complementaryFilter(accelPitch, gyro_y, deltaTime, pitch, alpha, gyroScale);
            yaw = complementaryFilter(accelYaw, gyro_z, deltaTime, yaw, alpha, gyroScale);
        }

        /*
         * M5AtomS3のIMU座標系:
         * - Z軸: 画面に垂直な方向（画面裏側が正）
         * - Y軸: デバイスの下方向が正
         * - X軸: デバイスの左方向が正（右向きが負）
         * 
         * 重力加速度の値は約1G（≒9.8m/s²）
         * 例：
         * - デバイスを水平に置いた場合（画面上向き）: (0, 0, +1G)
         * - デバイスを垂直に立てた場合（ケーブルが下向き）: (0, +1G, 0)
         * - デバイスを右に傾けた場合: (-1G, 0, 0)
         */

        // C++17 構造化束縛対応の加速度取得 (G)
        [[nodiscard]] Math::Vec3f getAccel() const noexcept {
            const auto data = M5.Imu.getImuData();
            return {data.accel.x, data.accel.y, data.accel.z};
        }

        // C++17 構造化束縛対応の角速度取得 (deg/s)
        [[nodiscard]] Math::Vec3f getGyro() const noexcept {
            const auto data = M5.Imu.getImuData();
            return {data.gyro.x, data.gyro.y, data.gyro.z};
        }

        // C++17 構造化束縛対応の地磁気取得 (μT)
        [[nodiscard]] Math::Vec3f getMag() const noexcept {
            const auto data = M5.Imu.getImuData();
            return {data.mag.x, data.mag.y, data.mag.z};
        }

        // ... rest of IMU implementation ...

    private:
        IMU() noexcept = default;

        // C++17 相補フィルタのヘルパー関数（constexpr制約を緩和）
        [[nodiscard]] float complementaryFilter(
            float accelAngle, float gyroRate, float deltaTime, 
            float currentAngle, float alpha, float gyroScale) const noexcept {
            const float gyroAngle = currentAngle + gyroRate * gyroScale * deltaTime;
            return alpha * gyroAngle + (1.0f - alpha) * accelAngle;
        }

        EulerAngles m_currentAngles{};  // C++17 デフォルト初期化
    };

    // C++17 nodiscard属性付きグローバルIMUインスタンス
    [[nodiscard]] inline IMU& getIMU() noexcept { return IMU::getInstance(); }
    #define IMU getIMU()

    // C++17対応のタッチ入力管理クラス
    class TouchInput
    {
    public:
        [[nodiscard]] static TouchInput& getInstance() noexcept {
            static TouchInput instance;
            return instance;
        }

        // C++17 構造化束縛対応のタッチ状態更新
        void update() noexcept {
            m_previousTouchState = m_currentTouchState;
            if (M5.Touch.isEnabled()) {
                const auto touchDetail = M5.Touch.getDetail();
                m_currentTouchState = {
                    .x = touchDetail.x,
                    .y = touchDetail.y,
                    .pressed = touchDetail.isPressed()
                };
            }
        }

        // C++17 nodiscard属性付きの位置取得（constexpr制約を緩和）
        [[nodiscard]] Math::Vec2i pos() const noexcept {
            return {m_currentTouchState.x, m_currentTouchState.y};
        }

        // C++17 nodiscard属性付きの状態取得（constexpr制約を緩和）
        [[nodiscard]] bool pressed() const noexcept {
            return m_currentTouchState.pressed;
        }

        [[nodiscard]] bool down() const noexcept {
            return m_currentTouchState.pressed && !m_previousTouchState.pressed;
        }

        [[nodiscard]] bool up() const noexcept {
            return !m_currentTouchState.pressed && m_previousTouchState.pressed;
        }

    private:
        TouchInput() noexcept = default;

        // C++17 指定初期化子対応の構造体
        struct TouchState {
            int32_t x{0};
            int32_t y{0};
            bool pressed{false};
        };

        TouchState m_currentTouchState{};
        TouchState m_previousTouchState{};
    };

    // C++17 nodiscard属性付きグローバルタッチ入力インスタンス
    [[nodiscard]] inline TouchInput& getTouch() noexcept { return TouchInput::getInstance(); }
    #define Touch getTouch()

    // =============================================================================
    // M5Dial Specific Features - Encoder and RFID Support
    // =============================================================================

#ifdef USE_M5_DIAL
    // M5Dialのエンコーダー機能を安全にラップ
    class SafeDialEncoder
    {
    public:
        static SafeDialEncoder& getInstance()
        {
            static SafeDialEncoder instance;
            return instance;
        }

        // 基本的なM5Dial API（ESP32安全版 - 例外なし）
        std::optional<long> read() const { 
            if (!isAvailable() || !isHardwareReady()) return std::nullopt;
            
            // ESP32では例外を使わず、直接ハードウェアの状態をチェック
            auto& encoder = M5RealUnified::getEncoder();
            long value = encoder.read();
            
            // 値の妥当性チェック（ESP32の制約内）
            if (value < LONG_MIN + 1000 || value > LONG_MAX - 1000) {
                return std::nullopt;  // 異常値の場合
            }
            
            return value;
        }
        
        bool write(long value) { 
            if (!isAvailable() || !isHardwareReady()) return false;
            
            // 値の範囲チェック
            if (value < LONG_MIN + 1000 || value > LONG_MAX - 1000) {
                return false;
            }
            
            auto& encoder = M5RealUnified::getEncoder();
            encoder.write(value);
            return true;  // M5Dialのwrite()は通常失敗しない
        }
        
        std::optional<long> readAndReset() { 
            if (!isAvailable() || !isHardwareReady()) return std::nullopt;
            
            auto& encoder = M5RealUnified::getEncoder();
            long result = encoder.readAndReset();
            
            // 値の妥当性チェック
            if (result < LONG_MIN + 1000 || result > LONG_MAX - 1000) {
                return std::nullopt;
            }
            
            m_previousValue = 0; // リセット後は前回値も0にする
            return result;
        }

        // Siv3D風の拡張API（ユーザビリティ向上）
        std::optional<long> getValue() const { return read(); }
        bool setValue(long value) { return write(value); }
        bool reset() { 
            auto result = readAndReset();
            return result.has_value();
        }

        // 安全な変化量取得（データ競合を防止）
        std::optional<long> getDelta() 
        {
            auto current = read();
            if (!current.has_value()) return std::nullopt;
            
            long delta = current.value() - m_previousValue;
            m_previousValue = current.value();
            return delta;
        }

        // 変化があったかどうか（getDelta()と独立）
        bool changed() const
        {
            auto current = read();
            if (!current.has_value()) return false;
            return current.value() != m_previousValue;
        }

        // エンコーダーが利用可能かチェック
        bool isAvailable() const {
            return true; // M5Dialでは常に利用可能
        }

        // 現在の値を安全に取得（デフォルト値付き）
        long getValueOr(long defaultValue = 0) const {
            auto value = getValue();
            return value.value_or(defaultValue);
        }

        // デバッグ用
        String getStatusString() const {
            auto value = getValue();
            if (!value.has_value()) {
                return "Encoder: Error reading value";
            }
            return String("Encoder: ") + String(value.value());
        }

        // 更新処理（InputManagerから呼び出される）
        void update()
        {
            // エンコーダーのハードウェア状態を定期的にチェック
            if (!isHardwareReady()) {
                m_hardwareErrorCount++;
                if (m_hardwareErrorCount > 100) {
                    // 連続エラーが多い場合はリセット
                    m_hardwareErrorCount = 0;
                    m_previousValue = 0;
                }
            } else {
                m_hardwareErrorCount = 0;
            }
        }

    private:
        SafeDialEncoder() = default;
        mutable long m_previousValue = 0; // mutableにしてconst関数からも更新可能
        mutable uint32_t m_hardwareErrorCount = 0; // ハードウェアエラーのカウント
        
        // ESP32でのハードウェア準備状態チェック（例外なし）
        bool isHardwareReady() const {
            // M5Dialが適切に初期化されているかチェック
#ifdef USE_M5_DIAL
            // M5Dialの基本的な状態をチェック
            if (!M5Dial.Display.width() || !M5Dial.Display.height()) {
                return false;  // ディスプレイが初期化されていない
            }
            
            // エンコーダーが物理的に接続されているかの簡易チェック
            // （実際のハードウェア依存の実装が必要）
            return true;
#else
            return false;  // M5Dial以外では利用不可
#endif
        }
    };

    // M5DialのRFID機能をSiv3D風にラップ（安全版）
    class SafeDialRFID
    {
    public:
        static SafeDialRFID& getInstance()
        {
            static SafeDialRFID instance;
            return instance;
        }

        // カードが検出されているかチェック（ESP32安全版 - 例外なし）
        bool isCardPresent() const
        {
            if (!isAvailable() || !isHardwareReady()) return false;
            
            // ESP32でのRFIDハードウェア状態チェック
#ifdef USE_M5_DIAL
            // M5DialのRFIDが適切に動作しているかチェック
            // ハードウェアの基本的な応答確認
            // TODO: 実際のMFRC522ステータスレジスタをチェック
            return false;  // 現在は未実装のためfalse
#else
            return false;
#endif
        }

        // カードのUIDを読み取り（ESP32安全版）
        std::optional<String> readCardUID() const
        {
            if (!isCardPresent()) return std::nullopt;
            
            // ESP32での安全なUID読み取り
#ifdef USE_M5_DIAL
            // バッファオーバーフローを防ぐためのサイズ制限
            constexpr uint8_t MAX_UID_SIZE = 16;
            uint8_t uidBuffer[MAX_UID_SIZE];
            uint8_t uidSize = 0;
            
            // TODO: 実際のM5Dial RFID APIでUID取得
            // M5RealUnified::getRfid().readUID(uidBuffer, &uidSize);
            
            if (uidSize == 0 || uidSize > MAX_UID_SIZE) {
                return std::nullopt;  // 無効なサイズ
            }
            
            // UIDを安全に文字列に変換
            String uidString = "";
            for (uint8_t i = 0; i < uidSize; i++) {
                if (i > 0) uidString += ":";
                if (uidBuffer[i] < 0x10) uidString += "0";
                uidString += String(uidBuffer[i], 16);
            }
            
            return uidString;
#else
            return std::nullopt;
#endif
        }

        // カードデータを読み取り（ESP32安全版）
        bool readCardData(uint8_t blockAddr, uint8_t* buffer, uint8_t bufferSize) const
        {
            if (!buffer || bufferSize == 0 || bufferSize > 64) return false;  // バッファサイズ制限
            if (!isCardPresent()) return false;
            
            // アドレス範囲チェック（MIFARE Classic制約）
            if (blockAddr > 63) return false;
            
#ifdef USE_M5_DIAL
            // TODO: 実際のM5Dial RFID読み取りAPI
            // return M5RealUnified::getRfid().readBlock(blockAddr, buffer, bufferSize);
            return false;  // 現在は未実装
#else
            return false;
#endif
        }

        // カードデータを書き込み（ESP32安全版）
        bool writeCardData(uint8_t blockAddr, const uint8_t* buffer, uint8_t bufferSize)
        {
            if (!buffer || bufferSize == 0 || bufferSize > 16) return false;  // MIFARE制限
            if (!isCardPresent()) return false;
            
            // アドレス範囲と書き込み可能性をチェック
            if (blockAddr > 63 || (blockAddr % 4) == 3) return false;  // トレーラーブロック禁止
            
#ifdef USE_M5_DIAL
            // TODO: 実際のM5Dial RFID書き込みAPI
            // return M5RealUnified::getRfid().writeBlock(blockAddr, buffer, bufferSize);
            return false;  // 現在は未実装
#else
            return false;
#endif
        }

        // RFIDが利用可能かチェック
        bool isAvailable() const {
            return true; // M5Dialでは常に利用可能
        }

        // デバッグ用
        String getStatusString() const {
            return String("RFID: ") + (isCardPresent() ? "Card Present" : "No Card");
        }

        // 更新処理（InputManagerから呼び出される）
        void update()
        {
            // RFIDハードウェアの状態を定期的にチェック
            if (!isHardwareReady()) {
                m_hardwareErrorCount++;
            } else {
                m_hardwareErrorCount = 0;
            }
        }

    private:
        SafeDialRFID() = default;
        mutable uint32_t m_hardwareErrorCount = 0;
        
        // ESP32でのRFIDハードウェア準備状態チェック（例外なし）
        bool isHardwareReady() const {
#ifdef USE_M5_DIAL
            // M5DialのRFIDモジュールが適切に初期化されているかチェック
            // SPI通信が正常に動作しているかの基本確認
            if (m_hardwareErrorCount > 50) {
                return false;  // 連続エラーが多い場合は無効
            }
            
            // TODO: 実際のMFRC522レジスタ読み取りでヘルスチェック
            // 現在は基本的なチェックのみ
            return true;
#else
            return false;  // M5Dial以外では利用不可
#endif
        }
    };

    // グローバルなM5Dial機能インスタンス
    SafeDialEncoder& getDialEncoder() { return SafeDialEncoder::getInstance(); }
    SafeDialRFID& getDialRFID() { return SafeDialRFID::getInstance(); }
    
    #define Encoder getDialEncoder()
    #define RFID getDialRFID()

#else
    // M5Dial以外のデバイス用の安全なダミークラス
    class SafeDummyEncoder
    {
    public:
        static SafeDummyEncoder& getInstance() { static SafeDummyEncoder instance; return instance; }
        
        std::optional<long> read() const { return std::nullopt; }
        bool write(long) { return false; }
        std::optional<long> readAndReset() { return std::nullopt; }
        std::optional<long> getValue() const { return std::nullopt; }
        bool setValue(long) { return false; }
        bool reset() { return false; }
        std::optional<long> getDelta() { return std::nullopt; }
        bool changed() const { return false; }
        bool isAvailable() const { return false; }
        long getValueOr(long defaultValue = 0) const { return defaultValue; }
        String getStatusString() const { return "Encoder: Not available on this device"; }
        void update() {}
    };

    class SafeDummyRFID
    {
    public:
        static SafeDummyRFID& getInstance() { static SafeDummyRFID instance; return instance; }
        
        bool isCardPresent() const { return false; }
        std::optional<String> readCardUID() const { return std::nullopt; }
        bool readCardData(uint8_t, uint8_t*, uint8_t) const { return false; }
        bool writeCardData(uint8_t, const uint8_t*, uint8_t) { return false; }
        bool isAvailable() const { return false; }
        String getStatusString() const { return "RFID: Not available on this device"; }
        void update() {}
    };

    // ダミーインスタンス（M5Dial以外では機能しない）
    SafeDummyEncoder& getDummyEncoder() { return SafeDummyEncoder::getInstance(); }
    SafeDummyRFID& getDummyRFID() { return SafeDummyRFID::getInstance(); }
    
    #define Encoder getDummyEncoder()
    #define RFID getDummyRFID()

#endif

    /*
     * 安全なエンコーダー使用例:
     * 
     * // 基本的な使用方法
     * if (Input::Encoder.isAvailable()) {
     *     if (auto value = Input::Encoder.getValue()) {
     *         Print << "Encoder value: " << value.value();
     *     }
     * }
     * 
     * // デフォルト値付きで安全に取得
     * long encoderValue = Input::Encoder.getValueOr(0);
     * 
     * // 変化量を安全に取得
     * if (auto delta = Input::Encoder.getDelta()) {
     *     if (delta.value() != 0) {
     *         Print << "Encoder changed by: " << delta.value();
     *     }
     * }
     * 
     * // RFID使用例
     * if (Input::RFID.isAvailable() && Input::RFID.isCardPresent()) {
     *     if (auto uid = Input::RFID.readCardUID()) {
     *         Print << "Card UID: " << uid.value();
     *     }
     * }
     * 
     * // デバッグ情報
     * Print << Input::Encoder.getStatusString();
     * Print << Input::RFID.getStatusString();
     */

  class InputManager
    {
    public:
        static InputManager &getInstance()
        {
            static InputManager instance;
            return instance;
        }

        // 実装は後で定義（統一感のあるパターン）
        void update();

    private:
        InputManager() = default;
    };
}



class System
{
public:
    // シングルトンインスタンスの取得
    static System &getInstance()
    {
        static System instance;
        return instance;
    }

    // システムの初期化
    void init()
    {
#ifdef USE_M5_DIAL
        // M5Dial専用初期化 - エンコーダーとRFIDを有効化
        M5Dial.begin(true, true);  // enableEncoder=true, enableRFID=true
#else
        auto cfg = M5RealUnified::config();
        M5RealUnified::begin(cfg);
#endif

        // キャンバスを画面のサイズで初期化
        canvas.createSprite(M5RealUnified::getDisplay().width(), M5RealUnified::getDisplay().height());
        canvas.setTextSize(2);
        lastDrawTime = millis();

        // オーディオシステムの初期化は遅延実行される
    }

    static void Init()
    {
        getInstance().init();
    }

    // 描画の開始
    void beginDraw()
    {
        canvas.fillSprite(m_backgroundColor.toRGB565());
    }

    // 描画の終了と画面更新
    void endDraw()
    {
        canvas.pushSprite(0, 0);
        // Print出力の描画（遅延初期化パターン）
        drawPrintOutput();
    }

    static void SetBackgroundColor(const Color &color)
    {
        getInstance().setBackgroundColor(color);
    }

    void setBackgroundColor(const Color &color)
    {
        m_backgroundColor = color;
    }

    static bool Update()
    {
        return getInstance().update();
    }

    // メインループの更新処理
    bool update()
    {
        uint32_t currentTime = millis();
        uint32_t elapsedTime = currentTime - lastDrawTime;

        if (elapsedTime >= FRAME_INTERVAL)
        {
            endDraw();
            updateTime();
            
            // システム全体の更新処理
            updateSubsystems();
            
            lastDrawTime = currentTime;

            int32_t remaining = FRAME_INTERVAL - (millis() - currentTime);
            if (remaining > 0)
            {
                M5RealUnified::delay(remaining);
            }

            beginDraw();
        }
        return true;
    }

    // キャンバスへのアクセス
    M5Canvas &getCanvas()
    {
        return canvas;
    }

    // 画面の幅と高さの取得
    static int Width()
    {
        return getInstance().getWidth();
    }

    static int Height()
    {
        return getInstance().getHeight();
    }

    int getWidth() const { return M5RealUnified::getDisplay().width(); }
    int getHeight() const { return M5RealUnified::getDisplay().height(); }

    // 時間管理関連のメソッドを追加

    static float DeltaTime()
    {
        return getInstance().getDeltaTime();
    }

    static float FPS()
    {
        return getInstance().getFPS();
    }

    static uint64_t FrameCount()
    {
        return getInstance().getFrameCount();
    }

    float getDeltaTime() const { return m_deltaTime; }
    float getFPS() const { return 1000.0f / m_averageFrameTime; }
    uint64_t getFrameCount() const { return m_frameCount; }

    // 経過時間を秒単位で取得
    double getElapsedTimeS() const
    {
        return millis() / 1000.0;
    }

    double getElapsedTimeMS() const
    {
        return millis();
    }

private:

    System()
    {
    } // プライベートコンストラクタ

    // システム変数
    static constexpr int FRAME_INTERVAL = 16; // 約60FPS
    uint32_t lastDrawTime = 0;
    M5Canvas canvas{&M5RealUnified::getDisplay()};

    // コピー禁止
    System(const System &) = delete;
    System &operator=(const System &) = delete;

    Color m_backgroundColor = Palette::Black;

    // 時間管理用メンバ変数
    float m_deltaTime = 0.0f;
    float m_averageFrameTime = FRAME_INTERVAL;
    uint64_t m_frameCount = 0;
    uint32_t m_previousTime = 0;

    // Update内で呼び出す時間更新処理
    void updateTime()
    {
        uint32_t currentTime = millis();
        m_deltaTime = (currentTime - m_previousTime) / 1000.0f;

        // 移動平均でFPSを計算
        m_averageFrameTime = m_averageFrameTime * 0.9f + (currentTime - m_previousTime) * 0.1f;

        m_previousTime = currentTime;
        m_frameCount++;
    }
    
    // C++17 システム全体のサブシステム更新処理（実装は後で定義）
    void updateSubsystems() noexcept;
    
    // Print出力描画（実装は後で定義）
    void drawPrintOutput() noexcept;
};


class PrintManager {
private:
    std::stringstream m_buffer;
    int32_t m_cursorX = 0;
    int32_t m_cursorY = 0;

public:
    static PrintManager& getInstance() {
        static PrintManager instance;
        return instance;
    }

    template <typename T>
    PrintManager& operator<<(const T& value) {
        m_buffer << value << '\n';  // 常に改行を追加
        return *this;
    }

    void clear() {
        m_buffer.str("");
        m_buffer.clear();
        m_cursorX = 0;
        m_cursorY = 0;
    }

    void draw() {
        if (m_buffer.str().empty()) return;

        auto& canvas = System::getInstance().getCanvas();
        canvas.setCursor(m_cursorX, m_cursorY);
        canvas.print(m_buffer.str().c_str());
    }

private:
    PrintManager() = default;
};

// グローバル関数として定義
PrintManager& getPrint() { return PrintManager::getInstance(); }
#define Print getPrint()

void ClearPrint() {
    getPrint().clear();
}

// 注意：前方宣言を削除し、シングルトンパターンで統一しました。
// PrintManager::getInstance().draw() を直接使用
// Audio更新は遅延初期化パターンで実装

// Print出力のグローバル関数（後方互換性のため）
void drawPrint() {
    PrintManager::getInstance().draw();
}


struct Circle
{
    int32_t m_x;
    int32_t m_y;
    int32_t m_r;

    // 既存のコンストラクタ
    Circle(int32_t x, int32_t y, int32_t r) : m_x(x), m_y(y), m_r(r)
    {
    }

    // Vec2iを使用するコンストラクタ
    Circle(const Math::Vec2i& center, int32_t r)
        : m_x(center.x), m_y(center.y), m_r(r)
    {
    }

    // Vec2fを使用するコンストラクタ
    Circle(const Math::Vec2f& center, int32_t r)
        : m_x(static_cast<int32_t>(center.x)), 
          m_y(static_cast<int32_t>(center.y)), 
          m_r(r)
    {
    }

    void draw(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().fillCircle(m_x, m_y, m_r, color.toRGB565());
    }

    void drawFrame(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().drawCircle(m_x, m_y, m_r, color.toRGB565());
    }

    void drawArc(int32_t thickness, int32_t startAngle, int32_t endAngle, const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().drawArc(m_x, m_y, m_r, thickness, startAngle, endAngle, color.toRGB565());
    }

    void fillArc(int32_t thickness, int32_t startAngle, int32_t endAngle, const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().fillArc(m_x, m_y, m_r, thickness, startAngle, endAngle, color.toRGB565());
    }

    // 点が円内にあるかどうかをチェック
    bool contains(const Math::Vec2i& point) const
    {
        int32_t dx = point.x - m_x;
        int32_t dy = point.y - m_y;
        return (dx * dx + dy * dy) <= (m_r * m_r);
    }

    // タッチ位置が円内にあるかどうか
    bool touchOver() const
    {
        return contains(Input::Touch.pos());
    }

    // タッチが開始されたかどうか
    bool touched() const
    {
        return Input::Touch.down() && contains(Input::Touch.pos());
    }

    // タッチが離されたかどうか
    bool released() const
    {
        return Input::Touch.up() && contains(Input::Touch.pos());
    }

    // 継続的なタッチ判定
    bool pressed() const
    {
        return Input::Touch.pressed() && contains(Input::Touch.pos());
    }
};

struct Rect
{
    int32_t m_x;
    int32_t m_y;
    int32_t m_width;
    int32_t m_height;

    // 既存のコンストラクタ
    Rect(int32_t x, int32_t y, int32_t width, int32_t height) 
        : m_x(x), m_y(y), m_width(width), m_height(height)
    {
    }

    // Vec2iを使用するコンストラクタ（位置とサイズ）
    Rect(const Math::Vec2i& pos, const Math::Vec2i& size)
        : m_x(pos.x), m_y(pos.y), 
          m_width(size.x), m_height(size.y)
    {
    }

    // Vec2fを使用するコンストラクタ（位置とサイズ）
    Rect(const Math::Vec2f& pos, const Math::Vec2f& size)
        : m_x(static_cast<int32_t>(pos.x)), 
          m_y(static_cast<int32_t>(pos.y)),
          m_width(static_cast<int32_t>(size.x)), 
          m_height(static_cast<int32_t>(size.y))
    {
    }

    // Vec2iを位置指定に使用するコンストラクタ
    Rect(const Math::Vec2i& pos, int32_t width, int32_t height)
        : m_x(pos.x), m_y(pos.y), 
          m_width(width), m_height(height)
    {
    }

    // Vec2fを位置指定に使用するコンストラクタ
    Rect(const Math::Vec2f& pos, int32_t width, int32_t height)
        : m_x(static_cast<int32_t>(pos.x)), 
          m_y(static_cast<int32_t>(pos.y)),
          m_width(width), m_height(height)
    {
    }

    // 既存のメソッド
    void draw(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().fillRect(m_x, m_y, m_width, m_height, color.toRGB565());
    }

    void drawFrame(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().drawRect(m_x, m_y, m_width, m_height, color.toRGB565());
    }

    void drawRoundFrame(int32_t radius, const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().drawRoundRect(m_x, m_y, m_width, m_height, radius, color.toRGB565());
    }

    void drawRound(int32_t radius, const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().fillRoundRect(m_x, m_y, m_width, m_height, radius, color.toRGB565());
    }

    // 点が矩形内にあるかどうかをチェック
    bool contains(const Math::Vec2i& point) const
    {
        return point.x >= m_x && point.x < (m_x + m_width) &&
               point.y >= m_y && point.y < (m_y + m_height);
    }

    // タッチ位置が矩形内にあるかどうか
    bool touchOver() const
    {
        return contains(Input::Touch.pos());
    }

    // タッチが開始されたかどうか
    bool touched() const
    {
        return Input::Touch.down() && contains(Input::Touch.pos());
    }

    // タッチが離されたかどうか
    bool released() const
    {
        return Input::Touch.up() && contains(Input::Touch.pos());
    }

    // 継続的なタッチ判定
    bool pressed() const
    {
        return Input::Touch.pressed() && contains(Input::Touch.pos());
    }
};

struct Triangle
{
    int32_t m_x1, m_y1, m_x2, m_y2, m_x3, m_y3;

    // 既存のコンストラクタ
    Triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3) 
        : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2), m_x3(x3), m_y3(y3)
    {
    }

    // Vec2iを使用するコンストラクタ
    Triangle(const Math::Vec2i& p1, const Math::Vec2i& p2, const Math::Vec2i& p3)
        : m_x1(p1.x), m_y1(p1.y), 
          m_x2(p2.x), m_y2(p2.y), 
          m_x3(p3.x), m_y3(p3.y)
    {
    }

    // Vec2fを使用するコンストラクタ
    Triangle(const Math::Vec2f& p1, const Math::Vec2f& p2, const Math::Vec2f& p3)
        : m_x1(static_cast<int32_t>(p1.x)), m_y1(static_cast<int32_t>(p1.y)),
          m_x2(static_cast<int32_t>(p2.x)), m_y2(static_cast<int32_t>(p2.y)),
          m_x3(static_cast<int32_t>(p3.x)), m_y3(static_cast<int32_t>(p3.y))
    {
    }

    void draw(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().fillTriangle(m_x1, m_y1, m_x2, m_y2, m_x3, m_y3, color.toRGB565());
    }

    void drawFrame(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().drawTriangle(m_x1, m_y1, m_x2, m_y2, m_x3, m_y3, color.toRGB565());
    }

    // 点が三角形内にあるかどうかをチェック
    bool contains(const Math::Vec2i& point) const
    {
        // 三角形の面積を計算する関数
        auto area = [](int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3) {
            return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0f);
        };

        // 全体の三角形の面積
        float A = area(m_x1, m_y1, m_x2, m_y2, m_x3, m_y3);

        // 点Pと各頂点で作られる3つの三角形の面積
        float A1 = area(point.x, point.y, m_x2, m_y2, m_x3, m_y3);
        float A2 = area(m_x1, m_y1, point.x, point.y, m_x3, m_y3);
        float A3 = area(m_x1, m_y1, m_x2, m_y2, point.x, point.y);

        // 面積の合計が元の三角形とほぼ同じなら内部にある
        return abs(A - (A1 + A2 + A3)) < 0.1f;
    }

    // タッチ位置が三角形内にあるかどうか
    bool touchOver() const
    {
        return contains(Input::Touch.pos());
    }

    // タッチが開始されたかどうか
    bool touched() const
    {
        return Input::Touch.down() && contains(Input::Touch.pos());
    }

    // タッチが離されたかどうか
    bool released() const
    {
        return Input::Touch.up() && contains(Input::Touch.pos());
    }

    // 継続的なタッチ判定
    bool pressed() const
    {
        return Input::Touch.pressed() && contains(Input::Touch.pos());
    }
};

struct Line
{
    int32_t m_x1, m_y1, m_x2, m_y2;

    // 既存のコンストラクタ
    Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) 
        : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2)
    {
    }

    // Vec2を使用するコンストラクタ
    Line(const Math::Vec2i& from, const Math::Vec2i& to)
        : m_x1(from.x), m_y1(from.y), m_x2(to.x), m_y2(to.y)
    {
    }

    // Vec2fからの変換コンストラクタ
    Line(const Math::Vec2f& from, const Math::Vec2f& to)
        : m_x1(static_cast<int32_t>(from.x)), 
          m_y1(static_cast<int32_t>(from.y)),
          m_x2(static_cast<int32_t>(to.x)), 
          m_y2(static_cast<int32_t>(to.y))
    {
    }

    void draw(const Color &color = Color(0, 0, 0))
    {
        System::getInstance().getCanvas().drawLine(m_x1, m_y1, m_x2, m_y2, color.toRGB565());
    }
};

// =============================================================================
// Font System - Easy Font Selection and Management
// =============================================================================

// フォントカテゴリ列挙型
namespace FontCategory
{
    enum class Type : uint8_t
    {
        ASCII,      // 英語・ASCII文字専用
        Japanese,   // 日本語対応
        Decorative, // 装飾フォント
        Monospace   // 等幅フォント
    };

    enum class Weight : uint8_t
    {
        Thin,
        Light,
        Regular,
        Bold
    };

    enum class Style : uint8_t
    {
        Normal,
        Italic,
        Oblique
    };

    enum class Size : uint8_t
    {
        XSmall,  // 8-10px
        Small,   // 12-14px
        Medium,  // 16-18px
        Large,   // 20-24px
        XLarge,  // 28-32px
        XXLarge  // 36px以上
    };
}

// C++17 フォント情報構造体（遅延読み込み対応）
struct FontInfo
{
    using FontGetter = const lgfx::IFont* (*)();  // フォント取得関数ポインタ
    
    FontGetter fontGetter;       // 遅延読み込み用関数ポインタ
    std::string_view name;       // C++17 string_view
    FontCategory::Type category;
    FontCategory::Weight weight;
    FontCategory::Style style;
    FontCategory::Size size;
    uint8_t pixelHeight;
    bool supportsJapanese;

    // C++17 constexpr コンストラクタ（遅延読み込み対応）
    constexpr FontInfo(FontGetter getter, std::string_view n, 
                      FontCategory::Type cat, FontCategory::Weight w, 
                      FontCategory::Style st, FontCategory::Size sz, 
                      uint8_t height, bool japanese = false) noexcept
        : fontGetter(getter), name(n), category(cat), weight(w), style(st), 
          size(sz), pixelHeight(height), supportsJapanese(japanese) {}
          
    // C++17 安全なアクセサ（遅延読み込み）
    [[nodiscard]] const lgfx::IFont& getFont() const noexcept { 
        return *fontGetter();  // 必要な時だけフォントを取得
    }
    
    // ポインタ取得メソッド（遅延読み込み）
    [[nodiscard]] const lgfx::IFont* getFontPtr() const noexcept {
        return fontGetter();  // 必要な時だけフォントを取得
    }
    
    // 実行時安全性チェック
    [[nodiscard]] bool isValid() const noexcept {
        return fontGetter != nullptr;
    }
};

// フォントレジストリ（組み込みフォント一覧）
namespace FontRegistry
{
    // C++17 日本語フォント（遅延読み込み対応）
    namespace Japanese
    {
#if M5SIV3D_ENABLE_JAPANESE_FONTS
        // フォント取得関数（実際に存在するM5GFXフォントを使用）
        inline const lgfx::IFont* getGothic8() { return &fonts::efontJA_10; }  // 8pxは存在しないので10pxを使用
        inline const lgfx::IFont* getGothic12() { return &fonts::efontJA_12; }
        inline const lgfx::IFont* getGothic16() { return &fonts::efontJA_16; }
        inline const lgfx::IFont* getGothic20() { return &fonts::efontJA_24; }  // 20pxは存在しないので24pxを使用
        inline const lgfx::IFont* getGothic24() { return &fonts::efontJA_24; }
        inline const lgfx::IFont* getGothic28() { return &fonts::efontJA_24; }  // 28pxは存在しないので24pxを使用
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getGothic32() { return &fonts::efontJA_24; }  // 32pxは存在しないので24pxを使用
        inline const lgfx::IFont* getGothic36() { return &fonts::efontJA_24; }  // 36pxは存在しないので24pxを使用
        inline const lgfx::IFont* getGothic40() { return &fonts::efontJA_24; }  // 40pxは存在しないので24pxを使用
#endif
        
        // Mincho系は実際に存在するフォントを使用
        inline const lgfx::IFont* getMincho8() { return &fonts::efontJA_10; }  // 8pxは存在しないので10pxを使用
        inline const lgfx::IFont* getMincho12() { return &fonts::efontJA_12; }  // Minchoの代替
        inline const lgfx::IFont* getMincho16() { return &fonts::efontJA_16; }  // Minchoの代替
        inline const lgfx::IFont* getMincho20() { return &fonts::efontJA_24; }  // 20pxは存在しないので24pxを使用
        inline const lgfx::IFont* getMincho24() { return &fonts::efontJA_24; }  // Minchoの代替
        inline const lgfx::IFont* getMincho28() { return &fonts::efontJA_24; }  // Minchoの代替
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getMincho32() { return &fonts::efontJA_24; }  // Minchoの代替（32pxは存在しないので24pxを使用）
        inline const lgfx::IFont* getMincho36() { return &fonts::efontJA_24; }  // Minchoの代替（36pxは存在しないので24pxを使用）
        inline const lgfx::IFont* getMincho40() { return &fonts::efontJA_24; }  // Minchoの代替（40pxは存在しないので24pxを使用）
#endif
#else
        // 日本語フォント無効時のフォールバック
        inline const lgfx::IFont* getGothic16() { return &fonts::Font2; }  // 基本フォントにフォールバック
        inline const lgfx::IFont* getMincho16() { return &fonts::Font2; }  // 基本フォントにフォールバック
#endif

        // FontInfo定義（遅延読み込み対応）
#if M5SIV3D_ENABLE_JAPANESE_FONTS
        inline constexpr FontInfo Gothic8{getGothic8, "Japanese Gothic 8px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XSmall, 8, true};
        inline constexpr FontInfo Gothic12{getGothic12, "Japanese Gothic 12px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Small, 12, true};
        inline constexpr FontInfo Gothic16{getGothic16, "Japanese Gothic 16px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16, true};
        inline constexpr FontInfo Gothic20{getGothic20, "Japanese Gothic 20px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 20, true};
        inline constexpr FontInfo Gothic24{getGothic24, "Japanese Gothic 24px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 24, true};
        inline constexpr FontInfo Gothic28{getGothic28, "Japanese Gothic 28px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 28, true};

#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo Gothic32{getGothic32, "Japanese Gothic 32px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32, true};
        inline constexpr FontInfo Gothic36{getGothic36, "Japanese Gothic 36px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XXLarge, 36, true};
        inline constexpr FontInfo Gothic40{getGothic40, "Japanese Gothic 40px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XXLarge, 40, true};
#endif

        inline constexpr FontInfo Mincho8{getMincho8, "Japanese Mincho 8px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XSmall, 8, true};
        inline constexpr FontInfo Mincho12{getMincho12, "Japanese Mincho 12px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Small, 12, true};
        inline constexpr FontInfo Mincho16{getMincho16, "Japanese Mincho 16px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16, true};
        inline constexpr FontInfo Mincho20{getMincho20, "Japanese Mincho 20px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 20, true};
        inline constexpr FontInfo Mincho24{getMincho24, "Japanese Mincho 24px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 24, true};
        inline constexpr FontInfo Mincho28{getMincho28, "Japanese Mincho 28px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 28, true};

#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo Mincho32{getMincho32, "Japanese Mincho 32px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32, true};
        inline constexpr FontInfo Mincho36{getMincho36, "Japanese Mincho 36px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XXLarge, 36, true};
        inline constexpr FontInfo Mincho40{getMincho40, "Japanese Mincho 40px", 
            FontCategory::Type::Japanese, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XXLarge, 40, true};
#endif

#else
        // 日本語フォント無効時のフォールバック定義
        inline constexpr FontInfo Gothic16{getGothic16, "Japanese Gothic 16px (Fallback)", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16, false};
        inline constexpr FontInfo Mincho16{getMincho16, "Japanese Mincho 16px (Fallback)", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16, false};
#endif
    }

    // C++17 英語フォント（遅延読み込み対応）
    namespace English
    {
        // フォント取得関数（条件付きコンパイル対応）
        // 基本フォント（常に有効）
        inline const lgfx::IFont* getFont0() { return &fonts::Font0; }
        inline const lgfx::IFont* getFont2() { return &fonts::Font2; }
        inline const lgfx::IFont* getFont4() { return &fonts::Font4; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getFont6() { return &fonts::Font6; }
        inline const lgfx::IFont* getFont7() { return &fonts::Font7; }
        inline const lgfx::IFont* getFont8() { return &fonts::Font8; }
#endif
        
        // FreeSans系
        inline const lgfx::IFont* getFreeSans9() { return &fonts::FreeSans9pt7b; }
        inline const lgfx::IFont* getFreeSans12() { return &fonts::FreeSans12pt7b; }
        inline const lgfx::IFont* getFreeSans18() { return &fonts::FreeSans18pt7b; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getFreeSans24() { return &fonts::FreeSans24pt7b; }
#endif
        
#if M5SIV3D_ENABLE_BOLD_FONTS
        inline const lgfx::IFont* getFreeSansBold9() { return &fonts::FreeSansBold9pt7b; }
        inline const lgfx::IFont* getFreeSansBold12() { return &fonts::FreeSansBold12pt7b; }
        inline const lgfx::IFont* getFreeSansBold18() { return &fonts::FreeSansBold18pt7b; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getFreeSansBold24() { return &fonts::FreeSansBold24pt7b; }
#endif
#endif
        
        // FreeMono系（等幅）
        inline const lgfx::IFont* getFreeMono9() { return &fonts::FreeMono9pt7b; }
        inline const lgfx::IFont* getFreeMono12() { return &fonts::FreeMono12pt7b; }
        inline const lgfx::IFont* getFreeMono18() { return &fonts::FreeMono18pt7b; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getFreeMono24() { return &fonts::FreeMono24pt7b; }
#endif
        
#if M5SIV3D_ENABLE_BOLD_FONTS
        inline const lgfx::IFont* getFreeMonoBold9() { return &fonts::FreeMonoBold9pt7b; }
        inline const lgfx::IFont* getFreeMonoBold12() { return &fonts::FreeMonoBold12pt7b; }
        inline const lgfx::IFont* getFreeMonoBold18() { return &fonts::FreeMonoBold18pt7b; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getFreeMonoBold24() { return &fonts::FreeMonoBold24pt7b; }
#endif
#endif
        
        // FreeSerif系
        inline const lgfx::IFont* getFreeSerif9() { return &fonts::FreeSerif9pt7b; }
        inline const lgfx::IFont* getFreeSerif12() { return &fonts::FreeSerif12pt7b; }
        inline const lgfx::IFont* getFreeSerif18() { return &fonts::FreeSerif18pt7b; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getFreeSerif24() { return &fonts::FreeSerif24pt7b; }
#endif
        
        // 極小フォント（常に有効）
        inline const lgfx::IFont* getTomThumb() { return &fonts::TomThumb; }

        // FontInfo定義（遅延読み込み対応）
        // 基本フォント（常に有効）
        inline constexpr FontInfo Font0{getFont0, "Default Font 0", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XSmall, 8, false};
        inline constexpr FontInfo Font2{getFont2, "Default Font 2", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16, false};
        inline constexpr FontInfo Font4{getFont4, "Default Font 4", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 26, false};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo Font6{getFont6, "Default Font 6", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 48, false};
        inline constexpr FontInfo Font7{getFont7, "Default Font 7", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 48, false};
        inline constexpr FontInfo Font8{getFont8, "Default Font 8", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XXLarge, 75, false};
#endif

        // FreeSans系
        inline constexpr FontInfo FreeSans9{getFreeSans9, "FreeSans 9pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Small, 12, false};
        inline constexpr FontInfo FreeSans12{getFreeSans12, "FreeSans 12pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16, false};
        inline constexpr FontInfo FreeSans18{getFreeSans18, "FreeSans 18pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 24, false};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo FreeSans24{getFreeSans24, "FreeSans 24pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
#endif

#if M5SIV3D_ENABLE_BOLD_FONTS
        inline constexpr FontInfo FreeSansBold9{getFreeSansBold9, "FreeSans Bold 9pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::Small, 12};
        inline constexpr FontInfo FreeSansBold12{getFreeSansBold12, "FreeSans Bold 12pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::Medium, 16};
        inline constexpr FontInfo FreeSansBold18{getFreeSansBold18, "FreeSans Bold 18pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo FreeSansBold24{getFreeSansBold24, "FreeSans Bold 24pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
#endif
#endif

        // FreeMono系（等幅）
        inline constexpr FontInfo FreeMono9{getFreeMono9, "FreeMono 9pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Small, 12};
        inline constexpr FontInfo FreeMono12{getFreeMono12, "FreeMono 12pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16};
        inline constexpr FontInfo FreeMono18{getFreeMono18, "FreeMono 18pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo FreeMono24{getFreeMono24, "FreeMono 24pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
#endif

#if M5SIV3D_ENABLE_BOLD_FONTS
        inline constexpr FontInfo FreeMonoBold9{getFreeMonoBold9, "FreeMono Bold 9pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::Small, 12};
        inline constexpr FontInfo FreeMonoBold12{getFreeMonoBold12, "FreeMono Bold 12pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::Medium, 16};
        inline constexpr FontInfo FreeMonoBold18{getFreeMonoBold18, "FreeMono Bold 18pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo FreeMonoBold24{getFreeMonoBold24, "FreeMono Bold 24pt", 
            FontCategory::Type::Monospace, FontCategory::Weight::Bold, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
#endif
#endif

        // FreeSerif系
        inline constexpr FontInfo FreeSerif9{getFreeSerif9, "FreeSerif 9pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Small, 12};
        inline constexpr FontInfo FreeSerif12{getFreeSerif12, "FreeSerif 12pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Medium, 16};
        inline constexpr FontInfo FreeSerif18{getFreeSerif18, "FreeSerif 18pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo FreeSerif24{getFreeSerif24, "FreeSerif 24pt", 
            FontCategory::Type::ASCII, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
#endif

        // 極小フォント（常に有効）
        inline constexpr FontInfo TomThumb{getTomThumb, "TomThumb 6px", 
            FontCategory::Type::ASCII, FontCategory::Weight::Light, FontCategory::Style::Normal, FontCategory::Size::XSmall, 6, false};
    }

    // C++17 装飾フォント（遅延読み込み対応）
    namespace Decorative
    {
        // フォント取得関数（条件付きコンパイル対応）
#if M5SIV3D_ENABLE_DECORATIVE_FONTS
        inline const lgfx::IFont* getOrbitron24() { return &fonts::Orbitron_Light_24; }
        inline const lgfx::IFont* getRoboto24() { return &fonts::Roboto_Thin_24; }
        inline const lgfx::IFont* getSatisfy24() { return &fonts::Satisfy_24; }
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline const lgfx::IFont* getOrbitron32() { return &fonts::Orbitron_Light_32; }
        inline const lgfx::IFont* getYellowtail32() { return &fonts::Yellowtail_32; }
#endif
#endif

        // FontInfo定義（遅延読み込み対応）
#if M5SIV3D_ENABLE_DECORATIVE_FONTS
        inline constexpr FontInfo Orbitron24{getOrbitron24, "Orbitron Light 24px", 
            FontCategory::Type::Decorative, FontCategory::Weight::Light, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        inline constexpr FontInfo Roboto24{getRoboto24, "Roboto Thin 24px", 
            FontCategory::Type::Decorative, FontCategory::Weight::Thin, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        inline constexpr FontInfo Satisfy24{getSatisfy24, "Satisfy 24px", 
            FontCategory::Type::Decorative, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::Large, 24};
        
#if M5SIV3D_ENABLE_LARGE_FONTS
        inline constexpr FontInfo Orbitron32{getOrbitron32, "Orbitron Light 32px", 
            FontCategory::Type::Decorative, FontCategory::Weight::Light, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
        inline constexpr FontInfo Yellowtail32{getYellowtail32, "Yellowtail 32px", 
            FontCategory::Type::Decorative, FontCategory::Weight::Regular, FontCategory::Style::Normal, FontCategory::Size::XLarge, 32};
#endif
#endif
    }

    // C++17 constexpr 全フォント一覧（メモリ最適化対応）
    inline std::vector<const FontInfo*> getAllFonts() {
        std::vector<const FontInfo*> fonts;
        
        // 基本フォント（常に有効）
        fonts.push_back(&English::Font0);
        fonts.push_back(&English::Font2);
        fonts.push_back(&English::Font4);
        fonts.push_back(&English::TomThumb);
        
        // 英語フォント（基本）
        fonts.push_back(&English::FreeSans9);
        fonts.push_back(&English::FreeSans12);
        fonts.push_back(&English::FreeSans18);
        fonts.push_back(&English::FreeMono9);
        fonts.push_back(&English::FreeMono12);
        fonts.push_back(&English::FreeMono18);
        fonts.push_back(&English::FreeSerif9);
        fonts.push_back(&English::FreeSerif12);
        fonts.push_back(&English::FreeSerif18);

#if M5SIV3D_ENABLE_JAPANESE_FONTS
        // 日本語フォント（条件付き）
        fonts.push_back(&Japanese::Gothic16);
        fonts.push_back(&Japanese::Mincho16);
#endif

        return fonts;
    }



    // C++17 カテゴリ別フォント取得（アルゴリズム使用）
    inline std::vector<const FontInfo*> getFontsByCategory(FontCategory::Type category) {
        const auto allFonts = getAllFonts();
        std::vector<const FontInfo*> result;
        
        std::copy_if(allFonts.begin(), allFonts.end(), std::back_inserter(result),
            [category](const FontInfo* font) { 
                return font && font->isValid() && font->category == category; 
            });
        
        return result;
    }

    // C++17 サイズ別フォント取得（アルゴリズム使用）
    inline std::vector<const FontInfo*> getFontsBySize(FontCategory::Size size) {
        const auto allFonts = getAllFonts();
        std::vector<const FontInfo*> result;
        
        std::copy_if(allFonts.begin(), allFonts.end(), std::back_inserter(result),
            [size](const FontInfo* font) { 
                return font && font->isValid() && font->size == size; 
            });
        
        return result;
    }

    // C++17 日本語対応フォント取得（アルゴリズム使用）
    inline std::vector<const FontInfo*> getJapaneseFonts() {
        const auto allFonts = getAllFonts();
        std::vector<const FontInfo*> result;
        
        std::copy_if(allFonts.begin(), allFonts.end(), std::back_inserter(result),
            [](const FontInfo* font) { 
                return font && font->isValid() && font->supportsJapanese; 
            });
        
        return result;
    }
}

// Font構造体の拡張
// C++17 Font構造体（安全な参照ラッパー使用）
struct Font
{
    // 水平方向のテキストアライメント
    enum class HorizontalAlign
    {
        Left,
        Center,
        Right
    };

    // 垂直方向のテキストアライメント
    enum class VerticalAlign
    {
        Top,
        Center,
        Bottom,
        Baseline  // テキストのベースラインに合わせる
    };

    HorizontalAlign hAlign;
    VerticalAlign vAlign;
    const lgfx::IFont* m_fontPtr;  // constexpr対応のためポインタ使用（安全性は設計で保証）
    float m_size = 1.0f;
    const FontInfo* m_fontInfo = nullptr;  // constexpr対応のためポインタ使用

    // C++17 デフォルトコンストラクタ
    Font(const lgfx::IFont &font = fonts::Font0) noexcept
        : m_fontPtr(&font), hAlign(HorizontalAlign::Left), vAlign(VerticalAlign::Baseline) {}

    // C++17 FontInfoからのコンストラクタ
    Font(const FontInfo& fontInfo) noexcept
        : m_fontPtr(fontInfo.fontGetter()), hAlign(HorizontalAlign::Left), vAlign(VerticalAlign::Baseline), 
          m_fontInfo(&fontInfo) {}

    // 水平アライメント設定
    Font &setHorizontalAlign(HorizontalAlign a)
    {
        hAlign = a;
        return *this;
    }

    // 垂直アライメント設定
    Font &setVerticalAlign(VerticalAlign a)
    {
        vAlign = a;
        return *this;
    }

    // サイズ設定メソッドを追加
    Font& setSize(float size)
    {
        m_size = size;
        return *this;
    }

    // C++17 フォント情報取得（nullチェック付き）
    [[nodiscard]] const FontInfo* getFontInfo() const noexcept {
        return m_fontInfo;
    }
    
    // C++17 フォント名取得（安全性チェック付き）
    [[nodiscard]] String getFontName() const {
        return (m_fontInfo && m_fontInfo->isValid()) ? 
            String{m_fontInfo->name.data()} : 
            String{"Unknown Font"};
    }

    // C++17 日本語対応チェック（安全性チェック付き）
    [[nodiscard]] bool supportsJapanese() const noexcept {
        return (m_fontInfo && m_fontInfo->isValid()) ? 
            m_fontInfo->supportsJapanese : false;
    }

    // フォントポインタ取得（デバッグ用）
    [[nodiscard]] const lgfx::IFont* getFontPtr() const noexcept {
        return m_fontPtr;
    }

    // C++17 安全な描画メソッド（nullチェック付き）
    void draw(const String &text, int x, int y, const Color &color = Palette::White)
    {
        // 安全性チェック
        if (!m_fontPtr) return;
        
        auto &canvas = System::getInstance().getCanvas();
        
        // フォントと色を設定
        canvas.setFont(m_fontPtr);  // 日本語フォントを設定
        canvas.setTextColor(color.toRGB565());
        canvas.setTextSize(1);  // テキストサイズは1に固定（フォント自体のサイズを使用）

        // C++17 構造化束縛を使用した位置計算
        const auto [actualX, actualY] = calculateDrawPosition(text, x, y, canvas);

        // 日本語対応の描画メソッドを使用
        canvas.drawString(text, actualX, actualY);
    }

private:
    // C++17 構造化束縛対応の位置計算ヘルパー
    template<typename CanvasType>
    [[nodiscard]] std::pair<int, int> calculateDrawPosition(const String& text, int x, int y, CanvasType& canvas) const {
        int actualX = x;
        int actualY = y;

        // 水平方向のアライメント処理
        if (hAlign != HorizontalAlign::Left) {
            const int w = textWidth(text);
            switch (hAlign) {
                case HorizontalAlign::Center:
                    actualX = x - (w / 2);
                    break;
                case HorizontalAlign::Right:
                    actualX = x - w;
                    break;
                default:
                    break;
            }
        }

        // 垂直方向のアライメント処理
        if (vAlign != VerticalAlign::Baseline) {
            const int h = textHeight();
            switch (vAlign) {
                case VerticalAlign::Center:
                    actualY = y - (h / 2) / 2;
                    break;
                case VerticalAlign::Bottom:
                    actualY = y - h;
                    break;
                default:
                    break;
            }
        }

        return {actualX, actualY};
    }

public:

    // 描画位置を指定するための構造体
    struct Pos
    {
        int x, y;
        Pos(int _x, int _y) : x(_x), y(_y) {}
    };

    // 演算子オーバーロードで簡潔な描画
    void operator()(const String &text, const Pos &pos, const Color &color = Palette::White)
    {
        draw(text, pos.x, pos.y, color);
    }

    // テキストの幅を取得するメソッドを修正
    int textWidth(const String &text) const
    {
        if (!m_fontPtr) return 0;
        auto &canvas = System::getInstance().getCanvas();
        canvas.setFont(m_fontPtr);  // 正しいフォントを設定
        return canvas.textWidth(text);
    }

    // テキストの高さを取得するメソッドを修正
    int textHeight() const
    {
        if (!m_fontPtr) return 0;
        auto &canvas = System::getInstance().getCanvas();
        canvas.setFont(m_fontPtr);  // 正しいフォントを設定
        return canvas.fontHeight();
    }

    // 描画領域を取得
    Rect region(const String &text, int x, int y) const
    {
        return Rect(x, y, textWidth(text), textHeight());
    }

    // 後方互換性のため、TextAlignを残す（非推奨）
    using TextAlign = HorizontalAlign;
    Font &setAlign(TextAlign a)
    {
        return setHorizontalAlign(static_cast<HorizontalAlign>(a));
    }
};

// =============================================================================
// Font Helper Functions - Easy Font Selection
// =============================================================================

namespace FontHelper
{
    // 簡単なフォント選択関数
    
    // 日本語フォント選択（サイズ指定）
    inline Font JapaneseGothic(FontCategory::Size size = FontCategory::Size::Medium) {
#if M5SIV3D_ENABLE_JAPANESE_FONTS
        // 日本語フォントが有効な場合は Gothic16 を使用
        (void)size;  // 未使用変数警告を回避
        return Font(FontRegistry::Japanese::Gothic16);
#else
        // 日本語フォントが無効な場合は英語フォントで代替
        (void)size;  // 未使用変数警告を回避
        return Font(FontRegistry::English::FreeSans12);
#endif
    }
    
    inline Font JapaneseMincho(FontCategory::Size size = FontCategory::Size::Medium) {
#if M5SIV3D_ENABLE_JAPANESE_FONTS
        // 日本語フォントが有効な場合は Mincho16 を使用
        (void)size;  // 未使用変数警告を回避
        return Font(FontRegistry::Japanese::Mincho16);
#else
        // 日本語フォントが無効な場合は英語フォントで代替
        (void)size;  // 未使用変数警告を回避
        return Font(FontRegistry::English::FreeSerif12);
#endif
    }
    
    // 英語フォント選択（メモリ最適化対応）
    inline Font EnglishSans(FontCategory::Size size = FontCategory::Size::Medium, FontCategory::Weight weight = FontCategory::Weight::Regular) {
#if M5SIV3D_ENABLE_BOLD_FONTS
        if (weight == FontCategory::Weight::Bold) {
            switch (size) {
                case FontCategory::Size::Small:  return Font(FontRegistry::English::FreeSansBold9);
                case FontCategory::Size::Medium: return Font(FontRegistry::English::FreeSansBold12);
                case FontCategory::Size::Large:  return Font(FontRegistry::English::FreeSansBold18);
#if M5SIV3D_ENABLE_LARGE_FONTS
                case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeSansBold24);
#endif
                default: return Font(FontRegistry::English::FreeSansBold12);
            }
        } else
#endif
        {
            switch (size) {
                case FontCategory::Size::XSmall: return Font(FontRegistry::English::Font0);
                case FontCategory::Size::Small:  return Font(FontRegistry::English::FreeSans9);
                case FontCategory::Size::Medium: return Font(FontRegistry::English::FreeSans12);
                case FontCategory::Size::Large:  return Font(FontRegistry::English::FreeSans18);
#if M5SIV3D_ENABLE_LARGE_FONTS
                case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeSans24);
#else
                case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeSans18);  // フォールバック
#endif
                default: return Font(FontRegistry::English::FreeSans12);
            }
        }
    }
    
    inline Font EnglishMono(FontCategory::Size size = FontCategory::Size::Medium, FontCategory::Weight weight = FontCategory::Weight::Regular) {
#if M5SIV3D_ENABLE_BOLD_FONTS
        if (weight == FontCategory::Weight::Bold) {
            // Bold版のMonoフォントは定義されていないため、通常版を使用
            switch (size) {
                case FontCategory::Size::Small:  return Font(FontRegistry::English::FreeMono9);
                case FontCategory::Size::Medium: return Font(FontRegistry::English::FreeMono12);
                case FontCategory::Size::Large:  return Font(FontRegistry::English::FreeMono18);
#if M5SIV3D_ENABLE_LARGE_FONTS
                case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeMono18);  // 24pxがない場合は18px
#endif
                default: return Font(FontRegistry::English::FreeMono12);
            }
        } else
#endif
        {
            switch (size) {
                case FontCategory::Size::Small:  return Font(FontRegistry::English::FreeMono9);
                case FontCategory::Size::Medium: return Font(FontRegistry::English::FreeMono12);
                case FontCategory::Size::Large:  return Font(FontRegistry::English::FreeMono18);
#if M5SIV3D_ENABLE_LARGE_FONTS
                case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeMono18);  // 24pxがない場合は18px
#else
                case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeMono18);  // フォールバック
#endif
                default: return Font(FontRegistry::English::FreeMono12);
            }
        }
    }
    
    inline Font EnglishSerif(FontCategory::Size size = FontCategory::Size::Medium) {
        switch (size) {
            case FontCategory::Size::Small:  return Font(FontRegistry::English::FreeSerif9);
            case FontCategory::Size::Medium: return Font(FontRegistry::English::FreeSerif12);
            case FontCategory::Size::Large:  return Font(FontRegistry::English::FreeSerif18);
#if M5SIV3D_ENABLE_LARGE_FONTS
            case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeSerif18);  // 24pxがない場合は18px
#else
            case FontCategory::Size::XLarge: return Font(FontRegistry::English::FreeSerif18);  // フォールバック
#endif
            default: return Font(FontRegistry::English::FreeSerif12);
        }
    }
    
    // 装飾フォント選択（メモリ最適化対応）
    inline Font Decorative(const String& name) {
#if M5SIV3D_ENABLE_DECORATIVE_FONTS
        // 装飾フォントが有効な場合でも、実際のフォントが定義されていない場合は基本フォントを返す
        return Font(FontRegistry::English::FreeSans12);  // フォールバック
#else
        // 装飾フォントが無効化されている場合は基本フォントを返す
        (void)name;  // 未使用変数警告を回避
        return Font(FontRegistry::English::FreeSans12);
#endif
    }
    
    // 極小フォント
    inline Font TinyFont() {
        return Font(FontRegistry::English::TomThumb);
    }
    
    // 最適なフォント自動選択
    inline Font AutoSelect(const String& text, FontCategory::Size preferredSize = FontCategory::Size::Medium) {
        // 日本語文字が含まれているかチェック
        bool hasJapanese = false;
        for (size_t i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            if (static_cast<unsigned char>(c) > 127) {  // ASCII以外の文字
                hasJapanese = true;
                break;
            }
        }
        
        if (hasJapanese) {
            return JapaneseGothic(preferredSize);
        } else {
            return EnglishSans(preferredSize);
        }
    }
    
    // フォント一覧表示用
    inline void PrintAvailableFonts() {
        Print << "=== Available Fonts ===";
        
        Print << "Japanese Fonts:";
        for (const auto* font : FontRegistry::getJapaneseFonts()) {
            Print << "  " << font->name;
        }
        
        Print << "English Fonts:";
        for (const auto* font : FontRegistry::getFontsByCategory(FontCategory::Type::ASCII)) {
            Print << "  " << font->name;
        }
        
        Print << "Monospace Fonts:";
        for (const auto* font : FontRegistry::getFontsByCategory(FontCategory::Type::Monospace)) {
            Print << "  " << font->name;
        }
        
        Print << "Decorative Fonts:";
        for (const auto* font : FontRegistry::getFontsByCategory(FontCategory::Type::Decorative)) {
            Print << "  " << font->name;
        }
    }
}

// グローバル便利関数
inline Font JapaneseFont(FontCategory::Size size = FontCategory::Size::Medium) {
    return FontHelper::JapaneseGothic(size);
}

inline Font EnglishFont(FontCategory::Size size = FontCategory::Size::Medium) {
    return FontHelper::EnglishSans(size);
}

inline Font MonospaceFont(FontCategory::Size size = FontCategory::Size::Medium) {
    return FontHelper::EnglishMono(size);
}

inline Font AutoFont(const String& text, FontCategory::Size size = FontCategory::Size::Medium) {
    return FontHelper::AutoSelect(text, size);
}

struct Bezier
{
    // 3点ベジェ曲線用の構造体
    struct Bezier3
    {
        int32_t x0, y0;  // 開始点
        int32_t x1, y1;  // 制御点
        int32_t x2, y2;  // 終了点

        Bezier3(int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, int32_t _x2, int32_t _y2)
            : x0(_x0), y0(_y0), x1(_x1), y1(_y1), x2(_x2), y2(_y2)
        {
        }

        void draw(const Color &color = Color(0, 0, 0))
        {
            System::getInstance().getCanvas().drawBezier(x0, y0, x1, y1, x2, y2, color.toRGB565());
        }
    };

    // 4点ベジェ曲線用の構造体
    struct Bezier4
    {
        int32_t x0, y0;  // 開始点
        int32_t x1, y1;  // 制御点1
        int32_t x2, y2;  // 制御点2
        int32_t x3, y3;  // 終了点

        Bezier4(int32_t _x0, int32_t _y0, int32_t _x1, int32_t _y1, 
                int32_t _x2, int32_t _y2, int32_t _x3, int32_t _y3)
            : x0(_x0), y0(_y0), x1(_x1), y1(_y1), x2(_x2), y2(_y2), x3(_x3), y3(_y3)
        {
        }

        void draw(const Color &color = Color(0, 0, 0))
        {
            System::getInstance().getCanvas().drawBezier(x0, y0, x1, y1, x2, y2, x3, y3, color.toRGB565());
        }
    };

    // ファクトリーメソッド
    static Bezier3 create3Point(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        return Bezier3(x0, y0, x1, y1, x2, y2);
    }

    static Bezier4 create4Point(int32_t x0, int32_t y0, int32_t x1, int32_t y1, 
                               int32_t x2, int32_t y2, int32_t x3, int32_t y3)
    {
        return Bezier4(x0, y0, x1, y1, x2, y2, x3, y3);
    }
};

class Image {
private:
    std::unique_ptr<M5Canvas> m_canvas;
    mutable std::vector<Color> m_pixels;  // 遅延読み込み用（mutable）
    mutable bool m_pixels_valid = false;  // ピクセルデータが有効かどうか
    mutable bool m_canvas_dirty = false;  // キャンバスが更新されたかどうか
    int32_t m_width = 0;
    int32_t m_height = 0;
    
    // ESP32メモリ安全性のための制限
    static constexpr size_t MAX_IMAGE_WIDTH = 1024;
    static constexpr size_t MAX_IMAGE_HEIGHT = 1024;
    static constexpr size_t MAX_BASE64_SIZE = 1024 * 1024;  // 1MB制限

public:
    // OpenSiv3D互換コンストラクタ
    
    // デフォルトコンストラクタ
    Image() : m_canvas(nullptr), m_width(0), m_height(0) {
        initializeCanvas();
    }
    
    // OpenSiv3D Style: Image{ Size{ width, height }, color }
    Image(const Math::Vec2i& size, const Color& color = Palette::Black)
        : Image(size.x, size.y, color) {}
    
    // OpenSiv3D Style: Image{ int32 width, int32 height, color }
    Image(int32_t width, int32_t height, const Color& color = Palette::Black)
        : m_canvas(nullptr), m_width(0), m_height(0) {
        initializeCanvas();
        create(width, height, color);
    }
    
    // OpenSiv3D Style: Image{ U"filepath" } (Base64エミュレーション)
    explicit Image(const String& data)
        : m_canvas(nullptr), m_width(0), m_height(0) {
        initializeCanvas();
        if (data.startsWith("data:") || data.length() > 100) {
            // Base64データとして処理
            loadBase64(data.c_str());
        }
    }
    
    // ムーブコンストラクタ
    Image(Image&& other) noexcept 
        : m_canvas(std::move(other.m_canvas))
        , m_pixels(std::move(other.m_pixels))
        , m_pixels_valid(other.m_pixels_valid)
        , m_canvas_dirty(other.m_canvas_dirty)
        , m_width(other.m_width)
        , m_height(other.m_height) {
        other.m_width = 0;
        other.m_height = 0;
        other.m_pixels_valid = false;
        other.m_canvas_dirty = false;
    }
    
    // ムーブ代入演算子
    Image& operator=(Image&& other) noexcept {
        if (this != &other) {
            cleanup();
            m_canvas = std::move(other.m_canvas);
            m_pixels = std::move(other.m_pixels);
            m_pixels_valid = other.m_pixels_valid;
            m_canvas_dirty = other.m_canvas_dirty;
            m_width = other.m_width;
            m_height = other.m_height;
            
            other.m_width = 0;
            other.m_height = 0;
            other.m_pixels_valid = false;
            other.m_canvas_dirty = false;
        }
        return *this;
    }
    
    // コピー禁止（リソース管理の明確化）
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    
    // ESP32安全版Base64ローダー（例外なし、メモリリークなし）
    bool loadBase64(const char* base64Data) {
        if (!base64Data || !isCanvasReady()) {
            return false;
        }

        // 入力サイズの安全性チェック
        size_t inputLen = strlen(base64Data);
        if (inputLen == 0 || inputLen > MAX_BASE64_SIZE) {
            return false;
        }

        // 既存リソースのクリーンアップ
        cleanup();
        
        // Base64デコードサイズの計算と検証
        size_t decodedLen = decode_base64_length((unsigned char*)base64Data);
        if (decodedLen == 0 || decodedLen > MAX_BASE64_SIZE) {
            return false;
        }
        
        // std::vectorを使った安全なメモリ管理（RAII）
        std::vector<uint8_t> decodedData(decodedLen);
        size_t actualLen = decode_base64((unsigned char*)base64Data, decodedData.data());
        if (actualLen == 0 || actualLen > decodedData.size()) {
            return false;
        }

        // PNG形式の安全な検証
        if (!validatePngHeader(decodedData, actualLen)) {
            return false;
        }

        // 安全な画像サイズ抽出
        auto dimensions = extractPngDimensions(decodedData);
        if (!dimensions.has_value()) {
            return false;
        }
        
        m_width = dimensions->x;
        m_height = dimensions->y;

        // サイズ制限チェック
        if (m_width <= 0 || m_height <= 0 || 
            static_cast<size_t>(m_width) > MAX_IMAGE_WIDTH || 
            static_cast<size_t>(m_height) > MAX_IMAGE_HEIGHT) {
            return false;
        }

        // キャンバスの安全な作成
        if (!createSafeSprite(m_width, m_height)) {
            return false;
        }

        // PNG描画の安全な実行
        if (!m_canvas->drawPng(decodedData.data(), actualLen, 0, 0)) {
            cleanup();
            return false;
        }

        // ピクセルデータは遅延読み込み（メモリ節約）
        m_pixels_valid = false;
        m_canvas_dirty = false;
        
        return true;
    }

    // 遅延ピクセルデータ読み込み（メモリ効率化）
    void ensurePixelsValid() const {
        if (m_pixels_valid && !m_canvas_dirty) return;
        
        if (!m_canvas || m_width <= 0 || m_height <= 0) {
            m_pixels.clear();
            m_pixels_valid = false;
            return;
        }
        
        // ピクセルデータを M5Canvas から読み取る
        m_pixels.resize(m_width * m_height);
        
        // M5Canvasからピクセルを1つずつ読み取り（重いが正確）
        for (int32_t y = 0; y < m_height; ++y) {
            for (int32_t x = 0; x < m_width; ++x) {
                uint16_t rgb565 = m_canvas->readPixel(x, y);
                m_pixels[y * m_width + x] = Color(rgb565);
            }
        }
        
        m_pixels_valid = true;
        m_canvas_dirty = false;
    }

    // キャンバスにピクセルデータを同期（必要な時のみ）
    void syncPixelsToCanvas() const {
        if (!m_canvas || !m_pixels_valid) return;
        
        for (int32_t y = 0; y < m_height; ++y) {
            for (int32_t x = 0; x < m_width; ++x) {
                const Color& pixel = m_pixels[y * m_width + x];
                m_canvas->drawPixel(x, y, pixel.toRGB565());
            }
        }
        m_canvas_dirty = false;
    }

    // OpenSiv3D互換のピクセルアクセス機能（遅延読み込み対応）
    
    // OpenSiv3D Style: image[y][x] アクセス（プロキシパターン）
    class RowProxy {
    private:
        Image* m_image;
        int32_t m_row;
    public:
        RowProxy(Image* image, int32_t row) : m_image(image), m_row(row) {}
        
        Color& operator[](int32_t x) { 
            m_image->ensurePixelsValid();
            m_image->m_canvas_dirty = true;  // 変更されたことをマーク
            return m_image->m_pixels[m_row * m_image->m_width + x]; 
        }
    };
    
    class ConstRowProxy {
    private:
        const Image* m_image;
        int32_t m_row;
    public:
        ConstRowProxy(const Image* image, int32_t row) : m_image(image), m_row(row) {}
        
        const Color& operator[](int32_t x) const { 
            m_image->ensurePixelsValid();
            return m_image->m_pixels[m_row * m_image->m_width + x]; 
        }
    };
    
    RowProxy operator[](int32_t y) {
        return RowProxy(this, y);
    }
    
    ConstRowProxy operator[](int32_t y) const {
        return ConstRowProxy(this, y);
    }
    
    // OpenSiv3D Style: image[pos] アクセス
    Color& operator[](const Math::Vec2i& pos) {
        ensurePixelsValid();
        m_canvas_dirty = true;
        return m_pixels[pos.y * m_width + pos.x];
    }
    
    const Color& operator[](const Math::Vec2i& pos) const {
        ensurePixelsValid();
        return m_pixels[pos.y * m_width + pos.x];
    }
    
    // 範囲for文対応（OpenSiv3D互換） - 遅延読み込み
    auto begin() { 
        ensurePixelsValid(); 
        m_canvas_dirty = true;
        return m_pixels.begin(); 
    }
    auto end() { 
        ensurePixelsValid(); 
        return m_pixels.end(); 
    }
    auto begin() const { 
        ensurePixelsValid(); 
        return m_pixels.begin(); 
    }
    auto end() const { 
        ensurePixelsValid(); 
        return m_pixels.end(); 
    }
    
    // ESP32安全版イメージ作成（境界チェック付き）
    bool create(int32_t width, int32_t height, const Color& backgroundColor = Palette::Black) {
        if (!isCanvasReady()) {
            return false;
        }

        // 安全な範囲チェック
        if (width <= 0 || height <= 0 || 
            static_cast<size_t>(width) > MAX_IMAGE_WIDTH || 
            static_cast<size_t>(height) > MAX_IMAGE_HEIGHT) {
            return false;
        }

        // 既存リソースのクリーンアップ
        cleanup();

        // サイズ設定
        m_width = width;
        m_height = height;

        // 安全なスプライト作成
        if (!createSafeSprite(width, height)) {
            return false;
        }
        
        // キャンバスを初期化
        m_canvas->fillSprite(backgroundColor.toRGB565());
        
        // ピクセルデータは遅延読み込み（メモリ節約）
        m_pixels_valid = false;
        m_canvas_dirty = false;
        
        return true;
    }

    // 安全な描画メソッド
    void draw(int32_t x, int32_t y) const {
        if (isValid() && m_canvas) {
            // ピクセルが変更されている場合は同期
            if (m_canvas_dirty) {
                syncPixelsToCanvas();
            }
            m_canvas->pushSprite(&System::getInstance().getCanvas(), x, y);
        }
    }

    // スケール描画（ESP32安全版）
    void draw(int32_t x, int32_t y, float scale_x, float scale_y) const {
        if (!isValid() || !m_canvas) return;
        
        // ピクセルが変更されている場合は同期
        if (m_canvas_dirty) {
            syncPixelsToCanvas();
        }
        
        // スケール値の安全性チェック
        if (scale_x <= 0.0f || scale_y <= 0.0f || scale_x > 10.0f || scale_y > 10.0f) {
            return;  // 異常なスケール値を拒否
        }
        
        // スケール後のサイズ計算と境界チェック
        int32_t scaled_w = static_cast<int32_t>(m_width * scale_x);
        int32_t scaled_h = static_cast<int32_t>(m_height * scale_y);
        
        if (scaled_w <= 0 || scaled_h <= 0 || 
            static_cast<size_t>(scaled_w) > MAX_IMAGE_WIDTH || 
            static_cast<size_t>(scaled_h) > MAX_IMAGE_HEIGHT) {
            return;  // スケール後のサイズが不正
        }
        
        // 一時キャンバスの安全な作成（RAII）
        std::unique_ptr<M5Canvas> temp = std::make_unique<M5Canvas>(&M5RealUnified::getDisplay());
        if (!temp || !temp->createSprite(scaled_w, scaled_h)) {
            return;  // メモリ不足またはスプライト作成失敗
        }
        
        // スケーリング処理
        temp->setPivot(0, 0);
        temp->pushRotateZoom(0, 0, 0, scale_x, scale_y);
        
        // 描画実行
        temp->pushSprite(&System::getInstance().getCanvas(), x, y);
        
        // temp は自動的に deleteSprite() とデストラクタで解放される
    }

    // 均等スケール描画
    void draw(int32_t x, int32_t y, float scale) const {
        draw(x, y, scale, scale);
    }

    // OpenSiv3D互換のメンバ関数
    
    // サイズ関連（OpenSiv3D互換）
    int32_t width() const noexcept { return m_width; }
    int32_t height() const noexcept { return m_height; }
    Math::Vec2i size() const noexcept { return Math::Vec2i(m_width, m_height); }
    bool isEmpty() const noexcept { return m_width <= 0 || m_height <= 0; }
    bool isValid() const noexcept { return m_canvas && m_width > 0 && m_height > 0; }
    
    // OpenSiv3D Style: 画像の塗りつぶし（メモリ効率版）
    void fill(const Color& color) {
        if (!m_canvas) return;
        
        // キャンバスを直接塗りつぶし
        m_canvas->fillSprite(color.toRGB565());
        
        // ピクセルデータが有効な場合は更新
        if (m_pixels_valid) {
            std::fill(m_pixels.begin(), m_pixels.end(), color);
        }
        
        m_canvas_dirty = false;
    }
    
    // OpenSiv3D Style: メモリ解放
    void release() noexcept {
        cleanup();
    }
    
    // OpenSiv3D Style: 画像の保存（ダミー実装）
    bool save(const String& path) const {
        // TODO: 実際のファイル保存を実装
        Print << "Image::save() called with path: " << path;
        return false;  // 現在は未実装
    }
    
    bool saveWithDialog() const {
        // TODO: 実際のダイアログ保存を実装
        Print << "Image::saveWithDialog() called";
        return false;  // 現在は未実装
    }
    
    // OpenSiv3D Style: 画像の拡大縮小（メモリ効率版）
    Image scaled(double scale) const {
        return scaled(Math::Vec2i(
            static_cast<int32_t>(m_width * scale),
            static_cast<int32_t>(m_height * scale)
        ));
    }
    
    Image scaled(const Math::Vec2i& newSize) const {
        Image result(newSize, Palette::Black);
        
        if (isEmpty() || result.isEmpty()) {
            return result;
        }
        
        // 遅延読み込みでピクセルデータを取得
        ensurePixelsValid();
        
        // 簡易的なバイリニア補間
        for (int32_t y = 0; y < newSize.y; ++y) {
            for (int32_t x = 0; x < newSize.x; ++x) {
                float srcX = (x / static_cast<float>(newSize.x)) * m_width;
                float srcY = (y / static_cast<float>(newSize.y)) * m_height;
                
                int32_t x0 = static_cast<int32_t>(srcX);
                int32_t y0 = static_cast<int32_t>(srcY);
                
                if (x0 >= 0 && x0 < m_width && y0 >= 0 && y0 < m_height) {
                    result[y][x] = m_pixels[y0 * m_width + x0];
                }
            }
        }
        
        return result;
    }
    
    // OpenSiv3D Style: 画像の部分コピー（メモリ効率版）
    Image clipped(int32_t x, int32_t y, int32_t w, int32_t h) const {
        Image result(w, h, Palette::Black);
        
        if (isEmpty()) {
            return result;
        }
        
        // 遅延読み込みでピクセルデータを取得
        ensurePixelsValid();
        
        for (int32_t dy = 0; dy < h; ++dy) {
            for (int32_t dx = 0; dx < w; ++dx) {
                int32_t srcX = x + dx;
                int32_t srcY = y + dy;
                
                if (srcX >= 0 && srcX < m_width && srcY >= 0 && srcY < m_height) {
                    result[dy][dx] = m_pixels[srcY * m_width + srcX];
                }
            }
        }
        
        return result;
    }

    // デストラクタ（RAII - 自動リソース管理）
    ~Image() noexcept {
        cleanup();
    }

private:
    // 安全な初期化処理
    void initializeCanvas() {
        m_canvas = std::make_unique<M5Canvas>(&M5RealUnified::getDisplay());
        if (m_canvas) {
            m_canvas->setColorDepth(16);  // 16bitカラーモード
        }
    }
    
    // キャンバスの準備状態をチェック
    bool isCanvasReady() const noexcept {
        return m_canvas != nullptr;
    }
    
    // 安全なリソースクリーンアップ
    void cleanup() noexcept {
        if (m_canvas) {
            m_canvas->deleteSprite();
        }
        m_pixels.clear();
        m_pixels_valid = false;
        m_canvas_dirty = false;
        m_width = 0;
        m_height = 0;
    }
    
    // 安全なスプライト作成
    bool createSafeSprite(int32_t width, int32_t height) {
        if (!m_canvas) {
            initializeCanvas();
            if (!m_canvas) return false;
        }
        
        return m_canvas->createSprite(width, height);
    }
    
    // PNG ヘッダーの安全な検証
    bool validatePngHeader(const std::vector<uint8_t>& data, size_t len) const noexcept {
        if (len < 24 || data.size() < 24) return false;
        
        // PNG シグネチャの確認
        return data[0] == 0x89 && data[1] == 'P' && data[2] == 'N' && data[3] == 'G' &&
               data[4] == 0x0D && data[5] == 0x0A && data[6] == 0x1A && data[7] == 0x0A;
    }
    
    // PNG 画像サイズの安全な抽出
    std::optional<Math::Vec2i> extractPngDimensions(const std::vector<uint8_t>& data) const noexcept {
        if (data.size() < 24) return std::nullopt;
        
        // IHDR チャンクから幅と高さを取得（ビッグエンディアン）
        int32_t width = (static_cast<int32_t>(data[16]) << 24) | 
                       (static_cast<int32_t>(data[17]) << 16) | 
                       (static_cast<int32_t>(data[18]) << 8) | 
                        static_cast<int32_t>(data[19]);
                        
        int32_t height = (static_cast<int32_t>(data[20]) << 24) | 
                        (static_cast<int32_t>(data[21]) << 16) | 
                        (static_cast<int32_t>(data[22]) << 8) | 
                         static_cast<int32_t>(data[23]);
        
        return Math::Vec2i(width, height);
    }
};


// SimpleGUI名前空間を追加
namespace SimpleGUI
{
    // GUIの共通スタイル設定
    struct Style
    {
        static constexpr int32_t DefaultFontSize = 2;
        static constexpr int32_t DefaultPadding = 4;
        static constexpr int32_t DefaultMargin = 4;
        static constexpr int32_t DefaultHeight = 24;
        static constexpr int32_t DefaultWidth = 120;
        
        Color TextColor = Palette::Black;
        Color BackgroundColor = Palette::White;
        Color ActiveColor = Color(0, 120, 215);
        Color DisabledColor = Color(200, 200, 200);
    };

    Style& getDefaultStyle() { static Style instance; return instance; }
    #define DefaultStyle getDefaultStyle()

    // 共通のフォントインスタンス
    namespace detail
    {
        inline Font& GetFont()
        {
            static Font font;
            static bool initialized = false;
            if (!initialized)
            {
                font.setSize(DefaultStyle.DefaultFontSize);
                initialized = true;
            }
            return font;
        }
    }

    // ボタンの領域を計算
    [[nodiscard]]
    inline Rect ButtonRegion(const String& label, const Math::Vec2i& pos, 
                           int32_t width = DefaultStyle.DefaultWidth)
    {
        return Rect(pos.x, pos.y, width, DefaultStyle.DefaultHeight);
    }

    // ボタン
    inline bool Button(const String& label, const Math::Vec2i& pos, 
                      int32_t width = DefaultStyle.DefaultWidth,
                      bool enabled = true)
    {
        auto button = ButtonRegion(label, pos, width);
        static constexpr int32_t cornerRadius = 4;  // 角の丸みの半径

        // ボタンの描画
        if (enabled)
        {
            if (button.pressed())
            {
                button.drawRound(cornerRadius, DefaultStyle.ActiveColor);
            }
            else if (button.touchOver())
            {
                button.drawRound(cornerRadius, Color(
                    Math::lerp(DefaultStyle.BackgroundColor.r, DefaultStyle.ActiveColor.r, 0.5),
                    Math::lerp(DefaultStyle.BackgroundColor.g, DefaultStyle.ActiveColor.g, 0.5),
                    Math::lerp(DefaultStyle.BackgroundColor.b, DefaultStyle.ActiveColor.b, 0.5)
                ));
            }
            else
            {
                button.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
            }
            button.drawRoundFrame(cornerRadius, DefaultStyle.TextColor); // 輪郭を追加
        }
        else
        {
            button.drawRound(cornerRadius, DefaultStyle.DisabledColor);
            button.drawRoundFrame(cornerRadius, Color(160, 160, 160)); // 無効時の輪郭
        }

        // テキストの描画
        auto& font = detail::GetFont();
        font.setHorizontalAlign(Font::HorizontalAlign::Center)
            .setVerticalAlign(Font::VerticalAlign::Center);
        
        font(label, Font::Pos(
            pos.x + button.m_width/2, 
            pos.y + DefaultStyle.DefaultHeight/2
        ), enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);

        return enabled && button.released();
    }

    // スライダーの領域を計算
    [[nodiscard]]
    inline Rect SliderRegion(const Math::Vec2i& pos, int32_t width = DefaultStyle.DefaultWidth)
    {
        return Rect(pos.x, pos.y, width, DefaultStyle.DefaultHeight);
    }

    // スライダー
    inline bool Slider(double& value, const Math::Vec2i& pos,
                      double min, double max,
                      int32_t width = DefaultStyle.DefaultWidth,
                      bool enabled = true)
    {
        auto slider = SliderRegion(pos, width);
        bool changed = false;
        static constexpr int32_t cornerRadius = 4;
        static constexpr int32_t trackHeight = 6;  // スライダーのトラック高さ

        // スライダーの背景
        slider.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
        slider.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);

        // スライダーの背景（トラック）
        const int32_t trackY = pos.y + (DefaultStyle.DefaultHeight - trackHeight) / 2;
        Rect track(pos.x + DefaultStyle.DefaultPadding, 
                  trackY, 
                  width - DefaultStyle.DefaultPadding * 2, 
                  trackHeight);
        
        if (enabled)
        {
            // 暗いトラックを描画
            track.drawRound(cornerRadius, Color(220, 220, 220));
            track.drawRoundFrame(cornerRadius, Color(180, 180, 180));

            // アクティブな部分を描画
            const double normalizedValue = (value - min) / (max - min);
            const int32_t activeWidth = static_cast<int32_t>(normalizedValue * (width - DefaultStyle.DefaultPadding * 2));
            if (activeWidth > 0)
            {
                Rect activeTrack(pos.x + DefaultStyle.DefaultPadding, 
                               trackY, 
                               activeWidth, 
                               trackHeight);
                activeTrack.drawRound(cornerRadius, DefaultStyle.ActiveColor);
            }

            // スライダーの操作
            if (slider.pressed())
            {
                const int32_t touchX = Input::Touch.pos().x - (pos.x + DefaultStyle.DefaultPadding);
                const int32_t effectiveWidth = width - DefaultStyle.DefaultPadding * 2;
                value = min + (max - min) * (Math::clamp(static_cast<double>(touchX) / effectiveWidth, 0.0, 1.0));
                changed = true;
            }

            // つまみの描画
            const int32_t thumbX = pos.x + DefaultStyle.DefaultPadding + 
                                 static_cast<int32_t>(normalizedValue * (width - DefaultStyle.DefaultPadding * 2 - DefaultStyle.DefaultHeight/2));
            Circle thumb(thumbX + DefaultStyle.DefaultHeight/4, 
                        pos.y + DefaultStyle.DefaultHeight/2, 
                        DefaultStyle.DefaultHeight/3);
            thumb.draw(Palette::White);
            thumb.drawFrame(DefaultStyle.TextColor);
        }
        else
        {
            // 無効時の描画
            track.drawRound(cornerRadius, DefaultStyle.DisabledColor);
            track.drawRoundFrame(cornerRadius, Color(180, 180, 180));

            const double normalizedValue = (value - min) / (max - min);
            const int32_t thumbX = pos.x + DefaultStyle.DefaultPadding + 
                                 static_cast<int32_t>(normalizedValue * (width - DefaultStyle.DefaultPadding * 2 - DefaultStyle.DefaultHeight/2));
            Circle thumb(thumbX + DefaultStyle.DefaultHeight/4, 
                        pos.y + DefaultStyle.DefaultHeight/2, 
                        DefaultStyle.DefaultHeight/3);
            thumb.draw(Color(240, 240, 240));
            thumb.drawFrame(Color(180, 180, 180));
        }

        return changed;
    }

    // ラベル付きスライダー
    inline bool Slider(const String& label, double& value,
                      const Math::Vec2i& pos,
                      double min, double max,
                      int32_t labelWidth = 80,
                      int32_t sliderWidth = DefaultStyle.DefaultWidth,
                      bool enabled = true)
    {
        // 背景の描画
        Rect background(pos.x, pos.y, labelWidth + DefaultStyle.DefaultMargin + sliderWidth, DefaultStyle.DefaultHeight);
        static constexpr int32_t cornerRadius = 4;
        background.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
        background.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);

        // ラベルの描画
        auto& font = detail::GetFont();
        font.setHorizontalAlign(Font::HorizontalAlign::Left)
            .setVerticalAlign(Font::VerticalAlign::Center);
            
        font(label, Font::Pos(
            pos.x + DefaultStyle.DefaultPadding, 
            pos.y + DefaultStyle.DefaultHeight/2
        ), enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);

        return Slider(value, 
                     Math::Vec2i(pos.x + labelWidth + DefaultStyle.DefaultMargin, pos.y),
                     min, max, sliderWidth, enabled);
    }

    // チェックボックスの領域を計算
    [[nodiscard]]
    inline Rect CheckBoxRegion(const Math::Vec2i& pos)
    {
        return Rect(pos.x, pos.y, DefaultStyle.DefaultHeight, DefaultStyle.DefaultHeight);
    }

    // チェックボックス
    inline bool CheckBox(bool& checked, const String& label,
                        const Math::Vec2i& pos,
                        int32_t width = DefaultStyle.DefaultWidth,
                        bool enabled = true)
    {
        auto box = CheckBoxRegion(pos);
        bool changed = false;
        static constexpr int32_t cornerRadius = 2;  // チェックボックスの角の丸み

        // 背景領域の描画
        Rect background(pos.x, pos.y, width, DefaultStyle.DefaultHeight);
        background.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
        background.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);

        // チェックボックスの描画
        if (enabled)
        {
            box.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);
            if (checked)
            {
                Rect inner(pos.x + 4, pos.y + 4, 
                          DefaultStyle.DefaultHeight - 8, 
                          DefaultStyle.DefaultHeight - 8);
                inner.drawRound(cornerRadius, DefaultStyle.ActiveColor);

                // チェックマークを白で描画
                const int32_t cx = pos.x + DefaultStyle.DefaultHeight/2;
                const int32_t cy = pos.y + DefaultStyle.DefaultHeight/2;
                Line(cx - 5, cy, cx - 2, cy + 3).draw(Palette::White);
                Line(cx - 2, cy + 3, cx + 4, cy - 4).draw(Palette::White);
            }

            if (box.released())
            {
                checked = !checked;
                changed = true;
            }
        }
        else
        {
            box.drawRoundFrame(cornerRadius, DefaultStyle.DisabledColor);
            if (checked)
            {
                Rect inner(pos.x + 4, pos.y + 4, 
                          DefaultStyle.DefaultHeight - 8, 
                          DefaultStyle.DefaultHeight - 8);
                inner.drawRound(cornerRadius, DefaultStyle.DisabledColor);
            }
        }

        // ラベルの描画
        auto& font = detail::GetFont();
        font.setHorizontalAlign(Font::HorizontalAlign::Left)
            .setVerticalAlign(Font::VerticalAlign::Center);
            
        font(label, Font::Pos(
            pos.x + DefaultStyle.DefaultHeight + DefaultStyle.DefaultMargin,
            pos.y + DefaultStyle.DefaultHeight/2
        ), enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);

        return changed;
    }

    // ラジオボタンの領域を計算
    [[nodiscard]]
    inline Rect RadioButtonRegion(const Math::Vec2i& pos, int32_t index)
    {
        return Rect(pos.x, 
                   pos.y + index * (DefaultStyle.DefaultHeight + DefaultStyle.DefaultMargin),
                   DefaultStyle.DefaultHeight, 
                   DefaultStyle.DefaultHeight);
    }

    // ラジオボタン
    inline bool RadioButtons(size_t& index,
                           const std::vector<String>& options,
                           const Math::Vec2i& pos,
                           int32_t width = DefaultStyle.DefaultWidth,
                           bool enabled = true)
    {
        bool changed = false;
        static constexpr int32_t cornerRadius = 4;

        // 全体の背景領域を計算
        int32_t totalHeight = options.size() * DefaultStyle.DefaultHeight + 
                            (options.size() - 1) * DefaultStyle.DefaultMargin;
        Rect background(pos.x, pos.y, width, totalHeight);
        
        // 背景の描画
        background.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
        background.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);

        for (size_t i = 0; i < options.size(); ++i)
        {
            auto button = RadioButtonRegion(pos, i);

            // ラジオボタンの描画
            if (enabled)
            {
                Circle outer(button.m_x + button.m_height/2, 
                           button.m_y + button.m_height/2, 
                           button.m_height/2 - 2);
                outer.drawFrame(DefaultStyle.TextColor);

                if (i == index)
                {
                    Circle inner(button.m_x + button.m_height/2, 
                               button.m_y + button.m_height/2, 
                               button.m_height/4 - 2);
                    inner.draw(DefaultStyle.ActiveColor);
                }

                if (button.released())
                {
                    index = i;
                    changed = true;
                }
            }
            else
            {
                Circle outer(button.m_x + button.m_height/2, 
                           button.m_y + button.m_height/2, 
                           button.m_height/2 - 2);
                outer.drawFrame(DefaultStyle.DisabledColor);

                if (i == index)
                {
                    Circle inner(button.m_x + button.m_height/2, 
                               button.m_y + button.m_height/2, 
                               button.m_height/4 - 2);
                    inner.draw(DefaultStyle.DisabledColor);
                }
            }

            // ラベルの描画
            auto& font = detail::GetFont();
            font.setHorizontalAlign(Font::HorizontalAlign::Left)
                .setVerticalAlign(Font::VerticalAlign::Center);
                
            font(options[i], Font::Pos(
                button.m_x + DefaultStyle.DefaultHeight + DefaultStyle.DefaultMargin,
                button.m_y + DefaultStyle.DefaultHeight/2
            ), enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);
        }

        return changed;
    }

    // フォント選択ドロップダウン
    inline bool FontSelector(Font& selectedFont, const String& label,
                           const Math::Vec2i& pos,
                           FontCategory::Type category = FontCategory::Type::ASCII,
                           int32_t width = DefaultStyle.DefaultWidth * 2,
                           bool enabled = true)
    {
        static bool isOpen = false;
        static FontCategory::Type currentCategory = category;
        bool changed = false;
        static constexpr int32_t cornerRadius = 4;

        // 現在選択されているフォント名を取得
        String currentFontName = selectedFont.getFontInfo() ? 
            String(selectedFont.getFontInfo()->name.data()) : 
            String("Default Font");

        // メインボタンの描画
        Rect mainButton(pos.x, pos.y, width, DefaultStyle.DefaultHeight);
        
        if (enabled) {
            if (mainButton.released()) {
                isOpen = !isOpen;
                currentCategory = category;
            }
            
            if (mainButton.touchOver()) {
                mainButton.drawRound(cornerRadius, Color(
                    Math::lerp(DefaultStyle.BackgroundColor.r, DefaultStyle.ActiveColor.r, 0.3),
                    Math::lerp(DefaultStyle.BackgroundColor.g, DefaultStyle.ActiveColor.g, 0.3),
                    Math::lerp(DefaultStyle.BackgroundColor.b, DefaultStyle.ActiveColor.b, 0.3)
                ));
            } else {
                mainButton.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
            }
            mainButton.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);
        } else {
            mainButton.drawRound(cornerRadius, DefaultStyle.DisabledColor);
            mainButton.drawRoundFrame(cornerRadius, Color(160, 160, 160));
        }

        // ラベルとフォント名の描画
        auto& font = detail::GetFont();
        font.setHorizontalAlign(Font::HorizontalAlign::Left)
            .setVerticalAlign(Font::VerticalAlign::Center);
            
        font(label + ": " + currentFontName, Font::Pos(
            pos.x + DefaultStyle.DefaultPadding,
            pos.y + DefaultStyle.DefaultHeight/2
        ), enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);

        // ドロップダウン矢印
        const int32_t arrowX = pos.x + width - 20;
        const int32_t arrowY = pos.y + DefaultStyle.DefaultHeight/2;
        if (isOpen) {
            Triangle(arrowX, arrowY - 3, arrowX + 6, arrowY + 3, arrowX - 6, arrowY + 3)
                .draw(enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);
        } else {
            Triangle(arrowX, arrowY + 3, arrowX + 6, arrowY - 3, arrowX - 6, arrowY - 3)
                .draw(enabled ? DefaultStyle.TextColor : DefaultStyle.DisabledColor);
        }

        // ドロップダウンリストの描画
        if (isOpen && enabled) {
            auto fonts = FontRegistry::getFontsByCategory(currentCategory);
            const int32_t itemHeight = DefaultStyle.DefaultHeight;
            const int32_t listHeight = Math::min(static_cast<int32_t>(fonts.size()) * itemHeight, 200);
            
            Rect listBackground(pos.x, pos.y + DefaultStyle.DefaultHeight + 2, width, listHeight);
            listBackground.drawRound(cornerRadius, DefaultStyle.BackgroundColor);
            listBackground.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);

            for (size_t i = 0; i < fonts.size() && i * itemHeight < listHeight; ++i) {
                Rect itemRect(pos.x + 2, 
                            pos.y + DefaultStyle.DefaultHeight + 4 + i * itemHeight, 
                            width - 4, 
                            itemHeight - 2);

                if (itemRect.touchOver()) {
                    itemRect.drawRound(cornerRadius - 1, DefaultStyle.ActiveColor);
                    
                    if (itemRect.released()) {
                        // C++17 安全性チェック付きアクセス
                        if (fonts[i] && fonts[i]->isValid()) {
                            selectedFont = Font(*fonts[i]);
                            isOpen = false;
                            changed = true;
                        }
                    }
                }

                font.setHorizontalAlign(Font::HorizontalAlign::Left)
                    .setVerticalAlign(Font::VerticalAlign::Center);
                    
                // C++17 安全性チェック付きアクセス
                if (fonts[i] && fonts[i]->isValid()) {
                    font(String{fonts[i]->name.data()}, Font::Pos(
                        itemRect.m_x + DefaultStyle.DefaultPadding,
                        itemRect.m_y + itemHeight/2
                    ), itemRect.touchOver() ? Palette::White : DefaultStyle.TextColor);
                }
            }
        }

        return changed;
    }

    // 簡単なフォントサイズ選択
    inline bool FontSizeSelector(FontCategory::Size& selectedSize, const String& label,
                                const Math::Vec2i& pos,
                                int32_t width = DefaultStyle.DefaultWidth,
                                bool enabled = true)
    {
        static const char* sizeNames[] = {"XSmall", "Small", "Medium", "Large", "XLarge", "XXLarge"};
        static size_t currentIndex = static_cast<size_t>(selectedSize);
        
        std::vector<String> options;
        for (const auto& name : sizeNames) {
            options.emplace_back(name);
        }
        
        bool changed = RadioButtons(currentIndex, options, pos, width, enabled);
        
        if (changed) {
            selectedSize = static_cast<FontCategory::Size>(currentIndex);
        }
        
        return changed;
    }

    // フォントプレビュー
    inline void FontPreview(const Font& font, const String& previewText,
                          const Math::Vec2i& pos,
                          int32_t width = DefaultStyle.DefaultWidth * 2,
                          int32_t height = DefaultStyle.DefaultHeight * 2)
    {
        static constexpr int32_t cornerRadius = 4;
        
        Rect previewArea(pos.x, pos.y, width, height);
        previewArea.drawRound(cornerRadius, Palette::White);
        previewArea.drawRoundFrame(cornerRadius, DefaultStyle.TextColor);

        // プレビューテキストの描画
        Font previewFont = font;
        previewFont.setHorizontalAlign(Font::HorizontalAlign::Center)
                   .setVerticalAlign(Font::VerticalAlign::Center);
                   
        previewFont(previewText, Font::Pos(
            pos.x + width/2,
            pos.y + height/2
        ), DefaultStyle.TextColor);

        // C++17 フォント情報の表示（安全性チェック付き）
        if (const auto fontInfo = font.getFontInfo(); fontInfo && fontInfo->isValid()) {
            auto& infoFont = detail::GetFont();
            infoFont.setHorizontalAlign(Font::HorizontalAlign::Left)
                   .setVerticalAlign(Font::VerticalAlign::Bottom);
                   
            infoFont(String{"Font: "} + font.getFontName(), Font::Pos(
                pos.x + DefaultStyle.DefaultPadding,
                pos.y + height - DefaultStyle.DefaultPadding
            ), Color(100, 100, 100));
        }
    }


}

// デフォルトはモダンスタイル
#ifndef M5SIV3D_LEGACY_STYLE

void Main();

void setup()
{
    System::Init();
    Main();
}

void loop()
{
    // OpenSiv3DスタイルではMain()内のwhile(System::Update())でループするため、
    // Arduino側のloop()は何もしない
}

#endif

// =============================================================================
// Audio System - Elegant & Loosely Coupled Design with Strategy Pattern
// =============================================================================

namespace Audio
{
    // 音の周波数定数（OpenSiv3D風）
    namespace Note
    {
        // オクターブ4の音階（基準）
        inline constexpr float C4  = 261.63f;
        inline constexpr float CS4 = 277.18f;  // C#
        inline constexpr float D4  = 293.66f;
        inline constexpr float DS4 = 311.13f;  // D#
        inline constexpr float E4  = 329.63f;
        inline constexpr float F4  = 349.23f;
        inline constexpr float FS4 = 369.99f;  // F#
        inline constexpr float G4  = 392.00f;
        inline constexpr float GS4 = 415.30f;  // G#
        inline constexpr float A4  = 440.00f;  // 基準音
        inline constexpr float AS4 = 466.16f;  // A#
        inline constexpr float B4  = 493.88f;

        // オクターブ3
        inline constexpr float C3  = C4 / 2.0f;
        inline constexpr float CS3 = CS4 / 2.0f;
        inline constexpr float D3  = D4 / 2.0f;
        inline constexpr float DS3 = DS4 / 2.0f;
        inline constexpr float E3  = E4 / 2.0f;
        inline constexpr float F3  = F4 / 2.0f;
        inline constexpr float FS3 = FS4 / 2.0f;
        inline constexpr float G3  = G4 / 2.0f;
        inline constexpr float GS3 = GS4 / 2.0f;
        inline constexpr float A3  = A4 / 2.0f;
        inline constexpr float AS3 = AS4 / 2.0f;
        inline constexpr float B3  = B4 / 2.0f;

        // オクターブ5
        inline constexpr float C5  = C4 * 2.0f;
        inline constexpr float CS5 = CS4 * 2.0f;
        inline constexpr float D5  = D4 * 2.0f;
        inline constexpr float DS5 = DS4 * 2.0f;
        inline constexpr float E5  = E4 * 2.0f;
        inline constexpr float F5  = F4 * 2.0f;
        inline constexpr float FS5 = FS4 * 2.0f;
        inline constexpr float G5  = G4 * 2.0f;
        inline constexpr float GS5 = GS4 * 2.0f;
        inline constexpr float A5  = A4 * 2.0f;
        inline constexpr float AS5 = AS4 * 2.0f;
        inline constexpr float B5  = B4 * 2.0f;

        // C++17 constexpr 便利な関数：オクターブ変換
        [[nodiscard]] constexpr float octave(float baseFreq, int octaveShift) noexcept {
            float result = baseFreq;
            if (octaveShift > 0) {
                for (int i = 0; i < octaveShift; ++i) {
                    result *= 2.0f;
                }
            } else if (octaveShift < 0) {
                for (int i = 0; i < -octaveShift; ++i) {
                    result /= 2.0f;
                }
            }
            return result;
        }
    }

    // 音の波形タイプ（デバイス非依存）
    enum class WaveType : uint8_t
    {
        Square = 0,    // 矩形波（デフォルト）
        Sine = 1,      // 正弦波（近似）
        Triangle = 2,  // 三角波（近似）
        Sawtooth = 3   // ノコギリ波（近似）
    };

    // C++17 オーディオパラメータ（値オブジェクト）
    struct AudioParams {
        float frequency{440.0f};
        uint32_t duration{1000};
        float volume{0.5f};  // 0.0 - 1.0
        WaveType waveType{WaveType::Square};
        
        // C++17 constexpr バリデーション
        [[nodiscard]] constexpr bool isValid() const noexcept {
            return frequency > 0.0f && 
                   duration > 0 && 
                   volume >= 0.0f && volume <= 1.0f;
        }
        
        // C++17 構造化束縛サポート
        template<std::size_t N>
        [[nodiscard]] constexpr auto& get() const noexcept {
            if constexpr (N == 0) return frequency;
            else if constexpr (N == 1) return duration;
            else if constexpr (N == 2) return volume;
            else if constexpr (N == 3) return waveType;
        }
    };

    // Strategy Pattern: オーディオデバイス抽象化インターフェース
    class IAudioDevice {
    public:
        virtual ~IAudioDevice() = default;
        
        // 純粋仮想関数（デバイス固有実装）
        [[nodiscard]] virtual bool playTone(const AudioParams& params) noexcept = 0;
        [[nodiscard]] virtual bool stopAudio() noexcept = 0;
        [[nodiscard]] virtual bool setVolume(float volume) noexcept = 0;
        [[nodiscard]] virtual bool isPlaying() const noexcept = 0;
        
        // デバイス機能クエリ
        [[nodiscard]] virtual bool supportsVolumeControl() const noexcept = 0;
        [[nodiscard]] virtual bool supportsPolyphony() const noexcept = 0;
        [[nodiscard]] virtual std::pair<float, float> getFrequencyRange() const noexcept = 0;
        [[nodiscard]] virtual uint32_t getMaxDuration() const noexcept = 0;
        
        // C++17 optional を使った安全なポリフォニー
        [[nodiscard]] virtual std::optional<bool> playChord(
            const std::vector<float>& frequencies, 
            uint32_t duration, 
            float volume) noexcept {
            return std::nullopt;  // デフォルトは非対応
        }
    };

    // M5Dial専用実装（Strategy Pattern）
    class M5DialAudioDevice final : public IAudioDevice {
    private:
        mutable bool m_isCurrentlyPlaying{false};
        mutable uint32_t m_playStartTime{0};
        mutable uint32_t m_playDuration{0};
        
    public:
        [[nodiscard]] bool playTone(const AudioParams& params) noexcept override {
            if (!params.isValid()) return false;
            
            // M5Dial固有の制限適用
            const auto [safeFreq, safeDuration, safeVolume] = applySafetyLimits(params);
            
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = M5RealUnified::getSpeaker();
                speaker.setVolume(255);  // M5Dialは固定ボリューム
                speaker.tone(safeFreq, safeDuration);
                
                m_isCurrentlyPlaying = true;
                m_playStartTime = millis();
                m_playDuration = safeDuration;
                return true;
            }
        }
        
        [[nodiscard]] bool stopAudio() noexcept override {
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = M5RealUnified::getSpeaker();
                speaker.stop();
                m_isCurrentlyPlaying = false;
                return true;
            }
        }
        
        [[nodiscard]] bool setVolume(float) noexcept override {
            return false;  // M5Dialはボリューム制御非対応
        }
        
        [[nodiscard]] bool isPlaying() const noexcept override {
            if (!m_isCurrentlyPlaying) return false;
            
            const uint32_t elapsed = millis() - m_playStartTime;
            if (elapsed >= m_playDuration) {
                m_isCurrentlyPlaying = false;
                return false;
            }
            return true;
        }

        // M5Dial機能クエリ
        [[nodiscard]] bool supportsVolumeControl() const noexcept override { return false; }
        [[nodiscard]] bool supportsPolyphony() const noexcept override { return false; }
        [[nodiscard]] std::pair<float, float> getFrequencyRange() const noexcept override { 
            return {100.0f, 5000.0f}; 
        }
        [[nodiscard]] uint32_t getMaxDuration() const noexcept override { return 500; }
        
    private:
        // C++17 構造化束縛を返すヘルパー
        [[nodiscard]] std::tuple<float, uint32_t, uint8_t> applySafetyLimits(
            const AudioParams& params) const noexcept {
            
            const auto [minFreq, maxFreq] = getFrequencyRange();
            const float safeFreq = Math::clamp(params.frequency, minFreq, maxFreq);
            const uint32_t safeDuration = Math::clamp(params.duration, 10u, getMaxDuration());
            const uint8_t safeVolume = static_cast<uint8_t>(Math::clamp(params.volume, 0.0f, 1.0f) * 255);
            
            return {safeFreq, safeDuration, safeVolume};
        }
    };

    // M5Unified専用実装（Strategy Pattern）
    class M5UnifiedAudioDevice final : public IAudioDevice {
    private:
        mutable std::vector<std::pair<uint32_t, uint32_t>> m_playingTones;  // {startTime, duration}

    public:
        [[nodiscard]] bool playTone(const AudioParams& params) noexcept override {
            if (!params.isValid()) return false;
            
            const auto [safeFreq, safeDuration, safeVolume] = applySafetyLimits(params);
            
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = M5RealUnified::getSpeaker();
                speaker.setVolume(safeVolume);
                speaker.tone(safeFreq, safeDuration);
                
                m_playingTones.emplace_back(millis(), safeDuration);
                return true;
            }
        }
        
        [[nodiscard]] bool stopAudio() noexcept override {
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = M5RealUnified::getSpeaker();
                speaker.stop();
                m_playingTones.clear();
                return true;
            }
        }
        
        [[nodiscard]] bool setVolume(float volume) noexcept override {
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = M5RealUnified::getSpeaker();
                const uint8_t vol = static_cast<uint8_t>(Math::clamp(volume, 0.0f, 1.0f) * 255);
                speaker.setVolume(vol);
                return true;
            }
        }
        
        [[nodiscard]] bool isPlaying() const noexcept override {
            updatePlayingTones();
            return !m_playingTones.empty();
        }
        
        // M5Unified機能クエリ
        [[nodiscard]] bool supportsVolumeControl() const noexcept override { return true; }
        [[nodiscard]] bool supportsPolyphony() const noexcept override { return true; }
        [[nodiscard]] std::pair<float, float> getFrequencyRange() const noexcept override { 
            return {20.0f, 20000.0f}; 
        }
        [[nodiscard]] uint32_t getMaxDuration() const noexcept override { return 60000; }
        
        // M5Unified固有：ポリフォニー対応
        [[nodiscard]] std::optional<bool> playChord(
            const std::vector<float>& frequencies, 
            uint32_t duration, 
            float volume) noexcept override {
            
            if (frequencies.empty() || frequencies.size() > 8) {
                return false;  // 最大8音まで
            }
            
            if constexpr (true) {  // ESP32では例外を使わない
                auto& speaker = M5RealUnified::getSpeaker();
                const uint8_t vol = static_cast<uint8_t>(Math::clamp(volume, 0.0f, 1.0f) * 255);
                speaker.setVolume(vol);
                
                for (size_t i = 0; i < frequencies.size(); ++i) {
                    const auto [minFreq, maxFreq] = getFrequencyRange();
                    const float safeFreq = Math::clamp(frequencies[i], minFreq, maxFreq);
                    speaker.tone(safeFreq, duration, i);
                }
                
                m_playingTones.emplace_back(millis(), duration);
                return true;
            }
        }
        
    private:
        // C++17 構造化束縛を返すヘルパー
        [[nodiscard]] std::tuple<float, uint32_t, uint8_t> applySafetyLimits(
            const AudioParams& params) const noexcept {
            
            const auto [minFreq, maxFreq] = getFrequencyRange();
            const float safeFreq = Math::clamp(params.frequency, minFreq, maxFreq);
            const uint32_t safeDuration = Math::clamp(params.duration, 10u, getMaxDuration());
            const uint8_t safeVolume = static_cast<uint8_t>(Math::clamp(params.volume, 0.0f, 1.0f) * 255);
            
            return {safeFreq, safeDuration, safeVolume};
        }
        
        void updatePlayingTones() const noexcept {
            const uint32_t currentTime = millis();
            m_playingTones.erase(
                std::remove_if(m_playingTones.begin(), m_playingTones.end(),
                    [currentTime](const auto& tone) {
                        return (currentTime - tone.first) >= tone.second;
                    }),
                m_playingTones.end()
            );
        }
    };

    // Factory Pattern: デバイス固有実装の生成
    class AudioDeviceFactory {
    public:
        [[nodiscard]] static std::unique_ptr<IAudioDevice> createDevice() noexcept {
#ifdef USE_M5_DIAL
            return std::make_unique<M5DialAudioDevice>();
#else
            return std::make_unique<M5UnifiedAudioDevice>();
#endif
        }
    };

    // OpenSiv3D風のサウンドクラス（疎結合設計）
    class Sound {
    private:
        AudioParams m_params;
        bool m_isPlaying{false};
        uint32_t m_playStartTime{0};
        
        // Dependency Injection: デバイス実装への参照
        static IAudioDevice& getAudioDevice() noexcept {
            static auto device = AudioDeviceFactory::createDevice();
            return *device;
        }

    public:
        // C++17 デフォルト初期化
        Sound() = default;
        
        // C++17 構造化束縛対応コンストラクタ
        Sound(float frequency, uint32_t duration = 1000, WaveType wave = WaveType::Square) noexcept
            : m_params{frequency, duration, 0.5f, wave} {}
            
        explicit Sound(const AudioParams& params) noexcept : m_params(params) {}

        // OpenSiv3D Style: 音の再生（デバイス非依存）
        [[nodiscard]] bool play() noexcept {
            if (!m_params.isValid()) return false;
            
            auto& device = getAudioDevice();
            const bool success = device.playTone(m_params);
            
            if (success) {
                m_isPlaying = true;
                m_playStartTime = millis();
            }
            
            return success;
        }

        // OpenSiv3D Style: 音の停止（デバイス非依存）
        void stop() noexcept {
            auto& device = getAudioDevice();
            device.stopAudio();
            m_isPlaying = false;
        }

        // OpenSiv3D Style: 再生状態の確認
        [[nodiscard]] bool isPlaying() const noexcept {
            if (!m_isPlaying) return false;
            
            // デバイス固有の状態も確認
            auto& device = getAudioDevice();
            if (!device.isPlaying()) {
                const_cast<Sound*>(this)->m_isPlaying = false;
                return false;
            }
            
            // 時間ベースの確認
            const uint32_t elapsed = millis() - m_playStartTime;
            if (elapsed >= m_params.duration) {
                const_cast<Sound*>(this)->m_isPlaying = false;
                return false;
            }
            
            return true;
        }

        // C++17 メソッドチェーン対応（fluent interface）
        [[nodiscard]] Sound& setFrequency(float freq) noexcept {
            m_params.frequency = freq;
            return *this;
        }

        [[nodiscard]] Sound& setDuration(uint32_t duration) noexcept {
            m_params.duration = duration;
            return *this;
        }

        [[nodiscard]] Sound& setVolume(float volume) noexcept {
            m_params.volume = Math::clamp(volume, 0.0f, 1.0f);
            return *this;
        }

        [[nodiscard]] Sound& setWaveType(WaveType wave) noexcept {
            m_params.waveType = wave;
            return *this;
        }

        // C++17 構造化束縛対応のパラメータ設定
        [[nodiscard]] Sound& setParams(const AudioParams& params) noexcept {
            if (params.isValid()) {
                m_params = params;
            }
            return *this;
        }

        // OpenSiv3D Style: パラメータ取得（const correctness）
        [[nodiscard]] float getFrequency() const noexcept { return m_params.frequency; }
        [[nodiscard]] uint32_t getDuration() const noexcept { return m_params.duration; }
        [[nodiscard]] float getVolume() const noexcept { return m_params.volume; }
        [[nodiscard]] WaveType getWaveType() const noexcept { return m_params.waveType; }
        [[nodiscard]] const AudioParams& getParams() const noexcept { return m_params; }

        // OpenSiv3D Style: 音の再生メソッド（playOneShot風）
        void playOneShot(float volume = 1.0f) noexcept {
            const float originalVolume = m_params.volume;
            m_params.volume = Math::clamp(volume, 0.0f, 1.0f);
            
            // グローバルミュート状態をチェック
            if (!checkGlobalMuteState()) {
                [[maybe_unused]] const bool success = play(); // C++17 maybe_unused属性で警告回避
            }
            
            m_params.volume = originalVolume;
        }
        
        // デバイス機能クエリ（疎結合）
        [[nodiscard]] static bool supportsVolumeControl() noexcept {
            return getAudioDevice().supportsVolumeControl();
        }
        
        [[nodiscard]] static bool supportsPolyphony() noexcept {
            return getAudioDevice().supportsPolyphony();
        }
        
    private:
        // グローバルミュート状態をチェックする関数（後で実装）
        [[nodiscard]] bool checkGlobalMuteState() const noexcept;
    };

    // Singleton Pattern: グローバル音声制御（疎結合設計）
    class AudioManager {
    private:
        float m_masterVolume{1.0f};
        bool m_muted{false};
        std::vector<Sound*> m_activeSounds;  // 弱参照（所有権なし）
        bool m_initialized{false};
        
        // Dependency Injection: デバイス実装への参照
        IAudioDevice& m_device;

    public:
        [[nodiscard]] static AudioManager& getInstance() noexcept {
            static AudioManager instance;
            return instance;
        }

        // マスターボリューム制御（デバイス非依存）
        void setMasterVolume(float volume) noexcept {
            m_masterVolume = Math::clamp(volume, 0.0f, 1.0f);
            
            if (m_device.supportsVolumeControl()) {
                m_device.setVolume(m_muted ? 0.0f : m_masterVolume);
            } else {
                // ボリューム制御非対応デバイスではミュート状態で代用
                if (m_masterVolume == 0.0f) {
                    m_muted = true;
                } else if (m_muted && m_masterVolume > 0.0f) {
                    m_muted = false;
                }
            }
        }

        [[nodiscard]] float getMasterVolume() const noexcept {
            return m_masterVolume;
        }

        // ミュート制御（デバイス非依存）
        void setMuted(bool muted) noexcept {
            m_muted = muted;
            if (m_device.supportsVolumeControl()) {
                m_device.setVolume(muted ? 0.0f : m_masterVolume);
            }
        }

        [[nodiscard]] bool isMuted() const noexcept {
            return m_muted;
        }

        // 全ての音を停止（デバイス非依存）
        void stopAll() noexcept {
            m_device.stopAudio();
            
            for (auto* sound : m_activeSounds) {
                if (sound) {
                    sound->stop();
                }
            }
            m_activeSounds.clear();
        }

        // 更新処理（System::Update()から呼び出される）
        void update() noexcept {
            // 再生終了した音を削除（弱参照なので安全）
            m_activeSounds.erase(
                std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
                    [](Sound* sound) { return sound == nullptr || !sound->isPlaying(); }),
                m_activeSounds.end()
            );
        }

        // 音の登録（弱参照、所有権なし）
        void registerSound(Sound* sound) noexcept {
            if (sound) {
                m_activeSounds.push_back(sound);
            }
        }

        // デバイス機能の取得（疎結合）
        [[nodiscard]] bool supportsPolyphony() const noexcept {
            return m_device.supportsPolyphony();
        }

        [[nodiscard]] bool supportsVolumeControl() const noexcept {
            return m_device.supportsVolumeControl();
        }
        
        [[nodiscard]] std::pair<float, float> getFrequencyRange() const noexcept {
            return m_device.getFrequencyRange();
        }

        [[nodiscard]] String getDeviceAudioInfo() const noexcept {
            String info = "Audio: ";
            if (supportsVolumeControl()) info += "VolumeControl ";
            if (supportsPolyphony()) info += "Polyphony ";
            
            const auto [minFreq, maxFreq] = getFrequencyRange();
            info += String("Range:") + String(minFreq) + "-" + String(maxFreq) + "Hz";
            return info;
        }
        
        // ポリフォニー再生（デバイス対応時のみ）
        [[nodiscard]] std::optional<bool> playChord(
            const std::vector<float>& frequencies, 
            uint32_t duration = 1000, 
            float volume = 1.0f) noexcept {
            
            return m_device.playChord(frequencies, duration, volume);
        }

    private:
        // Dependency Injection: コンストラクタでデバイス注入
        AudioManager() : m_device(*AudioDeviceFactory::createDevice()) {
            ensureInitialized();
        }
        
        void ensureInitialized() noexcept {
            if (!m_initialized) {
                // 初期化処理
            m_initialized = true;
        }
        }
    };

    // C++17対応のStringハッシュ関数
    struct StringHash {
        [[nodiscard]] std::size_t operator()(const String& s) const noexcept {
            std::size_t hash = 0;
            for (size_t i = 0; i < s.length(); ++i) {
                hash = hash * 31 + static_cast<std::size_t>(s.charAt(i));
            }
            return hash;
        }
    };

    // OpenSiv3D風のAudioアセット管理（疎結合設計）
    namespace AudioAsset
    {
        // RAII + unique_ptr による安全なリソース管理
        using AssetMap = std::unordered_map<String, std::unique_ptr<Sound>, StringHash>;
        
        [[nodiscard]] inline AssetMap& getAssetMap() noexcept {
            static AssetMap assets;
            return assets;
        }

        // C++17 perfect forwarding による効率的な登録
        template<typename... Args>
        void Register(const String& name, Args&&... args) noexcept {
            auto& assets = getAssetMap();
            assets[name] = std::make_unique<Sound>(std::forward<Args>(args)...);
        }

        // C++17 std::call_once を使った安全なプリセット登録
        [[nodiscard]] Sound& Get(const String& name) noexcept {
            static std::once_flag presetsFlag;
            std::call_once(presetsFlag, []() {
                Register("Beep", 1000.0f, 200);
                Register("Click", 2000.0f, 50);
                Register("Success", 800.0f, 300);
                Register("Error", 200.0f, 500);
            });
            
            auto& assets = getAssetMap();
            if (const auto it = assets.find(name); it != assets.end()) {
                return *it->second;
            }
            
            // 見つからない場合は空のサウンドを返す（Null Object Pattern）
            static Sound emptySound;
            return emptySound;
        }

        // OpenSiv3D Style: その他のアセット管理機能
        [[nodiscard]] inline bool Load(const String&) noexcept {
            return true;  // M5デバイスでは即座にロード完了
        }

        [[nodiscard]] inline bool IsReady(const String& name) noexcept {
            auto& assets = getAssetMap();
            return assets.find(name) != assets.end();
        }

        inline void Release(const String& name) noexcept {
            auto& assets = getAssetMap();
            if (const auto it = assets.find(name); it != assets.end()) {
                it->second->stop();
            }
        }

        inline void Unregister(const String& name) noexcept {
            auto& assets = getAssetMap();
            if (const auto it = assets.find(name); it != assets.end()) {
                it->second->stop();
                assets.erase(it);
            }
        }
    }

    // OpenSiv3D風のグローバルオーディオ制御（Facade Pattern）
    namespace GlobalAudio
    {
        inline void SetVolume(float volume) noexcept {
            AudioManager::getInstance().setMasterVolume(volume);
        }

        [[nodiscard]] inline float GetVolume() noexcept {
            return AudioManager::getInstance().getMasterVolume();
        }

        inline void SetMute(bool muted) noexcept {
            AudioManager::getInstance().setMuted(muted);
        }

        [[nodiscard]] inline bool IsMuted() noexcept {
            return AudioManager::getInstance().isMuted();
        }

        inline void StopAll() noexcept {
            AudioManager::getInstance().stopAll();
        }
        
        // デバイス機能クエリ（疎結合）
        [[nodiscard]] inline bool SupportsPolyphony() noexcept {
            return AudioManager::getInstance().supportsPolyphony();
        }
        
        [[nodiscard]] inline bool SupportsVolumeControl() noexcept {
            return AudioManager::getInstance().supportsVolumeControl();
        }
        
        [[nodiscard]] inline String GetDeviceInfo() noexcept {
            return AudioManager::getInstance().getDeviceAudioInfo();
        }
    }

    // 注意：UpdateAudioSystem()関数は削除されました。
    // 代わりに直接 AudioManager::getInstance().update() を呼び出してください。
    // これによりシングルトンパターンが統一され、より一貫性のある設計になります。
    
    // Sound::checkGlobalMuteState()の実装（AudioManager定義後）
    inline bool Sound::checkGlobalMuteState() const noexcept {
        return AudioManager::getInstance().isMuted();
    }

    // 便利な和音機能（デバイス対応時のみ）
    [[nodiscard]] inline std::optional<bool> PlayChord(
        const std::vector<float>& frequencies, 
        uint32_t duration = 1000, 
        float volume = 1.0f) noexcept {
        
        return AudioManager::getInstance().playChord(frequencies, duration, volume);
    }

    // 便利な和音プリセット
    inline void PlayMajorChord(float rootFreq, uint32_t duration = 1000) noexcept {
        const std::vector<float> chord = {
            rootFreq,           // ルート
            rootFreq * 1.25f,   // 長3度
            rootFreq * 1.5f     // 完全5度
        };
        [[maybe_unused]] const auto result = PlayChord(chord, duration); // C++17 maybe_unused属性で警告回避
    }

    inline void PlayMinorChord(float rootFreq, uint32_t duration = 1000) noexcept {
        const std::vector<float> chord = {
            rootFreq,           // ルート
            rootFreq * 1.2f,    // 短3度
            rootFreq * 1.5f     // 完全5度
        };
        [[maybe_unused]] const auto result = PlayChord(chord, duration); // C++17 maybe_unused属性で警告回避
    }
}



// OpenSiv3D風のグローバル関数定義（名前衝突を避けるため別名を使用）
inline Audio::Sound& GetAudioAsset(const String& name) {
    return Audio::AudioAsset::Get(name);
}

// OpenSiv3D互換のグローバル関数（関数ベース）
inline Audio::Sound& AudioAsset(const String& name) {
    return Audio::AudioAsset::Get(name);
}

// 便利な登録関数
inline void RegisterAudioAsset(const String& name, float frequency, uint32_t duration = 1000) {
    Audio::AudioAsset::Register(name, frequency, duration);
}

// グローバルオーディオ制御
inline void SetGlobalVolume(float volume) {
    Audio::GlobalAudio::SetVolume(volume);
}

inline float GetGlobalVolume() {
    return Audio::GlobalAudio::GetVolume();
}

inline void SetGlobalMute(bool muted) {
    Audio::GlobalAudio::SetMute(muted);
}

inline bool IsGlobalMuted() {
    return Audio::GlobalAudio::IsMuted();
}

inline void StopAllAudio() {
    Audio::GlobalAudio::StopAll();
}

// =============================================================================
// System クラスの実装（全ての依存クラス定義後）
// =============================================================================

// System::updateSubsystems()の実装（統一感のあるシングルトンパターン）
inline void System::updateSubsystems() noexcept {
    // 入力システムの更新
    Input::InputManager::getInstance().update();
    
    // オーディオシステムの更新（統一されたシングルトンパターン）
    Audio::AudioManager::getInstance().update();
}

// System::drawPrintOutput()の実装（PrintManager定義後）
inline void System::drawPrintOutput() noexcept {
    PrintManager::getInstance().draw();
}

// Input::InputManager::update()の実装（全ての依存クラス定義後）
inline void Input::InputManager::update() {
    M5RealUnified::update();  // M5デバイスの状態を更新
    
    // 統一されたシングルトンパターンでサブシステム更新
    Input::TouchInput::getInstance().update();
#ifdef USE_M5_DIAL
    Input::SafeDialEncoder::getInstance().update();
    Input::SafeDialRFID::getInstance().update();
#endif
}

// =============================================================================
// フォントカテゴリ（メモリ最適化対応）
// =============================================================================

// =============================================================================
// C++17 エレガントなフォント管理システム
// =============================================================================

// =============================================================================
// FontSystem (experimental)
//
// NOTE:
// 現状の FontSystem は API/設計が未整理で、テンプレート/if constexpr の条件が不正になり
// コンパイルエラーを起こすため、一旦無効化します。
// 既存の `FontInfo` + `FontRegistry`（このヘッダ前半）を使用してください。
// =============================================================================
#if 0
namespace FontSystem {
    // C++17 フォント取得関数型（型安全）
    using FontProvider = std::function<const lgfx::IFont*()>;
    
    // C++17 optional を使った安全なフォント情報
    struct FontDescriptor {
        std::string_view name;
        FontCategory::Type category;
        FontCategory::Weight weight;
        FontCategory::Style style;
        FontCategory::Size size;
        uint8_t pixelHeight;
        bool supportsJapanese;
        
        // C++17 constexpr コンストラクタ
        constexpr FontDescriptor(std::string_view n, FontCategory::Type cat, 
                               FontCategory::Weight w, FontCategory::Style st, 
                               FontCategory::Size sz, uint8_t height, 
                               bool japanese = false) noexcept
            : name(n), category(cat), weight(w), style(st), 
              size(sz), pixelHeight(height), supportsJapanese(japanese) {}
    };
    
    // C++17 フォントハンドル（RAII + 型安全）
    class FontHandle {
    private:
        FontProvider m_provider;
        FontDescriptor m_descriptor;
        mutable std::optional<const lgfx::IFont*> m_cachedFont;
        
    public:
        // C++17 perfect forwarding constructor
        template<typename Provider>
        constexpr FontHandle(Provider&& provider, const FontDescriptor& desc) noexcept
            : m_provider(std::forward<Provider>(provider)), m_descriptor(desc) {}
        
        // C++17 optional を使った安全なフォント取得（ESP32対応：例外なし）
        [[nodiscard]] std::optional<const lgfx::IFont*> tryGetFont() const noexcept {
            if (!m_cachedFont.has_value()) {
                if (m_provider) {
                    auto* font = m_provider();
                    if (font != nullptr) {
                        m_cachedFont = font;
                    } else {
                        return std::nullopt;
                    }
                } else {
                    return std::nullopt;
                }
            }
            return m_cachedFont;
        }
        
        // C++17 constexpr if を使った安全なアクセス
        [[nodiscard]] const lgfx::IFont& getFont() const noexcept {
            if constexpr (std::is_same_v<decltype(tryGetFont()), std::optional<const lgfx::IFont*>>) {
                auto font = tryGetFont();
                if (font.has_value() && font.value() != nullptr) {
                    return *font.value();
                }
            }
            // フォールバック: 基本フォント
            return fonts::Font2;
        }
        
        // C++17 structured bindings サポート
        [[nodiscard]] constexpr const FontDescriptor& getDescriptor() const noexcept {
            return m_descriptor;
        }
        
        // C++17 constexpr アクセサ
        [[nodiscard]] constexpr std::string_view getName() const noexcept { return m_descriptor.name; }
        [[nodiscard]] constexpr FontCategory::Type getCategory() const noexcept { return m_descriptor.category; }
        [[nodiscard]] constexpr bool supportsJapanese() const noexcept { return m_descriptor.supportsJapanese; }
        [[nodiscard]] constexpr uint8_t getPixelHeight() const noexcept { return m_descriptor.pixelHeight; }
        
        // C++17 explicit bool conversion
        [[nodiscard]] explicit operator bool() const noexcept {
            return tryGetFont().has_value();
        }
    };
    
    // C++17 constexpr if を使ったフォントファクトリ
    template<bool EnableJapanese = M5SIV3D_ENABLE_JAPANESE_FONTS,
             bool EnableLarge = M5SIV3D_ENABLE_LARGE_FONTS,
             bool EnableBold = M5SIV3D_ENABLE_BOLD_FONTS,
             bool EnableDecorative = M5SIV3D_ENABLE_DECORATIVE_FONTS>
    class FontFactory {
    public:
        // C++17 constexpr if による条件付きフォント作成
        [[nodiscard]] static constexpr auto createJapaneseGothic(FontCategory::Size size) noexcept {
            if constexpr (EnableJapanese) {
                switch (size) {
                    case FontCategory::Size::XSmall:
                        return FontHandle([]() { return &fonts::efontJA_10; },
                                        FontDescriptor("Japanese Gothic 10px", FontCategory::Type::Japanese,
                                                     FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                     FontCategory::Size::XSmall, 10, true));
                    case FontCategory::Size::Small:
                        return FontHandle([]() { return &fonts::efontJA_12; },
                                        FontDescriptor("Japanese Gothic 12px", FontCategory::Type::Japanese,
                                                     FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                     FontCategory::Size::Small, 12, true));
                    case FontCategory::Size::Medium:
                        return FontHandle([]() { return &fonts::efontJA_16; },
                                        FontDescriptor("Japanese Gothic 16px", FontCategory::Type::Japanese,
                                                     FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                     FontCategory::Size::Medium, 16, true));
                    case FontCategory::Size::Large:
                        return FontHandle([]() { return &fonts::efontJA_24; },
                                        FontDescriptor("Japanese Gothic 24px", FontCategory::Type::Japanese,
                                                     FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                     FontCategory::Size::Large, 24, true));
                    default:
                        return createFallbackFont();
                }
            } else {
                return createFallbackFont();
            }
        }
        
        // C++17 auto return type deduction
        [[nodiscard]] static auto createEnglishSans(FontCategory::Size size, FontCategory::Weight weight = FontCategory::Weight::Regular) noexcept {
            if constexpr (EnableBold && weight == FontCategory::Weight::Bold) {
                switch (size) {
                    case FontCategory::Size::Small:
                        return FontHandle([]() { return &fonts::FreeSansBold9pt7b; },
                                        FontDescriptor("FreeSans Bold 9pt", FontCategory::Type::ASCII,
                                                     FontCategory::Weight::Bold, FontCategory::Style::Normal,
                                                     FontCategory::Size::Small, 9, false));
                    case FontCategory::Size::Medium:
                        return FontHandle([]() { return &fonts::FreeSansBold12pt7b; },
                                        FontDescriptor("FreeSans Bold 12pt", FontCategory::Type::ASCII,
                                                     FontCategory::Weight::Bold, FontCategory::Style::Normal,
                                                     FontCategory::Size::Medium, 12, false));
                    default:
                        return createBasicFont(size);
                }
            } else {
                switch (size) {
                    case FontCategory::Size::Small:
                        return FontHandle([]() { return &fonts::FreeSans9pt7b; },
                                        FontDescriptor("FreeSans 9pt", FontCategory::Type::ASCII,
                                                     FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                     FontCategory::Size::Small, 9, false));
                    case FontCategory::Size::Medium:
                        return FontHandle([]() { return &fonts::FreeSans12pt7b; },
                                        FontDescriptor("FreeSans 12pt", FontCategory::Type::ASCII,
                                                     FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                     FontCategory::Size::Medium, 12, false));
                    default:
                        return createBasicFont(size);
                }
            }
        }
        
        // C++17 基本フォント（常に利用可能）
        [[nodiscard]] static constexpr auto createBasicFont(FontCategory::Size size) noexcept {
            switch (size) {
                case FontCategory::Size::XSmall:
                    return FontHandle([]() { return &fonts::TomThumb; },
                                    FontDescriptor("TomThumb", FontCategory::Type::ASCII,
                                                 FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                 FontCategory::Size::XSmall, 5, false));
                case FontCategory::Size::Small:
                    return FontHandle([]() { return &fonts::Font2; },
                                    FontDescriptor("Font2", FontCategory::Type::ASCII,
                                                 FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                 FontCategory::Size::Small, 8, false));
                case FontCategory::Size::Medium:
                    return FontHandle([]() { return &fonts::Font4; },
                                    FontDescriptor("Font4", FontCategory::Type::ASCII,
                                                 FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                 FontCategory::Size::Medium, 16, false));
                default:
                    return FontHandle([]() { return &fonts::Font2; },
                                    FontDescriptor("Font2 (Fallback)", FontCategory::Type::ASCII,
                                                 FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                                 FontCategory::Size::Small, 8, false));
            }
        }
        
    private:
        [[nodiscard]] static constexpr auto createFallbackFont() noexcept {
            return FontHandle([]() { return &fonts::Font2; },
                            FontDescriptor("Font2 (Fallback)", FontCategory::Type::ASCII,
                                         FontCategory::Weight::Regular, FontCategory::Style::Normal,
                                         FontCategory::Size::Small, 8, false));
        }
    };
    
    // C++17 フォントレジストリ（型安全なコンテナ）
    class FontRegistry {
    private:
        using FontMap = std::unordered_map<std::string, FontHandle>;
        static inline FontMap s_fonts; // C++17 inline static
        
    public:
        // C++17 perfect forwarding
        template<typename... Args>
        static void registerFont(std::string_view name, Args&&... args) {
            s_fonts.emplace(std::string(name), FontHandle(std::forward<Args>(args)...));
        }
        
        // C++17 optional を使った安全な検索
        [[nodiscard]] static std::optional<FontHandle> findFont(std::string_view name) noexcept {
            auto it = s_fonts.find(std::string(name));
            if (it != s_fonts.end()) {
                return it->second;
            }
            return std::nullopt;
        }
        
        // C++17 structured bindings 対応
        [[nodiscard]] static auto getAllFonts() noexcept {
            std::vector<std::pair<std::string_view, const FontHandle*>> result;
            result.reserve(s_fonts.size());
            
            for (const auto& [name, handle] : s_fonts) {
                result.emplace_back(name, &handle);
            }
            return result;
        }
        
        // C++17 constexpr if による条件付き初期化
        static void initializeDefaultFonts() noexcept {
            using Factory = FontFactory<>;
            
            // 基本フォント（常に利用可能）
            registerFont("basic_tiny", Factory::createBasicFont(FontCategory::Size::XSmall));
            registerFont("basic_small", Factory::createBasicFont(FontCategory::Size::Small));
            registerFont("basic_medium", Factory::createBasicFont(FontCategory::Size::Medium));
            
            // 条件付きフォント
            if constexpr (M5SIV3D_ENABLE_JAPANESE_FONTS) {
                registerFont("japanese_gothic_small", Factory::createJapaneseGothic(FontCategory::Size::Small));
                registerFont("japanese_gothic_medium", Factory::createJapaneseGothic(FontCategory::Size::Medium));
                registerFont("japanese_gothic_large", Factory::createJapaneseGothic(FontCategory::Size::Large));
            }
            
            // 英語フォント
            registerFont("english_sans_small", Factory::createEnglishSans(FontCategory::Size::Small));
            registerFont("english_sans_medium", Factory::createEnglishSans(FontCategory::Size::Medium));
            
            if constexpr (M5SIV3D_ENABLE_BOLD_FONTS) {
                registerFont("english_sans_bold_small", Factory::createEnglishSans(FontCategory::Size::Small, FontCategory::Weight::Bold));
                registerFont("english_sans_bold_medium", Factory::createEnglishSans(FontCategory::Size::Medium, FontCategory::Weight::Bold));
            }
        }
    };
}
#endif

// NOTE:
// `FontInfo` はこのヘッダ内ですでに定義されています（遅延読み込み版）。
// ここでの重複定義はコンパイルエラーになるため、レガシー互換Structは削除しました。
