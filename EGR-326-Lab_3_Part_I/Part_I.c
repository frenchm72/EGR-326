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
Sonar Sensor: GND - GND | ECHO_VoltDiv - P2.4 | TRIG_MosFet - P5.6 | VCC = 5V |
***********************************************************************************************************/
#include "PARTI.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();

    while(1)
   {
        TRIGPORT->OUT &= ~(TRIGPIN);
        delay_us(10);
        TRIGPORT->OUT |= (TRIGPIN);

        delay_ms(2000);
        distCM = (pulseWidth/3.0) / 58.0; //to find distance knowing speed of sound is 340m/s
        distIN = (pulseWidth/3.0) / 148.0;//find distance in inches
        printf("\nInches:%g\t\tCentimeters:%g", distIN, distCM);
    }
}
