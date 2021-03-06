/**************************************************************************************************************
  Title:        EGR 326 Lab 2
  Filename:     Part_II.c
  Author(s):    Mathew J. Yerian-French
  Date:         09/11/2020
  Instructor:   Professor Brian Krug
  Description:  This program is made to interface with a button that will control a RGB LED. The button will
                decide how quickly the LED is Changing/toggling.
  Note:
***************************************************************************************************************/
#include "PARTII.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

    while(1)
   {
        if(BUT1)
        RunLED();
    }
}
