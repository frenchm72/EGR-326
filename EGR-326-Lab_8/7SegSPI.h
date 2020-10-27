#ifndef __7SegSPI_H__
#define __7SegSPI_H__

#define CSPORT P5 //macros for the 8 digit 7 seg SPI protocol
#define CSPIN BIT0
#define DINPORT P1
#define DINPIN BIT6
#define CLKPORT P1
#define CLKPIN BIT7

void initSPI(void); // init SPI
void sendSPI(int Command, int Data); //send the command and data over SPI

void SysTickInit(void);// init SysTick
void delay_ms(int ms);//delay in milliseconds using SysTick
void delay_us(int us);//delay in microseconds using SysTick

#endif
