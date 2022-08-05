/*
 * capabilities
*/
#define NUMBER_OF_FEEDER 48
#define NO_ENABLE_PIN
#define NO_FEEDBACKLINES
#define NO_ANALOG_IN
#define NO_POWER_OUTPUTS



/*
* feederPinMap: Map IO-pins to specific feeder. First feeder is at index 0 (N0). Last feeder is NUMBER_OF_FEEDER-1
*/
const static uint8_t feederPinMap[NUMBER_OF_FEEDER] = {
  7,6,5,4,    // Feeder N0...N3
  22,23,24,25,
  26,27,28,29,
  30,31,32,33,
  34,35,36,37,
  38,39,40,41,
  42,43,44,45,
  46,47,48,49,
  A0,A1,A2,A3,
  A4,A5,A6,A7,
  A8,A9,A10,A11,
  A12,A13,A14,A15,
};


/*
* feederFeedbackPinMap: Map IO-pins to feedback-line to specific feeder. First feeder is at index 0 (N0). Last feeder is NUMBER_OF_FEEDER-1
* Feedback-line is usually connected to a microswitch to determine whether the feeder has a tape loaded and cover tape is tensioned
*
* to disable feedback-functionality completely at compile-time set pin to -1
*/
const static int8_t feederFeedbackPinMap[NUMBER_OF_FEEDER] = {
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
  -1,-1,-1,-1,
};

/*
* FEEDER_ENABLE_PIN
* IO-pin with a mosfet to switch power to all feeder on/off
*
* TODO: feature can't be disabled until today
*/
#define FEEDER_ENABLE_PIN 8



/* ----------------
  MOSFET power output pinmap

  on pcb 4 output are prepared. output 1 is at index 0 in pwrOutputPinMap
*/

#define NUMBER_OF_POWER_OUTPUT 0
const static uint8_t pwrOutputPinMap[NUMBER_OF_POWER_OUTPUT] = {

};

