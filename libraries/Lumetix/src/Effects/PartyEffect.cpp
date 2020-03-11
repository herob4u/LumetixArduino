#include "PartyEffect.h"

/* Invokation helper for animation functions */
#define INVOKE(func) ((this->*func)())

PartyEffect::PartyEffect()
    : EffectBase(EffectType::VIDEO_EFFECT)
    , m_BpmDelay(100)
    , m_ElapsedTime(0)
    , m_SequenceIt(0)
    , m_IsActive(false)
    , m_AnimMode(AnimationMode::COLOR_SEQ)
    , m_verticalIt(gContext->Panel.VerticalIterator())
    , m_horizontalIt(gContext->Panel.HorizontalIterator())
    , m_ringIt(gContext->Panel.RingIterator())
{
    m_ColorSequence[0] = ELedColor::BLUE;
    m_ColorSequence[1] = ELedColor::GREEN;
    m_ColorSequence[2] = ELedColor::RED;
    m_ColorSequence[3] = ELedColor::YELLOW;
    m_ColorSequence[4] = ELedColor::YELLOW;

    m_SequenceBrightness[0] = 128;
    m_SequenceBrightness[1] = 90;
    m_SequenceBrightness[2] = 64;
    m_SequenceBrightness[3] = 32;
    m_SequenceBrightness[4] = 200;

    m_AnimationFunctions[AnimationMode::COLOR_SEQ]  = &PartyEffect::DoSequenceAnimUpdate;
    m_AnimationFunctions[AnimationMode::HORIZONTAL] = &PartyEffect::DoHorizontalAnimUpdate;
    m_AnimationFunctions[AnimationMode::VERTICAL]   = &PartyEffect::DoVerticalAnimUpdate;
    m_AnimationFunctions[AnimationMode::RING]       = &PartyEffect::DoRingAnimUpdate;
}

void PartyEffect::OnApplied()
{

}

void PartyEffect::OnUpdate(float deltaTime)
{
    m_ElapsedTime += deltaTime;

    if(m_ElapsedTime >= m_BpmDelay)
    {
        // Toggle
        LedPanel& panel = gContext->Panel;

        m_IsActive ? panel.TurnOff(true) : INVOKE(m_AnimationFunctions[m_AnimMode]);
        m_IsActive = !m_IsActive;
    }
}

void PartyEffect::OnRemoved()
{

}

void PartyEffect::OnSetArgs(const EffectArgs& args)
{
    // Expects update mode or bpm update
}

void PartyEffect::SetAnimationMode(AnimationMode mode)
{
    m_AnimMode = mode;
}

void PartyEffect::SetBpmDelay(short bpmDelay_ms)
{
    m_BpmDelay = bpmDelay_ms;
}

/* ANIMATION FUNCTIONS */
void PartyEffect::DoSequenceAnimUpdate()
{
    ELedColor color = m_ColorSequence[m_SequenceIt];
    byte brightness = m_SequenceBrightness[m_SequenceIt++];

    LedPanel& panel = gContext->Panel;

    /* Turn on the LEDs in accordance to the sequence. Want exclusive lighting, so IGNORE_UNSELECTED */
    panel.SetBrightness(color, brightness, EUpdateMode::IGNORE_UNSELECTED);
}

void PartyEffect::DoVerticalAnimUpdate()
{
    if(!m_verticalIt)
    {
        LedPanel& panel = gContext->Panel;
        m_verticalIt = panel.VerticalIterator();
    }

    m_verticalIt.SetBrightness(255, EUpdateMode::IGNORE_UNSELECTED);
    ++m_verticalIt;
}

void PartyEffect::DoHorizontalAnimUpdate()
{
    if(!m_horizontalIt)
    {
        LedPanel& panel = gContext->Panel;
        m_horizontalIt = panel.HorizontalIterator();
    }

    m_horizontalIt.SetBrightness(255, EUpdateMode::IGNORE_UNSELECTED);
    ++m_horizontalIt;
}

void PartyEffect::DoRingAnimUpdate()
{
    if(!m_ringIt)
    {
        LedPanel& panel = gContext->Panel;
        m_ringIt = panel.RingIterator(3);
    }

    m_ringIt.SetBrightness(255, EUpdateMode::IGNORE_UNSELECTED);
    ++m_ringIt;
}