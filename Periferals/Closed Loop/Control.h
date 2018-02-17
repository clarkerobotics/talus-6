#include <math.h>

#ifndef __CONTROL_H__
#define __CONTROL_H__

	// initialize
	void setupPins();
	void setupI2C();

	// bounces between min/max
	void calibrate();

	// - setDirection(bool)
	void setDirection(bool dir);

	// - enable(bool)
	void enable(bool en);

  // Settings
  void saveSettings();
  void retrieveSettings();

	// - lock() // holds at current position with feedback & torque
	void lock();

	// - printAngle() // returns angles
	void printAngle();
  void printMotorSteps();

	// - readRawAngle() // returns raw angle
	int readRawAngle();

	// - readDegAngle() // returns deg angle
	int readDegAngle();

	// - setMin(theta)
	void setMin(long angle);

	// - setMax(theta)
	void setMax(long angle);

	// - step(speed) - speed is the pulse width
	void step(int speed);

	// - steps(count, dir)
	void steps(int count, bool dir);

	// - stepTo(angle, duration)
	void stepTo(int ang, int duration);

	// - serialCheck()
	void serialCheck();

	// - serialMenu()
	void serialMenu();

	// - serialMachineMenu() // for non-peoples
	void serialMachineMenu();

#endif

