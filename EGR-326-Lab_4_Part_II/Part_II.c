/***********************************************************************************************************
  Title:        EGR 326 Lab 4
  Filename:     Part_II.c, encoder.c, encoder.h
  Author(s):    Mathew J. Yerian-French
  Date:         09/25/2020
  Instructor:   Professor Brian Krug
  Description:  This code is used to interface a rotary encoder with the MSP432 using interrupts
  Note:
************************************************************************************************************
WIRING FOR MSP432
Encoder: + - 3.3V | GND - GND | DT - P5.2 // Cap | CLK = P5.1 // Cap| SW = P5.0 |
***********************************************************************************************************/
#include "msp.h"
#include "encoder.h"

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	rotorInit();
	SysTickInit();

    NVIC->ISER[1] = 1 << ((PORT6_IRQn) & 31);
    __enable_interrupt();

    while(1)
    {
        if(SWflag)
        {
            if((CCWcount-CWcount)>0)
                printf("\nAbsolute: CCW-%d\n", abs(CCWcount-CWcount));
            else if((CCWcount-CWcount)<0)
                printf("\nAbsolute: CW-%d\n", abs(CCWcount-CWcount));
            else
                printf("\nNet is Zero\n");
            CWcount = 0;
            CCWcount = 0;
            SWflag = 0;
        }
    }
}
