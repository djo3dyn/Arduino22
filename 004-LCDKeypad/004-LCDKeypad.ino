#include "common.h"
#include "displaymenu.h"

// Millis
unsigned long updateInterval = 100 ;
unsigned long savedMillis ;

void setup()
{
    Serial.begin(9600);
    dispInit();
    displayWeigth();
}

void loop()
{
    keypadHandle();
    if ((millis() - savedMillis) > updateInterval)
    {        
        updateDisplay();
        savedMillis = millis();
    }
}

