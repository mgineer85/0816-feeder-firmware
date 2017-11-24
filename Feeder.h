#ifndef _FEEDER_h
#define _FEEDER_h

#include "arduino.h"
#include "config.h"
#include <Servo.h>
#include <EEPROMex.h>



class FeederClass {
	protected:

			//on initialize it gets a number. Off feederNo the location EEPROM settings are stored is derived. Nothing else so: TODO: make it obsolete
			int feederNo=-1;


			enum tFeederErrorState {
				sOK=0,
				sOK_NOFEEDBACKLINE=1,
				sERROR_IGNORED=2,
				sERROR=-1,
			} ;
			tFeederErrorState getFeederErrorState();

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

	uint8_t remainingFeedLength=0;

	//operational status of the feeder
	enum sFeederState {
		sIDLE,
		sMOVING,
		sADVANCING_CYCLE_COMPLETED,
	} feederState = sIDLE;

	//store the position of the advancing lever
	//last state is stored to enable half advance moves (2mm tapes)
	enum sFeederPosition {
		sAT_UNKNOWN,
		sAT_FULL_ADVANCED_POSITION,
		sAT_HALF_ADVANCED_POSITION,
		sAT_RETRACT_POSITION,

	} lastFeederPosition = sAT_UNKNOWN, feederPosition = sAT_UNKNOWN;

	//store last tinestamp position changed to respect a settle time
	unsigned long lastTimePositionChange;
	
	//some variables for utilizing the feedbackline to feed for setup the feeder...
	uint8_t feedbackLineTickCounter=0;
	unsigned long lastTimeFeedbacklineCheck;
	int lastButtonState;

	//permanently in eeprom stored settings
	sFeederSettings feederSettings = {
		FEEDER_DEFAULT_FULL_ADVANCED_ANGLE,
		FEEDER_DEFAULT_HALF_ADVANCED_ANGLE,
		FEEDER_DEFAULT_RETRACT_ANGLE,
		FEEDER_DEFAULT_FEED_LENGTH,
		FEEDER_DEFAULT_TIME_TO_SETTLE,
		FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH,
		FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH,
		FEEDER_DEFAULT_IGNORE_FEEDBACK,
	};

	Servo servo;

	void initialize(uint8_t _feederNo);
	bool isInitialized();
	bool hasFeedbackLine();
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
	void gotoAngle(uint8_t angle);
	bool advance(uint8_t feedLength, bool overrideError);

	String reportFeederErrorState();
	bool feederIsOk();

  void enable();
  void disable();

	void update();
};

extern FeederClass Feeder;



#endif
