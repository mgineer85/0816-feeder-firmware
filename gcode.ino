#include "globals.h"

String inputBuffer = "";         // Buffer for incoming G-Code lines


/**
* Look for character /code/ in the inputBuffer and read the float that immediately follows it.
* @return the value found.  If nothing is found, /defaultVal/ is returned.
* @input code the character to look for.
* @input defaultVal the return value if /code/ is not found.
**/
float parseParameter(char code,float defaultVal) {
	int codePosition = inputBuffer.indexOf(code);
	if(codePosition!=-1) {
		//code found in buffer
		
		//find end of number (separated by " " (space))
		int delimiterPosition = inputBuffer.indexOf(" ",codePosition+1);
		
		float parsedNumber = inputBuffer.substring(codePosition+1,delimiterPosition).toFloat();
		
		return parsedNumber;
	} else {
		return defaultVal;
	}
	
}

void setupGCodeProc() {
	inputBuffer.reserve(MAX_BUFFFER_GCODE_LINE);
}


/**
* write a string followed by a float to the serial line.  Convenient for debugging.
* @input code the string.
* @input val the float.
*/
/*
void output(const char *code,float val) {
	Serial.print(code);
	Serial.println(val);
}*/

/**
* display helpful information
*/
void help() {
	Serial.print(F("Feeduino PnP "));
	Serial.println(F("Commands:"));
	Serial.println(F("none yet"));
	Serial.println(F("All commands must end with a newline."));
}

void sendAnswer(uint8_t error, String message) {
	if(error==0)
		Serial.print(F("ok "));
	else
		Serial.print(F("error "));
	
	Serial.println(message);
}

boolean validFeederNo(int8_t signedFeederNo) {
	if(signedFeederNo<0 || signedFeederNo>(NUMBER_OF_FEEDERS-1)) {
		return false;
	} else {
		return true;
	}
}

/**
* Read the input buffer and find any recognized commands.  One G or M command per line.
*/
void processCommand() {

	//get the command, default -1 if no command found
	int cmd = parseParameter('M',-1);
	
	#ifdef DEBUG
		Serial.print("command found: M");
		Serial.println(cmd);
	#endif
	

	switch(cmd) {
		
		/*
			FEEDER-CODES
		*/
		case GCODE_ADVANCE:
		case GCODE_ADVANCE+2:
		case GCODE_ADVANCE+4:
		case GCODE_ADVANCE+6:
		case GCODE_ADVANCE+8:
		case GCODE_ADVANCE+10:
		case GCODE_ADVANCE+12: {
			int8_t signedFeederNo = (int)parseParameter('N',-1);
			
			//check for presence of FeederNo
			if(signedFeederNo==-1) {
				sendAnswer(1,F("feederNo missing for command"));
				break;
			} else if(!validFeederNo(signedFeederNo)) {
				sendAnswer(1,F("invalid feeder selected"));
				break;
			}

			//can go on without further checks -> if number was given, it was checked for validity above already
			
			//determine feedLength
			uint8_t feedLength;
			if(cmd == GCODE_ADVANCE) {
				//base-command. two options for feedLength here:
				//F parameter omitted: use configured feed_length
				//F parameter given: go for given feedlength F
				//get feedLength if given, otherwise go for default configured feed_length in case of base-command GCODE_ADVANCE
				feedLength = (uint8_t)parseParameter('F',feeders[(uint8_t)signedFeederNo].feederSettings.feed_length);
			} else {
				//do the mapping of several m-codes to feedLengths'
				feedLength = cmd-GCODE_ADVANCE;
			}
			
			if ( ((feedLength%2) != 0) || feedLength>12 ) {
				//advancing is only possible for multiples of 2mm and 12mm max
				sendAnswer(1,F("Invalid feedLength"));
				break;
			}
			#ifdef DEBUG
				Serial.print("Determined feedLength ");
				Serial.print(feedLength);
				Serial.println("");
			#endif
			//start feeding
			feeders[(uint8_t)signedFeederNo].advance(feedLength);

			//answer OK to host
			//no answer to host here: wait to send OK, until finished. otherwise the pickup is started to early.
			//message is fired off in feeder.cpp
			
			break;
		}

		case GCODE_UPDATE_FEEDER_CONFIG: {
			int8_t signedFeederNo = (int)parseParameter('N',-1);
			
			//check for presence of FeederNo
			if(signedFeederNo==-1) {
				sendAnswer(1,F("feederNo missing for command"));
				break;
				} else if(!validFeederNo(signedFeederNo)) {
				sendAnswer(1,F("invalid feeder selected"));
				break;
			}
			
			//merge given parameters to old settings
			sFeederSettings oldFeederSettings=feeders[(uint8_t)signedFeederNo].getSettings();
			sFeederSettings updatedFeederSettings;
			updatedFeederSettings.full_advanced_angle=parseParameter('A',oldFeederSettings.full_advanced_angle);
			updatedFeederSettings.half_advanced_angle=parseParameter('B',oldFeederSettings.half_advanced_angle);
			updatedFeederSettings.retract_angle=parseParameter('C',oldFeederSettings.retract_angle);
			updatedFeederSettings.feed_length=parseParameter('F',oldFeederSettings.feed_length);
			updatedFeederSettings.time_to_settle=parseParameter('U',oldFeederSettings.time_to_settle);
			updatedFeederSettings.motor_min_pulsewidth=parseParameter('V',oldFeederSettings.motor_min_pulsewidth);
			updatedFeederSettings.motor_max_pulsewidth=parseParameter('W',oldFeederSettings.motor_max_pulsewidth);
			
			//set to feeder
			feeders[(uint8_t)signedFeederNo].setSettings(updatedFeederSettings);
			
			//save to eeprom
			feeders[(uint8_t)signedFeederNo].saveFeederSettings();
			
			//confirm
			sendAnswer(0,F("Config of feeder updated."));
			
			break;
		}
		
		/*
		CODES to Control ADC
		*/
		case GCODE_GET_ADC_RAW: {
			//answer to host
			int8_t channel=parseParameter('A',-1);
			if( channel>=0 && channel<8 ) {
				
				//send value in first line of answer, so it can be parsed by OpenPnP correctly
				Serial.println(String("value:")+String(adcRawValues[(uint8_t)channel]));
				
				//common answer
				sendAnswer(0,"value sent");
			} else {
				sendAnswer(1,F("invalid adc channel (0...7)"));
			}
			
			break;
		}
		case GCODE_GET_ADC_SCALED: {
			//answer to host
			int8_t channel=parseParameter('A',-1);
			if( channel>=0 && channel<8 ) {
				
				//send value in first line of answer, so it can be parsed by OpenPnP correctly
				Serial.println(String("value:")+String(adcScaledValues[(uint8_t)channel],4));
				
				//common answer
				sendAnswer(0,"value sent");
			} else {
				sendAnswer(1,F("invalid adc channel (0...7)"));
			}
			
			break;
		}
		case GCODE_SET_SCALING: {
			
			int8_t channel=parseParameter('A',-1);
			
			//check for valid parameters
			if( channel>=0 && channel<8 ) {
				commonSettings.adc_scaling_values[(uint8_t)channel][0]=parseParameter('S',commonSettings.adc_scaling_values[(uint8_t)channel][0]);
				commonSettings.adc_scaling_values[(uint8_t)channel][1]=parseParameter('O',commonSettings.adc_scaling_values[(uint8_t)channel][1]);
				
				sendAnswer(0,(F("scaling set")));
			} else {
				sendAnswer(1,F("invalid adc channel (0...7)"));
			}
			
			
			break;
		}
		
		
		case GCODE_SET_POWER_OUTPUT: {
			//answer to host
			int8_t powerPin=parseParameter('D',-1);
			int8_t powerState=parseParameter('S',-1);
			if( (powerPin>=0 && powerPin<NUMBER_OF_POWER_OUTPUT) && (powerState==0 || powerState==1) ) {
				digitalWrite(pwrOutputPinMap[(uint8_t)powerPin], (uint8_t)powerState);
				sendAnswer(0,F("Output set"));
			} else {
				sendAnswer(1,F("Invalid Parameters"));
			}
			break;
		}
		
			
		
		case 100:  help();  break;
		default:
			sendAnswer(0,F("unknown or empty command ignored"));
			
			break;
	}
	
}


/**
* prepares the input buffer to receive a new message and tells the serial connected device it is ready for more.
*/
void ready() {
	Serial.print(F(">"));  // signal ready to receive input
}


void listenToSerialStream() {
	
	while (Serial.available()) {
	
		// get the received byte, convert to char for adding to buffer
		char receivedChar = (char)Serial.read();
		
		// print back for debugging
		//#ifdef DEBUG
			Serial.print(receivedChar);
		//#endif
		
		// add to buffer
		inputBuffer += receivedChar;
		
		// if the received character is a newline, processCommand
		if (receivedChar == '\n') {
			
			//remove comments
			inputBuffer.remove(inputBuffer.indexOf(";"));
			inputBuffer.trim();
			
			
			processCommand();
			
			//clear buffer
			inputBuffer="";
			
			//ready();
		}
	}
}


