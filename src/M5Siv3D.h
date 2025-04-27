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

#include <M5Unified.h>
#include <base64.hpp>  // Densaugeoのライブラリ
#include <SPI.h>
#include <sstream>

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

    // 便利な型エイリアス
    using Vec2f = Vec2<float>;
    using Vec2d = Vec2<double>;
    using Vec2i = Vec2<int32_t>;
    using Vec3f = Vec3<float>;
    using Vec3d = Vec3<double>;
    using Vec3i = Vec3<int32_t>;

    // 基本的な数学定数
    constexpr float Pi = 3.14159265358979323846f;
    constexpr float TwoPi = Pi * 2.0f;
    constexpr float HalfPi = Pi / 2.0f;
    constexpr float QuarterPi = Pi / 4.0f;
    constexpr float E = 2.71828182845904523536f;

    // 基本的な数学関数
    template <typename T>
    T abs(T x) { return x < 0 ? -x : x; }

    template <typename T>
    T min(T a, T b) { return a < b ? a : b; }

    template <typename T>
    T max(T a, T b) { return a > b ? a : b; }

    // 三角関数（ラジアン）
    inline float sin(float x) { return ::sinf(x); }
    inline float cos(float x) { return ::cosf(x); }
    inline float tan(float x) { return ::tanf(x); }
    
    // 逆三角関数
    inline float asin(float x) { return ::asinf(x); }
    inline float acos(float x) { return ::acosf(x); }
    inline float atan(float x) { return ::atanf(x); }
    inline float atan2(float y, float x) { return ::atan2f(y, x); }

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

    // 角度変換
    inline float ToRadians(float degrees) { return degrees * Pi / 180.0f; }
    inline float ToDegrees(float radians) { return radians * 180.0f / Pi; }

    // 線形補間
    template <typename T>
    T lerp(T a, T b, float t) { return a + (b - a) * t; }

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
using Math::sin;
using Math::cos;
using Math::tan;
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

// Color構造体を先に定義
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

    // RGB888形式の整数からColorを生成するコンストラクタ
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

// 色定数の追加
namespace Palette
{
    // Basic colors
    const Color Black(0, 0, 0);
    const Color White(255, 255, 255);
    const Color Red(255, 0, 0);
    const Color Green(0, 128, 0);
    const Color Blue(0, 0, 255);
    const Color Yellow(255, 255, 0);
    const Color Magenta(255, 0, 255);
    const Color Cyan(0, 255, 255);

    // Gray shades
    const Color Dimgray(105, 105, 105);
    const Color Gray(128, 128, 128);
    const Color Darkgray(169, 169, 169);
    const Color Silver(192, 192, 192);
    const Color Lightgray(211, 211, 211);
    const Color Gainsboro(220, 220, 220);
    const Color Whitesmoke(245, 245, 245);

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
    const Color DefaultLetterbox(1, 2, 3);
    const Color DefaultBackground(11, 22, 33);
}

namespace Input
{
  

    class ButtonState
    {
    private:
        m5::Button_Class *m_button;

    public:
        ButtonState(m5::Button_Class *btn) : m_button(btn) {}

        bool down() const { return m_button->isPressed(); }
        bool up() const { return m_button->isReleased(); }
        bool pressed() const { return m_button->wasPressed(); }
        bool released() const { return m_button->wasReleased(); }
        bool pressedDuration(uint32_t ms) const { return m_button->pressedFor(ms); }
        bool releasedDuration(uint32_t ms) const { return m_button->releasedFor(ms); }
    };

    // グローバルなボタンステート
    inline ButtonState ButtonA{&M5.BtnA};
    inline ButtonState ButtonB{&M5.BtnB};
    inline ButtonState ButtonC{&M5.BtnC};

    class IMU
    {
    public:
        // 各軸の角度計算用の構造体
        struct EulerAngles {
            float roll;    // X軸周りの回転（横回転）
            float pitch;   // Y軸周りの回転（縦回転）
            float yaw;     // Z軸周りの回転（水平回転）
        };

        static IMU &getInstance()
        {
            static IMU instance;
            return instance;
        }

        // 現在の姿勢角度を取得
        const EulerAngles& getAngles( float deltaTime)  { 
                    // IMUの姿勢を更新
            updateAttitude(deltaTime);
            return m_currentAngles; }

        // 姿勢を更新（毎フレーム呼び出し）
        void updateAttitude( float deltaTime, float alpha = 0.96f, float gyroScale = 1.0f)
        {
            auto accel = getAccel();
            auto gyro = getGyro();

            // 加速度からの角度計算
            float accelPitch = atan2f(-accel.x, sqrtf(accel.y * accel.y + accel.z * accel.z)) * 180.0f / M_PI;
            float accelRoll = atan2f(accel.y, accel.z) * 180.0f / M_PI;
            float accelYaw = atan2f(accel.x, accel.y) * 180.0f / M_PI;

            // 相補フィルタを各軸に適用
            m_currentAngles.roll = complementaryFilter(accelRoll, gyro.x, deltaTime, m_currentAngles.roll, alpha, gyroScale);
            m_currentAngles.pitch = complementaryFilter(accelPitch, gyro.y, deltaTime, m_currentAngles.pitch, alpha, gyroScale);
            m_currentAngles.yaw = complementaryFilter(accelYaw, gyro.z, deltaTime, m_currentAngles.yaw, alpha, gyroScale);
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

        // 加速度を取得 (G)
        Math::Vec3f getAccel() const
        {
            auto data = M5.Imu.getImuData();
            return Math::Vec3f(data.accel.x, data.accel.y, data.accel.z);
        }

        // 角速度を取得 (deg/s)
        Math::Vec3f getGyro() const
        {
            auto data = M5.Imu.getImuData();
            return Math::Vec3f(data.gyro.x, data.gyro.y, data.gyro.z);
        }

        // 地磁気を取得 (μT)
        Math::Vec3f getMag() const
        {
            auto data = M5.Imu.getImuData();
            return Math::Vec3f(data.mag.x, data.mag.y, data.mag.z);
        }

        // ... rest of IMU implementation ...

    private:
        IMU() : m_currentAngles{0.0f, 0.0f, 0.0f} {}

        // 相補フィルタのヘルパー関数
        float complementaryFilter(float accelAngle, float gyroRate, float deltaTime, float currentAngle, 
                                float alpha, float gyroScale) const {
            float gyroAngle = currentAngle + gyroRate * gyroScale * deltaTime;
            return alpha * gyroAngle + (1.0f - alpha) * accelAngle;
        }

        EulerAngles m_currentAngles;  // 現在の姿勢角度
    };

    // グローバルなIMUインスタンス
    inline IMU &IMU = IMU::getInstance();

    // タッチ入力を管理するクラス
    class TouchInput
    {
    public:
        static TouchInput& getInstance()
        {
            static TouchInput instance;
            return instance;
        }

        // タッチの状態を更新（InputManagerから呼び出される）
        void update()
        {
            m_previousTouchState = m_currentTouchState;
            if (M5.Touch.isEnabled())
            {
                auto t = M5.Touch.getDetail();
                m_currentTouchState.pressed = t.isPressed();
                m_currentTouchState.x = t.x;
                m_currentTouchState.y = t.y;
            }
        }

        // 現在のタッチ位置を取得
        Math::Vec2i pos() const
        {
            return Math::Vec2i(m_currentTouchState.x, m_currentTouchState.y);
        }

        // タッチされているかどうか
        bool pressed() const
        {
            return m_currentTouchState.pressed;
        }

        // タッチが開始されたフレームかどうか
        bool down() const
        {
            return m_currentTouchState.pressed && !m_previousTouchState.pressed;
        }

        // タッチが終了したフレームかどうか
        bool up() const
        {
            return !m_currentTouchState.pressed && m_previousTouchState.pressed;
        }

    private:
        TouchInput() = default;

        struct TouchState
        {
            int32_t x = 0;
            int32_t y = 0;
            bool pressed = false;
        };

        TouchState m_currentTouchState;
        TouchState m_previousTouchState;
    };

    // グローバルなタッチ入力インスタンス
    inline TouchInput& Touch = TouchInput::getInstance();

  class InputManager
    {
    public:
        static InputManager &getInstance()
        {
            static InputManager instance;
            return instance;
        }

        void update()
        {
            M5.update();  // M5の状態を更新
            M5.Imu.update();
            Touch.update();
            }

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
        auto cfg = M5.config();
        M5.begin(cfg);

        // キャンバスを画面のサイズで初期化
        canvas.createSprite(M5.Display.width(), M5.Display.height());
        canvas.setTextSize(2);
        lastDrawTime = millis();
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
            lastDrawTime = currentTime;

            int32_t remaining = FRAME_INTERVAL - (millis() - currentTime);
            if (remaining > 0)
            {
                M5.delay(remaining);
            }

            Input::InputManager::getInstance().update();
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

    int getWidth() const { return M5.Display.width(); }
    int getHeight() const { return M5.Display.height(); }

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
    M5Canvas canvas{&M5.Display};

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
inline PrintManager& Print = PrintManager::getInstance();

inline void ClearPrint() {
    Print.clear();
}

// システムのendDraw()内で呼び出すための描画関数
inline void drawPrint() {
    Print.draw();
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

// Font構造体の拡張
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
    const lgfx::IFont *m_fontPtr;
    float m_size = 1.0f;

    // コンストラクタを更新
    Font(const lgfx::IFont &font = fonts::Font0)
        : m_fontPtr(&font), hAlign(HorizontalAlign::Left), vAlign(VerticalAlign::Baseline) {}

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

    void draw(const String &text, int x, int y, const Color &color = Palette::White)
    {
        auto &canvas = System::getInstance().getCanvas();
        canvas.setTextColor(color.toRGB565());
        canvas.setFont(m_fontPtr);
        canvas.setTextSize(m_size);

        // 水平方向のアライメント処理
        int actualX = x;
        if (hAlign != HorizontalAlign::Left)
        {
            int w = textWidth(text);
            if (hAlign == HorizontalAlign::Center)
                actualX = x - (w / 2);
            else if (hAlign == HorizontalAlign::Right)
                actualX = x - w;
        }

        // 垂直方向のアライメント処理
        int actualY = y;
        if (vAlign != VerticalAlign::Baseline)
        {
            int h = textHeight();
            if (vAlign == VerticalAlign::Center)
                actualY = y - (h / 2) / 2;
            else if (vAlign == VerticalAlign::Bottom)
                actualY = y - h;
            // Top alignment uses the original y position
        }

        canvas.drawString(text, actualX, actualY);
    }

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
        return System::getInstance().getCanvas().textWidth(text) * m_size;
    }

    // テキストの高さを取得するメソッドを修正
    int textHeight() const
    {
        return System::getInstance().getCanvas().fontHeight() * m_size;
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
    M5Canvas* m_canvas;
    bool m_valid = false;
    int32_t m_width = 0;
    int32_t m_height = 0;

public:
    Image() : m_canvas(nullptr) {
        // キャンバスを作成
        m_canvas = new M5Canvas(&M5.Display);
        if (m_canvas) {
            m_canvas->setColorDepth(16);  // 16bitカラーモードを設定
        }
    }
    
    bool loadBase64(const char* base64Data) {
        if (!m_canvas) {
            Serial.println("Canvas not initialized");
            return false;
        }

        // 既存のスプライトを削除
        m_canvas->deleteSprite();
        m_valid = false;
        
        // Base64デコード
        size_t decodedLen = decode_base64_length((unsigned char*)base64Data);
        if (decodedLen == 0) {
            Serial.println("Base64 decode length is 0");
            return false;
        }
        
        uint8_t* decodedData = new uint8_t[decodedLen];
        size_t actualLen = decode_base64((unsigned char*)base64Data, decodedData);
        if (actualLen == 0) {
            Serial.println("Base64 decode failed");
            delete[] decodedData;
            return false;
        }

        // PNGヘッダーからサイズを読み取る (IHDRチャンク)
        if (actualLen < 24 || decodedData[0] != 0x89 || decodedData[1] != 'P' || decodedData[2] != 'N' || decodedData[3] != 'G') {
            Serial.println("Invalid PNG format");
            delete[] decodedData;
            return false;
        }

        // 幅と高さを取得 (ビッグエンディアン)
        m_width = (decodedData[16] << 24) | (decodedData[17] << 16) | (decodedData[18] << 8) | decodedData[19];
        m_height = (decodedData[20] << 24) | (decodedData[21] << 16) | (decodedData[22] << 8) | decodedData[23];
        
        Serial.printf("Image dimensions from PNG header: %dx%d\n", m_width, m_height);

        // 実際のサイズでスプライトを作成
        if (!m_canvas->createSprite(m_width, m_height)) {
            Serial.println("Failed to create sprite");
            delete[] decodedData;
            return false;
        }

        
        // PNG画像を描画
        if (!m_canvas->drawPng(decodedData, actualLen, 0, 0)) {
            Serial.println("Failed to draw PNG");
            delete[] decodedData;
            return false;
        }

        delete[] decodedData;
        m_valid = true;
        
        Serial.println("Image loaded successfully");
        return true;
    }

    bool create(int32_t width, int32_t height, const Color& backgroundColor = Palette::Black) {
        if (!m_canvas) {
            Serial.println("Canvas not initialized");
            return false;
        }

        // 既存のスプライトを削除
        m_canvas->deleteSprite();
        m_valid = false;

        // 新しいサイズでスプライトを作成
        if (!m_canvas->createSprite(width, height)) {
            Serial.println("Failed to create sprite");
            return false;
        }

        m_width = width;
        m_height = height;
        m_canvas->fillSprite(backgroundColor.toRGB565());  
        m_valid = true;
        return true;
    }

    void draw(int32_t x, int32_t y) const {
        if (m_valid && m_canvas) {
            m_canvas->pushSprite(&System::getInstance().getCanvas(), x, y);
        }
    }

    int32_t width() const { return m_width; }
    int32_t height() const { return m_height; }
    bool isEmpty() const { return !m_valid || !m_canvas; }
    Math::Vec2i size() const { return Math::Vec2i(m_width, m_height); }

    void draw(int32_t x, int32_t y, float scale_x, float scale_y) const {
        if (!m_valid || !m_canvas) return;
        
        // Calculate scaled dimensions
        int32_t scaled_w = m_width * scale_x;
        int32_t scaled_h = m_height * scale_y;
        
        // Create temporary canvas for scaling
        M5Canvas temp(&M5.Display);
        temp.createSprite(scaled_w, scaled_h);
        
        // Scale the image
        temp.setPivot(0, 0);
        temp.pushRotateZoom(0, 0, 0, scale_x, scale_y);
        
        // Draw the scaled image to the main canvas
        temp.pushSprite(&System::getInstance().getCanvas(), x, y);
        
        // Clean up
        temp.deleteSprite();
    }

    // Overload for uniform scaling
    void draw(int32_t x, int32_t y, float scale) const {
        draw(x, y, scale, scale);
    }

    ~Image() {
        if (m_canvas) {
            m_canvas->deleteSprite();
            delete m_canvas;
            m_canvas = nullptr;
        }
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

    inline Style DefaultStyle;

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
    // メインループは System::Update() で処理
}

#endif
