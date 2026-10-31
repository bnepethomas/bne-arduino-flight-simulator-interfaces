# Lowrance AirMap GPS

`LowranceAirMapGps` makes the PC the GPS for a **Lowrance AirMap 2000C**. It receives the aircraft position from the bridge and sends it to the GPS over a serial cable as NMEA sentences twice a second.

![From the simulator to the GPS](diagrams/gps_flow.svg)

## Setting it up

1. Connect the GPS to the PC with its serial cable (a USB serial adapter works).
2. Run `LowranceAirMapGps.exe`. The first time it asks which serial port the GPS is on; it remembers the answer (*Change serial port...* changes it later). The port runs at **115200 bps, 8 data bits, no parity, 1 stop bit**.
3. Start the simulator and one bridge on the same PC. The bridge sends the aircraft information to UDP port 27002 without any setting; if the GPS interface runs on a different PC, change `GpsHost` in the bridge's `Form1.cs`.
4. The window shows the serial port state, the number of packets received, where the position comes from, the values being sent and the time source. The sentences being sent are listed at the bottom.

## What the GPS is told

- **Position, altitude, airspeed and heading** from the simulator, sent as GGA, RMC, GLL, VTG, GSA and HDT sentences. NMEA has no airspeed sentence a GPS accepts, so the airspeed is sent as the GPS speed.
- **Date and time** are the simulator's UTC, ticking in real time between packets (with DCS the mission clock less the **Mission time zone** set in the DCS bridge). If the packets have no time, or nothing has arrived for 30 seconds, the PC clock is used. The window says which.
- **No data for 30 seconds** (the simulator is paused, closed or not running): the GPS is told it is at Brisbane Airport, at the start of the runway, speed 0. It returns to the live position as soon as packets arrive again.

![Live data and the Brisbane default](diagrams/gps_states.svg)

The default position and the UDP port can be changed in `%AppData%\LowranceAirMapGps\settings.json`. The Brisbane coordinates came from SkyVector and were not checked against the AIP.

## Testing without the simulator

`LowranceAirMapGpsBenchTester` sends the same packets from a window:

- Choose a **Country, City and Airport** to put the aircraft on the ground at that airport.
- Type the **latitude, longitude, altitude, speed and heading**, and press Enter after each.
- Use the **8-way cursor** (the buttons or the arrow keys) to move the position in steps (default 100 m); hold to repeat.
- Tick **Fly** to move along the heading at the current speed; **Time x** speeds it up (1 to 100).
- Set the **date and time (UTC)** the GPS should show, or press *PC clock*.

Use one sender at a time (a bridge or the tester): a second one just overwrites the position. The GPS interface has not yet been tested with the real AirMap 2000C, so check that the GPS shows the position and that it accepts the 115200 bps rate.
