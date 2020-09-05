// OMX-10

#include "Adafruit_Keypad.h"
#include <Adafruit_NeoPixel.h>
#include <MIDI.h>

#include "ClearUI.h"
#include "sequencer.h"


#define LED_PIN    14
#define LED_COUNT 25

int mode = 0;
const char* modes[] = {"MIDI","SEQ"};

MIDI_CREATE_DEFAULT_INSTANCE();

// the MIDI channel number to send messages
const int midiChannel = 1;

// POTS/analog inputs				// CCS mapped to Organelle Defaults
int pots[] = {21,22,23,24,7};		// the MIDI CC (continuous controller) for each analog input
int previous[] = {-1,-1,-1,-1,-1};	// store previously sent values, to detect changes
int analogPins[] = {23,22,21,20,16};	// teensy pins for analog inputs
int analogValues[] = {0,0,0,0,0};		// default values

elapsedMillis msec = 0;

// KEYSWITCH ROWS/COLS
const byte ROWS = 5; //four rows
const byte COLS = 5; //four columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {0,1,2,3,4},
  {5,6,7,8,9},
  {10,11,12,13,14},
  {15,16,17,18,19},
  {20,21,22,23,24}
  };
byte rowPins[ROWS] = {6, 4, 3, 5, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 10, 9, 15}; //connect to the column pinouts of the keypad

int notes[] = {0,
 61,63,   66,68,70,   73,75,   78,80,82,
60,62,64,65,67,69,71,72,74,76,77,79,81,83};


Encoder myEncoder(12, 11); // encoder pins
Button encButton(0);
long newPosition = 0;
long oldPosition = -999;



//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

// Declare NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ####### SETUP #######

void setup() {
	Serial.begin(9600);
	

  	// display
	initializeDisplay();
		// startup screen		
		display.clearDisplay();
		testdrawrect();
		delay(200);
		display.clearDisplay();
		display.setCursor(16,0);
		display.setTextSize(2);
		display.setTextColor(SSD1306_WHITE);
		display.println("OMX-25");
		display.display();

	// keypad
	customKeypad.begin();

	// hardware midi
	MIDI.begin();
  
	// Handle incoming MIDI events
	//MIDI.setHandleClock(handleExtClock);
	//MIDI.setHandleStart(handleExtStart);
	//MIDI.setHandleContinue(handleExtContinue);
	//MIDI.setHandleStop(handleExtStop);
    //MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
    //usbMIDI.setHandleNoteOn(HandleNoteOn); 
    //MIDI.setHandleControlChange(HandleControlChange);
    //usbMIDI.setHandleControlChange(HandleControlChange);
    //MIDI.setHandleNoteOff(HandleNoteOff);
    //usbMIDI.setHandleNoteOff(HandleNoteOff);


	//LEDs
	strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	strip.show();            // Turn OFF all pixels ASAP
	strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

   for(int i=0; i<LED_COUNT; i++) { // For each pixel...
    strip.setPixelColor(i, strip.Color(150, 150, 150));
    strip.show();   // Send the updated pixel colors to the hardware.
    delay(20); // Pause before next pass through loop
  }
  rainbow(10);
  
  delay(100);
  strip.fill(0, 0, LED_COUNT);
  strip.show();            // Turn OFF all pixels ASAP

  delay(100);

  display.clearDisplay();
  display.display();

  //Serial.println(" loading... ");
}

void enc_selector(uint16_t z, uint16_t pos) {
	if (z == 1){
		display.clearDisplay();
		display.setCursor(16, 2);
		display.print("ENC: ");
		display.print(pos);
		display.display();
	}
}

void loop() {
	customKeypad.tick();
	

	// ENCODER
	auto u = myEncoder.update();
	if (u.active()) {
    	//Serial.print(u.dir() < 0 ? "ccw " : "cw ");
    	auto amt = u.accel(0); // where 5 is the acceleration factor if you want it, 0 if you don't)
    	mode = constrain(mode + amt, 0, 1);
    	Serial.println(mode);
		display.clearDisplay();
		display.setTextSize(1);
		display.setCursor(2, 0);
		display.print("MODE: ");
		display.print(mode);
		display.display();			
	}
	auto s = encButton.update();
	switch (s) {
		case Button::Down: Serial.println("down"); break;
		case Button::DownLong: Serial.println("downlong"); break;
		case Button::Up: Serial.println("up"); break;
		case Button::UpLong: Serial.println("uplong"); break;
	}
		
	// POTS
	switch(mode) {
		case 0:
			// --- READ POTS to MIDI ---
			if (msec >= 20) {
				msec = 0;
				for(int i=0; i<5; i++) {
					analogValues[i] = analogRead(analogPins[i]) / 8;
				}
				for(int i=0; i<5; i++) {
					if (analogValues[i] != previous[i]){
						usbMIDI.sendControlChange(pots[i], analogValues[i], midiChannel);
						MIDI.sendControlChange(pots[i], analogValues[i], midiChannel);
						previous[i] = analogValues[i];
						display.clearDisplay();
						display.setTextSize(1);
						display.setCursor(2, 0);
						display.print("MODE: ");
						display.print(mode);				
						display.setTextSize(2);
						display.setCursor(2, 12);
						display.print("Pot");
						display.print(i+1);
						display.print(": ");
						display.print(analogValues[i]);
						display.display();
					}
				}
			}
			break;
		case 1:
			break;
	}
	while(customKeypad.available()){
		keypadEvent e = customKeypad.read();
		display.clearDisplay();
		int thisKey = e.bit.KEY;
		display.setTextSize(1);
		display.setCursor(2, 0);
		display.print("MODE: ");
		display.print(mode);				

		switch(mode) {
		case 0: // midi controller
				//while(customKeypad.available()){
				//keypadEvent e = customKeypad.read();
				//int thisKey = e.bit.KEY;
				//Serial.print(e.bit.KEY);
				//Serial.print(":");
				//Serial.print(notes[e.bit.KEY]);
			if (e.bit.EVENT == KEY_JUST_PRESSED && thisKey != 0) {
			  //Serial.println(" pressed");
			  usbMIDI.sendNoteOn(notes[thisKey], 100, midiChannel);
			  MIDI.sendNoteOn(notes[thisKey], 100, midiChannel);
			  strip.setPixelColor(thisKey, strip.Color(0,   0,   128));         //  Set pixel's color (in RAM)
			  
			
			display.setTextSize(2);
			display.setCursor(2, 12);
			display.print("Note: ");
			display.print(notes[thisKey]);
			  

			} else if(e.bit.EVENT == KEY_JUST_RELEASED && thisKey != 0) {
			  //Serial.println(" released");
			  usbMIDI.sendNoteOff(notes[thisKey], 0, midiChannel);
			  MIDI.sendNoteOff(notes[thisKey], 0, midiChannel);
			  strip.setPixelColor(thisKey, strip.Color(128,   128,   128));         //  Set pixel's color (in RAM)
			}
			//strip.show();                          //  Update strip to match
			
			// what to do with AUX key?
			if (e.bit.EVENT == KEY_JUST_PRESSED && thisKey == 0) {
			  usbMIDI.sendControlChange(25, 100, midiChannel);
			  MIDI.sendControlChange(25, 100, midiChannel);
			  strip.setPixelColor(thisKey, strip.Color(128,   0,   0));
			
			} else if (e.bit.EVENT == KEY_JUST_RELEASED && thisKey == 0) { 
			  usbMIDI.sendControlChange(25, 0, midiChannel);
			  MIDI.sendControlChange(25, 0, midiChannel);
			  strip.setPixelColor(thisKey, strip.Color(128,   128,   128));
			}
			strip.show();						
			break;
		case 1: // sequencer
			// 
			break;
		}
		display.display();
	}
	
	// DISPLAY STUFF
	
	



	while (usbMIDI.read()) {
	// ignore incoming messages
	}
	while (MIDI.read()) {
	// ignore incoming messages
	}
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 1*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(500);
}
