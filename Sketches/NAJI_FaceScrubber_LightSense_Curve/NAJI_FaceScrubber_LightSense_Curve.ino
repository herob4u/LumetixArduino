#include <LumetixFwd.h>
#include <VariableResponse.h>
#include <ResponseCurves.h>
#include <SparkFunISL29125.h>

#include <TLC59116.h>
#include <TLC59116_Unmanaged.h>

#include <Wire.h>

/* Serial Transmission Params */
#define START_BYTE (0x20)
#define STOP_BYTE (0X7F)
#define CHANNELS_COUNT (16)
#define MSG_TIMEOUT (50) /*ms*/

#define FIXED_MSG_SIZE

static int bIsPaused = 0;

// Declare sensor object
SFE_ISL29125 RGB_sensor;

/* GLOBAL SYSTEMS */
TLC59116Manager tlcmanager;
LedPanel* ledPanel = new LedPanel(tlcmanager);
Context* gContext = new Context(*ledPanel, RGB_sensor);
EffectRegistry effectRegistry;

float g_CurrTime = 0;

void setup() 
{
    Serial.begin(9600); // for some reason, any other value causes issues. Cant light LED. Maybe android side bug when selecting baud rate?
    LOGN("Starting...");
    Serial.flush();
    
    tlcmanager.init();
    ledPanel->Init();
    LOGN("Init Panel");
    Serial.flush();
    
    // Initialize the ISL29125 with simple configuration so it starts sampling
    if (RGB_sensor.init())
    {
      LOGN("Sensor Initialization Successful\n\r");
    }
    else
    {
      LOGN("No RGB Sensor");
      Serial.flush();
      abort();
    }
    
    // Led Panel must be fully initialized before using this
    effectRegistry.Init();
    effectRegistry.ActivateEffect(1);
    
    g_CurrTime = millis()/1000.f;
    delay(25);

    flushSerialInput();
}

void loop() 
{   
  float deltaTime = (millis()/1000.f) - g_CurrTime;
  g_CurrTime = (millis()/1000.f);

  // Respond to any serial transmissions if any
  PollSerialEvents();

  // Update All Lumetix sub-systems
  ledPanel->Update(deltaTime);
  effectRegistry.Update(deltaTime);
  
  delay(5);
}

void PollSerialEvents()
{
    // Received potential START_BYTE and header
    if(Serial.available() >= 3)
    {
        Serial.println("HERE");
        ledPanel->TurnOff(true);
        delay(50);
        
        byte data = Serial.read();
        if(data == START_BYTE)
        {
            ledPanel->TurnOn(true);
            delay(50);
            Serial.println("START BYTE");
            
            // Header indicating message size
            #ifndef FIXED_MSG_SIZE
            byte msgSize = Serial.read();
            #else
            byte msgSize = 3;
            #endif

            // Message has no content, probably corrupt. Ignore
            if(msgSize < 1)
              flushSerialInput();

            // A transmission time-out occured, and not all data was successfully retrieved
            if(!WaitForData(msgSize))
              return;
            
            //int effectId = (int)Serial.read();
            byte effectId = Serial.read();

            if(effectId == 1)
            {
                ledPanel->bInterpolates = false;
                ledPanel->SetBrightness(ELedColor::GREEN, 255);
                ledPanel->Update(1.f);
                while(1) {}
            }
            else if(effectId == 49) // ascii character representing '1'
            {
                ledPanel->bInterpolates = false;
                ledPanel->SetBrightness(ELedColor::RED, 255);
                ledPanel->Update(1.f);
                while(1) {}
            }
            if(effectId == 'p')
            {
              ledPanel->TurnOn(true);
              delay(200);
              ledPanel->TurnOff(false);
              delay(200);
              ledPanel->TurnOn(true);
              delay(200);
              ledPanel->TurnOff(false);
            }
            EffectArgs outEffectArgs;
            ParseEffectArgs(effectId, --msgSize, outEffectArgs);

            effectRegistry.ActivateEffect(effectId);
            effectRegistry.NotifyArgsChanged(outEffectArgs);

            flushSerialInput();
        }
    }
}

bool WaitForData(byte remainingSize)
{
    /* Wait on serial until all data from transmission arrives safely */
    unsigned long timestamp = millis();
    while( (millis() - timestamp < MSG_TIMEOUT) && (Serial.available() < remainingSize))
    {
    }

    /* Tranmission Timeout, flush the serial buffer */
    if(Serial.available() < remainingSize)
    {
        flushSerialInput();
        return false;
    }
    Serial.println("FINISHED WAIT");
    return true;
}

#ifndef FIXED_MSG_SIZE
// @TODO: Is the cast from byte to size_t safe? Verify using setup code tests
ByteBuffer ParseEffectArgs(int effectId, size_t msgSize, EffectArgs& effectArgs)
{
    ByteBuffer argBuffer = ByteBuffer::Allocate(msgSize);

    /*
     *  Effect Ids are defined by our application. They are maintained in the EffectRegistry
     *  We could optionally handle this parsing in the Effect Registry, but that would be overkill.
     *  
     *  ID 0: Color Correct Effect (float RBf), (char calibrateSymbol)
     *  ID 1: Intensity Gradient Effect (byte intensityA, byte intensityB, byte dir)
     *  ID 2: Party Effect (float bpmDelay), (byte animMode)
     */
    switch(effectId)
    {
        /* Color Correction */
        case 0:
        {
            if(msgSize == sizeof(float))
            {
                effectArgs.NumArgs++;
                byte RBf[4];
                Serial.readBytes(RBf, sizeof(float));

                // Populates buffer with RBf value
                argBuffer.PutBytes(RBf, sizeof(float));
            }
            else if(msgSize == sizeof(char))
            {
                effectArgs.NumArgs++;
                char c = (char)Serial.read();

                // Populate buffer with calibration symbol
                argBuffer.PutChar(c);
            }
        }
        break;

        /* Intensity Gradient */
        case 1:
        {
            // Expecting at most 3 bytes for this effect
            const size_t argSize = 3*sizeof(byte);
            
            if(msgSize > 0 && msgSize <= argSize)
            {
                for(int i = 0; i < msgSize; i++)
                {
                    effectArgs.NumArgs++;
                    byte arg = Serial.read();
                    argBuffer.PutByte(arg);
                }
            }
        }
        break;

        /* Party Effect */
        case 2:
        {
            // Expecting either a float or a byte
            if(msgSize == sizeof(float))
            {
                effectArgs.NumArgs++;
                byte bpmDelay[4];
                Serial.readBytes(bpmDelay, sizeof(float));

                // Populates buffer with BPM delay value
                argBuffer.PutBytes(bpmDelay, sizeof(float));
            }
            else if(msgSize == sizeof(byte))
            {
                effectArgs.NumArgs++;
                byte animMode = Serial.read();

                // Populate buffer with animation mode "enum"
                argBuffer.PutByte(animMode);
            }
        }
        break;

        default: return argBuffer;
    }

    effectArgs.ArgBuffer = argBuffer;
    return argBuffer;
}
#else

ByteBuffer ParseEffectArgs(byte effectId, size_t msgSize, EffectArgs& effectArgs)
{
    LOG("PARSE"); LOGN(effectId);
    ByteBuffer argBuffer = ByteBuffer::Allocate(msgSize);

    /*
     *  Effect Ids are defined by our application. They are maintained in the EffectRegistry
     *  We could optionally handle this parsing in the Effect Registry, but that would be overkill.
     *  
     *  ID 0: Color Correct Effect (float RBf), (char calibrateSymbol)
     *  ID 1: Intensity Gradient Effect (byte intensityA, byte intensityB, byte dir)
     *  ID 2: Party Effect (float bpmDelay), (byte animMode)
     */
    switch(effectId)
    {
        /* Color Correction */
        case 0:
        {
            char arg = (char)Serial.read();
            effectArgs.NumArgs++;
            argBuffer.PutChar(arg);
        }
        break;

        /* BPM */
        case 'p':
        {
            LOGN("BPM");
            byte arg = Serial.read();
            effectArgs.NumArgs++;
            argBuffer.PutByte(arg);
        }
        break;
    }

    effectArgs.ArgBuffer = argBuffer;
    return argBuffer;
}
#endif

void abortReadTransmission()
{
    endReadTransmission();
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
