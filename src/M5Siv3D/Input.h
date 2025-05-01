#pragma once

#include <M5Unified.h>
#include "Math.h"

namespace Input
{
    class ButtonState
    {
    private:
        m5::Button_Class *m_button;

    public:
        ButtonState(m5::Button_Class *btn) : m_button(btn) {}

        bool down() const { return m_button->isPressed(); }
        bool up() const { return m_button->isReleased(); }
        bool pressed() const { return m_button->wasPressed(); }
        bool released() const { return m_button->wasReleased(); }
        bool pressedDuration(uint32_t ms) const { return m_button->pressedFor(ms); }
        bool releasedDuration(uint32_t ms) const { return m_button->releasedFor(ms); }
    };

    // グローバルなボタンステート
    inline ButtonState ButtonA{&M5.BtnA};
    inline ButtonState ButtonB{&M5.BtnB};
    inline ButtonState ButtonC{&M5.BtnC};

    class IMU
    {
    public:
        // 各軸の角度計算用の構造体
        struct EulerAngles {
            float roll;    // X軸周りの回転（横回転）
            float pitch;   // Y軸周りの回転（縦回転）
            float yaw;     // Z軸周りの回転（水平回転）
        };

        static IMU &getInstance()
        {
            static IMU instance;
            return instance;
        }

        // 現在の姿勢角度を取得
        const EulerAngles& getAngles(float deltaTime)  { 
                    // IMUの姿勢を更新
            updateAttitude(deltaTime);
            return m_currentAngles; }

        // 姿勢を更新（毎フレーム呼び出し）
        void updateAttitude(float deltaTime, float alpha = 0.96f, float gyroScale = 1.0f)
        {
            auto accel = getAccel();
            auto gyro = getGyro();

            // 加速度からの角度計算
            float accelPitch = atan2f(-accel.x, sqrtf(accel.y * accel.y + accel.z * accel.z)) * 180.0f / M_PI;
            float accelRoll = atan2f(accel.y, accel.z) * 180.0f / M_PI;
            float accelYaw = atan2f(accel.x, accel.y) * 180.0f / M_PI;

            // 相補フィルタを各軸に適用
            m_currentAngles.roll = complementaryFilter(accelRoll, gyro.x, deltaTime, m_currentAngles.roll, alpha, gyroScale);
            m_currentAngles.pitch = complementaryFilter(accelPitch, gyro.y, deltaTime, m_currentAngles.pitch, alpha, gyroScale);
            m_currentAngles.yaw = complementaryFilter(accelYaw, gyro.z, deltaTime, m_currentAngles.yaw, alpha, gyroScale);
        }

        /*
         * M5AtomS3のIMU座標系:
         * - Z軸: 画面に垂直な方向（画面裏側が正）
         * - Y軸: デバイスの下方向が正
         * - X軸: デバイスの左方向が正（右向きが負）
         * 
         * 重力加速度の値は約1G（≒9.8m/s²）
         * 例：
         * - デバイスを水平に置いた場合（画面上向き）: (0, 0, +1G)
         * - デバイスを垂直に立てた場合（ケーブルが下向き）: (0, +1G, 0)
         * - デバイスを右に傾けた場合: (-1G, 0, 0)
         */

        // 加速度を取得 (G)
        Math::Vec3f getAccel() const
        {
            auto data = M5.Imu.getImuData();
            return Math::Vec3f(data.accel.x, data.accel.y, data.accel.z);
        }

        // 角速度を取得 (deg/s)
        Math::Vec3f getGyro() const
        {
            auto data = M5.Imu.getImuData();
            return Math::Vec3f(data.gyro.x, data.gyro.y, data.gyro.z);
        }

        // 地磁気を取得 (μT)
        Math::Vec3f getMag() const
        {
            auto data = M5.Imu.getImuData();
            return Math::Vec3f(data.mag.x, data.mag.y, data.mag.z);
        }

    private:
        IMU() : m_currentAngles{0.0f, 0.0f, 0.0f} {}

        // 相補フィルタのヘルパー関数
        float complementaryFilter(float accelAngle, float gyroRate, float deltaTime, float currentAngle, 
                                float alpha, float gyroScale) const {
            float gyroAngle = currentAngle + gyroRate * gyroScale * deltaTime;
            return alpha * gyroAngle + (1.0f - alpha) * accelAngle;
        }

        EulerAngles m_currentAngles;  // 現在の姿勢角度
    };

    // グローバルなIMUインスタンス
    inline IMU &IMU = IMU::getInstance();

    // タッチ入力を管理するクラス
    class TouchInput
    {
    public:
        static TouchInput& getInstance()
        {
            static TouchInput instance;
            return instance;
        }

        // タッチの状態を更新（InputManagerから呼び出される）
        void update()
        {
            m_previousTouchState = m_currentTouchState;
            if (M5.Touch.isEnabled())
            {
                auto t = M5.Touch.getDetail();
                m_currentTouchState.pressed = t.isPressed();
                m_currentTouchState.x = t.x;
                m_currentTouchState.y = t.y;
            }
        }

        // 現在のタッチ位置を取得
        Math::Vec2i pos() const
        {
            return Math::Vec2i(m_currentTouchState.x, m_currentTouchState.y);
        }

        // タッチされているかどうか
        bool pressed() const
        {
            return m_currentTouchState.pressed;
        }

        // タッチが開始されたフレームかどうか
        bool down() const
        {
            return m_currentTouchState.pressed && !m_previousTouchState.pressed;
        }

        // タッチが終了したフレームかどうか
        bool up() const
        {
            return !m_currentTouchState.pressed && m_previousTouchState.pressed;
        }

    private:
        TouchInput() = default;

        struct TouchState
        {
            int32_t x = 0;
            int32_t y = 0;
            bool pressed = false;
        };

        TouchState m_currentTouchState;
        TouchState m_previousTouchState;
    };

    // グローバルなタッチ入力インスタンス
    inline TouchInput& Touch = TouchInput::getInstance();

    class InputManager
    {
    public:
        static InputManager &getInstance()
        {
            static InputManager instance;
            return instance;
        }

        void update()
        {
            M5.update();  // M5の状態を更新
            M5.Imu.update();
            Touch.update();
        }

    private:
        InputManager() = default;
    };
} 