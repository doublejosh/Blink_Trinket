/**
 * Make rainbows when you dance.
 */
#include "Adafruit_NeoPixel.h"

// Configs.
const int skip      = 4;   // Skip every X pixel. Helps with power and heat.
const int shift     = 15;  // Speed the color shifts around the strip (0-255).
const int wait      = 70;  // Delay in light chasing speed.
const float reduce  = 0.1; // Adjust the brightness. Helps with power and heat.
const int heartbeat = 10;  // Blink a heart beat to indicate it's on.

// Don't edit below if you're not sure...

// Hardware.
const int test_led = 1;        // Trinket built in LED is 1, standard Arduinos use 13. Debug.
const int PIN_IN_ONE = 2;      // Input for force: front.
const int ANALOG_READ_ONE = 1; // Analog input for force: front.
const int PIN_IN_TWO = 4;      // Input for force: front.
const int ANALOG_READ_TWO = 2; // Analog input for force: front.
const int PIN_DATA = 1;        // Digital data out to lights.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(35, PIN_DATA, NEO_GRB + NEO_KHZ800);

// Program.
const int FORCE_MAX = 1023;
int previous_force_reading;
int flexiforce_reading;
int adjust_reading;
boolean should_show = false;

void setup() {
  // Initialize hardware.
  pinMode(test_led, OUTPUT); // Debug.
  pinMode(PIN_IN_ONE, INPUT);
  pinMode(PIN_IN_TWO, INPUT);
  pinMode(PIN_DATA, OUTPUT);
  strip.begin();
}

void loop() {
  //digitalWrite(test_led, HIGH); // Debug.

  // Cycle all 256 colors in the wheel.
  for (int j=0; j < 256*shift; j=j+shift) {

    // Check force and settings changes.
    adjust_reading = analogRead(ANALOG_READ_TWO);
    flexiforce_reading = analogRead(ANALOG_READ_ONE);
    should_show = (flexiforce_reading - previous_force_reading) > (adjust_reading / 3);

    for (int q=0; q < skip; q++) {
      for (int i=0; i < strip.numPixels(); i=i+skip) {
        // Only display when stepped on.
        if (should_show) {
          // Turn every third pixel on.
          strip.setPixelColor(i+q, Wheel(((i * 256 / strip.numPixels()) - j) & 255));    
        }
        else {
          // Turn pixel off.
          strip.setPixelColor(i+q, 0);
        }
      }
      strip.show();

      for (int i=0; i < strip.numPixels(); i=i+skip) {
        // Turn every other pixel off.
        strip.setPixelColor(i+q, 0);
      }
      delay(wait);
    }

    // Heartbeat pulse.
    if ((j/shift) % heartbeat == 0 && (!should_show)) {
      strip.setPixelColor(1, Color(20, 0, 5));
    }

    previous_force_reading = flexiforce_reading;
  }

  //digitalWrite(test_led, LOW); // Debug.
}

/**
 * Create a 24 bit color value from R,G,B.
 */
uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;

  r = r * reduce;
  g = g * reduce;
  b = b * reduce;

  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

/**
 * Input a value 0 to 255 to get a color value.
 * The colors are a transition r - g -b - back to r.
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

