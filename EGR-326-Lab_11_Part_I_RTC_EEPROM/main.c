/***********************************************************************************************************
    Title:                EGR 326 Lab 11
    Filename:       main.c
    Author(s):      Mathew J. Yerian-French
    Date:               11/20/2020
    Instructor:     Professor Brian Krug
    Description:   Lab 11 -  Speed sensing using Hall effect sensors and with the MSP43
    Note:               ValvanoWare code was used as recommended by this lab. However a custom function was
  made to send letters easier. This was done by modifying a new function ST7735_DrawStringMod in the
  libraries. This was used in replace of ST7735_DrawString to generate a 'clear' background. The libraries
  were pulled from:http://users.ece.utexas.edu/~valvano/arm/downloadmsp432.html. A gvsulogo was
  used from web site
************************************************************************************************************
WIRING FOR MSP432:
RTC :   SCL - P6.5  |   SDA - P6.4  |   VCC - 3.3V  |   GND - GND   |
EEPROM :
************************************************************************************************************
Header files included:
***********************************************************************************************************/
#include <msp.h>
#include <rtc.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
/**********************************************************************************************************/
#define BUTPORT P1
#define BUT1PIN BIT1
#define BUT2PIN BIT4

#define EEADD (0x50 )
bool BUT1 = false, BUT2 = false;

void saveToEEPROM(int i);
void readFromEEPROM(int i);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    uint8_t i = 0, j;

    BUTPORT->SEL0 &= ~(BUT1PIN | BUT2PIN); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUT1PIN | BUT2PIN);
    BUTPORT->DIR &= ~(BUT1PIN | BUT2PIN);
    BUTPORT->REN |= (BUT1PIN | BUT2PIN);   //enable resistor
    BUTPORT->OUT |= (BUT1PIN | BUT2PIN);   //enable pull up
    BUTPORT->IES |= (BUT1PIN | BUT2PIN);   //set pin as interrupt
    BUTPORT->IE |= (BUT1PIN | BUT2PIN);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUT1PIN | BUT2PIN);  //clears interrupt flag

    I2C1_init();//init I2C
   // SetRTC();
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

while(1){
    for(i = 0 ; i < 5 ; i++){
        while(1){
            if(BUT1){
                ReadRTC();
                printf("Saving to Mem Location: #%d\n", i+1);
                saveToEEPROM(i);
                BUT1 = false;
                break;
            }
            if(BUT2){
                j++;
                if(j==5)
                    j = 0;
                printf("\nReading from Mem Location: #%d", j+1);
                readFromEEPROM(j);
                BUT2 = false;
            }
        }
    }
}
}
void saveToEEPROM(int i){
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x00 & 0xFF),  (readRTC.secO));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x01 & 0xFF),  (readRTC.secT));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x02 & 0xFF),  (readRTC.minO));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x03 & 0xFF),  (readRTC.minT));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x04 & 0xFF),  (readRTC.hourO));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x05 & 0xFF),  (readRTC.hourT));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x06 & 0xFF),  (readRTC.dayO));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x07 & 0xFF),  (readRTC.dayT));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x08 & 0xFF),  (readRTC.monthO));
    __delay_cycles(3000);
    I2C1_byteWrite(EEADD, (( i << 4) & 0xFF) | (0x09 & 0xFF),  (readRTC.monthT));
    __delay_cycles(3000);
}
void readFromEEPROM(int i){
    unsigned char red;

    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x09 & 0xFF),  &red);
    printf("\n%x", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x08 & 0xFF),  &red);
    printf("%x/", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x07 & 0xFF),  &red);
    printf("%x", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x06 & 0xFF),  &red);
    printf("%x\t", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x05 & 0xFF),  &red);
    printf("%x", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x04 & 0xFF),  &red);
    printf("%x:", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x03 & 0xFF),  &red);
    printf("%x", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x02 & 0xFF),  &red);
    printf("%x:", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x01 & 0xFF),  &red);
    printf("%x", red);
    __delay_cycles(3000);
    I2C1_byteRead(EEADD, (( i << 4) & 0xFF) | (0x00 & 0xFF),  &red);
    printf("%x\n", red);
    __delay_cycles(3000);
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

        error = I2C1_byteRead(RTCSLAVE_ADDR, 0x11, &readBus);//month and century
        readRTC.temp = ((((readBus >> 4) & 0b01111111)*10) + (readBus & 0x0F));
           error = 1;
           } while (error == 0);

    //comment this out if you dont want it to print the date
    printf("\n\nThe Date is:\n%s\n%s %d%d, 2%d%d%d ", dayOfWeekDecode(readRTC.dayOfWeek),
           monthDecode(readRTC.monthT *10 + readRTC.monthO),
           readRTC.dayT, readRTC.dayO,
           readRTC.yearC, readRTC.yearD, readRTC.yearO);
    printf("%d%d:%d%d:%d%d %s\n", readRTC.hourT, readRTC.hourO,
           readRTC.minT, readRTC.minO,
           readRTC.secT, readRTC.secO, (readRTC.PMAM ? "PM" : "AM"));
    printf("Temperature: %d\n", readRTC.temp);
}

void SetRTC(void){
    setRTC.secT = 3; setRTC.secO = 0;
    setRTC.minT = 4;  setRTC.minO = 5;
    setRTC.hourT = 0; setRTC.hourO = 6;
    setRTC.PMAM = 1;
    setRTC.dayOfWeek = 5;
    setRTC.dayT = 1; setRTC.dayO = 9;
    setRTC.monthT = 1; setRTC.monthO = 1;
    setRTC.yearC = 0; setRTC.yearD = 2; setRTC.yearO = 0;
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
    BUTPORT->IFG &= ~(BUT1PIN | BUT2PIN);
}
