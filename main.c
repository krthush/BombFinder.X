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

// High priority interrupt routine
void interrupt InterruptHandlerHigh ()
{
    if (PIR1bits.RCIF) {
        ReceivedString[i]=RCREG;
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
    // USERVARIABLE TOLERANCES
    unsigned char ScanAngle=6; // PLEASE NOTE: has to be even, units - tenth seconds
    
    // Enable interrupts
    INTCONbits.GIEH = 1; // Global Interrupt Enable bit
    RCONbits.IPEN = 1; // Enable interrupt priority
    INTCONbits.GIEL = 1; // // Peripheral/Low priority Interrupt Enable bit
    INTCONbits.INT0IE = 1; // INT0 External Interrupt Enables bit
    INTCONbits.PEIE = 1;    // Enable peripheral  interrupts
    
    IPR1bits.RCIP=1; //High Priority+
    PIE1bits.RCIE=1; //Enable interrupt on serial reception
    
    // Initialise Motor Structures
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

    OSCCON = 0x72; //8MHz clock
    while(!OSCCONbits.IOFS); //wait until stable
    
   while(1){
       
       switch (mode) {
           case 0 : //Start-up Mode
               //Initialise EVERYTHING
               initMotorPWM();  //setup PWM registers
               initRFID();
               initIR();
               
               mode = 1;
               enableSensor(0, 1); // DEBUG ONLY - enable sensors to test signals:
               enableSensor(1, 1); // DEBUG ONLY - enable sensors to test signals:
               break;
               
           case 1 : //Search Mode
               
//               if (DirectionFound==0) {
//                   // Scans a wide range if it's unsure about direction
//                   DirectionFound = ScanWithRange(&motorL, &motorR, ScanAngle); // USERVARIABLE
//               } else if (DirectionFound==1) {
//                   // Scans a smaller range when it thinks it's close
//                   DirectionFound = ScanWithRange(&motorL, &motorR, ScanAngle/2); // USERVARIABLE
//               } else if (DirectionFound==2) {
//                   mode=2;
//               }
               
               break;
               
            case 2 : //Move Mode
               //Move forward until RFID read and verified or a certain time
               //has elapsed
                delay_s(3); // DEBUG ONLY
                DirectionFound=1; // DEBUG ONLY
                mode = 1; // DEBUG ONLY - return to mode 2 to check direction of IR
                
                
//                if (ReceivedString[0]==0x02 & ReceivedString[15]==0x03){ //If we have a valid ASCII signal
//                    stop(motorL,motorR); //Stop while checking the RFID result
//                    if (VerifySignal(ReceivedString)){ //and if the checksum is correct
//                        //Put the RFID data into the Message variable
//                         for (i=0; i<10; i++){
//                             Message[i] = ReceivedString[i+1]; 
//                         }
//                        //Clear the received string 
//                         for (i=0; i<16; i++) {
//                             ReceivedString[i]=0;
//                         }
//                         
//                     }// else { //If the signal doesn't check out
////                        fullSpeedBack(motorL,motorR); //Go back a bit then stop
////                        delay_s(1);
////                        stop(motorL,motorR);
////                        fullSpeedAhead(motorL,motorR); //Try again
////                     }  
//                }
               break;
               
            case 3 : //Return Mode
               //Return to original position
               break;
                                      
       }      
   }
}
