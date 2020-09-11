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

volatile uint8_t LED;
volatile uint8_t BUT1;
volatile uint8_t Direction;
volatile uint8_t timeout;
#define BUTPORT P1
#define BUTPIN BIT6
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

void initMSP(void)
{
    LEDPORT->SEL0 &= ~(REDLED|GREENLED|BLUELED);                  //setting up LEDs
    LEDPORT->SEL1 &= ~(REDLED|GREENLED|BLUELED);
    LEDPORT->DIR |= (REDLED|GREENLED|BLUELED);
    LEDPORT->OUT |= (REDLED|GREENLED|BLUELED);
    LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off

    BUTPORT->SEL0 &= ~(BUTPIN); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUTPIN);
    BUTPORT->DIR &= ~(BUTPIN);
    BUTPORT->REN |= (BUTPIN);   //enable resistor
    BUTPORT->OUT |= (BUTPIN);   //enable pull up
    BUTPORT->IES |= (BUTPIN);   //set pin as interrupt
    BUTPORT->IE |= (BUTPIN);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUTPIN);  //clears interrupt flag

    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 3000000;//1 sec reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000007;//enables systick 3MHz no interrupts

    state = LED_OFF;//initial state
}

void delay_ms(int ms)//delay in milliseconds using systick
{
    SysTick->LOAD =(3000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}

void PORT1_IRQHandler(void) // port 1 interrupt handler
{
    if((BUTPORT->IFG & BUTPIN) && (!(BUTPORT->IN & BUTPIN)))
    {
       BUT1 = !BUT1;
       Direction = !Direction;
    }
    BUTPORT->IFG &= ~(BUTPIN);
}

void SysTick_Handler(void)
{
    timeout = 1;
}

void RunLED(void)
{
    if(!(BUTPORT->IN & BUTPIN))
    {
        switch(state)
                {
                case LED_OFF:                               //this if for the color black/off
                    LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off
                    if(timeout && (Direction == 0))
                    {
                        state =LED_RED;
                        timeout = 0;
                    }
                    else if(timeout && (Direction == 1))
                    {
                        state =LED_BLUE;
                        timeout = 0;
                    }
                    else
                        state =LED_OFF;
                    break;
                case LED_RED:
                    LEDPORT->OUT &= ~(GREENLED|BLUELED); //to turn LED RED
                    LEDPORT->OUT |= (REDLED);
                    if(timeout && (Direction == 0))
                    {
                        state =LED_GREEN;
                        timeout = 0;
                    }
                    else if(timeout && (Direction == 1))
                    {
                        state =LED_BLUE;
                        timeout = 0;
                    }
                    else
                        state =LED_RED;
                    break;
                case LED_GREEN:
                    LEDPORT->OUT &= ~(REDLED|BLUELED);
                    LEDPORT->OUT |= (GREENLED);                      //this turns LED GREEN
                    if(timeout && (Direction == 0))
                    {
                        state =LED_BLUE;
                        timeout = 0;
                    }
                    else if(timeout && (Direction == 1))
                    {
                        state =LED_RED;
                        timeout = 0;
                    }
                    else
                        state =LED_GREEN;
                    break;
                case LED_BLUE:
                    LEDPORT->OUT &= ~(REDLED|GREENLED);
                    LEDPORT->OUT |= (BLUELED);                      //this turns LED BLUE
                    if(timeout && (Direction == 0))
                    {
                        state =LED_RED;
                        timeout = 0;
                    }
                    else if(timeout && (Direction == 1))
                    {
                        state =LED_GREEN;
                        timeout = 0;
                    }
                    else
                        state =LED_BLUE;
                    break;
                default:
                    state = LED_OFF;
                    break;
                }
    }
    else
        BUT1 = 0;
}

#endif
