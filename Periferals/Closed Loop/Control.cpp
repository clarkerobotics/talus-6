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
void calibrate() {
  SerialUSB.println("TODO: calibrate");
}

void setDirection(bool dir) {
  SerialUSB.println("TESTING: direction");
  if (direction) digitalWrite(dirPin, HIGH);
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
void lock() {
  SerialUSB.println("TODO: lock");
}

// returns angles
void printAngle() {
  int angleRaw = amsInstance.angleR(1, true);
  int angleDeg = amsInstance.angleR(3, false);
  SerialUSB.print("Angle Raw: ");
  SerialUSB.print(angleRaw);
  SerialUSB.print(", Deg: ");
  SerialUSB.print(angleDeg);
  SerialUSB.println("");
}

// returns raw angle
int readRawAngle() {
  return amsInstance.angleR(1, true);
}

// returns deg angle
int readDegAngle() {
  return amsInstance.angleR(3, true);
}

// Optional: angle
void setMin(long angle) {
  SerialUSB.println("TODO: setMin");
}

// Optional: angle
void setMax(long angle) {
  SerialUSB.println("TODO: setMax");
}

// speed is the pulse width
void step(int speed) {
  SerialUSB.println("TESTING: step");
  speed = speed || 500;

  digitalWrite(stepPin, HIGH);
  delayMicroseconds(speed);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(speed);
}

void steps(int count, bool dir) {
  SerialUSB.println("TODO: steps");
  // direction(direction);

  // for(int x = 0; x < count; x++) {
  // for(int x = 0; x < 400; x++) {
  //   step(200);
  // }
}

void stepto(long theta, int duration, char easing) {
  SerialUSB.println("TODO: stepto");
}


void serialMenu() {
  SerialUSB.println("");
  SerialUSB.println("----- Closed Loop Control -----");
  SerialUSB.println("");
  SerialUSB.println("Menu");
  SerialUSB.println("");
  SerialUSB.println(" s  -  step");
  SerialUSB.println(" d  -  dir");
  SerialUSB.println(" p  -  print angle");
  SerialUSB.println(" m  -  print main menu");
  SerialUSB.println("");
}




//Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.
void serialCheck() {

  if (SerialUSB.available()) {

    char inChar = (char)SerialUSB.read();

    switch (inChar) {

      case 'p': //print
        printAngle();
        break;

      case 's': //step
        step(400);
        printAngle();
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
