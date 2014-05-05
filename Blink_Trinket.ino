/**
 * Sense and blink.
 */

// I/O.
int test_led = 1;        // Trinket built in LED is 1, standard Arduinos use 13.
int PIN_IN_ONE = 2;      // Input for force: front.
int ANALOG_READ_ONE = 1; // Analog input for force: front.
int PIN_DATA = 0;        // Digital data out to lights.
int PIN_CLOCK = 1;       // Digital clock out to lights.

// Manage strip mangually.
const int strip_length = 10;
unsigned int strip[strip_length];

// Program.
int wait = 300;
int force_curve = 2;

// Basic setup.
void setup() {                
  // Initialize I/O.
  pinMode(test_led, OUTPUT);
  pinMode(PIN_IN_ONE, INPUT);
}

// Master looper.
void loop() {
  //float flexiforce_reading = 1.0;
  int flexiforce_reading = analogRead(ANALOG_READ_ONE);
  flexiforce_reading = flexiforce_reading - (flexiforce_reading % 50);
  unsigned int color = fscale(0, 650, 96, 0, flexiforce_reading, force_curve);

  for (int i; i < strip_length; i++) {
    //int this_color = color + (i * 3);
    strip[i] = wheel(color);
  }
  show();

  digitalWrite(test_led, HIGH);
  delay(wait);
  digitalWrite(test_led, LOW);
}


/******************************
 * UTILITY FUNCTIONS...
 */

/**
 * Send color values from pixel array to led strip.
 */
void show() {
  unsigned int i, b;
  // Pass through lights.
  for (i=0; i < strip_length; i++) {
    bitWrite(PORTD, PIN_DATA, HIGH);
    bitWrite(PORTD, PIN_CLOCK, HIGH);
    bitWrite(PORTD, PIN_CLOCK, LOW);

    // Bit shift loop.
    for (b = 0x4000; b; b >>= 1) {
      if (strip[i] & b) {
        bitWrite(PORTD, PIN_DATA, HIGH);
      }
      else {
        bitWrite(PORTD, PIN_DATA, LOW);
      }
      bitWrite(PORTD, PIN_CLOCK, HIGH);
      bitWrite(PORTD, PIN_CLOCK, LOW);
    }
  }

  latch_leds(strip_length);
}

/**
 * Activate new color pattern in LED strip.
 */
void latch_leds(int n) {
  bitWrite(PORTD, PIN_DATA, LOW);
  for (int i = 8 * n; i > 0; i--) {
    bitWrite(PORTD, PIN_CLOCK, HIGH);
    bitWrite(PORTD, PIN_CLOCK, LOW);
  }
}

/**
 * Set color value as RGB argument, values = 0..31
 */
unsigned int color(int r, int g, int b) {
  r=r & 0x1F;
  g=g & 0x1F;
  b=b & 0x1F;
  return  (b << 10) | (r << 5) | g;
}

/**
 * Colorwheel with 96 colors, 0..95
 */
unsigned int wheel(int WheelPos) {
  byte r, g, b;
  switch (WheelPos / 32) {
  case 0:
    r = 31 - WheelPos % 32;   //Red down
    g = WheelPos % 32;      // Green up
    b = 0;                  //blue off
    break;
  case 1:
    g = 31 - WheelPos % 32;  //green down
    b = WheelPos % 32;      //blue up
    r = 0;                  //red off
    break;
  case 2:
    b = 31 - WheelPos % 32;  //blue down
    r = WheelPos % 32;      //red up
    g = 0;                  //green off
    break;
  }

  return(color(r,g,b));
}

/**
 * Map with curve.
 */
float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){

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

