#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <stdlib.h>
#include <string.h>

// first communication pin for neo pixel string
#define BASE_PIN 2

enum Direction { NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };
enum Ownership {  neutral = 0, enlightened, resistance };

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#define NUM_OF_LEDS 120

Adafruit_NeoPixel resonator[] = {
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 0, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 1, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 3, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_OF_LEDS, BASE_PIN + 7, NEO_GRB + NEO_KHZ800),
};



#define L0 0x000000
#define L1 0xEE8800
#define L2 0xFF6600
#define L3 0xCC3300
#define L4 0x990000
#define L5 0xFF0033
#define L6 0xCC0066
#define L7 0x660066
#define L8 0x330033
#define GREEN 0x00FF00
#define BLUE  0x0000FF

#define L0D 0x000000
#define L1D 0x332200
#define L2D 0x441500
#define L3D 0x240800
#define L4D 0x240000
#define L5D 0x330011
#define L6D 0x270015
#define L7D 0x150015
#define L8D 0x070007
#define GREEND 0x001500
#define BLUED  0x000015

uint8_t dir;
uint8_t percent;
uint8_t owner;    // 0=neutral, 1=enl, 2=res
static int16_t resonatorLocation[8] = { 48,48,48,48,48,48,48,48 }; // ascii zero

// Serial I/O
const int ioSize = 64;
int8_t command[ioSize];
int8_t in_index = 0;


void setup() {
  uint8_t i;
  start_serial();

  dir = NORTH; // begin on the north resonator
  owner = neutral;
  percent = 0;
    
  for(i=0; i<8; i++) {
    resonator[i].begin();
    resonator[i].show(); // Initialize all pixels to 'off'
  }
}


void start_serial(void) {
  in_index = 0;
  Serial.begin(115200);
}


void loop()
{
  uint16_t i, val;
  uint32_t ownerColor;

  update_status();
  pulse_animation();
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
        owner = resistance;
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
        resonatorLocation[i] = level; 
        deploy_resonator(i);
      }  
    }
  }
}


// This function will play an animation of a resonator being deployed at "location" 
void deploy_resonator(uint16_t location) {
  Serial.print("ResonatorDeployed...");
  Serial.print(location);
  Serial.write('-');
  Serial.write(resonatorLocation[location]);
  Serial.println();
}

// This function will play a pulsing animation for all 8 locations
// This fuction will run every cycle, so keep track on the index to play next frame
void pulse_animation() {
 
  // Do magic stuff here  
}

// This function will play an animation for ownership change or portal being nuetralized
void change_owner() {
  Serial.print("Owner Changed...");
  Serial.print(owner);
  Serial.println();  
  // Do magic stuff here
}
