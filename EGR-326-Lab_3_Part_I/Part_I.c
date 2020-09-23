/***********************************************************************************************************
  Title:        EGR 326 Lab 3
  Filename:     Part_I.C
  Author(s):    Mathew J. Yerian-French
  Date:         09/18/2020
  Instructor:   Professor Brian Krug
  Description:
  Note:
************************************************************************************************************
WIRING FOR MSP432
Sonar Sensor: GND - GND | ECHO_VoltDiv - P2.5 | TRIG_MosFet - P4.0 | VCC = 5V |
***********************************************************************************************************/
#include "PARTI.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();

    while(1)
   {
        TRIGPORT->OUT &= ~(TRIGPIN);
        delay_us(2);
        TRIGPORT->OUT |= (TRIGPIN);

        delay_ms(2000);
        printf("\nInches:%f\t\tCentimeters:%f", distIN, distCM);
    }
}
