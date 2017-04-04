#ifndef _FEEDER_h
#define _FEEDER_h

#include "arduino.h"
#include "config.h"
#include <Servo.h>



class FeederClass {
	protected:
	
	public:
		int feederNo=-1;
	
		enum sFeederState {
			sAT_UNKNOWN_ANGLE,
			sAT_IDLE_ANGLE,
			sMOVING,
			sAT_PULL_ANGLE,

			sAT_HALFPULL_ANGLE,
		
		} feederState = sAT_UNKNOWN_ANGLE;
	
		Servo servo;
		//sFeederState feeders_state[NUMBER_OF_FEEDERS]={sAT_UNKNOWN_ANGLE};


		struct sFeederSettings {
			//uint8_t pin;
			int angle_idle;
			int angle_pull;
			int time_to_settle;
			int motor_min_pulsewidth;
			int motor_max_pulsewidth;
			} feederSettings = {
			//0,
			FEEDER_DEFAULT_ANGLE_IDLE,
			FEEDER_DEFAULT_ANGLE_PULL,
			FEEDER_DEFAULT_TIME_TO_SETTLE,
			FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH,
			FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH,
		};


		void setup(uint8_t feederNo);
		void loadFeederSettings();
		void saveFeederSettings();
		void factoryReset();
		void advance();
	
};

extern FeederClass Feeder;



#endif
