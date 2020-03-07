#include "VariableResponse.h"

static inline float Clamp(float val, float min, float max)
{
	return val < min ? min : val > max ? max : val;	
}

VariableResponse::VariableResponse(float& monitorVar, float min, float max)
    : m_Curve(Curve(4))
    , m_MonitorVar(monitorVar)
    , m_MinVal(min)
    , m_MaxVal(max)
{

}

void VariableResponse::SetResponseCurve(const Curve& curve)
{
    // Just deep copy its ass
    m_Curve.Clone(curve);
}

void VariableResponse::ResetRange(float newMin, float newMax)
{
    m_MinVal = newMin;
    m_MaxVal = newMax;
}

float VariableResponse::GetValue() const
{
    const float clampedVar = Clamp(m_MonitorVar, m_MinVal, m_MaxVal);
    float t = (clampedVar - m_MinVal)/(m_MaxVal - m_MinVal);
  if (t == 1) t -= 0.1;
    return m_Curve.Evaluate(t);
}

float VariableResponse::DebugGetParameter() {
  const float clampedVar = Clamp(m_MonitorVar, m_MinVal, m_MaxVal);
  float t = (clampedVar - m_MinVal)/(m_MaxVal - m_MinVal);
  if (t == 1) t -= 0.1;
  return t;
}
