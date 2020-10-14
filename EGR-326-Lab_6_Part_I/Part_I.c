/***********************************************************************************************************
  Title:        EGR 326 Lab 5
  Filename:     Part_I.c, ST7735.h, ST7735.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/09/2020
  Instructor:   Professor Brian Krug
  Description:  Part I of Lab 6
  Note:
************************************************************************************************************
WIRING FOR MSP432
Full Color LCD: SCK - P9.5 | SDA - P9.7 | LED - 3.3V | A0 - P9.2 | VCC - 3.3V | GND - GND
***********************************************************************************************************/
#include "msp.h"
#include <ST7735.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void Clock_Init48MHz(void);

#define BGCOLOR ST7735_Color565(0, 0, 0)//black
#define TXTCOLOR ST7735_Color565(0, 255, 255) //white
#define TXTSIZE 1

int main(void){
  char display[100] = "EGR-326";
  int length = strlen(display);
  uint8_t i, j, bounce = 0, bounce2 = 0;
  Clock_Init48MHz();                   // set system clock to 48 MHz
  ST7735_InitR(INITR_GREENTAB);
  //Output_On();
  //Output_Clear();
  ST7735_SetTextColor(TXTCOLOR);
  //ST7735_DrawStringMod(2, 7, display, TXTCOLOR, BGCOLOR, 2);
  ST7735_FillScreen(BGCOLOR);//ST7735_FillScreen(ST7735_BLUE);
  while(1){
      //shifts the text
      ST7735_DrawStringMod(j, i, display, TXTCOLOR, BGCOLOR, TXTSIZE);
      __delay_cycles(20833333);
      //this erases just the text so it doesnt take as long
      ST7735_FillRect(j*6*TXTSIZE, i*10, length*6*TXTSIZE, TXTSIZE*10, BGCOLOR);
      if(j >= 14)// if size is 2 then 4 if 1 then 14
          bounce = 1;
      else if(j == 0)
          bounce = 0;
      if(bounce)
          j--;
      else
          j++;
    if(i >= 15)//if size 2 then 14 if size 1 then 15
        bounce2 = 1;
    else if(i == 0)
        bounce2 = 0;
    if(bounce2)
        i--;
    else
        i++;
  }
}



void Clock_Init48MHz(void)
{
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
