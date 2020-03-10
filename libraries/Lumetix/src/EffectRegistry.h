#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

class EffectBase;

#define NUM_EFFECTS 12
class EffectRegistry
{
public:
    EffectRegistry();
    void Init();

    bool ActivateEffect(unsigned int effectId);
    bool DeactivateEffect();

    const EffectBase* GetActiveEffect() const;
    inline constexpr int GetNumEffects() const { return NUM_EFFECTS; }
private:
    int m_ActiveEffect;
    EffectBase* m_Effects[NUM_EFFECTS];
};
#endif // !EFFECT_REGISTRY_H
