/**************************************************************************************************************
  Title:        EGR 326 Lab 2
  Filename:     PARTIII.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/11/2020
  Instructor:   Professor Brian Krug
  Description:  This program is made to interface with two buttons that will control a RGB LED. The first
                button will decide which LED is toggling. Then holding the second button will toggle the LED
                selected on and off
  Note:
***************************************************************************************************************/
#include "PARTIII.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

    while(1)
   {
       RunLED();
       if(BUTPORT->IN & BUTPIN2)
           BUT2 = 0;
    }
}
