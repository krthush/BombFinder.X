# BombFinder.X

Working:
-Motor Control
-LCD Control

TODO: (listed in priority)
-IR Signal
-Movement Algorithm
-RFID Reading
-Distance Tracking (and path storage)
-Rotation Tracking

RFID CHECKSUM:
String can be divided into 5 pairs of Hex numbers, e.g.

123456789A -> 0x12 | 0x34 | 0x56 | 0x78 | 0x9A
	=     Hex1 | Hex2 | Hex3 | Hex4 | Hex5
The checksum is calculated by taking the XOR of each of these
numbers with the result of those to is left, i.e.

Step1 = XOR(Hex1,Hex2)
Step2 = XOR(Step1, Hex3)
etc...

For our RFID card the checksum should be 0x60
