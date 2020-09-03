/**************************************************************************************************************
  Title:       EGR 326 Lab 1
  Filename:    EGR326-Lab_1_Part_I_RGBLED_BUT
  Author(s):   Mathew J. Yerian-French
  Date:        09/04/2020
  Instructor:  Professor Brian Krug
  Description: This program is made to interface with two buttons that will control a RGB LED.
  Note:
***************************************************************************************************************/
#include "LED.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initPins();

    uint8_t a;

    while(1)
   {
        a = butPres();
        delay_ms(5);
        switch(state)
        {
        case LED_OFF:                               //this if for the color black/off
            LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off
            if(a==1)
                state =LED_RED;
            break;
        case LED_RED:
            LEDPORT->OUT &= ~(GREENLED|BLUELED);
            LEDPORT->OUT |= (REDLED);
            if(a==1)
                state =LED_GREEN;
            break;
        case LED_GREEN:
            LEDPORT->OUT &= ~(REDLED|BLUELED);
            LEDPORT->OUT |= (GREENLED);                      //this turns LED GREEN
            if(a==1)
                state =LED_BLUE;
            break;
        case LED_BLUE:
            LEDPORT->OUT &= ~(REDLED|GREENLED);
            LEDPORT->OUT |= (BLUELED);                      //this turns LED BLUE
            if(a==1)
                state =LED_RED;
            break;
        default:
            state =LED_OFF;
            break;
        }
    }
}

