#include "dc_motor.h"

#pragma config OSC = IRCIO  // internal oscillator

#define PWMcycle 1 //need to calculate this

// function to delay in seconds
//__delay_ms() is limited to a maximum delay of 89ms with an 8Mhz
//clock so you need to write a function to make longer delays
void delay_s(char seconds) {
    unsigned int i=0;
    unsigned int j=0;
    for (i=1; i<=seconds; i++) {
        for (j=1; j<=20; j++) {
            __delay_ms(50);
        }
    }
}

void main(void){

    struct DC_motor motorL, motorR; //declare 2 motor structures
    motorL.power=0; //zero power to start
    motorL.direction=1; //set default motor direction
    motorL.dutyLowByte=(unsigned char *)(&PDC0L); //store address of PWM duty low byte
    motorL.dutyHighByte=(unsigned char *)(&PDC0H); //store address of PWM duty high byte
    motorL.dir_pin=0; //pin RB0/PWM0 controls direction
    motorL.PWMperiod=199; //store PWMperiod for motor
    //same for motorR but different PWM registers and direction pin
    motorR.power=0; //zero power to start
    motorR.direction=1; //set default motor direction
    motorR.dutyLowByte=(unsigned char *)(&PDC1L); //store address of PWM duty low byte
    motorR.dutyHighByte=(unsigned char *)(&PDC1H); //store address of PWM duty high byte
    motorR.dir_pin=2; //pin RB0/PWM0 controls direction
    motorR.PWMperiod=199; //store PWMperiod for motor

    // set bits as outputs
    TRISBbits.RB0=0;
    TRISBbits.RB1=0;
    TRISBbits.RB2=0;
    TRISBbits.RB3=0;

    TRISAbits.RA3=1;
    ANSEL0bits.ANS3=1;
    /* Initialise ADC */
    ADCON0=0b00001101; // Single shot, ADC port channel 3 (AN3), Enable ADC
    ADCON1=0b00000000; // Use Internal Voltage Reference
    // (Vdd and Vss)
    ADCON2=0b10101011; // Right justify result and timing settings

    OSCCON = 0x72; //8MHz clock
    while(!OSCCONbits.IOFS); //wait until stable

    initPWM();  //setup PWM registers

    //some code to set inital values of each structure

   while(1){
	//call your control functions, i.e. fullSpeedAhead(&motorL,&motorR);
        delay_s(2);
        stop(&motorL, &motorR);
        
        delay_s(2);
        fullSpeedAhead(&motorL, &motorR);
        
        delay_s(2);
        stop(&motorL, &motorR);
        
        delay_s(2);
        fullSpeedBack(&motorL, &motorR);
        
        delay_s(2);
        stop(&motorL, &motorR);
        
        delay_s(2);
        turnLeft(&motorL, &motorR);
        
        delay_s(2);
        stop(&motorL, &motorR);
        
        delay_s(2);
        turnRight(&motorL, &motorR);
   }

}
