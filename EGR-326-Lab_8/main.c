/*
 *                MSP432P401
 *              -----------------
 *             |                 |
 *             |             P4.3|-> Chip Select
 *             |                 |
 *             |             P1.6|-> Data Out (UCB0SIMO)
 *             |                 |
 *             |             P1.7|
 *             |                 |
 *             |             P1.5|-> Serial Clock Out (UCB0CLK)
 */
#include "C:\ti\msp\MSP432Ware_3_50_00_02\driverlib\driverlib\MSP432P4xx\driverlib.h"

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

/* Statics */
static volatile uint8_t RXData = 0;
static uint8_t TXData = 0;
static uint8_t ii = 0;
int j , p;

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig spiMasterConfig =
{
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        32000,                                     // SMCLK = DCO = 32khz
        32000,                                     // SPICLK = 32khz
        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
                EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
        EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode

};


void send16BitData(uint8_t addr, uint8_t value){
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);  // CS LOW
        TXData = addr;
        SPI_transmitData(EUSCI_B0_BASE, TXData); // Send address.
        TXData = value;
        SPI_transmitData(EUSCI_B0_BASE, TXData); // Send value.


        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);  // CS HIGH
        for (j=0; j<400; j++); //some delay
      GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);  // CS LOW

}


int main(void)
{
    volatile uint32_t ii;

    /* Halting WDT  */
    WDT_A_holdTimer();

        /* Selecting P 1.4 P1.5 P1.6 and P1.7 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);
        //EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    /* Enabling interrupts */
    //SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
    //Interrupt_enableInterrupt(INT_EUSCIB0);
    //Interrupt_enableSleepOnIsrExit();

        /* SPI --> P4.3 = CS */
        GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);

        /* Selecting P1.0 as LED */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

        /* Delaying waiting for the module to initialize */
    for (j=0; j<400; j++); //some delay

        while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT))); /* Polling to see if the TX buffer is ready */

//      // Run test
//      // All LED segments should light up
//
//      send16BitData(0x0F, 0x01);
//      for (int j=0; j<40000; j++); //some delay
//      GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED

        //Enable Mode B - decode mode
        send16BitData(0x09, 0x00);
        for (j=0; j<400; j++); //some delay
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED

        //Use Lowest Intensity
        send16BitData(0x0A, 0x0F);
        for (j=0; j<400; j++); //some delay
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED

        // scan limit
        send16BitData(0x0B, 0x07);
        for (j=0; j<400; j++); //some delay
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED

        //Display test - disable
        send16BitData(0x0F, 0x00);
        for (j=0; j<400; j++); //some delay
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED

        //Turn on Chip - Shutdown Mode
        send16BitData(0x0C, 0x01);
        for (j=0; j<400; j++); //some delay
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED

        //No test
        send16BitData(0x00, 0xFF);
        for (j=0; j<400; j++); //some delay
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //Toggle LED



        int up[8] = {0x18, 0x0C, 0x06, 0xFF, 0xFF, 0x06, 0x0C, 0x18};
        int left[8] = {0x0C, 0x1E, 0x3F, 0x0C, 0xFC, 0xFC, 0x00, 0x00};
        int right[8] = {0x00, 0x00, 0xFC, 0xFC, 0x0C, 0x3F, 0x1E, 0x0C};


        while(1){
            for (p=0x00; p<0x08; p++){
                    send16BitData(p+1,up[p]);
                    for (j=0; j<100; j++); //some delay
            }

            for (j=0; j<1000000; j++); //some delay

            for (p=0x00; p<0x08; p++){
                    send16BitData(p+1,left[p]);
                    for (j=0; j<100; j++); //some delay
            }

            for (j=0; j<1000000; j++); //some delay

            for (p=0x00; p<0x08; p++){
                    send16BitData(p+1,right[p]);
                    for (j=0; j<100; j++); //some delay
            }

            for (j=0; j<1000000; j++); //some delay

        }


}

//******************************************************************************
//
//This is the EUSCI_B0 interrupt vector service routine.
//
//******************************************************************************
void EUSCIB0_IRQHandler(void)
{
    uint32_t status = SPI_getEnabledInterruptStatus(EUSCI_B0_BASE);
    uint32_t jj;

    SPI_clearInterruptFlag(EUSCI_B0_BASE, status);

    if(status & EUSCI_B_SPI_RECEIVE_INTERRUPT)
    {
        /* USCI_B0 TX buffer ready? */
        while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

        RXData = SPI_receiveData(EUSCI_B0_BASE);

        /* Send the next data packet */
        SPI_transmitData(EUSCI_B0_BASE, ++TXData);

        /* Delay between transmissions for slave to process information */
        for(jj=50;jj<50;jj++);
    }

}
