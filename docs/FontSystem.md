# M5Siv3D フォントシステム

M5Siv3Dは、M5Unifiedに組み込まれている豊富なフォントを簡単に使用できるフォントシステムを提供します。

## 🎯 特徴

- **簡単な選択**: `JapaneseFont()`, `EnglishFont()`, `MonospaceFont()` などの便利関数
- **自動選択**: `AutoFont()` でテキスト内容に応じて最適なフォントを自動選択
- **カテゴリ分類**: 日本語、英語、等幅、装飾フォントに分類
- **GUI選択**: SimpleGUIでインタラクティブなフォント選択
- **豊富な種類**: 50種類以上のフォントをサポート

## 📚 利用可能なフォント

### 🇯🇵 日本語フォント
- **ゴシック体**: 8px〜40px（9サイズ）
- **明朝体**: 8px〜40px（9サイズ）

### 🇬🇧 英語フォント
- **FreeSans**: Regular/Bold（9pt〜24pt）
- **FreeMono**: Regular/Bold（9pt〜24pt）
- **FreeSerif**: Regular（9pt〜24pt）
- **基本フォント**: Font0〜Font8

### 🎨 装飾フォント
- **Orbitron**: 未来的なデザイン
- **Roboto**: モダンなデザイン
- **Satisfy**: 手書き風
- **Yellowtail**: エレガントなスクリプト

## 🚀 基本的な使い方

### 1. 簡単なフォント選択

```cpp
#include "M5Siv3D.h"

void Main()
{
    while (System::Update())
    {
        // 日本語フォント（中サイズ）
        auto japaneseFont = JapaneseFont(FontCategory::Size::Medium);
        japaneseFont("こんにちは世界！", Font::Pos(10, 50), Palette::Black);
        
        // 英語フォント（大サイズ、太字）
        auto englishBold = FontHelper::EnglishSans(FontCategory::Size::Large, FontCategory::Weight::Bold);
        englishBold("Hello World!", Font::Pos(10, 100), Palette::Blue);
        
        // 等幅フォント（コード表示に最適）
        auto monoFont = MonospaceFont(FontCategory::Size::Small);
        monoFont("int main() { return 0; }", Font::Pos(10, 150), Palette::Green);
    }
}
```

### 2. 自動フォント選択

```cpp
// テキスト内容に応じて自動的に最適なフォントを選択
auto autoFont1 = AutoFont("English Text", FontCategory::Size::Medium);
autoFont1("Auto: English Text", Font::Pos(10, 200), Palette::Red);

auto autoFont2 = AutoFont("日本語テキスト", FontCategory::Size::Medium);
autoFont2("Auto: 日本語テキスト", Font::Pos(10, 230), Palette::Red);
```

### 3. フォント情報の取得

```cpp
auto font = JapaneseFont(FontCategory::Size::Large);

// フォント名を取得
String fontName = font.getFontName();

// 日本語対応チェック
bool supportsJapanese = font.supportsJapanese();

// フォント情報を取得
const FontInfo* info = font.getFontInfo();
if (info) {
    Print << "Font: " << info->name;
    Print << "Height: " << info->pixelHeight << "px";
    Print << "Category: " << static_cast<int>(info->category);
}
```

## 🎛️ GUI フォント選択

SimpleGUIを使用してインタラクティブなフォント選択が可能です。

```cpp
void Main()
{
    Font selectedFont = JapaneseFont(FontCategory::Size::Medium);
    FontCategory::Size selectedSize = FontCategory::Size::Medium;
    
    while (System::Update())
    {
        // フォント選択ドロップダウン
        if (SimpleGUI::FontSelector(selectedFont, "Font", 
                                   Math::Vec2i(10, 50), 
                                   FontCategory::Type::Japanese, 280)) {
            Print << "Font changed to: " << selectedFont.getFontName();
        }
        
        // フォントサイズ選択
        if (SimpleGUI::FontSizeSelector(selectedSize, "Size", 
                                       Math::Vec2i(10, 100), 150)) {
            Print << "Size changed";
        }
        
        // フォントプレビュー
        SimpleGUI::FontPreview(selectedFont, "Hello こんにちは", 
                              Math::Vec2i(10, 200), 300, 80);
    }
}
```

## 📖 詳細API

### FontHelper 名前空間

```cpp
namespace FontHelper
{
    // 日本語フォント
    Font JapaneseGothic(FontCategory::Size size = FontCategory::Size::Medium);
    Font JapaneseMincho(FontCategory::Size size = FontCategory::Size::Medium);
    
    // 英語フォント
    Font EnglishSans(FontCategory::Size size, FontCategory::Weight weight = FontCategory::Weight::Regular);
    Font EnglishMono(FontCategory::Size size, FontCategory::Weight weight = FontCategory::Weight::Regular);
    Font EnglishSerif(FontCategory::Size size);
    
    // 装飾フォント
    Font Decorative(const String& name);  // "Orbitron", "Roboto", "Satisfy", "Yellowtail"
    
    // 特殊フォント
    Font TinyFont();  // 6px極小フォント
    
    // 自動選択
    Font AutoSelect(const String& text, FontCategory::Size preferredSize);
    
    // フォント一覧表示
    void PrintAvailableFonts();
}
```

### FontRegistry 名前空間

```cpp
namespace FontRegistry
{
    // フォント検索
    std::vector<const FontInfo*> getAllFonts();
    std::vector<const FontInfo*> getFontsByCategory(FontCategory::Type category);
    std::vector<const FontInfo*> getFontsBySize(FontCategory::Size size);
    std::vector<const FontInfo*> getJapaneseFonts();
    
    // 個別フォントアクセス
    namespace Japanese { /* Gothic8, Gothic12, ..., Mincho8, Mincho12, ... */ }
    namespace English { /* Font0, FreeSans9, FreeMono12, ... */ }
    namespace Decorative { /* Orbitron24, Roboto24, ... */ }
}
```

### SimpleGUI フォント機能

```cpp
namespace SimpleGUI
{
    // フォント選択ドロップダウン
    bool FontSelector(Font& selectedFont, const String& label,
                     const Math::Vec2i& pos,
                     FontCategory::Type category = FontCategory::Type::ASCII,
                     int32_t width = 240, bool enabled = true);
    
    // フォントサイズ選択
    bool FontSizeSelector(FontCategory::Size& selectedSize, const String& label,
                         const Math::Vec2i& pos,
                         int32_t width = 120, bool enabled = true);
    
    // フォントプレビュー
    void FontPreview(const Font& font, const String& previewText,
                    const Math::Vec2i& pos,
                    int32_t width = 240, int32_t height = 48);
}
```

## 🎨 フォントカテゴリ

### FontCategory::Type
- `ASCII`: 英語・ASCII文字専用
- `Japanese`: 日本語対応
- `Decorative`: 装飾フォント
- `Monospace`: 等幅フォント

### FontCategory::Size
- `XSmall`: 8-10px
- `Small`: 12-14px
- `Medium`: 16-18px
- `Large`: 20-24px
- `XLarge`: 28-32px
- `XXLarge`: 36px以上

### FontCategory::Weight
- `Thin`: 細字
- `Light`: 軽字
- `Regular`: 標準
- `Bold`: 太字

## 💡 使用のコツ

1. **日本語を含むテキスト**: `JapaneseFont()` または `AutoFont()` を使用
2. **英語のみ**: `EnglishFont()` で軽量化
3. **コード表示**: `MonospaceFont()` で等幅表示
4. **デザイン重視**: `FontHelper::Decorative()` で装飾フォント
5. **メモリ節約**: 大きなフォント（32px以上）は必要な時のみ使用

## 📝 サンプルコード

- `examples/FontBasicUsage.cpp`: 基本的な使用方法
- `examples/FontDemo.cpp`: GUI選択機能のデモ

## 🔧 カスタマイズ

独自のフォントを追加したい場合は、`FontRegistry` 名前空間に新しい `FontInfo` を追加してください。

```cpp
namespace FontRegistry::Custom
{
    inline constexpr FontInfo MyFont{&fonts::MyCustomFont, "My Custom Font", 
        FontCategory::Type::ASCII, FontCategory::Weight::Regular, 
        FontCategory::Style::Normal, FontCategory::Size::Medium, 16};
}
``` 