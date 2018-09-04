/*=============== main . c ===============*/
/*
BY:	Aravind Dhulipalla
		Micrroprocssor II and Embedded System Design
		Electrical and Computer Engineering Dept.
		UMASS Lowell
PURPOSE         
PIC code for PIC and Glileo Communication
*/

#include <pic16f18857.h>
#include "mcc_generated_files/mcc.h" //default library 

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define value 0x0
#define MSG_ACK 0xE
#define MSG_NOTHING 0xF
#define ADC_Temp 0b0000000000

/*--------------- Timer2_Init ( ) ---------------*/
/*
PURPOSE
Initializes the Timer2 on PIC with auto relod set and prescalar to 0xFF
*/
void Timer2_Init(void)
{
// CCPTMRS0 = 0x01; //SELECTED TIMER 2 FOR PWM
T2CON = 0x80; //CONFIGURED TIMER 2
T2CLKCONbits.CS = 0x01; //clk in relation with osc frequency
T2HLT = 0x00; //TIMER MODE
T2RST = 0x00; //Reset Source
PR2 = 0xFF; //lOAD THE PR2 VALUE
TMR2 = 0x00; //PRESCALE VALUE IS 0
PIR4bits.TMR2IF = 0; // CLEAR THE INTERRUPT FLAG
//T2CONbits.ON = 1; // START THE TIMER
}

/*--------------- PWM_Init( ) ---------------*/
/*
PURPOSE
Initializes the PWM module on PIC.
*/
void PWM_Init(void)
{
CCP1CONbits.EN = 1; // ENABLING THE
CCP1CONbits.FMT = 0; //RIGHT ALLIGNED FORMAT
CCP1CONbits.MODE = 0xF; // SETTING THE MODE TO PWM
CCPR1H = 0x00; // RH TO 0
CCPR1L = 0X00; //RL TO 0
CCPTMRS0 = 0X01; // SELECTS TIMER2
}

/*--------------- PWM_signal_out( ) ---------------*/
/*
Gernerate teh PWM signal with appropriate duty cycle
Input Parameters 
duty  -- Duty cycle to be set
*/
void PWM_signal_out(unsigned int duty)
{
    T2CONbits.ON = 1; // START THE TIMER   
PMD3bits.PWM6MD = 0; //PWM 6 is enabled
CCPR1H = duty >>2; // 2 MSB'S IN CCPR1H
CCPR1L = (duty & 0x0003)<<6; //8 LSB'S IN CCPR1L
}

/*--------------- ADC_conversion_results( ) ---------------*/
/*
PURPOSE
to convert the ADC value to a integer
Returns
ADC value
*/
 int ADC_conversion_results()
{
    TRISAbits.TRISA0 = 1; // SETTING PORTA PIN0 TRISTATE REGISTER TO INPUT
    ANSELAbits.ANSA0 = 1; // SETTING PORTA PIN0 AS A ANALOG INPUT
    ADCON0bits.ADON = 1;  // ACTIVATING THE ADC MODULE
    ADCON0bits.GO = 1;    // START CONVERTING
    while(ADCON0bits.ADGO)// WAIT UNTIL THE CONVERSION
    {
    }
    int b = (ADRESH<<8)+(ADRESL); // MAKE THE ADC RESULT IN 10BITS 
      ADCON0bits.GO = 0; // STOP CONVERTING
    return(b); // RETURN THE RESULT VALUE 
}

/*--------------- ADC_Init( ) ---------------*/

/*
PURPOSE
To initialize the ADC module
*/
void ADC_Init(void)  
{
    ADCON1 = 0x00; // setting control register 1 to 0
    ADCON2 = 0x00; // setting control register 2 to 0
    ADCON3 = 0x00; // setting control register 3 to 0
    ADSTAT = 0x00; // setting threshold register and not overflowed  to 0
    ADCAP  = 0x00; // disabling ADC capacitors
    ADACT  = 0x00; // disabling Auto conversion trigger control register
    ADPRE  = 0x00; // setting precharge time control to 0 
    ADCLK  = 0x00; // setting ADC clk
    ADREF  = 0x00; // setting ADC positive and negative reference voltages 
    ANSELAbits.ANSA0 = 1; // setting ADC analog channel input to 1
    ADCON0 = 0x84; // setting ADCON0 to the required mode.
    
}
 
 /*--------------- set_receive( ) ---------------*/

/*
PURPOSE
To set the GPIO's to as input 
*/
void set_receive()
{  
   //1.set RC6 as digital input
   //2.set RC2, RC3, RC4 and RC5 as digital inputs
   //TRISC = 0xFF; 
    ANSELB = 0x00;
   TRISBbits.TRISB0=1;
   TRISBbits.TRISB1=1;
   TRISBbits.TRISB2=1;
   TRISBbits.TRISB3=1;
   TRISBbits.TRISB4=1;
}

/*--------------- set_send( ) ---------------*/
/*
PURPOSE
To set the GPIO's to as output
*/
void set_send()
{
    ANSELB = 0x00;
    TRISBbits.TRISB0= 0;
    TRISBbits.TRISB1= 0;
    TRISBbits.TRISB2= 0;
    TRISBbits.TRISB3= 0;
}

/*--------------- receive_msg( ) ---------------*/
/*
PURPOSE
Receive the message according to strobe signal 
Returns
The message that is received
*/

unsigned char receive_msg()
{
    /* 1.wait strobe high
    2.wait strobe low
    3.read the data
    4.wait strobe high
    5.return the data*/
    set_receive();
    while(PORTBbits.RB4 == 0);
    unsigned char message = 0x00;
     message = ((PORTBbits.RB0)| (PORTBbits.RB1<<1) | (PORTBbits.RB2<<2) | (PORTBbits.RB3<<3));
    while(PORTBbits.RB4 == 1);
    return message;
}

/*--------------- Strobe( ) ---------------*/
/*
PURPOSE
To send a command message according to strobe signal
Input Parameters : 
Command message to be sent
*/
void Strobe(char message)
{
      ANSELB = 0x00;
    TRISBbits.TRISB0=0;
    TRISBbits.TRISB1=0;
    TRISBbits.TRISB2=0;
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB4 = 1;
  
    while(PORTBbits.RB4==1);
    LATBbits.LATB0 = message & 0x01;
    LATBbits.LATB1 = (message>>1)&0x01;
    LATBbits.LATB2 = (message>>2)&0x01;
    LATBbits.LATB3 = (message>>3)&0x01;
    while(PORTBbits.RB4==0);
}

/*--------------- SendADC( ) ---------------*/
/*
PURPOSE
To send the 10 bit ADC value.
Input Parameters : 
ADC Value
*/
void SendADC(int ADCValue)
{
   
    set_send();
    char a = (ADCValue & 0x0F);
    char b = (ADCValue & 0xF0)>>4;
    char c = (ADCValue & 0x300)>>8;
    Strobe(a);
    Strobe(b);
    Strobe(c);
   }
   
 
// Main program
void main (void)
{
    
    int ADC;
    SYSTEM_Initialize();
    ADC_Init();
    Timer2_Init();
    PWM_Init();
    TRISCbits.TRISC2=0;
    unsigned char msg;  
    while(1)
    {  
        msg=receive_msg();
        switch(msg)
        {
            //Reset
            case 0x00:
            Strobe(MSG_ACK);
             __delay_ms(10);
            SYSTEM_Initialize();
            break;
            //PING
            case 0x01:
            Strobe(MSG_ACK);
            __delay_ms(10);
            break;
            //Get
            case 0x02:
            Strobe(MSG_ACK);
            __delay_ms(10);
            ADC = ADC_conversion_results();
            SendADC(ADC);
            __delay_ms(10);
            break;
            //TURN 30
            case 0x03:
            Strobe(MSG_ACK);
            __delay_ms(10);
            PWM_signal_out(30);
            break;
            //TURN 90    
            case 0x04:
            Strobe(MSG_ACK);
            __delay_ms(10);
            PWM_signal_out(90);
            break;
            //TURN 120    
            case 0x05:
            Strobe(MSG_ACK);
            __delay_ms(10);
            PWM_signal_out(120);
            break;
        } 
    }
}
