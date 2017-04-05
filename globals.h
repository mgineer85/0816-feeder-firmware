#ifndef _GLOBALS_h
#define _GLOBALS_h


//used to transfer settings between different objects
struct sFeederSettings {
	//uint8_t pin;
	int full_advanced_angle;
	int half_advanced_angle;
	int retract_angle;
	uint16_t time_to_settle;
	int motor_min_pulsewidth;
	int motor_max_pulsewidth;
	//sFeederState lastFeederState;       //save last position to stay there on poweron? needs something not to wear out the eeprom. until now just go to retract pos.
};

#endif
