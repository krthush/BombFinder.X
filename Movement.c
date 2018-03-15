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

char ScanIR(struct DC_motor *mL, struct DC_motor *mR){
    // Initialise variable that is used to judge the strength of signals
    unsigned int SensorResult[2];
    // USERVARIABLE TOLERANCES
    unsigned int DirectionFoundTolerance=100;
    
    // Scan Data
    stop(mL,mR);
    delay_s(2);
    SensorResult[0]=grabLeftIR();
    SensorResult[1]=grabRightIR();
    stop(mL,mR);
    
    if (((SensorResult[1]-SensorResult[0])<DirectionFoundTolerance)
            ||((SensorResult[0]-SensorResult[1])<DirectionFoundTolerance)) { // USERVARIABLE
         // Direction of bomb is directly ahead
        return 2;
    // Left signal is greater -> turn left
    } else if (SensorResult[0]<SensorResult[1]) {
        // Turn left
        turnLeft(mL,mR);
        delay_tenth_s(4);
        stop(mL,mR);
        return 0;
    // Right signal is greater -> turn right
    } else if (SensorResult[0]>SensorResult[1]) {
        // Turn left
        turnRight(mL,mR);
        delay_tenth_s(4);
        stop(mL,mR);
        return 0;
    }
     return 0;
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
char ScanWithRange(struct DC_motor *mL, struct DC_motor *mR, char tenth_seconds) {
    // Initialise variable that is used to judge the strength of signals
    // Will be strength of left[0] OR right[1] sensor
    unsigned int SensorResultL[2];
    unsigned int SensorResultC[2];
    unsigned int SensorResultR[2];
    // USERVARIABLE TOLERANCES
    unsigned int DirectionFoundTolerance=500;
    
    //Turn on both IR sensors
    enableSensor(0, 1);
    enableSensor(1, 1);
    // Scan Data
    stop(mL,mR);
    SensorResultC[0]=grabLeftIR();
    SensorResultC[1]=grabRightIR();
    
    // Turn left
    turnLeft(mL,mR);
    delay_tenth_s(tenth_seconds);  
    // Then Scan Data
    stop(mL,mR);
    SensorResultL[0]=grabLeftIR();
    SensorResultL[1]=grabRightIR();
    
    // Turn right (note you must turn for twice as long)
    turnRight(mL,mR);
    delay_tenth_s(2*tenth_seconds);
    // Then Scan Data
    stop(mL,mR);
    SensorResultR[0]=grabLeftIR();
    SensorResultR[1]=grabRightIR();
    
    //Turn off both IR sensors
    enableSensor(0, 0);
    enableSensor(1, 0);
    
    // PLEASE NOTE: Robot is currently in RIGHT most position 
    
    // THIS SECTION NEEDS MORE COMMENTING OR PUT IN ANOTHER FUNCTION.
    
    // Logic for robot thinks its found the direction the bomb
    if (((SensorResultL[1]-SensorResultL[0])<DirectionFoundTolerance)
            ||((SensorResultL[0]-SensorResultL[1])<DirectionFoundTolerance)) { // USERVARIABLE
         // Move left to (right) position as its found direction of bomb
        turnLeft(mL,mR);
        delay_tenth_s(2*(tenth_seconds));
        stop(mL,mR);
        return 2;       
    } else if (((SensorResultC[1]-SensorResultC[0])<DirectionFoundTolerance)
            ||((SensorResultC[0]-SensorResultC[1])<DirectionFoundTolerance)) { // USERVARIABLE
         // Move left to (center) position as its found direction of bomb
        turnLeft(mL,mR);
        delay_tenth_s(tenth_seconds);
        stop(mL,mR);
        return 2;       
    } else if (((SensorResultR[1]-SensorResultR[0])<DirectionFoundTolerance)
            ||((SensorResultR[0]-SensorResultR[1])<DirectionFoundTolerance)) { // USERVARIABLE
         // Stay still facing right position as its found direction of bomb
        stop(mL,mR);
        return 2;
        
    // Logic to check for areas which robot needs to very rough scan
    } else if (((SensorResultL[0]-SensorResultL[1])>5000)&&((SensorResultL[0]-SensorResultC[0])>10000)) { // USERVARIABLE
        // Move to centre, then twice as far further to prevent scanning same range again
        turnLeft(mL,mR);
        delay_tenth_s(3*tenth_seconds);
        stop(mL,mR);
        return 0;
    } else if (((SensorResultR[1]-SensorResultR[0])>5000)&&((SensorResultR[1]-SensorResultC[1])>10000)) { // USERVARIABLE
        // Go to right, again prevent scanning of same range
        turnRight(mL,mR);
        delay_tenth_s(tenth_seconds);
        stop(mL,mR);        
        return 0;
        
    // Logic to check for areas which robot needs to do a more detail scan. 
    } else if ((SensorResultL[1]>SensorResultL[0])&&(SensorResultC[0]>SensorResultC[1])) { 
        // Move to left to (left) inner position for more detailed scanning
        turnLeft(mL,mR);
        delay_tenth_s((3*tenth_seconds)/2);
        stop(mL,mR);
        return 1;
    } else if ((SensorResultR[0]>SensorResultR[1])&&(SensorResultC[1]>SensorResultC[0])) {
        // Move left to (right) inner position for more detailed scanning
        turnLeft(mL,mR);
        delay_tenth_s((tenth_seconds)/2);
        stop(mL,mR);
        return 1;
    }
    
    return 0;
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
