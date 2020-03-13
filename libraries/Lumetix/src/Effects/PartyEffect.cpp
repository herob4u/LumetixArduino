#include "PartyEffect.h"

/* Invokation helper for animation functions */
#define INVOKE(func) ((this->*func)())

PartyEffect::PartyEffect()
    : EffectBase(EffectType::VIDEO_EFFECT)
    , m_BpmDelay(.25f)
    , m_ElapsedTime(0)
    , m_SequenceIt(0)
    , m_AnimMode(AnimationMode::COLOR_SEQ)
    , m_RandomizeAnimations(false)
    , m_horizontalIt(gContext->Panel.HorizontalIterator())
    , m_verticalIt(gContext->Panel.VerticalIterator())
    , m_ringIt(gContext->Panel.RingIterator(3))
{
    m_ColorSequence[0] = ELedColor::BLUE;
    m_ColorSequence[1] = ELedColor::GREEN;
    m_ColorSequence[2] = ELedColor::RED;
    m_ColorSequence[3] = ELedColor::WHITE;
    m_ColorSequence[4] = ELedColor::YELLOW;

    m_SequenceBrightness[0] = 128;
    m_SequenceBrightness[1] = 90;
    m_SequenceBrightness[2] = 150;
    m_SequenceBrightness[3] = 150;
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
        Serial.println("Toggle Party Effect");

        // Toggle
        LedPanel& panel = gContext->Panel;
        
        // Perform animation step
        m_RandomizeAnimations ? INVOKE(m_AnimationFunctions[GetRandomizedAnimation()]) :
                                INVOKE(m_AnimationFunctions[m_AnimMode]);
        //INVOKE(m_AnimationFunctions[m_AnimMode]);

        // Reset timer 
        m_ElapsedTime = 0.f;
    }
}

void PartyEffect::OnRemoved()
{

}

void PartyEffect::OnSetArgs(EffectArgs& args)
{
    LOGN("Party Effect Arg");
    // Expects update mode or bpm update
    LedPanel& panel = gContext->Panel;

    if(args.NumArgs == 1)
    {
        byte bpm_byte = args.ArgBuffer.GetByte();
        LOG("Arg: "); LOGN(bpm_byte);
        panel.TurnOn(true);
        panel.TurnOff(true);
        m_BpmDelay = (60000.f/(float)bpm_byte)/1000.f;
        LOG("Delay: ");LOGN(m_BpmDelay);
    }
}

void PartyEffect::SetAnimationMode(AnimationMode mode)
{
    m_AnimMode = mode;
}

void PartyEffect::SetBpmDelay(short bpmDelay_ms)
{
    Serial.print("newbpm: ");Serial.println(bpmDelay_ms);
    m_BpmDelay = (float)bpmDelay_ms/1000.f;
}

float PartyEffect::GetRandomizedBPM() const
{
    const float half_bpm = m_BpmDelay/2.f;

    // Don't ask, accept it as a hack
    unsigned int select = random(0,2) | random(0,5) | random(0,8);
    return (select == 0 ? half_bpm : m_BpmDelay);

}

PartyEffect::AnimationMode PartyEffect::GetRandomizedAnimation() const
{
    // Don't ask, accept it as a hack
    unsigned int select = random(0,4) | random(0, 3) | random(3, 4);
    LOGN(select);
    return (select == 0 ? AnimationMode::COLOR_SEQ:
            select == 1 ? AnimationMode::VERTICAL:
            select == 2 ? AnimationMode::HORIZONTAL:
            select == 3 ? AnimationMode::RING:
            m_AnimMode);
}

/* ANIMATION FUNCTIONS */
void PartyEffect::DoSequenceAnimUpdate()
{
    ELedColor color = m_ColorSequence[m_SequenceIt];
    byte brightness = m_SequenceBrightness[m_SequenceIt++];

    LedPanel& panel = gContext->Panel;
    panel.TurnOff(true);

    /* Turn on the LEDs in accordance to the sequence. Want exclusive lighting, so IGNORE_UNSELECTED */
    panel.SetBrightness(color, brightness, EUpdateMode::IGNORE_UNSELECTED);

    // Wrap around
    if(m_SequenceIt == NUM_COLORS)
    {
        m_SequenceIt = 0;
    }
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