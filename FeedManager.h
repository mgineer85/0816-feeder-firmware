// FeedManager.h

#ifndef _FEEDMANAGER_h
#define _FEEDMANAGER_h

#include "arduino.h"
#include "globals.h"
#include "config.h"
#include "Feeder.h"
#include <Servo.h>
#include <EEPROMex.h>



#define UPDATE_INTERVAL 50		//ms


class FeedManagerClass {
	protected:
	
	public:
	
	unsigned long lastUpdate;
	
	FeederClass feeders[NUMBER_OF_FEEDERS];
	
	void activateFeeders();
	void factoryReset();
	void retractAll();
	
	void setup();
	
	void update();
};

extern FeedManagerClass FeedManager;

#endif

