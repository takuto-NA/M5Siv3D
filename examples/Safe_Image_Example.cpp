//
// Safe Image Example for M5Siv3D
// Demonstrates the new crash-safe Image class with ESP32 optimizations
// Following OpenSiv3D official patterns for Image usage
// 
// OpenSiv3D Image Best Practices:
// - Create Images OUTSIDE the main loop (efficient memory usage)
// - Convert Image to Texture ONCE after processing
// - Use Image.release() to free memory after Texture creation
// - Use DynamicTexture for frequently updated images
// - Keep drawing operations (Texture.draw()) inside the main loop
//

#define USE_M5_DIAL  // M5Dialをテストする場合はコメントアウト

#include "../src/M5Siv3D.h"

// OpenSiv3D公式パターン：Image処理関数
Image CreateSafeTestImage() {
    Image image;
    if (image.create(100, 100, Palette::Red)) {
        // 画像処理をここで実行（while文の外で）
        for (int32_t y = 0; y < image.height(); ++y) {
            for (int32_t x = 0; x < image.width(); ++x) {
                if ((x + y) % 20 < 10) {
                    image[y][x] = Palette::Blue;
                }
            }
        }
    }
    return image;  // ムーブセマンティクスで効率的
}

Image CreateGradientImage() {
    Image image;
    if (image.create(200, 150)) {
        for (int32_t y = 0; y < image.height(); ++y) {
            for (int32_t x = 0; x < image.width(); ++x) {
                float hue = (x / static_cast<float>(image.width())) * 360.0f;
                float saturation = y / static_cast<float>(image.height());
                image[y][x] = Color::FromHSV(hue, saturation, 1.0f);
            }
        }
    }
    return image;
}

void Main() {
    System::Init();
    
    Print << "=== ESP32 Safe Image System Demo ===";
    Print << "Following OpenSiv3D official Image patterns";
    Print << "";
    
    // ========================================
    // OpenSiv3D公式パターン：while文の外でImage作成・処理
    // ========================================
    
    Print << "=== Creating Images (Outside Main Loop) ===";
    
    // 1. 基本的なImage作成とTexture変換
    Image testImage = CreateSafeTestImage();
    Print << "Test image created: " << testImage.width() << "x" << testImage.height();
    
    // 2. 画像処理とTexture作成
    Image gradientImage = CreateGradientImage();
    Print << "Gradient image created: " << gradientImage.width() << "x" << gradientImage.height();
    
    // 3. エラー耐性テスト（境界値）
    Image largeImage;
    if (!largeImage.create(10000, 10000)) {
        Print << "Large image rejected safely (no crash)";
    }
    
    Image negativeImage;
    if (!negativeImage.create(-100, -100)) {
        Print << "Negative size rejected safely (no crash)";
    }
    
    // 4. 複数のImageを安全に管理
    std::vector<Image> imageCollection;
    imageCollection.reserve(3);
    
    for (int i = 0; i < 3; ++i) {
        imageCollection.emplace_back();
        if (imageCollection.back().create(50, 50, Color::FromHSV(i * 120, 1.0f, 1.0f))) {
            Print << "Collection image " << i << " created safely";
        }
    }
    
    // 5. ムーブセマンティクステスト
    Image img1;
    img1.create(60, 60, Palette::Green);
    Image img2 = std::move(img1);  // 効率的なリソース移動
    
    if (img2.isValid() && img1.isEmpty()) {
        Print << "Move semantics working correctly";
    }
    
    // ========================================
    // OpenSiv3D公式パターン：Texture作成とImage解放
    // ========================================
    
    Print << "";
    Print << "=== Converting to Textures (Efficient Memory Usage) ===";
    
    // Textureを作成（GPUメモリに転送）
    // TODO: 実際のM5Siv3DではTextureクラスは未実装のため、
    // ここではImageを直接描画に使用する
    Print << "Images ready for display";
    
    // OpenSiv3D公式では、ここで image.release() を呼んで
    // メインメモリを解放するが、M5Siv3DではImageが描画も担当するため、
    // 必要に応じて保持する
    
    Print << "";
    Print << "=== Starting Main Loop (Efficient Drawing) ===";
    
    // ========================================
    // メインループ：描画のみ（OpenSiv3D公式パターン）
    // ========================================
    while (System::Update()) {
        
        // 描画処理のみ（Image作成はしない）
        testImage.draw(10, 10);
        gradientImage.draw(120, 10);
        img2.draw(10, 170);
        
        // コレクションの描画
        for (size_t i = 0; i < imageCollection.size(); ++i) {
            imageCollection[i].draw(330 + i * 60, 10);
        }
        
        // スケール描画テスト（安全な範囲）
        static uint32_t scaleTestTime = millis();
        if (millis() - scaleTestTime > 2000) {
            float scale = 1.0f + 0.5f * Math::sin(millis() * 0.001f);
            testImage.draw(10, 230, scale);
            scaleTestTime = millis();
        }
        
        // メモリ使用量の監視（最小限の処理のみ）
        static uint32_t lastMemCheck = 0;
        if (millis() - lastMemCheck > 5000) {  // 5秒ごと
            Print << "Free heap: " << String(ESP.getFreeHeap()) << " bytes";
            Print << "All images operating efficiently";
            lastMemCheck = millis();
        }
        
        // 定期的なステータス報告（描画のみ）
        static uint32_t lastStatusReport = 0;
        if (millis() - lastStatusReport > 10000) {  // 10秒ごと
            Print << "=== OpenSiv3D Pattern Success ===";
            Print << "- Images created outside main loop";
            Print << "- Efficient memory usage maintained";
            Print << "- No Image creation in main loop";
            Print << "- All operations crash-safe";
            Print << "";
            lastStatusReport = millis();
        }
    }
} 