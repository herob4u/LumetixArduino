#ifndef LED_PANEL_H
#define LED_PANEL_H

#include <stdint.h>
#include <Arduino.h>

#include "Common.h"

#include "../TLC59116/TLC59116.h"

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
ENUM_BITFLAG(ELedColor);

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

    #include "PanelIterators.inl"
    VerticalPanelIterator VerticalIterator(size_t maskSize = 1) const       { return VerticalPanelIterator(*const_cast<LedPanel*>(this), maskSize); }
    HorizontalPanelIterator HorizontalIterator(size_t maskSize = 1) const   { return HorizontalPanelIterator(*const_cast<LedPanel*>(this), maskSize); }
    RingPanelIterator RingIterator(size_t maskSize = 1) const               { return RingPanelIterator(*const_cast<LedPanel*>(this), maskSize); }
    
    void Update(float deltaTime);
    void SetTransitionSpeed(float scalar);

    /* Set the brightness of LEDs of a specified color across all panels */
    void SetBrightness(ELedColor color, byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED);

    /* Set the brightness of LEDs of a specified color for a given panel */
    void SetBrightness(EPanel panel, ELedColor color, byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED);

    /* Set the brightness for all LEDs on a given panel*/
    void SetBrightness(EPanel panel, byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED);

    /* Set the brightness of all LEDs on the board */
    void SetBrightness(byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED);

    /*  Set brightness for LEDs in a given channel map. A channel map is a direct indicator of selected LED channels
    *   on a per panel basis. The bit value of each channel corresponds to the toggled state of the LED - ON(1), OFF(0)
    */
    void FromChannelMap(int* channelMaps, byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED);

    /*  Set brightness for LEDs from a given channel map, where a channel maps are bitflags representing
    *   the toggled state of LEDs in the top, right, bottom, and left panels.
    */
    void FromChannelMap(unsigned short top, unsigned short right, unsigned short bottom, unsigned short left
                        , byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED);

    /*  Manual mode for selectively setting brightness of individual channels 
    *   Intakes an array of channels corresponding to the channels to be set,
    *   and the channel count in the array.
    */
    void SetChannelBrightness(EPanel panel, byte brightness, int* channels, size_t count);

    void TurnOff(bool bImmediate = false);
    void TurnOn(bool bImmediate = false);
private:
    byte BlendBrightness(byte prevVal, byte newVal, EUpdateMode updateMode) const;
    void UpdateLedBuffer(float deltaTime);
private:
    static ELedColor m_ColorMap[NUM_CHANNELS];

    /*  The LedBuffer represents the target values of intensities for all LEDs
    *   Conversely, the Current buffer represents the current values being interpolated.
    */
    byte m_LedBuffer[EPanel::MAX_VAL][NUM_CHANNELS];
    byte m_CurrLedBuffer[EPanel::MAX_VAL][NUM_CHANNELS];

    float m_TransitionSpeed;

    TLC59116Manager& m_TlcManager;
};
#endif