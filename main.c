#include "dc_motor.h"

#pragma config OSC = IRCIO  // internal oscillator

#define PWMcycle 1 //need to calculate this

void main(void){

struct DC_motor motorL, motorR; //declare 2 motor structures

OSCCON = 0x72; //8MHz clock
while(!OSCCONbits.IOFS); //wait until stable

initPWM(PWMcycle);  //setup PWM registers

//some code to set inital values of each structure

   while(1){
	//call your control functions, i.e. fullSpeedAhead(&motorL,&motorR);
   }

}
