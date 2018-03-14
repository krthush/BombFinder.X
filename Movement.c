#include <xc.h>
#include "dc_motor.h"
#define _XTAL_FREQ 8000000
#include "IR_Reading.h"
#include "Movement.h"

// Function to delay in seconds
//__delay_ms() is limited to a maximum delay of 89ms with an 8Mhz
//clock so you need to write a function to make longer delays
void delay_s(char seconds) {
    unsigned int i=0;
    for (i=1; i<=seconds*20; i++) {
        // repeat 50ms delay 20 times to match no. of seconds
        __delay_ms(50);
    }
}

// Function similar to delay in seconds, but for a 1/10th of a second
void delay_tenth_s(char tenth_seconds) {
    unsigned int i=0;
    for (i=1; i<=tenth_seconds*2; i++) {
        // repeat 50ms delay 20 times to match no. of tenth seconds
        __delay_ms(50);
    }
}

void ScanIR(struct DC_motor *mL, struct DC_motor *mR, unsigned char *buf){

    //Make sure we're stationary to begin with
    stop(mL,mR);
    __delay_ms(50);

    //Read IR around centre point
    buf[1] = grabRightIR();

    //Move a bit to the left
    turnLeft(mL,mR);
    delay_s(1);
    stop(mL,mR);
    buf[0] = grabRightIR();

    //Move a bit to the right
    turnRight(mL,mR);
    delay_s(2); //Twice as long so same angle to the right as was left
    stop(mL,mR);
    buf[2] = grabRightIR();

    //Move back to starting position and stop
    turnLeft(mL,mR);
    delay_s(1);
    stop(mL,mR);
}

//void main(void){
//    unsigned char IR_Result[3];
//
//    ScanIR(&motorL, &motorR, IR_Result); //THis is how to call function
//
//    //If value after left turn is biggest
//    if ((IR_Result[0]>IR_Result[1]) & (IR_Result[0]>IR_Result[2])) { //Maybe add some stuff so it's not too sensitive and doesn't end up going back and forth?
//        //Do something involving turning left and trying again
//    //Else if the centre value is greatest
//    } else if ((IR_Result[1]>IR_Result[0]) & (IR_Result[1]>IR_Result[2])) {
//        //Go forward for a bit then try again
//    //Else the right one is biggest
//    } else {
//        //Go right then try again
//    }
//}

// Scans IR strength for 3 points (left, centre, right),
// within two times the given range.
// The range is given in twice the number of tenth seconds the robot turns for
// Finally the robot positions facing the direction of highest IR strength
void ScanWithRange(struct DC_motor *mL, struct DC_motor *mR, char tenth_seconds) {
    // Initialise variable that is used to judge the strength of signals
    // Will be strength at left[0], centre[1], right[2]
    unsigned int SignalStrength[2];
    
    //Turn on both IR sensors
    enableSensor(0, 1);
    enableSensor(1, 1);
    
    // Scan Data
    stop(mL,mR);
    delay_tenth_s(tenth_seconds);
    SignalStrength[1]=grabAverageIR();
     
    // Turn left
    turnLeft(mL,mR);
    delay_tenth_s(tenth_seconds);
    
    // Then Scan Data
    stop(mL,mR);
    delay_tenth_s(tenth_seconds);
    SignalStrength[0]=grabAverageIR();
    
    // Turn right (note you must turn for twice as long)
    turnRight(mL,mR);
    delay_tenth_s(2*tenth_seconds);
    
    // Then Scan Data
    stop(mL,mR);
    delay_tenth_s(tenth_seconds);
    SignalStrength[2]=grabAverageIR();
    
    // Return to position of highest IR strength
    if(SignalStrength[2]>SignalStrength[0] && SignalStrength[2]>SignalStrength[1]){
        // Max SignalStrength[2]
        // Remain in current position right[2]
    } else if (SignalStrength[1]>SignalStrength[0] && SignalStrength[1]>SignalStrength[2]) {
        // Max SignalStrength[1]
        // Turn to position centre[1]
        // Turn left for tenth_seconds
        turnLeft(mL,mR);
        delay_tenth_s(tenth_seconds);
        stop(mL,mR);
    } else if (SignalStrength[0]>SignalStrength[1] && SignalStrength[0]>SignalStrength[2]){
        // Max SignalStrength[0]
        // Turn to position left[0]
        // Turn left for tenth_seconds
        turnLeft(mL,mR);
        delay_tenth_s(2*tenth_seconds);
        stop(mL,mR);
    } else {
        // Return to centre position and do nothing
        turnLeft(mL,mR);
        delay_tenth_s(tenth_seconds);
        stop(mL,mR);
    }
    
    //Turn off both IR sensors
    enableSensor(0, 0);
    enableSensor(1, 0);
}

//ALGORITHM PSEUDOCODE
//ScanCentre();
//ScanLeft();
//ScanRight(); //All 3 taking left and right readings
//
//if (left(ScanLeft) > right(ScanLeft)) { //If the left-most reading is highest
//    MoveLeft(2); // Move such that right of new position is left of old one
//    Repeat(); //Return to start
//} else if  (right(ScanRight) > left(ScanRight)){ //If the right-most reading is highest
//    MoveLeft(2); // Move such that the left of new position is right of old one
//    Repeat();
//} else if ((left(ScanLeft) < right(ScanLeft)) & (left(ScanCentre) > right(ScanCentre))) { //If the signal is somewhere between centre and left
//    MoveLeft(1); // Move such that new centre is between old centre and old left
//    RepeatWithSmallerRange(); // shrink the ringe so that it scans a smaller range
//} else if ((right(ScanRight) < left(ScanRight)) & (left(ScanCentre) > right(ScanCentre))) { //If the signal is somewhere between centre and left
//    MoveLeft(1); // Move such that new centre is between old centre and old left
//    RepeatWithSmallerRange(); // shrink the ringe so that it scans a smaller range
//}
//
