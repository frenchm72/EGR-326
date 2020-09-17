/**********************************************************************************************************
  Title:        EGR 326 Lab 3
  Filename:     PARTI.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/18/2020
  Instructor:   Professor Brian Krug
  Description:
  Note:
***********************************************************************************************************/
#ifndef __PARTI_H__
#define __PARTI_H__

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

#endif
