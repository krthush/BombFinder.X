#include "RFID_Reader.h"

void initRFID(void){
    TRISC = TRISC || 0b11000000; //set data direction registers
                        //both need to be 1 even though RC6
                        //is an output, check the datasheet!

        
    SPBRG=205; //set baud rate to 9600
    SPBRGH=0;
    BAUDCONbits.BRG16=1; //set baud rate scaling to 16 bit mode
    TXSTAbits.BRGH=1; //high baud rate select bit
    RCSTAbits.CREN=1; //continous receive mode
    RCSTAbits.SPEN=1; //enable serial port, other settings default
    TXSTAbits.TXEN=1; //enable transmitter, other settings default
    TXSTAbits.SYNC=0; //Asynchronous
    RCSTAbits.RX9=0; //8-bit reception
    
    
}

void sendCharToSerial(char charToSend){
 while (!PIR1bits.TXIF); // wait for flag to be set
 TXREG=charToSend; //transfer char H to transmitter
}

char getCharSerial(void){
while (!PIR1bits.RCIF); //wait for the data to arrive
    return RCREG; //return byte in RCREG
}

void Serial_String(char *string){
    //While the data pointed to isn't a 0x00 do below
    while(*string != 0){
    //Send out the current byte pointed to
    // and increment the pointer
    sendCharToSerial(*string++);
    __delay_us(50); //so we can see each character
    //being printed in turn (remove delay if you want
    //your message to appear almost instantly)
    }
}

