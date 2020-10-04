#include "msp.h"
#include "rtc.h"
#include "keypad.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void KeyInit(void)   //sets up keypad
{
        R0_BANK->SEL0 &= ~(R0_PORT); //setting up button as interrupt
        R0_BANK->SEL1 &= ~(R0_PORT);
        R0_BANK->DIR &= ~(R0_PORT);
        R0_BANK->REN |= (R0_PORT);   //enable resistor
        R0_BANK->OUT &= ~(R0_PORT);   //enable pull down
        R0_BANK->IES &= ~(R0_PORT);   //set pin as interrupt
        R0_BANK->IE |= (R0_PORT);    //enable interrupt for Px.x
        R0_BANK->IFG &= ~(R0_PORT);  //clears interrupt flag

        R1_BANK->SEL0 &= ~(R1_PORT); //setting up button as interrupt
        R1_BANK->SEL1 &= ~(R1_PORT);
        R1_BANK->DIR &= ~(R1_PORT);
        R1_BANK->REN |= (R1_PORT);   //enable resistor
        R1_BANK->OUT &= ~(R1_PORT);   //enable pull down
        R1_BANK->IES &= ~(R1_PORT);   //set pin as interrupt
        R1_BANK->IE |= (R1_PORT);    //enable interrupt for Px.x
        R1_BANK->IFG &= ~(R1_PORT);  //clears interrupt flag

        R2_BANK->SEL0 &= ~(R2_PORT); //setting up button as interrupt
        R2_BANK->SEL1 &= ~(R2_PORT);
        R2_BANK->DIR &= ~(R2_PORT);
        R2_BANK->REN |= (R2_PORT);   //enable resistor
        R2_BANK->OUT &= ~(R2_PORT);   //enable pull down
        R2_BANK->IES &= ~(R2_PORT);   //set pin as interrupt from low to high
        R2_BANK->IE |= (R2_PORT);    //enable interrupt for Px.x
        R2_BANK->IFG &= ~(R2_PORT);  //clears interrupt flag

        R3_BANK->SEL0 &= ~(R3_PORT); //setting up button as interrupt
        R3_BANK->SEL1 &= ~(R3_PORT);
        R3_BANK->DIR &= ~(R3_PORT);
        R3_BANK->REN |= (R3_PORT);   //enable resistor
        R3_BANK->OUT &= ~(R3_PORT);   //enable pull down
        R3_BANK->IES &= ~(R3_PORT);   //set pin as interrupt
        R3_BANK->IE |= (R3_PORT);    //enable interrupt for Px.x
        R3_BANK->IFG &= ~(R3_PORT);  //clears interrupt flag

       C0_BANK -> DIR &= ~C0_PORT; //sets column 1 to input
       C0_BANK -> REN |=  C0_PORT; //turns on pull up/down resistor
       C0_BANK -> OUT &= ~C0_PORT; //sets to a pull down

       C1_BANK -> DIR &= ~C1_PORT; //sets column  to input
       C1_BANK -> REN |=  C1_PORT; //turns on pull up/down resistor
       C1_BANK -> OUT &= ~C1_PORT; //sets to a pull down

       C2_BANK -> DIR &= ~C2_PORT; //sets column 3 to input
       C2_BANK -> REN |=  C2_PORT; //turns on pull up/down resistor
       C2_BANK -> OUT &= ~C2_PORT; //sets to a pull down
}

uint16_t readKey() //reads key column then row
{
    interOFF();//toggles col to check

    uint16_t keys = 0x0000; //this initial sets all the bits to so nothing is pressed and if something was it'll clear it

    COL0_ON;                       //turns column on one at a time
    keys |= (readRow() & 0x000F) << 0;  //reads in column and puts a 0 or 1 where the button was pressed from BIT0 to BITB
    COL0_OFF;                    //turns off one at a time to then look at next column

    COL1_ON;                        //turns column on one at a time
    keys |= (readRow() & 0x000F) << 4;//reads in column and puts a 0 or 1 where the button was pressed from BIT0 to BITB
    COL1_OFF;                      //turns off one at a time to then look at next column

    COL2_ON;                         //turns column on one at a time
    keys |= (readRow() & 0x000F) << 8;   //reads in column and puts a 0 or 1 where the button was pressed from BIT0 to BITB
    COL2_OFF;                        //turns off one at a time to then look at next column

    keyPressed = 0;//clears flag
    interON();//turns col back on for interrupt

    return keys;  //returns the value of the key pressed
}

uint8_t readRow() //reads rows
{
    uint8_t rows = 0x00;
    delay_ms(15); //debounce

    rows |= (R0_BANK -> IN & R0_PORT)?1<<0:0;// this sets the row in which the button has been pressed and sends it to the readKey pad function
    rows |= (R1_BANK -> IN & R1_PORT)?1<<1:0;
    rows |= (R2_BANK -> IN & R2_PORT)?1<<2:0;
    rows |= (R3_BANK -> IN & R3_PORT)?1<<3:0;

    while(!(R0_BANK->IN & R0_PORT)==0);  //this is for the button being held
    while(!(R1_BANK->IN & R1_PORT)==0);
    while(!(R2_BANK->IN & R2_PORT)==0);
    while(!(R3_BANK->IN & R3_PORT)==0);

    return rows;//returns which row was pressed
}

void PORT2_IRQHandler(void) // port 2 interrupt handler
{
    if((R0_BANK->IFG & R0_PORT) || (R1_BANK->IFG & R1_PORT) || (R2_BANK->IFG & R2_PORT))//row 0-2
            keyPressed = 1;
    R0_BANK->IFG &= ~(R0_PORT);//clears flags
    R1_BANK->IFG &= ~(R1_PORT);
    R2_BANK->IFG &= ~(R2_PORT);
}
void PORT5_IRQHandler(void) // port 5 interrupt handler this needs adjested for the correct handler
{
    if((R3_BANK->IFG & R3_PORT))//row 3
            keyPressed = 1;
    R3_BANK->IFG &= ~(R3_PORT);//clears flag
}

void interON(void)
{
    COL0_ON;//turns on the col so the handler can check
    COL1_ON;
    COL2_ON;
}
void interOFF(void)
{
    COL0_OFF;//turns off col to check which button is pressed like normal
    COL1_OFF;
    COL2_OFF;
}

char* decodeKey(int val) // decodes key val
{
    if(val == 1)//represent 1
    return "1";
    else if(val == 16)//represent 2
        return "2";
    else if(val == 256)//represent 3
        return "3";
    else if(val == 2)//represent 4
        return "4";
    else if(val == 32)// represent 5
        return "5";
    else if(val == 512)//represent 6
        return "6";
    else if(val == 4)//represent 7
            return "7";
    else if(val == 64)//represent 8
            return "8";
    else if(val == 1024)//represent 9
            return "9";
    else if(val == 8)//represent *
            return "*";
    else if(val == 128)//represent 0
            return "0";
    else if(val == 2048)//represent #
            return "#";
    else
            return "X";
}

void SysTickInit(void)
{
SysTick -> CTRL = 0;//disable systick during setup
SysTick -> LOAD = 0x00FFFFFF;//max reload value
SysTick -> VAL = 0;//clears it
SysTick -> CTRL = 0x00000005;//enables systick 3MHz no interrupts
}

void delay_ms(int ms)//delay in milliseconds using systick
{
SysTick->LOAD =(3333*ms)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}

void delay_us(int us)//delay in milliseconds using systick
{
SysTick->LOAD = (3*us)-1;
SysTick->VAL = 0;
while((SysTick->CTRL & BIT(16))==0);
}

