#include "displaymenu.h"

// Define
#define MAN_PIN A2
#define AUT_PIN A3

// Millis
const unsigned long updateInterval = 250 ;
unsigned long savedMillis ;
bool autoReq = false;

// millis check error
const unsigned long checkInterval = 1000;
unsigned long triggerMillis;
bool trigerred = false;

void setup()
{
    Serial.begin(9600);
    WeighingInit();
    displayInit();

    pinMode(AUT_PIN , INPUT_PULLUP);
    pinMode(MAN_PIN , INPUT_PULLUP);

    closeValve();

}

void loop()
{
    // Main button operation
    if (getMode()== M_IDLE )
    {
        if (getErrorCode() == 0)
        {
            if (!digitalRead(AUT_PIN) && !isReachTarget())
            {
                startAuto();
                autoReq = true;
            }
            if (!digitalRead(MAN_PIN))
            {
                openValve();
                autoReq = false;
            }
            else if (digitalRead(MAN_PIN) && !autoReq) closeValve();
        }   
        
    }
    // Keypad operation
    keypadHandle();
    // Motor handle
    motorHandle();
    // Check target
    if (autoReq && isReachTarget()) autoReq = false;
    // Periodically update display
    if ((millis() - savedMillis) > updateInterval)
    {        
        updateDisplay();
        savedMillis = millis();
    }

    // Periodically Check Error
    if (motorIsRun())
    {
        if (!trigerred)
        {
            trigerred = true;
            triggerMillis = millis();
        }
        if ((millis() - triggerMillis ) > checkInterval)
        {
            updateErrorCode();
            triggerMillis = millis();
#if DEBUG_MODE
            Serial.print("Trigger Run : ");
            Serial.println(millis());
#endif

        }
    }
    else trigerred = false;
    
}

