#ifndef MOTORVALVE_H
#define MOTORVALVE_H

#include "arduino.h"

#define S_STOP 0
#define S_CW 1
#define S_CCW 2

class MotorValve
{
    public :
        MotorValve(int cw_pin , int ccw_pin , int enca_pin , int encb_pin );
        void init();
        void setOpenPosition(int openPos);
        void setClosePosition(int closePos);
        void setZeroPosition();
        void start(bool ccw);
        void goToPosition(int pos);
        void open();
        void close();
        void stop();

        volatile int currentPosition ;
        bool isRun;
        int getStatus();
        void resetError();
    
    private :
        // Interrupt Handler variable
        static void ISR_readEncoder();
        void handleInterrupt();
        static MotorValve *instance0 ;

        // Motor pin
        int cwPin, ccwPin, encbPin, encaPin;
        
        // Motor Position
        
        int targetPosition ;
        int prevPosition;

        // Set point value
        bool posRequested ;
        bool autoPos ;
        int openPosition ;
        int closedPosition;

        // Motor Status
        int motorStatus;
        int errorCode;
};

#endif