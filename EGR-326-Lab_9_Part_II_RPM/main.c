/***********************************************************************************************************
    Title:                EGR 326 Lab 09
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French
    Date:               10/30/2020
    Instructor:     Professor Brian Krug
    Description:   Lab 09 - Speed sensing using Hall effect sensors and with the MSP432
    Note:               ValvanoWare code was used as recommended by this lab. However a custom function was
  made to send letters easier. This was done by modifying a new function ST7735_DrawStringMod in the
  libraries. This was used in replace of ST7735_DrawString to generate a 'clear' background. The libraries
  were pulled from:http://users.ece.utexas.edu/~valvano/arm/downloadmsp432.html. A gvsulogo was
  used from web site
************************************************************************************************************
WIRING FOR MSP432:
Hall-Effect         :   Pin1 - 5V    |   Pin2 - GND  |   Pin3 - P6.6   |   Valid when Marks are facing you |
Full Color LCD  :   SCK - P9.5  |   SDA - P9.7   |  LED - 3.3V |    A0 - P9.2   |   RST - P9.3   |
                                  VCC - 3.3V | GND - GND   | CS  - P9.4
************************************************************************************************************
Header files included:
***********************************************************************************************************/
#include "msp.h"
#include "image.h"
#include "wstep.h"
#include <ST7735.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/
#define CLEARX 160
#define CLEARY 25
#define STATUSX 30
#define STATUSY 80

#define HALLPORT P6 //Timer_A2.3
#define HALLPIN BIT6

bool count = false, print = true;
uint16_t pulseWidth;

void initMSP(void);
void itoa(int n, char s[]);
void reverse(char s[]);
void Clock_Init48MHz(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    Clock_Init48MHz();

    char speed[10], rpm[10];
    int rpmP, SpeedP;

    initMSP();                   // set system clock to 48 MHz
    SysTickInit();
   // initWhiteStepper(); //Stepper motor initialize if inculded

    ST7735_InitR(INITR_GREENTAB);
    Output_On();
    ST7735_SetRotation(0);
    ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);

    delay_ms(3000);
    ST7735_FillScreen(BGCOLOR);
    ST7735_SetTextColor(TXTCOLOR);
    ST7735_DrawStringMod(STATUSX, STATUSY-(TXTSIZE*10), "Speed", TXTCOLOR, BGCOLOR,  TXTSIZE);
    ST7735_DrawStringMod(STATUSX+15, STATUSY-STATUSY+5, "RPM", TXTCOLOR, BGCOLOR,  TXTSIZE);
        while(1){
            //step(1, 1);//if stepper is included
            if(print){
            rpmP = (((1000*1000*60)/(2*pulseWidth*250))+rpmP)/2;//calc rpm 250us by the number of counts
            SpeedP = ((((rpmP*60)*37.699)/5280)+SpeedP)/2;// calc speed
            itoa(SpeedP, speed);
            itoa(rpmP, rpm);
           ST7735_FillRect(STATUSX+15, STATUSY-STATUSY+25, CLEARX, CLEARY, BGCOLOR);
           ST7735_DrawStringMod(STATUSX+15, STATUSY-STATUSY+25, rpm, TXTCOLOR, BGCOLOR,  TXTSIZE);
           ST7735_FillRect(STATUSX+20, STATUSY, CLEARX, CLEARY, BGCOLOR);
            ST7735_DrawStringMod(STATUSX+20, STATUSY, speed, TXTCOLOR, BGCOLOR,  TXTSIZE);
            print = false;
            }
        }
}

void initMSP(void){
      HALLPORT->SEL0 |= (HALLPIN); //TA2.CCI2A input capture pin, second function
      HALLPORT->SEL1 &= ~(HALLPIN);//TA2.CCI2A input capture pin, second function
      HALLPORT->DIR &= ~(HALLPIN);

      TIMER_A2->CTL |= 0b0000000111100010;
      TIMER_A2->CCTL[3] = 0b0100100100010000;

      NVIC->ISER[0] = 1 << ((TA2_N_IRQn) & 31); // Enable interrupt in NVIC vector
        __enable_irq ( );//enable global interrupt
}

void TA2_N_IRQHandler(void)
{
    if(TIMER_A2->CCTL[3] & (TIMER_A_CCTLN_CCIFG)){
    if(count){
        pulseWidth = TIMER_A2->CCR[3]; // Get current count
        count = false;
        print = true;
    }
    else{
        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        count = true;
    }
    }
    TIMER_A2->CCTL[3] &= ~(TIMER_A_CCTLN_CCIFG);    // Clear the interrupt flag
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

/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}
/* A utility function to reverse a string  */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
