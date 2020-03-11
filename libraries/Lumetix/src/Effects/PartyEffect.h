#ifndef PARTY_EFFECT_H
#define PARTY_EFFECT_H
#include "../EffectBase.h"

class PartyEffect : public EffectBase
{
public:
    PartyEffect();

    enum AnimationMode
    {
        COLOR_SEQ = 0,
        VERTICAL = 1,
        HORIZONTAL = 2,
        RING = 3,
        MAX_VAL = 4
    };

    virtual void OnApplied() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRemoved() override;
    virtual void OnSetArgs(const EffectArgs& args) override;

    void SetBpmDelay(short bpmDelay_ms);
    short GetBpmDelay() const { return m_BpmDelay; }

    void SetAnimationMode(AnimationMode mode);
protected:
    /*  Animation Functions 
    *
    *   Provides various techniques to light up the panel in a rythmic fashion. All animations
    *   are blinking with an ON/OFF interval dictated by m_BPMDelay.
    *
    *   DoSequenceAnimUpdate:   Lights up colors in a sequential manner 
    *   DoVerticalAnimUpdate:   Performs a vertical panel iteration, blinking in between every step
    *   DoHorizontalAnimUpdate: Performs a horizontal panel iteration, blinking in between every step
    *   DoRingAnimUpdate:       Performs ring panel iteration, blinking in between every step.
    */
    void DoSequenceAnimUpdate();
    void DoVerticalAnimUpdate();
    void DoHorizontalAnimUpdate();
    void DoRingAnimUpdate();
private:
    unsigned short m_BpmDelay;
    unsigned short m_ElapsedTime;
    unsigned short m_SequenceIt;

    bool m_IsActive;

    typedef void(PartyEffect::*AnimFunction)();
    AnimFunction m_AnimationFunctions[AnimationMode::MAX_VAL];
    AnimationMode m_AnimMode;

    #define NUM_COLORS 5
    /* Sequentially blink with a specified color sequence. In this configuration,
    *   the led panel will turn all corresponding light colors in the provided sequence.
    *   OFF->COLOR[0]->OFF->COLOR[1]->OFF->COLOR[2]....etc
    */
    ELedColor m_ColorSequence[5];

    /* Describes the intensities of each color in the sequence */
    byte m_SequenceBrightness[5];

    /* Helper Iterators */
    LedPanel::HorizontalPanelIterator m_horizontalIt;
    LedPanel::VerticalPanelIterator m_verticalIt;
    LedPanel::RingPanelIterator m_ringIt;
};
#endif // !PARTY_EFFECT_H
