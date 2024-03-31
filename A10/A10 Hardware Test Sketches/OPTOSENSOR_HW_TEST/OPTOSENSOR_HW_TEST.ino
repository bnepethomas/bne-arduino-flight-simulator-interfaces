/*

Inbuilt LED changes state - if nothing is blocking sensor led is on.
CONNECT PIN 7 TO OPTO SENSOR OUTPUT

*/

bool SensorState = false;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(7, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  SensorState = digitalRead(7);
  if (SensorState == true) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {                           
                                     
    digitalWrite(LED_BUILTIN, LOW);  
  }                                 
}
