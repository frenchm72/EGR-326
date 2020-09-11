/**************************************************************************************************************
  Title:        EGR 326 Lab 2
  Filename:     PARTII.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/11/2020
  Instructor:   Professor Brian Krug
  Description:  This program is made to interface with a button that will control a RGB LED. The button will
                decide how quickly the LED is Changing/toggling.
  Note:
***************************************************************************************************************/
#ifndef __PARTII_H__
#define __PARTII_H__

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

volatile float Freq;
volatile uint8_t LED;
volatile uint8_t BUT1;
volatile uint8_t BUT2;
#define MINFREQ 0.5
#define MAXFREQ 250
#define BUTPORT P1
#define BUTPIN BIT6
#define BUTPIN2 BIT7
#define LEDPORT P4
#define BLUELED BIT0

void initMSP(void)
{
    LEDPORT->SEL0 &= ~(BLUELED);    //setting up LEDs
    LEDPORT->SEL1 &= ~(BLUELED);
    LEDPORT->DIR |= (BLUELED);
    LEDPORT->OUT |= (BLUELED);

    BUTPORT->SEL0 &= ~(BUTPIN|BUTPIN2); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUTPIN|BUTPIN2);
    BUTPORT->DIR &= ~(BUTPIN|BUTPIN2);
    BUTPORT->REN |= (BUTPIN|BUTPIN2);   //enable resistor
    BUTPORT->OUT |= (BUTPIN|BUTPIN2);   //enable pull up
    BUTPORT->IES |= (BUTPIN|BUTPIN2);   //set pin as interrupt
    BUTPORT->IE |= (BUTPIN|BUTPIN2);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUTPIN|BUTPIN2);  //clears interrupt flag

    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts
}

void delay_ms(int ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}

void PORT1_IRQHandler(void) // port 1 interrupt handler
{
    if((BUTPORT->IFG & BUTPIN))
        BUT1 = 1;
    if((BUTPORT->IFG & BUTPIN2))
        BUT2 = 1;
    BUTPORT->IFG &= ~(BUTPIN|BUTPIN2);
}

void FreqChange(void)
{
if((BUT1) && (!LED))
    {
        Freq = 0.5;
        LED = 1;
        BUT1 = 0;
    }
    else if ((BUT1) && (LED))
    {
        if(Freq >= MAXFREQ)
        {
            Freq = MAXFREQ;
            LED = 1;
            BUT1 = 0;
        }
        else
        {
            Freq = Freq * 2;
            LED = 1;
            BUT1 = 0;
        }
    }

    if((BUT2) && (!LED))
    {
        Freq = MINFREQ;
        LED = 0;
        BUT2 = 0;
    }
    else if ((BUT2) && (LED))
    {
        if(Freq <= MINFREQ)
        {
            Freq = MINFREQ;
            LED = 0;
            BUT2 = 0;
        }
        else
        {
            Freq = Freq / 2;
            LED = 1;
            BUT2 = 0;
        }
    }
}
#endif
