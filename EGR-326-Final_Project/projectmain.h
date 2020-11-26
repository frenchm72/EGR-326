#ifndef __PROJECTMAIN_H__
#define __PROJECTMAIN_H__

#define BUTPORT P1
#define BUT1PIN BIT1
#define BUT2PIN BIT4

 #define ADC_PORT P5
#define ADC_PIN BIT5
#define ADC_INST 0

#define LEDPORT P5
#define LEDPIN BIT6 //timer A2.1
#define PERIOD (10000-1)

#define SQWPORT P1
#define SQWPIN BIT6

#define DELAY 48000
#define CLEARX 100
#define CLEARY 25
#define STATUSX 30
#define STATUSY 80

void updateTime(void);
void Clock_Init48MHz(void);
void ADC14init(void);
void ReadADC(void);

void initMSP(void){
    __disable_irq();
    Clock_Init48MHz();
    ADC14init();

    BUTPORT->SEL0 &= ~(BUT1PIN | BUT2PIN); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUT1PIN | BUT2PIN);
    BUTPORT->DIR &= ~(BUT1PIN | BUT2PIN);
    BUTPORT->REN |= (BUT1PIN | BUT2PIN);   //enable resistor
    BUTPORT->OUT |= (BUT1PIN | BUT2PIN);   //enable pull up
    BUTPORT->IES |= (BUT1PIN | BUT2PIN);   //set pin as interrupt
    BUTPORT->IE |= (BUT1PIN | BUT2PIN);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUT1PIN | BUT2PIN);  //clears interrupt flag

    SQWPORT->SEL0 &= ~(SQWPIN); //setting up button as interrupt
    SQWPORT->SEL1 &= ~(SQWPIN);
    SQWPORT->DIR &= ~(SQWPIN);
    SQWPORT->REN |= (SQWPIN);   //enable resistor
    SQWPORT->OUT |= (SQWPIN);   //enable pull up
    SQWPORT->IES |= (SQWPIN);   //set pin as interrupt
    SQWPORT->IE |= (SQWPIN);    //enable interrupt for Px.x
    SQWPORT->IFG &= ~(SQWPIN);  //clears interrupt flag

    LEDPORT->SEL0 |= (LEDPIN); //TA2.CCI2A input capture pin, second function
    LEDPORT->SEL1 &= ~(LEDPIN);
    LEDPORT->DIR |= (LEDPIN);

    TIMER_A2->CCR[0]= PERIOD;                   // CCR1 PWM period for LED
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set
    TIMER_A2->CCR[1] = 0;                     // CCR1 PWM duty cycle
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK |TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

    ST7735_InitR(INITR_GREENTAB);
    ST7735_FillScreen(BGCOLOR);
   ST7735_SetTextColor(TXTCOLOR);
    I2C1_init();//init I2C
   //SetRTC();
    rotorInit();
    NVIC->ISER[1] = 1 << ((PORT6_IRQn) & 31);
    NVIC -> ISER[0] |= 1 << ADC14_IRQn;
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    __enable_interrupt();

    I2C1_byteWrite(RTCSLAVE_ADDR, 0x0E, 0x40);//seconds
}

void ADC14init(void){//pulled from my 226
    ADC_PORT -> SEL0 |= ADC_PIN;//set adc conversion
    ADC_PORT -> SEL1 |= ADC_PIN;
    ADC14 -> CTL0 &= ~ADC14_CTL0_ENC;//enable conersion
    ADC14 -> CTL0 |= ADC14_CTL0_PDIV__32 |//setpdiv to 32
                     ADC14_CTL0_SHP |//hold off sampling timer
                     ADC14_CTL0_SSEL__SMCLK |//sub master clock
                     ADC14_CTL0_SHT1__32 |//hol regisert for 32 clock
                     ADC14_CTL0_ON;//turns on
    ADC14 -> CTL1 |= ADC14_CTL1_RES__14BIT;//10 but resolution
    ADC14 -> MCTL[ADC_INST] = 0;//defult config for adc
    ADC14 -> IER0;//interrupt on
    ADC14 -> CTL0 |= ADC14_CTL0_ENC;//enable conversion
}
#endif
