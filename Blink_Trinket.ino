/**
 * Sense and blink.
 */
 
#include "Simple_WS2801.h"

// I/O.
//const int test_led = 1;        // Trinket built in LED is 1, standard Arduinos use 13.
const int PIN_IN_ONE = 2;      // Input for force: front.
const int ANALOG_READ_ONE = 1; // Analog input for force: front.
const int PIN_DATA = 0;        // Digital data out to lights.
const int PIN_CLOCK = 1;       // Digital clock out to lights.
const int FORCE_STEPS = 15;

// Manage strip mangually.
Simple_WS2801 strip = Simple_WS2801(7, PIN_DATA, PIN_CLOCK);

// Program.
const int force_curve = 8;
const int color_curve = 0;

// Utility.
const int WHEEL_MAX = 255;

// Basic setup.
void setup() {
  // Initialize hardware.
  //pinMode(test_led, OUTPUT);
  pinMode(PIN_IN_ONE, INPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  strip.begin();
}

// Master looper.
void loop() {
  int flexiforce_reading = analogRead(ANALOG_READ_ONE);
  flexiforce_reading = flexiforce_reading - (flexiforce_reading % FORCE_STEPS);
  int color = fscale(0, 650, 0, WHEEL_MAX, flexiforce_reading, color_curve);
  float blink_delay = fscale(0, 1023, 400, 10, flexiforce_reading, force_curve);

  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(color));
  }
  strip.show();

  /*
  digitalWrite(test_led, HIGH);
  delay(blink_delay);
  digitalWrite(test_led, LOW);
  delay(blink_delay);
  */
}


/******************************
 * UTILITY FUNCTIONS...
 */

/**
 * Create a 24 bit color value from R,G,B
 */
uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

/**
 * Input a value 0 to 255 to get a color value.
 * The colours are a transition r - g -b - back to r
 */
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/**
 * Map with curve.
 */
int fscale(int originalMin, int originalMax, int newBegin, int newEnd, int inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;

  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println();
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax) {
    return 0;
  }

  if (invFlag == 0) {
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else {  
    rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}

