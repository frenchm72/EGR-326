/***********************************************************************************************************
  Title:        EGR 326 Lab 3
  Filename:     Part_I.C
  Author(s):    Mathew J. Yerian-French
  Date:         09/18/2020
  Instructor:   Professor Brian Krug
  Description:
  Note:
************************************************************************************************************
WIRING FOR MSP432
Sonar Sensor: GND - GND | ECHO_VoltDiv - P6.4 | TRIG_MosFet - P4.0 | VCC = 5V |
***********************************************************************************************************/
#include "PARTI.h"

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initMSP();
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

    while(1)
   {

    }
}

//  digitalWrite(7, HIGH);                                                        //sets pin to high to produce sound
//  delayMicroseconds(10);                                                        //small wave of 10 microseconds
//  digitalWrite(7, LOW);                                                         //turns sound off
//
//while ( digitalRead(8) == 0 );                                                  //while loop for sonar sensor for #1
//
//  Tim1 = micros();                                                              //time thats calculated
//
//while ( digitalRead(8) == 1);                                                   //if the value is for #2 itll keep going
//
//  Tim2 = micros();                                                              //time in seconds calculated
//
//  PlseW = Tim2 - Tim1;                                                          //to find the width of the pulse
//  Cm = PlseW / 58.0;                                                            //to find cintimeters speed of sound is about 340m/s
//  In = PlseW / 148.0;                                                           //to find inches
