#include <xc.h>
#include "dc_motor.h"
#include "RFID_Reader.h"
//#include "LCD.h"

#pragma config OSC = IRCIO  // internal oscillator

#define PWMcycle 1 //need to calculate this

volatile unsigned char ReceivedString[16]; //Global variable to read from RFID
volatile unsigned char i=0;

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
    unsigned char SignalStrength[3]; 
    char PathTaken[100]; //Buffer for retaining movement instructions
    
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
               
               mode = 1;
               break;
               
           case 1 : //Search Mode
               //Search for strongest signal
               SignalStrength = ScanIR(&motorL, &motorR, SignalStrength);
               break;
               
            case 2 : //Move Mode
               //Move forward until RFID read and verified or a certain time
               //has elapsed
                if (ReceivedString[0]==0x02 & ReceivedString[15]==0x03){ //If we have a valid ASCII signal
                    if (VerifySignal(ReceivedString)){ //and if the checksum is correct
                        //Put the RFID data into the Message variable
                         for (i=0; i<10; i++){
                             Message[i] = ReceivedString[i+1]; 
                         }
                        //Clear the received string 
                         for (i=0; i<16; i++) {
                             ReceivedString[i]=0;
                         }
                     }   
                }
               break;
               
            case 3 : //Return Mode
               //Return to original position
               break;
                                      
       }      
   }
}
