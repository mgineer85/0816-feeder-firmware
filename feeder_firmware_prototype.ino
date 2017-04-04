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
	kAcknowledge			, // Command to acknowledge that cmd was received
	kError					, // Command to report errors
	kFloatAddition			, // Command to request add two floats //TODO: remove
	kFloatAdditionResult	, // Command to report addition result //TODO: remove.
	kAdvance				,
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
  }
	
	//handles the servo controlling stuff
	FeedManager.setup();
	
	
	
	//interface to OpenPnP, command parsing, etc.
	// attach callbacks to cmdMessenger
	cmdMessenger.attach(OnUnknownCommand);
	cmdMessenger.attach(kFloatAddition, OnFloatAddition);
	cmdMessenger.attach(kAdvance, OnAdvance);
	
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

// Callback function that responds that Arduino is ready (has booted up)
void OnArduinoReady() {
	cmdMessenger.sendCmd(kAcknowledge,"Arduino ready");
}

// Callback function calculates the sum of the two received float values
void OnFloatAddition() {
	// Retreive first parameter as float
	float a = cmdMessenger.readFloatArg();
	
	// Retreive second parameter as float
	float b = cmdMessenger.readFloatArg();
	
	// Send back the result of the addition
	//cmdMessenger.sendCmd(kFloatAdditionResult,a + b);
	cmdMessenger.sendCmdStart(kFloatAdditionResult);
	cmdMessenger.sendCmdArg(a+b);
	cmdMessenger.sendCmdArg(a-b);
	cmdMessenger.sendCmdEnd();
}
// Callback function that responds that Arduino is ready (has booted up)
void OnAdvance() {
	uint16_t feeder_number = cmdMessenger.readInt16Arg();
	cmdMessenger.sendCmdStart(kAcknowledge);
	cmdMessenger.sendCmdArg("FeederNo");
	cmdMessenger.sendCmdArg(feeder_number);
	cmdMessenger.sendCmdEnd();
	FeedManager.feeders[0].advance();
}
