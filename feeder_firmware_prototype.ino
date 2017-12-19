/*
* Author: Michael G.
* (c)2017-12-19
*
* This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
* http://creativecommons.org/licenses/by-nc-sa/4.0/
*
* v0.2
*
* CHANGELOG:
* v0.2 added support for sensor shield (experimental, no feedbackline supported)
*
*
*
*
*
*
*/

#include "config.h"
#include "boards.h"

// ------------------  I N C  L I B R A R I E S ---------------
#include <HardwareSerial.h>
#include <EEPROMex.h>
#include "Feeder.h"

// ------------------  V A R  S E T U P -----------------------

// ------ Feeder
FeederClass feeders[NUMBER_OF_FEEDER];
enum eFeederEnabledState {
  DISABLED,
  ENABLED,
} feederEnabled=DISABLED;

// ------ Settings-Struct (saved in EEPROM)
struct sCommonSettings {

	//add further settings here

	char version[4];   // This is for detection if settings suit to struct, if not, eeprom is reset to defaults

	float adc_scaling_values[8][2];
};
sCommonSettings commonSettings_default = {

	//add further settings here

	CONFIG_VERSION,

	{
		{ANALOG_A0_SCALING_FACTOR,ANALOG_A0_OFFSET},
		{ANALOG_A1_SCALING_FACTOR,ANALOG_A1_OFFSET},
		{ANALOG_A2_SCALING_FACTOR,ANALOG_A2_OFFSET},
		{ANALOG_A3_SCALING_FACTOR,ANALOG_A3_OFFSET},
		{ANALOG_A4_SCALING_FACTOR,ANALOG_A4_OFFSET},
		{ANALOG_A5_SCALING_FACTOR,ANALOG_A5_OFFSET},
		{ANALOG_A6_SCALING_FACTOR,ANALOG_A6_OFFSET},
		{ANALOG_A7_SCALING_FACTOR,ANALOG_A7_OFFSET},
	},
};
sCommonSettings commonSettings;

// ------ ADC readout
unsigned long lastTimeADCread;
uint16_t adcRawValues[8];
float adcScaledValues[8];

// ------------------  U T I L I T I E S ---------------

// ------ Operate command on all feeder
enum eFeederCommands {
	cmdSetup,
	cmdUpdate,

  cmdEnable,
  cmdDisable,

	cmdOutputCurrentSettings,
	cmdInitializeFeederWithId,
	cmdFactoryReset,

};
void executeCommandOnAllFeeder(eFeederCommands command);
void executeCommandOnAllFeeder(eFeederCommands command) {
	for (uint8_t i=0;i<NUMBER_OF_FEEDER;i++) {
		switch(command) {
			case cmdSetup:
				feeders[i].setup();
			break;
			case cmdUpdate:
				feeders[i].update();
			break;
			case cmdEnable:
				feeders[i].enable();
			break;
			case cmdDisable:
				feeders[i].disable();
			break;
			case cmdOutputCurrentSettings:
				feeders[i].outputCurrentSettings();
			break;
			case cmdInitializeFeederWithId:
				feeders[i].initialize(i);
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

void updateADCvalues() {

	for(uint8_t i=0; i<=7; i++) {
		adcRawValues[i]=analogRead(i);
		adcScaledValues[i]=(adcRawValues[i]*commonSettings.adc_scaling_values[i][0])+commonSettings.adc_scaling_values[i][1];
	}
}

void printCommonSettings() {

	//ADC-scaling values
	Serial.println("Analog Scaling Settings:");
	for(uint8_t i=0; i<=7; i++) {
		Serial.print("M");
		Serial.print(MCODE_SET_SCALING);
		Serial.print(" A");
		Serial.print(i);
		Serial.print(" S");
		Serial.print(commonSettings.adc_scaling_values[i][0]);
		Serial.print(" O");
		Serial.print(commonSettings.adc_scaling_values[i][1]);
		Serial.println();
	}
}

// ------------------  S E T U P -----------------------
void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial);
	Serial.println(F("Controller starting...")); Serial.flush();
	Serial.println(F("Here is some stuff saved in EEPROM. Paste in a textfile to backup these settings...")); Serial.flush();

	//feeder enable output
	pinMode(FEEDER_ENABLE_PIN,OUTPUT);
	digitalWrite(FEEDER_ENABLE_PIN,LOW);

	//power output init
	for(uint8_t i=0;i<NUMBER_OF_POWER_OUTPUT;i++) {
		pinMode(pwrOutputPinMap[i],OUTPUT);
		digitalWrite(pwrOutputPinMap[i],LOW);
	}
	
	// setup listener to serial stream
	setupGCodeProc();

	//initialize active feeders, this is giving them an valid ID
	//needs to be done before factory reset to have a valid ID (eeprom-settings location is derived off the ID)
	executeCommandOnAllFeeder(cmdInitializeFeederWithId);

	//load commonSettings from eeprom
	EEPROM.readBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);

	//factory reset on first start or version changing
	if(strcmp(commonSettings.version,CONFIG_VERSION) != 0) {
		Serial.println(F("First start/Config version changed"));

		//reset needed
		executeCommandOnAllFeeder(cmdFactoryReset);

		//update commonSettings in EEPROM to have no factory reset on next start
		EEPROM.writeBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings_default);
	}

	//print all settings to console
	printCommonSettings();

	//setup feeder objects
	digitalWrite(FEEDER_ENABLE_PIN, HIGH);	//enable first, because while setup feeder might retract.
	executeCommandOnAllFeeder(cmdSetup);
	delay(200);		//have the last feeder settled before disabling
	executeCommandOnAllFeeder(cmdDisable);
	digitalWrite(FEEDER_ENABLE_PIN, LOW);	//disable afterwards
	
	//print all settings of every feeder to console
	executeCommandOnAllFeeder(cmdOutputCurrentSettings);

	//init adc-values
	updateADCvalues();
	lastTimeADCread=millis();


	Serial.println(F("Controller up and ready! Have fun."));
}



// ------------------  L O O P -----------------------

void loop() {

	//debouncedButton.update();

	// Process incoming serial data and perform callbacks
	listenToSerialStream();

	// Process servo control
	executeCommandOnAllFeeder(cmdUpdate);

	// Process ADC inputs
	if (millis() - lastTimeADCread >= ADC_READ_EVERY_MS) {
		lastTimeADCread=millis();

		updateADCvalues();
	}

}
