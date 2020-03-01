#ifndef LED_PANEL_H
#define LED_PANEL_H

#include <stdint.h>
#include <Arduino.h>
#include "../TLC59116/TLC59116.h"

#define NUM_CHANNELS 16
enum EPanel : uint8_t
{
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3,
    MAX_VAL = 4
};

enum ELedColor : uint8_t
{
    WHITE   = (1 << 1),
    YELLOW  = (1 << 2),
    RED     = (1 << 3),
    GREEN   = (1 << 4),
    BLUE    = (1 << 5)
};

enum class EUpdateMode : uint8_t
{
    ZERO_UNSELECTED,
    IGNORE_UNSELECTED,
    ADD,
    SUBTRACT,
    MULTIPLY,
    IGNORE_NONZERO
};

class LedPanel
{
public:
    LedPanel(TLC59116Manager& tlcmanager);

    void Update(float deltaTime);
    void SetTransitionSpeed(float scalar);

    void SetBrightness(ELedColor color, byte brightness, EUpdateMode updateMode = EUpdateMode::ZERO_UNSELECTED);
    void SetBrightness(EPanel panel, ELedColor color, byte brightness, EUpdateMode updateMode = EUpdateMode::ZERO_UNSELECTED);
    void SetBrightness(EPanel panel, byte brightness, EUpdateMode updateMode = EUpdateMode::ZERO_UNSELECTED);
    void SetBrightness(byte brightness, EUpdateMode updateMode = EUpdateMode::ZERO_UNSELECTED);

private:
    byte BlendBrightness(byte prevVal, byte newVal, EUpdateMode updateMode) const;
    void UpdateLedBuffer(float deltaTime);
private:
    static ELedColor m_ColorMap[16] = 
    {
        WHITE, WHITE, YELLOW, RED, GREEN, BLUE, YELLOW, WHITE,
        WHITE, YELLOW, BLUE, GREEN, RED, YELLOW, WHITE, WHITE
    };

    /*  The LedBuffer represents the target values of intensities for all LEDs
    *   Conversely, the Current buffer represents the current values being interpolated.
    */
    byte m_LedBuffer[EPanel::MAX_VAL][16];
    byte m_CurrLedBuffer[EPanel::MAX_VAL][16];

    float m_TransitionSpeed;

    TLC59116Manager& m_TlcManager;
};
#endif