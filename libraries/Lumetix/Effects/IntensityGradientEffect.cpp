#include "IntensityGradientEffect.h"

#include "../PanelIterator.h"

IntensityGradientEffect::IntensityGradientEffect(byte intensityA, byte intensityB, EGradientDirection dir)
    : EffectBase(EffectType::PHOTO_EFFECT)
    , m_Dir(dir)
    , m_IntensityA(intentistyA)
    , m_IntensityB(intensityB)
{

}

void IntensityGradientEffect::OnApplied()
{
    LedPanel& panel = gContext->Panel;

    // Perform the gradient by spatially iterating over the panel and setting values
    if(m_Dir == EGradientDirection::HORIZONTAL)
    {
        ApplyHorizontalGradient();
    }
    else
    {   
        ApplyVerticalGradient();
    }
}

void IntensityGradientEffect::ApplyVerticalGradient()
{
    LedPanel& panel = gContext->Panel;

    /* Get a vertical iterator over the panel */
    auto it = panel.VerticalIterator();

    while(it)
    {
        const float alpha = (float)i / (NUM_CHANNELS-1);
        byte brightness = Lerp((float)m_IntensityA, (float)m_IntensityB, alpha);

        // Ignore Nonzero incorporates persistence - previously set LEDs remain set.
        it.SetBrightness(brightness, EUpdateMode::IGNORE_NONZERO);
        ++it;
    }
}

void IntensityGradientEffect::ApplyHorizontalGradient()
{
    LedPanel& panel = gContext->Panel;

    /* Get a horizontal iterator over the panel */
    auto it = panel.HorizontalIterator();

    while(it)
    {
        const float alpha = (float)i / (NUM_CHANNELS - 1);
        byte brightness = Lerp((float)m_IntensityA, (float)m_IntensityB, alpha);

        // Ignore Nonzero incorporates persistence - previously set LEDs remain set.
        it.SetBrightness(brightness, EUpdateMode::IGNORE_NONZERO);
        ++it;
    }
}

void IntensityGradientEffect::OnUpdate(float deltaTime)
{

}

void IntensityGradientEffect::OnRemoved()
{

}