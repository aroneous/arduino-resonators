#include <Adafruit_NeoPixel.h>

#include <stdlib.h>
#include <string.h>

//#include "FastLED.h"

struct pixel_string {
  uint16_t phase;
  uint32_t timing;
};


enum Direction { NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };
enum Ownership {  neutral = 0, enlightened, resistance };

#define BASE_PIN 2
#define NUM_STRINGS 8
#define NUM_LEDS_PER_STRIP 120

pixel_string strings[NUM_STRINGS];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, BASE_PIN, NEO_GRB + NEO_KHZ800);

static int ledIndex = 12;
static const int32_t resonatorColor[9] = {
  0x000000, // L0
  0xEE8800, // L1
  0xFF6600, // L2
  0xCC3300, // L3
  0x990000, // L4
  0xFF0033, // L5
  0xCC0066, // L6
  0x660066, // L7
  0x330033, // L8
} ;

#define GREEN 0x00FF00
#define BLUE  0x0000FF
#define BLACK 0x000000
#define GREY  0x111111

uint8_t dir;
uint8_t percent;
uint8_t owner;    // 0=neutral, 1=enl, 2=res
int32_t ownerColor;
static int16_t resonatorLocation[8] = { 0,0,0,0,0,0,0,0 }; // ascii zero

// Serial I/O
const int ioSize = 64;
int8_t command[ioSize];
int8_t in_index = 0;


void setup() {
  uint8_t i;
  start_serial();
  strip.begin();

  dir = NORTH; // begin on the north resonator
  owner = neutral;
  ownerColor = GREY;
  percent = 0;
    
  for( i = 0; i<NUM_STRINGS; i++) {
    strip.setPin(i+BASE_PIN);
    strip.show(); // Initialize all pixels to 'off'
    strings[i].phase = 0;
    strings[i].timing = 0;
  }
}


void start_serial(void) {
  in_index = 0;
  Serial.begin(115200);
}


void loop() {
  uint16_t i, val;
  uint32_t ownerColor;

  update_status();
  pulse_resonators();
}


bool collect_serial(void) {
  while ( Serial.available() > 0 ) {
    int8_t ch = Serial.read();
    if (ch == '\n') {
      command[in_index] = 0; // terminate
      if (in_index > 0) {
        in_index = 0;
        return true;
      }
    } else if (ch == 0x0a || ch == 0x13) { 
      // carriage return or linefeed (ignore
    } else {
      command[in_index] = ch;
      if (in_index < (sizeof(command) / sizeof(command[0])) - 2) {
        in_index++;
      }
      //  in_index = 0;
    }
  }
  return false;
}


// This function parses the serial command and sets the owner and 
// resonator levels accordingly
void update_status() {
  int16_t level;
    if (collect_serial()) {
    // we have valid buffer of serial input
    switch (command[0]) {
      case '*':
        Serial.println("Magnus Resonators Node");
        break;
      case 'E':
      case 'e':
        if (owner != enlightened) {
          owner = enlightened;
          change_owner();
        }
        break;
      case 'R':
      case 'r':
        if (owner != resistance) {
          owner = resistance;
          change_owner();
        }
        break;
      case 'n':
      case 'N':
        if (owner != neutral) {
          owner = neutral;
          change_owner();
        }
        break;
      default:
        break;
    }
    for (int i = 0; i < 8; i++) {
      level = command[i+1]; 
      if (resonatorLocation[i] != level) { // if resonator is not same as previous deploy new
        resonatorLocation[i] = level - '0'; 
        deploy_resonator(i);
      }  
    }
  }
}


// This function will play an animation of a resonator being deployed at "location" 
void deploy_resonator(uint16_t location) {
  //Serial.print("ResonatorDeployed...");
  //Serial.print(location);
  //Serial.write('-');
  //Serial.write(resonatorLocation[location] + '0');
  //Serial.println();
}

// This function will play a pulsing animation for all 8 locations
// This fuction will run every cycle, so keep track on the index to play next frame
void pulse_resonators() {
 
  // Do magic stuff here
  

  if (strings[dir].timing < millis() ) {
    strings[dir].timing += 100; // every 100 milliseconds we will check this direction
    strip.setPin(dir + BASE_PIN); // pick the string
    for (int i = 0; i < 12; i++) {
      strip.setPixelColor(i, ownerColor);  // first 12 LEDs nearest core will be color of the owner
      for (int j = 1; j < 10; j++) {   // address 10 segments of 12 leds each
        strip.setPixelColor(i + (j*12), resonatorColor[resonatorLocation[dir]]);
      }
      //strip.setBrightness((uint8_t)((uint16_t)(255 * percent) / 100));
    }
    strip.show();
    dir++;
    if ( dir >= NUM_STRINGS ) {
      dir = 0;
    }
}


}

// This function will play an animation for ownership change or portal being nuetralized
void change_owner() {
  //Serial.print("Owner Changed...");
  //Serial.println();  
  if (owner == enlightened) { 
    ownerColor = GREEN; 
  }
  if (owner == resistance) {
    ownerColor = BLUE;
  }
  if (owner == neutral) {
    ownerColor = GREY;
  } 
  // Do magic stuff here
}


void fadeall() {
    for(int i = NUM_LEDS_PER_STRIP-1; i > 0; i--) {
    // do fade stuff here?
    }
}





