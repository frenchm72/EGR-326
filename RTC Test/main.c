/***********************************************************************************************************
  Title:        EGR 326 Lab 5 - Testing
  Filename:     main.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/02/2020
  Description:  Part I of Lab 5
  Note:
************************************************************************************************************
WIRING FOR MSP432
RTC: SCL - PX.X | SDA - PX.X | VCC - 3.3V | GND - GND |
***********************************************************************************************************/

#include "msp.h"
#include "rtc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

void I2C1_init(void);
int I2C1_burstWrite(int slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data);
int I2C1_burstRead(int slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data);

//#define S1 BIT1
//#define SLAVE_ADDRESS 0x48
//
//char TXData[10] = "RGBBGRBGGR";
//int i = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	unsigned char timeDateReadback[7];
	unsigned char timeDateToSet[15] = {0x55, 0x58, 0x16, 0x05, 0x19, 0x11, 0x15, 0};
	//2015 November 19, Thur, 16:58:55
	P2->DIR |= 7;

	I2C1_init();

	I2C1_burstWrite(SLAVEADDR, 0, 7, timeDateToSet);

	for(;;){
	    I2C1_burstRead(SLAVEADDR, 0, 7, timeDateReadback);
	    printf("%s\n", timeDateReadback);
	    __delay_cycles(3000000);
	}

//    P1->DIR &= ~S1;
//    P1->REN  = S1;
//    P1->OUT  = S1;
//    P1->IE   = S1;
//    P1->IES  = S1;
//    P1->IFG  = 0x00;
//    P1->SEL0 |= BIT6 | BIT7; // P1.6 and P1.7 as UCB0SDA and UCB0SCL
//
//    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;         // Hold EUSCI_B0 module in reset state
//    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MODE_3|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC;
//    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;       // Select SMCLK as EUSCI_B0 clock
//    EUSCI_B0->BRW = 0x001E;      // Set BITCLK = BRCLK / (UCBRx+1) = 3 MHz / 30 = 100 kHz
//    EUSCI_B0->I2CSA = SLAVE_ADDRESS;
//    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;         // Clear SWRST to resume operation
//
//    NVIC->ISER[1] = 0x00000008;                 // Port P1 interrupt is enabled in NVIC
//    NVIC->ISER[0] = 0x00100000;             // EUSCI_B0 interrupt is enabled in NVIC
//    __enable_irq();                     // All interrupts are enabled
//
//    while (EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
//       EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR | EUSCI_B_CTLW0_TXSTT;
//       SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Sleep on exit
//       __sleep();                   // enter LPM0
}

// void EUSCIB0_IRQHandler(void){
//       uint32_t status = EUSCI_B0->IFG;            // Get EUSCI_B0 interrupt flag
//       EUSCI_B0->IFG &=~ EUSCI_B_IFG_TXIFG0;       // Clear EUSCI_B0 TX inter. flag
//       if(status & EUSCI_B_IFG_TXIFG0){            // Check if transmit interrupt occurs
//           EUSCI_B0->TXBUF = TXData[i++%10];   // Load current TXData value to transmit buffer
//           EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE0;}         // Disable EUSCI_B0 TX interrupt
//   }
//
//   void PORT1_IRQHandler(void){
//       uint32_t status = P1->IFG;
//       P1->IFG &= ~S1;
//       if(status & S1) EUSCI_B0->IE |= EUSCI_B_IE_TXIE0;   // Enable EUSCI_A0 TX interrupt
//   }

void I2C1_init(void){
    EUSCI_B1->CTLW0 |= 1;
    EUSCI_B1->CTLW0 = 0x0F81;
    EUSCI_B1->BRW = 30;
    P6->SEL0 |= 0x30;
    P6->SEL1 &= ~0x30;
    EUSCI_B1->CTLW0 &= ~1;
}

int I2C1_burstWrite(int slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data){
    if(byteCount <=0)
        return -1;

    EUSCI_B1->I2CSA = slaveAddr;
    EUSCI_B1->CTLW0 |= 0x0010;
    EUSCI_B1->CTLW0 |= 0x0002;


    EUSCI_B1->TXBUF = memAddr;

    do{
        while(!(EUSCI_B1->IFG & 2));
        EUSCI_B1->TXBUF = *data++;
        byteCount--;
    } while(byteCount > 0);

    while(!(EUSCI_B1->IFG & 2));
    EUSCI_B1->CTLW0 |= 0x0004;
    while(EUSCI_B1->CTLW0 & 4);

    return 0;
}
int I2C1_burstRead(int slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data){
    if(byteCount <=0)
        return -1;

    EUSCI_B1->I2CSA = slaveAddr;
    EUSCI_B1->CTLW0 |= 0x0010;
    EUSCI_B1->CTLW0 |= 0x0002;

    while(!(EUSCI_B1->IFG & 2));
    EUSCI_B1->TXBUF = memAddr;
    while(!(EUSCI_B1->IFG & 2));
    EUSCI_B1->CTLW0 |= 0x0010;
    EUSCI_B1->CTLW0 |= 0x0002;
    while(EUSCI_B1->CTLW0 & 2);

    do{
        if(byteCount == 1)
            EUSCI_B1->CTLW0 |= 0X0004;


        while(!(EUSCI_B1->IFG & 2));
        *data++ = EUSCI_B1->RXBUF;
        byteCount--;
    } while(byteCount);

    while(EUSCI_B1->CTLW0 & 4);

    return 0;
}
