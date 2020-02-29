#ifndef RESPONSE_CURVES_H
#define RESPONSE_CURVES_H

#include "Curve.h"

/* Response of White light in response to changes to a parameterized value t*/
static Key W_CoolWarmResponse[4]
{
    {0, 0.05},
    {0.25, 0.08},
    {0.5, 0.1},     // Neutral Light
    {0.75, 0.5},
    {1, 0.85}       // Maximum 85% brightness when at maximum response
};
#endif // !RESPONSE_CURVES_H
