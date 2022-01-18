#ifndef WEIGHING_H
#define WEIGHING_H

#include "Arduino.h"
#include "HX711.h"
#include "motorvalve.h"

// Define Motor
#define M_CW A1
#define M_CCW A0
#define ENC_A 2
#define ENC_B 3

// Load cell
#define SCK_PIN 4
#define DOUT_PIN 13

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

#endif