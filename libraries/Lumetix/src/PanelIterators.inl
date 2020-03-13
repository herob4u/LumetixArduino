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
*   A Mask is used to influence neighboring LEDs.
*/
struct PanelIterator
{
    PanelIterator(LedPanel& panel, size_t maskSize = 1)
    : Panel(panel)
    , Mask(1)
    {
        for(int i = 0; i < maskSize; i++)
        {
            Mask |= ( 1 << i );
        }
        LOG("MASK: "); LOGN(Mask);
    }

    PanelIterator& operator=(const PanelIterator& Other)
    {
        StepCount = Other.StepCount;
        SelectFlags[EPanel::TOP]    = Other.SelectFlags[EPanel::TOP];
        SelectFlags[EPanel::RIGHT]  = Other.SelectFlags[EPanel::RIGHT];
        SelectFlags[EPanel::BOTTOM] = Other.SelectFlags[EPanel::BOTTOM];
        SelectFlags[EPanel::LEFT]   = Other.SelectFlags[EPanel::LEFT];

        Mask = Other.Mask;
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
        LOG("Top: ");       LOGN(SelectFlags[0]);
        LOG("Right: ");     LOGN(SelectFlags[1]);
        LOG("Bottom: ");    LOGN(SelectFlags[2]);
        LOG("Left: ");      LOGN(SelectFlags[3]);
        LOG("\n");
    }
protected:
    size_t StepCount;
    unsigned short SelectFlags[EPanel::MAX_VAL];
    unsigned short Mask;     // Masking operation that allows selecting neighboring LEDs from a given selection
    LedPanel& Panel;
};

/*  The vertical iterator holds a reference to LEDs that form a 
*   a row across the panel. By that definition, the first and end
*   iterations will address 16 LEDs, whereas in-betweens have 2.
*
*   In our configuration, the left and right panels are physically MIRRORED.
*   As a result, the left and right panels iterate in reverse
*/
struct VerticalPanelIterator : public PanelIterator
{
    VerticalPanelIterator(LedPanel& panel, size_t maskSize = 1)
        : PanelIterator(panel, maskSize)
    {
        LOGN("Vertical It");
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
            SelectFlags[EPanel::RIGHT] = (LED_MASK_REVERSE(Mask) >> StepCount); //(Mask << StepCount);
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
            SelectFlags[EPanel::TOP] = LED_MASK_ALL;
            SelectFlags[EPanel::RIGHT] = 0;
            SelectFlags[EPanel::BOTTOM] = 0;
            SelectFlags[EPanel::LEFT] = 0;
        }
        else
        {
            SelectFlags[EPanel::TOP] = 0;
            SelectFlags[EPanel::RIGHT] = (Mask << StepCount);
            SelectFlags[EPanel::BOTTOM] = 0;
            SelectFlags[EPanel::LEFT] = (LED_MASK_REVERSE(Mask) >> StepCount);
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
    HorizontalPanelIterator(LedPanel& panel, size_t maskSize = 1)
        : PanelIterator(panel, maskSize)
    {
        LOGN("Horizontal It");
        Serial.flush();
        StepCount = 0;

        SelectFlags[EPanel::TOP] = 0;
        SelectFlags[EPanel::RIGHT] = 0;
        SelectFlags[EPanel::BOTTOM] = 0;
        SelectFlags[EPanel::LEFT] = LED_MASK_ALL;
    }

    void operator++()
    {
        // Last step. Right panel fully active.
        if(StepCount == NUM_CHANNELS - 2)
        {
            SelectFlags[EPanel::TOP] = 0;
            SelectFlags[EPanel::RIGHT] = LED_MASK_ALL;
            SelectFlags[EPanel::BOTTOM] = 0;
            SelectFlags[EPanel::LEFT] = 0;
        }
        else
        {
            SelectFlags[EPanel::TOP] = (Mask << StepCount);
            SelectFlags[EPanel::RIGHT] = 0;
            SelectFlags[EPanel::BOTTOM] = (LED_MASK_REVERSE(Mask) >> StepCount); // bottom is reversed?
            SelectFlags[EPanel::LEFT] = 0;
        }

        StepCount++;
    }

    void operator--()
    {
        // Last step. left panel fully active.
        if(StepCount == 1)
        {
            SelectFlags[EPanel::TOP] = 0;
            SelectFlags[EPanel::RIGHT] = 0;
            SelectFlags[EPanel::BOTTOM] = 0;
            SelectFlags[EPanel::LEFT] = LED_MASK_ALL;
        }
        else
        {
            SelectFlags[EPanel::TOP] = (LED_MASK_REVERSE(Mask) >> StepCount);
            SelectFlags[EPanel::RIGHT] = 0;
            SelectFlags[EPanel::BOTTOM] = (Mask << StepCount);
            SelectFlags[EPanel::LEFT] = 0;
        }

        StepCount--;
    }

    operator bool()
    {
        return StepCount >= 0 && StepCount < NUM_CHANNELS;
    }
};

/* Ring iterators allow for traversing the panel in a clockwise ring starting from the top panel to the left panel */
struct RingPanelIterator : public PanelIterator
{
    RingPanelIterator(LedPanel& panel, size_t maskSize = 1)
        : PanelIterator(panel, maskSize)
    {
        /* Initially, the zero-th bit of the top panel is on. aka start from the upper left corner*/
        StepCount = 0;
        SelectFlags[EPanel::TOP] = LED_MASK(0);
        SelectFlags[EPanel::RIGHT] = 0 ;
        SelectFlags[EPanel::BOTTOM] = 0 ;
        SelectFlags[EPanel::LEFT] =  0 ;
    }

    void operator++()
    {
        // Top Panel: 0 -> 15
        // Right Panel: 16->31
        // Bottom Panel: 32->47
        // Left Panel: 48->63
        
        unsigned short panel = ((++StepCount) / NUM_CHANNELS);
        unsigned short channel = ((StepCount) % NUM_CHANNELS);
        
        SelectFlags[EPanel::TOP]    = panel == 0 ? (Mask << channel) : 0;
        SelectFlags[EPanel::RIGHT]  = panel == 1 ? (LED_MASK_REVERSE(Mask) >> channel) : 0;
        SelectFlags[EPanel::BOTTOM] = panel == 2 ? (LED_MASK_REVERSE(Mask) >> channel) : 0; // Assumes bottom is reversed, needs validation
        SelectFlags[EPanel::LEFT]   = panel == 3 ? (Mask << channel) : 0;

    }

    void operator--()
    {
        unsigned short panel = ((--StepCount) / NUM_CHANNELS);
        unsigned short channel = ((StepCount) % NUM_CHANNELS);

        SelectFlags[EPanel::TOP]    = panel == 0 ? (LED_MASK_REVERSE(Mask) >> channel) : 0;
        SelectFlags[EPanel::RIGHT]  = panel == 1 ? (Mask << channel) : 0;
        SelectFlags[EPanel::BOTTOM] = panel == 2 ? (Mask << channel) : 0; // Assumes bottom is reversed, needs validation
        SelectFlags[EPanel::LEFT]   = panel == 3 ? (LED_MASK_REVERSE(Mask) >> channel) : 0;
    }

    operator bool()
    {
        return (StepCount >= 0 && StepCount < (NUM_CHANNELS * EPanel::MAX_VAL));
    }
};