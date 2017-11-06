#ifndef __CONTROL_H__
#define __CONTROL_H__

	// initialize
	void setupPins();
	void setupI2C();

	// - calibrate() ?? // bounces between min/max
	void calibrate();

	// - direction(bool)
	void direction(bool direction);

	// - easing(type) ?? // could choose between easing functions
	void easing(char type);

	// - enable(bool)
	viod enable(bool enabled);

	// - lock() // holds at current position with feedback & torque
	void lock();

	// - printAngle() // returns angles
	void printAngle();

	// - readRawAngle() // returns raw angle
	void readRawAngle();

	// - readDegAngle() // returns deg angle
	void readDegAngle();

	// - setMin([Optional: angle])
	void setMin(long angle);

	// - setMax([Optional: angle])
	void setMax(long angle);

	// - step(speed) - speed is the pulse width
	void step(int speed)

	// - steps(count, dir)
	void steps(int count, bool direction);

	// - goto(theta, duration, easing)
	void stepto(long theta, int duration, char easing);

	// - serialCheck()
	void serialCheck();

	// - serialMenu()
	void serialMenu();

	// - serialMachineMenu() // for non-peoples
	void serialMachineMenu();

#endif