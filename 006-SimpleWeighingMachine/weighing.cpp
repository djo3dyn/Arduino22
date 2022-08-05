#include "weighing.h"

// Init Weighing
HX711 scale;

// Motor
MotorValve motor(M_CW , M_CCW , ENC_A , ENC_B);
int openPos;
int closePos = 0;
int savedPos ;
unsigned int targetWeigth;
bool autoMode = false;
byte errorCode = 0;



// Weigthing function
void WeighingInit()
{
    scale.begin(DOUT_PIN , SCK_PIN);
    motor.init();
    
#if READ_EEPROM
    // EEPROM Init scale :
    float scaleDiv ;
    EEPROM.get(SCALE_ADDR , scaleDiv);
    setScale(scaleDiv);

    // EEPROM Init offset :
    long offset ;
    EEPROM.get(OFFSET_ADDR , offset);
    scale.set_offset(offset);

    //EEPROM Init targetWeigth
    EEPROM.get(TARGET_ADDR , targetWeigth);

    // EEPROM Init Open pos
    int oposRead;
    EEPROM.get(OPPOS_ADDR , oposRead);
    openPos = oposRead;
#endif
    
}

long getRaw() 
{ 
    return scale.read();
}

long getOffset()
{
    return scale.get_offset();
}

float getScale()
{
    return scale.get_scale();
}

void setScale(float scl)
{ 
    scale.set_scale(scl);

    // EEPROM Operation :
    EEPROM.put(SCALE_ADDR , scl);
}

float getWeigth()
{ 
    return scale.get_units();
}

void setTare()
{
    scale.set_offset(scale.read());
    
    // EEPROM Operation :
    EEPROM.put(OFFSET_ADDR , scale.get_offset());

}
void setTargetWeigth(int target)
{
    targetWeigth = target;
}

// Motor function
void startMotor(bool ccw)
{
    motor.start(ccw);
    //triggerMillis = millis();
#if DEBUG_MODE
    int pos = motor.currentPosition;
    Serial.println(pos);
#endif
}

void stopMotor()
{
    motor.stop();
    autoMode = false;
}

void setZeroPos()
{
    motor.setZeroPosition();
    motor.setClosePosition(0);
}

void setOpenPos()
{
    openPos = motor.currentPosition;
    motor.setOpenPosition(motor.currentPosition);

    // EEPROM Operation :
    EEPROM.put(OPPOS_ADDR , openPos);
}

bool isClosed()
{
    if (motor.currentPosition <= 0 ) return true;
    else return false;
}

bool isOpen()
{
    if (motor.currentPosition >= openPos  ) return true;
    else return false;
}

void openValve()
{
    if (!isOpen()) motor.goToPosition(openPos);
}

void closeValve()
{
    
    if(!isClosed()) motor.goToPosition(0);
}

void goToPos(int pos)
{
    motor.goToPosition(pos);
}

void startAuto()
{
    openValve();
    autoMode = true;

}

bool isReachTarget()
{
    if (getWeigth() >= targetWeigth) return true;
    else return false;
}

bool motorIsRun()
{
    return motor.isRun;
}

int getCurrentPos()
{
    
}

void updateErrorCode()
{
    errorCode = motor.getStatus();
}

int getErrorCode()
{
    return errorCode;
}

void resetError()
{
    motor.resetError();
}

void motorHandle()
{
    //check motor run
    if (!motor.isRun) return;

    // Check Target
    if (autoMode && isReachTarget())
    {
        closeValve();
        autoMode = false;      
    }

    
}
    
