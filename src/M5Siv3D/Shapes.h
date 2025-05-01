#pragma once

#include "Math.h"
#include "Color.h"
#include "System.h"
#include "Input.h"

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