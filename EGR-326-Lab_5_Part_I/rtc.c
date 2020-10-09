#include "msp.h"
#include "rtc.h"
#include "keypad.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


// configure UCB1 as I2C communicator
void I2C1_init(void) {
    EUSCI_B1->CTLW0 |= 1;           //disable UCB1 during config
    EUSCI_B1->CTLW0 = 0x0F81;       //7-bit slave addr, master, I2C, synch mode, use SMCLK
    EUSCI_B1->BRW = 30;             //set clock prescaler for 100kHz
    RTCPORT->SEL0 |= (SDAPIN|SCLPIN);               //P6.5, P6.4 for UCB1
    RTCPORT->SEL1 &= ~(SDAPIN|SCLPIN);
    EUSCI_B1->CTLW0 &= ~1;          //enable UCB1 after config
}

// Read one byte of data at memAddr
unsigned char I2C1_byteRead(int slaveAddr, unsigned char memAddr, unsigned char* data) {
    EUSCI_B1->CTLW1 &= ~0x000C;      // no automatic stop generation
    EUSCI_B1->I2CSA = slaveAddr;      // send slave address
    EUSCI_B1->CTLW0 |= 0x0010;        // enable transmitter
    EUSCI_B1->CTLW0 |= 0x0002;        // generate start
    while((EUSCI_B1->CTLW0 & 2));   // wait until slave address is sent
    EUSCI_B1->TXBUF = memAddr;        // send memory address to slave
    while(!(EUSCI_B1->IFG & 2));      // wait till it's ready to transmit
    EUSCI_B1->CTLW0 &= ~0x0010;       // enable receiver
    EUSCI_B1->CTLW0 |= 0x0002;        // generate restart
    while(EUSCI_B1->CTLW0 & 2);       // wait till restart is finished

    EUSCI_B1->CTLW0 |= 0x0004;    // send STOP
    while(!(EUSCI_B1->IFG & 1));  // wait till data is received
    *data++ = EUSCI_B1->RXBUF;
    while(EUSCI_B1->CTLW0 & 4) ;      // wait until stop is sent

    return 0;                   // no error
}
//read the data off I2C
unsigned char I2C1_byteWrite(int slaveAddr, unsigned char memAddr, unsigned char data){
    EUSCI_B1->I2CSA = slaveAddr;      // send slave address
    EUSCI_B1->CTLW0 |= 0x0010;        // enable transmitter
    EUSCI_B1->CTLW0 |= 0x0002;        // generate start and slave address sent
    while((EUSCI_B1->CTLW0 & 2));   // wait until slave address is sent
    EUSCI_B1->TXBUF = memAddr;        // send memory address to slave
    while(!(EUSCI_B1->IFG & 2));      // wait till it's ready to transmit
    EUSCI_B1->TXBUF = data;        // send memory address to slave
    while(!(EUSCI_B1->IFG & 2));      // wait
    EUSCI_B1->CTLW0 |= 0x0004;
    while(EUSCI_B1->CTLW0 & 4) ;      // wait until stop is sent
    return 0;                   // no error
}

//day of the week decoder
char* dayOfWeekDecode(int dayAsNum){
    if(dayAsNum == 1)
        return "Sunday";
    else if(dayAsNum == 2)
        return "Monday";
    else if(dayAsNum == 3)
        return "Tuesday";
    else if(dayAsNum == 4)
        return "Wednesday";
    else if(dayAsNum == 5)
        return "Thursday";
    else if(dayAsNum == 6)
        return "Friday";
    else if(dayAsNum == 7)
        return "Saturday";
    else
        return "Error";
}
//decodes month numerator
char* monthDecode(int monthAsNum){
    if(monthAsNum == 1)
        return "January";
    else if(monthAsNum == 2)
        return "Febuary";
    else if(monthAsNum == 3)
        return "March";
    else if(monthAsNum == 4)
        return "April";
    else if(monthAsNum == 5)
        return "May";
    else if(monthAsNum == 6)
        return "June";
    else if(monthAsNum == 7)
        return "July";
    else if(monthAsNum == 8)
        return "August";
    else if(monthAsNum == 9)
        return "September";
    else if(monthAsNum == 10)
        return "October";
    else if(monthAsNum == 11)
        return "November";
    else if(monthAsNum == 12)
        return "December";
    else
        return "Error";
}
