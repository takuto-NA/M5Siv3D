#include "M5Siv3D.h"
/*
 * M5Stack向けのシンプルなサンプルプログラム
 * 
 * 機能:
 * - タッチスクリーンで円を移動
 * - 虹色に変化する円のアニメーション
 * - ボタンAで四角形の色を切り替え
 * - FPS表示
 * 
 * このサンプルは、M5Siv3Dライブラリの基本的な機能を
 * わかりやすく紹介することを目的としています。
 */

// グローバル変数
Circle circle(160, 120, 30);
Rect rect(50, 50, 80, 40);
Font font;
float hue = 0;

void Main()
{
    // メインループ
    while (System::Update())
    {
        // 背景を白に設定
        System::SetBackgroundColor(Palette::White);

        // タッチ入力の取得と円の移動
        if (Input::Touch.pressed())
        {
            circle = Circle(Input::Touch.pos(), 30);
        }

        // 円を描画（虹色に変化）
        hue = Math::fmod(hue + 1.0f, 360.0f);
        circle.draw(Color::FromHSV(hue, 0.8f, 1.0f));

        // 四角形を描画（ボタンAが押されたら色が変わる）
        rect.draw(Input::ButtonA.pressed() ? Palette::Red : Palette::Blue);

        // テキストを中央揃えで描画
        font.setSize(2)
            .setAlign(Font::TextAlign::Center)
            ("Touch Screen!", Font::Pos(160, 200), Palette::White);

        // FPSを表示
        font.setSize(1)
            .setAlign(Font::TextAlign::Left)
            ("FPS: " + String(System::FPS(), 1), Font::Pos(10, 10), Palette::Yellow);
    }
}
