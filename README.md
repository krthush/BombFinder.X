# BombFinder.X

Working:
-Motor Control
-LCD Control
-RFID Reading (including checksum verification)

TODO: (listed in priority)
-IR Signal (almost there - some odd behaviour) - Thush Focus
	Need a function which reads the IR value and returns an int of some kind
-Movement Algorithm
-Distance Tracking (and path storage) - Owen Focus
-Rotation Tracking
-Rewire IR reader so it can turn off and on -> note LCD screen does vice versa

Robot Modes:
0 - Start-up: Initialisation, etc. Should initialise state variables (RDIF read, etc.)
1 - Search Mode: Checks signal ahead and to left and right. Determines region of strongest signal
2 - Move Mode: Moves set distance forward in optimum direction, then returns to search mode unless
	RFID detected, in which case goes to return mode. 
3 - Return Mode: Retraces its steps to return to start point. (Disable CCP and turn on LCD)


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
