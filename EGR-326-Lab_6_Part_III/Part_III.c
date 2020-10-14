/***********************************************************************************************************
  Title:        EGR 326 Lab 6
  Filename:     Part_III.c, ST7735.h, ST7735.c
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
Full Color LCD: SCK - P9.5 | SDA - P9.7 | LED - 3.3V | A0 - P9.2 | VCC - 3.3V | GND - GND
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

void Clock_Init48MHz(void);
char* itoa(int value, char* buffer, int base);
char* reverse(char *buffer, int i, int j);
inline void swap(char *x, char *y);

int main(void){
    int i = 0;
    char countC[10];
      Clock_Init48MHz();                   // set system clock to 48 MHz
      ST7735_InitR(INITR_GREENTAB);
      Output_On();

      ST7735_SetRotation(0);
      ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);

      __delay_cycles(48000000*3);

      ST7735_SetTextColor(TXTCOLOR);
      ST7735_FillScreen(BGCOLOR);

      while(1){
      for(i=0;i<10;i++){
          atoi(i, countC, 10);
          ST7735_DrawStringMod(2, 1, countC, TXTCOLOR, BGCOLOR, TXTSIZE);
          __delay_cycles(48000000);
      }
      }
}


char* itoa(int value, char* buffer, int base){
    // invalid input
    if (base < 2 || base > 32)
        return buffer;

    // consider absolute value of number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10)
            buffer[i++] = 65 + (r - 10);
        else
            buffer[i++] = 48 + r;

        n = n / base;
    }

    // if number is 0
    if (i == 0)
        buffer[i++] = '0';

    // If base is 10 and value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10)
        buffer[i++] = '-';

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}
// inline function to swap two numbers
inline void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j){
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);

    return buffer;
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
