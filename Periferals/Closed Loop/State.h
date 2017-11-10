// State variables for the control loop
#ifndef __STATE_H__
#define __STATE_H__

// Vars
extern bool direction;
extern int duration; // in millis
extern char easingType;
extern bool enabled;
extern int stepNumber;
extern volatile long stepTotal;
extern volatile long theta;
extern volatile long thetaMax;
extern volatile long thetaMin;
extern int maxSpeed;
extern int minSpeed;

// Pins
extern int ledPin;
extern int dirPin;
extern int enablePin;
extern int stepPin;

#endif
