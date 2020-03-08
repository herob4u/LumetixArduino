//#pragma once

/*  Enabled Indices of LEDs for a specific panel 
*   This is a bitmask as to avoid wasting memory
*   on dynamic arrays. 2 bytes for 16 bits, each
*   representing the select state of an LED in the panel.
*/
struct LedSelectFlags
{
    short Flags;

    void operator++()
    {
        Flags = (Flags << 1);
    }

    void operator--()
    {
        Flags = (Flags >> 1);
    }
};

/*  An iterator uses a virtual panel to spatially traverse 
*   the LEDs on the panels. An iterator keeps track of the current
*   selection of LEDs in its traversal.
*
*   A Mask is used to influence neighboring LEDs. The Falloff is used
*   to govern intensity attenuation with every increment of LED from the given
*   selection (100% for discrete intensity separation)
*/
struct PanelIterator
{
    PanelIterator(LedPanel& panel, size_t maskSize = 1)
    : Panel(panel)
    , Mask(1)
    , Falloff(1.f)
    {
        for(int i = 0; i < maskSize; i++)
        {
            Mask |= ( 1 << i );
        }
        Serial.print("MASK: "); Serial.println(Mask);
    }

    /* For the currently selected pins by the iterator, set their brightness to the specified value */
    void SetBrightness(byte brightness, EUpdateMode updateMode = EUpdateMode::IGNORE_UNSELECTED)
    {
        unsigned short topPanel = 0;
        unsigned short rightPanel = 0;
        unsigned short bottomPanel = 0;
        unsigned short leftPanel = 0;

        // Perform physical mapping ...
        for(int i = 0; i < NUM_CHANNELS; i++)
        {
            unsigned short channel = PhysicalMapping[i];

            topPanel    |= ((1 << channel) & SelectFlags[0]);
            rightPanel  |= ((1 << channel) & SelectFlags[1]);
            bottomPanel |= ((1 << channel) & SelectFlags[2]);
            leftPanel   |= ((1 << channel) & SelectFlags[3]);
        }
        Panel.FromChannelMap(topPanel, rightPanel, bottomPanel, leftPanel, brightness, updateMode);
    }

    void DebugPrint()
    {
        Serial.print("Top: "); Serial.println(SelectFlags[0]);
        Serial.print("Right: "); Serial.println(SelectFlags[1]);
        Serial.print("Bottom: "); Serial.println(SelectFlags[2]);
        Serial.print("Left: "); Serial.println(SelectFlags[3]);
        Serial.println("\n");
    }
protected:
    size_t StepCount;
    unsigned short SelectFlags[EPanel::MAX_VAL];
    unsigned short Mask;     // Masking operation that allows selecting neighboring LEDs from a given selection
    float Falloff; // Percent falloff of intensity with every LED step;
    LedPanel& Panel;
};

/*  The vertical iterator holds a reference to LEDs that form a 
*   a row across the panel. By that definition, the first and end
*   iterations will address 16 LEDs, whereas in-betweens have 2.
*/
struct VerticalPanelIterator : public PanelIterator
{
    VerticalPanelIterator(LedPanel& panel, size_t maskSize = 1)
        : PanelIterator(panel, maskSize)
    {
        Serial.println("CONSTRUCTED");
        Serial.flush();
        StepCount = 0;

        SelectFlags[EPanel::TOP] = LED_MASK_ALL;
        SelectFlags[EPanel::RIGHT] = 0;
        SelectFlags[EPanel::BOTTOM] = 0;
        SelectFlags[EPanel::LEFT] = 0;
    }

    void operator++()
    {
        // Last step. Bottom panel fully active.
        if(StepCount == NUM_CHANNELS - 2)
        {
            SelectFlags[EPanel::TOP] = 0;
            SelectFlags[EPanel::RIGHT] = 0;
            SelectFlags[EPanel::BOTTOM] = LED_MASK_ALL;
            SelectFlags[EPanel::LEFT] = 0;
        }
        else
        {
            SelectFlags[EPanel::TOP] = 0;
            SelectFlags[EPanel::RIGHT] = (Mask << StepCount);
            SelectFlags[EPanel::BOTTOM] = 0;
            SelectFlags[EPanel::LEFT] = (Mask << StepCount);
        }

        StepCount++;
    }

    void operator--()
    {
        // Last step. top panel fully active.
        if(StepCount == 1)
        {
        }
        else
        {
        }

        StepCount--;
    }

    operator bool()
    {
        return StepCount >= 0 && StepCount < NUM_CHANNELS;
    }
};

struct HorizontalPanelIterator : public PanelIterator
{
    HorizontalPanelIterator(LedPanel& panel)
        : PanelIterator(panel)
    {
    }

    operator bool()
    {
        return StepCount >= 0 && StepCount < NUM_CHANNELS;
    }
};

/* Ring iterators allow for traversing the panel in a clockwise ring starting from the top panel to the left panel */
struct RingPanelIterator : public PanelIterator
{
    RingPanelIterator(LedPanel& panel)
        : PanelIterator(panel)
    {
    /*
        StepCount = 0;
        SelectFlags[EPanel::TOP] = { LED_MASK(0) };
        SelectFlags[EPanel::RIGHT] = { 0 };
        SelectFlags[EPanel::BOTTOM] = { 0 };
        SelectFlags[EPanel::LEFT] = { 0 };
        */
    }

    void operator++()
    {
        // Top Panel: 0 -> 15
        // Right Panel: 16->31
        // Bottom Panel: 32->47
        // Left Panel: 48->63
        /*
        int panel = ((StepCount) / NUM_CHANNELS);
        int channel = ((StepCount) % NUM_CHANNELS);
        Serial.print("Step count from "); Serial.println(StepCount);

        SelectFlags[panel].Flags &= ~(1 << channel);
        ++StepCount;

        Serial.print("to "); Serial.println(StepCount);

        panel = ((StepCount) / NUM_CHANNELS);
        channel = ((StepCount) % NUM_CHANNELS);
        SelectFlags[panel].Flags |= (1 << channel);
        */
    }

    void operator--()
    {
    /*
        int panel = ((StepCount) / NUM_CHANNELS);
        int channel = ((StepCount) % NUM_CHANNELS);
        Serial.print("Step count from "); Serial.println(StepCount);

        SelectFlags[panel].Flags &= ~(1 << channel);
        --StepCount;

        Serial.print("to "); Serial.println(StepCount);

        panel = ((StepCount) / NUM_CHANNELS);
        channel = ((StepCount) % NUM_CHANNELS);
        SelectFlags[panel].Flags |= (1 << channel);
        */
    }

    operator bool()
    {
        return (StepCount >= 0 && StepCount < (NUM_CHANNELS * EPanel::MAX_VAL));
    }
};