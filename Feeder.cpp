#include "Feeder.h"
#include "config.h"
#include <Servo.h>
#include <EEPROMex.h>

void FeederClass::setup(uint8_t feederNo) {

	//store the id (used to load and save eeprom settings).
	this->feederNo=feederNo;
	
	//load settings from eeprom
	this->loadFeederSettings();
	
	//attach servo to pin
	this->servo.attach(feederPinMap[feederNo],this->feederSettings.motor_min_pulsewidth,this->feederSettings.motor_max_pulsewidth);

	//put in idle-angle
	this->servo.write(this->feederSettings.angle_idle);

	this->feederState=sAT_IDLE_ANGLE;
	
}

void FeederClass::loadFeederSettings() {
	uint16_t adressOfFeederSettingsInEEPROM = EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET + feederNo * sizeof(this->feederSettings);
	EEPROM.readBlock(adressOfFeederSettingsInEEPROM, this->feederSettings);
}
void FeederClass::saveFeederSettings() {
	uint16_t adressOfFeederSettingsInEEPROM = EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET + feederNo * sizeof(this->feederSettings);
	EEPROM.writeBlock(adressOfFeederSettingsInEEPROM, this->feederSettings);
}

void FeederClass::factoryReset() {
	//just save the defaults to eeprom...
	this->saveFeederSettings();
}

void FeederClass::advance() {
	
	static uint8_t pos=0;
	if(pos==0) {
		this->servo.write(0);
		pos=1;
		} else {
		this->servo.write(90);
		pos=0;
	}
	
	switch (this->feederState) {
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

