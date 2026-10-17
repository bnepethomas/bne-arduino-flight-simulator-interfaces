// Define the total number of encoders
#define NUM_ENCODERS 6

// Define the pins for each rotary encoder (A and B phases)
// The Arduino Due supports interrupts on ALL digital pins.
const int ENCODER_PINS_A[NUM_ENCODERS] = { 3, 38, 40, 42, 44, 46 };
const int ENCODER_PINS_B[NUM_ENCODERS] = { 4, 39, 41, 43, 45, 47 };

// Volatile variables for interrupt handling for each encoder
// These arrays store the state for each of the NUM_ENCODERS.
volatile long encoderValues[NUM_ENCODERS] = {0}; // Stores the current encoder count
volatile unsigned long lastEncoderChangeTimes[NUM_ENCODERS] = {0}; // Stores the micros() timestamp of the last encoder change
volatile int encoderDirections[NUM_ENCODERS] = {0}; // 1 for CW, -1 for CCW, 0 for no change detected by ISR

// Debounce and acceleration parameters (shared across all encoders for simplicity)
const unsigned long DEBOUNCE_DELAY_US = 5000; // Microseconds to debounce (adjust as needed)
const unsigned long FAST_TURN_THRESHOLD_US = 50000; // If step takes less than this, consider it a fast turn
const int ACCELERATION_MULTIPLIER = 5; // How much to multiply the step by during fast turns

// --- ISRs for each encoder ---
// Each encoder needs a dedicated ISR. This ISR will be attached to BOTH
// the A and B pins of its respective encoder. When either pin changes,
// this ISR will execute, read both pins, and determine the direction.

void readEncoder0() {
  unsigned long currentTime = micros();
  // Basic software debounce
  if (currentTime - lastEncoderChangeTimes[0] < DEBOUNCE_DELAY_US) return;

  int pinA_state = digitalRead(ENCODER_PINS_A[0]);
  int pinB_state = digitalRead(ENCODER_PINS_B[0]);

  if (pinA_state == pinB_state) {
    encoderDirections[0] = -1; // Counter-Clockwise
  } else {
    encoderDirections[0] = 1;  // Clockwise
  }
  lastEncoderChangeTimes[0] = currentTime;
}

void readEncoder1() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[1] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[1]);
  int pinB_state = digitalRead(ENCODER_PINS_B[1]);
  if (pinA_state == pinB_state) {
    encoderDirections[1] = -1;
  } else {
    encoderDirections[1] = 1;
  }
  lastEncoderChangeTimes[1] = currentTime;
}

void readEncoder2() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[2] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[2]);
  int pinB_state = digitalRead(ENCODER_PINS_B[2]);
  if (pinA_state == pinB_state) {
    encoderDirections[2] = -1;
  } else {
    encoderDirections[2] = 1;
  }
  lastEncoderChangeTimes[2] = currentTime;
}

void readEncoder3() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[3] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[3]);
  int pinB_state = digitalRead(ENCODER_PINS_B[3]);
  if (pinA_state == pinB_state) {
    encoderDirections[3] = -1;
  } else {
    encoderDirections[3] = 1;
  }
  lastEncoderChangeTimes[3] = currentTime;
}

void readEncoder4() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[4] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[4]);
  int pinB_state = digitalRead(ENCODER_PINS_B[4]);
  if (pinA_state == pinB_state) {
    encoderDirections[4] = -1;
  } else {
    encoderDirections[4] = 1;
  }
  lastEncoderChangeTimes[4] = currentTime;
}

void readEncoder5() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[5] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[5]);
  int pinB_state = digitalRead(ENCODER_PINS_B[5]);
  if (pinA_state == pinB_state) {
    encoderDirections[5] = -1;
  } else {
    encoderDirections[5] = 1;
  }
  lastEncoderChangeTimes[5] = currentTime;
}

// Array of ISR function pointers for easier iteration in setup
void (*encoderISRs[NUM_ENCODERS])() = {
  readEncoder0,
  readEncoder1,
  readEncoder2,
  readEncoder3,
  readEncoder4,
  readEncoder5
};


void setup() {
  Serial.begin(115200);
  Serial.println("Arduino Due Multiple Encoders with Acceleration Test");

  for (int i = 0; i < NUM_ENCODERS; i++) {
    // Configure encoder pins as inputs with internal pull-up resistors
    pinMode(ENCODER_PINS_A[i], INPUT_PULLUP);
    pinMode(ENCODER_PINS_B[i], INPUT_PULLUP);

    // Attach interrupts to both pins of each encoder
    // Both A and B pins of a single encoder will call the SAME ISR
    attachInterrupt(digitalPinToInterrupt(ENCODER_PINS_A[i]), encoderISRs[i], CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PINS_B[i]), encoderISRs[i], CHANGE);
  }
}

void loop() {
  for (int i = 0; i < NUM_ENCODERS; i++) {
    long currentEncoderValueSnapshot;
    unsigned long currentLastChangeTimeSnapshot;
    int currentEncoderDirectionSnapshot;

    // Safely read volatile variables that might be updated by the ISR.
    // 'noInterrupts()' temporarily disables interrupts to prevent data corruption
    // if the ISR tries to update the variables while they are being read in loop().
    noInterrupts(); // Disable interrupts
    currentEncoderValueSnapshot = encoderValues[i]; // (Not strictly needed here as it's updated later)
    currentLastChangeTimeSnapshot = lastEncoderChangeTimes[i];
    currentEncoderDirectionSnapshot = encoderDirections[i];
    encoderDirections[i] = 0; // Reset direction flag after reading it
    interrupts(); // Re-enable interrupts

    // Process the encoder change outside the ISR
    if (currentEncoderDirectionSnapshot != 0) { // If a change was detected by the ISR
      long actualStep = currentEncoderDirectionSnapshot; // Default step is 1 or -1

      unsigned long timeSinceLastChange = micros() - currentLastChangeTimeSnapshot;

      // Apply acceleration if the rotation is fast
      if (timeSinceLastChange < FAST_TURN_THRESHOLD_US) {
        actualStep *= ACCELERATION_MULTIPLIER;
      }

      // Update the main encoder count with acceleration
      encoderValues[i] += actualStep;

      Serial.print("Encoder ");
      Serial.print(i);
      Serial.print(" Value: ");
      Serial.print(encoderValues[i]);
      Serial.print(", Step: ");
      Serial.print(actualStep);
      Serial.print(", Time since last change (us): ");
      Serial.println(timeSinceLastChange);
    }
  }

  // Other tasks for your Arduino Due can go here.
  // A small delay can be useful to prevent flooding the Serial Monitor,
  // but remove or adjust it based on your application's needs.
  delay(10);
}