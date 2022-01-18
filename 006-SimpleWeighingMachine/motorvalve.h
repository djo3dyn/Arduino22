#ifndef MOTORVALVE_H
#define MOTORVALVE_H

#include "arduino.h"

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
    
    private :
        // Interrupt Handler variable
        static void ISR_readEncoder();
        void handleInterrupt();
        static MotorValve *instance0 ;

        // Motor pin
        int cwPin, ccwPin, encbPin, encaPin;
        
        // Motor Position
        int targetPosition ;

        // Set point value
        bool posRequested ;
        bool autoPos ;
        int openPosition ;
        int closedPosition;
};

#endif