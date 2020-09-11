/**************************************************************************************************************
  Title:        EGR 326 Lab 2
  Filename:     PARTIII.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/11/2020
  Instructor:   Professor Brian Krug
  Description:  This program is made to interface with two buttons that will control a RGB LED. The first
                button will decide which LED is toggling. Then holding the second button will toggle the LED
                selected on and off
  Note:
***************************************************************************************************************/
#ifndef __PARTIII_H__
#define __PARTIII_H__

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

volatile uint8_t LED;
volatile uint8_t BUT1;
volatile uint8_t BUT2;
volatile uint8_t timeout;
#define BUTPORT P1
#define BUTPIN BIT6
#define BUTPIN2 BIT7
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
} state, HoldState;     //sets up a holder for the case flow

void initMSP(void)
{
    LEDPORT->SEL0 &= ~(REDLED|GREENLED|BLUELED);                  //setting up LEDs
    LEDPORT->SEL1 &= ~(REDLED|GREENLED|BLUELED);
    LEDPORT->DIR |= (REDLED|GREENLED|BLUELED);
    LEDPORT->OUT |= (REDLED|GREENLED|BLUELED);
    LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off

    BUTPORT->SEL0 &= ~(BUTPIN|BUTPIN2); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUTPIN|BUTPIN2);
    BUTPORT->DIR &= ~(BUTPIN|BUTPIN2);
    BUTPORT->REN |= (BUTPIN|BUTPIN2);   //enable resistor
    BUTPORT->OUT |= (BUTPIN|BUTPIN2);   //enable pull up
    BUTPORT->IES |= (BUTPIN|BUTPIN2);   //set pin as interrupt
    BUTPORT->IE |= (BUTPIN|BUTPIN2);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUTPIN|BUTPIN2);  //clears interrupt flag

    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 1500000;//1 sec reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000007;//enables systick 3MHz no interrupts

    state = LED_OFF;//initial state
}

void PORT1_IRQHandler(void) // port 1 interrupt handler
{
    if((BUTPORT->IFG & BUTPIN))
            BUT1 = !BUT1;
    if((BUTPORT->IFG & BUTPIN2))
            BUT2 = !BUT2;
    BUTPORT->IFG &= ~(BUTPIN|BUTPIN2);
}

void SysTick_Handler(void)
{
    timeout = 1;
}

void RunLED(void)
{
    while(!(BUTPORT->IN & BUTPIN)){};

        switch(state)
                {
                case LED_OFF:                               //this if for the color black/off
                    LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off
                    if(BUT1)
                    {
                        state =LED_RED;
                        timeout = 0;
                        BUT1 = 0;
                    }
                    else if(timeout || !BUT2)
                    {
                        state = HoldState;
                        timeout = 0;
                    }
                    break;
                case LED_RED:
                    LEDPORT->OUT &= ~(GREENLED|BLUELED); //to turn LED RED
                    LEDPORT->OUT |= (REDLED);
                    if(BUT1)
                    {
                        state =LED_GREEN;
                        timeout = 0;
                        BUT1 = 0;
                    }
                    else if(timeout && BUT2)
                    {
                        HoldState = state;
                        state = LED_OFF;
                        timeout = 0;
                    }
                    break;
                case LED_GREEN:
                    LEDPORT->OUT &= ~(REDLED|BLUELED);
                    LEDPORT->OUT |= (GREENLED);                      //this turns LED GREEN
                    if(BUT1)
                    {
                        state =LED_BLUE;
                        timeout = 0;
                        BUT1 = 0;
                    }
                    else if(timeout && BUT2)
                    {
                        HoldState = state;
                        state =LED_OFF;
                        timeout = 0;
                    }
                    break;
                case LED_BLUE:
                    LEDPORT->OUT &= ~(REDLED|GREENLED);
                    LEDPORT->OUT |= (BLUELED);                      //this turns LED BLUE
                    if(BUT1)
                    {
                        state =LED_RED;
                        timeout = 0;
                        BUT1 = 0;
                    }
                    else if(timeout && BUT2)
                    {
                        HoldState = state;
                        state = LED_OFF;
                        timeout = 0;
                    }
                    break;
                default:
                    state = LED_OFF;
                    break;
                }
}

#endif
