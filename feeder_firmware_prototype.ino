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
#include "FeedManager.h"


// ------------------  V A R  S E T U P -----------------------

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

// ------------------  S E T U P -----------------------
void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial);
	Serial.println(F("Feeduino starting...")); Serial.flush();

	//factory reset on first start or version changing
	EEPROM.readBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);
	if(commonSettings.version != CONFIG_VERSION) {
		Serial.println(F("First start/Config version changed"));
		
		//reset needed
		FeedManager.factoryReset();

		//update commonSettings in EEPROM to have no factory reset on next start
		EEPROM.writeBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);
	}
	
	//handles the servo controlling stuff
	FeedManager.setup();
	

	
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
	FeedManager.update();
	
	
	if ( debouncedButton.fell() ) {
		FeedManager.feeders[0].advance(2);
	}

}


