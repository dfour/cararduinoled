#include <FastLED.h>

const short SW_pin = 2; // digital pin connected to switch output
const short X_pin = 0; // analog pin connected to X output
const short Y_pin = 1; // analog pin connected to Y output

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    7
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

short BRIGHTNESS = 150;
short FRAMES_PER_SECOND = 90;
short modifier = 0;

void setup() {
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  delay(1000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
  rainbowpush,
  rainbow,
  rainbowWithGlitter,
  rainbowroad,
  centercolour,
  breathline,
  rblerp,
  gblerp,
  redgreenblue,
  nightrider,
  colourrider,
  clockwise,
  clockwisecycle,
  solid,
  breath,
  strobe,
  highbpm,
  rainbow,
  rainbowWithGlitter,
  rainbowroad,
  confetti,
  sinelon,
  juggle,
  bpm,
  turpurrlarge,
  turpurr,
  mintchoc,
  runningwater,
  vortex,
  vortexauto,
  fireeffect };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  if(analogRead(X_pin) > 800){
    BRIGHTNESS+=10;
    if(BRIGHTNESS > 255){
      BRIGHTNESS = 255;
    }
  }else if(analogRead(X_pin) < 200){
     BRIGHTNESS-=10;
    if(BRIGHTNESS < 10){
      BRIGHTNESS = 10;
    }
  }
  FastLED.setBrightness(BRIGHTNESS);

  if(analogRead(Y_pin) > 800){
    modifier+=10;

    if(modifier > 1000){
      modifier = 1000;
    }
  }else if(analogRead(Y_pin) < 200){
     modifier-=10;

    if(modifier <= 1){
      modifier =1;
    }
  }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 30 ) { nextPattern(); } // change patterns periodically
  
  if(digitalRead(SW_pin) == 0 ){
    nextPattern();
    FastLED.clear();
    delay(500);
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 2+(modifier/100));
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
  leds[pos] += CHSV( gHue, 255, 255);
}

//chars favorite
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

void strobe(){
  for( int i = 0; i < NUM_LEDS; i++) {
      if(millis() % 1000 < 500){
      leds[i] = CHSV(modifier /4, 255,255);
    }else{
      leds[i] = CRGB::Black;
    }
  }
}

void breath(){
   for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(modifier /4, 255,sin8(millis()/10));
  }
}

void turpurr(){
   for( int i = 0; i < NUM_LEDS; i++) {
    int timer = (millis() / 200) % 10;

    if(i%10 < 5 ){
      leds[(i+timer)%NUM_LEDS] = CHSV(124, 255,255);
    }else{
      leds[(i+timer)%NUM_LEDS] = CHSV(210, 255,255);
    }
   }
}

void solid(){
   for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(modifier /4, 255,255);
   }
}

void rainbowpush(){
  leds[0] = CHSV(sin8(millis()/10), 255,255);
  push();
}

void clockwise(){
  fadeToBlackBy( leds, NUM_LEDS, 20);
  
  leds[(millis()/20)%NUM_LEDS] = CHSV(modifier /4, 255,255);
  leds[((millis()/20)+NUM_LEDS/3)%NUM_LEDS] = CHSV(modifier /4, 255,255);
  leds[((millis()/20)+((NUM_LEDS/3)*2))%NUM_LEDS] = CHSV(modifier /4, 255,255);
}

void clockwisecycle(){
  fadeToBlackBy( leds, NUM_LEDS, 20);
  
  leds[(millis()/20)%NUM_LEDS] = CHSV(sin8(millis()/50), 255,255);
  leds[((millis()/20)+100)%NUM_LEDS] = CHSV(sin8(millis()/50), 255,255);
  leds[((millis()/20)+200)%NUM_LEDS] = CHSV(sin8(millis()/50), 255,255);
}

void nightrider(){
   fadeToBlackBy( leds, NUM_LEDS, 10);
  for(int i = 0; i < 10; i++){
    int range = (millis()/10)%(NUM_LEDS*2);
    if(range > NUM_LEDS){
      leds[i + NUM_LEDS - (range-NUM_LEDS)] = CHSV(modifier /4, 255,255);
    }else{
      leds[i + range] = CHSV(modifier /4, 255,255);
    }
  }
}

void colourrider(){
  fadeToBlackBy( leds, NUM_LEDS, 10);
  for(int i = 0; i < 10; i++){
    int range = (millis()/10)%(NUM_LEDS*2);
    if(range > NUM_LEDS){
      leds[i + NUM_LEDS - (range-NUM_LEDS)] = CHSV(sin8(millis()/50), 255,255);
    }else{
      leds[i + range] = CHSV(sin8(millis()/50), 255,255);
    }
  }
}


void turpurrlarge(){
   for( int i = 0; i < NUM_LEDS; i++) {
    int timer = (millis() / 100) % 30;

    if(i%30 < 15 ){
      leds[(i+timer)%NUM_LEDS] = CHSV(modifier /4, 255,200);
    }else{
      leds[(i+timer)%NUM_LEDS] = CHSV((modifier /4+75)%255, 255,200);
    }
   }
}

void redgreenblue(){
  int rgbtimer = millis() / 1000 % 3;

  if(rgbtimer == 0){
    leds[0] = CRGB::Red;
  }else if(rgbtimer == 1){
    leds[0] = CRGB::Green;
  } else{
    leds[0] = CRGB::Blue;
  }
  push();
}


void gblerp(){
  leds[0].setRGB( 0, sin8(millis()/10), cos8(millis()/10));
   push();
}

void rblerp(){
  leds[0].setRGB( cos8(millis()/10),0, sin8(millis()/10) );
   push();
}

void breathline(){
  leds[0] = CHSV(modifier /4, 255,sin8(millis()/10));
  push();
}

void centercolour(){
  leds[NUM_LEDS/2] = CHSV(modifier /4, 255,sin8(millis()/20));
  centerpop();
}

void fireeffect(){
  fadeToBlackBy( leds, NUM_LEDS, 5);
  leds[millis()%NUM_LEDS] = CHSV(millis()%42, 255,255);
}

void highbpm(){
   // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 178;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, modifier /4, beat-gHue+(i*10));
  }
}

void rainbowroad(){
  leds[0] = CHSV(random8(255), 255,sin8(millis()/10));
  push();
}


void mintchoc(){
  int colour = random8(4);
  if(colour < 2){
    leds[0] = CHSV(85+random8(10), 200+random8(55),200);
  }else{
    leds[0] = CHSV(25, 190,100);
  }
  push();
}

void runningwater(){
  leds[0] = CHSV(128+random8(40), 100+random8(155),200);
  push();
}

void vortex(){
    leds[random16(NUM_LEDS)] = CHSV(modifier /4, 255,200);
    fadeToBlackBy( leds, NUM_LEDS, 20);
    push();
}

void vortexauto(){
    leds[random16(NUM_LEDS)] = CHSV(sin8(millis()/50), 255,200);
    fadeToBlackBy( leds, NUM_LEDS, 20);
    push();
}


// push leds through strip
void push(){
  for( int i = NUM_LEDS; i > 0; i--) {
    leds[i] = leds[i-1];
    }
}

// pop leds through strip
void pop(){
  for( int i = 1; i < NUM_LEDS; i++) {
    leds[i-1] = leds[i];
    }
}

void centerpop(){
  // end to center
  for( int i = NUM_LEDS; i > NUM_LEDS/2; i--) {
    leds[i] = leds[i-1];
  }
  // start tp center
  for( int i =  0; i < NUM_LEDS/2; i++) {
    leds[i] = leds[i+1];
  }
}
