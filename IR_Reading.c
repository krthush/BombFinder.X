#include "IR_Reading.h"
#include <xc.h>

void getTimerVal(void){
    //Disable interrupt to avoid false interrupt
    PIE1bits.CCP1IE=0; 
    //If set to falling edge
    if ((CCP1CON<<4)>>4 == 0b0100) { 
        //Reset Timer1
        TMR1H=0; 
        TMR1L=0;
        //Change to rising
        CCP1CON = 0b0101; //Syntax safe because bits 5 and 6 are unused
    //Else vice versa
    } else if ((CCP1CON<<4)>>4 == 0b0101) { 
        //Store rising timer value
        TimerVal=(CCPR1H);//<<8); //+ CCPR1L; 
        CCP1CON = 0b0100;
    }
    //Re-enable interrupt
    PIE1bits.CCP1IE=1;
}