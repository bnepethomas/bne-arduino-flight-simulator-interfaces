volatile long encoderCount = 0;  // Variable to store the encoder count
volatile bool lastEncoderStateA = LOW;

long lastupdate = 0;

float smoothedoutput = 0;

#include "LedControl.h"
LedControl lc = LedControl(12, 11, 10, 1);
#define DEVICE_ADDRESS 0


void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);

  // Set pin modes
  pinMode(2, INPUT_PULLUP);  // Encoder pin A
  pinMode(3, INPUT_PULLUP);  // Encoder pin B

  // Attach interrupts to pins 2 and 3
  attachInterrupt(digitalPinToInterrupt(2), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), doEncoderB, CHANGE);

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  displayFloat(118.25, 3);
}




void loop() {
  // Print encoder count for debugging
  smoothedoutput = 118 + 0.05 * int(encoderCount / 3);
  // Serial.println(String(encoderCount) + " " + String(smoothedoutput));
  displayFloat(smoothedoutput, 3);
  delay(100);
}

// Interrupt service routines
void doEncoderA() {

  int turnmultiplier = 0;
  bool A = digitalRead(2);
  bool B = digitalRead(3);

  if ((millis() - lastupdate) < 1000) {
    turnmultiplier = 3;
  }


  if (A != lastEncoderStateA) {
    if (A == B) {
      if (encoderCount <= 1400) {
        for (int i = 0; i <= turnmultiplier; i++) {}
        encoderCount++;  // Counting up
      }
    }
   else {
    if (encoderCount >= 1) {
      for (int i = 0; i <= turnmultiplier; i++) {
        encoderCount--;  // Counting down
      }
    }
  }
  lastEncoderStateA = A;
}
}

void doEncoderB() {
  // Optional: handle B channel if needed for more precise counting
}
void displayFloat(float number, int numDecimals) {
  // The display is indexed from the right: digit 0 is the rightmost digit.
  // Assuming a total display size of at least (numDecimals + 1) digits.

  // 1. Scale the number to move the decimal point past the desired precision.
  // For 3 decimal places, we multiply by 1000 (10^3).
  long scaleFactor = 1;
  for (int i = 0; i < numDecimals; i++) {
    scaleFactor *= 10;
  }

  // Round and scale the number to a long integer.
  // Adding 0.5 before casting ensures correct rounding.
  long integerValue = (long)(number * scaleFactor + 0.5);

  // Clear the display before setting new digits
  lc.clearDisplay(DEVICE_ADDRESS);

  int totalDigits = 8;  // Assumes an 8-digit display. Adjust as needed.

  // 2. Extract and display digits, starting from the decimal part.
  for (int i = 0; i < totalDigits; i++) {
    int digitIndex = i;  // The current digit position (0 is rightmost)
    bool hasDecimal = false;

    if (i < numDecimals) {
      // Decimal digits (right of the decimal point)
      // The decimal point is part of the digit *before* the decimal place.
      // So, the third decimal place (digit 0) has no DP.
      // The second decimal place (digit 1) has no DP.
      // The first decimal place (digit 2) has no DP.
    } else if (i == numDecimals) {
      // The digit *before* the decimal point (e.g., the "ones" place).
      hasDecimal = true;  // The decimal point is set on this digit.
    } else {
      // Integer digits (left of the decimal point, beyond the ones place)
    }

    // Get the current digit
    int digit = integerValue % 10;

    // Check if we are displaying leading zeros in the integer part (not including the ones place)
    if (integerValue == 0 && i > numDecimals) {
      // If the remaining number is zero and we're past the ones place, stop displaying.
      // Break once we pass the ones-place digit and are checking for higher places.
      if (i > numDecimals + 1) {  // Stop blanking after the tens place if the number is < 10
        break;
      }
    }

    // Display the digit. Note: The `setDigit` function takes the digit position from the right (0-7).
    lc.setDigit(DEVICE_ADDRESS, digitIndex, digit, hasDecimal);

    // Move to the next digit
    integerValue /= 10;

    // Stop if all digits of the integer part have been displayed
    if (integerValue == 0 && i >= numDecimals) {
      break;
    }
  }
}
