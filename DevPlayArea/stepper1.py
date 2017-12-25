#!/usr/bin/python
# Import required libraries
import sys
import time
import RPi.GPIO as GPIO
 
# Use BCM GPIO references
# instead of physical pin numbers
GPIO.setmode(GPIO.BCM)
 
# Define GPIO signals to use
# Physical pins 11,15,16,18
# GPIO17,GPIO22,GPIO23,GPIO24
StepPins = [17,22,23,24]
 
# Set all pins as output
for pin in StepPins:
  print "Setup pins"
  GPIO.setup(pin,GPIO.OUT)
  GPIO.output(pin, False)
 
# Define advanced sequence
# as shown in manufacturers datasheet
Seq = [[1,0,0,1],
       [0,0,0,1],
       [0,1,1,1],
       [0,1,1,0],
       [1,1,1,0],
       [1,0,0,1],
       [1,0,0,0]]
        
StepCount = len(Seq)
StepDir = 1 # Set to 1 or 2 for clockwise
            # Set to -1 or -2 for anti-clockwise
 
# Read wait time from command line
if len(sys.argv)>1:
  WaitTime = int(sys.argv[1])/float(1000)
else:
  WaitTime = 1/float(1000)

# Return to zero
StepDir = -1 # Set to 1 or 2 for clockwise
            # Set to -1 or -2 for anti-clockwise
# Initialise variables
StepCounter = 0
 
# Start return loop
print "Returning to 0"
for x in range(0, 1800):
  
  #print StepCounter,
  #print Seq[StepCounter]
 
  for pin in range(0,4):
    xpin=StepPins[pin]# Get GPIO
    if Seq[StepCounter][pin]!=0:
      #print " Enable GPIO %i" %(xpin)
      GPIO.output(xpin, True)
    else:
      GPIO.output(xpin, False)
 
  StepCounter += StepDir
 
  # If we reach the end of the sequence
  # start again
  if (StepCounter>=StepCount):
    StepCounter = 0
  if (StepCounter<0):
    StepCounter = StepCount+StepDir
 
  # Wait before moving on
  time.sleep(WaitTime)



# Read wait time from command line
if len(sys.argv)>1:
  WaitTime = int(sys.argv[1])/float(1000)
else:
  WaitTime = 2/float(1000)

for z in range(1,5):
  print z
  StepDir = 1 # Set to 1 or 2 for clockwise
            # Set to -1 or -2 for anti-clockwise
  # Initialise variables
  StepCounter = 0
   
  # Start main loop
  for x in range(0, 900):
   
    #print StepCounter,
    #print Seq[StepCounter]
   
    for pin in range(0,4):
      xpin=StepPins[pin]# Get GPIO
      if Seq[StepCounter][pin]!=0:
        #print " Enable GPIO %i" %(xpin)
        GPIO.output(xpin, True)
      else:
        GPIO.output(xpin, False)
   
    StepCounter += StepDir
   
    # If we reach the end of the sequence
    # start again
    if (StepCounter>=StepCount):
      StepCounter = 0
    if (StepCounter<0):
      StepCounter = StepCount+StepDir
   
    # Wait before moving on
    time.sleep(WaitTime)


  StepDir = -1 # Set to 1 or 2 for clockwise
              # Set to -1 or -2 for anti-clockwise
  # Initialise variables
  StepCounter = 0
   
  # Start return loop
  for x in range(0, 900):
    
    #print StepCounter,
    #print Seq[StepCounter]
   
    for pin in range(0,4):
      xpin=StepPins[pin]# Get GPIO
      if Seq[StepCounter][pin]!=0:
        #print " Enable GPIO %i" %(xpin)
        GPIO.output(xpin, True)
      else:
        GPIO.output(xpin, False)
   
    StepCounter += StepDir
   
    # If we reach the end of the sequence
    # start again
    if (StepCounter>=StepCount):
      StepCounter = 0
    if (StepCounter<0):
      StepCounter = StepCount+StepDir
   
    # Wait before moving on
    time.sleep(WaitTime)



for pin in range(0,4):
  xpin=StepPins[pin]# Get GPIO 
  GPIO.output(xpin, False)

