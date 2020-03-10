#include "EffectRegistry.h"

#include "EffectsFwd.h"

#define INDEX_NONE -1
EffectRegistry::EffectRegistry()
    : m_ActiveEffect(INDEX_NONE)
{
}

void EffectRegistry::Init()
{
    m_Effects[0] = new ColorCorrectEffect();
    m_Effects[1] = new IntensityGradientEffect(20, 255, EGradientDirection::VERTICAL);
    // ...
    // ...
}

bool EffectRegistry::ActivateEffect(unsigned int effectId)
{
    if(effectId < 0 || effectId >= GetNumEffects())
        return false;

    EffectBase* effect = m_Effects[effectId];
    
    if(!effect)
        return false;

    // Remove any previous effect if exists
    DeactivateEffect();

    // Assign the new active effect and apply it
    m_ActiveEffect = effectId;

    effect->OnApplied();
    return true;
}

bool EffectRegistry::DeactivateEffect()
{
    if(m_ActiveEffect == INDEX_NONE)
        return false;

    EffectBase* effect = m_Effects[m_ActiveEffect];
    if(!effect)
        return false;

    effect->OnRemoved();
    m_ActiveEffect = -1;
    return true;
}

const EffectBase* EffectRegistry::GetActiveEffect() const
{
    return (m_ActiveEffect == INDEX_NONE ? nullptr : m_Effects[m_ActiveEffect]);
}