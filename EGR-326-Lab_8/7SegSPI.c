#include "msp.h"
#include <7SegSPI.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void initSPI(void){// inits SPI
    CSPORT->SEL0 &= ~CSPIN;//chip select pin regular GPIO
    CSPORT->SEL1 &= ~CSPIN;
    CSPORT->DIR |= CSPIN;
    CSPORT->REN |= CSPIN;
    CSPORT->OUT |= CSPIN;

    DINPORT -> SEL0 |= DINPIN; //  data in and clk pin for 7 sement SPI
    CLKPORT -> SEL0 |= CLKPIN;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;                                                          // Hold EUSCI_B0 module in reset state
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_MST
                                                | EUSCI_B_CTLW0_SYNC
                                                | EUSCI_B_CTLW0_CKPL
                                                | EUSCI_B_CTLW0_MSB;
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2; // Select SMCLK as EUSCI_B0 clock
    EUSCI_B0 -> BRW = 0x0018;               // Set BITCLK = BRCLK/ (UCBRx+1) = 3 MHz/24 = 128K
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // resume operation

    sendSPI(DECODE, DECODE_MODE); //inits the 7 segment described by professor Krug
    sendSPI(INTENSITY, INTENSITY_MODE);
    sendSPI(SCAN_LIMIT, SCAN_LIMIT_MODE);
    sendSPI(SHUTDOWN, SHUTDOWN_MODE);// normal mode
   // sendSPI(DIS_TEST, DIS_TEST_MODE);
}

void sendSPI(uint8_t Command, uint8_t Data){// sends data like Professor Krug prompted
    CSPORT -> OUT &= ~CSPIN;
    delay_us(COMMANDDELAY);
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF = Command;
    while(!( EUSCI_B0->IFG&2));
    EUSCI_B0->TXBUF =Data;
    while(!( EUSCI_B0->IFG&2));
    delay_us(COMMANDDELAY);
    CSPORT -> OUT |= CSPIN;
}

void SysTickInit(void){ //SysTick Setup 3MHz, refer to lab 6 for 48MHz version
    SysTick -> CTRL = 0;//disable systick during setup
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts
    }

void delay_ms(int ms){//delay in milliseconds using SysTick
    SysTick->LOAD =(3333*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
    }

void delay_us(int us){//delay in microseconds using SysTick
    SysTick->LOAD = (3*us)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
    }
