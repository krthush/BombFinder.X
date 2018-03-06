#include <xc.h>
#include "dc_motor.h"

void initPWM(int PWMperiod){
	// your code to set up the PWM module
}

// function to set PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
    int PWMduty; //tmp variable to store PWM duty cycle

    PWMduty = (m->power*m->PWMperiod)/100;  //calculate duty cycle (value between 0 and PWMperiod)
    
    if (m->direction) //if forward direction
    {
        LATB=LATB|(1<<(m->dir_pin)); //set dir_pin high in LATB
		PWMduty=m->PWMperiod-PWMduty; //need to invert duty cycle as direction is high (100% power is a duty cycle of 0)
    }
    else //if reverse direction
    {
        LATB=LATB&(~(1<<(m->dir_pin))); //set dir_pin low in LATB
    }

	//write duty cycle value to appropriate registers
    *(m->dutyLowByte)=PWMduty<<2;
    *(m->dutyHighByte)=PWMduty>>6;
}

//increases a motor to full power over a period of time
void setMotorFullSpeed(struct DC_motor *m)
{
	for (m->power; (m->power)<=100; m->power++){ //increase motor power until 100
		setMotorPWM(m);	//pass pointer to m to setMotorSpeed function (not &m)
		__delay_ms(1);	//delay of 1 ms (100 ms from 0 to 100 full power)
	}
}

//function to stop a motor gradually 
void stopMotor(struct DC_motor *m)
{
	// a loop to slow the motor power down to zero
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
	// a loop to slow both motors down to zero power
}

//function to make the robot turn left 
void turnLeft(struct DC_motor *mL, struct DC_motor *mR)
{
	//remember to change the power gradually
}

//function to make the robot turn right 
void turnRight(struct DC_motor *mL, struct DC_motor *mR)
{
	//remember to change the power gradually
}

//function to make the robot go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{
	//remember to change the power gradually
}
