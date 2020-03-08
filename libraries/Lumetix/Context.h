#ifndef CONTEXT_H
#define CONTEXT_H

#include "LedPanel.h"
#include "../SparkFun_ISL29125_Breakout_Arduino_Library-master/src/SparkFunISL29125.h"

typedef SFE_ISL29125 RGBSensor;

/* Global context to facilitate working of de-coupled features such as Effects 
*  Simply include this header and access gContext. gContext must be defined in the .ino file
*  following sensor and led panel initializations.
*/
struct Context
{
    Context(LedPanel& panel, RGBSensor& sensor)
    : Panel(panel)
    , RgbSensor(sensor)
    {
    }

    LedPanel& Panel;
    RGBSensor& RgbSensor;
};
extern Context* gContext;
#endif // !CONTEXT_H
