// Blocking.pde
// -*- mode: C++ -*-
//
// Shows how to use the blocking call runToNewPosition
// Which sets a new target position and then waits until the stepper has
// achieved it.
//
// Copyright (C) 2009 Mike McCauley
// $Id: Blocking.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 9, 10, 11); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setup()
{
  // Settings for simple pointer
  //  stepper.setMaxSpeed(3000.0);
  //  stepper.setAcceleration(3000.0);
  // Settings for Compass
  //  stepper.setMaxSpeed(3000.0);
  //  stepper.setAcceleration(500.0);
  stepper.setMaxSpeed(200);
  stepper.setAcceleration(50.0);
}

void loop()
{
  stepper.runToNewPosition(0);
  stepper.runToNewPosition(10);
  delay(1000);
  stepper.runToNewPosition(10);
  delay(1000);
  stepper.runToNewPosition(120);
  delay(1000);
  stepper.runToNewPosition(420);
  delay(1000);
  stepper.runToNewPosition(0);
  delay(4000);
}
