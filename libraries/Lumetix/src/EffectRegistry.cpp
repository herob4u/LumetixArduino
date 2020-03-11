#include "EffectRegistry.h"

#include "Effects/EffectsFwd.h"

#define INDEX_NONE -1
EffectRegistry::EffectRegistry()
    : m_ActiveEffect(INDEX_NONE)
{
}

void EffectRegistry::Init()
{
    m_Effects[0] = new ColorCorrectEffect();
    m_Effects[1] = new IntensityGradientEffect(20, 255, EGradientDirection::VERTICAL);
    m_Effects[2] = new PartyEffect();
    // ...
    // ...
}

void EffectRegistry::Update(float deltaTime)
{
    if(m_ActiveEffect == INDEX_NONE || m_ActiveEffect >= GetNumEffects())
        return;

    EffectBase* effect = m_Effects[m_ActiveEffect];
    if(effect)
    {
        effect->OnUpdate(deltaTime);
    }
}

bool EffectRegistry::ActivateEffect(unsigned int effectId)
{
    // Invalid effect
    if(effectId < 0 || effectId >= GetNumEffects())
        return false;
    
    // Already active!
    if(effectId == m_ActiveEffect)
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

void EffectRegistry::NotifyArgsChanged(const EffectArgs& args)
{
    // No active effect to notify
    if(m_ActiveEffect == INDEX_NONE || m_ActiveEffect >= GetNumEffects())
        return;

    // No argument content
    if(args.NumArgs == 0)
        return;

    EffectBase* effect = m_Effects[m_ActiveEffect];

    if(effect)
    {
        effect->OnSetArgs(args);
    }
}

const EffectBase* EffectRegistry::GetActiveEffect() const
{
    return (m_ActiveEffect == INDEX_NONE ? nullptr : m_Effects[m_ActiveEffect]);
}