/***********************************************************************************************************
  Title:        EGR 326 Lab 3
  Filename:     PARTI.C
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

float distIN = 0.0, distCM = 0.0;//global variables
float rise, fall, pulseWidth;
unsigned int RiseFlag = 0;

#define TRIGPORT P5 //macros
#define TRIGPIN BIT6 //timer A2.1
#define ECHOPORT P2
#define ECHOPIN BIT4 //timer A0.2

void initMSP(void)
{
    TRIGPORT->SEL0 &= ~(TRIGPIN);                  //setting up pin as an output
    TRIGPORT->SEL1 &= ~(TRIGPIN);
    TRIGPORT->DIR |= (TRIGPIN);
    TRIGPORT->OUT |= (TRIGPIN);
    TRIGPORT->OUT &= ~(TRIGPIN);

    ECHOPORT->SEL0 |= (ECHOPIN); //TA0.CCI2A input capture pin, second function
    ECHOPORT->SEL1 &= ~(ECHOPIN);//TA0.CCI2A input capture pin, second function
    ECHOPORT->DIR &= ~(ECHOPIN);

    TIMER_A0->CTL |=TIMER_A_CTL_TASSEL_2 | // Use SMCLK as clock source,
                    TIMER_A_CTL_MC_2   | // Start timer in UP mode
                    TIMER_A_CTL_CLR;       // clear TA0R

    TIMER_A0->CCTL[1] =TIMER_A_CCTLN_CM_3    | // Capture rising and falling edge,
                       TIMER_A_CCTLN_CCIS_0  | // Use CCI2A
                       TIMER_A_CCTLN_CCIE    | // Enable capture interrupt
                       TIMER_A_CCTLN_CAP     | // Enable capture mode,
                       TIMER_A_CCTLN_SCS;      // Synchronous capture

    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts


    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31); // Enable interrupt in NVIC vector
    __enable_irq ( );//enable global interrupt
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

void TA0_N_IRQHandler(void)
{
    rise = TIMER_A0->CCR[1]; // Get current count
    if (ECHOPORT->IN&ECHOPIN)  //  record timer on a falling edge
        TIMER_A0->CTL |=TIMER_A_CTL_CLR;    // start timer on a rising edge
    else
        pulseWidth = rise; // record time on falling edge

    TIMER_A0->CCTL[1] &= ~(TIMER_A_CCTLN_CCIFG);    // Clear the interrupt flag
}

#endif
