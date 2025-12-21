//
// M5Dial Example (M5Siv3D)
// - `examples/M5Dial_Example.cpp` をベースに、現行の M5Siv3D API（optional返り値）へ合わせた実装
//

#include "M5Siv3D.h"

namespace {

// M5Dialは円形表示のため、四隅が欠ける。
// そのため座標は「中心 + 円の内側マージン」基準で配置する。
constexpr int32_t kCircleOuterMarginPixels = 6;

constexpr int32_t kTitleOffsetFromTopPixels = 16;
constexpr int32_t kHelpOffsetFromBottomLine1Pixels = 22;
constexpr int32_t kHelpOffsetFromBottomLine2Pixels = 10;

constexpr int32_t kGaugeOffsetFromCircleEdgePixels = 42;
constexpr int32_t kGaugeInnerRingOffsetPixels = 10;
constexpr int32_t kNeedleOffsetPixels = 16;
constexpr int32_t kCenterDotRadiusPixels = 5;

constexpr int32_t kEncoderValueOffsetPixels = 0;
constexpr int32_t kEncoderDeltaOffsetPixels = 34;
constexpr int32_t kAngleLabelOffsetFromTopPixels = 34;
constexpr int32_t kBottomValueOffsetFromBottomPixels = 40;

constexpr float kDegreesPerEncoderStep = 5.0f;
constexpr float kAngleDegreesFull = 360.0f;

enum class DisplayMode : int32_t {
    EncoderValue = 0,
    CircularDisplay = 1,
    RfidStatus = 2,
    Count,
};

struct AppState {
    long lastEncoderValue = 0;
    DisplayMode displayMode = DisplayMode::EncoderValue;
    Color currentColor = Palette::White;
    float hueDegrees = 0.0f;
};

struct CircleLayout {
    int32_t centerX = 0;
    int32_t centerY = 0;
    int32_t radius = 0;
    int32_t titleY = 0;
    int32_t helpLine1Y = 0;
    int32_t helpLine2Y = 0;
};

CircleLayout getCircleLayout() {
    const int32_t width = System::Width();
    const int32_t height = System::Height();
    const int32_t centerX = width / 2;
    const int32_t centerY = height / 2;
    const int32_t minSide = (width < height) ? width : height;
    const int32_t radius = (minSide / 2) - kCircleOuterMarginPixels;

    CircleLayout layout{};
    layout.centerX = centerX;
    layout.centerY = centerY;
    layout.radius = radius;
    layout.titleY = (centerY - radius) + kTitleOffsetFromTopPixels;
    layout.helpLine1Y = (centerY + radius) - kHelpOffsetFromBottomLine1Pixels;
    layout.helpLine2Y = (centerY + radius) - kHelpOffsetFromBottomLine2Pixels;
    return layout;
}

String getModeText(DisplayMode mode) {
    switch (mode) {
    case DisplayMode::EncoderValue:
        return "Encoder Value";
    case DisplayMode::CircularDisplay:
        return "Circular Display";
    case DisplayMode::RfidStatus:
        return "RFID Status";
    default:
        return "Unknown";
    }
}

void drawTitle(const CircleLayout& layout, const DisplayMode mode) {
    Font titleFont;
    titleFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);

    titleFont("M5Dial: " + getModeText(mode), Font::Pos(layout.centerX, layout.titleY), Palette::White);
}

void drawFooterHelp(const CircleLayout& layout) {
    Font helpFont;
    helpFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);

    helpFont("Rotate: Change value", Font::Pos(layout.centerX, layout.helpLine1Y), Palette::Gray);
    helpFont("Button A: Reset / Change mode", Font::Pos(layout.centerX, layout.helpLine2Y), Palette::Gray);
}

void drawEncoderValuePanel(const CircleLayout& layout, const long encoderValue, const Color& currentColor) {
    Font largeFont;
    largeFont.setSize(3)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);

    largeFont(String(encoderValue), Font::Pos(layout.centerX, layout.centerY + kEncoderValueOffsetPixels), currentColor);

    const auto encoderDelta = Input::Encoder.getDelta();
    if (!encoderDelta.has_value() || encoderDelta.value() == 0) {
        return;
    }

    Font smallFont;
    smallFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);

    const String deltaText = "Δ" + String(encoderDelta.value());
    const auto deltaColor = (encoderDelta.value() > 0) ? Palette::Green : Palette::Red;
    smallFont(deltaText, Font::Pos(layout.centerX, layout.centerY + kEncoderDeltaOffsetPixels), deltaColor);
}

void drawCircularDisplayPanel(const CircleLayout& layout, const long encoderValue, const Color& currentColor) {
    const int32_t requestedGaugeRadius = layout.radius - kGaugeOffsetFromCircleEdgePixels;
    const int32_t gaugeRadius = (requestedGaugeRadius < 10) ? 10 : requestedGaugeRadius;
    const int32_t centerX = layout.centerX;
    const int32_t centerY = layout.centerY;

    Circle(centerX, centerY, gaugeRadius).drawFrame(Palette::White);
    Circle(centerX, centerY, gaugeRadius - kGaugeInnerRingOffsetPixels).drawFrame(Palette::Darkgray);

    const float angleDegrees = Math::fmod(static_cast<float>(encoderValue), kAngleDegreesFull);
    const float angleRadians = angleDegrees * Math::Pi / 180.0f;

    const int needleX = centerX + (gaugeRadius - kNeedleOffsetPixels) * cos(angleRadians - Math::HalfPi);
    const int needleY = centerY + (gaugeRadius - kNeedleOffsetPixels) * sin(angleRadians - Math::HalfPi);

    Line(centerX, centerY, needleX, needleY).draw(currentColor);
    Circle(centerX, centerY, kCenterDotRadiusPixels).draw(currentColor);

    for (int tickIndex = 0; tickIndex < 12; ++tickIndex) {
        const float tickAngle = tickIndex * Math::Pi / 6;
        const int tick1X = centerX + (gaugeRadius - 5) * cos(tickAngle - Math::HalfPi);
        const int tick1Y = centerY + (gaugeRadius - 5) * sin(tickAngle - Math::HalfPi);
        const int tick2X = centerX + gaugeRadius * cos(tickAngle - Math::HalfPi);
        const int tick2Y = centerY + gaugeRadius * sin(tickAngle - Math::HalfPi);
        Line(tick1X, tick1Y, tick2X, tick2Y).draw(Palette::White);
    }

    Font valueFont;
    valueFont.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center);
    valueFont(String(encoderValue), Font::Pos(centerX, layout.helpLine1Y - kBottomValueOffsetFromBottomPixels), Palette::Green);

    Font angleFont;
    angleFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);
    angleFont("Angle: " + String(static_cast<int>(angleDegrees)) + "°", Font::Pos(centerX, layout.titleY + kAngleLabelOffsetFromTopPixels), Palette::White);
}

void drawRfidStatusPanel(const CircleLayout& layout) {
    Font titleFont;
    titleFont.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center);
    titleFont("RFID Reader", Font::Pos(layout.centerX, layout.titleY + kAngleLabelOffsetFromTopPixels), Palette::Cyan);

    Font statusFont;
    statusFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);

    // NOTE: 現状 `SafeDialRFID` は未実装箇所があり、カード検出は常に false になり得ます。
    // 将来的に M5Dial の MFRC522 API をここへ接続します。
    if (!Input::RFID.isCardPresent()) {
        statusFont("No Card", Font::Pos(layout.centerX, layout.centerY - 10), Palette::Red);
        statusFont("Place card near device", Font::Pos(layout.centerX, layout.centerY + 10), Palette::Gray);
        Circle(layout.centerX, layout.centerY + 40, 20).drawFrame(Palette::Red);
        return;
    }

    statusFont("Card Detected!", Font::Pos(layout.centerX, layout.centerY - 10), Palette::Green);

    const auto uid = Input::RFID.readCardUID();
    if (uid.has_value()) {
        statusFont("UID: " + uid.value(), Font::Pos(layout.centerX, layout.centerY + 10), Palette::Yellow);
    }
    Circle(layout.centerX, layout.centerY + 40, 20).draw(Palette::Green);
}

DisplayMode nextMode(DisplayMode mode) {
    const auto next = (static_cast<int32_t>(mode) + 1) % static_cast<int32_t>(DisplayMode::Count);
    return static_cast<DisplayMode>(next);
}

}  // namespace

void Main() {
    System::SetBackgroundColor(Palette::Black);

    AppState appState{};
    const auto layout = getCircleLayout();

    Print << "M5Siv3D with M5Dial Example";
    Print << "Rotate encoder to change values";
    Print << "Press Button A to change modes";

    while (System::Update()) {
        const long encoderValue = Input::Encoder.getValueOr(appState.lastEncoderValue);

        if (encoderValue != appState.lastEncoderValue) {
            appState.lastEncoderValue = encoderValue;
            appState.hueDegrees = Math::fmod(encoderValue * kDegreesPerEncoderStep, kAngleDegreesFull);
            appState.currentColor = Color::FromHSV(appState.hueDegrees, 1.0f, 1.0f);
        }

        if (Input::ButtonA.pressed()) {
            if (appState.displayMode == DisplayMode::RfidStatus) {
                appState.displayMode = DisplayMode::EncoderValue;
            } else {
                Input::Encoder.reset();
                appState.lastEncoderValue = 0;
                appState.displayMode = nextMode(appState.displayMode);
            }
        }

        // 円形表示に合わせ、UIは全て中心基準で配置する
        drawTitle(layout, appState.displayMode);

        switch (appState.displayMode) {
        case DisplayMode::EncoderValue:
            drawEncoderValuePanel(layout, encoderValue, appState.currentColor);
            break;
        case DisplayMode::CircularDisplay:
            drawCircularDisplayPanel(layout, encoderValue, appState.currentColor);
            break;
        case DisplayMode::RfidStatus:
            drawRfidStatusPanel(layout);
            break;
        default:
            break;
        }

        drawFooterHelp(layout);

        ClearPrint();
        drawPrint();
    }
}
