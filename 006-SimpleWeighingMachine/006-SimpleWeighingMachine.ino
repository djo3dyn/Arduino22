#include "displaymenu.h"

// Define
#define MAN_PIN A2
#define AUT_PIN A3

// Millis
unsigned long updateInterval = 100 ;
unsigned long savedMillis ;
bool autoReq = false;
void setup()
{
    Serial.begin(9600);
    WeighingInit();
    displayInit();

    pinMode(AUT_PIN , INPUT_PULLUP);
    pinMode(MAN_PIN , INPUT_PULLUP);

}

void loop()
{
    // Main button operation
    if (getMode()== M_IDLE)
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
    // Keypad operation
    keypadHandle();
    // Motor handle
    if (autoReq && motorHandle()) autoReq = false;
    // Periodically update display
    if ((millis() - savedMillis) > updateInterval)
    {        
        updateDisplay();
        savedMillis = millis();
    }
}

