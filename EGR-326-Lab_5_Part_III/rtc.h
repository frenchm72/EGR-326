#ifndef __RTC_H__
#define __RTC_H__

#define RTCPORT P6  //defines keypins for rows
#define SDAPIN BIT4
#define SCLPIN BIT5

#define SLAVE_ADDR 0x68     // 110 1000 7 but address from data sheet

struct time
{
    int monthT,monthO,
        dayT, dayO,
        dayOfWeek,
        yearC, yearD, yearO,
        hourT, hourO,
        minT,minO,
        secT,secO,
        PMAM;
signed int temp;
};

enum states // to run setting the time up
{
    getMonthT,getMonthO,
    getDayT, getDayO,
    getDayOfWeek,
    getYearC, getYearD, getYearO,
    getHourT, getHourO,
    getMinT,getMinO,
    getSecT,getSecO,
    getPMAM
} state;

struct time setRTC;//time to set to the RTC
struct time readRTC;//time read from the RTC

void I2C1_init(void);//functions from book
unsigned char I2C1_byteRead(int slaveAddr, unsigned char memAddr, unsigned char* data);
unsigned char I2C1_byteWrite(int slaveAddr, unsigned char memAddr, unsigned char data);
void ReadRTC(void);
void SetRTC(void);
char* dayOfWeekDecode(int dayAsNum);
char* monthDecode(int monthAsNum);

#endif
