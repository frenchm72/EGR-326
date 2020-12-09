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
#include <wstep.h>
#include <image.h>
#include <projectmain.h> // main project header
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/
#define EEADD (0x50 )

bool BUT1 = false, BUT2 = false, SQWinterrupt = false, speedEX = false, tempEX = false;
bool CWcount = false, CCWcount = false , SWflag = false;
int EXTR, EXdata;

uint16_t BGCOLOR;//black colors
uint16_t TXTCOLOR; //white colors

float rise, fall, pulseWidthECHO;
unsigned int RiseFlag = 0;
bool count = false, print = false;
uint16_t pulseWidth;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    initMSP();

     BGCOLOR = ST7735_Color565(255, 255, 255);//black colors
     TXTCOLOR = ST7735_Color565(0, 0, 0); //white colors

    float distIN = 0.0, distCM = 0.0;//global variables stepper and Sonar
    float rpmP, SpeedP, oldSpeedP;
    char num[10], dispDis[10];

    //zero steppers
    stateTempHeld = Step1;
    stateSpeedHeld = Step1;
    Sstep(MAXS, 1);
    Tstep(MAXT, 1);
    Tstep(MAXT, 0);
    Tstep(MAXT, 1);
    speedPos = 0.0;
    tempPos = 0.0;

    //goes after all init
    setWDT();

   int i, j;
   i = 0;
   j = 0;
   ST7735_FillScreen(BGCOLOR);
while(1){
            setWDT();
            delay_ms(173);//random delay for stepper
            if(pulseWidth != 0){
                rpmP = (((1000.0*1000.0*60.0)/(2.0*pulseWidth*250.0)));//calc rpm 250us by the number of counts
                SpeedP = ((((rpmP*60.0)*0.5235)/5280.0)+SpeedP)/2;// calc speed
                pulseWidth = 0;
            }
            else{
                SpeedP = 0;
                speedPos = 0;
                if(speedPos > 0)
                    speedPos = speedPos - 4;
                else
                    speedPos = 0;
                Sstep(4, 1);
            }
            setWDT();
           AbsSpeed = (3.4*SpeedP-38.0);//linear equation for the step this proved to be precise if the stepper is in the center
           if(AbsSpeed > speedPos){
               Sstep((AbsSpeed-speedPos), 0);
               speedPos = AbsSpeed;
           }
           else if(AbsSpeed < speedPos){
               Sstep((speedPos-AbsSpeed), 1);
               speedPos = AbsSpeed;
           }
              itoa((SpeedP), num);//prints speed
              strcat(num, "   ");
              ST7735_DrawStringMod(40, 40+20,  num, TXTCOLOR, BGCOLOR,  TXTSIZE+4);

                TRIGPORT->OUT &= ~(TRIGPIN);
                delay_us(10);
                TRIGPORT->OUT |= (TRIGPIN);
                distCM = (pulseWidthECHO/3.0) / 58.0; //to find distance knowing speed of sound is 340m/s
                distIN = (pulseWidthECHO/3.0) / 148.0;//find distance in inches
                if(distCM <= 15.00)//turns LED on if close to object
                    REDPORT->OUT &= ~(REDLED);
                else
                    REDPORT->OUT |= (REDLED);
                strcpy(dispDis, "Distance: ");
                itoa(distCM, num);
                strcat(dispDis, num);
                strcat(dispDis, " CM            ");
                ST7735_DrawStringMod(STATUSX-2*(TXTSIZE*10), STATUSY+4*(TXTSIZE*10),dispDis , TXTCOLOR, BGCOLOR,  TXTSIZE);

            if((((readRTC.minT*10+readRTC.minO)/5)==0) && ((readRTC.secT*10+readRTC.secO)==0)){//allows program to fault every 5 min to prevent redundant excursion
                speedEX = false;
                tempEX = false;
            }
            if((readRTC.temp >= 25 ) && (tempEX == false)){//temp fault
                delay_ms(5);
                ReadRTC();
                saveToEEPROM(i, 0, readRTC.temp);
                setWDT();
                tempEX = true;
                if(i>=5)
                    i = 0;
                i++;
            }
            if((SpeedP >= 60) && (speedEX == false)){//speed fault
                delay_ms(5);
                ReadRTC();
                saveToEEPROM(i, 1, SpeedP);
                setWDT();
                speedEX = true;
                if(i>=5)
                    i = 0;
                i++;
            }
            if(BUT1){
                delay_ms(5);
                    BUT1 = false;
                readFromEEPROM(j);
                updateEXT(j);
                setWDT();
                if(j>=5)
                    j = 0;
                j++;
            }
            if(SQWinterrupt){
                ReadADC();
                ReadRTC();
                if(((readRTC.hourT*10+readRTC.hourO) == 12) && ((readRTC.minT*10+readRTC.minO) >= 0) && ((readRTC.minT*10+readRTC.minO) <= 5))//makes sure no random letters are displayed refreash every 122 hrs
                    ST7735_FillScreen(BGCOLOR);
                updateTime();
                SQWinterrupt = false;
            }
            if(SWflag){
                int value = readRTC.dayOfWeek, count = 0, pos = 0;
                SWflag = false;
                while(1){
                    setWDT();
                    updateTime();
                    if(pulseWidth != 0){
                        rpmP = (((1000.0*1000.0*60.0)/(2.0*pulseWidth*250.0)));//calc rpm 250us by the number of counts
                        SpeedP = ((((rpmP*60.0)*0.5235)/5280.0)+SpeedP)/2;// calc speed
                        pulseWidth = 0;
                    }
                    else{
                        SpeedP = 0;
                        speedPos = 0;
                        if(speedPos > 0)
                            speedPos = speedPos - 4;
                        else
                            speedPos = 0;
                        Sstep(4, 1);
                    }
                    setWDT();
                   AbsSpeed = (3.4*SpeedP-38.0);//linear equation for the step this proved to be precise if the stepper is in the center
                   if(AbsSpeed > speedPos){
                       Sstep((AbsSpeed-speedPos), 0);
                       speedPos = AbsSpeed;
                   }
                   else if(AbsSpeed < speedPos){
                       Sstep((speedPos-AbsSpeed), 1);
                       speedPos = AbsSpeed;
                   }
                      itoa((SpeedP), num);//prints speed
                      strcat(num, "   ");
                      ST7735_DrawStringMod(40, 40+20,  num, TXTCOLOR, BGCOLOR,  TXTSIZE+4);

                    if(CCWcount){//if user wants to decrease
                        count = 0;
                        delay_ms(5);
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
                        delay_ms(5);
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
void saveToEEPROM(int i, int SPEEDTEMP, int data){
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x00 & 0xFF),  (readRTC.secO));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x01 & 0xFF),  (readRTC.secT));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x02 & 0xFF),  (readRTC.minO));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x03 & 0xFF),  (readRTC.minT));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x04 & 0xFF),  (readRTC.hourO));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x05 & 0xFF),  (readRTC.hourT));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x06 & 0xFF),  (readRTC.dayO));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x07 & 0xFF),  (readRTC.dayT));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x08 & 0xFF),  (readRTC.monthO));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x09 & 0xFF),  (readRTC.monthT));
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x0A & 0xFF), SPEEDTEMP);
    delay_ms(DELAY);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x0B & 0xFF), data);
    delay_ms(DELAY);
}
void readFromEEPROM(int i){
    unsigned char red;
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x0B & 0xFF),  &red);
    EXdata = red;
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x0A & 0xFF),  &red);
    EXTR = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x09 & 0xFF),  &red);
    readEE.monthT = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x08 & 0xFF),  &red);
    readEE.monthO = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x07 & 0xFF),  &red);
    readEE.dayT = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x06 & 0xFF),  &red);
    readEE.dayO = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x05 & 0xFF),  &red);
    readEE.hourT = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x04 & 0xFF),  &red);
    readEE.hourO = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x03 & 0xFF),  &red);
    readEE.minT = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x02 & 0xFF),  &red);
    readEE.minO = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x01 & 0xFF),  &red);
    readEE.secT = red;
    delay_ms(DELAY);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x00 & 0xFF),  &red);
    readEE.secO = red;
    delay_ms(DELAY);
}

void ReadADC(void){
       float adc_input;
       float refV = 3.3, readVoltage;
       float maxV = 2.6, minV = 2.5; //values from my room that will change automaticly with conditions found
       float percentage;
       float power = 16384.0;
    ADC14 -> CTL0 |= ADC14_CTL0_SC; //starts conversion
             adc_input = ADC14->MEM[0];
//             readVoltage = (refV * ((adc_input + 0.5)/power));
             percentage =(1- (((refV * ((adc_input + 0.5)/power)) - minV)/(maxV - minV)));
             if(percentage <= 0)
                 TIMER_A1->CCR[1] = PERIOD*0;//a percentage of the max voltage seen
             else
                 TIMER_A1->CCR[1] = PERIOD*percentage;//a percentage of the max voltage seen

             if(((percentage*100) <= 70) && (BGCOLOR != ST7735_Color565(255, 255, 255))){
              BGCOLOR = ST7735_Color565(255, 255, 255);//black colors
              TXTCOLOR = ST7735_Color565(0, 0, 0); //white colors
              ST7735_FillScreen(BGCOLOR);
             }
             else if(((percentage*100) > 70) && (BGCOLOR != ST7735_Color565(0, 0, 0))){
                 BGCOLOR = ST7735_Color565(0, 0, 0); //white colors
                 TXTCOLOR = ST7735_Color565(255, 255, 255);//black colors
                 ST7735_FillScreen(BGCOLOR);
                }
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

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_4;    // change the SMCLK clock speed to 3MHz.

    CS->KEY = 0x0000695A;                            //password for CS registers
    CS->CTL1 |= 0b00000010000000000000000000000000;   //dividing ACLCK by 4
    CS->KEY = 0;

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}
void updateTime(void){
 char date[100], num[10];
    setWDT();
    strcpy(date, dayOfWeekDecode(readRTC.dayOfWeek) );
    strcat(date, "  " );
    itoa((readRTC.temp ), num);
    strcat(date, num);
    //strcat(date, 176);//°
    strcat(date, "C          " );
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
    strcat(date, "       " );
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
    strcat(date, (readRTC.PMAM ? "PM        " : "AM         "));
    ST7735_DrawStringMod(STATUSX+0.75*(TXTSIZE*10), STATUSY+7*(TXTSIZE*10),  date, TXTCOLOR, BGCOLOR,  TXTSIZE);
}
void updateEXT(int j){
    char date[100], num[10];
    int placex=0;
    strcpy(date, "Extrusion #");
    itoa(j, num);
    strcat(date, num );
    ST7735_DrawStringMod(placex+3*(TXTSIZE*10), placex+0*(TXTSIZE*10),  date , TXTCOLOR, BGCOLOR,  TXTSIZE);
    if(EXTR)
        strcpy(date, "Speed: ");
    else
        strcpy(date, "Temp: ");
    itoa(EXdata, num);
    strcat(date, num );
    strcat(date, "             " );
    ST7735_DrawStringMod(placex+3*(TXTSIZE*10), placex+1*(TXTSIZE*10),  date, TXTCOLOR, BGCOLOR,  TXTSIZE);
    strcpy(date, monthDecode(readEE.monthT *10 + readEE.monthO) );
    strcat(date, " " );
    itoa((readEE.dayT ), num);
    strcat(date, num );
    itoa((readEE.dayO ), num);
    strcat(date, num );
    strcat(date, " " );
    ST7735_DrawStringMod(placex+3*(TXTSIZE*10), placex+2*(TXTSIZE*10),  date, TXTCOLOR, BGCOLOR,  TXTSIZE);
    itoa((readEE.hourT), num);
    strcpy(date, num );
    itoa((readEE.hourO), num);
    strcat(date, num );
    strcat(date, ":" );
    itoa((readEE.minT), num);
    strcat(date, num );
    itoa((readEE.minO), num);
    strcat(date, num );
    strcat(date, ":" );
    itoa((readEE.secT), num);
    strcat(date, num );
    itoa((readEE.secO), num);
    strcat(date, num );
    strcat(date, "              " );
    ST7735_DrawStringMod(placex+3*(TXTSIZE*10), placex+3*(TXTSIZE*10),  date, TXTCOLOR, BGCOLOR,  TXTSIZE);
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
    if(SQWPORT->IFG & SQWPIN)
        SQWinterrupt = true;
    SQWPORT->IFG &= ~(SQWPIN);
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

void TA2_N_IRQHandler(void)
{
    if(TIMER_A2->CCTL[HALLINSTANCE] & (TIMER_A_CCTLN_CCIFG)){
    if(count){
        pulseWidth = TIMER_A2->CCR[HALLINSTANCE]; // Get current count
        count = false;
        print = true;
    }
    else{
        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        count = true;
    }
    if(TIMER_A2->IV == 0x0E) {              //Overflow
            TIMER_A2->CTL |= TIMER_A_CTL_CLR;
    }
    }
    TIMER_A2->CCTL[HALLINSTANCE] &= ~(TIMER_A_CCTLN_CCIFG);    // Clear the interrupt flag
}

void TA0_N_IRQHandler(void)
{
    rise = TIMER_A0->CCR[1]; // Get current count
    if (ECHOPORT->IN&ECHOPIN)  //  record timer on a falling edge
        TIMER_A0->CTL |=TIMER_A_CTL_CLR;    // start timer on a rising edge
    else
        pulseWidthECHO = rise; // record time on falling edge

    TIMER_A0->CCTL[1] &= ~(TIMER_A_CCTLN_CCIFG);    // Clear the interrupt flag
}


void PORT3_IRQHandler(void) // port 1 interrupt handler
{
    if(WDBUTPORT->IFG & WDBUTPIN){
        while(1);
    }
    if(BUTPORT->IFG & BUT1PIN){
        BUT1 = true;
    }
    BUTPORT->IFG &= ~(BUT1PIN);
}
