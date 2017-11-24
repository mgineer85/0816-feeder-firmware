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
	inputBuffer.reserve(MAX_BUFFFER_MCODE_LINE);
}

void sendAnswer(uint8_t error, String message) {
	if(error==0)
	Serial.print(F("ok "));
	else
	Serial.print(F("error "));

	Serial.println(message);
}

bool validFeederNo(int8_t signedFeederNo, uint8_t feederNoMandatory = 0) {
	if(signedFeederNo == -1 && feederNoMandatory >= 1) {
		//no number given (-1) but it is mandatory.
		return false;
		} else {
		//state now: number is given, check for valid range
		if(signedFeederNo<0 || signedFeederNo>(NUMBER_OF_FEEDER-1)) {
			//error, number not in a valid range
			return false;
			} else {
			//perfectly fine number
			return true;
		}
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


		case MCODE_SET_FEEDER_ENABLE: {

			int8_t _feederEnabled=parseParameter('S',-1);
			if( (_feederEnabled==0 || _feederEnabled==1) ) {

				if((uint8_t)_feederEnabled==1) {
					digitalWrite(FEEDER_ENABLE_PIN, HIGH);
					feederEnabled=ENABLED;

					executeCommandOnAllFeeder(cmdEnable);

					sendAnswer(0,F("Feeder set enabled and operational"));
				} else {
					digitalWrite(FEEDER_ENABLE_PIN, LOW);
					feederEnabled=DISABLED;

					executeCommandOnAllFeeder(cmdDisable);

					sendAnswer(0,F("Feeder set disabled"));
				}
			} else if(_feederEnabled==-1) {
				sendAnswer(0,("current powerState: ") + String(feederEnabled));
			} else {
				sendAnswer(1,F("Invalid parameters"));
			}


			break;
		}


		case MCODE_ADVANCE: {
			//1st to check: are feeder enabled?
			if(feederEnabled!=ENABLED) {
				sendAnswer(1,String(String("Enable feeder first! M") + String(MCODE_SET_FEEDER_ENABLE) + String(" S1")));
				break;
			}

			int8_t signedFeederNo = (int)parseParameter('N',-1);
			int8_t overrideErrorRaw = (int)parseParameter('X',-1);
			bool overrideError = false;
			if(overrideErrorRaw >= 1) {
				overrideError = true;
				#ifdef DEBUG
				Serial.println("Argument X1 found, feedbackline/error will be ignored");
				#endif
			}

			//check for presence of a mandatory FeederNo
			if(!validFeederNo(signedFeederNo,1)) {
				sendAnswer(1,F("feederNo missing or invalid"));
				break;
			}

			//determine feedLength
			uint8_t feedLength;
			//get feedLength if given, otherwise go for default configured feed_length
			feedLength = (uint8_t)parseParameter('F',feeders[(uint8_t)signedFeederNo].feederSettings.feed_length);


			if ( ((feedLength%2) != 0) || feedLength>24 ) {
				//advancing is only possible for multiples of 2mm and 24mm max
				sendAnswer(1,F("Invalid feedLength"));
				break;
			}
			#ifdef DEBUG
			Serial.print("Determined feedLength ");
			Serial.print(feedLength);
			Serial.println();
			#endif

			//start feeding
			bool triggerFeedOK=feeders[(uint8_t)signedFeederNo].advance(feedLength,overrideError);
			if(!triggerFeedOK) {
				//report error to host at once, tape was not advanced...
				sendAnswer(1,F("feeder not OK (not activated, no tape or tension of cover tape not OK)"));
			} else {
				//answer OK to host in case there was no error -> NO, no answer now:
				//wait to send OK, until feed process finished. otherwise the pickup is started immediately, thus too early.
				//message is fired off in feeder.cpp
			}

			
			break;
		}

		case MCODE_RETRACT_POST_PICK: {
			//1st to check: are feeder enabled?
			if(feederEnabled!=ENABLED) {
				sendAnswer(1,String(String("Enable feeder first! M") + String(MCODE_SET_FEEDER_ENABLE) + String(" S1")));
				break;
			}


			int8_t signedFeederNo = (int)parseParameter('N',-1);

			//check for presence of FeederNo
			if(!validFeederNo(signedFeederNo,1)) {
				sendAnswer(1,F("feederNo missing or invalid"));
				break;
			}

			feeders[(uint8_t)signedFeederNo].gotoPostPickPosition();

			sendAnswer(0,F("feeder postPickRetract done if needed"));

			break;
		}

		case MCODE_FEEDER_IS_OK: {
			int8_t signedFeederNo = (int)parseParameter('N',-1);

			//check for presence of FeederNo
			if(!validFeederNo(signedFeederNo,1)) {
				sendAnswer(1,F("feederNo missing or invalid"));
				break;
			}

			sendAnswer(0,feeders[(uint8_t)signedFeederNo].reportFeederErrorState());

			break;
		}

		case MCODE_SERVO_SET_ANGLE: {
			//1st to check: are feeder enabled?
			if(feederEnabled!=ENABLED) {
				sendAnswer(1,String(String("Enable feeder first! M") + String(MCODE_SET_FEEDER_ENABLE) + String(" S1")));
				break;
			}


			int8_t signedFeederNo = (int)parseParameter('N',-1);
			uint8_t angle = (int)parseParameter('A',90);

			//check for presence of FeederNo
			if(!validFeederNo(signedFeederNo,1)) {
				sendAnswer(1,F("feederNo missing or invalid"));
				break;
			}
			//check for valid angle
			if( angle>180 ) {
				sendAnswer(1,F("illegal angle"));
				break;
			}

			feeders[(uint8_t)signedFeederNo].gotoAngle(angle);

			sendAnswer(0,F("angle set"));

			break;
		}

		case MCODE_UPDATE_FEEDER_CONFIG: {
			int8_t signedFeederNo = (int)parseParameter('N',-1);

			//check for presence of FeederNo
			if(!validFeederNo(signedFeederNo,1)) {
				sendAnswer(1,F("feederNo missing or invalid"));
				break;
			}

			//merge given parameters to old settings
			FeederClass::sFeederSettings oldFeederSettings=feeders[(uint8_t)signedFeederNo].getSettings();
			FeederClass::sFeederSettings updatedFeederSettings;
			updatedFeederSettings.full_advanced_angle=parseParameter('A',oldFeederSettings.full_advanced_angle);
			updatedFeederSettings.half_advanced_angle=parseParameter('B',oldFeederSettings.half_advanced_angle);
			updatedFeederSettings.retract_angle=parseParameter('C',oldFeederSettings.retract_angle);
			updatedFeederSettings.feed_length=parseParameter('F',oldFeederSettings.feed_length);
			updatedFeederSettings.time_to_settle=parseParameter('U',oldFeederSettings.time_to_settle);
			updatedFeederSettings.motor_min_pulsewidth=parseParameter('V',oldFeederSettings.motor_min_pulsewidth);
			updatedFeederSettings.motor_max_pulsewidth=parseParameter('W',oldFeederSettings.motor_max_pulsewidth);
			updatedFeederSettings.ignore_feedback=parseParameter('X',oldFeederSettings.ignore_feedback);
			
			//set to feeder
			feeders[(uint8_t)signedFeederNo].setSettings(updatedFeederSettings);

			//save to eeprom
			feeders[(uint8_t)signedFeederNo].saveFeederSettings();

			//confirm
			sendAnswer(0,F("Feeders config updated."));

			break;
		}

		/*
		CODES to Control ADC
		*/
		case MCODE_GET_ADC_RAW: {
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
		case MCODE_GET_ADC_SCALED: {
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
		case MCODE_SET_SCALING: {

			int8_t channel=parseParameter('A',-1);

			//check for valid parameters
			if( channel>=0 && channel<8 ) {
				commonSettings.adc_scaling_values[(uint8_t)channel][0]=parseParameter('S',commonSettings.adc_scaling_values[(uint8_t)channel][0]);
				commonSettings.adc_scaling_values[(uint8_t)channel][1]=parseParameter('O',commonSettings.adc_scaling_values[(uint8_t)channel][1]);

				EEPROM.writeBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);

				sendAnswer(0,(F("scaling set and stored to eeprom")));
			} else {
				sendAnswer(1,F("invalid adc channel (0...7)"));
			}


			break;
		}

		case MCODE_SET_POWER_OUTPUT: {
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

		case MCODE_FACTORY_RESET: {
			commonSettings.version[0]=commonSettings.version[0]+1;

			EEPROM.writeBlock(EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET, commonSettings);

			sendAnswer(0,F("EEPROM invalidated, defaults will be loaded on next restart. Please restart now."));


			break;
		}

		default:
		sendAnswer(0,F("unknown or empty command ignored"));

		break;

	}

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

		}
	}
}
