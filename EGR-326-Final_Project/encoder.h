#ifndef __ENCODER_H__
#define __ENCODER_H__

#define CLKPORT P6  //defines keypins for rows
#define CLKPIN BIT0
#define DTPORT P6
#define DTPIN BIT6
#define SWPORT P6
#define SWPIN BIT1

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

#endif
