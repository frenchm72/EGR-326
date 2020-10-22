#ifndef __KEYPAD_H__
#define __KEYPAD_H__

int keyPressed;

#define R0_BANK P2  //defines keypins for rows
#define R0_PORT BIT7
#define R1_BANK P2
#define R1_PORT BIT6
#define R2_BANK P2
#define R2_PORT BIT4
#define R3_BANK P2
#define R3_PORT BIT5

#define C0_BANK P3 //defines keypins for columns
#define C0_PORT BIT0
#define C1_BANK P5
#define C1_PORT BIT6
#define C2_BANK P5
#define C2_PORT BIT7

#define COL0_ON        C0_BANK->DIR |=  C0_PORT; C0_BANK->OUT |=  C0_PORT   //to turn on column 0
#define COL0_OFF       C0_BANK->DIR &= ~C0_PORT; C0_BANK->OUT &= ~C0_PORT   //to turn off column 0
#define COL1_ON        C1_BANK->DIR |=  C1_PORT; C1_BANK->OUT |=  C1_PORT   //to turn on column 1
#define COL1_OFF       C1_BANK->DIR &= ~C1_PORT; C1_BANK->OUT &= ~C1_PORT   //to turn off column 1
#define COL2_ON        C2_BANK->DIR |=  C2_PORT; C2_BANK->OUT |=  C2_PORT   //to turn on column 2
#define COL2_OFF       C2_BANK->DIR &= ~C2_PORT; C2_BANK->OUT &= ~C2_PORT   //to turn off column 2

void KeyInit(); //my function prototypes for the key pad
uint16_t readKey(void);
uint8_t readRow(void);
void interON(void);//turns on col
void interOFF(void);//turns off col
int decodeKey(int val);//decodes press

void SysTickInit(void);
void delay_ms(int ms);//delay in milliseconds using systick
void delay_us(int us);//delay in milliseconds using systick

#endif
