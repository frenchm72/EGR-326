/***********************************************************************************************************
    Title:                EGR 326 Lab 09
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French
    Date:               11/06/2020
    Instructor:     Professor Brian Krug
    Description:   Lab 10 -  Speed sensing using Hall effect sensors and with the MSP43
    Note:               ValvanoWare code was used as recommended by this lab. However a custom function was
  made to send letters easier. This was done by modifying a new function ST7735_DrawStringMod in the
  libraries. This was used in replace of ST7735_DrawString to generate a 'clear' background. The libraries
  were pulled from:http://users.ece.utexas.edu/~valvano/arm/downloadmsp432.html. A gvsulogo was
  used from web site
************************************************************************************************************
WIRING FOR MSP432:
LED                      :  P2.2 - BLUE|
Full Color LCD  :   SCK - P9.5  |   SDA - P9.7   |  LED - 3.3V |    A0 - P9.2   |   RST - P9.3   |
                                  VCC - 3.3V | GND - GND   | CS  - P9.4
************************************************************************************************************
Header files included:
***********************************************************************************************************/
#include <msp.h>
#include <image.h>
#include <ST7735.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/
#define CLEARX 100
#define CLEARY 25
#define STATUSX 30
#define STATUSY 80

#define LEDPORT P2
#define LEDPIN BIT2

int count = 0, time = 0;
bool timeout = false;

void initMSP(void);
void Clock_Init48MHz(void);
void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in microeconds using systick

void main(void){
    WDT_A->CTL = WDT_A_CTL_PW          |
                 WDT_A_CTL_SSEL__SMCLK |        // SMCLK as clock source
                 WDT_A_CTL_TMSEL       |        // Interval timer mode
                 WDT_A_CTL_CNTCL       |        // Clear WDT counter (initial value = 0)
                 WDT_A_CTL_IS_4;                // Watchdog interval = 32K = 2^15 ticks

    char countS[2];

    initMSP();

        while(1){
                    count++;
                    itoa(count, countS);
                    ST7735_DrawStringMod(STATUSX, STATUSY, countS, TXTCOLOR, BGCOLOR,  TXTSIZE);
//                    if(count == 4)
//                        count = 0;//resetWD
                    time = 0;
                    delay_ms(1000);
                }
    }

void initMSP(void){
        LEDPORT->SEL0 &= ~(LEDPIN);                  //setting up LED
        LEDPORT->SEL1 &= ~(LEDPIN);
        LEDPORT->DIR |= (LEDPIN);
        LEDPORT->OUT &= ~(LEDPIN); //turns ledsoff

        Clock_Init48MHz();
        SysTickInit();

//        Output_On();
//        ST7735_SetRotation(0);
//        ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);
        LEDPORT->OUT |= (LEDPIN); //turns ledsoff
        delay_ms(500);
        LEDPORT->OUT &= ~(LEDPIN); //turns ledsoff
        //delay_ms(2500);
        ST7735_InitR(INITR_GREENTAB);
        ST7735_FillScreen(BGCOLOR);
        ST7735_SetTextColor(TXTCOLOR);
        ST7735_DrawStringMod(STATUSX, STATUSY-(TXTSIZE*10), "Count", TXTCOLOR, BGCOLOR,  TXTSIZE);
        NVIC->ISER[0] = 1 << ((WDT_A_IRQn) & 31);
            __enable_irq();                             // Enable global interrupt
}
void WDT_A_IRQHandler(void){
    timeout = true;
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

void SysTickInit(void){
    SysTick -> CTRL = 0;//disable systick during setup
    SysTick -> LOAD = 0x00FFFFFF;//max reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000005;//enables systick 48MHz no interrupts
}
//delete later when interrupts work
void delay_ms(int ms){//will roll over at 349ms//delay in milliseconds using systick
if(ms <= 349){
SysTick->LOAD =(48000*ms)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}
else{
    do{
        SysTick->LOAD =(48000*345)-1;
        SysTick->VAL = 0;
        while((SysTick->CTRL & BIT(16))==0);
        ms = ms - 349;
    }while(ms > 349);
    SysTick->LOAD =(48000*ms)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}
}
