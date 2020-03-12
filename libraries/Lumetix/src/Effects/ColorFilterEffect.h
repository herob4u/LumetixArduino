#ifndef COLOR_FILTER_EFFECT_H
#define COLOR_FILTER_EFFECT_H
#include "../EffectBase.h"

class ColorFilterEffect : public EffectBase
{
public:
    ColorFilterEffect();

    enum FilterType : short
    {
        BLUE = 2,
        YELLOW = 3,
        RED = 4,
        GREEN = 5,
        RED_YELLOW = 6,
        BLUE_GREEN = 7,
        RED_BLUE = 8,
        TYPE9 = 9,
        TYPE10 = 10,
        TYPE11 = 11,
        TYPE12 = 12,
        TYPE13 = 13,
        TYPE14 = 14
    };

    virtual void OnApplied() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRemoved() override;
    virtual void OnSetArgs(EffectArgs& args) override;
protected:
    void ApplyFilter(byte brightness);
private:
    FilterType m_ActiveFilter;
    float m_StrobeFrequency; // In radians
    float m_StrobeOffset;    // 0 - 255
    float m_StrobeAmplitude; // Scalar

    float m_Intensity; // 0 - 255
    float m_Time; // We keep track of our own time so that we dont overflow over time
    bool m_WasInterpEnabled;
};
#endif // !COLOR_FILTER_EFFECT_H
