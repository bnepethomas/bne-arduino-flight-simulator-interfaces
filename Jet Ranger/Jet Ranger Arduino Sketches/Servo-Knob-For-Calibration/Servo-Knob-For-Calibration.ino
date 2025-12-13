/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create Servo object to control a servo

int lastpotchange = 0;
int potpin = A0;  // analog pin used to connect the potentiometer
int val;          // variable to read the value from the analog pin

void setup() {

  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
}

void loop() {
  val = analogRead(potpin);  // reads the value of the potentiometer (value between 0 and 1023)
  if (val != lastpotchange) {
    lastpotchange = val;
    Serial.println(String(lastpotchange));
  }
  val = map(val, 1023, 0, 0, 180);  // scale it for use with the servo (value between 0 and 180)
  myservo.write(val);               // sets the servo position according to the scaled value
  delay(15);                        // waits for the servo to get there
}

void EngineTorque(int torque) {
  int val = map(torque, 0, 120, 33, 809);
  myservo.write(val);
}

void TransmissionPressure(int pressure) {
  int val = map(pressure, 0, 150, 9, 288);
  myservo.write(val);
}

void TransmissionTemperature(int temperature) {
  int val = map(temperature, 0, 150, 424, 107);
  myservo.write(val);
}

void EnginePressure(int pressure) {
  int val = map(pressure, 0, 150, 560, 864);
  myservo.write(val);
}

void EngineTemperature(int temperature) {
  int val = map(temperature, 0, 150, 310, 20);
  myservo.write(val);
}

void GasProducer(int gaspercentage) {
  int val = map(gaspercentage, 0, 106, 57, 848);
  myservo.write(val);
}

void EGT(int temperature) {

  if (temperature <= 600) {
    int val = map(temperature, 0, 600, 121, 256);
  } else if (temperature <= 700) {
    int val = map(temperature, 600, 700, 256, 442);
  } else if (temperature <= 800) {
    int val = map(temperature, 700, 800, 442, 656);
  } else  {
    int val = map(temperature, 800, 910, 656, 802);
  }
  
  myservo.write(val);
}

void FuelPressure(int pressure) {
  int val = map(pressure, 0, 30, 280, 73);
  myservo.write(val);
}

void ElectricalLoad(int load) {
  int val = map(load, 0, 100, 527, 740);
  myservo.write(val);
}

void FuelLevel(int Level) {
  // Note Fuel tank is 75 Gallons
  // So multiplying by 10
  int val = map(Level, 0, 75, 124, 736);
  myservo.write(val);
}

void VSI(int FPM) {
  /*
  -6000 10
  -4000 105
  -2000 268
  -1000 388
  0     498
  1000  620
  2000  728
  4000  866
  6000  952
  */
}

void TurbineSpeed(int Speed) {
  int val = map(Speed, 0, 120, 242, 986);
  myservo.write(val);
}

void RotorSpeed(int Speed) {
  int val = map(Speed, 0, 120, 28, 895);
  myservo.write(val);
}


void Airspeed(int AirSpeed) {

  if (AirSpeed <= 20) {
    int val = map(AirSpeed, 0, 20, 44, 70);
  } else if (AirSpeed <= 80) {
    int val = map(AirSpeed, 20, 80, 70, 531);
  } else if (AirSpeed <= 100) {
    int val = map(AirSpeed, 80, 100, 531, 654);
  } else  {
    int val = map(AirSpeed, 100, 150, 654, 955);
  }
  
  myservo.write(val);
}

