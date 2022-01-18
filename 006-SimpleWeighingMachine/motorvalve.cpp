#include "arduino.h"
#include "MotorValve.h"

MotorValve::MotorValve(int cw_pin , int ccw_pin , int enca_pin , int encb_pin)
{
    cwPin = cw_pin;
    ccwPin = ccw_pin;
    encaPin = encaPin;
    encbPin = encb_pin;
}

// Instance pointer
MotorValve * MotorValve::instance0;

void MotorValve::init()
{
    // pin initialize
    pinMode(cwPin , OUTPUT);
    pinMode(ccwPin , OUTPUT);
    pinMode(encaPin , INPUT);
    pinMode(encbPin , INPUT);
    
    attachInterrupt(encaPin , ISR_readEncoder , RISING);
    instance0 = this;
}


void MotorValve::setClosePosition(int pos)
{
    openPosition = pos;
}

void MotorValve::setOpenPosition(int pos)
{
    closedPosition = pos;
}

void MotorValve::start(bool ccw)
{
    if (ccw)
    {
        digitalWrite(ccwPin , HIGH);
        digitalWrite(cwPin , LOW);
    }
    else
    {
        digitalWrite(ccwPin , LOW);
        digitalWrite(cwPin , HIGH);
    }

}

void MotorValve::stop()
{
    digitalWrite(cwPin , LOW);
    digitalWrite(ccwPin , LOW);
    posRequested = false;

}

void MotorValve::goToPosition(int pos)
{
    targetPosition = pos ;
    if (targetPosition < currentPosition) start(true);
    else if (targetPosition > currentPosition) start(false); 
    posRequested = true;

}

void MotorValve::open()
{
    goToPosition(openPosition);
}

void MotorValve::close()
{
    goToPosition(closedPosition);
}

void MotorValve::setZeroPosition()
{
    currentPosition = 0;
}

void MotorValve::ISR_readEncoder()
{
    instance0->handleInterrupt();
}

// Read encoder
void MotorValve::handleInterrupt()
{
    if (digitalRead(encbPin)) 
    {
        currentPosition--;
        if (posRequested && (currentPosition <= targetPosition)) stop();
    }
    else 
    {
        currentPosition++;
        if (posRequested && (currentPosition >= targetPosition)) stop();
    }
}