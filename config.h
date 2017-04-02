#define PIN_BUTTON 5


#define NUMBER_OF_FEEDERS 2
#define SERIAL_BAUD 115200

/*
 * Set the G-Code as desired.
 * Feeders (up to 12) will be mapped to an incrementing number:
 * Example: GCODE_ADVANCE_4MM is set to 640
 * Feeder 1 will advance if sending G641, Feeder 2 if sending G642 and so on...
 * 
 * Futher implementation will handle multiple or half feed-lengths to advance tapes with sprocket pitch more/less than 4mm.
 * e.g. Feed 8mm send G
 * 
 */
#define GCODE_ADVANCE 600						//	
//1st Parameter: feederNo
//2nd Parameter: feedlength
//3rd Parameter: speed	//maybe
#define GCODE_UPDATE_FEEDER_CONFIG	601			// set new angles in eeprom for given feeder
//1st Parameter: feederNo
//2nd Parameter: idle-angle
//3rd Parameter: pull-angle
//4th Parameter: settletime

/*
 * These are default values you can refer to in the config below for every single feeder
 */
#define FEEDER_DEFAULT_ANGLE_IDLE  5				// [°]  usually 0, might be adjusted to servo or feeder
#define FEEDER_DEFAULT_ANGLE_PULL  80				// [°]  usually about 80-110. Is effected by motor constants as well!
#define FEEDER_DEFAULT_TIME_TO_SETTLE  200			// [ms] time the servo needs to travel from ANGLE_IDLE to ANGLE_PULL
#define FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH 544		// [µs] see motor specs or experiment at bit. Value set here should bring the servo to 0°
#define FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH 2400		// [µs] see motor specs or experiment at bit. Value set here should bring the servo to 180° (even if 180° cannot be reached) (?)

/*
 * Feeder 1
 */
#define FEEDER_1_PIN                        2
#define FEEDER_1_ANGLE_IDLE                 FEEDER_DEFAULT_ANGLE_IDLE
#define FEEDER_1_ANGLE_PULL                 FEEDER_DEFAULT_ANGLE_PULL
#define FEEDER_1_TIME_TO_SETTLE             FEEDER_DEFAULT_TIME_TO_SETTLE
#define FEEDER_1_MOTOR_MIN_PULSEWIDTH       FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH
#define FEEDER_1_MOTOR_MAX_PULSEWITH        FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH

#define FEEDER_2_PIN                        3
#define FEEDER_2_ANGLE_IDLE                 FEEDER_DEFAULT_ANGLE_IDLE
#define FEEDER_2_ANGLE_PULL                 FEEDER_DEFAULT_ANGLE_PULL
#define FEEDER_2_TIME_TO_SETTLE             FEEDER_DEFAULT_TIME_TO_SETTLE
#define FEEDER_2_MOTOR_MIN_PULSEWIDTH       FEEDER_DEFAULT_MOTOR_MIN_PULSEWIDTH
#define FEEDER_2_MOTOR_MAX_PULSEWITH        FEEDER_DEFAULT_MOTOR_MAX_PULSEWITH

