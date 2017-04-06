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
#define EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET 32


/*
*  Serial
*/
#define SERIAL_BAUD 115200
#define MAX_BUFFFER_GCODE_LINE 64	// no line can be longer than this



/* -----------------------------------------------------------------
*  FEEDER CONFIG
*  ----------------------------------------------------------------- */
#define NUMBER_OF_FEEDERS 10				// max 12, if 360° servo is used, max 11 feeder.
const static uint8_t feederPinMap[NUMBER_OF_FEEDERS] = {
	2,    // Feeder 0
	3,    // Feeder 1
	4,		//...
	5,
	6,
	7,
	8,
	9,
	10,
	11
};

#define FEEDER_PITCH  4                   // [mm]  default: 4 mm. given by mechanical design.


/* Default settings for feeders */
#define FEEDER_DEFAULT_RETRACT_ANGLE  0				      // [°]  usually 0, might be adjusted to servo or feeder
#define FEEDER_DEFAULT_FULL_ADVANCED_ANGLE  90				      // [°]  usually about 80-110. Is effected by motor constants as well!
#define FEEDER_DEFAULT_HALF_ADVANCED_ANGLE  40              // [°]  usually about 40-60. Is effected by motor constants as well!
#define FEEDER_DEFAULT_TIME_TO_SETTLE  300			  // [ms] time the servo needs to travel from ANGLE_IDLE to ANGLE_PULL
#define FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH 544		// [µs] see motor specs or experiment at bit. Value set here should bring the servo to 0°
#define FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH 2400		// [µs] see motor specs or experiment at bit. Value set here should bring the servo to 180° (even if 180° cannot be reached) (?)


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
* U
* V
* W
*
* Example:
*
*/
#define GCODE_UPDATE_FEEDER_CONFIG	601



/* ------------ M-CODE: RETRACT -----------
*
* Set feeder to retracted position
*
* Parameters:
* N Number of Feeder, optional: 0...11
*
*/
#define GCODE_RETRACT 602


/* ------------ M-CODE: FACTORY RESET -----------
*
* Clear EEPROM and use DEFAULT values for feeder config
*
*/
#define GCODE_FACTORY_RESET 699


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


#endif


