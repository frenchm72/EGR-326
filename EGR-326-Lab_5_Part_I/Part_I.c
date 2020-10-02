/***********************************************************************************************************
  Title:        EGR 326 Lab 5
  Filename:     Part_I.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/02/2020
  Instructor:   Professor Brian Krug
  Description:  INIT of eUSCI as I2C Mode
  Note:
************************************************************************************************************
WIRING FOR MSP432
RTC: SCL - PX.X | SDA - PX.X | VCC - 3.3V | GND - GND |
***********************************************************************************************************/
#include "msp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SLAVEADD 0x00 //solder pads on A0, A1, and A2 on the module

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //configuration of eUSCI as I2C mode
    //this code was pulled and heavily commented from professor kandalofts slide with permissions
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;// disables the eUSCI0 while configuration
    //or holds in a reset state for configuration
    EUSCI_B0->CTLW0 |=  EUSCI_B_CTLW0_MODE_3| // this directly points to I2C mode
                        EUSCI_B_CTLW0_MST|//master mode select
                        EUSCI_B_CTLW0_SYNC;//enabling syncornus mode
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;       // Select the SMCLK for the module
    EUSCI_B0->BRW = 0x001E;      // Commands a divder (30) resulting in 100kHz
    EUSCI_B0->I2CSA = SLAVEADD; //slave address will be a macro pointing to the address of the RTC
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST; //enables eUSCI0 after config
}
