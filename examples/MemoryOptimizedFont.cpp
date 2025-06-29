// メモリ最適化例：M5Dial用の最小構成
// 日本語フォントを無効化してメモリを節約

#define M5SIV3D_ENABLE_JAPANESE_FONTS 0    // 日本語フォント無効（大幅メモリ節約）
#define M5SIV3D_ENABLE_LARGE_FONTS 0       // 大きなフォント無効
#define M5SIV3D_ENABLE_BOLD_FONTS 0        // ボールドフォント無効
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0  // 装飾フォント無効

#include <M5Siv3D.h>

void Main() {
    // C++17 テンプレート特殊化による最適化
    using OptimizedFactory = FontSystem::FontFactory<false, false, false, false>;
    
    // 基本フォントのみ使用（メモリ効率最大）
    auto tinyFont = OptimizedFactory::createBasicFont(FontCategory::Size::XSmall);
    auto smallFont = OptimizedFactory::createBasicFont(FontCategory::Size::Small);
    auto mediumFont = OptimizedFactory::createBasicFont(FontCategory::Size::Medium);
    
    Print << "Memory Optimized Font System";
    Print << "Japanese fonts: " << (M5SIV3D_ENABLE_JAPANESE_FONTS ? "Enabled" : "Disabled");
    Print << "Large fonts: " << (M5SIV3D_ENABLE_LARGE_FONTS ? "Enabled" : "Disabled");
    Print << "Bold fonts: " << (M5SIV3D_ENABLE_BOLD_FONTS ? "Enabled" : "Disabled");
    
    while (System::Update()) {
        // C++17 constexpr if による条件付きコンパイル
        if constexpr (!M5SIV3D_ENABLE_JAPANESE_FONTS) {
            Font(mediumFont.getFont()).draw("Japanese fonts disabled", 10, 50, Palette::Red);
            Font(smallFont.getFont()).draw("Memory usage optimized", 10, 80, Palette::Green);
        }
        
        // 基本フォントは常に利用可能
        Font(tinyFont.getFont()).draw("Tiny font (5px)", 10, 110, Palette::White);
        Font(smallFont.getFont()).draw("Small font (8px)", 10, 130, Palette::White);
        Font(mediumFont.getFont()).draw("Medium font (16px)", 10, 160, Palette::White);
        
        // C++17 auto + structured bindings でフォント情報表示
        const auto& desc = mediumFont.getDescriptor();
        Font(smallFont.getFont()).draw(
            String("Font: ") + String(desc.name.data()) + 
            " (" + String(desc.pixelHeight) + "px)", 
            10, 190, Palette::Yellow
        );
        
        drawPrint();
    }
}

// C++17 コンパイル時メモリ使用量チェック
static_assert(!M5SIV3D_ENABLE_JAPANESE_FONTS, "Japanese fonts should be disabled for memory optimization");
static_assert(!M5SIV3D_ENABLE_LARGE_FONTS, "Large fonts should be disabled for memory optimization"); 