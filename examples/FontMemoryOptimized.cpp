// =============================================================================
// M5Siv3D メモリ最適化版フォントサンプル
// M5Dialなど小容量デバイス向け設定
// =============================================================================

// 段階的最適化設定（必要に応じてコメントアウト）

// レベル1: 装飾フォントのみ無効化（軽微な削減）
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0

// レベル2: 大きなフォントも無効化（中程度の削減）
#define M5SIV3D_ENABLE_LARGE_FONTS 0

// レベル3: 日本語フォント無効化（大幅な削減）
// #define M5SIV3D_ENABLE_JAPANESE_FONTS 0

// レベル4: 太字フォント無効化（最大の削減）
// #define M5SIV3D_ENABLE_BOLD_FONTS 0

#include "M5Siv3D.h"

void Main()
{
    // 利用可能なフォント数を表示
    Print << "=== Memory Optimized Font Demo ===";
    Print << "Available fonts: " << FontRegistry::getAllFonts().size();
    
    // 基本フォント（常に利用可能）
    auto basicFont = EnglishFont(FontCategory::Size::Medium);
    auto smallFont = EnglishFont(FontCategory::Size::Small);
    auto tinyFont = FontHelper::TinyFont();
    
    // 条件付きフォント
    auto monoFont = MonospaceFont(FontCategory::Size::Small);
    
#if M5SIV3D_ENABLE_JAPANESE_FONTS
    auto japaneseFont = JapaneseFont(FontCategory::Size::Medium);
    Print << "Japanese fonts: ENABLED";
#else
    Print << "Japanese fonts: DISABLED (using fallback)";
    auto japaneseFont = EnglishFont(FontCategory::Size::Medium);  // フォールバック
#endif

#if M5SIV3D_ENABLE_BOLD_FONTS
    auto boldFont = FontHelper::EnglishSans(FontCategory::Size::Medium, FontCategory::Weight::Bold);
    Print << "Bold fonts: ENABLED";
#else
    Print << "Bold fonts: DISABLED (using regular)";
    auto boldFont = EnglishFont(FontCategory::Size::Medium);  // フォールバック
#endif

#if M5SIV3D_ENABLE_DECORATIVE_FONTS
    auto decorativeFont = FontHelper::Decorative("Orbitron");
    Print << "Decorative fonts: ENABLED";
#else
    Print << "Decorative fonts: DISABLED (using basic)";
    auto decorativeFont = EnglishFont(FontCategory::Size::Medium);  // フォールバック
#endif

    // アライメント設定
    auto centerFont = EnglishFont(FontCategory::Size::Medium);
    centerFont.setHorizontalAlign(Font::HorizontalAlign::Center);
    
    // 計算値（一度だけ）
    const int centerX = System::Width() / 2;
    const String memoryInfo = "Memory optimized build";
    
    while (System::Update())
    {
        System::SetBackgroundColor(Palette::Lightgray);
        
        // タイトル
        centerFont("Memory Optimized Font Demo", Font::Pos(centerX, 30), Palette::Black);
        
        // 基本フォント（常に利用可能）
        basicFont("Basic English Font", Font::Pos(10, 70), Palette::Black);
        smallFont("Small English Font", Font::Pos(10, 100), Palette::Blue);
        tinyFont("Tiny font (6px)", Font::Pos(10, 120), Palette::Gray);
        
        // 等幅フォント
        monoFont("Monospace: int x = 42;", Font::Pos(10, 150), Palette::Green);
        
        // 条件付きフォント
        japaneseFont("Japanese: こんにちは", Font::Pos(10, 180), Palette::Red);
        boldFont("Bold Text (if enabled)", Font::Pos(10, 210), Palette::Purple);
        decorativeFont("Decorative (if enabled)", Font::Pos(10, 240), Palette::Orange);
        
        // メモリ情報
        smallFont(memoryInfo, Font::Pos(10, 280), Color(100, 100, 100));
        
        // 設定状況表示
        tinyFont("Settings:", Font::Pos(10, 310), Color(80, 80, 80));
        
#if M5SIV3D_ENABLE_JAPANESE_FONTS
        tinyFont("✓ Japanese fonts", Font::Pos(10, 325), Color(0, 150, 0));
#else
        tinyFont("✗ Japanese fonts", Font::Pos(10, 325), Color(150, 0, 0));
#endif

#if M5SIV3D_ENABLE_BOLD_FONTS
        tinyFont("✓ Bold fonts", Font::Pos(10, 340), Color(0, 150, 0));
#else
        tinyFont("✗ Bold fonts", Font::Pos(10, 340), Color(150, 0, 0));
#endif

#if M5SIV3D_ENABLE_DECORATIVE_FONTS
        tinyFont("✓ Decorative fonts", Font::Pos(10, 355), Color(0, 150, 0));
#else
        tinyFont("✗ Decorative fonts", Font::Pos(10, 355), Color(150, 0, 0));
#endif

#if M5SIV3D_ENABLE_LARGE_FONTS
        tinyFont("✓ Large fonts (32px+)", Font::Pos(10, 370), Color(0, 150, 0));
#else
        tinyFont("✗ Large fonts (32px+)", Font::Pos(10, 370), Color(150, 0, 0));
#endif

        // 使用方法の説明
        tinyFont("Edit #define settings at top of file to optimize memory usage", 
                Font::Pos(10, 400), Color(120, 120, 120));
    }
} 