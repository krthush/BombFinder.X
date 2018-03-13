/* 
 * File:   Movement.h
 * Author: tr514
 *
 * Created on 13 March 2018, 16:31
 */

#ifndef MOVEMENT_H
#define	MOVEMENT_H

void delay_s(char seconds);
// Scans left, ahead and right for IR signal, stores the obtained data in an
// array 'buf'
void ScanIR(struct DC_motor *mL, struct DC_motor *mR, unsigned char *buf);

#endif	/* MOVEMENT_H */

