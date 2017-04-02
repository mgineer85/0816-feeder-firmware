#include "FeedManager.h"
#include "config.h"

//----------------- Include Libraries
#include <SoftwareSerial.h>
#include <Bounce2.h>
//#include <ControlledServo.h>   TODO: maybe later?
#include <Servo.h>
#include "FeedManager.h"

//----------------- variable setup


//debounce one switch for testing purposes
Bounce debouncedButton = Bounce(); 

// common vars
int pos = 0;
  
void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial);
	Serial.println(F("Feeduino starting...")); Serial.flush();//setup servo objects
	FeedManager.setup();
  
  
	// Setup the button
	pinMode(PIN_BUTTON,INPUT_PULLUP);
	debouncedButton.attach(PIN_BUTTON);
	debouncedButton.interval(5); // interval in ms
	
	
	
}

void loop() {

	debouncedButton.update();
	FeedManager.update();
  
	//feeder1.writeMicroseconds(1);
	if ( debouncedButton.fell() ) {
		//FeedManager.advance(1);
		if(pos==0) {
			FeedManager.feeders[0].write(10);
			pos=1;
		} else {
			FeedManager.feeders[0].write(80);
			pos=0;
		}
	}

}
