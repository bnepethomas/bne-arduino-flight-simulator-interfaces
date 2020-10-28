#include "FastLED.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// RGB Calibration code
//
// Use this sketch to determine what the RGB ordering for your chipset should be.  Steps for setting up to use:

// * Uncomment the line in setup that corresponds to the LED chipset that you are using.  (Note that they
//   all explicitly specify the RGB order as RGB)
// * Define DATA_PIN to the pin that data is connected to.
// * (Optional) if using software SPI for chipsets that are SPI based, define CLOCK_PIN to the clock pin
// * Compile/upload/run the sketch

// You should see six leds on.  If the RGB ordering is correct, you should see 1 red led, 2 green
// leds, and 3 blue leds.  If you see different colors, the count of each color tells you what the
// position for that color in the rgb orering should be.  So, for example, if you see 1 Blue, and 2
// Red, and 3 Green leds then the rgb ordering should be BRG (Blue, Red, Green).

// You can then test this ordering by setting the RGB ordering in the addLeds line below to the new ordering
// and it should come out correctly, 1 red, 2 green, and 3 blue.
//
//////////////////////////////////////////////////

#define NUM_LEDS 52
// Pin for driving the Led Chain
#define DATA_PIN 7

const int buttonPin = 2;            // the number of the pushbutton pin

int i = 0;
unsigned long Timelastchange = 0;

int buttonState;                    // the current reading from the input pin
int lastButtonState = HIGH;         // the previous reading from the input pin


unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

const unsigned long delaywhilewaitingforinput = 1000;


bool ButtonPressed = false;
CRGB leds[NUM_LEDS];
int LedColourInt = 0;


void setup() {
  
    pinMode(buttonPin, INPUT_PULLUP);
    
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  
}



void loop() {


  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {

  

    if (reading != buttonState) {
      buttonState = reading;

      ButtonPressed = true;
  
      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {

        LedColourInt++; 
        if (LedColourInt > 3){
          LedColourInt = 0;       
        }

        if (LedColourInt == 0) {
          for (i=0; i<= NUM_LEDS-1; i++) {
            leds[i] = CRGB(255,0,0);
          }
          FastLED.show();          
        }
        else if (LedColourInt == 1) {
          for (i=0; i<= NUM_LEDS-1; i++) {
            leds[i] = CRGB(0,255,0);
          }
          FastLED.show();          
        }
        else if (LedColourInt == 2) {
          for (i=0; i<= NUM_LEDS-1; i++) {
            leds[i] = CRGB(0,0,255);
          }
          FastLED.show();          
        }
        else if (LedColourInt == 3) {
          for (i=0; i<= NUM_LEDS-1; i++) {
            leds[i] = CRGB(0,0,255);
          }
          FastLED.show();          
        }
      }
    }
  }


  if (ButtonPressed == false) {
    if ((millis() - Timelastchange) >= delaywhilewaitingforinput){
      Timelastchange = millis();

      LedColourInt++; 
      if (LedColourInt > 3){
        LedColourInt = 0;       
      }
      
      if (LedColourInt == 0) {
        for (i=0; i<= NUM_LEDS-1; i++) {
          leds[i] = CRGB(255,0,0);
        }
        FastLED.show();          
      }
      else if (LedColourInt == 1) {
        for (i=0; i<= NUM_LEDS-1; i++) {
          leds[i] = CRGB(0,255,0);
        }
        FastLED.show();          
      }
      else if (LedColourInt == 2) {
        for (i=0; i<= NUM_LEDS-1; i++) {
          leds[i] = CRGB(0,0,255);
        }
        FastLED.show();          
      }
      else if (LedColourInt == 3) {
        for (i=0; i<= NUM_LEDS-1; i++) {
          leds[i] = CRGB(0,0,255);
        }
        FastLED.show();          
      }
      
    }
    
  }
  lastButtonState = reading;
}
