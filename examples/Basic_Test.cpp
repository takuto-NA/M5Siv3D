//
// Basic Compatibility Test for M5Siv3D
// Tests C++11 compatibility without M5Dial features
//

#include "../src/M5Siv3D.h"

void Main() {
    Print << "M5Siv3D C++11 Compatibility Test";
    Print << "System initialized successfully";
    
    int counter = 0;
    
    while (System::Update()) {
        // 背景をクリア
        System::SetBackgroundColor(Palette::Black);
        
        // 基本的な図形描画
        Circle(System::Width()/2, System::Height()/2, 50).draw(Palette::Red);
        
        // カウンターの表示
        Font font;
        font.setSize(2)
            .setHorizontalAlign(Font::HorizontalAlign::Center);
        
        font(String(counter), Font::Pos(System::Width()/2, 30), Palette::White);
        
        // ボタンでカウンター増加
        if (Input::ButtonA.pressed()) {
            counter++;
            Print << "Counter: " << counter;
        }
        
        // 簡単なアニメーション
        float time = System::getElapsedTimeS();
        int x = System::Width()/2 + sin(time) * 30;
        Circle(x, System::Height()/2 + 60, 10).draw(Palette::Blue);
        
        // デバッグ情報
        ClearPrint();
        drawPrint();
    }
} 