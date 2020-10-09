/***********************************************************************************************************
  Title:        EGR 326 Lab 5
  Filename:     Part_I.c, rtc.h, rtc.c, keypad.c, keypad.h
  Author(s):    Mathew J. Yerian-French
  Date:         10/02/2020
  Instructor:   Professor Brian Krug
  Description:  Part I of Lab 5
  Note:
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

    printf("\n\n\nPlease Press '*' to Enter the Time and Date\n\n");
    while(1)
    {
        if(keyPressed)//waits for an interrupt
        {
            keyMain = readKey();//reads the key
            if(!(strcmp(decodeKey(keyMain),"*")))
            {
                   printf("Please enter information as a single digit when "
                            "prompted.\nIf incorrect formating such as an 2 "
                            "for the tens of the month it'll prompt you again.\n");

                   getSetDateTime();

                   printf("\nYou Have Entered:"
                           "\n%s\n%s %d%d, 2%d%d%d %d%d:%d%d:%d%d %s\n",
                          dayOfWeekDecode(setRTC.dayOfWeek),
                          monthDecode(setRTC.monthT *10 + setRTC.monthO),
                          setRTC.dayT, setRTC.dayO,
                          setRTC.yearC, setRTC.yearD, setRTC.yearO,
                          setRTC.hourT, setRTC.hourO,
                          setRTC.minT, setRTC.minO,
                          setRTC.secT, setRTC.secO,
                          (setRTC.PMAM ? "PM" : "AM"));
                   SetRTC();//setDate and time
                   break;
            }
        }
    }
    while(1){
    printf("\n\nPlease Press '*' to See the Time and Date\n");
    while(1){
        if(keyPressed)//waits for an interrupt
        {
            keyMain = readKey();//reads the key
            if(!(strcmp(decodeKey(keyMain),"*")))
            {
                ReadRTC();
                break;
            }
        }
    }
    }

}

void getSetDateTime(void)
{
    state = getMonthT;
    int key;
    int complete = 1;
while(complete)
{
switch(state)
    {
    case getMonthT:
                printf("Please Enter the Ten's of the Month\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if(!(strcmp(decodeKey(key),"X")) ||
                        (atoi(decodeKey(key)) > 1) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));

                    else
                    {
                        setRTC.monthT = atoi(decodeKey(key));
                        printf("%d\n", setRTC.monthT);//print the decoded key
                        key = 0;
                        state = getMonthO;
                        break;
                    }
                }
            }
        break;
    case getMonthO:
                printf("Please Enter the One's of the Month\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if((setRTC.monthT == 0) && (atoi(decodeKey(key)) == 0))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if((setRTC.monthT == 1) && (atoi(decodeKey(key)) > 2))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.monthO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.monthO);//print the decoded key
                        printf("%s\n",monthDecode(setRTC.monthT *10 + setRTC.monthO));
                        key = 0;
                        state = getDayT;
                        break;
                    }
                }
            }
        break;

    case getDayT:
                printf("Please Enter the Ten's of the Day\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if(!(strcmp(decodeKey(key),"X")) ||
                        (atoi(decodeKey(key)) > 3) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.dayT = atoi(decodeKey(key));
                        printf("%d\n", setRTC.dayT);//print the decoded key
                        key = 0;
                        state = getDayO;
                        break;
                    }
                }
            }
        break;
    case getDayO:
                printf("Please Enter the One's of the Day\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if((setRTC.dayT == 0) && (atoi(decodeKey(key)) == 0))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if((setRTC.dayT == 3) && (atoi(decodeKey(key)) > 1))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.dayO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.dayO);//print the decoded key
                        key = 0;
                        state = getDayOfWeek;
                        break;
                    }
                }
            }
        break;
    case getDayOfWeek:
                printf("Please Enter the Day of week from Sun-Saturday (1-7)\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if((atoi(decodeKey(key)) == 0))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if((atoi(decodeKey(key)) > 7))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.dayOfWeek = atoi(decodeKey(key));
                        printf("%s\n", dayOfWeekDecode(setRTC.dayOfWeek));//print the decoded key
                        key = 0;
                        state = getYearC;
                        break;
                    }
                }
            }
        break;
    case getYearC:
                printf("Please Enter the Century of the Year (1 or 0)\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if(!(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")) ||
                        (atoi(decodeKey(key)) > 1))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.yearC = atoi(decodeKey(key));
                        printf("%d\n", setRTC.yearC);//print the decoded key
                        key = 0;
                        state = getYearD;
                        break;
                    }
                }
            }
        break;
    case getYearD:
                printf("Please Enter the Decade of the Year\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if(!(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.yearD = atoi(decodeKey(key));
                        printf("%d\n", setRTC.yearD);//print the decoded key
                        key = 0;
                        state = getYearO;
                        break;
                    }
                }
            }
        break;
    case getYearO:
                printf("Please Enter the One's of the Year\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if(!(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.yearO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.yearO);//print the decoded key
                        key = 0;
                        state = getHourT;
                        break;
                    }
                }
            }
        break;

    case getHourT:
               printf("Please enter time in 12-hour time NOT 24-hour time.\n");
               printf("Please Enter the Ten's of the hours\n->");
           while(1)
           {
               if(keyPressed)//waits for an interrupt
               {
                   key = readKey();//reads the key
                   if(!(strcmp(decodeKey(key),"X")) ||
                       (atoi(decodeKey(key)) > 1) ||
                       !(strcmp(decodeKey(key),"#")) ||
                       !(strcmp(decodeKey(key),"*")))
                           printf("%s is not valid.\nPlease Try Again\n->"
                               , decodeKey(key));
                   else
                   {
                       setRTC.hourT = atoi(decodeKey(key));
                       printf("%d\n", setRTC.hourT);//print the decoded key
                       key = 0;
                       state = getHourO;
                       break;
                   }
               }
           }
        break;
    case getHourO:
                printf("Please Enter the One's of the Hour\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if((setRTC.hourT == 0) && (atoi(decodeKey(key)) == 0))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if((setRTC.hourT == 1) && (atoi(decodeKey(key)) > 2))
                        printf("%s is not valid."
                                "\nPlease Try Again\n->", decodeKey(key));
                    else if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));
                    else
                    {
                        setRTC.hourO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.hourO);//print the decoded key
                        key = 0;
                        state = getMinT;
                        break;
                    }
                }
            }
        break;

    case getMinT:
               printf("Please Enter the Ten's of the Minute\n->");
           while(1)
           {
               if(keyPressed)//waits for an interrupt
               {
                   key = readKey();//reads the key
                   if(!(strcmp(decodeKey(key),"X")) ||
                       (atoi(decodeKey(key)) > 5) ||
                       !(strcmp(decodeKey(key),"#")) ||
                       !(strcmp(decodeKey(key),"*")))
                           printf("%s is not valid.\nPlease Try Again\n->"
                               , decodeKey(key));

                   else
                   {
                       setRTC.minT = atoi(decodeKey(key));
                       printf("%d\n", setRTC.minT);//print the decoded key
                       key = 0;
                       state = getMinO;
                       break;
                   }
               }
           }
        break;
    case getMinO:
                printf("Please Enter the One's of the Minute\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));

                    else
                    {
                        setRTC.minO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.minO);//print the decoded key
                        key = 0;
                        state = getSecT;
                        break;
                    }
                }
            }
        break;

    case getSecT:
                printf("Please Enter the Ten's of the Seconds\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if(!(strcmp(decodeKey(key),"X")) ||
                        (atoi(decodeKey(key)) > 5) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));

                    else
                    {
                        setRTC.secT = atoi(decodeKey(key));
                        printf("%d\n", setRTC.secT);//print the decoded key
                        key = 0;
                        state = getSecO;
                        break;
                    }
                }
            }
        break;
    case getSecO:
                printf("Please Enter the One's of the Seconds\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")))
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));

                    else
                    {
                        setRTC.secO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.secO);//print the decoded key
                        key = 0;
                        state = getPMAM;
                        break;
                    }
                }
            }
        break;
    case getPMAM:
                printf("Please Enter 1 for PM and 0 for AM\n->");
            while(1)
            {
                if(keyPressed)//waits for an interrupt
                {
                    key = readKey();//reads the key
                    if( !(strcmp(decodeKey(key),"X")) ||
                        !(strcmp(decodeKey(key),"#")) ||
                        !(strcmp(decodeKey(key),"*")) ||
                        (atoi(decodeKey(key)) > 1) )
                            printf("%s is not valid.\nPlease Try Again\n->"
                                , decodeKey(key));

                    else
                    {
                        setRTC.PMAM = atoi(decodeKey(key));
                        printf("%s\n", (setRTC.PMAM ? "PM" : "AM"));//print the decoded key
                        key = 0;
                        state = getMonthT;
                        complete = 0;
                        break;
                    }
                }
            }
        break;
    default:
            state = getMonthT;
            break;
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
        printf("\n\nThe Date is:\n%s\n%s %d%d, 2%d%d%d ", dayOfWeekDecode(readRTC.dayOfWeek),
               monthDecode(readRTC.monthT *10 + readRTC.monthO),
               readRTC.dayT, readRTC.dayO,
               readRTC.yearC, readRTC.yearD, readRTC.yearO);
        printf("%d%d:%d%d:%d%d %s\n", readRTC.hourT, readRTC.hourO,
               readRTC.minT, readRTC.minO,
               readRTC.secT, readRTC.secO, (readRTC.PMAM ? "PM" : "AM"));
        //printf("Tempature: %d\n", readRTC.temp);
           error = 1;
           } while (error == 0);
}
void SetRTC(void){

    I2C1_byteWrite(SLAVE_ADDR, 0x00, ((setRTC.secT << 4) + setRTC.secO));//seconds
    I2C1_byteWrite(SLAVE_ADDR, 0x01, ((setRTC.minT << 4) + setRTC.minO));//minutes
    I2C1_byteWrite(SLAVE_ADDR, 0x02, (0b01000000 + (setRTC.PMAM << 5) + (setRTC.hourT << 4) + setRTC.hourO));//hour
    I2C1_byteWrite(SLAVE_ADDR, 0x03, (setRTC.dayOfWeek)); //day of week
    I2C1_byteWrite(SLAVE_ADDR, 0x04, ((setRTC.dayT << 4) + setRTC.dayO));//date
    I2C1_byteWrite(SLAVE_ADDR, 0x05, ((setRTC.yearC << 7) + (setRTC.monthT << 4) + setRTC.monthO));//centery/ month
    I2C1_byteWrite(SLAVE_ADDR, 0x06, ((setRTC.yearD << 4) + setRTC.yearO));//year
}
