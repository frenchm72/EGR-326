/***********************************************************************************************************
  Title:        EGR 326 Lab 4
  Filename:     Part_III.c, encoder.c, encoder.h, keypad.c, keypad.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/25/2020
  Instructor:   Professor Brian Krug
  Description:  This code is used to interface a rotary encoder and keypad with the MSP432 using interrupts
  Note:
************************************************************************************************************
WIRING FOR MSP432
Encoder: + - 3.3V | GND - GND | DT - P5.2 // Cap | CLK - P5.1 // Cap| SW - P5.0 |
KeyPad [Left -> Right]: ROW0 - P2.7 | ROW1 - P2.6 | ROW2 - P2.4 | ROW3 - P5.6 |
                      | COL0 - P6.6 | COL1 - P6.7 | COL2 - P2.3 |
RGB LED: R - P3.7 | G - Float | B - P3.6 |
***********************************************************************************************************/
#include "msp.h"
#include "encoder.h"
#include "keypad.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LEDPORT P3
#define BLUELED BIT6
#define REDLED BIT7

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int breakFlag = 0, numFlag = 1, printFlag = 0;;
    uint16_t keys, key = 0;

    LEDPORT->SEL0 &= ~(BLUELED|REDLED);    //setting up LEDs
    LEDPORT->SEL1 &= ~(BLUELED|REDLED);
    LEDPORT->DIR |= (BLUELED|REDLED);
    LEDPORT->OUT |= (BLUELED|REDLED);
    LEDPORT->OUT &= ~(BLUELED|REDLED); //turns leds off

    rotorInit();
    KeyInit();//init keypad
    SysTickInit();
    interON();//turns col on

    NVIC->ISER[1] = 1 << ((PORT6_IRQn) & 31);
    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);
    NVIC->ISER[1] = 1 << ((PORT2_IRQn) & 31);
    __enable_interrupt();

    while(1)
    {
        while(numFlag)//asks user for a two digit number
        {
            if(printFlag==0)
            {
            printf("\nPlease Enter a Two Digit Number Using the Keypad\nValues can be from 00-99"
            "\nThe symbols # and * are treated as Zeros\n");
            printFlag = 1;
            }
            if(keyPressed)//waits for an interrupt
                key = readKey();//reads the key
            if(key && (breakFlag == 0))//if a key was pressed
            {
                keys = 0;
                keys = 10 * atoi(decodeKey(key));//decoded key
                key = 0;//clears the key press

                breakFlag = 1;
            }
            else if(key && (breakFlag == 1))//if a key was pressed
            {
                keys = keys + atoi(decodeKey(key));//decoded key
                key = 0;//clears the key press
                breakFlag = 0;
//                CCWcount = 0;//uncomment if need to clear before looking
//                CWcount = 0;
                numFlag = 0;
                printFlag = 0;
                printf("\nEntered: %d\n", keys);
                break;
            }
        }
        if(SWflag)
        {
            LEDPORT->OUT &= ~(BLUELED|REDLED); //turns leds off
            CWcount = 0;
            CCWcount = 0;
            SWflag = 0;
            numFlag = 1;
        }

        if((CCWcount - CWcount)== keys)//this block will look at the difference
            // or (CCWcount == keys)
            LEDPORT->OUT |= (BLUELED); //turns Blue leds on
        if((CWcount - CCWcount) == keys) // or (CWcount == keys)
            LEDPORT->OUT |= (REDLED); //turns Red leds on
        else
            LEDPORT->OUT &= ~(BLUELED|REDLED); //turns leds off
    }
}

void SysTickInit(void)
{
SysTick -> CTRL = 0;//disable systick during setup
SysTick -> LOAD = 0x00FFFFFF;//max reload value
SysTick -> VAL = 0;//clears it
SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts
}

void delay_ms(int ms)//delay in milliseconds using systick
{
SysTick->LOAD =(3333*ms)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}

void delay_us(int us)//delay in milliseconds using systick
{
SysTick->LOAD = (3*us)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}
