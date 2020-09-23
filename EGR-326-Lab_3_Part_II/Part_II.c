/***********************************************************************************************************
  Title:        EGR 326 Lab 3
  Filename:     Part_II.C
  Author(s):    Mathew J. Yerian-French
  Date:         09/18/2020
  Instructor:   Professor Brian Krug
  Description:
  Note:
************************************************************************************************************
WIRING FOR MSP432
Sonar Sensor: GND - GND | ECHO_VoltDiv - P6.4 | TRIG_MosFet - P4.0 | VCC - 5V |
Blue LED: Anode - 5V | Cathode - Resistor - MosFet - GND | P2.4 - MosFet|
***********************************************************************************************************/
#include "PARTII.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

    while(1)
   {

    }
}
