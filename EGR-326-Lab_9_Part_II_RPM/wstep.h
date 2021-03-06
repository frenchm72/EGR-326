#ifndef __WSTEP_H__
#define __WSTEP_H__

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define IN1PORT P6
#define IN1PIN BIT4
#define IN2PORT P6
#define IN2PIN BIT5
#define IN3PORT P4
#define IN3PIN BIT6
#define IN4PORT P1
#define IN4PIN BIT5

#define SPEED 10

enum states{//count and corner sequence
    Step1, Step2,
    Step3, Step4
} state;

void initWhiteStepper(void);
void step(int steps, int direction);
void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in microeconds using systick

#endif
