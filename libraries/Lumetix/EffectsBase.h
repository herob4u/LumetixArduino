#ifndef EFFECTS_BASE_H
#define EFFECTS_BASE_H

#include "Context.h"

enum class EffectType
{
    PHOTO_EFFECT,
    VIDEO_EFFECT
};

/*  Represents an Effect that modulates the behavior of the Led Panel
*   This is the class/OOP representation of an effect. Depending on 
*   preference and number of supported effects, this may be more desirable.
*
*   See FuntionalEffects.h for C-style effect functions.
*
*   ASIDE: Should effects be persistent (statically created and swapped in realtime),
*   or should they be malloced on demand? The former is more intuitive since effects
*   are RESOURCES! But effects might store variables like curves and response - having
*   over 10 effects will huge memory footprint! Prefer mallocing on demand for now.
*/
class EffectBase
{
public:
    EffectBase(EffectType type)
        : m_Type(type)
    {
    }

    virtual ~EffectBase(){}

    /* User Defined Overridable methods 
    *
    *   OnApplied:  Occurs when an effect is first activated to handle initial applications of the effect.
    *               This is called only once and is typically used by static photo effects that perform
    *               their effects upon immediate application.
    *
    *   OnUpdate:   Typically used by dynamic effects that need to continuously correct over time. Called
    *               every application update to give the Effect a chance to perform dynamic processing.
    *               Usually the application abstains from calling this on PHOTO_EFFECTs.
    *
    *   OnRemoved:  Occurs when an Effect is swapped out or removed; allows the Effect to handle its removal.
    *
    */
    virtual void OnApplied() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnRemoved() = 0;

    inline EffectType GetType() const { return m_Type; }
private:
    EffectType m_Type;
};
#endif // !EFFECTS_BASE_H
