/***********************************************************************************************************
  Title:        EGR 326 Lab 7
  Filename:     Part_III_Slave.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/16/2020
  Instructor:   Professor Brian Krug
  Description:  Part III of Lab 7
  Note:
************************************************************************************************************
WIRING FOR MSP432
Push Button   : In  - P3.6
White Stepper : IN1 - P2.3 | IN2 - P5.1 | IN3 - P3.5 | IN4 - P3.7
Full Color LCD: SCK - P9.5 | SDA - P9.7 | LED - 3.3V | A0 - P9.2  | RST - P9.3 |
                VCC - 3.3V | GND - GND  | CS  - P9.4
***********************************************************************************************************/
#include "msp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

volatile uint8_t BUT1;
volatile int Direction;
#define STEPSmain 10 //645 is the largest
#define BUTPORT P3
#define BUTPIN BIT6

void Clock_Init48MHz(void);

int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    BUTPORT->SEL0 &= ~(BUTPIN); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUTPIN);
    BUTPORT->DIR &= ~(BUTPIN);
    BUTPORT->REN |= (BUTPIN);   //enable resistor
    BUTPORT->OUT |= (BUTPIN);   //enable pull up
    BUTPORT->IES |= (BUTPIN);   //set pin as interrupt
    BUTPORT->IE |= (BUTPIN);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUTPIN);  //clears interrupt flag

    Clock_Init48MHz();
    SysTickInit();
    initWhiteStepper();
    Direction = 0;
    BUT1 = 0;

    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    __enable_interrupt();

    while(1){
        if(BUT1){
            Direction = !Direction;
            step(STEPSmain, Direction);
            BUT1 = 0;
        }
    }
}

void PORT3_IRQHandler(void) // port 3 interrupt handler
{
    if((BUTPORT->IFG & BUTPIN)){
        while(!(BUTPORT->IN & BUTPIN));
        BUT1 = 1;
    }
    BUTPORT->IFG &= ~(BUTPIN);
}

void Clock_Init48MHz(void){
    // Configure Flash wait-state to 1 for both banks 0 & 1
       FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK0_RDCTL_WAIT_1;
       FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK1_RDCTL_WAIT_1;

    //Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK
       PJ->SEL0 |= BIT2 | BIT3;                     // Configure PJ.2/3 for HFXT function
       PJ->SEL1 &= ~(BIT2 | BIT3);
       CS->KEY = CS_KEY_VAL ;                       // Unlock CS module for register access
       CS->CTL2 |= CS_CTL2_HFXT_EN | CS_CTL2_HFXTFREQ_6 | CS_CTL2_HFXTDRIVE;
          while(CS->IFG & CS_IFG_HFXTIFG)
                    CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

      CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK     |// select MCLK and HSMCLK no divider
                              CS_CTL1_DIVM_MASK     |
                              CS_CTL1_SELS_MASK     |
                              CS_CTL1_DIVHS_MASK)   |
                              CS_CTL1_SELM__HFXTCLK |
                              CS_CTL1_SELS__HFXTCLK;

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_2;    // change the SMCLK clock speed to 12 MHz.

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}

