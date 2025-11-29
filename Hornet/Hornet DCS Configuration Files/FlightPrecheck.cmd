@echo off
echo Testing PC
ping 172.16.1.10 -n 2
echo .
echo Upper Inputs
ping 172.16.1.101 -n 2
echo .
echo Left Inputs
ping 172.16.1.109 -n 2
echo .
echo Right Inputs
ping 172.16.1.103 -n 2
echo .
echo Pixel Leds
ping 172.16.1.105 -n 2
echo .
echo Mega Max7219
ping 172.16.1.106 -n 2
echo .
echo Guages Instruments
ping 172.16.1.108 -n 2
echo .
echo Leonardo Keyboard Sender
ping 172.16.1.110 -n 2
echo .
echo Pi GPS
ping 172.16.1.112 -n 2
echo.
echo Standby Instruments
ping 172.16.1.114 -n 2
echo .
echo LIP Controller
ping 172.16.1.115 -n 2
pause