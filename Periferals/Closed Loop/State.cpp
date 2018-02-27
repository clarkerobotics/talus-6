#include <String.h>
// State variables for the control loop

// Vars
int direction = 0;
int duration = 3000;
bool enabled = false;
int stepTotal = 0;
int stepNumber = 0;
long theta = 0;
long thetaMax = 0;
long thetaMin = 0;

// Speed: in microseconds
// lower number means faster!
// depends on full/half/quarter stepping
int maxSpeed = 260;
int minSpeed = 2000;

// Pins
int ledPin = 13;
int enablePin = 14;
int stepPin = 3;
int dirPin = 4;

//int dirPin = 14;
//int enablePin = 16;
//int stepPin = 15;

// Specific Chip Params
int address = 0x10;
const char* addrs = "0x10";

// TODO: Refactor this :)
const int MEM_LEN = 50;
const int numChars = MEM_LEN;
volatile int received;
volatile char receivedData[numChars];
volatile char* strArgs[7];
volatile bool newData = false;
volatile bool internalBusy = false;
