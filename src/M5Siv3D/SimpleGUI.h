#pragma once

#include <vector>
#include "Math.h"
#include "Color.h"
#include "System.h"
#include "Shapes.h"
#include "Font.h"
#include "Input.h"

// SimpleGUI名前空間
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