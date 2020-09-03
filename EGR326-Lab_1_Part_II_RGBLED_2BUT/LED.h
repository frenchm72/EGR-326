#ifndef __LED_H__
#define __LED_H__

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DELAY 5                              //macro for the delay function, can be easily changed
#define BUTPORT P6
#define BUTPIN BIT4
#define BUTPIN2 BIT5
#define LEDPORT P4
#define REDLED BIT4
#define GREENLED BIT2
#define BLUELED BIT0

enum  states                                        //this is the set up for the states used in the program
{
    LED_OFF,                                        //State for OFF
    LED_RED,                                        //State for RED
    LED_GREEN,                                      //State for GREEN
    LED_BLUE                                        //State for BLUE

} state;

void initPins(void)
{
    LEDPORT->SEL0 &= ~(REDLED|GREENLED|BLUELED);                  //setting up LEDs
    LEDPORT->SEL1 &= ~(REDLED|GREENLED|BLUELED);
    LEDPORT->DIR |= (REDLED|GREENLED|BLUELED);
    LEDPORT->OUT |= (REDLED|GREENLED|BLUELED);

    BUTPORT->SEL0 &= ~(BUTPIN|BUTPIN2);                            //setting up buttons
    BUTPORT->SEL1 &= ~(BUTPIN|BUTPIN2);
    BUTPORT->DIR &= ~(BUTPIN|BUTPIN2);
    BUTPORT->REN |= (BUTPIN|BUTPIN2);
    BUTPORT->OUT |= (BUTPIN|BUTPIN2);
}

void StsTick_Init(void)//sys tick initialization
{
    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts

}

int delay_ms(int ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while(!(SysTick->CTRL & BIT(16))==0);
    return ms;
}

int butPres(uint8_t b)
{
    if((!((BUTPORT->IN & BUTPIN)))|(!((BUTPORT->IN & BUTPIN2))))
    {
        delay_ms(DELAY);
        while((!((BUTPORT->IN & BUTPIN)))|(!((BUTPORT->IN & BUTPIN2))))
        {
            if((!((BUTPORT->IN & BUTPIN)))&(!((BUTPORT->IN & BUTPIN2))))
            {
                b = 3;
            }
            else if((!((BUTPORT->IN & BUTPIN2))))
            {
                b = 2;
            }
            else
            {
                b = 1;
            }
        }
    }
    else
    {
        b = 0;
    }
    return b;
}

#endif
