#ifndef DISPLAYMENU_H
#define DISPLAYMENU_H

#include "arduino.h"
#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include "Keypad.h"
#include "weighing.h"


// Define menu mode
#define M_IDLE 0
#define M_INPUT 1
#define M_ZERO 2
#define M_FULL 3
#define M_CAL 4
#define M_CAL_TARE 5

// Define keypad
#define R1_PIN 12
#define R2_PIN 11
#define R3_PIN 10
#define R4_PIN 9
#define C1_PIN 8
#define C2_PIN 7
#define C3_PIN 6
#define C4_PIN 5

// Functions
void displayInit();
void updateDisplay();
void keypadHandle();
int getMode();

#endif