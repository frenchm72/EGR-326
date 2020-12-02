/***********************************************************************************************************
    Title:                EGR 326 Final Project
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French
    Date:               11/25/2020
    Instructor:     Professor Brian Krug
    Description:  Final Project - Automotive Cluster
    Note:
************************************************************************************************************
WIRING FOR MSP432:
Photo Diode :   3.3 - Diode - P5.5 - 10kohm - GND |
Full Color LCD  :   SCK - P9.5  |   SDA - P9.7   |  LED - 3.3V |    A0 - P9.2   |   RST - P9.3   |
                                  VCC - 3.3V | GND - GND   | CS  - P9.4
LED  :  Gate of MOS - P 5.6 (TA2.1)  |  GND - Source - Drain - 120ohm - LED - 5V | |
RTC :   SCL - P6.5  |   SDA - P6.4  |   VCC - 3.3V  |   GND - GND   |
EEPROM :   SCL - P6.5  |   SDA - P6.4  |   VCC - Diode - 5V  |   GND - GND   |
Encoder: + - 3.3V | GND - GND | DT - P6.6 // Cap | CLK = P6.0 // Cap| SW = P6.0 |
************************************************************************************************************
Header files included:
***********************************************************************************************************/
#include <msp.h>
#include <rtc.h>
#include <encoder.h>
#include <ST7735.h>
#include <projectmain.h> // main project header
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/
#define EEADD (0x50 )
bool BUT1 = false, BUT2 = false, SQWinterrupt = false, speedEX = false;
bool CWcount = false, CCWcount = false , SWflag = false;

void saveToEEPROM(int i);
void readFromEEPROM(int i);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    initMSP();

   uint8_t i = 0, j=0;

while(1){
    for(i = 0 ; i < 5 ; i++){
        while(1){
            if(BUT1|speedEX){
                ReadRTC();
                saveToEEPROM(i);
                BUT1 = false;
                speedEX = false;
                break;
            }
            if(BUT2){
                j++;
                if(j==5)
                    j = 0;
                readFromEEPROM(j);
                BUT2 = false;
            }
            if(SQWinterrupt){
                ReadADC();
                ReadRTC();
                if((readRTC.hourT*10+readRTC.hourO) == 12)//makes sure no random letters are displayed
                    ST7735_FillScreen(BGCOLOR);
                updateTime();
                SQWinterrupt = false;
            }
            if(SWflag){
                int value = readRTC.dayOfWeek, count = 0, pos = 0;
                SWflag = false;
                while(1){
                    updateTime();
                    if(CCWcount){//if user wants to decrease
                        count = 0;
                        __delay_cycles(48000*5);
                        if(pos == 0){//set day of week
                          if(value > 1)
                              value--;
                          readRTC.dayOfWeek = value;
                      }
                       else if(pos == 1){//set month
                          if(value > 1)
                              value--;
                          readRTC.monthT = value/10;
                          readRTC.monthO = value%10;
                      }
                       else if(pos == 2){//set day
                          if(value > 1)
                              value--;
                          readRTC.dayT = value/10;
                          readRTC.dayO = value%10;
                      }
                       else if(pos == 3){//set year
                          if(value > 0)
                              value--;
                          readRTC.yearC = value/100;
                          readRTC.yearD = value/10;
                          readRTC.yearO = value%10;
                      }
                       else if(pos == 4){//set hour
                           if(value > 1)
                               value--;
                           readRTC.hourT = value/10;
                           readRTC.hourO = value%10;
                       }
                       else  if(pos == 5){//set min
                           if(value > 0)
                               value--;
                           readRTC.minT = value/10;
                           readRTC.minO = value%10;
                       }
                       else  if(pos == 6){
                           if(value > 0)
                               value--;
                           readRTC.secT = value/10;
                           readRTC.secO = value%10;
                       }
                       else if(pos == 7){//set pm/am
                           value = !value;
                           readRTC.PMAM = value;
                       }
                        CCWcount = false;
                    }
                    else if(CWcount){//if user wants to increase
                        count = 0;
                        __delay_cycles(48000*5);
                        if(pos == 0){//set day of week
                           if(value < 7)
                               value++;
                           readRTC.dayOfWeek = value;
                       }
                        else if(pos == 1){//set month
                           if(value < 12)
                               value++;
                           readRTC.monthT = value/10;
                           readRTC.monthO = value%10;
                       }
                        else if(pos == 2){//set day
                           if(value < 32)
                               value++;
                           readRTC.dayT = value/10;
                           readRTC.dayO = value%10;
                       }
                        else if(pos == 3){//set year
                           if(value < 999)
                               value++;
                           readRTC.yearC = value/100;
                           readRTC.yearD = value/10;
                           readRTC.yearO = value%10;
                       }
                        else if(pos == 4){//set hour
                            if(value < 12)
                                value++;
                            readRTC.hourT = value/10;
                            readRTC.hourO = value%10;
                        }
                        else  if(pos == 5){//set min
                            if(value < 59)
                                value++;
                            readRTC.minT = value/10;
                            readRTC.minO = value%10;
                        }
                        else  if(pos == 6){
                            if(value < 59)
                                value++;
                            readRTC.secT = value/10;
                            readRTC.secO = value%10;
                        }
                        else if(pos == 7){//set pm/am
                            value = !value;
                            readRTC.PMAM = value;
                        }
                        CWcount = false;
                    }
                    if(SQWinterrupt){//user is waiting
                        count++;
                        ReadADC();
                        SQWinterrupt = false;
                    }
                   if((pos == 0) && SWflag){//user wasts to move to year
                        pos++;
                        count = 0;
                        value = readRTC.monthT*10+readRTC.monthO;
                        SWflag = false;
                    }
                    else if((pos == 1) && SWflag){//user wasts to move to day
                        pos++;
                        count = 0;
                        value = readRTC.dayT*10+readRTC.dayO;
                        SWflag = false;
                    }
                    else if((pos == 2) && SWflag){//user wasts to move to hour
                        pos++;
                        count = 0;
                        value = readRTC.yearC*100+readRTC.yearD*10+readRTC.yearO;
                        SWflag = false;
                    }
                    else if((pos == 3) && SWflag){//user wasts to move to month
                        pos++;
                        count = 0;
                        value = readRTC.hourT*10+readRTC.hourO;
                        SWflag = false;
                    }
                    else if((pos == 4) && SWflag){//user wasts to move to sec
                        pos++;
                        count = 0;
                        value = readRTC.minT*10+readRTC.minO;
                        SWflag = false;
                    }
                    else if((pos == 5) && SWflag){
                        pos++;
                        count = 0;
                        value = readRTC.secT*10+readRTC.secO;
                        SWflag = false;
                    }
                    else if((pos == 6) && SWflag){
                            pos++;
                            count = 0;
                            value = readRTC.PMAM;
                            SWflag = false;
                        }
                    else if((pos == 7) && SWflag){//done
                        pos++;
                        count = 0;
                        setRTC.hourT = readRTC.hourT;
                        setRTC.hourO = readRTC.hourO;
                        setRTC.minT = readRTC.minT;
                        setRTC.minO = readRTC.minO;
                        setRTC.secT = readRTC.secT;
                        setRTC.secO = readRTC.secO;
                        setRTC.PMAM = readRTC.PMAM;
                        setRTC.yearC = readRTC.yearC;
                        setRTC.yearD = readRTC.yearD;
                        setRTC.yearO = readRTC.yearO;
                        setRTC.monthT = readRTC.monthT;
                        setRTC.monthO = readRTC.monthO;
                        setRTC.dayT = readRTC.dayT;
                        setRTC.dayO = readRTC.dayO;
                        setRTC.dayOfWeek = readRTC.dayOfWeek;
                        SetRTC();
                        ST7735_FillScreen(BGCOLOR);
                        SWflag = false;
                        break;
                    }
                    if(count == 60){
                        ST7735_FillScreen(BGCOLOR);
                        break;
                    }
                }
            }
        }
    }
}
}
void saveToEEPROM(int i){
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x00 & 0xFF),  (readRTC.secO));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x01 & 0xFF),  (readRTC.secT));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x02 & 0xFF),  (readRTC.minO));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x03 & 0xFF),  (readRTC.minT));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x04 & 0xFF),  (readRTC.hourO));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x05 & 0xFF),  (readRTC.hourT));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x06 & 0xFF),  (readRTC.dayO));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x07 & 0xFF),  (readRTC.dayT));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x08 & 0xFF),  (readRTC.monthO));
    __delay_cycles(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x09 & 0xFF),  (readRTC.monthT));
    __delay_cycles(DELAY);
}
void readFromEEPROM(int i){
    unsigned char red;

    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x09 & 0xFF),  &red);
    readEE.monthT = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x08 & 0xFF),  &red);
    readEE.monthO = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x07 & 0xFF),  &red);
    readEE.dayT = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x06 & 0xFF),  &red);
    readEE.dayO = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x05 & 0xFF),  &red);
    readEE.hourT = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x04 & 0xFF),  &red);
    readEE.hourO = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x03 & 0xFF),  &red);
    readEE.minT = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x02 & 0xFF),  &red);
    readEE.minO = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x01 & 0xFF),  &red);
    readEE.secT = red;
    __delay_cycles(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x00 & 0xFF),  &red);
    readEE.secO = red;
    __delay_cycles(DELAY);
}

void ReadADC(void){
       uint32_t adc_input;
       float readVoltage, refV = 3.3;
       float maxV = 3.3, minV = 0.5; //values from my room that will change automaticly with conditions found
       float percentage;

    ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion
             adc_input = ADC14->MEM[0];
             readVoltage = refV * ((adc_input + 0.5)/pow(2,14));
             percentage =(1- ((readVoltage - minV)/(maxV - minV))) - 0.25 ;
             if(percentage <= 0)
                 TIMER_A2->CCR[1] = PERIOD*0;//a percentage of the max voltage seen
             else
                 TIMER_A2->CCR[1] = PERIOD*percentage;//a percentage of the max voltage seen
}

void Clock_Init48MHz(void){
    // Configure Flash wait-state to 1 for both banks 0 & 1
       FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK0_RDCTL_WAIT_1;
       FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK1_RDCTL_WAIT_1;

    //Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*
       PJ->SEL0 |= BIT2 | BIT3;                     // Configure PJ.2/3 for HFXT function
       PJ->SEL1 &= ~(BIT2 | BIT3);
       CS->KEY = CS_KEY_VAL ;                       // Unlock CS module for register access
       CS->CTL2 |= CS_CTL2_HFXT_EN | CS_CTL2_HFXTFREQ_6 | CS_CTL2_HFXTDRIVE;
          while(CS->IFG & CS_IFG_HFXTIFG)
                    CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
      CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK     |
                              CS_CTL1_DIVM_MASK     |
                              CS_CTL1_SELS_MASK     |
                              CS_CTL1_DIVHS_MASK)   |
                              CS_CTL1_SELM__HFXTCLK |
                              CS_CTL1_SELS__HFXTCLK;

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_4;    // change the SMCLK clock speed to 12 MHz.

    CS->KEY = 0x0000695A;                            //password for CS registers
    CS->CTL1 |= 0b00000010000000000000000000000000;   //dividing ACLCK by 4
    CS->KEY = 0;

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}
void updateTime(void){
    char date[100], num[10];
    strcpy(date, dayOfWeekDecode(readRTC.dayOfWeek) );
    strcat(date, "   " );
    itoa((readRTC.temp ), num);
    strcat(date, num);
    //strcat(date, 176);//�
    strcat(date, " C" );
    ST7735_DrawStringMod(STATUSX-2*(TXTSIZE*10), STATUSY+5*(TXTSIZE*10),date , TXTCOLOR, BGCOLOR,  TXTSIZE);
    strcpy(date, monthDecode(readRTC.monthT *10 + readRTC.monthO) );
    strcat(date, " " );
    itoa((readRTC.dayT ), num);
    strcat(date, num );
    itoa((readRTC.dayO ), num);
    strcat(date, num );
    strcat(date, ", " );
    itoa((2), num);
    strcat(date, num );
    itoa((readRTC.yearC), num);
    strcat(date, num );
    itoa((readRTC.yearD), num);
    strcat(date, num );
    itoa((readRTC.yearO), num);
    strcat(date, num );
    ST7735_DrawStringMod(STATUSX-2*(TXTSIZE*10), STATUSY+6*(TXTSIZE*10),  date, TXTCOLOR, BGCOLOR,  TXTSIZE);
    itoa((readRTC.hourT), num);
    strcpy(date, num );
    itoa((readRTC.hourO), num);
    strcat(date, num );
    strcat(date, ":" );
    itoa((readRTC.minT), num);
    strcat(date, num );
    itoa((readRTC.minO), num);
    strcat(date, num );
    strcat(date, ":" );
    itoa((readRTC.secT), num);
    strcat(date, num );
    itoa((readRTC.secO), num);
    strcat(date, num );
    strcat(date, " " );
    strcat(date, (readRTC.PMAM ? "PM" : "AM"));
    ST7735_DrawStringMod(STATUSX+0.75*(TXTSIZE*10), STATUSY+7*(TXTSIZE*10),  date, TXTCOLOR, BGCOLOR,  TXTSIZE);
}

//must be in main due to structure
void ReadRTC(void){
    unsigned char readBus;
    unsigned char error;
    do{
        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x00, &readBus);//seconds
        readRTC.secT = (readBus >> 4);
        readRTC.secO = (readBus & 0x0F);

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x01, &readBus);//min
        readRTC.minT = (readBus >> 4);
        readRTC.minO = (readBus & 0x0F);

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x02, &readBus);//hour and am/pm
        readRTC.hourT = ((readBus >> 4) & 0b0001);
        readRTC.hourO = (readBus & 0x0F);
        readRTC.PMAM = ((readBus & 0b00100000) >> 5);//pm/am

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x03, &readBus);//day of week
        readRTC.dayOfWeek = (readBus & 0x0F);

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x04, &readBus);//date
        readRTC.dayT = (readBus >> 4);
        readRTC.dayO = (readBus & 0x0F);

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x05, &readBus);//month and century
        readRTC.monthT = ((readBus  & 0b01111111) >> 4);
        readRTC.monthO = (readBus & 0x0F);
        readRTC.yearC = ((readBus  & 0b10000000) >> 7);

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x06, &readBus);//month and century
        readRTC.yearD = (readBus >> 4);
        readRTC.yearO = (readBus & 0x0F);

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x11, &readBus);//temp
        readRTC.temp = ((((readBus >> 4) & 0b01111111)*10) + (readBus & 0x0F));
           error = 1;
           } while (error == 0);
}

void SetRTC(void){
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x00, ((setRTC.secT << 4) + setRTC.secO));//seconds
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x01, ((setRTC.minT << 4) + setRTC.minO));//minutes
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x02, (0b01000000 + (setRTC.PMAM << 5) + (setRTC.hourT << 4) + setRTC.hourO));//hour
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x03, (setRTC.dayOfWeek)); //day of week
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x04, ((setRTC.dayT << 4) + setRTC.dayO));//date
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x05, ((setRTC.yearC << 7) + (setRTC.monthT << 4) + setRTC.monthO));//centery/ month
    I2C1_byteWrite(RTCSLAVE_ADDR, 0x06, ((setRTC.yearD << 4) + setRTC.yearO));//year
}

void PORT1_IRQHandler(void) // port 1 interrupt handler
{
    if(BUTPORT->IFG & BUT1PIN)
        BUT1 = true;
    if(BUTPORT->IFG & BUT2PIN)
       BUT2 = true;
    if(SQWPORT->IFG & SQWPIN)
        SQWinterrupt = true;
    BUTPORT->IFG &= ~(BUT1PIN | BUT2PIN|SQWPIN);
}

void PORT6_IRQHandler(void) // port 5 interrupt handler this needs adjusted for the correct handler
{
    if((SWPORT->IFG & SWPIN))//flag for button on rotary encoder
        SWflag = true;
    if((CLKPORT->IFG & CLKPIN))
    {
        if(CLKPORT->IES & CLKPIN) //rising edge
        {
          if(DTPORT->IN & DTPIN) //direction count incremented respectively
              CWcount = true;
          else if(!CWcount)
              CCWcount = true;

        }
        else if(!(CLKPORT->IES & CLKPIN))//falling edge
        {
          if(DTPORT->IN & DTPIN)//direction count incremented respectively
              CCWcount = true;
          else if(!CCWcount)
              CWcount = true;
        }
        CLKPORT->IES ^= CLKPIN;//switching the edge to read (rise or fall)
    }
    P6->IFG &= ~(SWPIN|CLKPIN|DTPIN);//clears flag
}
