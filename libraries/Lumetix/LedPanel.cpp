#include "LedPanel.h"

#include "HardwareSerial.h"

#define CLAMP(val, min, max) (val < min ? min : val > max ? max : val)
static float Lerp(float A, float B, float t)
{
    return (1-t)*A + t*B;
}

LedPanel::LedPanel(TLC59116Manager& tlcmanager)
    : m_TransitionSpeed = 1.f
    , m_TlcManager(tlcmanager)
{
    if(tlcmanager.device_count() != EPanel::MAX_VAL)
    {
        Serial.println("TLCManager and panel mismatch. Aborting program");
        abort();
    }

    /* Zero out the buffers */
    for(int panel = 0; panel < EPanel::MAX_VAL; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            m_LedBuffer[panel][i] = 0;
            m_CurrLedBuffer[panel][i] = 0;
        }
    }
}

void LedPanel::Update(float deltaTime)
{
    UpdateLedBuffer(deltaTime);
}

void LedPanel::SetTransitionSpeed(float scalar)
{
    m_TransitionSpeed = scalar;
}

void LedPanel::SetBrightness(ELedColor color, byte brightness, EUpdateMode updateMode )
{
    for(int panel = 0; panel < EPanel::MAX_VAL; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            if(m_ColorMap[i] == color)
            {
                m_LedBuffer[panel][i] = brightness;
            }
            else if(updateMode == EUpdateMode::ZERO_UNSELECTED)
            {
                m_LedBuffer[panel][i] = 0;
            }
        }
    }
}

void LedPanel::SetBrightness(EPanel panel, ELedColor color, byte brightness, EUpdateMode updateMode)
{
    for(int i = 0; i < NUM_CHANNELS; i++)
    {
        if(m_ColorMap[i] == color)
        {
            m_LedBuffer[panel][i] = brightness;
        }
        else if(updateMode == EUpdateMode::ZERO_UNSELECTED)
        {
            m_LedBuffer[panel][i] = brightness;
        }
    }
}

void LedPanel::SetBrightness(EPanel panel, byte brightness, EUpdateMode updateMode)
{
    for(int i = 0; i < NUM_CHANNELS; i++)
    {
        byte currBrightness = m_LedBuffer[panel][i];
        byte newBrightness = BlendBrightness(currBrightness, brightness, updateMode);

        m_LedBuffer[panel][i] = newBrightness;
    }
}

void LedPanel::SetBrightness(byte brightness, EUpdateMode updateMode)
{
    for(int panel = 0; panel < EPanel::MAX_VAL; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            byte currBrightness = m_LedBuffer[panel][i];
            byte newBrightness = BlendBrightness(currBrightness, brightness, updateMode);

            m_LedBuffer[panel][i] = newBrightness;
        }
    }
}

byte LedPanel::BlendBrightness(byte prevVal, byte newVal, EUpdateMode updateMode) const
{
    int _prev = prevVal;
    int _new = newVal;

    switch(updateMode)
    {
        case EUpdateMode::IGNORE_UNSELECTED:
        case EUpdateMode::ZERO_UNSELECTED:
        return newVal;

        case EUpdateMode::ADD:              return CLAMP(_prev + _new, 0, 255);
        case EUpdateMode::SUBTRACT:         return CLAMP(_prev - _new, 0 , 255);
        case EUpdateMode::MULTIPLY:         return CLAMP(_prev * _new, 0, 255);
        case EUpdateMode::IGNORE_NONZERO:   return _prev == 0 ? _new : _prev;
    }
}

void LedPanel::UpdateLedBuffer(float deltaTime)
{
    for(int panel = 0; panel < EPanel::MAX_VAL; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            /* Simple linear interpolation of values */
            byte newIntensity = Lerp(m_PrevLedBuffer[panel][i], m_LedBuffer[panel][i], deltaTime * m_TransitionSpeed);
            m_CurrLedBuffer[panel][i] = newIntensity;

            m_TlcManager[panel].pwm(i, newIntensity);
        }
    }
}