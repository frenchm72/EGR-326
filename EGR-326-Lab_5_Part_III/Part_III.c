/***********************************************************************************************************
  Title:        EGR 326 Lab 5
  Filename:     Part_III.c, rtc.h, rtc.c, keypad.c, keypad.h
  Author(s):    Mathew J. Yerian-French
  Date:         10/02/2020
  Instructor:   Professor Brian Krug
  Description:  Part III of Lab 5
  Note:         "%c", 176 = degree C
************************************************************************************************************
WIRING FOR MSP432
RTC: SCL - P6.5 | SDA - P6.4 | VCC - 3.3V | GND - GND |
***********************************************************************************************************/
#include "msp.h"
#include "keypad.h"
#include "rtc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SLAVEADD 0x00 //solder pads on A0, A1, and A2 on the module

void getSetDateTime(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int keyMain = 0;

    I2C1_init();//init I2C
    KeyInit();//init keypad
    SysTickInit();
    interON();//turns col on

    NVIC->ISER[1] = 1 << ((PORT2_IRQn) & 31);
    __enable_interrupt();

    printf("\n\nPlease Press '*' to Enter the Sub Menu\n\n");
    while(1){
        if(keyPressed)//waits for an interrupt
            {
            keyMain = readKey();//reads the key
            if(!(strcmp(decodeKey(keyMain),"*"))){
                printf("Please Enter:\n1 - Day Month Year\n2 - "
                        "Hour Minute Second\n3 - Temperature\n");
                while(1){
                    if(keyPressed)//waits for an interrupt
                        {
                        ReadRTC();
                        keyMain = readKey();//reads the key
                        if(!(strcmp(decodeKey(keyMain),"1")))
                        {
                            printf("\nThe Date is:\n%s\n%s %d%d, 2%d%d%d\n\n",
                                   dayOfWeekDecode(readRTC.dayOfWeek),
                                   monthDecode(readRTC.monthT *10 + readRTC.monthO),
                                   readRTC.dayT, readRTC.dayO,
                                   readRTC.yearC, readRTC.yearD, readRTC.yearO);

                            printf("\nPlease Press '*' to Enter the Sub Menu\n\n");
                            break;

                        }
                        else if(!(strcmp(decodeKey(keyMain),"2")))
                        {
                            printf("\nTime:\n%d%d:%d%d:%d%d %s\n", readRTC.hourT, readRTC.hourO,
                               readRTC.minT, readRTC.minO,
                               readRTC.secT, readRTC.secO, (readRTC.PMAM ? "PM" : "AM"));

                            printf("\nPlease Press '*' to Enter the Sub Menu\n\n");
                            break;

                        }
                        else if(!(strcmp(decodeKey(keyMain),"3")))
                        {
                            printf("\nTempature:\n%d%cC\n", readRTC.temp, 176);
                            printf("\nPlease Press '*' to Enter the Sub Menu\n\n");
                            break;

                        }
                        }
                    }
                }
    }
}
}

//must be in main due to structure
void ReadRTC(void){
    unsigned char readBus;
    unsigned char error;
    do{
        error = I2C1_byteRead(SLAVE_ADDR, 0x00, &readBus);//seconds
        readRTC.secT = (readBus >> 4);
        readRTC.secO = (readBus & 0x0F);
        //printf("Sec:%d%d\n", readRTC.secT, readRTC.secO);

        error = I2C1_byteRead(SLAVE_ADDR, 0x01, &readBus);//min
        readRTC.minT = (readBus >> 4);
        readRTC.minO = (readBus & 0x0F);
        //printf("Min:%d%d\n", readRTC.minT, readRTC.minO);

        error = I2C1_byteRead(SLAVE_ADDR, 0x02, &readBus);//hour and am/pm
        readRTC.hourT = ((readBus >> 4) & 0b0001);
        readRTC.hourO = (readBus & 0x0F);
        //printf("Hour:%d%d\n", readRTC.hourT, readRTC.hourO);
        readRTC.PMAM = ((readBus & 0b00100000) >> 5);//pm/am
        //printf("PMAM:%s\n", (readRTC.PMAM ? "PM" : "AM"));

        error = I2C1_byteRead(SLAVE_ADDR, 0x03, &readBus);//day of week
        readRTC.dayOfWeek = (readBus & 0x0F);
        //printf("Day of Week:%s\n", dayOfWeekDecode(readRTC.dayOfWeek));

        error = I2C1_byteRead(SLAVE_ADDR, 0x04, &readBus);//date
        readRTC.dayT = (readBus >> 4);
        readRTC.dayO = (readBus & 0x0F);
        //printf("Date:%d%d\n", readRTC.dayT, readRTC.dayO);

        error = I2C1_byteRead(SLAVE_ADDR, 0x05, &readBus);//month and century
        readRTC.monthT = ((readBus  & 0b01111111) >> 4);
        readRTC.monthO = (readBus & 0x0F);
        readRTC.yearC = ((readBus  & 0b10000000) >> 7);
        //printf("Month:%s\n yearC:%d\n", monthDecode(readRTC.monthT *10 + readRTC.monthO), readRTC.yearC);

        error = I2C1_byteRead(SLAVE_ADDR, 0x06, &readBus);//month and century
        readRTC.yearD = (readBus >> 4);
        readRTC.yearO = (readBus & 0x0F);
        //printf("Year:%d%d\n", readRTC.yearD, readRTC.yearO);

        error = I2C1_byteRead(SLAVE_ADDR, 0x11, &readBus);//month and century
        readRTC.temp = ((((readBus >> 4) & 0b01111111)*10) + (readBus & 0x0F));
        //printf("Temp:%d\n\n", readRTC.temp);

        //comment this out if you dont want it to print the date
//        printf("\n\nThe Date is:\n%s\n%s %d%d, 2%d%d%d ", dayOfWeekDecode(readRTC.dayOfWeek),
//               monthDecode(readRTC.monthT *10 + readRTC.monthO),
//               readRTC.dayT, readRTC.dayO,
//               readRTC.yearC, readRTC.yearD, readRTC.yearO);
//        printf("%d%d:%d%d:%d%d %s\n", readRTC.hourT, readRTC.hourO,
//               readRTC.minT, readRTC.minO,
//               readRTC.secT, readRTC.secO, (readRTC.PMAM ? "PM" : "AM"));
        //printf("Tempature: %d\n", readRTC.temp);
           error = 1;
           } while (error == 0);
}
