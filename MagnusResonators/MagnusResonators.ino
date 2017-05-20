

#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <string.h>
#include "animation.hpp"
#include "circbuff.hpp"

// first communication pin for neo pixel string
const unsigned int BASE_PIN = 2;

const unsigned int NUM_STRINGS = 8;

const uint16_t LEDS_PER_STRAND = 120; // 120
const bool RGBW_SUPPORT = false; // true
const unsigned int QUEUE_SIZE = 3;

// Mask to clear 'upper case' ASCII bit
const char CASE_MASK = ~0x20;

const unsigned long LED_UPDATE_PERIOD = 5; // in ms. Time between drawing a frame on _any_ LED strip.

//const long BAUD_RATE = 115200;
const long BAUD_RATE = 9600;

// Command format:
// Fnnnnnnnnp12345678mmmm\n
// F = faction: EeRrNn. Capitalized on state change; lowercase if same as before
// n = resonator level
// p = portal health percentage
// 1-8 = resonator health percentage
// m = mod status
//
// percentage health encoded as single character. Space (' ') is 0%, and then each ASCII character
// above it is 2% greater, up to 'R' for 100%
//
// Mods:
// ' ' - No mod present in this slot
// '0' - FA Force Amp
// '1' - HS-C Heat Shield, Common
// '2' - HS-R Heat Shield, Rare
// '3' - HS-VR Heat Shield, Very Rare
// '4' - LA-R Link Amplifier, Rare
// '5' - LA-VR Link Amplifier, Very Rare
// '6' - SBUL SoftBank Ultra Link
// '7' - MH-C MultiHack, Common
// '8' - MH-R MultiHack, Rare
// '9' - MH-VR MultiHack, Very Rare
// 'A' - PS-C Portal Shield, Common
// 'B' - PS-R Portal Shield, Rare
// 'C' - PS-VR Portal Sheild, Very Rare
// 'D' - AXA AXS Shield
// 'E' - T Turret

const unsigned int COMMAND_LENGTH = 22;
const unsigned int PORTAL_STRENGTH_INDEX = 9;

enum Direction { NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };
//enum Ownership {  neutral = 0, enlightened, resistance };
static int16_t resonatorLevel[8] = { 8,8,8,8,8,8,8,8 };
static int16_t newResonatorLevel[8] = { 8,8,8,8,8,8,8,8 };
enum SerialStatus { IDLE, IN_PROGRESS, COMMAND_COMPLETE };

// Static animation implementations singleton
Animations animations;

typedef CircularBuffer<Animation *, QUEUE_SIZE> QueueType;

AnimationState states[NUM_STRINGS][QUEUE_SIZE];
QueueType animationQueues[NUM_STRINGS];

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(120, BASE_PIN, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS_PER_STRAND, BASE_PIN, (RGBW_SUPPORT ? NEO_GRBW : NEO_GRB) + NEO_KHZ800);


// Serial I/O
const int ioSize = 64;
char command[32];
int8_t in_index = 0;

unsigned long nextUpdate;

void start_serial(void)
{
  in_index = 0;
  Serial.begin(BAUD_RATE);
}

SerialStatus collect_serial(void)
{
    SerialStatus status = in_index == 0 ? IDLE : IN_PROGRESS;
    int avail = Serial.available();
    while (Serial.available() > 0)
    {
        int8_t ch = Serial.read();
        if ( ch == '\n' )
        {
            command[in_index] = 0; // terminate
            if ( in_index > 0 )
            {
                in_index = 0;
                return COMMAND_COMPLETE;
            }
        }
        else if (ch != '\r' && ch != '.') // ignore CR
        {
            command[in_index] = ch;
            if ( in_index < (sizeof(command)/sizeof(command[0])) - 2 ) {
                in_index++;
            }
        }
    }
    return status;
}

uint8_t getPercent(const char *buffer)
{
  unsigned long inVal = strtoul(buffer, NULL, 10);
  return uint8_t( constrain(inVal, 0, 100) );
}

unsigned int decodePercent(const char encoded) {
    return (encoded - ' ') * 2;
}

uint8_t dir;
uint8_t percent;
Ownership owner;    // 0=neutral, 1=enl, 2=res


// the setup function runs once when you press reset or power the board
void setup()
{
  start_serial();
  strip.begin();
  uint8_t i;
  for ( i = 0; i < NUM_STRINGS; i++)
  {
    strip.setPin(i + BASE_PIN);
    strip.show(); // Initialize all pixels to 'off'
  }

  dir = 0;
  //owner = neutral;
  owner = initial;
  percent = 0;
  nextUpdate = millis();
}

// the loop function runs over and over again forever
void loop()
{
    uint16_t i, val;
    unsigned long now = millis();

    SerialStatus status = collect_serial();
    if (status == COMMAND_COMPLETE)
    {
        // we have valid buffer of serial input
        char cmd = command[0];
        
        for (int i = 0; i < 8; i++) {
          int level = command[i+1]; 
          if ((resonatorLevel[i] == 0) and (level != '0')) {
            Serial.println("Deploy Resonator - case 1");
          }
          if ((resonatorLevel[i] !=0) and (level == '0')) {
            Serial.println("Destroy Resonator - case 2");
          }
          
          newResonatorLevel[i] = level - '0'; 
        }
        
        switch (cmd) {
            Color c;
            case '*':
                Serial.println("Magnus Resonators Node");
                break;
            case 'E':
            case 'R':
            case 'e':
            case 'r':
                owner = (cmd & CASE_MASK) == 'E' ? enlightened : resistance;
                percent = getPercent(&command[1]);
                uint8_t red, green, blue, white;
                c = ToColor(0x00, 
                        owner == enlightened ? 0xff : 0x00,
                        owner == resistance ? 0xff : 0x00,
                        0x00);
                for (int i = 0; i < NUM_STRINGS; i++) {
                    if (resonatorLevel[i] != newResonatorLevel[i]) {
                        resonatorLevel[i] = newResonatorLevel[i];
                        QueueType& animationQueue = animationQueues[i];
                        animationQueue.setTo(&animations.movingPulse);
                        unsigned int stateIdx = animationQueue.lastIdx();
                        double initialPhase = ((double) i) / NUM_STRINGS;
                        animations.movingPulse.init(now, states[i][stateIdx], strip, resonatorLevel[i], owner); //, initialPhase);
                    }
                }
                break;

            case 'N':
            case 'n':
                owner = neutral;
                percent = 20;
                /*
                if (RGBW_SUPPORT) {
                    c = ToColor(0x00, 0x00, 0x00, 0xff);
                } else {
                    c = ToColor(0xff, 0xff, 0xff);
                }
                */
                c = ToColor(0x00);
                for (int i = 0; i < NUM_STRINGS; i++) {
                    QueueType& animationQueue = animationQueues[i];
                    /*
                    animationQueue.setTo(&animations.redFlash);
                    unsigned int stateIdx = animationQueue.lastIdx();
                    animations.redFlash.init(now, states[i][stateIdx], strip, RGBW_SUPPORT);
                    animationQueue.add(&animations.solid);
                    */

                    animationQueue.setTo(&animations.solid);
                    unsigned int stateIdx = animationQueue.lastIdx();
                    animations.solid.init(now, states[i][stateIdx], strip, c);
                }
                break;
                
                /*
                owner = neutral;
                percent = 20;
                if (RGBW_SUPPORT) {
                    c = ToColor(0x00, 0x00, 0x00, 0xff);
                } else {
                    c = ToColor(0xff, 0xff, 0xff);
                }
                for (int i = 0; i < NUM_STRINGS; i++) {
                    QueueType& animationQueue = animationQueues[i];
                    animationQueue.setTo(&animations.movingPulse);
                    unsigned int stateIdx = animationQueue.lastIdx();
                    double initialPhase = ((double) i) / NUM_STRINGS;
                    animations.movingPulse.init(now, states[i][stateIdx], strip, resonatorLevel[i], owner); //, initialPhase);
                }
                break;
                */

            default:
                Serial.print("Unkwown command "); Serial.println(cmd);
                break;
        }
        Serial.print((char *)command); Serial.print(" - ");Serial.print(command[0],DEC);Serial.print(": "); 
        Serial.print("owner "); Serial.print(owner,DEC); Serial.print(", percent "); Serial.println(percent,DEC); 
    }

    // If we're in the process of getting a serial command, don't service the LEDs: this process disables interrupts,
    // which can cause us to miss characters
    if (status == IDLE && now >= nextUpdate) {
        nextUpdate = now + LED_UPDATE_PERIOD;

        strip.setPin(dir + BASE_PIN);  // pick the string

        QueueType& animationQueue = animationQueues[dir];
        unsigned int queueSize = animationQueue.size();
        if (queueSize > 1) {
            if (animationQueue.peek()->done(now, states[dir][animationQueue.currIdx()])) {
                animationQueue.remove();
                queueSize--;
                animationQueue.peek()->start(now, states[dir][animationQueue.currIdx()]);
            }
        }
        if (queueSize > 0) {
            //strip.setBrightness(255);
            strip.setBrightness((uint8_t)((uint16_t)(255*(percent/100.0))));
            //pCurrAnimation->doFrame(states[0], strip);
            animationQueue.peek()->doFrame(now, states[dir][animationQueue.currIdx()], strip);
        }

        // Update one strand each time through the loop
        dir = (dir + 1) % NUM_STRINGS;
    }
}
