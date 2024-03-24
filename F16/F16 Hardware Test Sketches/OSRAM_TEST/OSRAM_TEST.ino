/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(8, OUTPUT);   //OSRAM 1 Write
  pinMode(22, OUTPUT);  //D0
  pinMode(23, OUTPUT);  //D1
  pinMode(24, OUTPUT);  //D2
  pinMode(25, OUTPUT);  //D3
  pinMode(26, OUTPUT);  //D4
  pinMode(27, OUTPUT);   //D5
  pinMode(28, OUTPUT);   //D6

  digitalWrite(22, LOW);  //D0
  digitalWrite(23, LOW);  //D1
  digitalWrite(24, LOW);  //D2
  digitalWrite(25, LOW);  //D3
  digitalWrite(26, HIGH);  //D4
  digitalWrite(27, HIGH);   //D5
  digitalWrite(28, LOW);   //D6

  digitalWrite(8, HIGH);   //D6
  delay(1);
  digitalWrite(8, LOW);   //D6
  delay(1);
  digitalWrite(8, HIGH);   //D6

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
