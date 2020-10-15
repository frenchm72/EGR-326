/***********************************************************************************************************
  Title:        EGR 326 Lab 6
  Filename:     Part_II.c, ST7735.h, ST7735.c, image.h
  Author(s):    Mathew J. Yerian-French
  Date:         10/09/2020
  Instructor:   Professor Brian Krug
  Description:  Part II of Lab 6
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
#define TXTSIZE 2

void Clock_Init48MHz(void);

int main(void){
    char  Opt1[15] = "1-Set Time",
          Opt2[15] = "2-Adjust",
          Opt3[15] = "3-Run",
          Opt4[15] = "4-Unlock";
    const int col = 0;

      Clock_Init48MHz();                   // set system clock to 48 MHz
      ST7735_InitR(INITR_GREENTAB);
      Output_On();

      ST7735_SetRotation(0);
      ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);

      __delay_cycles(48000000*3);

      ST7735_SetTextColor(TXTCOLOR);
      ST7735_FillScreen(BGCOLOR);
      ST7735_DrawStringMod(col, 4, Opt1, TXTCOLOR, BGCOLOR, TXTSIZE);
      ST7735_DrawStringMod(col, 6, Opt2, TXTCOLOR, BGCOLOR, TXTSIZE);
      ST7735_DrawStringMod(col, 8, Opt3, TXTCOLOR, BGCOLOR, TXTSIZE);
      ST7735_DrawStringMod(col, 10, Opt4, TXTCOLOR, BGCOLOR, TXTSIZE);

      while(1);
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
