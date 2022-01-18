#include "displaymenu.h"

// Weigthing
int currentWeigth ;
int savedWeigth ;
int setWeigth ;
byte menuMode ;

// LCD Char
char lcdLineOne[16];
char lcdLineTwo[16];

// Keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {   
    {'7','8','9','A'},
    {'4','5','6','B'},
    {'1','2','3','C'},
    {'*','0','#','D'}    
};

byte rowPins[ROWS] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN}; //connect to the column pinouts of the keypad

// Menus...
LiquidCrystal_I2C lcd(0x27,16,2);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void dispInit()
{
    lcd.begin();
    lcd.backlight();

    // Weigthing
    currentWeigth = 0 ;
    savedWeigth = 0 ;
    setWeigth = 0 ;
    menuMode = M_IDLE;

}
void displayWeigth()
{
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print("Berat (gr) :       ");
    lcd.setCursor(0,1);
    lcd.print(currentWeigth);
    lcd.print("               ");
    lcd.setCursor(0,1);
    lcd.print(currentWeigth);
    savedWeigth = currentWeigth;
}

void updateDisplay()
{
    if (menuMode != M_IDLE) return;
    if (currentWeigth != savedWeigth) displayWeigth() ;
    
}

void keyDecode(char key)
{
    switch (menuMode)
    {
        case M_IDLE:
        {
            // Menu select
            if (key == 'A') {
                menuMode = M_INPUT;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Set Berat (gr) : ");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print(setWeigth);
                lcd.blink();
                strcpy(lcdLineTwo , "");
                itoa(setWeigth , lcdLineTwo , 10);
            }
            else if (key == 'B')
            {
                // Set Tare
                currentWeigth = 0;
            }
            else if (key == 'C')
            {

            }
            else if (key == 'D')
            {
                menuMode = M_CAL;
                
            }
        }
        break;
        case M_INPUT:
        {
            if (key >= 48 && key <= 57) 
            {
                lcd.print(key);
                // Append number
                char buff[1] = {key};
                strcat(lcdLineTwo , buff);    
            }
            else if (key == 'A') // Cancel
            {    
                menuMode = M_IDLE;   
                displayWeigth();         
            }
            else if (key == 'C') // Clear
            {
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                strcpy(lcdLineTwo , "");
            }
            else if (key == 'D') // OK
            {
                // Parse int
                int temp = atoi(lcdLineTwo);
                if (temp <= 20000 && temp > 0)
                {
                    setWeigth = temp;
                    // Go to Idle
                    menuMode = M_IDLE;
                    displayWeigth();
                    
                }     
            }
        }
        break;
    }
    
}

void keypadHandle()
{
    char key = keypad.getKey();
    keyDecode(key);
}
