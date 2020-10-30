/***********************************************************************************************************
    Title:                EGR 326 Lab 8
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French
    Date:               10/23/2020
    Instructor:     Professor Brian Krug
    Description:   Lab 8 - interfacing the 8 digit, 7 segment display with the MSP432
    Note:               Most code was complied using the LCD code and editing register that needed be.
************************************************************************************************************
WIRING FOR MSP432:
7-Segment 8-Digit       :   DIN - P1.6    |   CLK - P1.5  |   CS - P5.0   |   Vcc - 3.3V  |   GND - GND|
Button                            :  GND -GND   |   PIN - P5.2   |
***********************************************************************************************************/
#include "msp.h"
#include <7SegSPI.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define BUTPORT P5
#define BUTPIN BIT2

bool bFlag = false;
bool cFlag = false;
bool tFlag = false;

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW
                                | WDT_A_CTL_HOLD;

    uint8_t digC1 = 0, digC2 = 0, digC3 = 0, digC4 = 0;
    uint8_t digT1 = 0, digT2 = 0, digT3 = 0, digT4 = 0;
    int count = 0;

    SysTickInit();
    initSPI();

    BUTPORT->SEL0 &= ~BUTPIN;// button set up as interrupt
    BUTPORT->SEL1 &= ~BUTPIN;
    BUTPORT->DIR &= ~BUTPIN;
    BUTPORT->REN |= BUTPIN;
    BUTPORT->OUT |= BUTPIN;
    BUTPORT->IE |= BUTPIN;
    BUTPORT->IFG = 0;// clears flag
    BUTPORT->IES |= BUTPIN;

    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);
     __enable_interrupt();

     sendSPI(DIGIT_0, digC1);
     sendSPI(DIGIT_1, digC2);
     sendSPI(DIGIT_2, digC3);
     sendSPI(DIGIT_3, digC4);
     sendSPI(DIGIT_4, digC1);
     sendSPI(DIGIT_5, digC2);
     sendSPI(DIGIT_6, digC3);
     sendSPI(DIGIT_7, digC4);

    while(1){
        if(bFlag){
            delay_ms(5);
            while(!(BUTPORT -> IN & BUTPIN));
            bFlag = false;
            tFlag = !tFlag;
//            if(!tFlag){
                digC1++;
                if (digC1 >9){
                    digC1 = 0;
                    digC2++;
                }
                if(digC2>9){
                    digC2 = 0;
                    digC3++;
                }
                if(digC3>9){
                    digC3 = 0;
                    digC4++;
                }
                if(digC4>9){
                    digC4 = 0;
                }
                sendSPI(DIGIT_0, digC1);//updates count to display
                sendSPI(DIGIT_1, digC2);
                sendSPI(DIGIT_2, digC3);
                sendSPI(DIGIT_3, digC4);
 //           }
            }
        else  if(tFlag && (count >= 500)){
            digT1++;
            if (digT1 >9){
                digT1 = 0;
                digT2++;
            }
            if(digT2>9){
                digT2 = 0;
                digT3++;
            }
            sendSPI(DIGIT_4, digT1);//updates timer to display
            sendSPI(DIGIT_5, digT2);
            sendSPI(DIGIT_6, digT3);
            sendSPI(DIGIT_7, digT4);
            if(digT3>=1){
                digT3 = 0;
                digT1 = 0;
            }
            count = 0;
        }

        delay_ms(1);
        count++;
        }
    }

void PORT5_IRQHandler(void){
    if(BUTPORT -> IFG & BUTPIN)
        bFlag = true;
    BUTPORT -> IFG &= ~BUTPIN;
}
