// State variables for the control loop
#ifndef __STATE_H__
#define __STATE_H__

// Vars
extern bool direction;
extern int duration; // in millis
extern char easing;
extern bool enabled;
extern int stepNumber;
extern volatile long stepTotal;
extern volatile long theta;
extern volatile long thetaMax;
extern volatile long thetaMin;

// Pins
int dirPin;
int enablePin;
int stepPin;

#endif
