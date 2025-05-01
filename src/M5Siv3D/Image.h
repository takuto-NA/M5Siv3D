#pragma once

#include <M5Unified.h>
#include <base64.hpp>
#include "Math.h"
#include "Color.h"
#include "System.h"

class Image {
private:
    M5Canvas* m_canvas;
    bool m_valid = false;
    int32_t m_width = 0;
    int32_t m_height = 0;

public:
    Image() : m_canvas(nullptr) {
        // キャンバスを作成
        m_canvas = new M5Canvas(&M5.Display);
        if (m_canvas) {
            m_canvas->setColorDepth(16);  // 16bitカラーモードを設定
        }
    }
    
    bool loadBase64(const char* base64Data) {
        if (!m_canvas) {
            Serial.println("Canvas not initialized");
            return false;
        }

        // 既存のスプライトを削除
        m_canvas->deleteSprite();
        m_valid = false;
        
        // Base64デコード
        size_t decodedLen = decode_base64_length((unsigned char*)base64Data);
        if (decodedLen == 0) {
            Serial.println("Base64 decode length is 0");
            return false;
        }
        
        uint8_t* decodedData = new uint8_t[decodedLen];
        size_t actualLen = decode_base64((unsigned char*)base64Data, decodedData);
        if (actualLen == 0) {
            Serial.println("Base64 decode failed");
            delete[] decodedData;
            return false;
        }

        // PNGヘッダーからサイズを読み取る (IHDRチャンク)
        if (actualLen < 24 || decodedData[0] != 0x89 || decodedData[1] != 'P' || decodedData[2] != 'N' || decodedData[3] != 'G') {
            Serial.println("Invalid PNG format");
            delete[] decodedData;
            return false;
        }

        // 幅と高さを取得 (ビッグエンディアン)
        m_width = (decodedData[16] << 24) | (decodedData[17] << 16) | (decodedData[18] << 8) | decodedData[19];
        m_height = (decodedData[20] << 24) | (decodedData[21] << 16) | (decodedData[22] << 8) | decodedData[23];
        
        Serial.printf("Image dimensions from PNG header: %dx%d\n", m_width, m_height);

        // 実際のサイズでスプライトを作成
        if (!m_canvas->createSprite(m_width, m_height)) {
            Serial.println("Failed to create sprite");
            delete[] decodedData;
            return false;
        }

        
        // PNG画像を描画
        if (!m_canvas->drawPng(decodedData, actualLen, 0, 0)) {
            Serial.println("Failed to draw PNG");
            delete[] decodedData;
            return false;
        }

        delete[] decodedData;
        m_valid = true;
        
        Serial.println("Image loaded successfully");
        return true;
    }

    bool create(int32_t width, int32_t height, const Color& backgroundColor = Palette::Black) {
        if (!m_canvas) {
            Serial.println("Canvas not initialized");
            return false;
        }

        // 既存のスプライトを削除
        m_canvas->deleteSprite();
        m_valid = false;

        // 新しいサイズでスプライトを作成
        if (!m_canvas->createSprite(width, height)) {
            Serial.println("Failed to create sprite");
            return false;
        }

        m_width = width;
        m_height = height;
        m_canvas->fillSprite(backgroundColor.toRGB565());  
        m_valid = true;
        return true;
    }

    void draw(int32_t x, int32_t y) const {
        if (m_valid && m_canvas) {
            m_canvas->pushSprite(&System::getInstance().getCanvas(), x, y);
        }
    }

    int32_t width() const { return m_width; }
    int32_t height() const { return m_height; }
    bool isEmpty() const { return !m_valid || !m_canvas; }
    Math::Vec2i size() const { return Math::Vec2i(m_width, m_height); }

    void draw(int32_t x, int32_t y, float scale_x, float scale_y) const {
        if (!m_valid || !m_canvas) return;
        
        // Calculate scaled dimensions
        int32_t scaled_w = m_width * scale_x;
        int32_t scaled_h = m_height * scale_y;
        
        // Create temporary canvas for scaling
        M5Canvas temp(&M5.Display);
        temp.createSprite(scaled_w, scaled_h);
        
        // Scale the image
        temp.setPivot(0, 0);
        temp.pushRotateZoom(0, 0, 0, scale_x, scale_y);
        
        // Draw the scaled image to the main canvas
        temp.pushSprite(&System::getInstance().getCanvas(), x, y);
        
        // Clean up
        temp.deleteSprite();
    }

    // Overload for uniform scaling
    void draw(int32_t x, int32_t y, float scale) const {
        draw(x, y, scale, scale);
    }

    ~Image() {
        if (m_canvas) {
            m_canvas->deleteSprite();
            delete m_canvas;
            m_canvas = nullptr;
        }
    }
}; 