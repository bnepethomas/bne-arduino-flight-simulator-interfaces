@echo off
echo Testing PC
ping 172.16.1.10 -n 2
echo .
echo Left Inputs
ping 172.16.1.100 -n 2
echo .
echo Forward Inputs
ping 172.16.1.101 -n 2
echo .
echo Right Inputs
ping 172.16.1.103 -n 2
echo .
echo Mega Pixel Leds
ping 172.16.1.105 -n 2
echo .
echo Mega Max7219, Nextron IFEI Display
ping 172.16.1.106 -n 2
echo .
echo DUE Keyboard Sender
ping 172.16.1.110 -n 2
echo .
echo Mega Stepper, Servo, Digital Out
ping 172.16.1.111 -n 2
echo .
echo Pi GPS
ping 172.16.1.112 -n 2
echo.
echo Standby Instruments
ping 172.16.1.114 -n 2
pause