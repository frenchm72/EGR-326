#ifndef __ENCODER_H__
#define __ENCODER_H__

int CWcount, CCWcount, SWflag;


#define CLKPORT P6  //defines keypins for rows
#define CLKPIN BIT0
#define DTPORT P6
#define DTPIN BIT4
#define SWPORT P6
#define SWPIN BIT1

void rotorInit(void);
void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in milliseconds using systick

#endif
