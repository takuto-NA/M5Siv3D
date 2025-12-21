//
// M5Dial Example (M5Siv3D)
// - `examples/M5Dial_Example.cpp` をベースに、現行の M5Siv3D API（optional返り値）へ合わせた実装
//

#include "M5Siv3D.h"

namespace {

constexpr int32_t kHeaderHeight = 30;
constexpr int32_t kContentTop = kHeaderHeight;
constexpr int32_t kContentHeight = 180;
constexpr int32_t kFooterTop = kHeaderHeight + kContentHeight;

constexpr int32_t kCenterY = 120;
constexpr int32_t kCircleRadius = 60;
constexpr int32_t kCircleInnerRadiusOffset = 10;
constexpr int32_t kNeedleRadiusOffset = 15;

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

void drawHeader(const DisplayMode mode) {
    Rect(0, 0, System::Width(), kHeaderHeight).draw(Palette::Darkblue);

    Font headerFont;
    headerFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Left)
        .setVerticalAlign(Font::VerticalAlign::Center);

    headerFont("M5Dial: " + getModeText(mode), Font::Pos(5, kHeaderHeight / 2), Palette::White);
}

void drawFooterHelp() {
    Font helpFont;
    helpFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);

    helpFont("Rotate: Change value", Font::Pos(System::Width() / 2, kFooterTop + 30), Palette::Gray);
    helpFont("Button A: Reset / Change mode", Font::Pos(System::Width() / 2, kFooterTop + 45), Palette::Gray);
}

void drawEncoderValuePanel(const long encoderValue, const Color& currentColor) {
    Rect(0, kContentTop, System::Width(), kContentHeight).draw(Palette::Black);

    Font largeFont;
    largeFont.setSize(3)
        .setHorizontalAlign(Font::HorizontalAlign::Center)
        .setVerticalAlign(Font::VerticalAlign::Center);

    largeFont(String(encoderValue), Font::Pos(System::Width() / 2, kCenterY), currentColor);

    const auto encoderDelta = Input::Encoder.getDelta();
    if (!encoderDelta.has_value() || encoderDelta.value() == 0) {
        return;
    }

    Font smallFont;
    smallFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);

    const String deltaText = "Δ" + String(encoderDelta.value());
    const auto deltaColor = (encoderDelta.value() > 0) ? Palette::Green : Palette::Red;
    smallFont(deltaText, Font::Pos(System::Width() / 2, kCenterY + 40), deltaColor);
}

void drawCircularDisplayPanel(const long encoderValue, const Color& currentColor) {
    Rect(0, kContentTop, System::Width(), kContentHeight).draw(Palette::Black);

    const int centerX = System::Width() / 2;
    const int centerY = kCenterY;

    Circle(centerX, centerY, kCircleRadius).drawFrame(Palette::White);
    Circle(centerX, centerY, kCircleRadius - kCircleInnerRadiusOffset).drawFrame(Palette::Darkgray);

    const float angleDegrees = Math::fmod(static_cast<float>(encoderValue), kAngleDegreesFull);
    const float angleRadians = angleDegrees * Math::Pi / 180.0f;

    const int needleX = centerX + (kCircleRadius - kNeedleRadiusOffset) * cos(angleRadians - Math::HalfPi);
    const int needleY = centerY + (kCircleRadius - kNeedleRadiusOffset) * sin(angleRadians - Math::HalfPi);

    Line(centerX, centerY, needleX, needleY).draw(currentColor);
    Circle(centerX, centerY, 5).draw(currentColor);

    for (int tickIndex = 0; tickIndex < 12; ++tickIndex) {
        const float tickAngle = tickIndex * Math::Pi / 6;
        const int tick1X = centerX + (kCircleRadius - 5) * cos(tickAngle - Math::HalfPi);
        const int tick1Y = centerY + (kCircleRadius - 5) * sin(tickAngle - Math::HalfPi);
        const int tick2X = centerX + kCircleRadius * cos(tickAngle - Math::HalfPi);
        const int tick2Y = centerY + kCircleRadius * sin(tickAngle - Math::HalfPi);
        Line(tick1X, tick1Y, tick2X, tick2Y).draw(Palette::White);
    }

    Font valueFont;
    valueFont.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center);
    valueFont(String(encoderValue), Font::Pos(centerX, kFooterTop - 10), Palette::Green);

    Font angleFont;
    angleFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);
    angleFont("Angle: " + String(static_cast<int>(angleDegrees)) + "°", Font::Pos(centerX, kContentTop + 15), Palette::White);
}

void drawRfidStatusPanel() {
    Rect(0, kContentTop, System::Width(), kContentHeight).draw(Palette::Black);

    Font titleFont;
    titleFont.setSize(2)
        .setHorizontalAlign(Font::HorizontalAlign::Center);
    titleFont("RFID Reader", Font::Pos(System::Width() / 2, kContentTop + 30), Palette::Cyan);

    Font statusFont;
    statusFont.setSize(1)
        .setHorizontalAlign(Font::HorizontalAlign::Center);

    // NOTE: 現状 `SafeDialRFID` は未実装箇所があり、カード検出は常に false になり得ます。
    // 将来的に M5Dial の MFRC522 API をここへ接続します。
    if (!Input::RFID.isCardPresent()) {
        statusFont("No Card", Font::Pos(System::Width() / 2, kContentTop + 70), Palette::Red);
        statusFont("Place card near device", Font::Pos(System::Width() / 2, kContentTop + 90), Palette::Gray);
        Circle(System::Width() / 2, kContentTop + 120, 20).drawFrame(Palette::Red);
        return;
    }

    statusFont("Card Detected!", Font::Pos(System::Width() / 2, kContentTop + 70), Palette::Green);

    const auto uid = Input::RFID.readCardUID();
    if (uid.has_value()) {
        statusFont("UID: " + uid.value(), Font::Pos(System::Width() / 2, kContentTop + 90), Palette::Yellow);
    }
    Circle(System::Width() / 2, kContentTop + 120, 20).draw(Palette::Green);
}

DisplayMode nextMode(DisplayMode mode) {
    const auto next = (static_cast<int32_t>(mode) + 1) % static_cast<int32_t>(DisplayMode::Count);
    return static_cast<DisplayMode>(next);
}

}  // namespace

void Main() {
    System::SetBackgroundColor(Palette::Black);

    AppState appState{};

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

        drawHeader(appState.displayMode);

        switch (appState.displayMode) {
        case DisplayMode::EncoderValue:
            drawEncoderValuePanel(encoderValue, appState.currentColor);
            break;
        case DisplayMode::CircularDisplay:
            drawCircularDisplayPanel(encoderValue, appState.currentColor);
            break;
        case DisplayMode::RfidStatus:
            drawRfidStatusPanel();
            break;
        default:
            Rect(0, kContentTop, System::Width(), kContentHeight).draw(Palette::Black);
            break;
        }

        drawFooterHelp();

        ClearPrint();
        drawPrint();
    }
}
