// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

const int zeroPin =  12;
const int ledPin = 13;
const int zeroOffset = 2;
int zeroState = 0;
long lastcheckedTime = 0;
long intervalBetweenZeroReset = 120000; // mS between resetting zero point - it does cause a brief pause
                                        // in compass movement as it needs to accelerate from zero

int targetPos = 360;
#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 9, 10, 11); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setup()
{
  
  Serial.begin(250000);
  Serial.println("Start up");
  pinMode(zeroPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1);
  delay(1000);
  digitalWrite(ledPin, 0);
  // Change these to suit your stepper if you want



  // Good baseline
  stepper.setMaxSpeed(200);
  stepper.setAcceleration(50);

  stepper.move(60);
  stepper.runToPosition();

  stepper.move(-40);
  stepper.runToPosition();

  zeroState = digitalRead(zeroPin);
  stepper.move(740);
  while ( zeroState == 1) {
    stepper.run();
    zeroState = digitalRead(zeroPin);
  }
  
  stepper.move(zeroOffset);
  digitalWrite(ledPin, 1);
  stepper.runToPosition();
  delay(3000);

  stepper.setCurrentPosition(0);

}

void loop()
{
  // If at the end of travel go to the other end
  if (stepper.distanceToGo() == 0) {
    targetPos = targetPos * -1;
    stepper.moveTo(-targetPos);
    stepper.run();
  }
  zeroState = digitalRead(zeroPin);


  if (zeroState == 0) {
    digitalWrite(ledPin, 1);
    if (millis() >= (lastcheckedTime + intervalBetweenZeroReset))
    {
      stepper.setCurrentPosition(zeroOffset);
      lastcheckedTime = millis();
      stepper.moveTo(-targetPos);
    }
  } else
    digitalWrite(ledPin, 0);

  stepper.run();
}
