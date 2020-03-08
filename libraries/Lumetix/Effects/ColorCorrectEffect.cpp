#include "ColorCorrectEffect.h"

ColorCorrectEffect::ColorCorrectEffect()
    : EffectBase(EffectType::VIDEO_EFFECT)
{
}

void ColorCorrectEffect::OnApplied()
{

}

void ColorCorrectEffect::OnUpdate(float deltaTime)
{
    RGBSensor& sensor = gContext->RgbSensor;

    sensor.readRed();
    sensor.readGreen();
    sensor.readBlue();
}

void ColorCorrectEffect::OnRemoved()
{

}