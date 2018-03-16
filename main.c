#include <xc.h>
#include "dc_motor.h"
#include "RFID_Reader.h"
#include "Movement.h"
#include "IR_Reading.h"
//#include "LCD.h"

#pragma config OSC = IRCIO  // internal oscillator

#define PWMcycle 1 //need to calculate this

volatile unsigned char ReceivedString[16]; //Global variable to read from RFID
volatile unsigned char i=0;
volatile unsigned char RFID_Read=0;

// High priority interrupt routine
void interrupt InterruptHandlerHigh ()
{
    if (PIR1bits.RCIF) {
        ReceivedString[i]=RCREG;
        RFID_Read=1;
        if (i==15){
            i=0;
        }else{
            i++;  
        }
        PIR1bits.RCIF=0;
    }
}

void main(void){
    
    //Initialise Variables
    unsigned char Message[10]; //Code on RFID Card
    unsigned char i=0; //Counter variable
    unsigned char mode=0; //Robot mode - see switch case tree in main loop
    char DirectionFound=0; // Flag for if the robot has decided it knows where the bomb is
    unsigned char SignalStrength[3]; 
    char PathTaken[100]; //Buffer for retaining movement instructions
    unsigned int test=0;
    char MoveTime[100]; //Array to store time spent on each type of movement
    char MoveType[100]; //Array to store movement types - 0 is forwards, 1 is left/right
    char Move=0; //Move counter
    // USERVARIABLE TOLERANCES
    unsigned char ScanAngle=6; // PLEASE NOTE: has to be even, units - tenth seconds
    
    // Enable interrupts
    INTCONbits.GIEH = 1; // Global Interrupt Enable bit
    RCONbits.IPEN = 1; // Enable interrupt priority
    INTCONbits.GIEL = 1; // // Peripheral/Low priority Interrupt Enable bit
    INTCONbits.INT0IE = 1; // INT0 External Interrupt Enables bit
    INTCONbits.PEIE = 1;    // Enable Peripheral  interrupts
    
    IPR1bits.RCIP=1; //High Priority+
    PIE1bits.RCIE=1; //Enable interrupt on serial reception
    PIR1bits.RC1IF = 0;//Clear interrupt flag at start for serial reception
    
    // Initialise Motor Structures
    struct DC_motor mL, mR; //declare 2 motor structures
    mL.power=0; //zero power to start
    mL.direction=1; //set default motor direction
    mL.dutyLowByte=(unsigned char *)(&PDC0L); //store address of PWM duty low byte
    mL.dutyHighByte=(unsigned char *)(&PDC0H); //store address of PWM duty high byte
    mL.dir_pin=0; //pin RB0/PWM0 controls direction
    mL.PWMperiod=199; //store PWMperiod for motor
    //same for motorR but different PWM registers and direction pin
    mR.power=0; //zero power to start
    mR.direction=1; //set default motor direction
    mR.dutyLowByte=(unsigned char *)(&PDC1L); //store address of PWM duty low byte
    mR.dutyHighByte=(unsigned char *)(&PDC1H); //store address of PWM duty high byte
    mR.dir_pin=2; //pin RB0/PWM0 controls direction
    mR.PWMperiod=199; //store PWMperiod for motor

    OSCCON = 0x72; //8MHz clock
    while(!OSCCONbits.IOFS); //wait until stable
    
   while(1){
       
       switch (mode) {
           case 0 : //Start-up Mode
               //Initialise EVERYTHING
               initMotorPWM();  //setup PWM registers
               initRFID();
               initIR(); 
               initLCD();
               initIR();              
               
               // Bot goes forward, stops, then back and stop
               // TODO: do calibration routine here
               fullSpeedAhead(&mL, &mR);
               delay_s(1);
               stop(&mL, &mR);
               fullSpeedBack(&mL, &mR);
               delay_s(1);
               stop(&mL, &mR);
              
               enableSensor(0, 1); // DEBUG ONLY - enable sensors to test signals:
               enableSensor(1, 1); // DEBUG ONLY - enable sensors to test signals:
               mode = 1;  //TODO: Make mode change on button press
               
               break;
               
           case 1 : //Search Mode
               
               if (DirectionFound==0) {
                   // Scans a wide range if it's unsure about direction
                   DirectionFound = ScanWithRange(&mL, &mR, ScanAngle, *MoveTime[Move]); // USERVARIABLE
               } else if (DirectionFound==1) {
                    // Scans a smaller range when it thinks it's close
                    DirectionFound = ScanIR(&mL, &mR); // USERVARIABLE
               } else if (DirectionFound==2) {
                   mode=2;
                   fullSpeedAhead(&mL, &mR);
               }
               
               MoveType[Move] = 1;
               Move++;
//                if (DirectionFound==0) {
//                    // Scans a wide range if it's unsure about direction
//                    DirectionFound = ScanIR(&motorL, &motorR); // USERVARIABLE
//                } else if (DirectionFound==1) {
//                    // Scans a smaller range when it thinks it's close
//                    DirectionFound = ScanIR(&motorL, &motorR); // USERVARIABLE
//                } else if (DirectionFound==2) {
//                    mode=2;
//                }
               
               break;
               
            case 2 : //Move Mode
               //Move forward until RFID read and verified or a certain time
               //has elapsed
//                delay_s(3); // DEBUG ONLY
                if (RFID_Read) {
                    stop(&mL, &mR);
                    if (ReceivedString[0]==0x02 & ReceivedString[15]==0x03){ //If we have a valid ASCII signal
                        if (VerifySignal(ReceivedString)){ //and if the checksum is correct
                            //Put the RFID data into the Message variable
                             for (i=0; i<10; i++){
                                 Message[i] = ReceivedString[i+1]; 
                             }
//                             LCDString(Message); //Display code on LCD
                            //Clear the received string 
                             for (i=0; i<16; i++) {
                                 ReceivedString[i]=0;
                             }
                             mode = 3; //Return to home!

                         } else { //If the signal doesn't check out
                            fullSpeedBack(mL,mR); //Go back a bit then stop
                            delay_tenth_s(5);
                            stop(mL,mR);
                            fullSpeedAhead(mL,mR); //Try again
                         }  
                }
                }
//                DirectionFound=1; // DEBUG ONLY
//                mode = 1; // DEBUG ONLY - return to mode 2 to check direction of IR
               break;
               
            case 3 : //Return Mode
                //Return to original position using MoveType and MoveTime
                for (Move=Move; Move>0; Move--) { //Go backwards along the moves
                    if (MoveType[Move]==0) { //If move was forwards
                        fullSpeedBack(mL,mR);
                        delay_tenth_s(MoveTime[Move]);
                    } else if (MoveType[Move]==1) { //If move was left/right
                        if (MoveTime[Move]>0) { //If left turn
                            turnRight(mL,mR);
                            delay_tenth_s(MoveTime[Move]);
                        } else {
                            turnLeft(mL,mR);
                            delay_tenth_s(MoveTime[Move]);
                        }
                    }
                }
               break;
               
       }      
   }
}
