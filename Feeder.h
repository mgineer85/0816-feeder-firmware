#ifndef _FEEDER_h
#define _FEEDER_h

#include "arduino.h"
#include "config.h"
#include <Servo.h>



class FeederClass {
	protected:
	
	public:

    
    
		int feederNo=-1;

    uint8_t remainingFeedLength=0;
    
    
		enum sFeederState {
			sAT_UNKNOWN,
			sAT_IDLE,
      sAT_FULL_ADVANCED_POSITION,
      sAT_HALF_ADVANCED_POSITION,
      sGOTO_RETRACT_POSITION,
      sAT_RETRACT_POSITION,
			sAT_ADVANCING,
		
		} lastFeederState= sAT_UNKNOWN, feederState = sAT_UNKNOWN;
    unsigned long lastTimeStateChange;
    bool sStateChanged=false;
    
    //
		struct sFeederSettings {
			//uint8_t pin;
			int full_advanced_angle;
			int half_advanced_angle;
      int retract_angle;
			int time_to_settle;
			int motor_min_pulsewidth;
			int motor_max_pulsewidth;
      //sFeederState lastFeederState;       //save last position to stay there on poweron? needs something not to wear out the eeprom. until now just go to retract pos.
		} feederSettings = {
			//0,
			FEEDER_DEFAULT_FULL_ADVANCED_ANGLE,
			FEEDER_DEFAULT_HALF_ADVANCED_ANGLE,
      FEEDER_DEFAULT_RETRACT_ANGLE,
			FEEDER_DEFAULT_TIME_TO_SETTLE,
			FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH,
			FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH,
      //sAT_UNKNOWN,
		};

    Servo servo;
    
    
		void setup(uint8_t feederNo);
		void loadFeederSettings();
		void saveFeederSettings();
		void factoryReset();
		void advance(uint8_t feedLength);
	
};

extern FeederClass Feeder;



#endif
