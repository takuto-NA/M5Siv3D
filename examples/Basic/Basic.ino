#include <M5Siv3D.h>

void setup() {
  System::getInstance().setBackgroundColor(Color(100, 255, 100));
}

void loop() {
 System::getInstance().Update();

  // アニメーションの更新
    circleX += directionX * 2;
    if (circleX >= System::getInstance().getWidth() - CIRCLE_RADIUS || 
        circleX <= CIRCLE_RADIUS) {
      directionX *= -1;
    }

    Rect(circleX + 3, 10 + 3, 20, 20).draw(Palette::Black);
    Rect(circleX, 10, 20, 20).draw(Palette::White);
    Rect(circleX, 10, 20, 20).drawFrame(Palette::Black);

    
    Circle(50, 50, 30).draw(Color::FromHSV(circleX, 1.0, 1.0));
    Circle(50, 50, 30).drawFrame(Palette::Black);

    font.draw("日本語を描画したい!", 21, 101, Palette::Black);
    font.draw("日本語を描画したい!", 20, 100, Palette::White);
}
