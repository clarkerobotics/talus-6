/**
 * Closed Loop Controller
 * Author: @trevorjtclarke
 */
#include "State.h"
#include "Control.h"

// SETUP
void setup() {
  setupPins();                      // configure pins
  digitalWrite(ledPin, HIGH);       // turn LED on

  SerialUSB.begin(115200);
  delay(300);                       // This delay seems to make it easier to establish a connection
  serialMenu();                     // Prints menu to serial monitor
  setupI2C();                       // Communicating with encoder
  retrieveSettings();               // Read stored settings/defaults
  digitalWrite(ledPin, LOW);        // turn LED off
}

// Initialize
void loop() {
  //must have this execute in loop for serial commands to function
  serialCheck();
}

