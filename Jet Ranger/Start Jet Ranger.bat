@echo off
rem Double-click to start the Jet Ranger programs (asks which simulator). Options pass through, for example:  "Start Jet Ranger.bat" -Simulator DCS -DcsTimeZone 4
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Start Jet Ranger.ps1" %*
pause
