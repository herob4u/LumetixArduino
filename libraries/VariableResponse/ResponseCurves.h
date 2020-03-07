#ifndef RESPONSE_CURVES_H
#define RESPONSE_CURVES_H

#include "Curve.h"

#define KARR_LEN(x) (sizeof(x)/sizeof(Key))
/* Response of White light in response to changes to a parameterized value t*/
static Key W_CoolWarmResponse[]
{
    {0, 0.0},
    {0.25, 0.1},
    {0.5, 0.4},     // Neutral Light
    {0.75, 0.8},
    {1, 1}       // Maximum 100% brightness when at maximum response
};

static Key Y_CoolWarmResponse[]
{
	{0, 1},
	{0.25, 0.8},
	{0.5, 0.4},
	{0.75, 0.1},
	{1, 0.00}
};

static Key R_CCResponse[]
{
	{0, 0},	
	{0.25, 0},	
	{0.5, 0.1},	
	{0.75, 0.15},
	{1, 0.35}
};
#endif // !RESPONSE_CURVES_H
