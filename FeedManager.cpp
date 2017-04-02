// 
// 
// 

#include "FeedManager.h"
#include "config.h"
#include <Servo.h>

void FeedManagerClass::setup() {
	
	//for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
	this->feeders[0].attach(FEEDER_1_PIN,FEEDER_1_MOTOR_MIN_PULSEWIDTH,FEEDER_1_MOTOR_MAX_PULSEWITH);
	this->feeders[1].attach(FEEDER_2_PIN,FEEDER_2_MOTOR_MIN_PULSEWIDTH,FEEDER_2_MOTOR_MAX_PULSEWITH);
	Serial.println(F("Arduino Pins attached to control..."));
	
	//put all servos to idle angle
	this->feeders[0].write(FEEDER_1_ANGLE_IDLE);
	this->feeders[1].write(FEEDER_1_ANGLE_IDLE);
	Serial.println(F("Servos reseted to ANGLE_IDLE..."));
	
	
	lastUpdate = millis()-UPDATE_INTERVAL;	//triggers immediate update when called next.
	
}


void FeedManagerClass::advance(uint8_t feederNo) {
	
	sFeederState state = this->feeders_state[feederNo-1];
	static uint8_t pos=0;
	if(pos==0) {
		FeedManager.feeders[0].write(0);
		pos=1;
		} else {
		FeedManager.feeders[0].write(90);
		pos=0;
	}
	
	switch (state) {
		case sAT_UNKNOWN_ANGLE: {
			
		}
		break;
		
		case sAT_IDLE_ANGLE: {
			
		}
		break;
		
		case sMOVING: {
			
		}
		break;
		
		case sAT_PULL_ANGLE: {
			
		}
		break;
		
		case sAT_HALFPULL_ANGLE: {
			
		}
		break;
		
		default: {
			//state not relevant for advancing...
			
		}
		break;
	}
	
	return;
}

void FeedManagerClass::update() {
	
	if (millis() - lastUpdate >= UPDATE_INTERVAL) {
		lastUpdate=millis();
		
		//do the necessary things...
		
		
	}
	
	return;
}

FeedManagerClass FeedManager;

