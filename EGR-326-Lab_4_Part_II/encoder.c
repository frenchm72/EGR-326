#include "msp.h"
#include "encoder.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void rotorInit(void)
{
    SWPORT->SEL0 &= ~(SWPIN); //setting up sw pin as interrupt
    SWPORT->SEL1 &= ~(SWPIN);
    SWPORT->DIR &= ~(SWPIN);
    SWPORT->REN |= (SWPIN);
    SWPORT->OUT |= (SWPIN);
    SWPORT->IES &= ~(SWPIN);
    SWPORT->IE |= (SWPIN);
    SWPORT->IFG &= ~(SWPIN);

    CLKPORT->SEL0 &= ~(CLKPIN); //setting up clk pin as interrupt
    CLKPORT->SEL1 &= ~(CLKPIN);
    CLKPORT->DIR &= ~(CLKPIN);
    CLKPORT->REN |= (CLKPIN);
    CLKPORT->OUT |= (CLKPIN);
    CLKPORT->IES &= ~(CLKPIN);
    CLKPORT->IE |= (CLKPIN);
    CLKPORT->IFG &= ~(CLKPIN);

    DTPORT->SEL0 &= ~(DTPIN); //setting up dt pin as interrupt
    DTPORT->SEL1 &= ~(DTPIN);
    DTPORT->DIR &= ~(DTPIN);
    DTPORT->REN |= (DTPIN);
    DTPORT->OUT |= (DTPIN);
    DTPORT->IES &= ~(DTPIN);
    DTPORT->IE |= (DTPIN);
    DTPORT->IFG &= ~(DTPIN);
}
void PORT6_IRQHandler(void) // port 5 interrupt handler this needs adjusted for the correct handler
{
    if((SWPORT->IFG & SWPIN))//flag for button on rotary encoder
        SWflag = 1;
    if((CLKPORT->IFG & CLKPIN))
    {
        if(CLKPORT->IES & CLKPIN) //rising edge
        {
          if(DTPORT->IN & DTPIN) //direction count incremented respectively
              CWcount++;
          else
              CCWcount++;

        }
        else if(!(CLKPORT->IES & CLKPIN))//falling edge
        {
          if(DTPORT->IN & DTPIN)//direction count incremented respectively
              CCWcount++;
          else
              CWcount++;
        }
        CLKPORT->IES ^= CLKPIN;//switching the edge to read (rise or fall)
    }
    P6->IFG &= ~(SWPIN|CLKPIN|DTPIN);//clears flag
}

void SysTickInit(void)
{
SysTick -> CTRL = 0;//disable systick during step
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
