/***********************************************************************************************************
  Title:        EGR 326 Lab 7
  Filename:     Part_II_Master.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/16/2020
  Instructor:   Professor Brian Krug
  Description:  Part II of Lab 7
  Note:         This code was provided by professor kandalaft. This code implements a sleep more and
  interrupt use by the EUSCI
************************************************************************************************************
WIRING FOR MSP432
Push Button   : In  - P1.1
I2C           : SDA - P1.6 | SCL - P1.7 | GND - GND
***********************************************************************************************************/
#include "msp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define S1 BIT1
#define SLAVE_ADDRESS 0x48

char TXData[10] = "RGBRGBRGBG";
int i = 0;

void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    P1->DIR &= ~S1;//push button 1 onboard MSP
    P1->REN   = S1;
    P1->OUT   = S1;
    P1->IE       = S1;
    P1->IES     = S1;
    P1->IFG    = 0x00;
    P1->SEL0 |= BIT6 | BIT7; // P1.6 and P1.7 as UCB0SDA and UCB0SCL

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;  //like previous set up just with different pins and adding interrupts
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MODE_3|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;
    EUSCI_B0->BRW = 0x001E;
    EUSCI_B0->I2CSA = SLAVE_ADDRESS;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    NVIC->ISER[1] = 0x00000008;// Port P1 interrupt is enabled in NVIC
    NVIC->ISER[0] = 0x00100000;// EUSCI_B0 interrupt is enabled in NVIC
    __enable_irq();

    while (EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR | EUSCI_B_CTLW0_TXSTT;
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Sleep on exit
        __sleep();                   // enter LPM0
}

void EUSCIB0_IRQHandler(void){//like previous set up just with different pins and adding interrupts
    uint32_t status = EUSCI_B0->IFG;
    EUSCI_B0->IFG &=~ EUSCI_B_IFG_TXIFG0;
    if(status & EUSCI_B_IFG_TXIFG0){
        EUSCI_B0->TXBUF = TXData[i++%10];
        EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE0;}
}

void PORT1_IRQHandler(void){//push button interrupt
    uint32_t status = P1->IFG;
    P1->IFG &= ~S1;
    if(status & S1) EUSCI_B0->IE |= EUSCI_B_IE_TXIE0;   // Enable EUSCI_A0 TX interrupt
}
