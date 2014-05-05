/**
 * Sense and blink.
 */

#include "SPI.h"
#include "WS2801.h"

// Trinket built in LED is 1, standard Arduinos use 13.
int test_led = 1; // Trinket built in LED.
int dataPin = 7;
int clockPin = 8;

int wait = 300;
boolean DEBUG = true;
int color_index = 0;
WS2801 strip = WS2801(1, dataPin, clockPin);

// Basic setup.
void setup() {                
  // Initialize the digital pin as an output.
  pinMode(test_led, OUTPUT);
  pinMode(A2, INPUT);
  strip.begin();

  if (DEBUG) {
    Serial.begin(9600);
  }
}

// Master looper.
void loop() {
  //float flexiforce_reading = 1.0;
  int flexiforce_reading = analogRead(A2);
  //int blink_delay = map(flexiforce_reading, 0, 1023, 30, 1200);
  flexiforce_reading = flexiforce_reading - (flexiforce_reading % 25);
  float color = fscale(0, 650, 255, 0, flexiforce_reading, 2);
  
  strip.setPixelColor(0, Wheel(color));
  strip.show();
  
  if (DEBUG) {
    Serial.print("force: ");
    Serial.print(flexiforce_reading);
    Serial.print(" color:");
    Serial.println(color);
  }

  digitalWrite(test_led, HIGH);
  delay(wait);

  digitalWrite(test_led, LOW);
}


/**
 * Map with curve.
 */
float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

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

  /*
  Serial.print(OriginalRange, DEC);  
   Serial.print("   ");  
   Serial.print(NewRange, DEC);  
   Serial.print("   ");  
   Serial.println(zeroRefCurVal, DEC);  
   Serial.println();  
   */

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {  
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
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

