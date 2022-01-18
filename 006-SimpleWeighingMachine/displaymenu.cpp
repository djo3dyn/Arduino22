#include "displaymenu.h"

// Weigthing
float currentWeigth ;
float savedWeigth ;
int setWeigth ;
float divider ;
long rawOffset;

// Menu
int menuMode ;

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

void displayWeigth()
{
     
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print("Berat (gr) :       ");
    lcd.setCursor(0,1);
    lcd.print("               ");
    lcd.setCursor(0,1);
    lcd.print(currentWeigth , 0);
    savedWeigth = currentWeigth;
}

void keypadEvent(KeypadEvent key)
{
    KeyState state = keypad.getState();
    if (menuMode == M_ZERO || menuMode == M_FULL)
    {
        switch (state)
        {
            case PRESSED:
                if (key == '#' ) startMotor(false);
                else if (key == '*') startMotor(true);
                break;
            case RELEASED:
                if (key == '#' || key == '*' )stopMotor();
                break;
        }

    }
}

void displayInit()
{
    lcd.begin();
    lcd.backlight();

    // Weigthing
    currentWeigth = 0 ;
    savedWeigth = 0 ;
    setWeigth = 0 ;
    menuMode = M_IDLE;

    displayWeigth();
    keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
}

int getMode() { return menuMode;}

void updateDisplay()
{
    currentWeigth = getWeigth();
    //Serial.println(getRaw());
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
                setTare();
                rawOffset = getOffset() * getScale();
            }
            else if (key == 'C')
            {
                menuMode = M_CAL;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Kalibrasi (gr) : ");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print(currentWeigth , 0);
                lcd.blink();
                strcpy(lcdLineTwo , "");
                itoa(currentWeigth , lcdLineTwo , 10);   
            }
            else if (key == 'D')
            {
                menuMode = M_ZERO;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Set Pos. tutup :");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print("Tekan (# , *)");
                closeValve();
            }
            // test
            else if (key == '1')
            {
                goToPos(100);
            }
            
        }
        break;
        // Input weigth menu
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
                    setTargetWeigth(setWeigth);
                    // Go to Idle
                    menuMode = M_IDLE;
                    displayWeigth();
                    
                }     
            }
        }
        break;
        case M_CAL: // Calibration menu
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
                    divider = (getRaw() - rawOffset) / (float)temp;
                    setScale(divider);
                    // Go to Idle
                    menuMode = M_IDLE;
                    displayWeigth(); 
                }     
            }
        }
        break;
        case M_ZERO: // Calibration menu
        {
            if (key == 'A') // Cancel
            {    
                menuMode = M_IDLE;   
                displayWeigth();         
            }
            else if (key == 'D') 
            {
                setZeroPos();

                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Set Pos. buka : ");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print("Tekan (# , *)");
                openValve();
                menuMode = M_FULL;
            }
        }
        break;
        case M_FULL: // Calibration menu
        {
            if (key == 'A') // Cancel
            {    
                menuMode = M_IDLE;   
                displayWeigth();         
            }
            else if (key == 'D') 
            {
                setOpenPos();
                closeValve();
                displayWeigth(); 
                menuMode = M_IDLE;
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
