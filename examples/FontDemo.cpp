#include <M5Siv3D.h>

void Main()
{
    System::SetBackgroundColor(Palette::Black);
    
    // 確実に存在するフォントのみを使用
    auto englishFont = EnglishFont();
    
    // 直接M5GFXの日本語フォントを使用
    Font directJapaneseFont(FontRegistry::Japanese::Gothic16);
    
    while (System::Update())
    {
        // 背景をクリア
        System::SetBackgroundColor(Palette::Black);
        
        // 英語テキストのテスト
        englishFont("English: Hello World!", Font::Pos(10, 30), Palette::White);
        
        // M5Siv3D経由の日本語テスト
        auto japaneseFont = JapaneseFont();
        japaneseFont("M5Siv3D: こんにちは", Font::Pos(10, 60), Palette::White);
        
        // 直接フォント指定でのテスト
        directJapaneseFont("Direct: こんにちは世界", Font::Pos(10, 90), Palette::Yellow);
        
        // 直接M5.Displayでのテスト（比較用）
        M5.Display.setFont(&fonts::efontJA_16);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.setCursor(10, 120);
        M5.Display.println("M5.Display: こんにちは");
        
        // フォント情報の表示
        englishFont(String("English Font: ") + englishFont.getFontName(), Font::Pos(10, 150), Palette::Cyan);
        englishFont(String("Japanese Font: ") + japaneseFont.getFontName(), Font::Pos(10, 170), Palette::Cyan);
        englishFont(String("Direct Font: ") + directJapaneseFont.getFontName(), Font::Pos(10, 190), Palette::Cyan);
        
        // 日本語フォントサポート状況
        englishFont(String("JP Support: ") + (japaneseFont.supportsJapanese() ? "Yes" : "No"), Font::Pos(10, 210), Palette::Green);
        
        // フォントポインタの確認（デバッグ情報）
        static bool debugShown = false;
        if (!debugShown) {
            Serial.println("=== Font Debug Info ===");
            Serial.printf("Japanese Font Ptr: %p\n", japaneseFont.getFontPtr());
            Serial.printf("Direct Font Ptr: %p\n", directJapaneseFont.getFontPtr());
            Serial.printf("efontJA_16 Ptr: %p\n", &fonts::efontJA_16);
            Serial.printf("Japanese Support: %s\n", japaneseFont.supportsJapanese() ? "true" : "false");
            Serial.printf("Direct Support: %s\n", directJapaneseFont.supportsJapanese() ? "true" : "false");
            debugShown = true;
        }
    }
} 