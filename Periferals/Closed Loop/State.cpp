// State variables for the control loop

// Vars
bool direction = false;
int duration = 3000;
char easing;
bool enabled = false;
volatile int stepNumber = 0;
volatile long stepTotal = 0;
volatile long theta = 0;
volatile long thetaMax = 0;
volatile long thetaMin = 0;

// Pins
int ledPin = 13;
int dirPin = 14;
int enablePin = 15;
int stepPin = 23;
