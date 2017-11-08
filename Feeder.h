#ifndef _FEEDER_h
#define _FEEDER_h

#include "arduino.h"
#include "config.h"
#include <Servo.h>



class FeederClass {
	protected:
	
	public:

	
	//used to transfer settings between different objects
	struct sFeederSettings {
		int full_advanced_angle;
		int half_advanced_angle;
		int retract_angle;
		uint8_t feed_length;
		int time_to_settle;
		int motor_min_pulsewidth;
		int motor_max_pulsewidth;
		uint8_t ignore_feedback;
		//sFeederState lastFeederState;       //save last position to stay there on poweron? needs something not to wear out the eeprom. until now just go to retract pos.
	};
	
	int feederNo=-1;

	uint8_t remainingFeedLength=0;
	
	uint8_t flagAdvancingFinished=0;
	
	
	enum sFeederState {
		sAT_UNKNOWN,
		sAT_FULL_ADVANCED_POSITION,
		sAT_HALF_ADVANCED_POSITION,
		sAT_RETRACT_POSITION,
		
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

  void gotoPostPickPosition();
	void gotoRetractPosition();
	void gotoHalfAdvancedPosition();
	void gotoFullAdvancedPosition();
	void advance(uint8_t feedLength);
	
	uint8_t feederIsOk();

  void enable();
  void disable();
  
	void update();
};

extern FeederClass Feeder;



#endif
