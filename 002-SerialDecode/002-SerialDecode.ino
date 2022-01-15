/*
    - Arduino Serial Decode
    - Use DHT21 Sensor
    -
*/
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

#define BYTE_DELAY 15000 // Inter byte delay in microseconds
#define BYTE_COUNT 32

// Frame variable
const uint8_t frameSOF[3] = {0xff , 0xff , 0xff};
uint8_t dataFrame[BYTE_COUNT];

LiquidCrystal_I2C lcd(0x27,16,2);;
void setup()
{
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Program Ready...");
    
}

void loop()
{
    // Decode method
    // Serial Communication Handler
    if (Serial.available()>0) {
        int i = 0;
        while (Serial.available()){
            if (i <= BYTE_COUNT)
            {
                dataFrame[i] = Serial.read();
                i++; // total bytes
                delayMicroseconds(BYTE_DELAY);  
            }
            else
            {
                Serial.read(); // read from buffer and dischard
                delayMicroseconds(BYTE_DELAY);
            }
        }
        decodeFrame(dataFrame , i);  
        clearDataFrame();
    }
}

void decodeFrame(uint8_t frame[] , int count)
{
    // Filter if incoming byte bellow 5 bytes
    lcd.clear();
    for (int i = 0 ; i < count ; i++)
    {
        lcd.print(frame[i] , HEX);
    }
    if (count < 5) return ;
    // Start Decoding
    for (int i = 0 ; i < count ; i++)
    {
        // Detect SOF
        if (frame[i] == frameSOF[0] && frame[i+1] == frameSOF[1] && frame[i+2] == frameSOF[2])
        {
            uint8_t fCode = frame[i+3];
            uint8_t fData[4] = { frame[i+4] , frame[i+5] , frame[i+6] ,frame[i+7] } ;
            // Decode function Code (fCode)
            switch (fCode)
            {
            case 0: // Handshaking code
                
                break;
            case 1: // Pin Mode
                pinMode(fData[0] , fData[1]);
                
                i += 4 ;
                break;
            case 2: // Digital Write
                digitalWrite(fData[0] , fData[1]);
                i += 4 ;
                break;
            case 3: // Digital Read
                
                break;
            case 4: // Analog Write
                analogWrite(fData[0] , fData[1]);
                break;
            case 5: // Analog Write
                
                break;
            
            default:
                break;
            }
            
            
        }
        
    }
}

void clearDataFrame()
{
    for (int i = 0 ; i < BYTE_COUNT ; i++)
    {
        dataFrame[i] = 0;
    }

}