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

	//put on defined position
	this->servo.write(this->feederSettings.retract_angle);      //TODO: one could go to last position if stored in eeprom...
	this->feederState=sAT_RETRACT_POSITION;

  //wait settle time to not having all servos run at the same time if power-supply is not dimensioned adequate
  delay(this->feederSettings.time_to_settle);
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

void FeederClass::advance(uint8_t feedLength) {
	
	static uint8_t pos=0;
	if(pos==0) {
		this->servo.write(this->feederSettings.retract_angle);
		pos=1;
	} else {
		this->servo.write(this->feederSettings.full_advanced_angle);
		pos=0;
	}

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

  

  //state machine-update-stuff (for settle time)
  if(this->lastFeederState!=this->feederState) {
    sStateChanged=true;
    lastTimeStateChange=millis();
    this->lastFeederState=this->feederState;
  }

  //time to change the position?
  if (millis() - lastTimeStateChange >= this->feederSettings.time_to_settle) {
    //now servo is expected to have settled at its designated position
    
  	switch (this->feederState) {
      /* ------------------------------------- RETRACT POS ---------------------- */
      case sAT_RETRACT_POSITION: {

        if(this->remainingFeedLength>=FEEDER_PITCH) {
          //goto full advance-pos
          this->servo.write(this->feederSettings.full_advanced_angle);
          this->feederState=sAT_FULL_ADVANCED_POSITION;
          this->remainingFeedLength-=FEEDER_PITCH;
        } else {
          //goto half advance-pos
          this->servo.write(this->feederSettings.half_advanced_angle);
          this->feederState=sAT_HALF_ADVANCED_POSITION;
          this->remainingFeedLength-=FEEDER_PITCH/2;
        }
        
      }
      break;
  
      /* ------------------------------------- HALF-ADVANCED POS ---------------------- */
  		case sAT_HALF_ADVANCED_POSITION: {
        if(this->remainingFeedLength>=FEEDER_PITCH/2) {
          //goto full advance-pos
          this->servo.write(this->feederSettings.full_advanced_angle);
          this->feederState=sAT_FULL_ADVANCED_POSITION;
          this->remainingFeedLength-=FEEDER_PITCH;
        }
  		}
  		break;
  
      /* ------------------------------------- FULL-ADVANCED POS ---------------------- */
      case sAT_FULL_ADVANCED_POSITION: {
          //just retract after having settled in full-advanced-pos for next task or finishing the current one...
          this->servo.write(this->feederSettings.retract_angle);
          this->feederState=sAT_RETRACT_POSITION;
      }
      break;
  
  		default: {
  			//state not relevant for advancing...
  			//return error, should not occur?
  		}
  		break;
  	}
  }
	return;
}

