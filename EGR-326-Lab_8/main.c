/***********************************************************************************************************
  Title:        EGR 326 Lab 8
  Filename:     main.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/23/2020
  Instructor:   Professor Brian Krug
  Description:  Lab 8
  Note:
************************************************************************************************************
WIRING FOR MSP432
Push Button   : In  - P1.1
***********************************************************************************************************/
#include "msp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    P5->SEL0 &= ~BIT0;
    P5->SEL1 &= ~BIT0;
    P5->DIR |= BIT0;
    P5->REN |= BIT0;
    P5->OUT &= ~BIT0;
    P1->SEL0 |= BIT6 | BIT7; //P1.6 and P1.7 as UCB0SDA and UCB0SCL

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;  //like previous set up just with different pins and adding interrupts
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MODE_0|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Clear SWRST to resume operation


    __delay_cycles(150*3000);
    P5->OUT |= BIT0;
    __delay_cycles(150*3000);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x09;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0xFF;
    while(!( EUSCI_B0->IFG&2));
    __delay_cycles(150*3000);
    P5->OUT &= ~BIT0;
    __delay_cycles(150*3000);
    P5->OUT |= BIT0;
    __delay_cycles(150*3000);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x0A;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x03;
    while(!( EUSCI_B0->IFG&2));
    __delay_cycles(150*3000);
    P5->OUT &= ~BIT0;
    __delay_cycles(150*3000);
    P5->OUT |= BIT0;
    __delay_cycles(150*3000);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x0B;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x07;
    while(!( EUSCI_B0->IFG&2));
    __delay_cycles(150*3000);
    P5->OUT &= ~BIT0;
    __delay_cycles(150*3000);
    P5->OUT |= BIT0;
    __delay_cycles(150*3000);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x0C;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x01;
    while(!( EUSCI_B0->IFG&2));
    __delay_cycles(150*3000);
    P5->OUT &= ~BIT0;
    __delay_cycles(150*3000);
    P5->OUT |= BIT0;
    __delay_cycles(150*3000);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x0F;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = 0x01;
    while(!( EUSCI_B0->IFG&2));
    __delay_cycles(150*3000);
    P5->OUT &= ~BIT0;

    while(1);
}
