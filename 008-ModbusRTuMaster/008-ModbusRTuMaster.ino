#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// instantiate ModbusMaster object
ModbusMaster node;

SoftwareSerial MBSerial(12, 13); // RX, TX
SoftwareSerial HMISerial(19, 18); // RX, TX

uint16_t data[6];

void setup()
{
  // use Serial (port 0); initialize Modbus communication baud rate
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
  delay(250);
}

void updateDisplay()
{
    // Discard Buffer
    HMISerial.print("\xFF\xFF\xFF");

    // Heart beat
    int hearbeat = random(1000 , 9999);
    HMISerial.print("hb.val=");
    HMISerial.print(hearbeat);
    HMISerial.print("\xFF\xFF\xFF");
    HMISerial.print("tm0.en=0");
    HMISerial.print("\xFF\xFF\xFF");
    HMISerial.print("tm0.en=1");
    HMISerial.print("\xFF\xFF\xFF");

    // Print value 1
    HMISerial.print("t0.txt=\"");
    if(data[0] < 65000) HMISerial.print(data[0]);
    else HMISerial.print("***");
    HMISerial.print(" \"");
    HMISerial.print("\xFF\xFF\xFF");

    // Print value 2
    HMISerial.print("t1.txt=\"");
    if(data[1] < 65000) HMISerial.print(data[1]);
    else HMISerial.print("***");
    HMISerial.print(" \"");
    HMISerial.print("\xFF\xFF\xFF");

    // Print value 3
    HMISerial.print("t2.txt=\"");
    if(data[2] < 65000) HMISerial.print(data[2]);
    else HMISerial.print("***");
    HMISerial.print(" \"");
    HMISerial.print("\xFF\xFF\xFF");

}