#include <math.h>
#include <Wire.h>
#include <ams_as5048b.h>
#include <EEPROM.h>
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

void enable(bool en) {
  SerialUSB.println("TODO: enable");
}

// Settings
// Saves all settings in EEPROM
void saveSettings() {
  // Set EEPROM memory valid flag
  EEPROM.write(0, 0x01);
  // Store all numbers in little-endian format
  EEPROM.write(1, lowByte(direction));
  EEPROM.write(2, highByte(direction));
  EEPROM.write(3, lowByte(stepTotal));
  EEPROM.write(4, highByte(stepTotal));
  EEPROM.write(5, lowByte(theta));
  EEPROM.write(6, highByte(theta));
  EEPROM.write(7, lowByte(thetaMax));
  EEPROM.write(8, highByte(thetaMax));
  EEPROM.write(9, lowByte(thetaMin));
  EEPROM.write(10, highByte(thetaMin));
  EEPROM.write(11, lowByte(maxSpeed));
  EEPROM.write(12, highByte(maxSpeed));
  EEPROM.write(13, lowByte(minSpeed));
  EEPROM.write(14, highByte(minSpeed));
}
void retrieveSettings() {
  if (EEPROM.read(0) ==  0x01) {
    // Only load if valid data has been stored in EEPROM
    direction = EEPROM.read(1) + 256 * EEPROM.read(2);
    stepTotal = EEPROM.read(3) + 256 * EEPROM.read(4);
    theta = EEPROM.read(5) + 256 * EEPROM.read(6);
    thetaMax = EEPROM.read(7) + 256 * EEPROM.read(8);
    thetaMin = EEPROM.read(9) + 256 * EEPROM.read(10);
    maxSpeed = EEPROM.read(11) + 256 * EEPROM.read(12);
    minSpeed = EEPROM.read(13) + 256 * EEPROM.read(14);
  } else {
    // NO EEPROM, Use defaults:
    direction = 0;
    stepTotal = 4400;
    theta = 0;
    thetaMax = 0;
    thetaMin = 0;
    maxSpeed = 260;
    minSpeed = 2000;
  }
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
// returns stored motor steps
void printMotorSteps() {
  SerialUSB.print("Motor Total Steps: ");
  SerialUSB.println(stepTotal);
}

// Optional: angle
void setMin(long angle) {
//  SerialUSB.print("setMin: ");
//  SerialUSB.println(angle);
  thetaMin = angle;
  saveSettings();
}

// Optional: angle
void setMax(long angle) {
//  SerialUSB.print("setMax: ");
//  SerialUSB.println(angle);
  thetaMax = angle;
  saveSettings();
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
  return ((minSpeed - maxSpeed) * val) + maxSpeed;
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
  stepTotal = average;
  SerialUSB.print("Total Steps Found: ");
  SerialUSB.println(stepTotal);

  // calibration complete
  digitalWrite(ledPin, HIGH);
  saveSettings();
  SerialUSB.print("Calibration Complete! :D");

  // once EEPROM saved, turn off LED
  delay(500);
  digitalWrite(ledPin, LOW);
}


// stepTo
// - set desired angle, duration
// - check that it doesnt exceed min/max
// - travel & lock to given angle position
void stepTo(int ang, int duration) {
  // Get start point
  int tmpStart = readDegAngle();

  // Get steps needed
  int angDiff = tmpStart - ang;
  float angAbs = abs(angDiff);
//
//  // Set direction
//  int tmpDir = 0;

//  if (angDiff < 0) tmpDir = 1;
//  else tmpDir = 0;
//  setDirection(tmpDir);

//  // Set duration (TODO:)
//  int tmpDur = 3000;
  
  // Calculate & Fire steps
  float stepOffset;
  float angF = ang;
  if (thetaMin != thetaMax) {
    // MIN/MAX LOGIC
    // Logic:
    // 1. remove out of bounds
    // 2. get steps to move from current

    // 1. Determine inside or outside, enforce limits
    if (thetaMin > thetaMax) {

//      // Use stored direction, since we can't assume we know everything here
//      setDirection(direction);

      // SET boundary, if falls between min/max, check direction and truncate to dir TO side
      if (angF <= thetaMin && angF >= thetaMax) {
        // truncate based on direction
        // 0 => negative, 1 => positive
        if (direction == 1) angF = thetaMax;
        if (direction == 0) angF = thetaMin;
      } else if (angF > 360) {
        // This is outside (EX: 340 -> 45)
        angF = 360;
      }

      // 2. Get total steps!
      if (angF >= thetaMin && tmpStart <= thetaMax) {
        // longest (angF = 120, tmpStart = 30)
        stepOffset = ((abs(360 - angF) + tmpStart) / 360) * stepTotal;
      } else if (angF <= thetaMax && tmpStart >= thetaMin) {
        // longest (angF = 15, tmpStart = 210)
        stepOffset = ((abs(360 - tmpStart) + angF) / 360) * stepTotal;
      } else {
        // shortest (ang same side as start)
        stepOffset = (abs(tmpStart - angF) / 360) * stepTotal;
      }
    } else {
      // This is inside (EX: 45 -> 340)
      if (angF < thetaMin) angF = thetaMin;
      if (angF > thetaMax) angF = thetaMax;
      stepOffset = abs((angF - tmpStart) / 360) * stepTotal;
    }
    
  } else if (angAbs < 360) {
    // SIMPLE MOVE LOGIC
    stepOffset = (angAbs / 360) * stepTotal;
  } else {
    // WRAP AROUND LOGIC
    // 1. if < 360, do simple
    // 2. if > 360, calc wrap around
    int rem = ang % 360;
    // types maths requires likes types
    float remF = rem;
    float tmpStartF = tmpStart;
    float remSteps = remF / 360 * stepTotal;
    float angSteps = round(angAbs / 360) * stepTotal;
    float startSteps = tmpStartF / 360 * stepTotal;
    
    // revolution times * total steps + angle total steps - current taken steps
    stepOffset = remSteps + angSteps - startSteps;

    // Reset start angle so when we execute and end up on the far side of 360, its okay
    angF = rem;
  }

  //  crude convert to integer :P
  int goToSteps = stepOffset;
  steps(goToSteps);

  // Finally, angle correction
  delay(40);
  int finAngle = readDegAngle();
  int angI = angF;
  SerialUSB.print("finAngle: ");
  SerialUSB.println(finAngle);
  if (finAngle != angI) {
    // Set direction then go
    if (angI > finAngle) setDirection(1);
    if (angI < finAngle) setDirection(0);
    stepTo(angI, 400);
  }
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
  SerialUSB.println(" s  -  step short distance");
  SerialUSB.println(" u  -  steps demo: 3400 default");
  SerialUSB.println(" t  -  step input: t0000");
  SerialUSB.println(" d  -  dir: d0 -> 0 = negative | 1 = positive");
  SerialUSB.println(" ");
  SerialUSB.println(" c  -  calibrate - finds steps, angle, zero, etc");
  SerialUSB.println(" r  -  set point - r000 -> r270, use DEG");
  SerialUSB.println(" ");
  SerialUSB.println(" m  -  print main menu");
  SerialUSB.println(" p  -  print angle");
  SerialUSB.println(" x  -  print motor step total");
  SerialUSB.println(" ");
  SerialUSB.println(" i  -  set Min - i000 -> i30, use DEG");
  SerialUSB.println(" o  -  set Max - o000 -> i270, use DEG");
  SerialUSB.println("");
}

//Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.
void serialOptions(String stringToParse) {
  int stepsToGo = 0;
  char inChar = stringToParse.charAt(0);

  switch (inChar) {
    case 'c': //calibrate
      calibrate();
      break;
    case 'p': //print
      printAngle();
      break;
    case 'x': //print motor steps
      printMotorSteps();
      break;
    case 'u': //steps test
      steps(3400);
      printAngle();
      break;
    case 's': //step short distance
      steps(50);
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

    case 'r': //new setpoint
      stepsToGo = getCommandPayload(stringToParse);
      stepTo(stepsToGo, 3000);
      break;

    case 'i': // Set Min
      stepsToGo = getCommandPayload(stringToParse);
      if (stepsToGo > 360) stepsToGo = 360;
      if (stepsToGo < 0) stepsToGo = 0;
      setMin(stepsToGo);
      break;

    case 'o': // Set Max
      stepsToGo = getCommandPayload(stringToParse);
      if (stepsToGo > 360) stepsToGo = 360;
      if (stepsToGo < 0) stepsToGo = 0;
      setMax(stepsToGo);
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

