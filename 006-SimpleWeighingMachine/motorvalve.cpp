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
    isRun = false;
    errorCode = 0;
    motorStatus = S_STOP;
    prevPosition = 0;
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
    if (errorCode > 0)
    {
        isRun = false;
        motorStatus = S_STOP;
        return;
    }
    if (ccw)
    {
        digitalWrite(ccwPin , HIGH);
        digitalWrite(cwPin , LOW);
        motorStatus = S_CCW;
    }
    else
    {
        digitalWrite(ccwPin , LOW);
        digitalWrite(cwPin , HIGH);
        motorStatus = S_CW;
    }
    isRun = true;
}

void MotorValve::stop()
{
    digitalWrite(cwPin , LOW);
    digitalWrite(ccwPin , LOW);
    posRequested = false;
    isRun = false;
    motorStatus = S_STOP;

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

int MotorValve::getStatus()
{
    if (errorCode == 0 && motorStatus == S_CW)
    {
        if (currentPosition < prevPosition) errorCode = 2;
        else if (currentPosition == prevPosition) errorCode = 1;
        else errorCode = 0;
        prevPosition = currentPosition;
    }
    else if (errorCode == 0 && motorStatus == S_CCW)
    {
        if (currentPosition > prevPosition) errorCode = 2;
        else if (currentPosition == prevPosition) errorCode = 1;
        else errorCode = 0;
        prevPosition = currentPosition;
    }

    if (errorCode > 0) stop();
    return errorCode;
}

void MotorValve::resetError()
{
    errorCode = 0;
}