#include "displaymenu.h"

// Weigthing
float currentWeigth ;
float savedWeigth ;
unsigned int setWeigth ;
float divider ;
long rawOffset;

// Menu
int menuMode ;

// LCD Char
char lcdLineOne[16];
char lcdLineTwo[16];
int charPos = 0;
int savedLength;

// Keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

#if SIMULATION
char keys[ROWS][COLS] = {   
    {'7','8','9','A'},
    {'4','5','6','B'},
    {'1','2','3','C'},
    {'*','0','#','D'}    
};
#else
char keys[ROWS][COLS] = {   
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}    
};
#endif
byte rowPins[ROWS] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN}; //connect to the column pinouts of the keypad

// Menus...
LiquidCrystal_I2C lcd(0x27,16,2);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void displayWeigth()
{   
    char s[16];
    int i = round(currentWeigth);
    //itoa(i, s , 10);
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print("Berat (gr) :       ");
    lcd.setCursor(0,1);
    lcd.print(i);
    lcd.print("                ");
    //savedLength = strlen(s);
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

#ifdef READ_EEPROM
    //EEPROM Init targetWeigth
    EEPROM.get(TARGET_ADDR , setWeigth);
    EEPROM.get(RAWOFFSET_ADDR , rawOffset);
#endif

}

int getMode() { return menuMode;}

void inputChar(char c)
{
    lcdLineTwo[charPos] = c;
    charPos++;
}

void clearInput()
{
    memset(lcdLineTwo , 0 , 16);
    charPos = 0 ;  
}

void updateDisplay()
{
    currentWeigth = getWeigth();
    if (menuMode != M_IDLE) return;
    int current = round(currentWeigth);
    int saved = round(savedWeigth);
    if (current != saved ) displayWeigth() ;
    
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
                // Set input
                clearInput();
                itoa(setWeigth , lcdLineTwo , 10);
            }
            else if (key == 'B')
            {
                // Set Tare
                setTare();
            }
            else if (key == 'C')
            {
                menuMode = M_CAL_TARE;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Kalb. (0 gr) :  ");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("                ");
            }
            else if (key == 'D')
            {
                menuMode = M_ZERO;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Set Pos. tutup :");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print("Tekan (# , *)");
                closeValve();
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
                inputChar(key);

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
                clearInput();
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

                    //EEPROM Operation :
                    EEPROM.put(TARGET_ADDR , setWeigth);

                    clearInput();
                    
                }     
            }
        }
        break;
        case M_CAL_TARE: // Calibration menu tare
        {
            if (key == 'A') // Cancel
            {    
                menuMode = M_IDLE; 
                lcd.clear();  
                displayWeigth();         
            }
            else if (key == 'D') // OK
            {
                setScale(1.f);
                setTare();
                rawOffset = getOffset() * getScale();

                // go to cal unit
                menuMode = M_CAL;
                // Print header text
                lcd.setCursor(0,0);
                lcd.print("Kalb. (n gr) :  ");
                // Print input text
                lcd.setCursor(0,1);
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(currentWeigth , 0);
                lcd.blink();
                
                clearInput();
                itoa(currentWeigth , lcdLineTwo , 10);

            }
        }
        break;
        case M_CAL: // Calibration menu
        {
            if (key >= 48 && key <= 57) 
            {
                lcd.print(key);
                // Append number
                inputChar(key);    
            }
            else if (key == 'A') // Cancel
            {    
                menuMode = M_IDLE;
                lcd.clear(); 
                displayWeigth();         
            }
            else if (key == 'C') // Clear
            {
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                clearInput();
            }
            else if (key == 'D') // OK
            {
                // Parse int
                int temp = atoi(lcdLineTwo);
                if (temp <= 20000 && temp > 0)
                {
                    divider = (getRaw() - rawOffset) / (float) temp;
                    setScale(divider);
                    // Go to Idle
                    menuMode = M_IDLE;
                    displayWeigth();   
                   
#if DEBUG_MODE
                    // Debug
                    Serial.print("Raw Read : ");
                    Serial.println(getRaw());
                    Serial.print("Raw Offset : ");
                    Serial.println(rawOffset);
                    Serial.print("Raw Input : ");
                    Serial.println(lcdLineTwo);
                    Serial.print("Input : ");
                    Serial.println(temp);
                    Serial.print("Divider : ");
                    Serial.println(divider , 6);
#endif
                    clearInput();
                }     
            }
        }
        break;
        case M_ZERO: // Calibration menu
        {
            if (key == 'A') // Cancel
            {    
                menuMode = M_IDLE; 
                lcd.clear();  
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
                lcd.clear();  
                displayWeigth();         
            }
            else if (key == 'D') 
            {
                setOpenPos();
                closeValve();
                lcd.clear();
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
