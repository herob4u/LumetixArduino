#ifndef INTENSITY_GRADIENT_EFFECT_H
#define INTENSITY_GRADIENT_EFFECT_H

#include "../EffectsBase.h"

class IntensityGradientEffect : public EffectBase
{
public:
    IntensityGradientEffect(byte intensityA, byte intensityB, EGradientDirection dir = EGradientDirection::HORIZONTAL);

    virtual void OnApplied() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRemoved() override;

private:
    EGradientDirection m_Dir;
    byte m_IntensityA;
    byte m_IntensityB;
};
#endif