/***********************************************************************************************************
    Title:                EGR 326 Lab 09
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French & Luke Metz
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
#define BUT P1
#define S1 BIT1

int count = 0, time = 0;
bool timeout = false;

void initMSP(void);
void Clock_Init48MHz(void);
void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in microeconds using systick
void countW(void);

int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

        initMSP();

          while(1){
              countW();                     //count from 1 - 4 on LCD
          }
}


void initMSP(void){
        LEDPORT->SEL0 &= ~(LEDPIN);                  //setting up LED
        LEDPORT->SEL1 &= ~(LEDPIN);
        LEDPORT->DIR |= (LEDPIN);
        LEDPORT->OUT &= ~(LEDPIN); //turns ledsoff

        BUT->SEL0 &=~ (S1);    //S1 as GPIO output
        BUT->SEL1 &=~ (S1);
        BUT->DIR &= ~(S1);    //s1 and s1 for interrupts
        BUT->REN = (S1);
        BUT->OUT = (S1);
        BUT->IE = (S1);
        BUT->IES = (S1);      //falling edge
        BUT->IFG = 0x00;

        Clock_Init48MHz();
        SysTickInit();

        ST7735_InitR(INITR_GREENTAB);
//        Output_On();
//        ST7735_SetRotation(0);
//        ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);
        LEDPORT->OUT |= (LEDPIN); //turns ledsoff
        delay_ms(500);
        LEDPORT->OUT &= ~(LEDPIN); //turns ledsoff
//        delay_ms(2500);
        ST7735_FillScreen(BGCOLOR);
        ST7735_SetTextColor(TXTCOLOR);
        ST7735_DrawStringMod(STATUSX, STATUSY-(TXTSIZE*10), "Count", TXTCOLOR, BGCOLOR,  TXTSIZE);

        NVIC_EnableIRQ(PORT1_IRQn);
        __enable_irq();

        WDT_A->CTL = 0x5A00      // Watchdog Password
             | 1<<5                   //Set to ACLK
             | 0<<4                   //Set to Watchdog mode
             | 1<<3                   // Clear Timer
             | 4;
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

    CS->KEY = 0x0000695A;                            //password for CS registers
    CS->CTL1 |= 0b00000010000000000000000000000000;   //dividing ACLCK by 4
    CS->KEY = 0;

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}

void SysTickInit(void){
SysTick -> CTRL = 0;//disable systick during setup
SysTick -> LOAD = 0x00FFFFFF;//max reload value
SysTick -> VAL = 0;//clears it
SysTick -> CTRL = 0x00000005;//enables systick 48MHz no interrupts
}
void SysTick_Handler(void){
    timeout = true;
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
void PORT1_IRQHandler(void)
{
    if (BUT->IFG & S1)           //if S1 pressed set flag high
    {
        WDT_A->CTL = 0x5A00      // Watchdog Password
        | 1<<5                   //Set to ACLK
        | 0<<4                   //Set to Watchdog mode
        | 1<<3                   // Clear Timer
        | 4;
    }//Set to 2^15 interval (1 second but 4 seconds with ACLCK divider)
    BUT->IFG = 0x00;             //clear interrupt flags
}

void countW()
{
    int i, num=0;
    char numS[10];
    //for loop that prints 0-9 with 1 second interval in middle of screen
    for(i=0; i<100; i++){
        itoa(num+i, numS);
        ST7735_DrawStringMod(STATUSX, STATUSY, numS, TXTCOLOR, BGCOLOR,  TXTSIZE);
        delay_ms(1000);
    }
}
