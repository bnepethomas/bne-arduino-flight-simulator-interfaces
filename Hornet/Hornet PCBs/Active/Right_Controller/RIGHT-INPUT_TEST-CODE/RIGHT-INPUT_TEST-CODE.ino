// Define the row and column pins
const int rowPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34};
const int colPins[] = {35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45};

const int numRows = sizeof(rowPins) / sizeof(rowPins[0]);
const int numCols = sizeof(colPins) / sizeof(colPins[0]);

void setup() {
  Serial.begin(9600);

  // Initialize row pins as outputs
  for (int i = 0; i < numRows; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH); // Set row lines HIGH initially
  }

  // Initialize column pins as inputs with pull-up resistors
  for (int j = 0; j < numCols; j++) {
    pinMode(colPins[j], INPUT_PULLUP);
  }
}

void loop() {
  int switchNumber = 1; // Start switch numbering from 1

  // Loop through each row
  for (int row = 0; row < numRows; row++) {
    // Set the current row LOW to scan
    digitalWrite(rowPins[row], LOW);
    delayMicroseconds(5); // small delay for stabilization

    // Loop through each column
    for (int col = 0; col < numCols; col++) {
      int colState = digitalRead(colPins[col]);
      // When using pull-up resistors, LOW means pressed
      if (colState == LOW) {
        Serial.print("Switch ");
        Serial.print(switchNumber);
        Serial.println(" PRESSED");
      }
      switchNumber++;
    }

    // Set the row back HIGH
    digitalWrite(rowPins[row], HIGH);
  }

  delay(100); // Delay to slow down the scan for readability
}
