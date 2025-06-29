#include <M5Siv3D.h>

void Main()
{
    System::Init();
    
    static int page = 0;
    static int maxPage = 4;
    
    while (System::Update())
    {
        System::SetBackgroundColor(Palette::Black);
        
        // 英語フォント（比較用）
        Font englishFont = FontHelper::EnglishSans();
        
        // ページ切り替え（タッチまたはボタン）
        if (Input::getTouch().down() || Input::getButton<0>().down()) {
            page = (page + 1) % maxPage;
        }
        
        // ページ番号表示
        englishFont(String("Page ") + String(page + 1) + "/" + String(maxPage) + " (Touch to change)", Font::Pos(5, 5), Palette::Yellow);
        
        if (page == 0) {
            // ページ1: M5GFX直接テスト
            englishFont("=== M5GFX Direct Test ===", Font::Pos(5, 25), Palette::White);
            
            // M5GFXで直接日本語を描画
            auto& canvas = System::getInstance().getCanvas();
            canvas.setFont(&fonts::efontJA_16);
            canvas.setTextColor(TFT_GREEN);
            canvas.drawString("M5GFX: こんにちは", 5, 45);
            
            canvas.setFont(&fonts::Font2);
            canvas.setTextColor(TFT_CYAN);
            canvas.drawString("M5GFX: Hello", 5, 65);
            
            // フォントポインタ確認
            englishFont("efontJA_16 ptr:", Font::Pos(5, 85), Palette::Cyan);
            char ptrStr[32];
            sprintf(ptrStr, "%p", &fonts::efontJA_16);
            englishFont(ptrStr, Font::Pos(5, 105), Palette::Green);
            
            englishFont("Font2 ptr:", Font::Pos(5, 125), Palette::Cyan);
            sprintf(ptrStr, "%p", &fonts::Font2);
            englishFont(ptrStr, Font::Pos(5, 145), Palette::Green);
            
        } else if (page == 1) {
            // ページ2: M5Siv3D フォントテスト
            englishFont("=== M5Siv3D Font Test ===", Font::Pos(5, 25), Palette::White);
            
            // 日本語フォント（FontHelperから）
            Font japaneseFont = FontHelper::JapaneseGothic();
            
            englishFont("English: Hello World", Font::Pos(5, 45), Palette::Cyan);
            japaneseFont("Japanese: こんにちは", Font::Pos(5, 65), Palette::Green);
            
            // フォント情報
            englishFont(String("EN Font: ") + englishFont.getFontName(), Font::Pos(5, 85), Palette::Cyan);
            englishFont(String("JP Font: ") + japaneseFont.getFontName(), Font::Pos(5, 105), Palette::Green);
            
            // 日本語サポート確認
            englishFont(String("EN JP Support: ") + (englishFont.supportsJapanese() ? "Yes" : "No"), Font::Pos(5, 125), Palette::Cyan);
            englishFont(String("JP JP Support: ") + (japaneseFont.supportsJapanese() ? "Yes" : "No"), Font::Pos(5, 145), Palette::Green);
            
        } else if (page == 2) {
            // ページ3: フォントポインタ比較
            englishFont("=== Font Pointer Test ===", Font::Pos(5, 25), Palette::White);
            
            Font japaneseFont = FontHelper::JapaneseGothic();
            Font directJapaneseFont(FontRegistry::Japanese::Gothic16);
            
            // フォントポインタ取得
            const lgfx::IFont* helperPtr = japaneseFont.getFontPtr();
            const lgfx::IFont* directPtr = directJapaneseFont.getFontPtr();
            const lgfx::IFont* efontPtr = &fonts::efontJA_16;
            
            char ptrStr[32];
            englishFont("Helper ptr:", Font::Pos(5, 45), Palette::Cyan);
            sprintf(ptrStr, "%p", helperPtr);
            englishFont(ptrStr, Font::Pos(5, 65), Palette::Green);
            
            englishFont("Direct ptr:", Font::Pos(5, 85), Palette::Cyan);
            sprintf(ptrStr, "%p", directPtr);
            englishFont(ptrStr, Font::Pos(5, 105), Palette::Green);
            
            englishFont("efontJA ptr:", Font::Pos(5, 125), Palette::Cyan);
            sprintf(ptrStr, "%p", efontPtr);
            englishFont(ptrStr, Font::Pos(5, 145), Palette::Green);
            
            // 比較結果
            englishFont(String("Helper==Direct: ") + (helperPtr == directPtr ? "Yes" : "No"), Font::Pos(5, 165), Palette::Yellow);
            englishFont(String("Direct==efont: ") + (directPtr == efontPtr ? "Yes" : "No"), Font::Pos(5, 185), Palette::Yellow);
            
        } else if (page == 3) {
            // ページ4: 文字コードテスト
            englishFont("=== Character Code Test ===", Font::Pos(5, 25), Palette::White);
            
            // UTF-8バイト確認
            const char* jpText = "こんにちは";
            englishFont("UTF-8 bytes:", Font::Pos(5, 45), Palette::Cyan);
            
            char byteStr[64] = "";
            for (int i = 0; i < 15 && jpText[i] != '\0'; i++) {
                char temp[8];
                sprintf(temp, "%02X ", (unsigned char)jpText[i]);
                strcat(byteStr, temp);
            }
            englishFont(byteStr, Font::Pos(5, 65), Palette::Green);
            
            // String長さ確認
            String jpString = "こんにちは";
            englishFont(String("String length: ") + String(jpString.length()), Font::Pos(5, 85), Palette::Cyan);
            
            // 直接M5GFXで描画（比較用）
            auto& canvas = System::getInstance().getCanvas();
            canvas.setFont(&fonts::efontJA_16);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString(jpText, 5, 105);
            
            // M5Siv3Dで描画
            Font japaneseFont = FontHelper::JapaneseGothic();
            japaneseFont(jpString, Font::Pos(5, 125), Palette::White);
        }
    }
} 