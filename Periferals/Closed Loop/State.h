// State variables for the control loop
#ifndef __STATE_H__
#define __STATE_H__

// Vars
extern bool direction;
extern int duration; // in millis
extern bool enabled;
extern int stepNumber;
extern int stepTotal;
extern long theta;
extern long thetaMax;
extern long thetaMin;
extern int maxSpeed;
extern int minSpeed;

// Pins
extern int ledPin;
extern int dirPin;
extern int enablePin;
extern int stepPin;

#endif

