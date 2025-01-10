#include <M5Siv3D.h>

void setup() {
    System::setBackgroundColor(Palette::Blue);
}

void loop() {
    if (System::Update()) {
        auto& canvas = System::getInstance().getCanvas();
        const int centerX = canvas.width() / 2;
        const int centerY = canvas.height() / 2;

        // 回転する三角形を描画
        const int triangleSize = 40;
        float rad = Input::IMU.getAngles(System::getInstance().getDeltaTime()).yaw * M_PI / 180.0f;
        Math::Vec2f p1(0, -triangleSize);
        Math::Vec2f p2(-triangleSize, triangleSize);
        Math::Vec2f p3(triangleSize, triangleSize);

        // 回転行列を適用
        auto rotate = [rad](Math::Vec2f p) {
            return Math::Vec2f(
                p.x * cosf(rad) - p.y * sinf(rad),
                p.x * sinf(rad) + p.y * cosf(rad)
            );
        };

        p1 = rotate(p1);
        p2 = rotate(p2);
        p3 = rotate(p3);

        // 中心に移動
        Triangle(
            centerX + p1.x, centerY + p1.y,
            centerX + p2.x, centerY + p2.y,
            centerX + p3.x, centerY + p3.y
        ).draw(Palette::White);

        // 角度テキストを表示
        Font font;
        font.setSize(2);
        font.setAlign(Font::TextAlign::Left);
        String angleText = String(rad) + "°";
        font(angleText, Font::Pos(50, 20), Palette::White);
    }
}