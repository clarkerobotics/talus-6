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


// Specific Chip Params
extern int address;
extern char* addrs;

// TODO: Refactor this :)
extern int MEM_LEN;
extern int received;
extern int numChars;
extern char receivedData[50];
extern char* strArgs[7];
extern bool newData;
extern bool internalBusy;

#endif

