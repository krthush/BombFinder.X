#include "IR_Reading.h"
#include <xc.h>
#define _XTAL_FREQ 8000000

// PLEASE NOTE: Decided to not use CCP module, 
// using motion capture module instead for IR tracking

//void getTimerVal(void){
//    //Disable interrupt to avoid false interrupt
//    PIE1bits.CCP1IE=0; 
//    //If set to falling edge
//    if ((CCP1CON<<4)>>4 == 0b0100) { 
//        //Reset Timer1
//        TMR1H=0; 
//        TMR1L=0;
//        //Change to rising
//        CCP1CON = 0b0101; //Syntax safe because bits 5 and 6 are unused
//    //Else vice versa
//    } else if ((CCP1CON<<4)>>4 == 0b0101) { 
//        //Store rising timer value
//        TimerVal=(CCPR1H);//<<8); //+ CCPR1L; 
//        CCP1CON = 0b0100;
//    }
//    //Re-enable interrupt
//    PIE1bits.CCP1IE=1;
//}

// Function that initialises the motion capture module such that it can be used
// to obtain IR PWM measurements.
void initIR(void){
    // Note: It is recommended that when initialising the port, 
    // the PORT data latch (LAT or PORT register) should be initialised first, 
    // and then the data direction (TRIS register). 
    // This will eliminate a possible pin glitch, 
    // since the LAT register (PORT data latch values) power up in a random state.
    LATAbits.LA2=0;
    // Set RA2 pin as an input for the IR PWM signal
    TRISAbits.RA2=1;
    // Set port to digital I/O (for AN2 specifically)
    ANSEL0bits.ANS2=0;
    
    // Initialise CAP module for CAP1 pin
    // Disable selected time base Reset on capture
    // Pulse-Width Measurement mode, every falling to rising edge
    CAP1CON=0b00000110;
    
    // CAP module users Timer5, so this must be initialised as well.
    // bit7=0, Timer5 is disabled during Sleep
    // bit6=1, Special Event Trigger Reset is disabled
    // bit5=0, Continuous Count mode is enabled
    // bit4-3=11, 1:8 Prescale
    // bit2=0, Synchronize external clock input
    // bit1=0, Internal clock (TCY)
    // bit0=1, Timer5 is enabled
    T5CON=0b01011001;
}

// Function that gives the strength of the IR signal (PWM pulse length) 
// in arbitrary units 
unsigned int grabIR(void){
    // initialise variables used in function
    unsigned int IR_signal=0;
    unsigned char i=0;
    // measure is taken 4 times (with delay)
    for (i=0; i<4; i++) {
        // combine low and high register into one int using bit shift and OR
        IR_signal+=((CAP1BUFH << 8) | CAP1BUFL);
        __delay_ms(50);
    }
    // return the average signal to be used
    return IR_signal<<2;
}