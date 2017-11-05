#ifndef _CONFIG_h
#define _CONFIG_h
#include "arduino.h"


/*
*     DEBUG
*/
// prints some extra information via serial
#define DEBUG

/*
*  EEPROM-Settings
*/
//change to something other unique if structure of data to be saved in eeprom changed (max 3 chars)
#define CONFIG_VERSION "aab"

//2 blocks to store data, do not change
#define EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET 4
#define EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET 128


/*
*  Serial
*/
#define SERIAL_BAUD 115200
#define MAX_BUFFFER_GCODE_LINE 64	// no line can be longer than this



/* -----------------------------------------------------------------
*  FEEDER CONFIG
*  ----------------------------------------------------------------- */
#define NUMBER_OF_FEEDER 24
const static uint8_t feederPinMap[NUMBER_OF_FEEDER] = {
	2,    // Feeder 1
	3,    // Feeder 2
	4,		//...
	5,
	6,
	53,
	
	8,	// Feeder 7
	9,
	10,
	11,
	12,
	13,
	
	22,	// Feeder 13
	24,
	26,
	28,
	30,
	32,
	
	34,	//Feeder 19
	36,
	38,
	40,
	42,
	44,
};

const static int8_t feederFeedbackPinMap[NUMBER_OF_FEEDER] = {
	//use -1 to disable feedback pin for specific feeder. otherwise set input pin
	-1,    // Feeder 1
	-1,    // Feeder 2
	-1,		//...
	-1,
	-1,
	 7,
	
	-1,    // Feeder 7
	-1,
	-1,
	-1,
	-1,
	-1,
	
	-1,    // Feeder 13
	-1,
	-1,
	-1,
	-1,
	-1,
	
	-1,    // Feeder 19
	-1,
	-1,
	-1,
	-1,
	-1,
	
};

#define FEEDER_PITCH  4                   // [mm]  default: 4 mm. given by mechanical design.


/* Default settings for feeders */
// calculate angles: https://de.wikipedia.org/wiki/Schubkurbel
/*
	when setting these values, make sure, the values for 
	FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH and
	FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH
	reflect the according position for servo being used
	
	when everything is setup right, the lever is in advanced position at servo-angle ~85 degree
	fully retracted is about ~10°
	
*/
#define FEEDER_DEFAULT_FULL_ADVANCED_ANGLE  90				      // [°]  usually about 90.
#define FEEDER_DEFAULT_HALF_ADVANCED_ANGLE  40              // [°]  usually about 40-60.
#define FEEDER_DEFAULT_RETRACT_ANGLE  20				      // [°]  usually 80, might be adjusted to servo or feeder
#define FEEDER_DEFAULT_FEED_LENGTH FEEDER_PITCH			// [mm] distance to be fed if no feedlength was given
#define FEEDER_DEFAULT_TIME_TO_SETTLE  240			  // [ms] time the servo needs to travel from FEEDER_DEFAULT_FULL_ADVANCED_ANGLE to FEEDER_DEFAULT_RETRACT_ANGLE
/*
	 0° == ~ 544 µs	--> min, default 544 and seems it fits to the sg90 from tower pro
	90° ==          --> "middle"
   180° == ~2400 µs --> max, default 2400 and seems it fits to the sg90 from tower pro
	 --> SERVO.attach(PIN, 544, 2400);
*/
#define FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH 544		// [µs] see motor specs or experiment at bit. Value set here should bring the servo to 0°
#define FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH 2400		// [µs] see motor specs or experiment at bit. Value set here should bring the servo to 180°
#define FEEDER_DEFAULT_IGNORE_FEEDBACK 0			// 0: before feeding the feedback-signal is checked. if signal is as expected, the feeder advances tape and returns OK to host. otherwise an error is thrown.
													// 1: the feedback-signal is not checked, feeder advances tape and returns OK always

/* -----------------------------------------------------------------
*  M-CODES
*  ----------------------------------------------------------------- */

/* ------------ M-CODE: ADVANCE -----------
*
* Command issued to advance tape by one feeder
*
* Parameters:
* N Number of Feeder: 0...11
* F Feedlength, optional: If given, has to be a multiple of 2 mm
*
* Example commands:
* M600 N3 F4 -> Advance Feeder No. 3 by 4mm
* M600 N0    -> Advance Feeder No. 0 by FEEDER_PITCH (mechanical default)
* M600 N11   -> Advance Feeder No. 11 (last feeder) by 2 mm (feeder holds on half way at configured HALF_ADVANCED_ANGLE), might need to be tweaked for accuracy
*/
#define GCODE_ADVANCE 600


/* ------------ M-CODE: UPDATE FEEDER CONFIG -----------
*
* Tweak feeder parameters for specific servo, to improve accuracy, etc.
*
* Parameters:
* N Number of Feeder: 0...(NUMBER_OF_FEEDERS-1)
* A advanced angle, defaults to FEEDER_DEFAULT_FULL_ADVANCED_ANGLE
* B half advanced angle, defaults to FEEDER_DEFAULT_HALF_ADVANCED_ANGLE
* C retract angle, defaults to FEEDER_DEFAULT_RETRACT_ANGLE
* F standard feed length, defaults to FEEDER_DEFAULT_FEED_LENGTH, which is FEED_PITCH, which is 4mm usually
* U settle time to go from advanced angle to retract angle and reverse, defaults to FEEDER_DEFAULT_TIME_TO_SETTLE. make sure the servo is fast enough to reach the angles within given settle time
* V pulsewidth at which servo is at about 0°, defaults to FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH
* W pulsewidth at which servo is at about 180°, defaults to FEEDER_DEFAULT_MOTOR_MAX_PULSEWIDTH
* X ignore feedback pin, defaults to FEEDER_DEFAULT_IGNORE_FEEDBACK
*
* Example for feeder 18:
* > M700 N18 A90 B40 C20 F4 U240 V544 W2400 X0
* 
*
*/
#define GCODE_UPDATE_FEEDER_CONFIG	700


/* ------------ M-CODE: CHECK FEEDER -----------
*
* Command issued to check status of feeder
*
* Example commands:
* M710 N3
*/
#define GCODE_FEEDER_IS_OK 710


/* ------------ M-CODE: FACTORY RESET -----------
*
* Invalidate EEPROM and restore default settings given from config.h
*
*/
#define GCODE_FACTORY_RESET 799

/* ----------------
  Analog Reading Config
*/

#define ADC_READ_EVERY_MS 20

#define GCODE_GET_ADC_RAW 143
#define GCODE_GET_ADC_SCALED 144

/* ------------ M-CODE: SET SCALING -----------
*
* Read out scaled values from ADC instead raw values
*
* Parameters:
* A analog input (0...7)
* S scaling factor, multiplied with raw value read from adc
* O offset, added to raw value read from adc
*
* Example commands:
* > M145 A3 S1.00 O0.00 -> Set scaling to 1 offset to 0 for A3 (that is no scaling active)
* > M145 A0 S0.1277 O-120.23 -> Set scaling to 0.1277 offset to -120.23 for A0 (scale example for NXP vacuum sensors)
*/
#define GCODE_SET_SCALING 145



// Scaling Factors for output of GCODE_GET_ADC_SCALED
#define ANALOG_A0_SCALING_FACTOR 0.1277			//preset for NXP vacuum sensor, formula pressure [kPa]=(ADCval/1023-0.92)/0.007652
#define ANALOG_A0_OFFSET -120.23
#define ANALOG_A1_SCALING_FACTOR 0.1277			//preset for NXP vacuum sensor, formula pressure [kPa]=(ADCval/1023-0.92)/0.007652
#define ANALOG_A1_OFFSET -120.23
#define ANALOG_A2_SCALING_FACTOR 0.1277			//preset for NXP vacuum sensor, formula pressure [kPa]=(ADCval/1023-0.92)/0.007652
#define ANALOG_A2_OFFSET -120.23
#define ANALOG_A3_SCALING_FACTOR 1
#define ANALOG_A3_OFFSET 0
#define ANALOG_A4_SCALING_FACTOR 1
#define ANALOG_A4_OFFSET 0
#define ANALOG_A5_SCALING_FACTOR 1
#define ANALOG_A5_OFFSET 0
#define ANALOG_A6_SCALING_FACTOR 1
#define ANALOG_A6_OFFSET 0
#define ANALOG_A7_SCALING_FACTOR 1
#define ANALOG_A7_OFFSET 0

/* ----------------
  Switches Config
  
  EXAMPLE:
  
  turn on output on pin D0
  > M155 D0 S1;

  turn off output on pin D2
  > M155 D2 S0;
*/

#define GCODE_SET_POWER_OUTPUT 155



#define NUMBER_OF_POWER_OUTPUT 4
const static uint8_t pwrOutputPinMap[NUMBER_OF_POWER_OUTPUT] = {
	52,    // Output 1 (D0)
	50,    // Output 2 (D1)
	48,	   //...
	46,
};


//DEFINE config_h-ENDIF!!!
#endif
