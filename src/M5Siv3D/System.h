#pragma once

#include <M5Unified.h>
#include "Color.h"
#include "Input.h"

class System
{
public:
    // シングルトンインスタンスの取得
    static System &getInstance()
    {
        static System instance;
        return instance;
    }

    // システムの初期化
    void init()
    {
        auto cfg = M5.config();
        M5.begin(cfg);

        // キャンバスを画面のサイズで初期化
        canvas.createSprite(M5.Display.width(), M5.Display.height());
        canvas.setTextSize(2);
        lastDrawTime = millis();
    }

    static void Init()
    {
        getInstance().init();
    }

    // 描画の開始
    void beginDraw()
    {
        canvas.fillSprite(m_backgroundColor.toRGB565());
    }

    // 描画の終了と画面更新
    void endDraw()
    {
        canvas.pushSprite(0, 0);
    }

    static void SetBackgroundColor(const Color &color)
    {
        getInstance().setBackgroundColor(color);
    }

    void setBackgroundColor(const Color &color)
    {
        m_backgroundColor = color;
    }

    static bool Update()
    {
        return getInstance().update();
    }

    // メインループの更新処理
    bool update()
    {
        uint32_t currentTime = millis();
        uint32_t elapsedTime = currentTime - lastDrawTime;

        if (elapsedTime >= FRAME_INTERVAL)
        {
            endDraw();
            updateTime();
            lastDrawTime = currentTime;

            int32_t remaining = FRAME_INTERVAL - (millis() - currentTime);
            if (remaining > 0)
            {
                M5.delay(remaining);
            }

            Input::InputManager::getInstance().update();
            beginDraw();
        }
        return true;
    }

    // キャンバスへのアクセス
    M5Canvas &getCanvas()
    {
        return canvas;
    }

    // 画面の幅と高さの取得
    static int Width()
    {
        return getInstance().getWidth();
    }

    static int Height()
    {
        return getInstance().getHeight();
    }

    int getWidth() const { return M5.Display.width(); }
    int getHeight() const { return M5.Display.height(); }

    // 時間管理関連のメソッドを追加

    static float DeltaTime()
    {
        return getInstance().getDeltaTime();
    }

    static float FPS()
    {
        return getInstance().getFPS();
    }

    static uint64_t FrameCount()
    {
        return getInstance().getFrameCount();
    }

    float getDeltaTime() const { return m_deltaTime; }
    float getFPS() const { return 1000.0f / m_averageFrameTime; }
    uint64_t getFrameCount() const { return m_frameCount; }

    // 経過時間を秒単位で取得
    double getElapsedTimeS() const
    {
        return millis() / 1000.0;
    }

    double getElapsedTimeMS() const
    {
        return millis();
    }

private:

    System() {} // プライベートコンストラクタ

    // システム変数
    static constexpr int FRAME_INTERVAL = 16; // 約60FPS
    uint32_t lastDrawTime = 0;
    M5Canvas canvas{&M5.Display};

    // コピー禁止
    System(const System &) = delete;
    System &operator=(const System &) = delete;

    Color m_backgroundColor = Palette::Black;

    // 時間管理用メンバ変数
    float m_deltaTime = 0.0f;
    float m_averageFrameTime = FRAME_INTERVAL;
    uint64_t m_frameCount = 0;
    uint32_t m_previousTime = 0;

    // Update内で呼び出す時間更新処理
    void updateTime()
    {
        uint32_t currentTime = millis();
        m_deltaTime = (currentTime - m_previousTime) / 1000.0f;

        // 移動平均でFPSを計算
        m_averageFrameTime = m_averageFrameTime * 0.9f + (currentTime - m_previousTime) * 0.1f;

        m_previousTime = currentTime;
        m_frameCount++;
    }
}; 