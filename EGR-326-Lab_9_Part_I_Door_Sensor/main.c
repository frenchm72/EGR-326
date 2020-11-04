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
#include <ST7735.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/
void Clock_Init48MHz(void);
#define CLEARX 128
#define CLEARY 40
#define STATUSX 10
#define STATUSY 25

#define HALLPORT P6 //Timer_A2.4
#define HALLPIN BIT6

bool Sense = false;
bool OCdirect;

void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    Clock_Init48MHz();                   // set system clock to 48 MHz
    ST7735_InitR(INITR_GREENTAB);
    Output_On();
    SysTickInit();

    ST7735_SetRotation(0);
    ST7735_DrawBitmap(0, 160, gvlogo, 128, 160);

	HALLPORT->SEL0 &= ~(HALLPIN); //setting up button as interrupt
	HALLPORT->SEL1 &= ~(HALLPIN);
	HALLPORT->DIR &= ~(HALLPIN);
	HALLPORT->REN |= (HALLPIN);   //enable resistor
	HALLPORT->OUT |= (HALLPIN);   //enable pull up
	HALLPORT->IES |= (HALLPIN);   //set pin as interrupt high to low
	HALLPORT->IE |= (HALLPIN);    //enable interrupt for Px.x
	HALLPORT->IFG &= ~(HALLPIN);  //clears interrupt flag

	 NVIC->ISER[1] = 1 << ((PORT6_IRQn) & 31);
	    __enable_interrupt();

	   // delay_ms(3000);
	    ST7735_FillScreen(BGCOLOR);
	    ST7735_SetTextColor(TXTCOLOR);
        ST7735_DrawStringMod(STATUSX, STATUSX, "Door", TXTCOLOR, BGCOLOR,  TXTSIZE);

	    while(1){
	        if(Sense){
	            if(OCdirect){//door closed
                    ST7735_FillRect(STATUSX, STATUSY, CLEARX, CLEARY, BGCOLOR);
                    ST7735_DrawStringMod(STATUSX, STATUSY, "Closed!", TXTCOLOR, BGCOLOR,  TXTSIZE);
	            }
	            else{// door open
                    ST7735_FillRect(STATUSX, STATUSY, CLEARX, CLEARY, BGCOLOR);
                    ST7735_DrawStringMod(STATUSX, STATUSY, "Open!", TXTCOLOR, BGCOLOR,  TXTSIZE);
	            }
	            Sense = false;
	        }
	    }
}

void PORT6_IRQHandler(void) {// port 6 interrupt handler for hall effect sensor
    if(HALLPORT->IFG & HALLPIN){
      if(!(HALLPORT->IN & HALLPIN)){// door is closed
          OCdirect = true;
          HALLPORT->IES &= ~(HALLPIN);   //set pin as interrupt high to low
      }
      else{//for door open
          OCdirect = false;
          HALLPORT->IES |= (HALLPIN);   //set pin as interrupt high to low
      }
    }
    Sense = true;
    HALLPORT->IFG &= ~(HALLPIN);
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

void SysTickInit(void)
{
SysTick -> CTRL = 0;//disable systick during setup
SysTick -> LOAD = 0x00FFFFFF;//max reload value
SysTick -> VAL = 0;//clears it
SysTick -> CTRL = 0x00000005;//enables systick 48MHz no interrupts
}

void delay_ms(int ms)//delay in milliseconds using systick
{//will roll over at 349ms
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
