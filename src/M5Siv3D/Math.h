#pragma once

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