// RotaryEncoder_Mega_Polling.cpp
#include "RotaryEncoder_Mega_Polling.h"

// Quadrature lookup table stored in FLASH
const int8_t RotaryEncoder::quadratureTable[16] PROGMEM = {
  0,  -1,  1,  0,    // from 00
  1,   0,  0, -1,    // from 01
 -1,   0,  0,  1,    // from 10
  0,   1, -1,  0     // from 11
};

RotaryEncoder::RotaryEncoder(uint8_t pinA, uint8_t pinB, int16_t minVal, int16_t maxVal, 
                             uint8_t debounceMs)
  : pinA(pinA), pinB(pinB), position(0), lastPosition(0), 
    minValue(minVal), maxValue(maxVal), lastStateA(0), lastStateB(0),
    lastDebounceTime(0), debounceDelay(debounceMs),
    lastEncoderEventTime(0), timeBetweenEvents(0), accelerationCounter(0),
    minEventInterval(5), maxEventInterval(80), accelerationThreshold(3),
    accelerationMultiplier(3), accelerationEnabled(true) {}

void RotaryEncoder::begin() {
  // Set pins as inputs with pull-ups
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  
  // Read initial states
  lastStateA = digitalRead(pinA);
  lastStateB = digitalRead(pinB);
  
  // Initialize timing
  lastDebounceTime = millis();
  lastEncoderEventTime = millis();
}

void RotaryEncoder::end() {
  // Nothing to do for polling
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
}

// Read both pins
inline uint8_t RotaryEncoder::readPins() {
  uint8_t stateA = digitalRead(pinA);
  uint8_t stateB = digitalRead(pinB);
  return (stateA << 1) | stateB;
}

inline int8_t RotaryEncoder::calculateAcceleration() {
  if (!accelerationEnabled) return 1;
  
  uint32_t currentTime = millis();
  timeBetweenEvents = currentTime - lastEncoderEventTime;
  lastEncoderEventTime = currentTime;
  
  // Clamp to bounds
  if (timeBetweenEvents < minEventInterval) {
    timeBetweenEvents = minEventInterval;
  }
  if (timeBetweenEvents > maxEventInterval) {
    timeBetweenEvents = maxEventInterval;
    accelerationCounter = 0;
    return 1;
  }
  
  // Calculate midpoint
  uint8_t midpoint = (minEventInterval + maxEventInterval) >> 1;
  
  if (timeBetweenEvents < midpoint) {
    if (accelerationCounter < 255) {
      accelerationCounter++;
    }
  } else {
    accelerationCounter = 0;
  }
  
  // Apply acceleration
  if (accelerationCounter >= accelerationThreshold) {
    uint8_t range = maxEventInterval - minEventInterval;
    uint8_t speedDiff = maxEventInterval - timeBetweenEvents;
    uint8_t mult = 1 + ((speedDiff * (accelerationMultiplier - 1)) / range);
    return constrain(mult, 1, 8);
  }
  
  return 1;
}

inline void RotaryEncoder::updatePosition() {
  // Read current states
  uint8_t newState = readPins();
  uint8_t oldState = (lastStateA << 1) | lastStateB;
  
  // Get direction from lookup table
  int8_t direction = pgm_read_byte(&quadratureTable[(oldState << 2) | newState]);
  
  if (direction != 0) {
    int8_t multiplier = calculateAcceleration();
    position += (direction * multiplier);
    
    // Clamp to min/max
    if (position > maxValue) position = maxValue;
    else if (position < minValue) position = minValue;
  }
  
  // Update states
  lastStateA = (newState >> 1) & 1;
  lastStateB = newState & 1;
}

void RotaryEncoder::poll() {
  // Simple debouncing: only update if enough time has passed
  uint32_t currentTime = millis();
  
  if (currentTime - lastDebounceTime >= debounceDelay) {
    updatePosition();
    lastDebounceTime = currentTime;
  }
}

inline int16_t RotaryEncoder::getChange() {
  int16_t change = position - lastPosition;
  lastPosition = position;
  return change;
}

inline void RotaryEncoder::setPosition(int16_t value) {
  position = constrain(value, minValue, maxValue);
  lastPosition = position;
}

inline void RotaryEncoder::reset() {
  position = 0;
  lastPosition = 0;
  accelerationCounter = 0;
  lastEncoderEventTime = millis();
  lastDebounceTime = millis();
}

// inline void RotaryEncoder::setAccelerationParams(uint8_t minInt, uint8_t maxInt, uint8_t thresh, uint8_t mult) {
//   minEventInterval = minInt;
//   maxEventInterval = maxInt;
//   accelerationThreshold = thresh;
//   accelerationMultiplier = mult;
// }

inline uint8_t RotaryEncoder::getCurrentMultiplier() const {
  if (!accelerationEnabled || accelerationCounter < accelerationThreshold) {
    return 1;
  }
  
  uint8_t range = maxEventInterval - minEventInterval;
  uint8_t speedDiff = maxEventInterval - timeBetweenEvents;
  return 1 + ((speedDiff * (accelerationMultiplier - 1)) / range);
}




