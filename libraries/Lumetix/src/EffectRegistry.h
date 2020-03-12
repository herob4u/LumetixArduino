#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

#include "EffectBase.h"

#define NUM_EFFECTS 12
class EffectRegistry
{
public:
    EffectRegistry();
    void Init();
    void Update(float deltaTime);

    bool ActivateEffect(unsigned int effectId);
    bool DeactivateEffect();
    void NotifyArgsChanged(EffectArgs& args);

    inline int GetActiveEffectId() const { return m_ActiveEffect; }
    const EffectBase* GetActiveEffect() const;
    inline constexpr int GetNumEffects() const { return NUM_EFFECTS; }
private:
    int m_ActiveEffect;
    EffectBase* m_Effects[NUM_EFFECTS];
};
#endif // !EFFECT_REGISTRY_H
