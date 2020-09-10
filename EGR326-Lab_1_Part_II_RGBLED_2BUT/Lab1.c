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

    int count = 0;

    while(1)
   {
        a = butPres();
        b = butPres2();
        if(a)
        {
            while(1)
            {
                b = butPres2();
                count++;
                if(count == CHECK | b)
                {
                    count = 0;
                    break;
                }
            }
        }
        else if(b)
        {
            while(1)
            {
                a = butPres();
                count++;
                if(count == CHECK | a)
                {
                    count = 0;
                    break;
                }
            }
        }
        switch(state)
        {
        case LED_OFF:                               //this if for the color black/off
            LEDPORT->OUT &= ~(REDLED|GREENLED|BLUELED); //turns leds off
            if((a+b)==2)
                state =LED_OFF;
            else if(a==1)
                state =LED_GREEN;
            else if(b==1)
                state =LED_BLUE;
            break;
        case LED_RED:
            LEDPORT->OUT &= ~(GREENLED|BLUELED);
            LEDPORT->OUT |= (REDLED);
            if((a+b)==2)
                state =LED_OFF;
            else if(a==1)
                state =LED_GREEN;
            else if(b==1)
                state =LED_BLUE;
            break;
        case LED_GREEN:
            LEDPORT->OUT &= ~(REDLED|BLUELED);
            LEDPORT->OUT |= (GREENLED);                      //this turns LED GREEN
            if((a+b)==2)
                state =LED_OFF;
            else if(a==1)
                state =LED_BLUE;
            else if(b==1)
                state =LED_RED;
            break;
        case LED_BLUE:
            LEDPORT->OUT &= ~(REDLED|GREENLED);
            LEDPORT->OUT |= (BLUELED);                      //this turns LED BLUE
            if((a+b)==2)
                state =LED_OFF;
            else if(a==1)
                state =LED_RED;
            else if(b==1)
                state =LED_GREEN;
            break;
        default:
            state =LED_OFF;
            break;
        }
   }
}

