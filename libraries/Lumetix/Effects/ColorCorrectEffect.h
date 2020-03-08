#ifndef COLOR_CORRECT_EFFECT_H
#define COLOR_CORRECT_EFFECT_H

#include "../EffectsBase.h"

class ColorCorrectEffect : public EffectBase
{
public:
    ColorCorrectEffect();

    virtual void OnApplied() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRemoved() override;
};
#endif // !COLOR_CORRECT_EFFECT_H
