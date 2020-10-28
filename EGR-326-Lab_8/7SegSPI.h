#ifndef __7SegSPI_H__
#define __7SegSPI_H__

#define CSPORT P5 //macros for the 8 digit 7 seg SPI protocol
#define CSPIN BIT0
#define DINPORT P1
#define DINPIN BIT6
#define CLKPORT P1
#define CLKPIN BIT5

#define COMMANDDELAY 100

#define DECODE 0x09 // procedure descriptions given by professor Krug
#define DECODE_MODE 0xFF //decode setting
#define INTENSITY 0x0A //intensity address
#define INTENSITY_MODE 0x03 //intensity setting
#define SCAN_LIMIT 0x0B // scan address
#define SCAN_LIMIT_MODE 0x07 //scan setting
#define SHUTDOWN 0x0C //shutdown address
#define SHUTDOWN_MODE 0x01 // shutdown setting normal operation
#define DIS_TEST 0x0F//display address
#define DIS_TEST_MODE 0x01 // test mode setting

#define DIGIT_0 0x01 // macro for digit addresses
#define DIGIT_1 0x02
#define DIGIT_2 0x03
#define DIGIT_3 0x04
#define DIGIT_4 0x05
#define DIGIT_5 0x06
#define DIGIT_6 0x07
#define DIGIT_7 0x08

void initSPI(void); // init SPI
void sendSPI(uint8_t Command, uint8_t Data); //send the command and data over SPI

void SysTickInit(void);// init SysTick
void delay_ms(int ms);//delay in milliseconds using SysTick
void delay_us(int us);//delay in microseconds using SysTick

#endif
