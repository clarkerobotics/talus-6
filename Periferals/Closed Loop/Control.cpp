#include <math.h>
#include <Wire.h>
#include <ams_as5048b.h>
#include "State.h"

AMS_AS5048B amsInstance;

void setupPins() {
  pinMode(ledPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(stepPin, OUTPUT);
}

void setDirection(int dir) {
  if (dir == 1) digitalWrite(dirPin, HIGH);
  else digitalWrite(dirPin, LOW);
}

void setupI2C() {
  amsInstance.begin();
  setDirection(0);
}

// could choose between easing functions
void easing(char type) {
  SerialUSB.println("TODO: easing");
}

void enable(bool en) {
  SerialUSB.println("TODO: enable");
}

// holds at current position with feedback & torque
// Idea:
// - loop until interrupt
// - check angle
// - run steps if offset
void lock() {
  SerialUSB.println("TODO: lock");
}

// returns raw angle
int readRawAngle() {
  return amsInstance.angleR(1, true);
}

// returns deg angle
int readDegAngle() {
  return amsInstance.angleR(3, true);
}

// returns angles
void printAngle() {
  int angleRaw = readRawAngle();
  int angleDeg = readDegAngle();
  SerialUSB.print("Angle Raw: ");
  SerialUSB.print(angleRaw);
  SerialUSB.print(", Deg: ");
  SerialUSB.print(angleDeg);
  SerialUSB.println("");
}

// Optional: angle
void setMin(long angle) {
  SerialUSB.println("TESTING: setMin");
  thetaMin = angle;
}

// Optional: angle
void setMax(long angle) {
  SerialUSB.println("TESTING: setMax");
  thetaMax = angle;
}

// Easing: easeInSine
float easeInSine(float t) {
  return -1 * cos(t * (M_PI / 2)) + 1;
}

// return number between 0 & 1
float norm(float val, float min, float max) {
  return 1 - ((val - min) / (max - min));
}

// return number between min & max
float denorm(float val) {
  return ((maxSpeed - minSpeed) * val) + minSpeed;
}

// speed is the pulse width offset
void step(int speed) {
  speed = speed || 160;
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(speed);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(speed);
}

// Helps loop steps to simple speed
void loopSteps(int count, int sp) {
  for (int i = 0; i < count; i++) {
    step(160);
    delayMicroseconds(sp);
  }
}

// Execute steps in given direction
// NOTE: Direction must be given first
void steps(int count) {
  SerialUSB.print("TODO: steps ");
  SerialUSB.println(count);
  int half = count / 2;
  int quarter = count / 4;
  float speed = minSpeed;
  float ease = 0;
  float p = 0;

  // Step through half
  for (int i = 1; i < quarter; i++) {
    p = norm(i, 0, quarter);
    ease = easeInSine(p);
    speed = denorm(ease);
    if (speed < 260) speed = 260;
    step(160);
    delayMicroseconds(speed);
  }

  loopSteps(half, speed);

  // Step through other half
  for (int x = quarter; x > 0; x--) {
    p = norm(x, 0, quarter);
    ease = easeInSine(p);
    speed = denorm(ease);
    speed = speed;
    if (speed < 260) speed = 260;
    step(160);
    delayMicroseconds(speed);
  }
}

// Idea:
// - get desired angle, duration, and optional easingType
// - check that it doesnt exceed min/max
// - travel & lock to given angle position
void stepto(long theta, int duration, char easing) {
  SerialUSB.println("TODO: stepto");
}

int getFullRevolutionSteps() {
  // Get zero
  int tmpZero = readDegAngle();
  volatile int tmpAngle = tmpZero + 1;
  volatile int tmpStepsTotal = 0;

  // run initial steps, so we dont read the same initial angle
  for (int x = 0; x < 100; x++) {
    step(160);
    delayMicroseconds(400);
    tmpStepsTotal++;
  }
  
  // Run loop to find step count, only stops if found range
  while (tmpAngle != tmpZero && tmpStepsTotal < 8000) {
    // run more steps
    step(160);
    delayMicroseconds(400);
    tmpStepsTotal++;

    // Check the angle
    tmpAngle = readDegAngle();
  }

  delay(200);
  return tmpStepsTotal;
}

// bounces between min/max?
// goes full revolution and returns total steps?
void calibrate() {
  SerialUSB.println("Calibrating...");
  // Setup
  int avg[5];
  setDirection(1);

  // Get first step total
  avg[0] = getFullRevolutionSteps();

  // Validate step total 4 more times
  avg[1] = getFullRevolutionSteps();
  avg[2] = getFullRevolutionSteps();
  // switch direction for sanity??
  setDirection(0);
  avg[3] = getFullRevolutionSteps();
  avg[4] = getFullRevolutionSteps();

  // Average all steps
  int avgDiv = 5;
  int avgTotal = 0;
  long average = 0;
  for (int x = 0; x < avgDiv; x++) {
    avgTotal += avg[x];
  }
  average = avgTotal / avgDiv;

  // Store step total
  // TODO: EEPROM?
  stepTotal = average;
  SerialUSB.print("Total Steps Found: ");
  SerialUSB.println(stepTotal);

  // calibration complete
  SerialUSB.print("Calibration Complete! :D");
}


float getCommandPayload(String stringWithPayload) {
  // Extracts the numeric payload from a command
  // Just skip the first character to get the payload, and parse it as a float
  String payload = stringWithPayload.substring(1);
  return payload.toFloat();
}

void serialMenu() {
  SerialUSB.println("");
  SerialUSB.println("----- Closed Loop Control -----");
  SerialUSB.println("");
  SerialUSB.println("Menu");
  SerialUSB.println("");
  SerialUSB.println(" s  -  step");
  SerialUSB.println(" u  -  steps: 40 default");
  SerialUSB.println(" t  -  steps: t0000");
  SerialUSB.println(" d  -  dir: 0 | 1");
  SerialUSB.println(" p  -  print angle");
  SerialUSB.println(" ");
  SerialUSB.println(" c  -  calibrate - finds steps, angle, zero, etc");
  SerialUSB.println(" ");
  SerialUSB.println(" m  -  print main menu");
  SerialUSB.println("");
}




//Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.
void serialOptions(String stringToParse) {
  int stepsToGo = 0;
  char inChar = stringToParse.charAt(0);
  SerialUSB.print("inChar: ");
  SerialUSB.println(inChar);

  switch (inChar) {

    case 'c': //calibrate
      calibrate();
      break;

    case 'p': //print
      printAngle();
      break;

    case 'u': //steps test
      steps(3400);
      printAngle();
      break;

    case 's': //step
      step(400);
      printAngle();
      break;

    case 't': //steps & amount
      stepsToGo = getCommandPayload(stringToParse);
      steps(stepsToGo);
      printAngle();
      break;

    case 'd': //dir
      direction = getCommandPayload(stringToParse);
      setDirection(direction);
      break;

    case 'r':             //new setpoint
      SerialUSB.println("Enter setpoint:");
      while (SerialUSB.available() == 0)  {}
//        r = SerialUSB.parseFloat();
//        SerialUSB.println(r);
      break;

    case 'm':
      serialMenu();
      break;

    default:
      break;
  }

}

// for non-peoples
// - Send distance, time
// - lock - auto correct if change
// - easing
// - set min/max
// - calibrate
void serialMachineMenu() {
  SerialUSB.println("TODO: Machine Version!");
}

void serialCheck() {
  // Checks for and processes any serial input
  static String inputString = "";
  static boolean inputStringComplete = false;
  while (Serial.available()) {
    // Read and add the new character
    char inChar = (char)Serial.read();
    inputString += inChar;
    // if the incoming character is a newline, then the string is complete
    if (inChar == '\n') {
      inputStringComplete = true;
      break;                          // Stop here even if more characters are in buffer
    }
  }
  // If a complete message is available, parse it, then clear the buffer before returning
  if (inputStringComplete == true) {
    serialOptions(inputString);
    inputString = "";
    inputStringComplete = false;
  }
}
