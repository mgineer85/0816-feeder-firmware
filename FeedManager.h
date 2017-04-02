// FeedManager.h

#ifndef _FEEDMANAGER_h
#define _FEEDMANAGER_h

#include "arduino.h"
#include "config.h"
#include <Servo.h>


#define UPDATE_INTERVAL 10		//ms


class FeedManagerClass {
	protected:
		
	public:
		
		unsigned long lastUpdate;
		
		enum sFeederState {
			sAT_UNKNOWN_ANGLE,
			sAT_IDLE_ANGLE,
			sMOVING,
			sAT_PULL_ANGLE,

			sAT_HALFPULL_ANGLE,
			
		};
		
		Servo feeders[NUMBER_OF_FEEDERS];
		sFeederState feeders_state[NUMBER_OF_FEEDERS]={sAT_UNKNOWN_ANGLE};

		void setup();
		
		void advance(uint8_t feederNo);
		
		void update();
};

extern FeedManagerClass FeedManager;

#endif

