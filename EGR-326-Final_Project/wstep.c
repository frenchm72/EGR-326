#include "msp.h"
#include "wstep.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void initWhiteStepper(void){
    SIN1PORT->SEL0 &= ~(SIN1PIN);    //setting up In1
    SIN1PORT->SEL1 &= ~(SIN1PIN);
    SIN1PORT->DIR |= (SIN1PIN);
    SIN1PORT->OUT |= (SIN1PIN);

    SIN2PORT->SEL0 &= ~(SIN2PIN);    //setting up In2
    SIN2PORT->SEL1 &= ~(SIN2PIN);
    SIN2PORT->DIR |= (SIN2PIN);
    SIN2PORT->OUT |= (SIN2PIN);

    SIN3PORT->SEL0 &= ~(SIN3PIN);    //setting up In3
    SIN3PORT->SEL1 &= ~(SIN3PIN);
    SIN3PORT->DIR |= (SIN3PIN);
    SIN3PORT->OUT |= (SIN3PIN);

    SIN4PORT->SEL0 &= ~(SIN4PIN);    //setting up In4
    SIN4PORT->SEL1 &= ~(SIN4PIN);
    SIN4PORT->DIR |= (SIN4PIN);
    SIN4PORT->OUT |= (SIN4PIN);

    TIN1PORT->SEL0 &= ~(TIN1PIN);    //setting up In1
    TIN1PORT->SEL1 &= ~(TIN1PIN);
    TIN1PORT->DIR |= (TIN1PIN);
    TIN1PORT->OUT |= (TIN1PIN);

    TIN2PORT->SEL0 &= ~(TIN2PIN);    //setting up In2
    TIN2PORT->SEL1 &= ~(TIN2PIN);
    TIN2PORT->DIR |= (TIN2PIN);
    TIN2PORT->OUT |= (TIN2PIN);

    TIN3PORT->SEL0 &= ~(TIN3PIN);    //setting up In3
    TIN3PORT->SEL1 &= ~(TIN3PIN);
    TIN3PORT->DIR |= (TIN3PIN);
    TIN3PORT->OUT |= (TIN3PIN);

    TIN4PORT->SEL0 &= ~(TIN4PIN);    //setting up In4
    TIN4PORT->SEL1 &= ~(TIN4PIN);
    TIN4PORT->DIR |= (TIN4PIN);
    TIN4PORT->OUT |= (TIN4PIN);
}


void Sstep(int steps, int direction){//input is direction and number of steps
    int count = 0;
    state = stateSpeedHeld;
    //Comment out for half step mode
    //steps = steps * 2;//due to the half step method
//this is the half step method
while(1){
    WDT_A->CTL = 0x5A00      // Watchdog Password
                 | 1<<5                   //Set to ACLK
                 | 0<<4                   //Set to Watchdog mode
                 | 1<<3                   // Clear Timer
                 | 4;
    if(steps <= 80000){
    switch(state)
     {
         case Step1:
            if(count >= steps){stateSpeedHeld = state ; break;}
            SIN1PORT->OUT |= (SIN1PIN);
            SIN2PORT->OUT   |= (SIN2PIN);
            SIN3PORT->OUT  |= (SIN3PIN);
            SIN4PORT->OUT &= ~(SIN4PIN);
            delay_us(SPEED);
            count++;
            if(direction == 0){ state = Step2; break;}
            if(direction == 1){ state = Step4; break;}
         case Step2:
            if(count >= steps){stateSpeedHeld = state ; break;}
            SIN1PORT->OUT &= ~(SIN1PIN);
            SIN2PORT->OUT |= (SIN2PIN);
            SIN3PORT->OUT  |= (SIN3PIN);
            SIN4PORT->OUT  |=  (SIN4PIN);
            delay_us(SPEED);
            count++;
            if(direction == 0){ state = Step3; break;}
            if(direction == 1){ state = Step1; break;}
         case Step3:
             if(count >= steps){stateSpeedHeld = state ; break;}
             SIN1PORT->OUT  |= (SIN1PIN);
             SIN2PORT->OUT &= ~(SIN2PIN);
             SIN3PORT->OUT |= (SIN3PIN);
             SIN4PORT->OUT  |= (SIN4PIN);
             delay_us(SPEED);
             count++;
             if(direction == 0){ state = Step4; break;}
             if(direction == 1){ state = Step2; break;}
         case Step4:
             if(count >= steps){stateSpeedHeld = state ; break;}
             SIN1PORT->OUT  |= (SIN1PIN);
             SIN2PORT->OUT  |= (SIN2PIN);
             SIN3PORT->OUT &= ~(SIN3PIN);
             SIN4PORT->OUT |= (SIN4PIN);
             delay_us(SPEED);
             count++;
             if(direction == 0){ state = Step1; break;}
             if(direction == 1){ state = Step3; break;}
         default:
             if(count >= steps){stateSpeedHeld = state ; break;}
             state = Step1;
     }
    }
    else break;
    if(count >= steps) break;
}
}

void Tstep(int steps, int direction){//input is direction and number of steps
    int count = 0;
    state = stateTempHeld;
    //Comment out for half step mode
   // steps = steps * 2;//due to the half step method
//this is the half step method
while(1){
    WDT_A->CTL = 0x5A00      // Watchdog Password
                 | 1<<5                   //Set to ACLK
                 | 0<<4                   //Set to Watchdog mode
                 | 1<<3                   // Clear Timer
                 | 4;
    if(steps <= 80000){
    switch(state)
     {
         case Step1:
            if(count >= steps){stateTempHeld = state ; break;}
            TIN1PORT->OUT |= (TIN1PIN);
            TIN2PORT->OUT |= (TIN2PIN);
            TIN3PORT->OUT&= ~(TIN3PIN);
            TIN4PORT->OUT &= ~(TIN4PIN);
            delay_us(SPEED);
            count++;
            if(direction == 0){ state = Step2; break;}
            if(direction == 1){ state = Step4; break;}
         case Step2:
            if(count >= steps){stateTempHeld = state ; break;}
            TIN1PORT->OUT &= ~(TIN1PIN);
            TIN2PORT->OUT |= (TIN2PIN);
            TIN3PORT->OUT |= (TIN3PIN);
            TIN4PORT->OUT &= ~ (TIN4PIN);
            delay_us(SPEED);
            count++;
            if(direction == 0){ state = Step3; break;}
            if(direction == 1){ state = Step1; break;}
         case Step3:
             if(count >= steps){stateTempHeld = state ; break;}
             TIN1PORT->OUT &= ~ (TIN1PIN);
             TIN2PORT->OUT &= ~(TIN2PIN);
             TIN3PORT->OUT |= (TIN3PIN);
             TIN4PORT->OUT |= (TIN4PIN);
             delay_us(SPEED);
             count++;
             if(direction == 0){ state = Step4; break;}
             if(direction == 1){ state = Step2; break;}
         case Step4:
             if(count >= steps){stateTempHeld = state ; break;}
             TIN1PORT->OUT  |= (TIN1PIN);
             TIN2PORT->OUT &= ~(TIN2PIN);
             TIN3PORT->OUT &= ~(TIN3PIN);
             TIN4PORT->OUT |= (TIN4PIN);
             delay_us(SPEED);
             count++;
             if(direction == 0){ state = Step1; break;}
             if(direction == 1){ state = Step3; break;}
         default:
             if(count >= steps){stateTempHeld = state ; break;}
             state = Step1;
     }
    }
    else {stateTempHeld = state ; break;}
    if(count >= steps){stateTempHeld = state ; break;}
}
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

void delay_us(int us)//delay in microeconds using systick
{//will roll over at 349ms
if(us <= 349000){
SysTick->LOAD =(48000*us)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}
else{
    do{
        SysTick->LOAD =(48000*345)-1;
        SysTick->VAL = 0;
        while((SysTick->CTRL & BIT(16))==0);
        us = us - 349000;
    }while(us > 349000);
    SysTick->LOAD =(48000*us)-1;
    SysTick->VAL = 0;
    while((SysTick->CTRL & BIT(16))==0);
}
}
