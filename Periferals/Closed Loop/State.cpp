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
int dirPin = 14;
int enablePin = 15;
int stepPin = 23;
