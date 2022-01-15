#include "arduino.h"
#include "menucorong.h"

void writeMenu(char *text , int pos)
{
    for (int i = 0 ; i < 16 ; i++)
    {
        lineMenu_1[i] = text[i];
    }

}

void test()
{
    writeMenu("test" , 1);
    
}