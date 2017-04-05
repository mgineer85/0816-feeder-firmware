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
		this->feeders[i].setup();
	}
	Serial.println(F("FeedManagerClass setup finished."));
	
	this->lastUpdate = millis()-UPDATE_INTERVAL;	//triggers immediate update when called next.
	
}

void FeedManagerClass::retractAll() {
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].gotoRetractPosition();
	}
}


void FeedManagerClass::activateFeeders() {
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].feederNo=i;
	}
}

void FeedManagerClass::factoryReset() {
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].factoryReset();
	}

	
}

void FeedManagerClass::update() {
	
	//check whether there have to be feeders to feed something?
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].update();
	}
		
	
	return;
}

FeedManagerClass FeedManager;

