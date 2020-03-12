#include "ColorCorrectEffect.h"

#include "../VariableResponse/ResponseCurves.h"

static const char CALIBRATE_SYMBOL = 's';
const float ColorCorrectEffect::s_DefaultMinResponse = 0.f;
const float ColorCorrectEffect::s_DefaultMaxResponse = 1.f;

static const float s_DefaultRIntensity = .2f;
static const float s_DefaultBIntensity = .3f;

ColorCorrectEffect::ColorCorrectEffect()
    : EffectBase(EffectType::VIDEO_EFFECT)
    , RBf(1.f)
    , RIntensityMultiplier(s_DefaultRIntensity)
    , BIntensityMultiplier(s_DefaultBIntensity)
    , WarmResponse(RBf, s_DefaultMinResponse, s_DefaultMaxResponse)
    , CoolResponse(RBf, s_DefaultMinResponse, s_DefaultMaxResponse)
{
    // Init Curves...
    Curve CoolResponseCurve(W_CoolWarmResponse, KARR_LEN(W_CoolWarmResponse));
    Curve WarmResponseCurve(Y_CoolWarmResponse, KARR_LEN(Y_CoolWarmResponse));

    // Just in case...
    CoolResponseCurve.Rebuild();
    WarmResponseCurve.Rebuild();

    CoolResponse.SetResponseCurve(CoolResponseCurve);
    WarmResponse.SetResponseCurve(WarmResponseCurve);
}

void ColorCorrectEffect::OnApplied()
{

}

void ColorCorrectEffect::OnUpdate(float deltaTime)
{
    RGBSensor& sensor = gContext->RgbSensor;

    // Read sensor values (16 bit integers)
    float red   = sensor.readRed();
    float green = sensor.readGreen();
    float blue  = sensor.readBlue();
    //
    float REDf      = red / (green + blue);
    float GREENf    = green / (red + blue);
    float BLUEf     = blue / (red + green);

    RBf         = REDf / BLUEf;

    LOG("Rf/Bf: "); LOGN(RBf);

    float W_response = CoolResponse.GetValue();
    float Y_response = WarmResponse.GetValue();

    LOG("Parameter t = "); LOGN(CoolResponse.DebugGetParameter());
    LOG("Cool Response Value = "); LOGN(W_response);
    LOG("Warm Response Value = "); LOGN(Y_response);

    // Proportionally Attenuated Response
    W_response *= RIntensityMultiplier;
    Y_response *= BIntensityMultiplier;

    float Yellow_Resp = max(W_response, Y_response);

    float R_response = Y_response;
    float B_response = max(W_response, Y_response);
    float G_response = max(W_response, Y_response);

    LedPanel& ledPanel = gContext->Panel;
    ledPanel.SetBrightness(ELedColor::WHITE, ceil(W_response * 255));
    ledPanel.SetBrightness(ELedColor::YELLOW, ceil(Yellow_Resp * 255));
    ledPanel.SetBrightness(ELedColor::RED, ceil(R_response * 255));
    ledPanel.SetBrightness(ELedColor::GREEN, ceil(G_response * 255));
    ledPanel.SetBrightness(ELedColor::BLUE, ceil(B_response * 255));

}

void ColorCorrectEffect::OnRemoved()
{

}

void ColorCorrectEffect::Calibrate()
{
    RGBSensor& sensor = gContext->RgbSensor;
    float RBfAvg = 0;

    for(int i = 0; i < 30; i++) {
        // Read sensor values (16 bit integers)
        float red   = sensor.readRed();
        float green = sensor.readGreen();
        float blue  = sensor.readBlue();
        //
        float REDf      = red / (green + blue);
        float GREENf    = green / (red + blue);
        float BLUEf     = blue / (red + green);

        RBfAvg += REDf / BLUEf;
    }

    RBfAvg /= 30.0;

    float newMin = 0.f;
    float newMax = RBfAvg * 2.f;

    CoolResponse.ResetRange(newMin, newMax);
    WarmResponse.ResetRange(newMin, newMax);
}

void ColorCorrectEffect::OnSetArgs(EffectArgs& args)
{
    // Expecting 1 argument, telling us the new average RBf value OR a symbol for calibration
    /*
    if(args.NumArgs == 1)
    {
        // See if the message is an already calibrated value for RBf
        ByteBuffer buffer = args.ArgBuffer;
        if(buffer.GetNumBytes() == sizeof(float))
        {
            float RBfAvg = buffer.GetFloat();

            float newMin = 0.f;
            float newMax = RBfAvg * 2.f;

            CoolResponse.ResetRange(newMin, newMax);
            WarmResponse.ResetRange(newMin, newMax);
        }
        else if(buffer.GetNumBytes() == sizeof(char))
        {
            // We received a calibration notice instead.
            char c = buffer.GetChar();
            if(c == CALIBRATE_SYMBOL)
            {
                Calibrate();
            }
        }
    }
    */
    if(args.NumArgs == 1)
    {
        if(args.ArgBuffer.GetNumBytes() == sizeof(char))
        {
            char c = args.ArgBuffer.GetChar();
            if(c == CALIBRATE_SYMBOL)
            {
                LedPanel& panel = gContext->Panel;
                panel.SetBrightness(EPanel::TOP, ELedColor::RED, 255);
                delay(500);
                Calibrate();
            }
        }
    }
}