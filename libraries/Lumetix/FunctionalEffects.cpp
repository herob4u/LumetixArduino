#include "FunctionalEffects.h"
#include "LedPanel.h"

#include "../SparkFun_ISL29125_Breakout_Arduino_Library-master/src/SparkFunISL29125.h"
#include "../VariableResponse/Curve.h"
#include "../VariableResponse/ResponseCurves.h"
#include "../VariableResponse/VariableResponse.h"

static float R = 0;
static float G = 0;
static float B = 0;
static float Rf = 0;
static float Gf = 0;
static float Bf = 0;
static float RGf = 0;
static float RBf = 0;

// This is really difficult, somehow need to initialize variable response array for all effects in advance.
// This involves providing a variable to track, range, and a fully constructed curve. Repeat that for all variables
// and for all effects. Extremely verbose. Besides, who is to say all effects need all color curves?

void InitEffectCurves()
{

}

void ColorCorrect(LedPanel& panel, SFE_ISL29125& rgbSensor)
{

}