/***********************************************************************************************************
  Title:        EGR 326 Lab 5
  Filename:     Part_I.c
  Author(s):    Mathew J. Yerian-French
  Date:         10/02/2020
  Instructor:   Professor Brian Krug
  Description:  Part I of Lab 5
  Note:
************************************************************************************************************
WIRING FOR MSP432
RTC: SCL - PX.X | SDA - PX.X | VCC - 3.3V | GND - GND |
***********************************************************************************************************/
#include "msp.h"
#include "keypad.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SLAVEADD 0x00 //solder pads on A0, A1, and A2 on the module

struct time
{
int monthT,monthO,
    dayT, dayO,
    yearC, yearH, yearD, yearO,
    hourT, hourO,
    minT,minO,
    secT,secO;
};

enum states
{
    getMonthT,getMonthO,
    getDayT, getDayO,
    getYearC, getYearH, getYearD, getYearO,
    getHourT, getHourO,
    getMinT,getMinO,
    getSecT,getSecO
} state;

struct time setRTC;//time to set to the RTC
struct time readRTC;//time read from the RTC

void getSetDateTime(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int keyMain = 0;

    KeyInit();//init keypad
    SysTickInit();
    interON();//turns col on

    NVIC->ISER[1] = 1 << ((PORT2_IRQn) & 31);
    __enable_interrupt();

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
                           "%d%d/%d%d/%d%d%d%d %d%d:%d%d:%d%d\n",
                          setRTC.monthT, setRTC.monthO,
                          setRTC.dayT, setRTC.dayO,
                          setRTC.yearC, setRTC.yearH, setRTC.yearD, setRTC.yearO,
                          setRTC.hourT, setRTC.hourO,
                          setRTC.minT, setRTC.minO,
                          setRTC.secT, setRTC.secO);
            }
        }
    }

}

////configuration of eUSCI as I2C mode
////this code was pulled and heavily commented from professor kandalofts slide with permissions
//EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;// disables the eUSCI0 while configuration
////or holds in a reset state for configuration
//EUSCI_B0->CTLW0 |=  EUSCI_B_CTLW0_MODE_3| // this directly points to I2C mode
//                    EUSCI_B_CTLW0_MST|//master mode select
//                    EUSCI_B_CTLW0_SYNC;//enabling syncornus mode
//EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2;       // Select the SMCLK for the module
//EUSCI_B0->BRW = 0x001E;      // Commands a divder (30) resulting in 100kHz
//EUSCI_B0->I2CSA = SLAVEADD; //slave address will be a macro pointing to the address of the RTC
//EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST; //enables eUSCI0 after config


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
                    else if((setRTC.dayT == 3) && (atoi(decodeKey(key)) > 2))
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
                        state = getYearC;
                        break;
                    }
                }
            }
        break;

    case getYearC:
                printf("Please Enter the Century's of the Year\n->");
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
                        setRTC.yearC = atoi(decodeKey(key));
                        printf("%d\n", setRTC.yearC);//print the decoded key
                        key = 0;
                        state = getYearH;
                        break;
                    }
                }
            }
        break;
    case getYearH:
                printf("Please Enter the Hundred's of the Year\n->");
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
                        setRTC.yearH = atoi(decodeKey(key));
                        printf("%d\n", setRTC.yearH);//print the decoded key
                        key = 0;
                        state = getYearD;
                        break;
                    }
                }
            }
        break;
    case getYearD:
               printf("Please Enter the Decade's of the Year\n->");
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
                printf("Please Enter the Ten's of the Second\n->");
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
                        setRTC.secO = atoi(decodeKey(key));
                        printf("%d\n", setRTC.secO);//print the decoded key
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
