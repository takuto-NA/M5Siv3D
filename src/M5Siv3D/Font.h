#pragma once

#include <M5Unified.h>
#include "Color.h"
#include "Shapes.h"
#include "System.h"

// Font構造体の定義
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