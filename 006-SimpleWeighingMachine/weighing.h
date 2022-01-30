#ifndef WEIGHING_H
#define WEIGHING_H

#include "Arduino.h"
#include "HX711.h"
#include "motorvalve.h"

#include "EEPROM.h"

// Define Motor
#define M_CW A1
#define M_CCW A0
#define ENC_A 2
#define ENC_B 3

// Load cell
#define SCK_PIN 4
#define DOUT_PIN 13

// EEPROM Address
#define SCALE_ADDR 0 // Float 4 bytes
#define OFFSET_ADDR 4 // Long 8 bytes
#define RAWOFFSET_ADDR 12 // Long 8 bytes
#define TARGET_ADDR 20 // uint 4 bytes 
#define OPPOS_ADDR 24 // int 4 bytes

#define READ_EEPROM 1
#define SIMULATION 0
#define DEBUG_MODE 1

#define UP_TOL 2.F
#define POS_TOL 10

// Init
void WeighingInit();

// Weigth function
long getRaw();
long getOffset();
float getWeigth();
void setScale(float scl);
float getScale();
void setTare();
void setTargetWeigth(int target);
bool isReachTarget();

// Motor function
void startMotor(bool ccw);
void stopMotor();
void goToPos(int pos);
void setZeroPos();
void setOpenPos();
void startAuto();
void openValve();
void closeValve();
bool motorHandle();
int getCurrentPos();

#endif