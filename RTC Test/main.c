/***********************************************************************************************************
  Title:        EGR 326 Lab 5 - Testing
  Filename:     main.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/02/2020
  Description:  Part I of Lab 5
  Note:         Code help from MSP432 handbook
************************************************************************************************************
WIRING FOR MSP432
RTC: SCL - PX.X | SDA - PX.X | VCC - 3.3V | GND - GND |
***********************************************************************************************************/
#include "msp.h"
#include "rtc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

int main(void) {

    I2C1_init();//init I2C
    I2C1_byteWrite(SLAVE_ADDR, 0x02, 0b01100100);

   while(1){
       ReadRTC();
       __delay_cycles(3000000);
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
        printf("\n\n%s %s %d%d, %d%d\n", dayOfWeekDecode(readRTC.dayOfWeek),
               monthDecode(readRTC.monthT *10 + readRTC.monthO),
               readRTC.dayT, readRTC.dayO,
               readRTC.yearD, readRTC.yearO);
        printf("%d%d:%d%d:%d%d %s\n", readRTC.hourT, readRTC.hourO,
               readRTC.minT, readRTC.minO,
               readRTC.secT, readRTC.secO, (readRTC.PMAM ? "PM" : "AM"));
        printf("Tempature: %d\n", readRTC.temp);

        error = 1;
    } while (error == 0);
}
