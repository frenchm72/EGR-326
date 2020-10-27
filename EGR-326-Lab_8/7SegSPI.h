#ifndef __7SegSPI_H__
#define __7SegSPI_H__

#define CSPORT P5
#define CSPIN BIT0
#define DINPORT P1
#define DINPIN BIT6
#define CLKPORT P1
#define CLKPIN BIT7

void initSPI(void);
void sendSPI(int Command, int Data);

void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in microseconds using systick

#endif
