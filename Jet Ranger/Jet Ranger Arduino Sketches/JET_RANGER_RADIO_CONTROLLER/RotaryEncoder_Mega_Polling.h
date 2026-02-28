// RotaryEncoder_Mega_Polling.h
#ifndef ROTARY_ENCODER_MEGA_POLLING_H
#define ROTARY_ENCODER_MEGA_POLLING_H

#include <Arduino.h>

// Encoder state machine values
#define ENC_STATE_00 0
#define ENC_STATE_01 1
#define ENC_STATE_10 2
#define ENC_STATE_11 3

// Maximum encoders supported (no interrupt limitation)
#define MAX_ENCODERS 8

class RotaryEncoder {
private:
  // Pin configuration
  uint8_t pinA;
  uint8_t pinB;

  // Position and state
  int16_t position;
  int16_t lastPosition;
  int16_t minValue;
  int16_t maxValue;

  // Debouncing (state tracking)
  uint8_t lastStateA;
  uint8_t lastStateB;
  uint32_t lastDebounceTime;
  uint8_t debounceDelay;

  // Acceleration
  uint32_t lastEncoderEventTime;
  uint16_t timeBetweenEvents;
  uint8_t accelerationCounter;
  uint8_t minEventInterval;
  uint8_t maxEventInterval;
  uint8_t accelerationThreshold;
  uint8_t accelerationMultiplier;
  bool accelerationEnabled;

  // Fast lookup table for quadrature decoding
  static const int8_t quadratureTable[16] PROGMEM;

  // Inline methods for speed
  inline void updatePosition();
  inline uint8_t readPins();
  inline int8_t calculateAcceleration();

public:
  RotaryEncoder(uint8_t pinA, uint8_t pinB, int16_t minVal = -128, int16_t maxVal = 127,
                uint8_t debounceMs = 2);

  void begin();
  void end();

  // Main polling method (call in loop)
  void poll();

  // Getters
  inline int16_t getPosition() const {
    return position;
  }
  inline int16_t getChange();
  inline uint16_t getTimeBetweenEvents() const {
    return timeBetweenEvents;
  }
  inline uint8_t getAccelerationCounter() const {
    return accelerationCounter;
  }
  inline uint8_t getCurrentMultiplier() const;

  // Setters
 
  inline void reset();
  inline void setDebounceDelay(uint8_t delayMs) {
    debounceDelay = delayMs;
  }
  inline void setAccelerationParams(uint8_t minInt, uint8_t maxInt, uint8_t thresh, uint8_t mult) {
    minEventInterval = minInt;
    maxEventInterval = maxInt;
    accelerationThreshold = thresh;
    accelerationMultiplier = mult;
  }
  inline void enableAcceleration(bool enable) {
    accelerationEnabled = enable;
  }
  inline void RotaryEncoder::setPosition(int16_t value) {
  position = constrain(value, minValue, maxValue);
  lastPosition = position;
}
};

#endif