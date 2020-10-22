/***********************************************************************************************************
  Title:        EGR 326 Lab 7
  Filename:     Part_II_Master.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/16/2020
  Instructor:   Professor Brian Krug
  Description:  Part II of Lab 7
  Note:
************************************************************************************************************
WIRING FOR MSP432
Push Button   : In  - P1.1
I2C           : SDA - P1.6 | SCL - P1.7 | GND - GND
KeyPad        : ROW0 - P2.7 | ROW1 - P2.6 | ROW2 - P2.4 | ROW3 - P2.5 |
                            | COL0 - P3.0 | COL1 - P5.6 | COL2 - P5.7 |
***********************************************************************************************************/
#include "msp.h"
#include "keypad.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define S1 BIT1
#define S2 BIT4
uint32_t status1, status2;

#define SLAVE_ADDRESS 0x48
int TXData[2];
int i = 0;
int keyMain;

void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    P1->DIR &= ~(S1|S2);//push button 1 onboard MSP
    P1->REN   = (S1|S2);
    P1->OUT   = (S1|S2);
    P1->IE       = (S1|S2);
    P1->IES     = (S1|S2);
    P1->IFG    = 0x00;
    P1->SEL0 |= BIT6 | BIT7; // P1.6 and P1.7 as UCB0SDA and UCB0SCL

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;  //like previous set up just with different pins and adding interrupts
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MODE_3|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;
    EUSCI_B0->BRW = 0x001E;
    EUSCI_B0->I2CSA = SLAVE_ADDRESS;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    KeyInit();//init keypad
    SysTickInit();
    interON();

    NVIC->ISER[1] = 0x00000008;// Port P1 interrupt is enabled in NVIC
    NVIC->ISER[0] = 0x00100000;// EUSCI_B0 interrupt is enabled in NVIC
    NVIC->ISER[1] = 1 << ((PORT2_IRQn) & 31);//for keypad
    __enable_irq();

    while (EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR | EUSCI_B_CTLW0_TXSTT;

    while(1){
            if(keyPressed)//waits for an interrupt
                {
                keyMain = readKey();//reads the key
                keyMain = decodeKey(keyMain);
                if((keyMain != 10) && (keyMain != 11) && (keyMain != 12)){
                    i = !i;
                    TXData[i] = keyMain;
                    printf("\nYour Step:%d%d\n", TXData[1], TXData[0]);
                    }
        }
    }
    }

void EUSCIB0_IRQHandler(void){//like previous set up just with different pins and adding interrupts
    EUSCI_B0->IFG &=~ EUSCI_B_IFG_TXIFG0;
    if(status1 && EUSCI_B_IFG_TXIFG0){//send pos value
        EUSCI_B0->TXBUF = 0x2B;
        while(!(EUSCI_B0->IFG & 2));
        EUSCI_B0->TXBUF = TXData[1]+48;
        while(!(EUSCI_B0->IFG & 2));
        EUSCI_B0->TXBUF = TXData[0]+48;
        EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE0;
        status1 = 0;}
    if(status2 && EUSCI_B_IFG_TXIFG0){//send neg value
        EUSCI_B0->TXBUF = 0x2D;
        while(!(EUSCI_B0->IFG & 2));
        EUSCI_B0->TXBUF = TXData[1]+48;
        while(!(EUSCI_B0->IFG & 2));
        EUSCI_B0->TXBUF = TXData[0]+48;
        EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE0;
        status2 = 0;}
}

void PORT1_IRQHandler(void){//push button interrupt
    status1 = P1->IFG & S1;
    status2 = P1->IFG & S2;
    P1->IFG &= ~S1;
    P1->IFG &= ~S2;
    if((status1 & S1) | (status2 & S2)) EUSCI_B0->IE |= EUSCI_B_IE_TXIE0;   // Enable EUSCI_A0 TX interrupt
}

