/* 
 * File:   RFID_Reader.h
 * Author: tr514
 *
 * Created on 06 March 2018, 15:51
 */

#ifndef RFID_READER_H
#define	RFID_READER_H

void initRFID(void);
void sendCharToSerial(char charToSend);
char getCharSerial(void);
void Serial_String(char *string);

#endif	/* RFID_READER_H */

