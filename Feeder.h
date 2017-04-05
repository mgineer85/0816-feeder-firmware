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
		sFeederSettings feederSettings = {
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
    sFeederSettings FeederClass::getSettings();
    void FeederClass::setSettings(sFeederSettings UpdatedFeederSettings);
		void loadFeederSettings();
		void saveFeederSettings();
		void factoryReset();

    void gotoRetractPosition();
    void gotoHalfAdvancedPosition();
    void gotoFullAdvancedPosition();
		void advance(uint8_t feedLength);
	
};

extern FeederClass Feeder;



#endif
