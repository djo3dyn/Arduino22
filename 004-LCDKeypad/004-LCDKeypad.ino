#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include "Keypad.h"

// Define menu mode
#define IDLE_MENU 0
#define INPUT_MENU 1
#define ZERO_MENU 2
#define CAL_MENU 3

// LCD Char
char lcdLineOne[16];
char lcdLineTwo[16];

// Millis
unsigned long updateInterval = 100 ;
unsigned long savedMillis ;

// Weigthing
int currentWeigth = 156 ;
int savedWeigth = 0 ;
int setWeigth = 156 ;
int tempInputNum = 0;

// Keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {   
    {'7','8','9','A'},
    {'4','5','6','B'},
    {'1','2','3','C'},
    {'*','0','#','D'}    
};

byte rowPins[ROWS] = {12, 11, 10, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6, 5}; //connect to the column pinouts of the keypad

// Menus...
byte menuMode = IDLE_MENU;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd(0x27,16,2);

void setup()
{
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
    lcd.print("Berat (gram) : ");
    lcd.setCursor(0,1);
    lcd.print(currentWeigth);

    keypad.addEventListener(keypadEvent); // Add an event listener for this keypad

}

void loop()
{
    char key = keypad.getKey();
    keyDecode(key);
    if ((millis() - savedMillis) > updateInterval)
    {        
        updateDisplay();
        savedMillis = millis();
    }

}

void updateDisplay()
{
    if (menuMode != IDLE_MENU) return;
    if (currentWeigth != savedWeigth) updateWeigth() ;
    
}

void updateWeigth()
{
    lcd.setCursor(0,0);
    lcd.print("Berat (gr) :       ");
    lcd.setCursor(0,1);
    lcd.print(currentWeigth);
    lcd.print("               ");
    lcd.setCursor(0,1);
    lcd.print(currentWeigth);
    savedWeigth = currentWeigth;
}

void keyDecode(char key)
{
    switch (menuMode)
    {
        case IDLE_MENU:
        {
            // Menu select
            if (key == 'A') {
                menuMode = INPUT_MENU;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Set Berat (gr) : ");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print(setWeigth);
                lcd.blink();
                strcpy(lcdLineTwo , "\n");
                itoa(setWeigth , lcdLineTwo , 10);
            }
            else if (key == 'B')
            {
                // Set Tare
                currentWeigth = 0;

            }
            else if (key == 'C')
            {
                // Adjust 0


            }
            else if (key == 'D')
            {

            }
        }
        break;
        case INPUT_MENU:
        {
            if (key >= 48 && key <= 57) {
                lcd.print(key);
                // Append number
                char buff[1] = {key};
                strcat(lcdLineTwo , buff);    
            }
            else if (key == 'C')
            {
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                strcpy(lcdLineTwo , "\n");
            }
            else if (key == 'D')
            {
                // Parse int
                int temp = atoi(lcdLineTwo);
                if (temp <= 20000 && temp > 0)
                {
                    setWeigth = temp;
                    // Go to Idle
                    menuMode = IDLE_MENU;
                    lcd.noBlink();
                    updateWeigth();
                }     
            }
        }
        break;
    }
    
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        if (key == '#') {
            
        }
        break;

    case RELEASED:
        if (key == '*') {
            
        }
        break;

    case HOLD:
        if (key == '*') {
            
        }
        break;
    }
}

