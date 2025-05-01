#include <M5Siv3D.h>

void Main()
{
  System.setBackgroundColor(Palette::White);

  
  Font font(fonts::lgfxJapanGothicP_12);

  while(System::Update())
  {
    font.draw("Hello, World!", 10, 10, Palette::Black);
  }
}