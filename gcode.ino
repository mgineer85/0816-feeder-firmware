#include "globals.h"

/**
* Parts in this file base on GcodeCNCDemo. Original copyright following:
*
* GcodeCNCDemo is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GcodeCNCDemo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this software. If not, see <http://www.gnu.org/licenses/>.
*
* please see http://www.github.com/MarginallyClever/GcodeCNCDemo for more information.
* ------------------------------------------------------------------------------
*  2 Axis CNC Demo
*  dan@marginallycelver.com 2013-08-30
* ------------------------------------------------------------------------------
*/


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
		
		float parsedNumber = inputBuffer.substring(codePosition+1,delimiterPosition-1).toFloat();
		
		return parsedNumber;
	} else {
		return defaultVal;
	}
	
}

void setupGCodeProc() {
	inputBuffer.reserve(MAX_BUF);
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

	//check for feederNo - if present, it has to be right.
	int8_t signedFeederNo = (int8_t)parseParameter('N',-1);
	if(signedFeederNo!=-1) {  //feederNo given -> check for a valid number
		if(!validFeederNo(signedFeederNo)) {
			sendAnswer(1,F("Invalid feederNo"));
			return;
		}
	}

	switch(cmd) {
		case  GCODE_ADVANCE: {
			//check for valid FeederNo
			if(signedFeederNo==-1) {
				sendAnswer(1,F("feederNo missing for command"));
			}

			//can go on without further checks -> if number was given, it was checked for validity above already

			
			//get feedLength, default is FEEDER_PITCH
			uint8_t feedLength = (uint8_t)parseParameter('F',FEEDER_PITCH);
			if ( ((feedLength%2) != 0) || feedLength>12 ) {
				//advancing is only possible for multiples of 2mm and 12mm max
				sendAnswer(1,F("Invalid feedLength"));
			}

			//start feeding
			FeedManager.feeders[(uint8_t)signedFeederNo].advance(feedLength);

			//answer OK to host
			sendAnswer(0,F("advanced."));
			
			break;
		}

		
		case GCODE_RETRACT: {
			
			if(signedFeederNo==-1) {
				//retract all
				FeedManager.retractAll();
				
				} else {
				//retract specified feeder
				FeedManager.feeders[(uint8_t)signedFeederNo].gotoRetractPosition();
			}
			
			//answer to host
			sendAnswer(0,F("retracted."));
			
			break;
		}

		
		case GCODE_UPDATE_FEEDER_CONFIG: {
			
			//check for valid FeederNo
			if(signedFeederNo==-1) {
				sendAnswer(1,F("feederNo missing for command"));
			}
			
			//merge given parameters to old settings
			sFeederSettings oldFeederSettings=FeedManager.feeders[(uint8_t)signedFeederNo].getSettings();
			
			oldFeederSettings.full_advanced_angle=parseParameter('A',-1);  //full_advanced_angle
			oldFeederSettings.half_advanced_angle=parseParameter('B',-1);  //half_advanced_angle
			oldFeederSettings.retract_angle=parseParameter('C',-1);  //retract_angle
			oldFeederSettings.time_to_settle=parseParameter('U',-1);  //time_to_settle
			oldFeederSettings.motor_min_pulsewidth=parseParameter('V',-1);  //motor_min_pulsewidth
			oldFeederSettings.motor_max_pulsewidth=parseParameter('W',-1);  //motor_max_pulsewidth
			
			
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
		if (receivedChar == '\n' || receivedChar == '\r') {
			#ifdef DEBUG
				Serial.print(F("\r\n"));
			#endif
			
			processCommand();
			
			//clear buffer
			inputBuffer="";
			
			ready();
		}
	}
}


// ------------------  C A L L B A C K S -----------------------

void DoUpdateFeederConfig() {
	//get commands parameters
	uint8_t feederNo = (uint8_t)0;

	//do the neccessary things
	//FeedManager.feeders[feederNo].updateConfig();

	//answer to host
	//--TODO

	
}

void DoRetract() {
	//get commands parameters

	if(true) {
		//specific feeder
		uint8_t feederNo = (uint8_t)0;
		
		//do the neccessary things
		FeedManager.feeders[feederNo].gotoRetractPosition();
		
		//answer to host
		//--TODO
		
		} else {
		//do the neccessary things
		FeedManager.retractAll();
		
		//answer to host
		//--TODO
	}
	
	
}


