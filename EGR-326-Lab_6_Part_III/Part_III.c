/***********************************************************************************************************
  Title:        EGR 326 Lab 6
  Filename:     Part_III.c, ST7735.h, ST7735.c, image.h
  Author(s):    Mathew J. Yerian-French
  Date:         10/09/2020
  Instructor:   Professor Brian Krug
  Description:  Part III of Lab 6
  Note:         ValvanoWare code was used as recommended by this lab. However a custom function was made to
  send letters easier. This was done by modifying a new function ST7735_DrawStringMod in the libraries.
  This was used in replace of ST7735_DrawString to generate a 'clear' background. The libraries were pulled
  from:http://users.ece.utexas.edu/~valvano/arm/downloadmsp432.html. A gvsulogo was used from web site
************************************************************************************************************
WIRING FOR MSP432
Full Color LCD: SCK - P9.5 | SDA - P9.7 | LED - 3.3V | A0 - P9.2 | RST - P9.3 |
                VCC - 3.3V | GND - GND  | CS  - P9.4
***********************************************************************************************************/
#include "msp.h"
#include "image.h"
#include <ST7735.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define BGCOLOR ST7735_Color565(0, 0, 0)//black
#define TXTCOLOR ST7735_Color565(0, 0, 255) //white
#define TXTSIZE 4

#define rightwall 106
#define leftwall 2
#define top 2
#define bot 130

#define centerX 55
#define centerY 65

void Clock_Init48MHz(void);

enum states{//count and corner sequence
    zero, one, two,
    three, four, five,
    six, seven,
    eight, nine
} state;

int main(void){
      Clock_Init48MHz();                   // set system clock to 48 MHz
      ST7735_InitR(INITR_GREENTAB);
      Output_On();

      ST7735_SetRotation(0);
      ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);

      __delay_cycles(48000000*3);

//      ST7735_SetTextColor(TXTCOLOR);
      ST7735_FillScreen(BGCOLOR);
      while(1){
          switch(state)
              {
              case zero:
                  ST7735_DrawStringMod(leftwall, top, "0", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(leftwall, top, 2*40, 2*40, BGCOLOR);
                  state = one;
              case one:
                  ST7735_DrawStringMod(rightwall+4, bot, "1", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(rightwall+4, bot, 2*40, 2*40, BGCOLOR);
                  state = two;
              case two:
                  ST7735_DrawStringMod(leftwall, bot, "2", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(leftwall, bot, 2*40, 2*40, BGCOLOR);
                  state = three;
              case three:
                  ST7735_DrawStringMod(rightwall, top, "3", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(rightwall, top, 2*40, 2*40, BGCOLOR);
                  state = four;
              case four:
                  ST7735_DrawStringMod(leftwall, top, "4", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(leftwall, top, 2*40, 2*40, BGCOLOR);
                  state = five;
              case five:
                  ST7735_DrawStringMod(rightwall, bot, "5", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(rightwall, bot, 2*40, 2*40, BGCOLOR);
                  state = six;
              case six:
                  ST7735_DrawStringMod(leftwall, bot, "6", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(leftwall, bot, 2*40, 2*40, BGCOLOR);
                  state = seven;
              case seven:
                  ST7735_DrawStringMod(rightwall, top, "7", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(rightwall, top, 2*40, 2*40, BGCOLOR);
                  state = eight;
              case eight:
                  ST7735_DrawStringMod(leftwall, top, "8", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000);
                  ST7735_FillRect(leftwall, top, 2*40, 2*40, BGCOLOR);
                  state = nine;
              case nine:
                  ST7735_DrawStringMod(centerX, centerY, "9", TXTCOLOR, BGCOLOR, TXTSIZE);
                  __delay_cycles(48000000*3);
                  ST7735_FillRect(centerX, centerY, 2*40, 2*40, BGCOLOR);
                  state = zero;
              default:
                  state = zero;
                  break;
              }
      }
}

void Clock_Init48MHz(void){
    // Configure Flash wait-state to 1 for both banks 0 & 1
       FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK0_RDCTL_WAIT_1;
       FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK1_RDCTL_WAIT_1;

    //Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*
       PJ->SEL0 |= BIT2 | BIT3;                     // Configure PJ.2/3 for HFXT function
       PJ->SEL1 &= ~(BIT2 | BIT3);
       CS->KEY = CS_KEY_VAL ;                       // Unlock CS module for register access
       CS->CTL2 |= CS_CTL2_HFXT_EN | CS_CTL2_HFXTFREQ_6 | CS_CTL2_HFXTDRIVE;
          while(CS->IFG & CS_IFG_HFXTIFG)
                    CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
      CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK     |
                              CS_CTL1_DIVM_MASK     |
                              CS_CTL1_SELS_MASK     |
                              CS_CTL1_DIVHS_MASK)   |
                              CS_CTL1_SELM__HFXTCLK |
                              CS_CTL1_SELS__HFXTCLK;

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_2;    // change the SMCLK clock speed to 12 MHz.

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}
