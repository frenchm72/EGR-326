/**************************************************************************************************************
  Title:        EGR 326 Lab 2
  Filename:     Part_I.c
  Author(s):    Mathew J. Yerian-French
  Date:         09/11/2020
  Instructor:   Professor Brian Krug
  Description:  This program is made to interface with two buttons that will control a Blue LED. The Two
                buttons will decide how quickly the LED is toggling.
  Note:
***************************************************************************************************************/
#include "PARTI.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

    while(1)
   {
        if(BUT1 | BUT2)
            FreqChange();
        if(LED)
        {
            LEDPORT->OUT ^= (BLUELED);
            delay_ms(((((1.0/Freq))*1000.0)/2.0));
        }
        else
            LEDPORT->OUT &= ~(BLUELED);
   }
}
