#include <M5Siv3D.h>

void Main() {
    // C++17 フォントシステムの初期化
    FontSystem::FontRegistry::initializeDefaultFonts();
    
    // C++17 auto type deduction でフォント作成
    auto japaneseFont = FontSystem::FontFactory<>::createJapaneseGothic(FontCategory::Size::Medium);
    auto englishFont = FontSystem::FontFactory<>::createEnglishSans(FontCategory::Size::Medium);
    auto boldFont = FontSystem::FontFactory<>::createEnglishSans(FontCategory::Size::Medium, FontCategory::Weight::Bold);
    
    // C++17 structured bindings でフォント情報取得
    const auto& [name, category, weight, style, size, height, japanese] = japaneseFont.getDescriptor();
    
    Print << "Font: " << name;
    Print << "Category: " << static_cast<int>(category);
    Print << "Supports Japanese: " << (japanese ? "Yes" : "No");
    
    while (System::Update()) {
        // C++17 constexpr if による条件付き描画
        if constexpr (M5SIV3D_ENABLE_JAPANESE_FONTS) {
            // 日本語フォントが有効な場合
            if (japaneseFont) {  // explicit bool conversion
                Font(japaneseFont.getFont()).draw("こんにちは世界", 10, 50, Palette::White);
            }
        }
        
        // C++17 optional を使った安全なフォント検索
        if (auto font = FontSystem::FontRegistry::findFont("english_sans_medium")) {
            Font(font->getFont()).draw("Hello World", 10, 100, Palette::Green);
        }
        
        // C++17 auto return type deduction
        auto basicFont = FontSystem::FontFactory<>::createBasicFont(FontCategory::Size::Small);
        Font(basicFont.getFont()).draw("Basic Font (Always Available)", 10, 150, Palette::Yellow);
        
        // フォント情報の表示
        Print << "Available fonts:";
        for (const auto& [fontName, fontHandle] : FontSystem::FontRegistry::getAllFonts()) {
            Print << "- " << fontName << " (" << fontHandle->getPixelHeight() << "px)";
        }
        
        drawPrint();
    }
} 