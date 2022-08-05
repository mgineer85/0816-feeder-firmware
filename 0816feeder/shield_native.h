/*
 * capabilities
*/
#define NUMBER_OF_FEEDER 24
#define HAS_ENABLE_PIN
#define HAS_FEEDBACKLINES
#define HAS_ANALOG_IN
#define HAS_POWER_OUTPUTS

/*
* feederPinMap: Map IO-pins to specific feeder. First feeder is at index 0 (N0). Last feeder is NUMBER_OF_FEEDER-1
*/
const static uint8_t feederPinMap[NUMBER_OF_FEEDER] = {
  13,    // Feeder N0
  11,    // Feeder N1
  9,    //...
  7,
  5,
  3,

  14, // Feeder N6
  16,
  18,
  22,
  24,
  26,

  30, // Feeder N12
  32,
  34,
  36,
  38,
  40,

  42, //Feeder N18
  44,
  46,
  48,
  50,
  52, // Feeder N23
};


/*
* feederFeedbackPinMap: Map IO-pins to feedback-line to specific feeder. First feeder is at index 0 (N0). Last feeder is NUMBER_OF_FEEDER-1
* Feedback-line is usually connected to a microswitch to determine whether the feeder has a tape loaded and cover tape is tensioned
*
* to disable feedback-functionality completely at compile-time set pin to -1
*/
const static int8_t feederFeedbackPinMap[NUMBER_OF_FEEDER] = {
  12,    // Feeder N0
  10,    // Feeder N1
  8,    //...
  6,
  4,
  2,

  15,    // Feeder N6
  17,
  19,
  23,
  25,
  27,

  31,    // Feeder N12
  33,
  35,
  37,
  39,
  41,

  43,    // Feeder N18
  45,
  47,
  49,
  51,
  53,   // Feeder N23

};

/*
* FEEDER_ENABLE_PIN
* IO-pin with a mosfet to switch power to all feeder on/off
*
* TODO: feature can't be disabled until today
*/
#define FEEDER_ENABLE_PIN A15



/* ----------------
  MOSFET power output pinmap

  on pcb 4 output are prepared. output 1 is at index 0 in pwrOutputPinMap
*/

#define NUMBER_OF_POWER_OUTPUT 4
const static uint8_t pwrOutputPinMap[NUMBER_OF_POWER_OUTPUT] = {
  65,    // Output 1 (D0)
  66,    // Output 2 (D1)
  67,     //...
  68,
};
