#ifndef FUNCTIONAL_EFFECTS_H
#define FUNCTIONAL_EFFECTS_H

class LedPanel;
class SFE_ISL29125;

namespace EPhotoEffects
{
    enum Type: short
    {
        PE_1 = 0,
        PE_2,
        PE_3,
        MAX_VAL
    };
}

namespace EVideoEffects
{
    enum Type : short
    {
        ColorCorrect = 0,
        VE_2,
        VE_3,
        MAX_VAL
    };
}


void InitEffectResponses();

void ColorCorrect(LedPanel& panel, SFE_ISL29125& rgbSensor);
#endif // !FUNCTIONAL_EFFECTS_H
