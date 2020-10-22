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
***********************************************************************************************************/
#include "msp.h"
#include <string.h>
#in- clude <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define S1 BIT1
#define S2 BIT4
uint32_t status1, status2;

#define SLAVE_ADDRESS 0x48

char TXData[2] = "00";
int i = 0;

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

    NVIC->ISER[1] = 0x00000008;// Port P1 interrupt is enabled in NVIC
    NVIC->ISER[0] = 0x00100000;// EUSCI_B0 interrupt is enabled in NVIC
    NVIC->ISER[1] = 1 << ((PORT2_IRQn) & 31);//for keypad
    __enable_irq();

    while (EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR | EUSCI_B_CTLW0_TXSTT;
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Sleep on exit

        while(1){
            if(keyPressed)//waits for an interrupt
                {
                keyMain = readKey();//reads the key
                if(!(strcmp(decodeKey(keyMain),"*"))){
                    printf("Please Enter:\n1 - Day Month Year\n2 - "
                            "Hour Minute Second\n3 - Temperature\n");
                    while(1){
                        if(keyPressed)//waits for an interrupt
                            {
                            ReadRTC();
                            keyMain = readKey();//reads the key
                            if(!(strcmp(decodeKey(keyMain),"1")))
                            {
                                printf("\nThe Date is:\n%s\n%s %d%d, 2%d%d%d\n\n",
                                       dayOfWeekDecode(readRTC.dayOfWeek),
                                       monthDecode(readRTC.monthT *10 + readRTC.monthO),
                                       readRTC.dayT, readRTC.dayO,
                                       readRTC.yearC, readRTC.yearD, readRTC.yearO);

                                printf("\nPlease Press '*' to Enter the Sub Menu\n\n");
                                break;

                            }
                            else if(!(strcmp(decodeKey(keyMain),"2")))
                            {
                                printf("\nTime:\n%d%d:%d%d:%d%d %s\n", readRTC.hourT, readRTC.hourO,
                                   readRTC.minT, readRTC.minO,
                                   readRTC.secT, readRTC.secO, (readRTC.PMAM ? "PM" : "AM"));

                                printf("\nPlease Press '*' to Enter the Sub Menu\n\n");
                                break;

                            }
                            else if(!(strcmp(decodeKey(keyMain),"3")))
                            {
                                printf("\nTempature:\n%d%cC\n", readRTC.temp, 176);
                                printf("\nPlease Press '*' to Enter the Sub Menu\n\n");
                                break;

                            }
                            }
                        }
                    }
        }
    }

}

void EUSCIB0_IRQHandler(void){//like previous set up just with different pins and adding interrupts
    status1 = EUSCI_B0->IFG;
    status2 = EUSCI_B0->IFG;
    EUSCI_B0->IFG &=~ EUSCI_B_IFG_TXIFG0;
    if(status1 & EUSCI_B_IFG_TXIFG0){//send pos value
        EUSCI_B0->TXBUF = "+";
        EUSCI_B0->TXBUF = TXData[0];
        EUSCI_B0->TXBUF = TXData[1];
        EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE0;}
    if(status1 & EUSCI_B_IFG_TXIFG0){//send neg value
        EUSCI_B0->TXBUF = "-";
        EUSCI_B0->TXBUF = TXData[0];
        EUSCI_B0->TXBUF = TXData[1];
        EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE0;}
}

void PORT1_IRQHandler(void){//push button interrupt
    status1 = P1->IFG;
    status2 = P1->IFG;
    P1->IFG &= ~S1;
    P1->IFG &= ~S2;
    if(status1 & S1) EUSCI_B0->IE |= EUSCI_B_IE_TXIE0;   // Enable EUSCI_A0 TX interrupt
    if(status2 & S2) EUSCI_B0->IE |= EUSCI_B_IE_TXIE0;   // Enable EUSCI_A0 TX interrupt
}

