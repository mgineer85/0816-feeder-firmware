#ifndef _CONFIG_h
#define _CONFIG_h
//#include "arduino.h"
#include <stdint.h>


/*
*     DEBUG
*/
// prints some extra information via serial
// uncomment to disable in production
//#define DEBUG

/*
*  Select controller shield
*/
// defaults to the native shield, optional sensor shield can be selected
#define CONTROLLER_SHIELD SHIELD_NATIVE     //SHIELD_NATIVE or SHIELD_SENSOR
//change config_version, if change shield!


/*
*  EEPROM-Settings
*/
//change to something other unique if structure of data to be saved in eeprom changed (max 3 chars)
#define CONFIG_VERSION "aab"

/*
*  Serial
*/
#define SERIAL_BAUD 115200


/* -----------------------------------------------------------------
*  FEEDER CONFIG
*  -----------------------------------------------------------------
* default settings per feeder

// calculate angles: https://de.wikipedia.org/wiki/Schubkurbel, crankshift


	when setting these values, make sure, the values for
	FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH and
	FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH
	reflect the according position for servo being used

	when everything is setup right, the lever is in advanced position at servo-angle ~90 degree
	fully retracted is about ~20°


	| STATE RETRACTED, looking from the top
	|
	|   .
	|  /_\
	|   |			<- direction tape is advanced
	|   |
	|
	| -x-----		<- advancing lever, x falls into the hole of a tape and advances it.
	| -------
	| ----
	| ----
	| ----
	| ----
	| -------
	| -------
	| -------
	| -\\o\\-       <- piston / Pleuelstange
	|   \\\\\
	|    \\\\\
	|     \\\\\
	|      \\\\\
	|    O====o\\
	|    ^   \\\\\
	|    \_ servo-motor at FEEDER_DEFAULT_RETRACT_ANGLE (about 20°)

	| STATE FULL ADVANCED, looking from the top
	|
	|   .
	|  /_\
	|   |			<- direction tape is advanced
	|   |
	|
	| -x-----		<- advancing lever, x falls into the hole of a tape and advances it.
	| -------
	| ----
	| ----
	| ----
	| ----
	| -------
	| -------
	| -------
	| -||o||-       <- piston / Pleuelstange
	|  |||||
	|  |||||
	|  |||||
	|  |||||
	|  |||||
	|  ||o||
	|    :
	|    :			<- arm of servo motor fully extended
	|    :
	|    :
	|    0			<- servo-motor at FEEDER_DEFAULT_FULL_ADVANCED_ANGLE (about 90°)

*/
#define FEEDER_DEFAULT_FULL_ADVANCED_ANGLE  90				      // [°]  usually 90 (type: uint8_t)
#define FEEDER_DEFAULT_HALF_ADVANCED_ANGLE  44              // [°]  exact math would be 43.85. may need tweaking. only needed if advancing half pitch (for 0401 smds) (type: uint8_t)
#define FEEDER_DEFAULT_RETRACT_ANGLE  15				      // [°]  usually 20, chose 15 to be failsafe (type: uint8_t)
#define FEEDER_DEFAULT_FEED_LENGTH FEEDER_MECHANICAL_ADVANCE_LENGTH			// [mm] distance to be fed if no feedlength was given in a feed command
#define FEEDER_DEFAULT_TIME_TO_SETTLE  240			  // [ms] time the servo needs to travel from FEEDER_DEFAULT_FULL_ADVANCED_ANGLE to FEEDER_DEFAULT_RETRACT_ANGLE (type: uint8_t -> max 255ms)
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


/* ----------------
  Analog Reading Config
*/
// ADC is polled regularly and scaled afterwards. When a command is issued, the pre-calculated values are sent to host immediately.
#define ADC_READ_EVERY_MS 20

// Scaling Factors to convert raw ADC value to real units.
#define ANALOG_A0_SCALING_FACTOR 0.1277			//preset for NXP vacuum sensor, formula pressure [kPa]=(ADCval/1023-0.92)/0.007652
#define ANALOG_A0_OFFSET -120.23
#define ANALOG_A1_SCALING_FACTOR 0.1277			//preset for NXP vacuum sensor, formula pressure [kPa]=(ADCval/1023-0.92)/0.007652
#define ANALOG_A1_OFFSET -120.23
#define ANALOG_A2_SCALING_FACTOR 1
#define ANALOG_A2_OFFSET 0
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







// ------------------------------------------------------
//
//
//
//
//
//
//
// STOP
// do not edit stuff below this line...
//
//
//
//
//
//
//
//
// ------------------------------------------------------




//where in eeprom to store common settings and feeder specific data
#define EEPROM_COMMON_SETTINGS_ADDRESS_OFFSET 4
#if CONTROLLER_SHIELD == NATIVE_SHIELD
  #define EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET 128
#else
  #define EEPROM_FEEDER_SETTINGS_ADDRESS_OFFSET 8
#endif

//buffer size for serial commands received
#define MAX_BUFFFER_MCODE_LINE 64	// no line can be longer than this

//to calculate how often advancing has to be repeated if commanded to advance more than 4 millimeter per feed
#define FEEDER_MECHANICAL_ADVANCE_LENGTH  4                   // [mm]  default: 4 mm. fixed as per mechanical design.

/* -----------------------------------------------------------------
*  M-CODES
*  ----------------------------------------------------------------- */


#define MCODE_ADVANCE 600
#define MCODE_RETRACT_POST_PICK 601
#define MCODE_FEEDER_IS_OK 602
#define MCODE_SERVO_SET_ANGLE 603
#define MCODE_SET_FEEDER_ENABLE 610
#define MCODE_UPDATE_FEEDER_CONFIG	620

#define MCODE_GET_ADC_RAW 143
#define MCODE_GET_ADC_SCALED 144
#define MCODE_SET_SCALING 145

#define MCODE_SET_POWER_OUTPUT 155

#define MCODE_FACTORY_RESET 799



//DEFINE config_h-ENDIF!!!
#endif
