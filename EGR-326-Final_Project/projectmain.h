#ifndef __PROJECTMAIN_H__
#define __PROJECTMAIN_H__

#define WDBUTPORT P3
#define WDBUTPIN BIT7

#define BUTPORT P3
#define BUT1PIN BIT6

 #define ADC_PORT P5
#define ADC_PIN BIT5
#define ADC_INST 0

#define LEDPORT P7
#define LEDPIN BIT7 //timer A1.1
#define PERIOD (10000-1)

#define SQWPORT P1
#define SQWPIN BIT6

#define DELAY 1
#define CLEARX 100
#define CLEARY 25
#define STATUSX 30
#define STATUSY 80

#define HALLPORT P6 //Timer_A2.4
#define HALLPIN BIT7
#define HALLINSTANCE 4

#define TRIGPORT P2 //macros
#define TRIGPIN BIT6
#define ECHOPORT P2
#define ECHOPIN BIT4 //timer A0.2

#define REDPORT P3//break led
#define REDLED BIT5

void updateEXT(int j);
void updateTime(void);
void Clock_Init48MHz(void);
void ADC14init(void);
void ReadADC(void);
void setWDT(void);

void saveToEEPROM(int i, int SPEEDTEMP, int data);
void readFromEEPROM(int i);

void initMSP(void){
    __disable_irq();
    Clock_Init48MHz();

    REDPORT->SEL0 &= ~(REDLED);    //setting up LEDs
    REDPORT->SEL1 &= ~(REDLED);
    REDPORT->DIR |= (REDLED);
    REDPORT->OUT |= (REDLED);

    WDBUTPORT->SEL0 &= ~(WDBUTPIN); //setting up button as interrupt
    WDBUTPORT->SEL1 &= ~(WDBUTPIN);
    WDBUTPORT->DIR &= ~(WDBUTPIN);
    WDBUTPORT->REN |= (WDBUTPIN);   //enable resistor
    WDBUTPORT->OUT |= (WDBUTPIN);   //enable pull up
    WDBUTPORT->IES |= (WDBUTPIN);   //set pin as interrupt
    WDBUTPORT->IE |= (WDBUTPIN);    //enable interrupt for Px.x
    WDBUTPORT->IFG &= ~(WDBUTPIN);  //clears interrupt flag

    BUTPORT->SEL0 &= ~(BUT1PIN); //setting up button as interrupt
    BUTPORT->SEL1 &= ~(BUT1PIN);
    BUTPORT->DIR &= ~(BUT1PIN);
    BUTPORT->REN |= (BUT1PIN);   //enable resistor
    BUTPORT->OUT |= (BUT1PIN);   //enable pull up
    BUTPORT->IES |= (BUT1PIN);   //set pin as interrupt
    BUTPORT->IE |= (BUT1PIN);    //enable interrupt for Px.x
    BUTPORT->IFG &= ~(BUT1PIN);  //clears interrupt flag

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

    TIMER_A1->CCR[0]= PERIOD;                   // CCR1 PWM period for LED
    TIMER_A1->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set
    TIMER_A1->CCR[1] = 0;                     // CCR1 PWM duty cycle
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK |TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

    HALLPORT->SEL0 |= (HALLPIN); //TA2.CCI2A input capture pin, second function
    HALLPORT->SEL1 &= ~(HALLPIN);//TA2.CCI2A input capture pin, second function
    HALLPORT->DIR &= ~(HALLPIN);

    TIMER_A2->CTL |= 0b0000000111100010;
    TIMER_A2->CCTL[HALLINSTANCE] = 0b0100100100010000;

    TRIGPORT->SEL0 &= ~(TRIGPIN);                  //setting up pin as an output
     TRIGPORT->SEL1 &= ~(TRIGPIN);
     TRIGPORT->DIR |= (TRIGPIN);
     TRIGPORT->OUT |= (TRIGPIN);
     TRIGPORT->OUT &= ~(TRIGPIN);

     ECHOPORT->SEL0 |= (ECHOPIN); //TA0.CCI2A input capture pin, second function
     ECHOPORT->SEL1 &= ~(ECHOPIN);//TA0.CCI2A input capture pin, second function
     ECHOPORT->DIR &= ~(ECHOPIN);

     TIMER_A0->CTL |=TIMER_A_CTL_TASSEL_2 | // Use SMCLK as clock source,
                     TIMER_A_CTL_MC_2     | // Start timer in UP mode
                     TIMER_A_CTL_CLR;       // clear TA0R

     TIMER_A0->CCTL[1] =TIMER_A_CCTLN_CM_3    | // Capture rising and falling edge,
                        TIMER_A_CCTLN_CCIS_0  | // Use CCI2A
                        TIMER_A_CCTLN_CCIE    | // Enable capture interrupt
                        TIMER_A_CCTLN_CAP     | // Enable capture mode,
                        TIMER_A_CCTLN_SCS;      // Synchronous capture

    ST7735_InitR(INITR_GREENTAB);
    Output_On();
   //splash screen
   ST7735_SetRotation(0);
   ST7735_DrawBitmap(0, 160, gvlogo , 128, 160);
    I2C1_init();//init I2C
   //SetRTC();
    rotorInit();
    ADC14init();
    SysTickInit();
    initWhiteStepper();
    NVIC->ISER[1] = 1 << ((PORT6_IRQn) & 31);
    NVIC -> ISER[0] |= 1 << ADC14_IRQn;
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA2_N_IRQn) & 31); // Enable interrupt in NVIC vector
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31); // Enable interrupt in NVIC vector
    __enable_interrupt();
    __enable_irq ( );//enable global interrupt

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

void setWDT(void){
    WDT_A->CTL = 0x5A00      // Watchdog Password
                 | 1<<5                   //Set to ACLK
                 | 0<<4                   //Set to Watchdog mode
                 | 1<<3                   // Clear Timer
                 | 4;
}

#endif
