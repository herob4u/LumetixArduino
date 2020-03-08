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

    }
    else
    {   
        auto it = panel.VerticalIterator();

        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            if(it)
            {
                const float alpha = (float)i / (NUM_CHANNELS - 1);

                byte val = Lerp((float)m_IntensityA, (float)m_IntensityB, alpha);

                // Additively apply brightness without affecting previous set values.
                it.SetBrightness(val, EUpdateMode::IGNORE_NONZERO);
            }
            ++it;
        }
    }
}

void IntensityGradientEffect::OnUpdate(float deltaTime)
{

}

void IntensityGradientEffect::OnRemoved()
{

}