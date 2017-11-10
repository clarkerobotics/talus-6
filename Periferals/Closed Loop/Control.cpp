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

void setupI2C() {
  amsInstance.begin();
}

// bounces between min/max?
// goes full revolution and returns total steps?
void calibrate() {
  SerialUSB.println("TODO: calibrate");
}

void setDirection(bool dir) {
  SerialUSB.println("TESTING: direction");
  if (dir) digitalWrite(dirPin, HIGH);
  else digitalWrite(dirPin, LOW);
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
float norm(float val) {
  return 1 - ((val - minSpeed) / (maxSpeed - minSpeed));
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

// Execute steps in given direction
void steps(int count, bool dir) {
  SerialUSB.print("TODO: steps ");
  SerialUSB.println(count);
  int half = count / 2;
  float speed = minSpeed;
  float ease = 0;
  float p = 0;

  // Go this wayyyyy
  setDirection(dir);

  // Step through half
  for (int i = 1; i < half; i++) {
    p = norm(i);
    ease = easeInSine(p);
    speed = denorm(ease);
    if (speed < 260) speed = 260;
    step(160);
    delayMicroseconds(speed);
  }

  // Step through other half
  for (int x = half; x > 1; x--) {
    p = norm(x);
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
  SerialUSB.println(" d  -  dir");
  SerialUSB.println(" p  -  print angle");
  SerialUSB.println(" m  -  print main menu");
  SerialUSB.println("");
}




//Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.
void serialCheck() {
  int stepsToGo = 0;

  if (SerialUSB.available()) {

    char inChar = (char)SerialUSB.read();
    String chars = SerialUSB.read();

    switch (inChar) {

      case 'p': //print
        printAngle();
        break;

      case 'u': //steps test
        steps(1400, HIGH);
//        printAngle();
        break;

      case 's': //step
        step(400);
        printAngle();
        break;

      case 't': //steps & amount
        stepsToGo = getCommandPayload(chars);
        // TODO: dir!
        steps(stepsToGo, HIGH);
        // printAngle();
        break;

      case 'd': //dir
        if (direction) {
          direction = false;
        } else {
          direction = true;
        }
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
