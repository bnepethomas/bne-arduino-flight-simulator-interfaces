int previousValues[16] = {0}; // Array to store previous analog readings for each pin
int analogPins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
const int numAnalogPins = 16;

void setup() {
  Serial.begin(115200);
  // Optionally set pinMode for analog pins as INPUT, though it's the default
  for (int i = 0; i < numAnalogPins; i++) {
    pinMode(analogPins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < numAnalogPins; i++) {
    int currentValue = analogRead(analogPins[i]);
    if (currentValue != previousValues[i]) {
      Serial.print("Pin ");
      Serial.print(analogPins[i], DEC);
      Serial.print(": ");
      Serial.print(currentValue);
      Serial.print(" (previous: ");
      Serial.print(previousValues[i]);
      Serial.println(")");
      previousValues[i] = currentValue;
    }
  }
  delay(10); // Small delay to debounce and prevent rapid updates
}