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

void FeedManagerClass::retractAll() {
  for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
    this->feeders[i].gotoRetractPosition();
  }
}

void FeedManagerClass::factoryReset() {
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		this->feeders[i].factoryReset();
	}

	
}

void FeedManagerClass::update() {
	
	if (millis() - lastUpdate >= UPDATE_INTERVAL) {
		lastUpdate=millis();

    //check whether there have to be feeders to feed something?
    for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
      this->feeders[i].advance(0);
    }
    
    
		//do the necessary things...
		
		
	}
	
	return;
}

FeedManagerClass FeedManager;

