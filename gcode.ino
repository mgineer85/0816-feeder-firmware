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
void output(const char *code,float val) {
	Serial.print(code);
	Serial.println(val);
}

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
	Serial.print(F("OK "));
	else
	Serial.print(F("ERROR "));
	
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
		Serial.print("Determined CMD ");
		Serial.print(cmd);
		Serial.println("");
	#endif
	
	//check for feederNo - if present, it has to be right.
	int8_t signedFeederNo = (int)parseParameter('N',-1);
	if(signedFeederNo!=-1) {  //feederNo given -> check for a valid number
		if(!validFeederNo(signedFeederNo)) {
			sendAnswer(1,F("Invalid feederNo"));
			return;
		}
	}
	#ifdef DEBUG
		Serial.print("Determined signedFeederNo ");
		Serial.print(signedFeederNo);
		Serial.println("");
	#endif
	

	switch(cmd) {
		case GCODE_ADVANCE:
		case GCODE_ADVANCE+2:
		case GCODE_ADVANCE+4:
		case GCODE_ADVANCE+6:
		case GCODE_ADVANCE+8:
		case GCODE_ADVANCE+10:
		case GCODE_ADVANCE+12: {
			//check for presence of FeederNo
			if(signedFeederNo==-1) {
				sendAnswer(1,F("feederNo missing for command"));
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
			sendAnswer(0,F("advanced."));
			
			break;
		}

		
		case GCODE_RETRACT: {
			
			if(signedFeederNo==-1) {
				//retract all
				executeCommand(cmdRetract);
				
				} else {
				//retract specified feeder
				feeders[(uint8_t)signedFeederNo].gotoRetractPosition();
			}
			
			//answer to host
			sendAnswer(0,F("retracted."));
			
			break;
		}

		
		case GCODE_UPDATE_FEEDER_CONFIG: {
			
			//check for valid FeederNo
			if(signedFeederNo==-1) {
				sendAnswer(1,F("feederNo not optional for this command"));
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
			break;
		}

		
		case 100:  help();  break;
		default:  break;
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
		#ifdef DEBUG
			Serial.print(receivedChar);
		#endif
		
		// add to buffer
		inputBuffer += receivedChar;
		
		// if the received character is a newline, processCommand
		if (receivedChar == '\n') {

			processCommand();
			
			//clear buffer
			inputBuffer="";
			
			ready();
		}
	}
}


