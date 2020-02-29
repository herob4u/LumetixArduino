#include <VariableResponse.h>
#include <ResponseCurves.h>

#include <SparkFunISL29125.h>

#include <TLC59116.h>
#include <TLC59116_Unmanaged.h>

#include <Wire.h>

#define START_BYTE (0x20)
#define STOP_BYTE (0X7F)
#define CHANNELS_COUNT (16)

// Returns the nth bit in x
#define BIT(x, n) (1 << n & x)
#define TOP_PANEL_MASK      0x0
#define RIGHT_PANEL_MASK    0x1
#define BOTTOM_PANEL_MASK   0x2
#define LEFT_PANEL_MASK     0x3


static const float intensity_multiplier = 0.5;
static const float neutral = 2.4;
static const float buffer = 0.2;
static const float maxWarm = 5.0;
static const float maxCool = 0.0;

static float _min = 1.5;
static float _max = 3.5;
static float RBf = _min;
static float Rf = 1;
/* Variable Responses */
static VariableResponse W_LedResponse(RBf, _min, _max);
static VariableResponse Y_LedResponse(RBf, _min, _max);
static VariableResponse R_LedResponse(Rf, 0, .5);
static int bIsPaused = 0;

// Declare sensor object
SFE_ISL29125 RGB_sensor;

TLC59116Manager tlcmanager;

byte ledBuffer[4][CHANNELS_COUNT];

byte white_channels[6] = {7, 1, 0, 15, 14, 8};
byte yellow_channels[4] = {6, 2, 13, 9};
byte red_channels[] = {3, 12};

/* Example test code to demonstrate curve usage
#include "VariableResponse.h"
#include "ResponseCurves.h"

static float RfBf = 0;
static VariableResponse ledResponse(RfBf, 1.5, 3.5); // Will monitor RfBf and give us an intensity response based on its current value

void setup()
{
    // Create some curves. Here, I am making one out of an existing template. #DataDrivenDesign
    Curve whiteResponseCurve(W_CoolWarmResponse, sizeof(W_CoolWarmResponse)/sizeof(Key));
    ledResponse.SetResponseCurve(whiteResponseCurve);

    // If you want custom curves, either statically initialize one in ResponseCurves.h or create one on the spot
    Curve invSquareCurve(3); // 3 being a capacity hint for how many points are in the curve
    invSquareCurve.AddKey(0.5, 0.75);   // output = 0.75 when parameter t = 0.5
    invSquareCurve.AddKey(1.0, 1);      // output = 1 when parameter t = 1

    // Alternatively...
    // Output that oscillates every 0.2t, looks like a triangular wave, which will be smoothed into a sinusoidal-like curve
    static Key keys[] = 
    {
        {0, -1}, {0.2, 1}, {0.4, -1}, {0.6, 1}, {0.8, -1}, {1, 1}
    };
    
    Curve wavyCurve(keys, sizeof(keys)/sizeof(Key));
}

void loop()
{
    float R = RGB_sensor.readRed();
    float G = RGB_sensor.readGreen();
    float B = RGB_sensor.readBlue();

    RfBf = R/(G+B);

    float whiteIntensity = ledResponse.GetValue();
    
    Light(WhitLeds, whiteIntensity);
}

*/
static const int PACKET_SIZE = 19;

void setup() 
{
    Serial.begin(9600); // for some reason, any other value causes issues. Cant light LED. Maybe android side bug when selecting baud rate?
    Serial.println("Starting...");
    Serial.flush();

    tlcmanager.init();


    // Initialize the ISL29125 with simple configuration so it starts sampling
    if (RGB_sensor.init())
    {
      Serial.println("Sensor Initialization Successful\n\r");
    }
    else
    {
      Serial.println("No RGB Sensor");
    }

    // Assign the response curve for white light
    Curve W_ResponseCurve(W_CoolWarmResponse, KARR_LEN(W_CoolWarmResponse));
    Curve Y_ResponseCurve(Y_CoolWarmResponse, KARR_LEN(Y_CoolWarmResponse));
    Curve R_ResponseCurve(R_CCResponse, KARR_LEN(R_CCResponse));
    
    W_ResponseCurve.Rebuild();
    Y_ResponseCurve.Rebuild();
    
    W_LedResponse.SetResponseCurve(W_ResponseCurve);
    Y_LedResponse.SetResponseCurve(Y_ResponseCurve);
    R_LedResponse.SetResponseCurve(R_ResponseCurve);

    Key* keys = W_ResponseCurve.DebugGetKeys();
    size_t count = W_ResponseCurve.GetNumKeys();

    for(int i = 0; i < count; i++)
    {
      Serial.print("t, value: "); Serial.print(keys[i].Alpha); Serial.print(", "); Serial.println(keys[i].Value);
    }
    
    // Zero out all LED values (physical board inits to zero as well)
    clearLedBuffer();

    delay(25);

    flushSerialInput();
}

void loop() 
{   
    // Important: Must be EQUAL to packet size. Failure to do so introduces oddities in the transmission
    // That makes us respond at later steps - confuses the hell out of UI.

    if(Serial.available() > 0)
    {
        // Read special character for pause
        byte input = Serial.read();
        if(input == 'p')
        {
            bIsPaused = !bIsPaused;
        } else {
        switch(input)
        {
          case '1':
            RBf = ((_max - _min)* 0.1 + _min);
          break;

          case '2':
            RBf = ((_max - _min)* 0.2 + _min);
          break;

          case '3':
            RBf = ((_max - _min)* 0.3 + _min);
          break;

          case '4':
            RBf = ((_max - _min)* 0.4 + _min);
          break;

          case '5':
            RBf = ((_max - _min)* 0.5 + _min);
          break;

          case '6':
            RBf = ((_max - _min)* 0.6 + _min);
          break;

          case '7':
            RBf = ((_max - _min)* 0.7 + _min);
          break;

          case '8':
            RBf = ((_max - _min)* 0.8 + _min);
          break;

          case '9':
            RBf = ((_max - _min)* 0.9 + _min);
          break;

          case '0':
            RBf = ((_max - _min)* 1 + _min);
          break;
        }
        }
    }

    if(bIsPaused)
    {
        Serial.println("PAUSED");
        return; 
    }
    
  // Read sensor values (16 bit integers)
  float red = RGB_sensor.readRed();
  float green = RGB_sensor.readGreen();
  float blue = RGB_sensor.readBlue();
//
  float REDf = red /(green + blue);
  float GREENf = green/(red + blue);
  float BLUEf = blue/(red + green);
//  float RG = red/green;
//  float RGf = REDf/GREENf;
//
  
  Rf = red/(red+green+blue);
  RBf = REDf/BLUEf;
  float GBf = GREENf/BLUEf;
//  
//  // Print out readings, change HEX to DEC if you prefer decimal output
//  Serial.print("Red: "); Serial.println(REDf);
//  //Serial.print("Green: "); Serial.println(GREENf);
//  Serial.print("Blue: "); Serial.println(BLUEf);
//  //Serial.print("R/G: "); Serial.println(RG);
//  //Serial.print("Rf/Gf: "); Serial.println(RGf);
//

  Serial.print("InvRedf: "); Serial.println(Rf);
  Serial.print("Rf/Bf: "); Serial.println(RBf);
//  //Serial.print("Gf/Bf: "); Serial.println(GBf);

  //float delta = tempDelta(RBf);
  float W_response = W_LedResponse.GetValue();
  float Y_response = Y_LedResponse.GetValue();
  float R_response = R_LedResponse.GetValue();
  
//  Serial.print("Parameter t = "); Serial.println(W_LedResponse.DebugGetParameter());
//  Serial.print("Response Value = "); Serial.println(W_response);
//
//  Serial.print("Parameter t2 = "); Serial.println(Y_LedResponse.DebugGetParameter());
//  Serial.print("Response Value2 = "); Serial.println(Y_response);

  Serial.print("Parameter t_red = "); Serial.println(R_LedResponse.DebugGetParameter());
  
  Light(white_channels, sizeof(white_channels), W_response);
  Light(yellow_channels, sizeof(yellow_channels), Y_response);
  Light(red_channels, sizeof(red_channels), R_response);
  
  /*
  if(delta < 0)
  {
      Light(yellow_channels, sizeof(yellow_channels), -delta);
      Light(white_channels, sizeof(white_channels), 0);
  }
  else if (delta > 0)
  {
      Light(white_channels, sizeof(white_channels), delta);
      Light(yellow_channels, sizeof(yellow_channels), 0);  
  }
  else
  {
      Light(white_channels, sizeof(white_channels), 0);
      Light(yellow_channels, sizeof(yellow_channels), 0);  
  }
  */
  Serial.println();
  delay(450);
}

void Light(byte* arr, int count, float intensity)
{
    byte val = 255 * intensity;
    if(val < 0)
      val = 0;
    if(val > 255)
      val = 255;
      
    Serial.print("Intensity: "); Serial.println(val);
    for(int i = 0; i < count ; i++)
    {
        tlcmanager[0].pwm(arr[i], val);
        tlcmanager[1].pwm(arr[i], val);
        tlcmanager[2].pwm(arr[i], val);
        tlcmanager[3].pwm(arr[i], val);
    }
}

void updateLeds()
{
    const int device_count = tlcmanager.device_count();

    if(device_count == 0)
      return;

    // In our case, we refer to the top panel as index 0, and we go clockwise from there
    // Top -> Right -> Bottom -> Left
    for(int i = 0; i < device_count; i++)
    {
        for(int channel = 0; channel < CHANNELS_COUNT; channel++)
        {
            tlcmanager[i].pwm(channel, ledBuffer[i][channel]);
        }
    }
}

float tempDelta(float ratio)
{
    if(ratio < neutral - buffer) {
      float bufferBoundary = neutral - buffer;
      return (float)(ratio - bufferBoundary)/(abs(maxWarm - bufferBoundary));
    }
    else if (ratio > neutral + buffer) {
      float bufferBoundary = neutral + buffer;
      return (float)(ratio - bufferBoundary)/(abs(maxCool - bufferBoundary));
    }

    return 0;
}

void clearLedBuffer()
{
    const int device_count = tlcmanager.device_count();
    for(int i = 0; i < device_count; i++)
    {
        for(int channel = 0; channel < CHANNELS_COUNT; channel++)
        {
            ledBuffer[i][channel] = 0;
        }
    }
}

void beginReadTransmission()
{    

    // Delay for safety to ensure all bytes arrived
    delay(25);

    int numBytes = 16;

    if(numBytes < 16)
    {
        Serial.println("Expected at least 16 bytes transmission");
        abortReadTransmission();
        return;
    }

    const int headerInfo = Serial.read(); // Reads the HEADER BYTE (This is a placeholder since we are not using the header yet)
    const int panelId = ParsePanelId(headerInfo);

    // We expect the header to tell us whether one of the four panels are being addressed
    if(panelId < 0 || panelId > 3)
    {
        abortReadTransmission();
        return;
    }

    // Update the correspond panel
    for(int i = 0; i < CHANNELS_COUNT; i++)
    {
        byte brightness = Serial.read();
        ledBuffer[panelId][i] = brightness;
        // Might want to delay here
    }

    // This is also another possible culprit. For correctness, the conditional should be ANDed. It wasn't until I changed this to an OR that I have seen results.
    // Perhaps it is leaving us at an undefined state because it is not popping off the STOP_BYTE (Serial.available() takes precedence)
    if( !(Serial.available() || Serial.read() == STOP_BYTE) )
    {
        Serial.println("Corrupt transmission, expected STOP BYTE");
        abortReadTransmission();
        return;
    }

    // Make sure we clear characters like new line, return carriage, and other irrelevant characters just to be safe
    flushSerialInput();

    updateLeds();
}

int ParsePanelId(int header)
{
    const int result = header & 0x3;

    switch(result)
    {
        case TOP_PANEL_MASK: return 0;
        case RIGHT_PANEL_MASK: return 1;
        case BOTTOM_PANEL_MASK: return 2;
        case LEFT_PANEL_MASK: return 3;
        default: return 0;
    }
}
void abortReadTransmission()
{
    endReadTransmission();

    // Reset the LED buffer
    clearLedBuffer();
}

void endReadTransmission()
{
    Serial.flush();
    flushSerialInput();
}

void flushSerialInput()
{
    while(Serial.available())
    {
        Serial.read();
    }
}
