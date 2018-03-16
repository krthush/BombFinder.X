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

// Simple search routine that compares the signal strength of the left IR and
// right IR readers to figure out the direction of the IR beacon.
// Function can be toggled to be:
// - continuous, the drone moves while scanning until there is a change
// - stop n scan, the drone stops every time it scans
char ScanIR(struct DC_motor *mL, struct DC_motor *mR){
    // Initialise variable that is used to judge the strength of signals
    unsigned int SensorResult[2];
    // USERVARIABLE TOLERANCES
    const unsigned int ClearSignalTolerance=200;
    const unsigned int DirectionFoundLimit=1500;
    const unsigned int DirectionFoundTolerance=500;
    
    // Scan Data
//    stop(mL,mR); // TOGGLE: continuous OR stop n scan
    SensorResult[0]=grabLeftIR();
    SensorResult[1]=grabRightIR();
    
    // If there is significant signal
    if ((SensorResult[0]+SensorResult[1])>ClearSignalTolerance) {
        // If both signals are greater than 2500 AND the difference between them
        // is less than the specified DirectionFoundTolerance
        if (((SensorResult[0]>DirectionFoundLimit)&&(SensorResult[1]>DirectionFoundLimit)
            &&(((SensorResult[0]-SensorResult[1])<DirectionFoundTolerance)
                ||((SensorResult[1]-SensorResult[0])<DirectionFoundTolerance)))) {
           return 2; // Direction of bomb is directly ahead
        // Left signal is greater -> turn left
        } else if (SensorResult[0]<=SensorResult[1]) {
           stop(mL,mR); // TOGGLE: continuous OR stop n scan        
           turnLeft(mL,mR); // Turn left
           delay_tenth_s(3);
           stop(mL,mR);
           return 1;
        // Right signal is greater -> turn right
        } else if (SensorResult[0]>SensorResult[1]) {
           stop(mL,mR); // TOGGLE: continuous OR stop n scan
           turnRight(mL,mR); // Turn Right
           delay_tenth_s(3);
           stop(mL,mR);
           return 1;
        }
    } else {
        // No clear signal is found, stop!
        stop(mL,mR);
        // Turn a LARGE distant towards greater signal constant direction
        // Left signal is greater -> turn left
        if (SensorResult[0]<=SensorResult[1]) {       
           turnLeft(mL,mR); // Turn left
           delay_tenth_s(5);
           stop(mL,mR);
           return 0;
        // Right signal is greater -> turn right
        } else if (SensorResult[0]>SensorResult[1]) {
           turnRight(mL,mR); // Turn Right
           delay_tenth_s(5);
           stop(mL,mR);
           return 0;
        }
    }
    return 0;
}

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
    unsigned char ResultFalseL=0;
    unsigned char ResultFalseC=0;
    unsigned char ResultFalseR=0;
    
    // USERVARIABLE TOLERANCES
    const unsigned int ClearSignalTolerance=200;
    const unsigned int DirectionFoundLimit=1500;
    const unsigned int DirectionFoundTolerance=500;
    
//    //Turn on both IR sensors
//    enableSensor(0, 1);
//    enableSensor(1, 1);
    
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
    
//    //Turn off both IR sensors
//    enableSensor(0, 0);
//    enableSensor(1, 0);
    
    // *** PLEASE NOTE: Robot is currently in RIGHT most position ***
    
    // THIS SECTION NEEDS MORE COMMENTING OR PUT IN ANOTHER FUNCTION.
    
    // Check if they results are not valid and are above ClearSignalTolerance
    if((SensorResultL[0]+SensorResultL[1])<ClearSignalTolerance){
        SensorResultL[0]=0;
        SensorResultL[1]=0;
        ResultFalseL=1;
    }
    if((SensorResultC[0]+SensorResultC[1])<ClearSignalTolerance){
        SensorResultC[0]=0;
        SensorResultC[1]=0;
        ResultFalseC=1;
    }
    if((SensorResultR[0]+SensorResultR[1])<ClearSignalTolerance){
        SensorResultR[0]=0;
        SensorResultR[1]=0;
        ResultFalseR=1;
    }

    if ((ResultFalseL+ResultFalseC+ResultFalseR)==3) {
        //No clear signal found, just turn left a lot + move a bit and hope to find it!
        turnLeft(mL,mR);
        delay_tenth_s(5*tenth_seconds);
        stop(mL,mR);
        return 3;
    } else {
        // Logic for robot thinks its found the direction the bomb
        if (((SensorResultL[0]>DirectionFoundLimit)&&(SensorResultL[1]>DirectionFoundLimit)
            &&(((SensorResultL[0]-SensorResultL[1])<DirectionFoundTolerance)
                ||((SensorResultL[1]-SensorResultL[0])<DirectionFoundTolerance)))) {
             // Move left to (left) position as its found direction of bomb
            turnLeft(mL,mR);
            delay_tenth_s(2*(tenth_seconds));
            stop(mL,mR);
            return 2;       
        } else if (((SensorResultC[0]>DirectionFoundLimit)&&(SensorResultC[1]>DirectionFoundLimit)
            &&(((SensorResultC[0]-SensorResultC[1])<DirectionFoundTolerance)
                ||((SensorResultC[1]-SensorResultC[0])<DirectionFoundTolerance)))) {
             // Move left to (center) position as its found direction of bomb
            turnLeft(mL,mR);
            delay_tenth_s(tenth_seconds);
            stop(mL,mR);
            return 2;       
        } else if (((SensorResultR[0]>DirectionFoundLimit)&&(SensorResultR[1]>DirectionFoundLimit)
            &&(((SensorResultR[0]-SensorResultR[1])<DirectionFoundTolerance)
                ||((SensorResultR[1]-SensorResultR[0])<DirectionFoundTolerance)))) {
             // Stay still facing right position as its found direction of bomb
            stop(mL,mR);
            return 2;

        // Logic to check for areas which robot needs to very rough scan
        } else if (SensorResultL[0]>SensorResultL[1]) {
            // Move to centre, then twice as far further to prevent scanning same range again
            turnLeft(mL,mR);
            delay_tenth_s(3*tenth_seconds);
            stop(mL,mR);
            return 0;
        } else if (SensorResultR[1]>SensorResultR[0]) {
            // Go to right, again prevent scanning of same range
            turnRight(mL,mR);
            delay_tenth_s(tenth_seconds);
            stop(mL,mR);        
            return 0;

        // Robot thinks its found rough direction of bomb, will run ScanIR on it
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
    }
    return 0;
}

char BombDirectionFound(unsigned int *SensorResult){
    // USERVARIABLE TOLERANCES
    const unsigned int DirectionFoundLimit=2000;
    const unsigned int DirectionFoundTolerance=500;
    
    return ((SensorResult[0]>DirectionFoundLimit)&&(SensorResult[1]>DirectionFoundLimit)
            &&(((SensorResult[0]-SensorResult[1])<DirectionFoundTolerance)
                ||((SensorResult[1]-SensorResult[0])<DirectionFoundTolerance)));
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


//OLD CODE FOR SCAN WITH RANGE:
// else if((ResultFalseL+ResultFalseC+ResultFalseR)==2) {
//        
//    } else if(ResultFalseC) {
//        // Signal unclear but is apparently in front and not noticed by other directions
//        // Move Forward
//        fullSpeedAhead(mL, mR);
//        delay_tenth_s(5);
//        stop(mL, mR);
//        return 0;
//    } else if(ResultFalseL) {
//        // Signal unclear but is apparently left and not noticed by other directions
//        // Move Left
//        turnLeft(mL,mR);
//        delay_tenth_s(tenth_seconds*3);
//        stop(mL,mR);
//        return 0;
//    } else if(ResultFalseR) {
//        // Signal unclear but is apparently right and not noticed by other directions
//        // Move Left
//        turnRight(mL,mR);
//        delay_tenth_s(tenth_seconds);
//        stop(mL,mR);
//        return 0;
//    } 