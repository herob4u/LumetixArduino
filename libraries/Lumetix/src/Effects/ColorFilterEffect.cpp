#include "ColorFilterEffect.h"

ColorFilterEffect::ColorFilterEffect()
    : EffectBase(EffectType::PHOTO_EFFECT)
    , m_ActiveFilter(FilterType::RED_BLUE)
    , m_StrobeFrequency(0.f)
    , m_StrobeOffset(0.f)
    , m_StrobeAmplitude(1.f)
    , m_Intensity(150.f)
    , m_WasInterpEnabled(true)
{
}

void ColorFilterEffect::OnApplied()
{
    LOGN("Color Filter Applied");

    // We probably want this mode to be immediate because the strobing is a result of
    // sinusoidal modulations. Safe the previous state of the panel to restore it later.
    LedPanel& panel = gContext->Panel;
    m_WasInterpEnabled = panel.bInterpolates;

    ApplyFilter(m_Intensity);
}

void ColorFilterEffect::OnUpdate(float deltaTime)
{
    if(m_StrobeFrequency == 0 || m_StrobeAmplitude == 0)
        return;

    m_Time += deltaTime;

    if(m_Time > m_StrobeFrequency)
        m_Time = 0.f;

    float strobeBrightness = m_Intensity * cos(m_StrobeFrequency * m_Time);
    strobeBrightness = Clamp(m_StrobeAmplitude * strobeBrightness + m_StrobeOffset, 0.01f, 254.9f);

    ApplyFilter((byte)strobeBrightness);
    LOG("Filter brightness: "); LOGN((byte)strobeBrightness);
}

void ColorFilterEffect::OnRemoved()
{
    // Restore previous panel interpolation state
    LedPanel& panel = gContext->Panel;
    panel.bInterpolates = m_WasInterpEnabled;
}

void ColorFilterEffect::OnSetArgs(EffectArgs& args)
{
    if(args.NumArgs == 1)
    {
        byte filterType = args.ArgBuffer.GetByte();
        
        m_ActiveFilter = static_cast<FilterType>(filterType);
        OnApplied();
    }
}

void ColorFilterEffect::ApplyFilter(byte brightness)
{
    LedPanel& panel = gContext->Panel;

    switch(m_ActiveFilter)
    {
        case FilterType::BLUE:
        {
            panel.SetBrightness(ELedColor::BLUE, brightness, EUpdateMode::ZERO_UNSELECTED);
        }
        break;

        case FilterType::YELLOW:
        {
            panel.SetBrightness(ELedColor::YELLOW, brightness, EUpdateMode::ZERO_UNSELECTED);
        }
        break;

        case FilterType::RED:
        {
            panel.SetBrightness(ELedColor::RED, brightness, EUpdateMode::ZERO_UNSELECTED);
        }
        break;

        case FilterType::GREEN:
        {
            panel.SetBrightness(ELedColor::GREEN, brightness, EUpdateMode::ZERO_UNSELECTED);
        }
        break;

        case FilterType::RED_YELLOW:
        {
            panel.SetBrightness(ELedColor::RED, brightness, EUpdateMode::ZERO_UNSELECTED);
            panel.SetBrightness(ELedColor::YELLOW, brightness, EUpdateMode::IGNORE_UNSELECTED);
        }
        break;

        case FilterType::BLUE_GREEN:
        {
            panel.SetBrightness(ELedColor::BLUE, brightness, EUpdateMode::ZERO_UNSELECTED);
            panel.SetBrightness(ELedColor::GREEN, brightness, EUpdateMode::IGNORE_UNSELECTED);
        }
        break;

        case FilterType::RED_BLUE:
        {
            panel.SetBrightness(ELedColor::RED, brightness, EUpdateMode::ZERO_UNSELECTED);
            panel.SetBrightness(ELedColor::BLUE, brightness, EUpdateMode::IGNORE_UNSELECTED);
        }
        break;

        /* *******************Thematic Effects********************** */
        case FilterType::TYPE10:
        {
            int left[2] = { 4, 11 };
            int right[2] = { 3, 12 };
            int top[2] = { 3, 11 };
            int bottom[2] = { 4, 12 };

            panel.SetChannelBrightness(EPanel::TOP, brightness, top, 2);
            panel.SetChannelBrightness(EPanel::LEFT, brightness, left, 2);
            panel.SetChannelBrightness(EPanel::RIGHT, brightness, right, 2);
            panel.SetChannelBrightness(EPanel::BOTTOM, brightness, bottom, 2);
        }
        break;

        case FilterType::TYPE11:
        {
            int left[2] = { 3, 12 };
            int right[2] = { 5, 10 };
            int top[2] = { 5, 12 };
            int bottom[2] = { 3, 10 };

            panel.SetChannelBrightness(EPanel::TOP, brightness, top, 2);
            panel.SetChannelBrightness(EPanel::LEFT, brightness, left, 2);
            panel.SetChannelBrightness(EPanel::RIGHT, brightness, right, 2);
            panel.SetChannelBrightness(EPanel::BOTTOM, brightness, bottom, 2);
        }
        break;

        case FilterType::TYPE12:
        {
            int left[2] = { 4, 11 };
            int right[2] = { 5, 10 };
            int top[2] = { 5, 11 };
            int bottom[2] = { 4, 10 };

            panel.SetChannelBrightness(EPanel::TOP, brightness, top, 2);
            panel.SetChannelBrightness(EPanel::LEFT, brightness, left, 2);
            panel.SetChannelBrightness(EPanel::RIGHT, brightness, right, 2);
            panel.SetChannelBrightness(EPanel::BOTTOM, brightness, bottom, 2);
        }
        break;

        case FilterType::TYPE13:
        {
            int left[2] = { 6, 13 };
            int right[2] = { 3, 12 };
            int top[2] = { 3, 13 };
            int bottom[2] = { 2, 12 };

            panel.SetChannelBrightness(EPanel::TOP, brightness, top, 2);
            panel.SetChannelBrightness(EPanel::LEFT, brightness, left, 2);
            panel.SetChannelBrightness(EPanel::RIGHT, brightness, right, 2);
            panel.SetChannelBrightness(EPanel::BOTTOM, brightness, bottom, 2);
        }
        break;

        case FilterType::TYPE14:
        {
            int left[2] = { 6 };
            int right[1] = { 9 };
            int top[4] = { 2, 3, 12, 13 };
            int bottom[2] = { 5, 10 };

            panel.SetChannelBrightness(EPanel::TOP, brightness, top, 4);
            panel.SetChannelBrightness(EPanel::LEFT, brightness, left, 1);
            panel.SetChannelBrightness(EPanel::RIGHT, brightness, right, 1);
            panel.SetChannelBrightness(EPanel::BOTTOM, brightness, bottom, 2);
        }
        break;
    }
}