//
//
//

#include "FeedManager.h"
#include "config.h"
#include <Servo.h>
#include "Feeder.h"
#include <EEPROMex.h>

void FeedManagerClass::setup() {
	
	
	
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].setup(i);
	}
	Serial.println(F("FeedManagerClass setup finished."));
	
	lastUpdate = millis()-UPDATE_INTERVAL;	//triggers immediate update when called next.
	
}

void FeedManagerClass::factoryReset() {
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].factoryReset();
	}

	
}

void FeedManagerClass::update() {
	
	if (millis() - lastUpdate >= UPDATE_INTERVAL) {
		lastUpdate=millis();
		
		//do the necessary things...
		
		
	}
	
	return;
}

FeedManagerClass FeedManager;

