Hornet Altimeter

Originally tried building out of a single Arduino - but due to my lack of multithreading skills (OLED present heavy graphical workload), split into Raspberry pi and Arduino.

The Arduino remained in the project as needed analog input for the zero sensor on the stepper motor, the pi doesn't have a native analog input.  The OLED is being driven using the SPI bus for performance.

Both will receive data from ethernet.
Test rig has pot on Arduino sending altitude variations
Test rig will use 192.168.3.X network.

