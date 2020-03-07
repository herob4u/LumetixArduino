#ifndef VARIABLE_RESPONSE_H
#define VARIABLE_RESPONSE_H

#include "Curve.h"

/*  Variable Response
*
*   A poorly named class that simply outputs a response value
*   to a continously changing monitored variable. The output due
*   to the monitored variable is dictated by a curve that best fits
*   the required output characteristic. i.e How much brightness compensation is 
*   needed for a change in an intensity variable.
*/
class VariableResponse
{
public:
    VariableResponse(float& monitorVar, float min, float max);

    void SetResponseCurve(const Curve& curve);
    void ResetRange(float newMin, float newMax);

    /* Returns the response to the currently monitored variable based on interpolation on a curve */
    float GetValue() const;
    
    float DebugGetParameter();
private:
    Curve m_Curve;

    /* Non-nullable reference to variable being monitored */
    float& m_MonitorVar;

    /* Represent the range of the monitored variable */
    float m_MinVal;
    float m_MaxVal;
};
#endif // !VARIABLE_RESPONSE
