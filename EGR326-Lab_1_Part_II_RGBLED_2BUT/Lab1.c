/**************************************************************************************************************
  Title:       EGR 326 Lab 1
  Filename:    EGR326-Lab_1_Part_II_RGBLED_2BUT
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

    uint8_t a, b;

    while(1)
   {
        a = butPres();
        b = butPres2();
        delay_ms(5);
        switch(state)
        {
        case LED_OFF:                               //this if for the color black/off
            LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off
            if(a==1)
                state =LED_RED;
            else if(b==1)
                state =LED_BLUE;
            else if((a==b)&((a+b)!=0))
                state =LED_OFF;
            break;
        case LED_RED:
            LEDPORT->OUT &= ~(GREENLED|BLUELED);
            LEDPORT->OUT |= (REDLED);
            if(a==1)
                state =LED_GREEN;
            else if(b==1)
                state =LED_BLUE;
            else if((a==b)&((a+b)!=0))
                state =LED_OFF;
            break;
        case LED_GREEN:
            LEDPORT->OUT &= ~(REDLED|BLUELED);
            LEDPORT->OUT |= (GREENLED);                      //this turns LED GREEN
            if(a==1)
                state =LED_BLUE;
            else if(b==1)
                state =LED_RED;
            else if((a==b)&((a+b)!=0))
                state =LED_OFF;
            break;
        case LED_BLUE:
            LEDPORT->OUT &= ~(REDLED|GREENLED);
            LEDPORT->OUT |= (BLUELED);                      //this turns LED BLUE
            if(a==1)
                state =LED_RED;
            else if(b==1)
                state =LED_GREEN;
            else if((a==b)&((a+b)!=0))
                state =LED_OFF;
            break;
        default:
            state =LED_OFF;
            break;
        }
   }
}

