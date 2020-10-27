#include "msp.h"
#include <7SegSPI.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void initSPI(void){
    DINPORT -> SEL0 |= DINPIN;
    CLKPORT -> SEL0 |= CLKPIN;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST
                                            |EUSCI_B_CTLW0_MODE_0
                                            |EUSCI_B_CTLW0_MST
                                            |EUSCI_B_CTLW0_SYNC;
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Clear SWRST to resume operation
}

void sendSPI(int Command, int Data){
    delay_us(100);
    CSPORT -> OUT |= CSPIN;
    delay_us(100);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = Command;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF =Data;
    while(!( EUSCI_B0->IFG&2));
    delay_us(100);
    CSPORT -> OUT &= ~CSPIN;
}

void SysTickInit(void){
SysTick -> CTRL = 0;//disable systick during setup
SysTick -> LOAD = 0x00FFFFFF;//max reload value
SysTick -> VAL = 0;//clears it
SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts
}

void delay_ms(int ms){//delay in milliseconds using systick
SysTick->LOAD =(3333*ms)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}

void delay_us(int us){//delay in milliseconds using systick
SysTick->LOAD = (3*us)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}
