#include "Feeder.h"
#include "config.h"
#include <Servo.h>
#include <EEPROMex.h>

void FeederClass::outputCurrentSettings() {
	Serial.print("M");
	Serial.print(MCODE_UPDATE_FEEDER_CONFIG);
	Serial.print(" N");
	Serial.print(this->feederNo);
	Serial.print(" A");
	Serial.print(this->feederSettings.full_advanced_angle);
	Serial.print(" B");
	Serial.print(this->feederSettings.half_advanced_angle);
	Serial.print(" C");
	Serial.print(this->feederSettings.retract_angle);
	Serial.print(" F");
	Serial.print(this->feederSettings.feed_length);
	Serial.print(" U");
	Serial.print(this->feederSettings.time_to_settle);
	Serial.print(" V");
	Serial.print(this->feederSettings.motor_min_pulsewidth);
	Serial.print(" W");
	Serial.print(this->feederSettings.motor_max_pulsewidth);
	Serial.print(" X");
	Serial.print(this->feederSettings.ignore_feedback);
	Serial.println();
}

void FeederClass::setup() {
	
	//load settings from eeprom
	this->loadFeederSettings();
	
	//feedback input
	//microswitch is active low (NO connected to feedback-pin)
	if(feederFeedbackPinMap[this->feederNo]!=-1) {
		pinMode((uint8_t)feederFeedbackPinMap[this->feederNo],INPUT_PULLUP);
	}
	
	//attach servo to pin
	this->servo.attach(feederPinMap[this->feederNo],this->feederSettings.motor_min_pulsewidth,this->feederSettings.motor_max_pulsewidth);

	//put on defined position
	this->gotoFullAdvancedPosition();
	this->feederState=sAT_FULL_ADVANCED_POSITION;

	//wait a little time not having all servos run at the same time if power-supply is not dimensioned adequate
	delay(50);	//50ms
}

FeederClass::sFeederSettings FeederClass::getSettings() {
	return this->feederSettings;
}
void FeederClass::setSettings(sFeederSettings UpdatedFeederSettings) {
	this->feederSettings=UpdatedFeederSettings;
	
	
	#ifdef DEBUG
		Serial.println(F("updated feeder settings"));
		this->outputCurrentSettings();
	#endif
}


void FeederClass::loadFeederSettings() {
	uint16_t adressOfFeederSettingsInEEPROM = EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET + this->feederNo * sizeof(this->feederSettings);
	EEPROM.readBlock(adressOfFeederSettingsInEEPROM, this->feederSettings);
	
	#ifdef DEBUG
		Serial.println(F("loaded settings from eeprom:"));
		this->outputCurrentSettings();
	#endif
	
}
void FeederClass::saveFeederSettings() {
	uint16_t adressOfFeederSettingsInEEPROM = EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET + this->feederNo * sizeof(this->feederSettings);
	EEPROM.writeBlock(adressOfFeederSettingsInEEPROM, this->feederSettings);
	
	
	#ifdef DEBUG
		Serial.println(F("stored settings to eeprom:"));
		this->outputCurrentSettings();
	#endif
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
	
	//check whether feeder is OK first
	if(!this->feederIsOk()) {
		Serial.print(F("error "));
		Serial.println(F("feeder not OK (not activated, no tape or tension of cover tape not OK)"));
		return;
	}
	
	
	#ifdef DEBUG
		if(this->feederIsOk()) {
			Serial.println(F("feederIsOk = 1 (no error)"));
		} else {
			Serial.println(F("feederIsOk = 0 (error)"));
		}
	#endif
	
	
	//check, what to do? if not, return quickly
	if(feedLength==0 && this->remainingFeedLength==0) {
		//nothing to do, just return
		return;
	} else if (feedLength>0 && this->remainingFeedLength>0) {
		//last advancing not completed! ignore newly received command
		//TODO: one could use a queue
		return;
	} else {
		//OK, start new advance-proc
		//feed multiples of 2 possible: 2/4/6/8/10/12,...
		this->remainingFeedLength=feedLength;
	}

}


uint8_t FeederClass::feederIsOk() {
	if(this->feederSettings.ignore_feedback==1 || feederFeedbackPinMap[this->feederNo]==-1) {
		//no feedback pin defined or feedback shall be ignored
		return 1;
	} else {
		if( digitalRead((uint8_t)feederFeedbackPinMap[this->feederNo]) == LOW ) {
			//the microswitch pulls feedback-pin LOW if tension of cover tape is OK. motor to pull tape is off then
			return 1;
		} else {
			//microswitch is open, this is considered as an error
			return 0;
		}
	}
	
}

void FeederClass::update() {
	
	
	//state machine-update-stuff (for settle time)
	if(this->lastFeederState!=this->feederState) {
		this->lastTimePositionChange=millis();
		this->lastFeederState=this->feederState;
	}

	//time to change the position?
	if (millis() - this->lastTimePositionChange >= (unsigned long)this->feederSettings.time_to_settle) {
		//now servo is expected to have settled at its designated position, so do some stuff
		
		
		if(this->flagAdvancingFinished==1) {
			Serial.println("ok");
			this->flagAdvancingFinished=0;
		}
		
		
		
		//if no need for feeding exit fast.
		if(this->remainingFeedLength==0)
			return;
		
		
		#ifdef DEBUG
			Serial.print("remainingFeedLength before working: ");
			Serial.println(this->remainingFeedLength);
		#endif
		switch (this->feederState) {
			/* ------------------------------------- RETRACT POS ---------------------- */
			case sAT_RETRACT_POSITION: {

				if(this->remainingFeedLength>=FEEDER_MECHANICAL_ADVANCE_LENGTH) {
					//goto full advance-pos
					this->gotoFullAdvancedPosition();
					this->feederState=sAT_FULL_ADVANCED_POSITION;
					this->remainingFeedLength-=FEEDER_MECHANICAL_ADVANCE_LENGTH;
				} else if(this->remainingFeedLength>=FEEDER_MECHANICAL_ADVANCE_LENGTH/2) {
					//goto half advance-pos
					this->gotoHalfAdvancedPosition();
					this->feederState=sAT_HALF_ADVANCED_POSITION;
					this->remainingFeedLength-=FEEDER_MECHANICAL_ADVANCE_LENGTH/2;
				}
				
			}
			break;
			
			/* ------------------------------------- HALF-ADVANCED POS ---------------------- */
			case sAT_HALF_ADVANCED_POSITION: {
				if(this->remainingFeedLength>=FEEDER_MECHANICAL_ADVANCE_LENGTH/2) {
					//goto full advance-pos
					this->gotoFullAdvancedPosition();
					this->feederState=sAT_FULL_ADVANCED_POSITION;
					this->remainingFeedLength-=FEEDER_MECHANICAL_ADVANCE_LENGTH/2;
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
		
		//just finished advancing? set flag to send ok in next run after settle-time to let the pnp go on
		if(this->remainingFeedLength==0) {
			this->flagAdvancingFinished=1;
		}
	}
	
	
	
	return;
}


