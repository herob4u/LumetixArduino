#ifndef COLOR_CORRECT_EFFECT_H
#define COLOR_CORRECT_EFFECT_H

#include "../EffectBase.h"
#include "../../../VariableResponse/VariableResponse.h"

class ColorCorrectEffect : public EffectBase
{
public:
    ColorCorrectEffect();

    virtual void OnApplied() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRemoved() override;
    virtual void OnSetArgs(const EffectArgs& args) override;
protected:
    void Calibrate();
protected:
    float RBf;

    float RIntensityMultiplier;
    float BIntensityMultiplier;
private:
    static const float s_DefaultMinResponse;
    static const float s_DefaultMaxResponse;

    VariableResponse WarmResponse; // Response of warm colors to a cool input
    VariableResponse CoolResponse; // Response of cool colors to a warm input
    
};
#endif // !COLOR_CORRECT_EFFECT_H
