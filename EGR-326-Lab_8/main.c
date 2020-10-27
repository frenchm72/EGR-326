/***********************************************************************************************************
  Title:        EGR 326 Lab 8
  Filename:     main.c
  Author(s):    Mathew J. Yerian-French, Luke Metz
  Date:         10/23/2020
  Instructor:   Professor Brian Krug
  Description:  Lab 8
  Note:
************************************************************************************************************
WIRING FOR MSP432
Push Button   : In  - P1.1
***********************************************************************************************************/
#include "msp.h"
#include <7SegSPI.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW
                                | WDT_A_CTL_HOLD;

    CSPORT->SEL0 &= ~CSPIN;
    CSPORT->SEL1 &= ~CSPIN;
    CSPORT->DIR |= CSPIN;
    CSPORT->REN |= CSPIN;
    CSPORT->OUT &= ~CSPIN;

    initSPI();
    SysTickInit();

    sendSPI(0x09, 0xFF);
    sendSPI(0x0A, 0x03);
    sendSPI(0x0B, 0x07);
    sendSPI(0x0C, 0x01);
    sendSPI(0x0F, 0x01);
    while(1);
}
