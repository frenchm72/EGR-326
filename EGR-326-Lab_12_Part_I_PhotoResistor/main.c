/***********************************************************************************************************
    Title:                EGR 326 Lab 12
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French
    Date:               11/20/2020
    Instructor:     Professor Brian Krug
    Description:   Lab 12 -  Light Sensing with photo diode
    Note:
************************************************************************************************************
WIRING FOR MSP432:
Photo Diode :   3.3 - Diode - P5.5 - 10kohm - GND |
LED  :  Gate of MOS - P 5.6 (TA2.1)  |  GND - Source - Drain - 120ohm - LED - 5V | |
************************************************************************************************************
Header files included:
***********************************************************************************************************/
#include <msp.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/

 #define ADC_PORT P5
#define ADC_PIN BIT5
#define ADC_INST 0
#define LEDPORT P5
#define LEDPIN BIT6 //timer A2.1
#define PERIOD (10000-1)
void ADC14init(void);
void SysTickInit(void);

bool timeout = false;

       uint32_t adc_input;
       float readVoltage, refV = 3.3;
       float maxV = 2.5, minV = 1.5; //values from my room that will change automaticly with conditions found
       float percentage = 0;
       int count = 0;

void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

       __disable_irq();
       SysTickInit();
       ADC14init();

       LEDPORT->SEL0 |= (LEDPIN); //TA2.CCI2A input capture pin, second function
       LEDPORT->SEL1 &= ~(LEDPIN);
       LEDPORT->DIR |= (LEDPIN);

       TIMER_A2->CCR[0]= PERIOD;                   // CCR1 PWM period for LED
       TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set
       TIMER_A2->CCR[1] = percentage;                     // CCR1 PWM duty cycle
       TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK |TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

       NVIC -> ISER[0] |= 1 << ADC14_IRQn;
       __enable_irq();

       while(1){
           if(timeout){
               ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion
               adc_input = ADC14->MEM[0];
               readVoltage = refV * ((adc_input + 0.5)/pow(2,14));
               if(readVoltage > maxV){//this allows the program to adjust on the fly for max intensity
                   maxV = readVoltage;
                   count = 0;
               }
               if(readVoltage < minV){
                   minV = readVoltage;
                   count = 0;
               }
               percentage = ((readVoltage - minV)/(maxV - minV)) ;
               TIMER_A2->CCR[1] = PERIOD*percentage;//a percentage of the max voltage seen
               count++;
               if(count >= 60000){//starts decreasing/increassing the max min every 60 seconds in case the room hasn't changed to get the best representation
                   count = 0;
                   maxV = maxV - 0.25;
                   minV = minV + 0.25;
               }
               timeout = false;
               }
       }
}

void ADC14init(void){//pulled from my 226
    ADC_PORT -> SEL0 |= ADC_PIN;//set adc conversion
    ADC_PORT -> SEL1 |= ADC_PIN;
    ADC14 -> CTL0 &= ~ADC14_CTL0_ENC;//enable conersion
    ADC14 -> CTL0 |= ADC14_CTL0_PDIV__32 |//setpdiv to 32
                     ADC14_CTL0_SHP |//hold off sampling timer
                     ADC14_CTL0_SSEL__SMCLK |//sub master clock
                     ADC14_CTL0_SHT1__32 |//hol regisert for 32 clock
                     ADC14_CTL0_ON;//turns on
    ADC14 -> CTL1 |= ADC14_CTL1_RES__14BIT;//10 but resolution
    ADC14 -> MCTL[ADC_INST] = 0;//defult config for adc
    ADC14 -> IER0;//interrupt on
    ADC14 -> CTL0 |= ADC14_CTL0_ENC;//enable conversion
}

void SysTickInit(void){
    SysTick -> CTRL = 0;//disable systick during step
    SysTick -> LOAD = 3000;//1 msec reload value
    SysTick -> VAL = 0;//clears it
    SysTick -> CTRL = 0x00000007;//enables systick 3MHz interrupts
}
void SysTick_Handler(void){
    timeout = true;
}
