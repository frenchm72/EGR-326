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

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31); // Enable interrupt in NVIC vector
    __enable_irq ( );//enable global interrupt

    while(1)
   {
        TRIGPORT->OUT |= (TRIGPIN);
        delay_us(10);
        TRIGPORT->OUT &= ~(TRIGPIN);

        delay_ms(500);
        printf("Inches:%f\tCentimeters:%f\n", distIN, distCM);
    }
}
