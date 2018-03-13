#include <xc.h>
#include "dc_motor.h"
#define _XTAL_FREQ 8000000
#include "IR_Reading.h"

void ScanIR(struct DC_motor *mL, struct DC_motor *mR, unsigned char *buf){

    //Make sure we're stationary to begin with
    stop(mL,mR);
    __delay_ms(50);

    //Read IR around centre point
    buf[1] = grabIR();

    //Move a bit to the left
    turnLeft(mL,mR);
    __delay_s(1);
    stop(mL,mR);
    buf[0] = grabIR();

    //Move a bit to the right
    turnRight(mL,mR);
    __delay_s(2); //Twice as long so same angle to the right as was left
    stop(mL,mR);
    buf[2] = grabIR();

    //Move back to starting position and stop
    turnLeft(mL,mR);
    __delay_s(1);
    stop(mL,mR);
}

void main(void){
    unsigned char IR_Result[3];

    ScanIR(&motorL, &motorR, IR_Result); //THis is how to call function

    //If value after left turn is biggest
    if ((IR_Result[0]>IR_Result[1]) & (IR_Result[0]>IR_Result[2])) { //Maybe add some stuff so it's not too sensitive and doesn't end up going back and forth?
        //Do something involving turning left and trying again
    //Else if the centre value is greatest
    } else if ((IR_Result[1]>IR_Result[0]) & (IR_Result[1]>IR_Result[2])) {
        //Go forward for a bit then try again
    //Else the right one is biggest
    } else {
        //Go right then try again
    }
}