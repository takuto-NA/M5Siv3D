#pragma once

#include <sstream>
#include "System.h"

class PrintManager {
private:
    std::stringstream m_buffer;
    int32_t m_cursorX = 0;
    int32_t m_cursorY = 0;

public:
    static PrintManager& getInstance() {
        static PrintManager instance;
        return instance;
    }

    template <typename T>
    PrintManager& operator<<(const T& value) {
        m_buffer << value << '\n';  // 常に改行を追加
        return *this;
    }

    void clear() {
        m_buffer.str("");
        m_buffer.clear();
        m_cursorX = 0;
        m_cursorY = 0;
    }

    void draw() {
        if (m_buffer.str().empty()) return;

        auto& canvas = System::getInstance().getCanvas();
        canvas.setCursor(m_cursorX, m_cursorY);
        canvas.print(m_buffer.str().c_str());
    }

private:
    PrintManager() = default;
};

// グローバル関数として定義
inline PrintManager& Print = PrintManager::getInstance();

inline void ClearPrint() {
    Print.clear();
}

// システムのendDraw()内で呼び出すための描画関数
inline void drawPrint() {
    Print.draw();
} 