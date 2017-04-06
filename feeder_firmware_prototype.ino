/*
* Author: Michael Groene
* (c)2017
*
* This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
* http://creativecommons.org/licenses/by-nc-sa/4.0/
*
*
*/


#include "config.h"

// ------------------  I N C  L I B R A R I E S ---------------
#include <HardwareSerial.h>
#include <Bounce2.h>
#include <EEPROMex.h>
#include "Feeder.h"

// ------------------  V A R  S E T U P -----------------------

// ------ Feeders
FeederClass feeders[NUMBER_OF_FEEDERS];

// ------ DEBOUNCE test button
Bounce debouncedButton = Bounce();

// ------ Settings-Struct (saved in EEPROM)
struct sCommonSettings {
	//add further settings here, above CONFIG_VERSION
	
	char version[4];   // This is for detection if settings suit to struct
	} commonSettings = {
	
	//add further settings here, above CONFIG_VERSION
	CONFIG_VERSION,
};

// ------------------  U T I L I T I E S ---------------

// ------ Operate command on all feeders 
void executeCommand(eFeederCommands command) {
	for (uint8_t i=0;i<NUMBER_OF_FEEDERS;i++) {
		switch(command) {
			case cmdSetup:
				feeders[i].setup();
			break;
			case cmdUpdate:
				feeders[i].update();
			break;
			case cmdRetract:
				feeders[i].gotoRetractPosition();
			break;
			case cmdActivateFeeder:
				feeders[i].feederNo=i;
			break;
			case cmdFactoryReset:
				feeders[i].factoryReset();
			break;
			default:
				{}
			break;
		}
	}
}


// ------------------  S E T U P -----------------------
void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial);
	Serial.println(F("Feeduino starting...")); Serial.flush();
	
	// setup listener to serial stream
	setupGCodeProc();
	
	//initialize active feeders, this is giving them an valid ID
	//needs to be done before factory reset to have a valid ID (eeprom-settings location is derived off the ID)
	executeCommand(cmdActivateFeeder);
	
	//factory reset on first start or version changing
	EEPROM.readBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);
	if(strcmp(commonSettings.version,CONFIG_VERSION) != 0) {
		Serial.println(F("First start/Config version changed"));
		
		//reset needed
		executeCommand(cmdFactoryReset);

		//update commonSettings in EEPROM to have no factory reset on next start
		EEPROM.writeBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);
	}
	
	//handles the servo controlling stuff
	executeCommand(cmdSetup);

	#if USE_SERVO_TO_SPOOL_COVER_TAPE == 1 
		//attach servo to pin
		Servo spoolServo;
		spoolServo.attach(SPOOLSERVO_PIN,SPOOLSERVO_MIN_PULSEWIDTH,SPOOLSERVO_MAX_PULSEWIDTH);
		spoolServo.write(SPOOLSERVO_SPEED_RATE);
	#endif
	
	// Setup the button for debugging purposes
	pinMode(PIN_BUTTON,INPUT_PULLUP);
	debouncedButton.attach(PIN_BUTTON);
	debouncedButton.interval(5); // interval in ms
	
	
}



// ------------------  L O O P -----------------------

void loop() {

	debouncedButton.update();
	
	// Process incoming serial data and perform callbacks
	listenToSerialStream();
	
	// Process servo control
	executeCommand(cmdUpdate);
	
	
	if ( debouncedButton.fell() ) {
		feeders[0].advance(4);
	}

}


