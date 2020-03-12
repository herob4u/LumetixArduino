#include "IntensityGradientEffect.h"

#include "Context.h"

IntensityGradientEffect::IntensityGradientEffect(byte intensityA, byte intensityB, EGradientDirection dir)
    : EffectBase(EffectType::PHOTO_EFFECT)
    , m_Dir(dir)
    , m_IntensityA(intensityA)
    , m_IntensityB(intensityB)
{

}

void IntensityGradientEffect::OnApplied()
{
    LedPanel& panel = gContext->Panel;
    panel.TurnOff(true);

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

    int i = 0;
    while(it)
    {
        const float alpha = (float)i / (NUM_CHANNELS-1);
        byte brightness = Lerp((float)m_IntensityA, (float)m_IntensityB, alpha);

        // Ignore Nonzero incorporates persistence - previously set LEDs remain set.
        it.SetBrightness(brightness, EUpdateMode::IGNORE_NONZERO);
        ++it;
        ++i;
    }
}

void IntensityGradientEffect::ApplyHorizontalGradient()
{
    LedPanel& panel = gContext->Panel;

    /* Get a horizontal iterator over the panel */
    auto it = panel.HorizontalIterator();

    int i = 0;
    while(it)
    {
        const float alpha = (float)i / (NUM_CHANNELS - 1);
        byte brightness = Lerp((float)m_IntensityA, (float)m_IntensityB, alpha);

        // Ignore Nonzero incorporates persistence - previously set LEDs remain set.
        it.SetBrightness(brightness, EUpdateMode::IGNORE_NONZERO);
        ++it;
        ++i;
    }
}

void IntensityGradientEffect::OnUpdate(float deltaTime)
{

}

void IntensityGradientEffect::OnRemoved()
{

}

void IntensityGradientEffect::OnSetArgs(EffectArgs& args)
{
    /* Expecting arguments of type (byte, byte, byte) */
    if(args.NumArgs == 3)
    {
        ByteBuffer buffer = args.ArgBuffer;
        if(buffer.GetNumBytes() == 3) // 3 bytes
        {
            byte intensityA = buffer.GetByte();
            byte intensityB = buffer.GetByte();
            byte dir = buffer.GetByte();
 
            m_IntensityA = intensityA;
            m_IntensityB = intensityB;
            m_Dir = (dir == 0 ? EGradientDirection::HORIZONTAL : EGradientDirection::VERTICAL);

            // Refresh
            OnApplied();
        }
    }
}