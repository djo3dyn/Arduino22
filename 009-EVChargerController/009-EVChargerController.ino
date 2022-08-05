#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#define OUT 6

// instantiate ModbusMaster object
ModbusMaster node;

SoftwareSerial MBSerial(12, 13); // RX, TX
SoftwareSerial HMISerial(19, 18); // RX, TX

// RAW Data
uint16_t data[6];

// Value data
int16_t volts , amps , stats , faults;

void setup()
{
    pinMode(OUT , OUTPUT);
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);
  HMISerial.begin(9600);
  MBSerial.begin(9600);

  // communicate with Modbus slave ID 1 over Serial (port 0)
  node.begin(1, MBSerial);

}


void loop()
{

  uint8_t result;
  // slave: read (6) 16-bit registers starting at register 2 to RX buffer
  result = node.readHoldingRegisters(0, 6);
  
  // do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    for (int j = 0; j < 5; j++)
    {
      data[j] = node.getResponseBuffer(j);
    }
  }
  else
  {
    for (int j = 0; j < 5; j++)
    {
      data[j] = 0xFFFF;
    }
    
  }
  updateDisplay();

  // Update out
  if (data[3] == 2) digitalWrite(OUT , HIGH);
  else digitalWrite(OUT , LOW);

  delay(250);
}

void updateDisplay()
{
    // Discard Buffer
    HMISerial.print("\xFF\xFF\xFF");
    /*
    // Heart beat
    int hearbeat = random(1000 , 9999);
    HMISerial.print("hb.val=");
    HMISerial.print(hearbeat);
    HMISerial.print("\xFF\xFF\xFF");
    HMISerial.print("tm0.en=0");
    HMISerial.print("\xFF\xFF\xFF");
    HMISerial.print("tm0.en=1");
    HMISerial.print("\xFF\xFF\xFF");
    */

    // Print volt
    HMISerial.print("volts.txt=\"");
    if(data[1] < 65000) HMISerial.print(data[1]);
    else HMISerial.print("***");
    HMISerial.print(" V\"");
    HMISerial.print("\xFF\xFF\xFF");

    // Print Amps
    HMISerial.print("amps.txt=\"");
    if(data[2] < 65000) HMISerial.print(data[2]);
    else HMISerial.print("***");
    HMISerial.print(" A\"");
    HMISerial.print("\xFF\xFF\xFF");

    // Print Status
    HMISerial.print("stats.txt=\"");
    switch (data[3])
    {
    case 0:
        HMISerial.print("Standby\"");
        break;
    case 1:
        HMISerial.print("Connected\"");
        break;
    case 2:
        HMISerial.print("Charging\"");
        break;
    default:
        HMISerial.print("N/A\"");
        break;
    }
    HMISerial.print("\xFF\xFF\xFF");

    // Print Faults
    HMISerial.print("faults.txt=\"");
    switch (data[4])
    {
    case 0:
        HMISerial.print("No Fault\"");
        break;
    case 1:
        HMISerial.print("Need Ventilation!\"");
        break;
    case 2:
        HMISerial.print("Socket Fault!\"");
        break;
    case 3:
        HMISerial.print("Circuit Overload!\"");
        break;
    default:
        HMISerial.print("N/A\"");
        break;
    }
    HMISerial.print("\xFF\xFF\xFF");

}