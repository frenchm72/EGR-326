/***********************************************************************************************************
  Title:        EGR 326 Lab 3
  Filename:     Part_I.c, keypad.c, keypad.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/25/2020
  Instructor:   Professor Brian Krug
  Description:  This code is used to interface a Keypad with the MSP432 using interrupts
  Note:
************************************************************************************************************
WIRING FOR MSP432
KeyPad [Left -> Right]: ROW0 - P2.7 | ROW1 - P2.6 | ROW2 - P2.4 | ROW3 = P5.6 |
                      | COL0 - P6.6 | COL1 - P6.7 | COL2 = P2.3 |
***********************************************************************************************************/
#include "msp.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	uint16_t keys = 0;

	KeyInit();//init keypad
	SysTickInit();//init systick
	interON();//turns col on

	while(1)
	{
	    if(keyPressed)//waits for an interrupt
	    {
	        keys = readKey();//reads the key
	    }
	    if(keys)//if a key was pressed
	    {
	        printf("%s\n", decodeKey(keys));//print the decoded key
	        keys = 0;//clears the key press
	    }
	}
}
