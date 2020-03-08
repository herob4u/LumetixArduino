#include "LedPanel.h"

#include "HardwareSerial.h"

#define CLAMP(val, min, max) (val < min ? min : val > max ? max : val)

ELedColor LedPanel::m_ColorMap[] =
{
    WHITE, WHITE, YELLOW, RED, GREEN, BLUE, YELLOW, WHITE,
    WHITE, YELLOW, BLUE, GREEN, RED, YELLOW, WHITE, WHITE
};

LedPanel::LedPanel(TLC59116Manager& tlcmanager)
    : m_TransitionSpeed(1.f)
    , m_TlcManager(tlcmanager)
{
    if(tlcmanager.device_count() != EPanel::MAX_VAL)
    {
        Serial.println("TLCManager and panel mismatch. Aborting program");
        Serial.flush();
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
    Serial.println("Update");
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

void LedPanel::FromChannelMap(int* channelMaps, byte brightness, EUpdateMode updateMode)
{
#if 0
    for(int panel = 0; panel < 4; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            int enabled = channelMaps[panel][i];
            Serial.print("[");Serial.print(panel);Serial.print("]");
            Serial.print("["); Serial.print(i); Serial.print("]");
            Serial.print("=");
            Serial.println(i);
            Serial.flush();
            byte currBrightness = m_LedBuffer[panel][i];
            byte newBrightness = BlendBrightness(currBrightness, brightness * enabled, updateMode);

            m_LedBuffer[panel][i] = newBrightness;
            m_CurrLedBuffer[panel][i] = newBrightness;
        }
    }
    Serial.println("FromChannelMap update");
    UpdateLedBuffer(1);
#else
    for(int panel = 0; panel < 4; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            int enabled = 1;//channelMaps[i + 16*panel];
            Serial.print("["); Serial.print(panel); Serial.print("]");
            Serial.print("["); Serial.print(i); Serial.print("]");
            Serial.print("=");
            Serial.println(enabled);
            Serial.flush();

            byte currBrightness = m_LedBuffer[panel][i];
            byte newBrightness = BlendBrightness(currBrightness, brightness * enabled, updateMode);

            m_LedBuffer[panel][i] = newBrightness;
            m_CurrLedBuffer[panel][i] = newBrightness;
        }
    }

    UpdateLedBuffer(1);
#endif
}

void LedPanel::FromChannelMap(unsigned short top, unsigned short right, unsigned short bottom, unsigned short left, byte brightness, EUpdateMode updateMode)
{
    for(int channel = 0; channel < NUM_CHANNELS; channel++)
    {
        // Effectively toggle LEDs ON or OFF using a masking operation
        byte topBrightness      = (((1 << channel) & top) == 0)       ? 0 : brightness;
        byte rightBrightness    = (((1 << channel) & right) == 0)     ? 0 : brightness;
        byte bottomBrightness   = (((1 << channel) & bottom) == 0)    ? 0 : brightness;
        byte leftBrightness     = (((1 << channel) & left) == 0)      ? 0 : brightness;

        // Update both the current and target buffer for immediate change
        m_LedBuffer[0][channel] = topBrightness;
        m_LedBuffer[1][channel] = rightBrightness;
        m_LedBuffer[2][channel] = bottomBrightness;
        m_LedBuffer[3][channel] = leftBrightness;

        m_CurrLedBuffer[0][channel] = topBrightness;
        m_CurrLedBuffer[1][channel] = rightBrightness;
        m_CurrLedBuffer[2][channel] = bottomBrightness;
        m_CurrLedBuffer[3][channel] = leftBrightness;
    }

    UpdateLedBuffer(1);
}

void LedPanel::SetChannelBrightness(EPanel panel, byte brightness, int* channels, size_t count)
{
    if(!channels)
        return;

    const int panelId = static_cast<int>(panel);

    for(int i = 0; i < count; i++)
    {
        const int channel = channels[i];
        m_LedBuffer[panelId][channel] = brightness;
        m_CurrLedBuffer[panelId][channel] = brightness;
    }

    UpdateLedBuffer(1);
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
            byte newIntensity = Lerp(m_CurrLedBuffer[panel][i], m_LedBuffer[panel][i], deltaTime * m_TransitionSpeed);
            m_CurrLedBuffer[panel][i] = newIntensity;

            m_TlcManager[panel].pwm(i, newIntensity);
        }
    }
}

void LedPanel::TurnOff(bool bImmediate)
{
    for(int panel = 0; panel < EPanel::MAX_VAL; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            m_LedBuffer[panel][i] = 0;
            
            if(bImmediate)
            {
                m_CurrLedBuffer[panel][i] = 0;
                m_TlcManager[panel].pwm(i, 0);
            }
        }
    }
}

void LedPanel::TurnOn(bool bImmediate)
{
    for(int panel = 0; panel < EPanel::MAX_VAL; panel++)
    {
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            m_LedBuffer[panel][i] = 255;

            if(bImmediate)
            {
                m_CurrLedBuffer[panel][i] = 255;
                m_TlcManager[panel].pwm(i, 255);
            }
        }
    }
}