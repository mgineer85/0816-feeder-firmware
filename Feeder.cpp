#include "Feeder.h"
#include "config.h"
#include <Servo.h>
#include <EEPROMex.h>

void FeederClass::outputCurrentSettings() {
	Serial.print("A:");
	Serial.print(this->feederSettings.full_advanced_angle);
	Serial.print(" B:");
	Serial.print(this->feederSettings.half_advanced_angle);
	Serial.print(" C:");
	Serial.print(this->feederSettings.retract_angle);
	Serial.print(" U:");
	Serial.print(this->feederSettings.time_to_settle);
	Serial.print(" V:");
	Serial.print(this->feederSettings.motor_min_pulsewidth);
	Serial.print(" W:");
	Serial.print(this->feederSettings.motor_max_pulsewidth);
	Serial.println("");
}

void FeederClass::setup() {
	
	//load settings from eeprom
	this->loadFeederSettings();
	
	#ifdef DEBUG
		Serial.print(F("Loaded settings for feederNo "));
		Serial.print(this->feederNo);
		Serial.print(F(" following"));
		this->outputCurrentSettings();
	#endif
	
	//attach servo to pin
	this->servo.attach(feederPinMap[feederNo],this->feederSettings.motor_min_pulsewidth,this->feederSettings.motor_max_pulsewidth);

	//put on defined position
	this->gotoRetractPosition();
	this->feederState=sAT_RETRACT_POSITION;

	//wait settle time to not having all servos run at the same time if power-supply is not dimensioned adequate
	delay(this->feederSettings.time_to_settle);
}

sFeederSettings FeederClass::getSettings() {
	return this->feederSettings;
}
void FeederClass::setSettings(sFeederSettings UpdatedFeederSettings) {
	this->feederSettings=UpdatedFeederSettings;
}


void FeederClass::loadFeederSettings() {
	uint16_t adressOfFeederSettingsInEEPROM = EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET + this->feederNo * sizeof(this->feederSettings);
	EEPROM.readBlock(adressOfFeederSettingsInEEPROM, this->feederSettings);
}
void FeederClass::saveFeederSettings() {
	uint16_t adressOfFeederSettingsInEEPROM = EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET + this->feederNo * sizeof(this->feederSettings);
	EEPROM.writeBlock(adressOfFeederSettingsInEEPROM, this->feederSettings);
}

void FeederClass::factoryReset() {
	//just save the defaults to eeprom...
	
	this->saveFeederSettings();
}


void FeederClass::gotoRetractPosition() {
	this->servo.write(this->feederSettings.retract_angle);
	#ifdef DEBUG
		Serial.println("going to retract now");
	#endif
}

void FeederClass::gotoHalfAdvancedPosition() {
	this->servo.write(this->feederSettings.half_advanced_angle);
	#ifdef DEBUG
		Serial.println("going to half adv now");
	#endif
}

void FeederClass::gotoFullAdvancedPosition() {
	this->servo.write(this->feederSettings.full_advanced_angle);
	#ifdef DEBUG
		Serial.println("going to full adv now");
	#endif
}

void FeederClass::advance(uint8_t feedLength) {
	
	//check, what to do? if not, return quickly
	if(feedLength==0 && this->remainingFeedLength==0) {
		//nothing to do, just return
		return;
	} else if (feedLength>0 && this->remainingFeedLength>0) {
		//last advancing not completed! ignoring newly received command->return error
		//TODO.
	} else {
		//OK, start new advance-proc
		//feed multiples of 2 possible: 2/4/6/8/10/12,...
		this->remainingFeedLength=feedLength;
	}

	//this->update();
}

void FeederClass::update() {
	
	//if no need for feeding exit fast. take care: if in FULL_ADVANCED_POS and remainingLength is 0 go calcs through to retract automatically
	if(this->remainingFeedLength==0 && this->feederState!=sAT_FULL_ADVANCED_POSITION)
		return;
	
	//state machine-update-stuff (for settle time)
	if(this->lastFeederState!=this->feederState) {
		this->sStateChanged=true;
		this->lastTimePositionChange=millis();
		this->lastFeederState=this->feederState;
	}

	//time to change the position?
	if (millis() - this->lastTimePositionChange >= (unsigned long)this->feederSettings.time_to_settle) {
		//now servo is expected to have settled at its designated position
		
		#ifdef DEBUG
			Serial.print("remainingFeedLength before working: ");
			Serial.println(this->remainingFeedLength);
		#endif
		switch (this->feederState) {
			/* ------------------------------------- RETRACT POS ---------------------- */
			case sAT_RETRACT_POSITION: {

				if(this->remainingFeedLength>=FEEDER_PITCH) {
					//goto full advance-pos
					this->gotoFullAdvancedPosition();
					this->feederState=sAT_FULL_ADVANCED_POSITION;
					this->remainingFeedLength-=FEEDER_PITCH;
				} else if(this->remainingFeedLength>=FEEDER_PITCH/2) {
					//goto half advance-pos
					this->gotoHalfAdvancedPosition();
					this->feederState=sAT_HALF_ADVANCED_POSITION;
					this->remainingFeedLength-=FEEDER_PITCH/2;
				}
				
			}
			break;
			
			/* ------------------------------------- HALF-ADVANCED POS ---------------------- */
			case sAT_HALF_ADVANCED_POSITION: {
				if(this->remainingFeedLength>=FEEDER_PITCH/2) {
					//goto full advance-pos
					this->gotoFullAdvancedPosition();
					this->feederState=sAT_FULL_ADVANCED_POSITION;
					this->remainingFeedLength-=FEEDER_PITCH/2;
				}
			}
			break;
			
			/* ------------------------------------- FULL-ADVANCED POS ---------------------- */
			case sAT_FULL_ADVANCED_POSITION: {
				//just retract after having settled in full-advanced-pos for next task or finishing the current one...
				this->gotoRetractPosition();
				this->feederState=sAT_RETRACT_POSITION;
			}
			break;
			
			default: {
				//state not relevant for advancing...
				//return error, should not occur?
			}
			break;
		}
		
		#ifdef DEBUG
			Serial.print("remainingFeedLength after working: ");
			Serial.println(this->remainingFeedLength);
		#endif
	}
	return;
}


