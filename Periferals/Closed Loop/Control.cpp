#include <math.h>
#include <ams_as5048b_t3.h>
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
  internalBusy = true;
  if (dir == 1) digitalWrite(dirPin, HIGH);
  else digitalWrite(dirPin, LOW);
  internalBusy = false;
}

void enable(bool en) {
  en = !en;
  enabled = en;
  digitalWrite(enablePin, en);
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
  bool hasErr = amsInstance.hasError;
  if (hasErr != 0) {
    SerialUSB.println("SENSOR READ ERR");
    return 0;
  }
  return amsInstance.angleR(3, true);
}

// returns angles
void printAngle() {
  internalBusy = true;
  int angleRaw = readRawAngle();
  int angleDeg = readDegAngle();
  SerialUSB.print("Angle Raw: ");
  SerialUSB.print(angleRaw);
  SerialUSB.print(", Deg: ");
  SerialUSB.print(angleDeg);
  SerialUSB.println("");
  internalBusy = false;
}
// returns stored motor steps
void printMotorSteps() {
  SerialUSB.print("Motor Total Steps: ");
  SerialUSB.println(stepTotal);
}

// Optional: angle
void setMin(long angle) {
  internalBusy = true;
  thetaMin = angle;
  saveSettings();
  internalBusy = false;
}

// Optional: angle
void setMax(long angle) {
  internalBusy = true;
  thetaMax = angle;
  saveSettings();
  internalBusy = false;
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
  internalBusy = true;

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
  internalBusy = false;
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
  internalBusy = true;
  int tmpStart = readDegAngle();
  SerialUSB.print("tmpStart");
  SerialUSB.println(tmpStart);
  if (!tmpStart) {
    internalBusy = false;
    return;
  }

  // Get steps needed
  int angDiff = tmpStart - ang;
  float angAbs = abs(angDiff);
  
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
  // SerialUSB.print("finAngle: ");
  // SerialUSB.println(finAngle);
  if (finAngle != angI) {
    // Set direction then go
    if (angI > finAngle) setDirection(1);
    if (angI < finAngle) setDirection(0);
    stepTo(angI, 400);
  }
  internalBusy = false;
}





float getCommandPayload(String stringWithPayload) {
  // Extracts the numeric payload from a command
  // Just skip the first character to get the payload, and parse it as a float
  String payload = stringWithPayload.substring(1);
  return payload.toFloat();
}

int getCommandPayloadInt(String stringWithPayload) {
  // Extracts the numeric payload from a command
  // Just skip the first character to get the payload, and parse it as a float
  String payload = stringWithPayload.substring(1);
  return payload.toInt();
}


// for non-peoples
// Handles i2c cmds
// MSG FORMAT: <ID,DIRECTION,MENU-CMD,EXTENDED-DATA,CB-ID>
// Examples:
// 0x10 1 t1200 0 0 -> address 10, dir 1, cmd t1200, extra 0, cb 0
// 0x20 0 e 0 1
// 0x30 0 p 0 1
// 0x40 1 s 1 0
void machineCmdCenter() {
//  digitalWrite(ledPin, HIGH);

  // TODO: REMOVE
  // SerialUSB.print("ADD: ");
  // SerialUSB.print(strArgs[0]);
  // SerialUSB.print(", DIR: ");
  // SerialUSB.print(strArgs[1]);
  // SerialUSB.print(", CMD: ");
  // SerialUSB.print(strArgs[2]);
  // SerialUSB.print(", EXT: ");
  // SerialUSB.print(strArgs[3]);
  // SerialUSB.print(", CB-ID: ");
  // SerialUSB.println(strArgs[4]);

  if (sizeof(strArgs[2]) > 0) {

    // Use cmd args & EXECUTE!
    int cmdInt = 0;
    char cmdChar = strArgs[2][0];
    SerialUSB.print("cmdChar: ");
    SerialUSB.println(cmdChar);
  
    // Format Inputs
    cmdInt = getCommandPayload(strArgs[2]);
    setDirection(atoi(strArgs[1]));
    SerialUSB.print("cmdInt: ");
    SerialUSB.println(cmdInt);
  
    switch (cmdChar) {
      // Quick Tests
      case 'a':
        steps(250);
        break;
      case 's':
        steps(50);
        break;
  
      // Commands
      case 't':
        steps(cmdInt);
        break;
      case 'e':
        enable(enabled);
        break;
      case 'r':
        stepTo(cmdInt, 3000);
        break;
      case 'd':
        setDirection(direction);
        break;
  
      // Settings
      case 'i':
        if (cmdInt > 360) cmdInt = 360;
        if (cmdInt < 0) cmdInt = 0;
        setMin(cmdInt);
        break;
      case 'o':
        if (cmdInt > 360) cmdInt = 360;
        if (cmdInt < 0) cmdInt = 0;
        setMax(cmdInt);
        break;
      case 'n':
        stepNumber = cmdInt;
        break;
    }
  }

  // Cleanup
//  digitalWrite(ledPin, LOW);
}


int counter = 0;
void assignNewData() {
  if (newData == true) {
    newData = false;
    counter = 0;

    // Splitting strings... or splitting hairs?!
    char *token = strtok(receivedData, ",");
    if (token) {
       // You've got to COPY the data pointed to
       strArgs[counter++] = strdup(token);
       // Keep parsing the same string
       token = strtok(NULL, ",");

       while(token) {
          // You've got to COPY the data pointed to
          strArgs[counter++] = strdup(token);
          token = strtok(NULL, ",");
       }

       // If address matches, fire cmd
       char* addy = strArgs[0];
       if (addy[2] == addrs[2]) machineCmdCenter();
    }

    // Reset for future use
    memset(receivedData, 0, sizeof(receivedData));
    counter = 0;
  }
}


void recvWireDataWithMarkers(uint count) {
  boolean recvInProgress = false;
  int ndx = 0;
  char wireData[50];
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  // copy Rx data to receivedData
  Wire1.read(wireData, count);

  // Loop through and only find data that we want
  for (size_t ra = 0; ra < count; ra++) {
    rc = wireData[ra];
    if (rc == endMarker) break;
    if (recvInProgress == true && rc != endMarker) {
      receivedData[ndx] = rc;
      ndx++;
    }
    if (rc == startMarker) recvInProgress = true;
  }
  newData = (sizeof(receivedData) > 1) ? true : false;
  ndx = 0;

  // Let's do this
  if (newData) assignNewData();
}

//
// handle Rx Event (incoming I2C data)
void receiveEvent(uint count) {
  if (internalBusy) return;
  if (count < 1) return;
  memset(receivedData, 0, sizeof(receivedData));
  recvWireDataWithMarkers(count);
}

//
// handle Tx Event (outgoing I2C data)
//void requestEvent() {
//  // fill Tx buffer (send full mem)
//  Wire1.write(receivedData, 50);
//}



// Communicate with Sensors & External
void setupI2C() {
  amsInstance.begin();
  setDirection(0);

  // Listens to External Coms
  Wire1.begin(address);

  // Data init
  received = 0;
  memset(receivedData, 0, sizeof(receivedData));

  // register events
  Wire1.onReceive(receiveEvent);
//  Wire1.onRequest(requestEvent);
}



void serialMenu() {
  SerialUSB.println("---- Closed Loop Control ----");
  SerialUSB.println("");
  SerialUSB.println(" Quick Tests");
  SerialUSB.println(" u  -  steps demo: 3400 default");
  SerialUSB.println(" s  -  50 step distance");
  SerialUSB.println(" a  -  250 step distance");
  SerialUSB.println(" ");
  SerialUSB.println(" Commands");
  SerialUSB.println(" t  -  step input: t0000");
  SerialUSB.println(" e  -  enable toggle");
  SerialUSB.println(" r  -  set point - r000 -> r270, use DEG");
  SerialUSB.println(" d  -  dir: d0 -> 0 = negative | 1 = positive");
  SerialUSB.println(" ");
  SerialUSB.println(" Settings");
  SerialUSB.println(" i  -  set Min - i000 -> i30, use DEG");
  SerialUSB.println(" o  -  set Max - o000 -> o270, use DEG");
  SerialUSB.println(" n  -  set Step Number - n000 -> n3994, use full revolution step count");
  SerialUSB.println(" ");
  SerialUSB.println(" Utilities");
  SerialUSB.println(" m  -  print main menu");
  SerialUSB.println(" p  -  print angle");
  SerialUSB.println(" x  -  print motor step total");
  SerialUSB.println(" c  -  calibrate - finds steps, angle, zero, etc");
  SerialUSB.println("");
}

//Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.
void serialOptions(String stringToParse) {
  int cmdInt = 0;
  char cmdChar = stringToParse.charAt(0);

  // Format Inputs
  cmdInt = getCommandPayload(stringToParse);

  switch (cmdChar) {
    // Quick Tests
    case 'u':
      steps(3400);
      printAngle();
      break;
    case 's':
      steps(50);
      printAngle();
      break;
    case 'a':
      steps(250);
      printAngle();
      break;

    // Commands
    case 't':
      steps(cmdInt);
      printAngle();
      break;
    case 'e':
      enable(enabled);
      break;
    case 'r':
      stepTo(cmdInt, 3000);
      break;
    case 'd':
      direction = cmdInt;
      setDirection(direction);
      break;

    // Settings
    case 'i':
      if (cmdInt > 360) cmdInt = 360;
      if (cmdInt < 0) cmdInt = 0;
      setMin(cmdInt);
      break;
    case 'o':
      if (cmdInt > 360) cmdInt = 360;
      if (cmdInt < 0) cmdInt = 0;
      setMax(cmdInt);
      break;
    case 'n':
      stepNumber = cmdInt;
      printMotorSteps();
      break;

    // Utilities
    case 'm':
      serialMenu();
      break;
    case 'p':
      printAngle();
      break;
    case 'x':
      printMotorSteps();
      break;
    case 'c':
      calibrate();
      break;
  }

}

void serialCheck() {
  if (internalBusy) return;
  // Checks for and processes any serial input
  static String inputString = "";
  static boolean inputStringComplete = false;
  while (SerialUSB.available()) {
    // Read and add the new character
    char inChar = (char)SerialUSB.read();
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

