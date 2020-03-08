#include "LightSequencer.h"

#include "Context.h"

void LightSequencer::Hello()
{
    if(!gContext)
        return;
    
    LedPanel& panel = gContext->Panel;

    /* Circle around 5 times with increasing intensities */
    for(int i = 0; i < 5; i++)
    {
        auto ringIt = panel.RingIterator();
        byte brightness = (50.f*(i+1)); // Arbitrary

        while(ringIt)
        {
            ringIt.SetBrightness(brightness);
            ++ringIt;
            delay(25);
        }
    }

    /* Vertical Swipe, or as Chad would put it - "Boop! boop boop boop boop!" */
    auto verticalIt = panel.VerticalIterator();
    while(verticalIt)
    {
        verticalIt.SetBrightness(128U);
        ++verticalIt;
        delay(60);
    }

    --verticalIt;
    delay(20);
    while(verticalIt)
    {
        verticalIt.SetBrightness(128U);
        --verticalIt;
        delay(60);
    }

    // Prep for Additive Mode
    panel.TurnOff(true);

    /* For good measures... horizontal swipe */
    auto horizontalIt = panel.HorizontalIterator(3U);
    while(horizontalIt)
    {
        // Should give us a nice fade where rightward LEDs are weaker
        horizontalIt.SetBrightness(30, EUpdateMode::ADD);
        ++horizontalIt;
        delay(60);
    }
}

void LightSequencer::FadeRing()
{
    if(!gContext)
        return;

    LedPanel& panel = gContext->Panel;

    auto ringIt = panel.RingIterator();

    // Increment by 4 such that the last LED will have brightness 64*4 = 255
    byte incr = 4;
    while(ringIt)
    {
        ringIt.SetBrightness(incr);
        ++ringIt;
        incr += incr;
        delay(25);
    }
}

void LightSequencer::FillRing()
{
    if(!gContext)
        return;

    LedPanel& panel = gContext->Panel;

    auto ringIt = panel.RingIterator();

    // Increment by 4 such that the last LED will have brightness 64*4 = 255
    while(ringIt)
    {
        ringIt.SetBrightness(255);
        ++ringIt;
        delay(25);
    }
}


