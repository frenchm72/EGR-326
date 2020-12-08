#ifndef __WSTEP_H__
#define __WSTEP_H__

#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SIN1PORT P10
#define SIN1PIN BIT0
#define SIN2PORT P10
#define SIN2PIN BIT1
#define SIN3PORT P10
#define SIN3PIN BIT2
#define SIN4PORT P10
#define SIN4PIN BIT3

#define TIN1PORT P8
#define TIN1PIN BIT4
#define TIN2PORT P8
#define TIN2PIN BIT5
#define TIN3PORT P8
#define TIN3PIN BIT6
#define TIN4PORT P8
#define TIN4PIN BIT7

#define SPEED 10

#define MAXS 520
#define MAXT 12

float speedPos, tempPos, AbsSpeed;

enum states{//count and corner sequence
    Step1, Step2,
    Step3, Step4
} state;

int stateSpeedHeld, stateTempHeld;

void initWhiteStepper(void);
void Sstep(int steps, int direction);
void Tstep(int steps, int direction);
void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in microeconds using systick

#endif
