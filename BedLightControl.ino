#include <Wire.h>
#include <FastLED.h>

// BLV mgn Cube Bed Light Control
// Connect DUET2 Wifi using I2C
// Command on I2C (like M150)
// I2C address : 8
// Red   : 0-255
// Green : 0-255
// Blue  : 0-255
// Brightness : 0-31
// LED number : 1-256
// Function : 0-end of constant color set, or all same color
//            1-start or middle of constant color set
//            2-LED demo mode (same as Demoreel100 in FastLED library
//
// Examples (commands from DuetWeb screen or PanelDue)
// M260 A8 B255:255:255:31:46:0 default power on all white(255:255:255) led with maximum brightness(31), 46 leds
// M260 A8 B255:0:0:15:46:0 Red(255:0:0) leds with half brightness(15), 46 leds
// M260 A8 B255:0:0:8:16:1 Start Red(255:0:0) leds with quater brightness(8), 16 leds
// M260 A8 B0:255:0:8:16:1 Next Green(0:255:0) leds with quater brightness(8), 16 leds (first 16 leds are Red)
// M260 A8 B0:0:255:8:14:0 Last Blue(0:0:255) leds with quater brightness(8), 14 leds (first Red, next Green, last Blue)
// M260 A8 B0:0:0:15:46:2 Demo mode with half brightness(15), RGB and LED numbers ignore
//
// Caution: you need these libraries,
//  Wire : Arduino standard library
//  FastLED : http://fastled.io/
//
// Caution: you must use Arduino 1.8.5 or later, but not use 1.9.0 beta
//
// 2019.09.16 Y.Mita

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014
// -Y.Mita modified 2019

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//#define DATA_PIN    3
#define DATA_PIN    11
//#define CLK_PIN   4
#define CLK_PIN   13
//#define LED_TYPE    WS2811
#define LED_TYPE    APA102
#define COLOR_ORDER GRB
// use for demo and no command default
#define NUM_LEDS    46
// maximum number of leds in this sketch
#define SIZ_ARRAY  256
CRGB leds[SIZ_ARRAY];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

unsigned int R = 255;
unsigned int G = 255;
unsigned int B = 255;
unsigned int Bright = 31;
unsigned int Number = NUM_LEDS;
unsigned int Function = 0;
unsigned int Pos = 0;

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  Wire.begin(8);                // join i2c bus with address #150
  Wire.onReceive(receiveCommand); // register event
  
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{

  switch (Function) {
    case 0:
      // end of constant color set
      for(int p =Pos; p < Pos + Number; p++) {
        leds[p].setRGB(G,B,R);
      }
      Pos = 0;
      Function = -1;
      break;
    case 1:
      // start or middle of constant color set
      for(int p = Pos; p < Pos + Number; p++) {
        leds[p].setRGB(G,B,R);
      }
      Pos = Pos + Number;
      Function = -1;
      break;
    case 2:
      // Call the current pattern function once, updating the 'leds' array
      gPatterns[gCurrentPatternNumber]();
      break;
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  if(Function == 2) {
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveCommand(int howMany) {
//  Serial.print(howMany);
  if(howMany != 6) {  // command format fail!
    for(int i = 0; i < howMany ; i++)
      char c = Wire.read();   // read and ignore 
    return;
  }

  // recieve command
  R = Wire.read();
  G = Wire.read();
  B = Wire.read();
  Bright = Wire.read(); // set global brighness (can't set locally)
  if(Bright > 31) Bright = 31;
  if(Bright == 0)
    FastLED.setBrightness(0);
  else
    FastLED.setBrightness(dim8_raw(Bright * 8 + 7));
  Number = Wire.read();
  if(Pos + Number > NUM_LEDS) Number = NUM_LEDS;
  Function = Wire.read();
  if(Function > 2) Function = 0;

  return;
}

// under here, original "demoreel100" source code

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
