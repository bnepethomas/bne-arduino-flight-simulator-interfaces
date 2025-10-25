// Rotary Encoder Sketch with Interrupts and Acceleration
// Designed for Arduino UNO/Nano/Mega

//Interrupt Number	Arduino Pin
// 0	Pin 2
// 1	Pin 3
// 2	Pin 21
// 3	Pin 20
// 4	Pin 19
// 5	Pin 18

//The Arduino Due supports the most interrupt pins by far because its microcontroller (the Atmel SAM3X8E, which uses an ARM Cortex-M3 core) 
// allows all 54 digital pins and all 12 analog pins to be used as external interrupt sources.
float smoothedoutput = 0;

#include "LedControl.h"
LedControl lc = LedControl(12, 11, 10, 1);
#define DEVICE_ADDRESS 0


#include <Arduino.h>

// --- ENCODER PIN DEFINITIONS ---
// Change these to match your wiring
const int ENCODER_CLK_PIN = 2;  // Must be an interrupt pin (2 or 3 on Uno/Nano)
const int ENCODER_DT_PIN = 3;   // Must be an interrupt pin (2 or 3 on Uno/Nano)
const int ENCODER_SW_PIN = 4;   // Standard digital pin for the push button

// --- ENCODER STATE VARIABLES ---
volatile long encoderPos = 0;             // Current position (volatile for ISR access)
volatile byte lastEncoded = 0;            // Stores the previous state of CLK and DT pins
volatile unsigned long lastTurnTime = 0;  // Time of the last detected turn (for acceleration)

// --- ACCELERATION CONFIGURATION ---
// These thresholds define how fast the encoder must be turned to trigger acceleration.
const unsigned long FAST_TURN_THRESHOLD_US = 1000;    // Time in microseconds for 'fast' turn
const unsigned long MEDIUM_TURN_THRESHOLD_US = 5000;  // Time in microseconds for 'medium' turn
const int FAST_ACCEL_FACTOR = 4;                      // Multiplier for very fast turns
const int MEDIUM_ACCEL_FACTOR = 2;                    // Multiplier for medium-speed turns
const int NORMAL_FACTOR = 1;                          // Multiplier for slow turns

// --- BUTTON STATE VARIABLES ---
unsigned long buttonPressTime = 0;
const int DEBOUNCE_DELAY_MS = 50;

// --- INTERRUPT SERVICE ROUTINE (ISR) ---
// This function is executed every time the CLK or DT pin state changes.
void readEncoder() {
  // Read the current state of the CLK and DT pins
  byte MSB = digitalRead(ENCODER_CLK_PIN);  // Most Significant Bit
  byte LSB = digitalRead(ENCODER_DT_PIN);   // Least Significant Bit

  // Combine the two bits into a single byte value (0-3)
  byte encoded = (MSB << 1) | LSB;

  // Calculate the change in state using the gray code sequence logic
  byte sum = (lastEncoded << 2) | encoded;  // Form a 4-bit state history

  // This logic works by looking at the 4-bit sequence:
  // 10->00->01 = CW turn (increment position)
  // 01->00->10 = CCW turn (decrement position)

  int change = 0;
  if (sum == 0b1000 || sum == 0b0001 || sum == 0b0111 || sum == 0b1110) {
    // Clockwise (CW) Turn
    change = 1;
  } else if (sum == 0b0100 || sum == 0b0010 || sum == 0b1011 || sum == 0b1101) {
    // Counter-Clockwise (CCW) Turn
    change = -1;
  }

  // Only apply acceleration if a valid turn was detected
  if (change != 0) {
    unsigned long currentTime = micros();
    unsigned long elapsedTime = currentTime - lastTurnTime;  // Time since the last step
    lastTurnTime = currentTime;

    int factor = NORMAL_FACTOR;

    // Apply acceleration logic based on turn speed
    if (elapsedTime < FAST_TURN_THRESHOLD_US) {
      factor = FAST_ACCEL_FACTOR;
    } else if (elapsedTime < MEDIUM_TURN_THRESHOLD_US) {
      factor = MEDIUM_ACCEL_FACTOR;
    }

    // Update position with acceleration factor
    encoderPos += (long)change * factor;
  }

  // Store the current state for the next check
  lastEncoded = encoded;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Encoder with Acceleration and Interrupts Started");

  // --- PIN SETUP ---
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);  // Button pin

  // --- INTERRUPT SETUP ---
  // Attach the ISR to the two data pins (CLK and DT)
  // RISING, FALLING, or CHANGE can be used. CHANGE is the most reliable
  // and detects movement on both edges.
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT_PIN), readEncoder, CHANGE);

  // Initialize lastEncoded state
  lastEncoded = (digitalRead(ENCODER_CLK_PIN) << 1) | digitalRead(ENCODER_DT_PIN);

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  displayFloat(118.25, 3);
}


void loop() {
  // --- ENCODER POSITION REPORTING ---

  // Use 'noInterrupts()' block to safely read the volatile variable
  // while preventing the ISR from modifying it mid-read.
  static long lastReportedPos = 0;
  long currentPos;

  noInterrupts();
  currentPos = encoderPos;
  interrupts();

  if (currentPos != lastReportedPos) {

    smoothedoutput = 118 + 0.05 * int(currentPos / 3);
    displayFloat(smoothedoutput, 3);
    Serial.print("Position: ");
    Serial.print(currentPos);
    Serial.print(" (Change: ");
    Serial.print(currentPos - lastReportedPos);
    Serial.println(")");
    lastReportedPos = currentPos;
  }

  // --- BUTTON HANDLING (Non-interrupt based debounce) ---

  // The button is active LOW due to INPUT_PULLUP
  int buttonState = digitalRead(ENCODER_SW_PIN);

  if (buttonState == LOW) {
    // Check if enough time has passed since the last reported press
    if ((millis() - buttonPressTime) > DEBOUNCE_DELAY_MS) {
      Serial.println("--- BUTTON PRESSED (Reset Position) ---");

      // Reset position
      noInterrupts();
      encoderPos = 0;
      interrupts();
      lastReportedPos = 0;

      buttonPressTime = millis();  // Update time of press
    }
  } else {
    // If the button is released, reset the debounce timer reference
    buttonPressTime = 0;
  }
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
