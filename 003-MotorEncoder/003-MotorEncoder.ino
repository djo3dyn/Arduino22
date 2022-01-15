#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include "MotorValve.h"

// Define pin
#define ENC_A 2
#define ENC_B 3
#define CW_PIN 4
#define CCW_PIN 5
#define MAN_PIN 6
#define AUT_PIN 7


// Milis
unsigned long updateInterval = 100 ;
unsigned long savedMillis ;

LiquidCrystal_I2C lcd(0x27,16,2);
MotorValve motor(CW_PIN , CCW_PIN , ENC_A , ENC_B);
void setup()
{
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Position : ");

    pinMode(MAN_PIN , INPUT_PULLUP);
    pinMode(AUT_PIN , INPUT_PULLUP);

    // pin initialize
    motor.init();
    motor.setClosePosition(0);
    motor.setOpenPosition(50);
    
}

void loop()
{
    if (!digitalRead(MAN_PIN)) motor.start(false);
    else if (!digitalRead(AUT_PIN)) motor.start(true);
    else motor.stop();
    
    if ((millis() - savedMillis) > updateInterval)
    {        
        updateDisplay();
        savedMillis = millis();
    }


}
void updateDisplay()
{
    lcd.setCursor(0,1);
    lcd.print("     ");
    lcd.setCursor(0,1);
    lcd.print(motor.currentPosition);
}