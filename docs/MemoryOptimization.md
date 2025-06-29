# M5Siv3D メモリ最適化ガイド

## 🚨 メモリ不足の問題

M5Stackデバイス（特にM5Dial）では、フラッシュメモリが限られているため、すべてのフォントを有効にするとメモリ不足になる場合があります。

## 💡 解決策：選択的フォント読み込み

M5Siv3Dでは、**遅延読み込み**と**条件付きコンパイル**を組み合わせて、必要なフォントのみをメモリに読み込むことができます。

## 🔧 設定方法

### 1. 基本設定（推奨）

```cpp
// M5Siv3D.hをインクルードする前に設定
#define M5SIV3D_ENABLE_JAPANESE_FONTS 0    // 日本語フォント無効
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0  // 装飾フォント無効
#define M5SIV3D_ENABLE_LARGE_FONTS 0       // 大きなフォント無効
#define M5SIV3D_ENABLE_BOLD_FONTS 0        // 太字フォント無効

#include "M5Siv3D.h"
```

### 2. 段階的最適化

#### レベル1: 装飾フォントのみ無効化（軽微な削減）
```cpp
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#include "M5Siv3D.h"
```

#### レベル2: 大きなフォントも無効化（中程度の削減）
```cpp
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#define M5SIV3D_ENABLE_LARGE_FONTS 0
#include "M5Siv3D.h"
```

#### レベル3: 日本語フォント無効化（大幅な削減）
```cpp
#define M5SIV3D_ENABLE_JAPANESE_FONTS 0
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#define M5SIV3D_ENABLE_LARGE_FONTS 0
#include "M5Siv3D.h"
```

#### レベル4: 最小構成（最大の削減）
```cpp
#define M5SIV3D_ENABLE_JAPANESE_FONTS 0
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#define M5SIV3D_ENABLE_LARGE_FONTS 0
#define M5SIV3D_ENABLE_BOLD_FONTS 0
#include "M5Siv3D.h"
```

## 📊 メモリ使用量の目安

| 設定 | フラッシュメモリ削減量 | 利用可能フォント |
|------|----------------------|------------------|
| 全有効 | 0% | 全フォント（50個） |
| レベル1 | ~10% | 装飾フォント以外（45個） |
| レベル2 | ~25% | 28px以下のフォント（35個） |
| レベル3 | ~60% | 英語フォントのみ（25個） |
| レベル4 | ~70% | 基本英語フォントのみ（15個） |

## 🎯 用途別推奨設定

### 英語のみのアプリケーション
```cpp
#define M5SIV3D_ENABLE_JAPANESE_FONTS 0
#include "M5Siv3D.h"
```

### 日本語対応だが装飾不要
```cpp
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#define M5SIV3D_ENABLE_LARGE_FONTS 0
#include "M5Siv3D.h"
```

### 最小限の表示機能のみ
```cpp
#define M5SIV3D_ENABLE_JAPANESE_FONTS 0
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#define M5SIV3D_ENABLE_LARGE_FONTS 0
#define M5SIV3D_ENABLE_BOLD_FONTS 0
#include "M5Siv3D.h"
```

## 🔄 フォールバック機能

フォントが無効化されている場合、自動的に基本フォントにフォールバックします：

```cpp
// 日本語フォントが無効の場合
auto font = JapaneseFont();  // → 自動的に基本英語フォントを使用

// 装飾フォントが無効の場合
auto font = FontHelper::Decorative("Orbitron");  // → 基本フォントを使用
```

## 🛠️ 実装例

### メモリ最適化版のサンプル

```cpp
// メモリ最適化設定
#define M5SIV3D_ENABLE_JAPANESE_FONTS 0
#define M5SIV3D_ENABLE_DECORATIVE_FONTS 0
#define M5SIV3D_ENABLE_LARGE_FONTS 0

#include "M5Siv3D.h"

void Main()
{
    // 利用可能なフォントのみ使用
    auto basicFont = EnglishFont(FontCategory::Size::Medium);
    auto smallFont = EnglishFont(FontCategory::Size::Small);
    auto monoFont = MonospaceFont(FontCategory::Size::Small);
    
    while (System::Update())
    {
        basicFont("Hello World", Font::Pos(10, 50), Palette::Black);
        smallFont("Small text", Font::Pos(10, 80), Palette::Blue);
        monoFont("Code: int x = 0;", Font::Pos(10, 110), Palette::Green);
    }
}
```

## 📈 パフォーマンス向上

遅延読み込みにより、以下の効果が期待できます：

1. **起動時間短縮**: 使用しないフォントは読み込まれない
2. **メモリ使用量削減**: 必要なフォントのみメモリに配置
3. **コンパイル時間短縮**: 無効化されたフォントはコンパイル対象外

## ⚠️ 注意事項

1. **設定はインクルード前に**: `#include "M5Siv3D.h"`の前に設定する必要があります
2. **フォールバック確認**: 無効化したフォントを使用する場合、適切にフォールバックされることを確認してください
3. **段階的テスト**: 一度にすべて無効化せず、段階的にテストすることを推奨します

## 🔍 デバッグ方法

利用可能なフォント数を確認：

```cpp
void Main()
{
    Print << "Available fonts: " << FontRegistry::getAllFonts().size();
    FontHelper::PrintAvailableFonts();  // 詳細一覧表示
}
```

これにより、現在の設定でどのフォントが利用可能かを確認できます。 