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
Sonar Sensor: GND - GND | ECHO_VoltDiv - P2.4 | TRIG_MosFet - P2.6 | VCC = 5V |
LED Wiring: GND - GND | MosFet - 5.6|
***********************************************************************************************************/
#include "PARTII.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();

    while(1)
   {
        TRIGPORT->OUT &= ~(TRIGPIN);
        delay_us(10);
        TRIGPORT->OUT |= (TRIGPIN);

        distCM = (pulseWidth/3.0) / 58.0; //to find distance knowing speed of sound is 340m/s
        distIN = (pulseWidth/3.0) / 148.0;//find distance in inches

        if(distIN <= 1.00)
            {
            TIMER_A2->CCR[1] = 1000-1;
            delay_ms(250);
            TIMER_A2->CCR[1] = 0;
            delay_ms(250);
            }
        else if(distIN >= 10.00) TIMER_A2->CCR[1] = 0;
        else TIMER_A2->CCR[1] = 1000-(distIN*100-1);
    }
}
