#include "VariableResponse.h"

VariableResponse::VariableResponse(float& monitorVar, float min, float max)
    : m_Curve(Curve(4))
    , m_MonitorVar(monitorVar)
    , m_MinVal(min)
    , m_MaxVal(max)
{

}

void VariableResponse::SetResponseCurve(const Curve& curve)
{
    // Just copy its ass
    m_Curve = curve;
}

void VariableResponse::ResetRange(float newMin, float newMax)
{
    m_MinVal = newMin;
    m_MaxVal = newMax;
}

float VariableResponse::GetValue() const
{
    const float t = (m_MonitorVar - m_MinVal)/(m_MaxVal - m_MinVal);
    return m_Curve.Evaluate(t);
}