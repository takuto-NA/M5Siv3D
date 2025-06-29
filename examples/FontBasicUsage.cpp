// =============================================================================
// メモリ最適化設定 - M5Dialなど小容量デバイス向け
// =============================================================================

// 日本語フォントを無効化してメモリを節約（英語のみの場合）
// #define M5SIV3D_ENABLE_JAPANESE_FONTS 0

// 装飾フォントを無効化してメモリを節約
// #define M5SIV3D_ENABLE_DECORATIVE_FONTS 0

// 大きなフォント（32px以上）を無効化してメモリを節約
// #define M5SIV3D_ENABLE_LARGE_FONTS 0

// 太字フォントを無効化してメモリを節約
// #define M5SIV3D_ENABLE_BOLD_FONTS 0

#include "M5Siv3D.h"

void Main()
{
    // フォントオブジェクトは一度だけ作成（パフォーマンス向上）
    auto japaneseFont = JapaneseFont(FontCategory::Size::Large);
    auto englishBold = FontHelper::EnglishSans(FontCategory::Size::Medium, FontCategory::Weight::Bold);
    auto monoFont = MonospaceFont(FontCategory::Size::Small);
    auto autoFont1 = AutoFont("English Text", FontCategory::Size::Medium);
    auto autoFont2 = AutoFont("日本語テキスト", FontCategory::Size::Medium);
    auto decorativeFont = FontHelper::Decorative("Orbitron");
    auto tinyFont = FontHelper::TinyFont();
    
    // アライメント設定済みフォント
    auto centerFont = EnglishFont(FontCategory::Size::Medium);
    centerFont.setHorizontalAlign(Font::HorizontalAlign::Center);
    
    auto rightFont = EnglishFont(FontCategory::Size::Medium);
    rightFont.setHorizontalAlign(Font::HorizontalAlign::Right);
    
    auto infoFont = EnglishFont(FontCategory::Size::XSmall);
    auto helpFont = EnglishFont(FontCategory::Size::XSmall);
    
    // 一度だけ計算する値
    const String info = "Available fonts: " + String(FontRegistry::getAllFonts().size());
    const int centerX = System::Width() / 2;
    const int rightX = System::Width() - 10;
    
    while (System::Update())
    {
        System::SetBackgroundColor(Palette::White);
        
        // 1. 簡単な日本語フォント使用
        japaneseFont("こんにちは世界！", Font::Pos(10, 50), Palette::Black);
        
        // 2. 英語フォント（太字）
        englishBold("Hello World (Bold)", Font::Pos(10, 100), Palette::Blue);
        
        // 3. 等幅フォント（コード表示に最適）
        monoFont("int main() { return 0; }", Font::Pos(10, 130), Palette::Green);
        
        // 4. 自動フォント選択（テキストに応じて最適なフォントを選択）
        autoFont1("Auto: English Text", Font::Pos(10, 160), Palette::Red);
        autoFont2("Auto: 日本語テキスト", Font::Pos(10, 190), Palette::Red);
        
        // 5. 装飾フォント
        decorativeFont("Decorative Font", Font::Pos(10, 230), Palette::Purple);
        
        // 6. 極小フォント
        tinyFont("Tiny font for small spaces", Font::Pos(10, 270), Palette::Gray);
        
        // 7. 中央揃え・右揃えの例
        centerFont("Centered Text", Font::Pos(centerX, 320), Palette::Black);
        rightFont("Right Aligned", Font::Pos(rightX, 350), Palette::Black);
        
        // 8. フォント情報の表示
        infoFont(info, Font::Pos(10, 400), Color(100, 100, 100));
        
        // 9. 使用方法の説明
        helpFont("Use JapaneseFont(), EnglishFont(), MonospaceFont(), AutoFont() for easy font selection", 
                Font::Pos(10, 420), Color(120, 120, 120));
    }
} 