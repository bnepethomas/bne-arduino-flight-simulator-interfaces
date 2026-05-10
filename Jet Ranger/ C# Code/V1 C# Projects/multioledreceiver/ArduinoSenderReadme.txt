Arduino Sender

A simple interface that drives multiple OLED displays off a single Arduino. The formating of strigs is performed on the host PC, leaving the Arduino to act as a Serial to I2C bridge/mux.  As the OLED displays have the same target I2C address on the I2C bus the system must either have a bunch of Arduinos (which means more USB ports and space consumed in the pit), or a muxing mechanism is added to software select the display to send data.

Principles:
Keep the Arduino code real simple, basically it receives a 5 part message
1. SOM (Start of Message)
2. Display Ptr (which display is the text going to)
3. Line Ptr (the original displays had two lines, so select which line the text is destined for)
4. Text (including trailing spaces to overwrite all previous displayed text)
5. EOM

The I2C bus is unidirectional ,i.e. the displays are never queried, it is assumed the data was received.


Start-up

On start the Arduino initialises all OLED displays, raising all Output 

Hardware:
1. Currently a 4081 Quad AND gate is used.  For some reason the I2C Clock (A4 pin) cannot be buffered via a AND gate.  So there may be issues with Bus loadings as displays are added. I also could be due to not having a pull up resistor on the input to the gate
2. The initial design had the LEDs tied to the +5V rail, this means they are lit when the output is low.
3. Output pins D0 and D1 are used for internal purposes, so the first pin used is D2.


Issues:
1. When moving to a second line noise is seen along the baseof the first line of text. It it occurs when either the command 0x02 or 0xC0 is send
2. Not an issue as such but increase speed of serial port to 115200 over 9600 to minimise chance of overruns. This supported the display of time messages in a long format at 1mS update with only occasional overruns. So best to keep updates at intervals of greater than 30mS.  This should be an issue as only when changes have occured will data be send to the Arduino.

