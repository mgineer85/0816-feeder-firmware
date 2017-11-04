#ifndef _GLOBALS_h
#define _GLOBALS_h


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

enum eFeederCommands {
		cmdSetup,
		cmdUpdate,
		cmdActivateFeeder,
		cmdFactoryReset,
		
	};

#endif
