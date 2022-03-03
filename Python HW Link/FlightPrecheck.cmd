@echo off
echo Testing PC
ping 172.16.1.10 -n 2
echo Left Inputs
ping 172.16.1.100 -n 2
echo Forward Inputs
ping 172.16.1.101 -n 2
echo Right Inputs
ping 172.16.1.103 -n 2
echo Mega Pixel Leds
ping 172.16.1.105 -n 2
echo Max7219, Nextron IFEI Display
ping 172.16.1.106 -n 2
echo DUE Stepper, Servo, Digital Out
ping 172.16.1.110 -n 2
pause