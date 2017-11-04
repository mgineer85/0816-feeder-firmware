#ifndef _FEEDER_h
#define _FEEDER_h

#include "arduino.h"
#include "globals.h"
#include "config.h"
#include <Servo.h>



class FeederClass {
	protected:
	
	public:

	
	
	int feederNo=-1;

	uint8_t remainingFeedLength=0;
	
	uint8_t flagAdvancingFinished=0;
	
	
	enum sFeederState {
		sAT_UNKNOWN,
		sAT_IDLE,
		sAT_FULL_ADVANCED_POSITION,
		sAT_HALF_ADVANCED_POSITION,
		sGOTO_RETRACT_POSITION,
		sAT_RETRACT_POSITION,
		sAT_ADVANCING,
		
	} lastFeederState= sAT_UNKNOWN, feederState = sAT_UNKNOWN;
	unsigned long lastTimePositionChange;
	
	//
	sFeederSettings feederSettings = {
		FEEDER_DEFAULT_FULL_ADVANCED_ANGLE,
		FEEDER_DEFAULT_HALF_ADVANCED_ANGLE,
		FEEDER_DEFAULT_RETRACT_ANGLE,
		FEEDER_DEFAULT_FEED_LENGTH,
		FEEDER_DEFAULT_TIME_TO_SETTLE,
		FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH,
		FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH,
		FEEDER_DEFAULT_IGNORE_FEEDBACK,
		//sAT_UNKNOWN,
	};

	Servo servo;
	
	
	void outputCurrentSettings();
	void setup();
	sFeederSettings getSettings();
	void setSettings(sFeederSettings UpdatedFeederSettings);
	void loadFeederSettings();
	void saveFeederSettings();
	void factoryReset();

	void gotoRetractPosition();
	void gotoHalfAdvancedPosition();
	void gotoFullAdvancedPosition();
	void advance(uint8_t feedLength);
	
	uint8_t feederIsOk();
	
	void update();
};

extern FeederClass Feeder;



#endif
