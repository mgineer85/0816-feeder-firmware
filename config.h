#ifndef _CONFIG_h
#define _CONFIG_h
#include "arduino.h"


/*
*     DEBUG
*/
// prints some extra information via serial
#define DEBUG

// might use a button for debugging -> TODO remove this or make at least configureable
#define PIN_BUTTON 13

/*
*  EEPROM-Settings
*/
//change to something other unique if structure of data to be saved in eeprom changed (max 3 chars)
#define CONFIG_VERSION "aaa"

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
#define NUMBER_OF_FEEDERS 24				// max 12, if 360° servo is used, max 11 feeder.
const static uint8_t feederPinMap[NUMBER_OF_FEEDERS] = {
	2,    // Feeder 1
	3,    // Feeder 2
	4,		//...
	5,
	6,
	7,
	
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
#define FEEDER_DEFAULT_RETRACT_ANGLE  10				      // [°]  usually 80, might be adjusted to servo or feeder
#define FEEDER_DEFAULT_FULL_ADVANCED_ANGLE  85				      // [°]  usually about 90.
#define FEEDER_DEFAULT_HALF_ADVANCED_ANGLE  40              // [°]  usually about 40-60.
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
* To tweak feeder parameters for improve accuracy, etc.
*
* Parameters:
* N Number of Feeder: 0...11
* A
* B
* C
* F
* U
* V
* W
*
* Example:
*
*/
#define GCODE_UPDATE_FEEDER_CONFIG	700



/* ------------ M-CODE: RETRACT -----------
*
* Set feeder to retracted position
*
* Parameters:
* N Number of Feeder, optional: 0...11
*
*/
#define GCODE_RETRACT 701


/* ------------ M-CODE: FACTORY RESET -----------
*
* Clear EEPROM and use DEFAULT values for feeder config
*
*/
#define GCODE_FACTORY_RESET 799


/* -----------------------------------------------------------------
*  COVER TAPE LEAD AWAY
*
* 360° servo-motor to lead away cover tape
* 
* If using a continuous servo motor to lead away the cover tape, it can be configured here.
* That motor needs to be switched on/off via microswitch in every feeder, but a common signal can be generated
* at the expense of one feeder less to be controlled via arduino.
*  ----------------------------------------------------------------- */
#define USE_SERVO_TO_SPOOL_COVER_TAPE     1
#if USE_SERVO_TO_SPOOL_COVER_TAPE == 1
	#define SPOOLSERVO_PIN                     12
	#define SPOOLSERVO_MIN_PULSEWIDTH          544
	#define SPOOLSERVO_MAX_PULSEWIDTH          2400
	#define SPOOLSERVO_SPEED_RATE              88    //[°]
#endif



/* ----------------
  Analog Reading Config
*/

#define GCODE_GET_ADC_RAW 143
#define GCODE_GET_ADC_SCALED 144
#define GCODE_SET_SCALING 145

#define ADC_READ_EVERY_MS 20	


/* ----------------
  Switches Config
*/

#define GCODE_SET_POWER_OUTPUT 155

/*
turn on Output 1 (D0)
> M155 D0 S1;

turn off Output 3 (D2)
> M155 D2 S0;

*/


#define NUMBER_OF_POWER_OUTPUT 4
const static uint8_t pwrOutputPinMap[NUMBER_OF_POWER_OUTPUT] = {
	52,    // Output 1 (D0)
	50,    // Output 2 (D1)
	48,	   //...
	46,
};


//DEFINE config_h-ENDIF!!!
#endif
