/*
    - Arduino Humidity Controller with HMI Hextion
    - Use DHT21 Sensor
    -
*/
#include <TimerOne.h>
#include "DHT.h"
#include "EEPROM.h"

// Define Preprocessing
#define DHT_PIN 2
#define R1_PIN 14
#define R2_PIN 15
#define R3_PIN 16
#define R4_PIN 17

#define INPUT_PIN 3

#define OP_ON 1
#define OP_OFF 2
#define OP_AUTO 3

// Frame variable
byte dataFrame[32];
const int byteLatency = 15000 ;// --- microseconds

// DHT Variable
DHT dht(DHT_PIN , DHT21);
volatile float temperatureValue;
volatile float humidityValue;

// Operation status
bool opStat;
bool heatingStat;
int opMode = OP_AUTO;
int setHumidity;
int setTemperature;
int humHisterisis ;


void setup()
{
    // Init serial
    Serial.begin(9600);

    // Init DHT 
    dht.begin();

    // Init I/O 
    pinMode(R1_PIN , OUTPUT);
    pinMode(R2_PIN , OUTPUT);
    pinMode(R3_PIN , OUTPUT);
    pinMode(R4_PIN , OUTPUT);
    pinMode(INPUT_PIN , INPUT_PULLUP);

    // Read EEPROM
    readEEPROM();

    // Init timer to update display
    Timer1.initialize(500000); 
    Timer1.attachInterrupt(updateDisplay);
    
}

void loop()
{
    // Serial Communication Handler
    if (Serial.available()>0)
    {
        int i = 0;
        while (Serial.available())
        {
            dataFrame[i] = Serial.read();
            i++;
            delayMicroseconds(byteLatency);
        }
        decodeFrame(dataFrame);  
    }

    // Operation Hanlder
    if(digitalRead(INPUT_PIN) == LOW) opStat = true;
    else opStat = false;
    if (opStat)
    {
        switch (opMode)
        {
        case OP_ON:
            setHeater(true);
            break;
        case OP_OFF:
            setHeater(false);
            break;
        case OP_AUTO :
            OpAutoHandler();
            break; 
        default:
            break;
        }

    }
    else setHeater(false);
     
}

void OpAutoHandler()
{
    if (temperatureValue < setTemperature )
    {
        if (humidityValue < ( setHumidity - humHisterisis)) setHeater(false);
        else if (humidityValue > setHumidity ) setHeater(true);
    }
    else
    {
        setHeater(false);
    }

}

void setHeater(bool stat)
{
   
    if (stat)
    {
        digitalWrite(R1_PIN , 0);
        digitalWrite(R2_PIN , 0);
        heatingStat = true;
    }
    else
    {
        digitalWrite(R1_PIN , 1);
        digitalWrite(R2_PIN , 1);
        heatingStat = false; 
    }
    
}

void updateDisplay()
{
    readDHT();
    // Discard Buffer
    Serial.print("\xFF\xFF\xFF");

    // Print humidity set command
    Serial.print("hum.txt=\"");
    if (!isnan(humidityValue)) Serial.print(humidityValue);
    else Serial.print("**");
    Serial.print(" %\"");
    Serial.print("\xFF\xFF\xFF");

    // Print temp command
    Serial.print("temp.txt=\"");
    if (!isnan(temperatureValue)) Serial.print(temperatureValue);
    else Serial.print("**");
    Serial.print(" \xB0""C\"");
    Serial.print("\xFF\xFF\xFF");

    // Print operation status
    Serial.print("opstat.txt=\"");
    if (opStat) Serial.print("ON\"");
    else Serial.print("OFF\"");
    Serial.print("\xFF\xFF\xFF");

    // Print heating status
    Serial.print("heatstat.txt=\"");
    if (heatingStat) Serial.print("ON\"");
    else Serial.print("OFF\"");
    Serial.print("\xFF\xFF\xFF");
}

void readDHT()
{
    temperatureValue = dht.readTemperature();
    humidityValue = dht.readHumidity();
}

void decodeFrame(byte frame[])
{
    for (int i = 0 ; i < 31 ; i++)
    {
        int fCode = frame[i+2];
        int fValue = frame[i+3];
        if (frame[i]==0xfe && frame[i+1]==0xfe)
        {
              
            if (fCode== 1 && fValue > 0 && fValue < 4) opMode = fValue;
            else if (fCode == 2)
            {
                setHumidity = fValue;
                writeEEPROM();

            }
            else if (fCode == 3)
            {
                setTemperature = fValue;
                writeEEPROM();
                
            }
            else if (fCode == 4)
            {
                humHisterisis = fValue;
                writeEEPROM();
                
            }
            else if (fCode == 0 && fValue == 1)
            {
                sendSetVal();
            }
            
            
        }
        else
        {
            
        }
    }
    clearDataFrame();
}

void clearDataFrame()
{
    for (int i = 0 ; i < 32 ; i++)
    {
        dataFrame[i] = 0;
    }

}

void sendSetVal()
{
    readEEPROM();
    // Discard Buffer
    Serial.print("\xFF\xFF\xFF");

    // Print humidity set command
    Serial.print("hum_set.val=");
    Serial.print(setHumidity);
    Serial.print("\xFF\xFF\xFF");
    // Print temperature set command
    Serial.print("temp_set.val=");
    Serial.print(setTemperature);
    Serial.print("\xFF\xFF\xFF");
    // Print histerisis set command
    Serial.print("his_set.val=");
    Serial.print(humHisterisis);
    Serial.print("\xFF\xFF\xFF");
}

void readEEPROM()
{
    setHumidity = EEPROM.read(0);
    setTemperature = EEPROM.read(2);
    humHisterisis = EEPROM.read(4);
    
}

void writeEEPROM()
{
    EEPROM.write(0,setHumidity);
    EEPROM.write(2,setTemperature);
    EEPROM.write(4,humHisterisis);
}