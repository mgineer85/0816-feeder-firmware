/*
 *
 *
 *
 *
 *
 *
 *
*/


#include "config.h"

// ------------------  I N C  L I B R A R I E S ---------------
#include <SoftwareSerial.h>
#include <Bounce2.h>
#include <EEPROMex.h>
#include <CmdMessenger.h>

#include "FeedManager.h"



// ------------------  V A R  S E T U P -----------------------

// ------ DEBOUNCE test button
Bounce debouncedButton = Bounce(); 

// ------ Settings-Struct (saved in EEPROM)
struct sCommonSettings {
    char version[4];   // This is for mere detection if they are your settings
} commonSettings = {
    CONFIG_VERSION,
};

// ------ cmdMessenger
char field_separator   = ' ';
char command_separator = '\n';
char escape_separator = '/';

// setup cmdMessenger, listen to serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);

// commandlist
enum {
	// Commands
	kAcknowledge=0,             // Command to acknowledge that cmd was received
	kError=1,                   // Command to report errors
	kAdvance=GCODE_ADVANCE,                 //
  kUpdateFeederConfig=GCODE_UPDATE_FEEDER_CONFIG,
};

// ------------------  S E T U P -----------------------
void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial);
	Serial.println(F("Feeduino starting...")); Serial.flush();//setup servo objects

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
	
	
	
	//interface to OpenPnP, command parsing, etc.
	// attach callbacks to cmdMessenger
	cmdMessenger.attach(OnUnknownCommand);
	cmdMessenger.attach(kAdvance, OnAdvance);
  cmdMessenger.attach(kUpdateFeederConfig, OnUpdateFeederConfig);
	
	// Adds newline to every command
	cmdMessenger.printLfCr();

	// Send the status to the PC that says the Arduino has booted
	cmdMessenger.sendCmd(kAcknowledge,"Arduino has started!");
	
	
  
	// Setup the button for debugging purposes
	pinMode(PIN_BUTTON,INPUT_PULLUP);
	debouncedButton.attach(PIN_BUTTON);
	debouncedButton.interval(5); // interval in ms
	
	
}

// ------------------  L O O P -----------------------

void loop() {

	debouncedButton.update();
	
	// Process incoming serial data and perform callbacks
	cmdMessenger.feedinSerialData();
	
	// Process servo control
	FeedManager.update();
	
	
	if ( debouncedButton.fell() ) {
		FeedManager.feeders[0].advance();
	}

}



// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void OnUnknownCommand() {
	cmdMessenger.sendCmd(kError,"Unknown Command");
}

void OnAdvance() {
	//get commands parameters
  uint8_t feederNo = (uint8_t)cmdMessenger.readInt16Arg();

  //do the neccessary thingscmdMessenger.sendCmdStart(kAcknowledge);
  FeedManager.feeders[feederNo].advance();

  //answer to host
	cmdMessenger.sendCmdArg("Advancing FeederNo ");
	cmdMessenger.sendCmdArg(feederNo);
	cmdMessenger.sendCmdEnd();
	
}

void OnUpdateFeederConfig() {
  //get commands parameters
  uint8_t feederNo = (uint8_t)cmdMessenger.readInt16Arg();

  //do the neccessary things
  //FeedManager.feeders[feederNo].updateConfig();

  //answer to host
  cmdMessenger.sendCmdStart(kAcknowledge);
  cmdMessenger.sendCmdArg("Updated FeederNo ");
  cmdMessenger.sendCmdArg(feederNo);
  cmdMessenger.sendCmdEnd();

  
}

