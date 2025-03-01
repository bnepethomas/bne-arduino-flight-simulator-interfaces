@echo off
REM The COMPORTS variable should be set to a space-separated list of COM port numbers:

set COMPORTS=3 4  5 9 10 11 13 25 28 

for %%i in (%COMPORTS%) do start /b cmd /c "C:\Users\bnepe\Documents\GitHub\dcs-bios\Programs\connect-serial-port.cmd" /Q %%i