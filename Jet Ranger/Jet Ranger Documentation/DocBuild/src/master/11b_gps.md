# Lowrance AirMap GPS interface (LowranceAirMapGps)

`LowranceAirMapGps` makes the PC act as the GPS for a **Lowrance AirMap 2000C**. It is a Windows Forms program (.NET 10) that receives aircraft information over UDP and sends NMEA 0183 sentences with the position, altitude, airspeed, heading and time over a serial port, twice a second.

![From the simulator to the GPS](diagrams/gps_flow.svg)

## Behaviour

1. **Serial port:** 115200 bps, 8 data bits, no parity, 1 stop bit, no handshake. The chosen port is saved in `%AppData%\LowranceAirMapGps\settings.json`. If none is saved (or the saved port no longer exists) a dialog asks for one at start-up, and *Change serial port...* reopens it. If the port cannot be opened or a write fails the program retries every 3 seconds, so a USB serial adapter can be plugged in later.
2. **UDP input:** UDP port **27002** (`UdpPort` in the settings file). Two layouts are accepted: key/value `D,LAT:-33.9461,LON:151.1772,ALT:1500,IAS:110,HDG:90,UTC:20261018031205.250` (any subset of keys; missing keys keep the previous value) and CSV `lat,lon,altitude_ft,airspeed_kt,heading_deg[,utc]`. Units are degrees (south and west negative), feet, knots and degrees true. Packets that are not aircraft data or are out of range are ignored and do not reset the 30 second timer.
3. **Default position:** with no accepted packet for 30 seconds, or since start-up, the program sends Brisbane Airport: -27.3830, 153.1068, 13 ft, speed 0, heading 027 degrees true, the threshold of runway 01L (016 magnetic). The values can be changed in the settings file (`DefaultLatitude`, `DefaultLongitude`, `DefaultAltitudeFt`, `DefaultHeadingDeg`). They come from SkyVector and were not checked against the AIP.
4. **Output rate:** one set of sentences every 500 ms.

![Live data and the Brisbane default](diagrams/gps_states.svg)

## NMEA sentences

Each update sends `$GPGGA` (position, altitude in metres), `$GPRMC` (position, speed in knots, track, date), `$GPGLL`, `$GPVTG` (track, speed in knots and km/h), `$GPGSA` (a fixed 3D fix with 8 satellites) and `$GPHDT` (heading), each with its `*CS` checksum and CR LF. NMEA has no airspeed sentence a GPS accepts, so the airspeed is sent as the GPS speed in RMC and VTG, and the heading as the track and as HDT.

## GPS date and time

When a packet carries `UTC:` the program takes the offset between the simulator's UTC and the PC's UTC and adds it to the PC clock for every sentence, so the GPS time and date (the RMC date field and every sentence time) are the simulator's and tick smoothly between packets. The offset is only re-taken when the simulator time moves more than 1.5 seconds from where it predicts, for example when the user changes the time or the simulation rate is not 1x. The window shows *GPS time ... (simulator time)* or *(PC clock)*. The PC clock is used when no packet has arrived for 30 seconds or when a packet has no `UTC:`.

The FSUIPC, MSFS, DCS and X-Plane bridges all send `UTC:`. The DCS bridge's clock is the mission's local time, so it has a time zone box (see the DCS bridge chapter); the X-Plane bridge uses the simulator's zulu clock with the PC's year.

## Files

| File | Role |
|---|---|
| `MainForm.cs` | Window, serial port handling, UDP listener task, 500 ms timer and the 30 second fallback |
| `NmeaBuilder.cs` | Builds the sentences and checksums |
| `AircraftData.cs` | The `AircraftData` record and the UDP packet parser |
| `AppSettings.cs` | JSON settings: serial port, UDP port, default position |
| `SerialPortDialog.cs` | The serial port picker |

## Status

Built clean. The NMEA builder (checksums checked against two reference sentences) and the parser passed a scratch test program, and the running window was exercised with UDP packets from the bench tester: it switched to the packet position and time, and went back to Brisbane Airport 30 seconds after the last packet. **Not yet tested against a real GPS**, so the AirMap 2000C's acceptance of these sentences and of the 115200 bps rate is unverified. Run one sender at a time: a second sender's packets simply overwrite the position.

# GPS bench tester (LowranceAirMapGpsBenchTester)

A Windows Forms bench tool that sends aircraft information to the GPS interface (default `127.0.0.1:27002`, twice a second) so the GPS can be exercised without a simulator.

## Window

- **Send to UDP** host and port, and a *Send twice a second* check box.
- **Country, City and Airport** drop-down lists (starting on Australia, Brisbane, YBBN). Choosing an airport puts the aircraft on the ground there (the airport's reference point and field elevation, speed 0; the heading is left as it is). The lists hold 10,133 airports with an ICAO code in 236 countries, from an embedded list trimmed from the public-domain OurAirports `airports.csv`. The positions are reference points, not runway thresholds.
- **Latitude, longitude, altitude (ft), speed (kt) and heading (degrees true)** boxes: type a value and press Enter (an unreadable value turns the box pink and is not used). They follow the cursor while it moves.
- **8-way cursor:** a 3 by 3 pad of buttons (N, NE, E, SE, S, SW, W, NW) moves the position by the **Cursor step** (metres, default 100). A press moves one step; holding repeats every 100 ms after 400 ms. The arrow keys do the same, two together giving a diagonal, while the window is active and no box is being edited. With *Point heading in direction of travel* ticked (the default) the cursor also sets the heading.
- **Fly:** while ticked the position advances along the heading at the current speed with altitude, speed and heading constant. Distance is the real time since the last 100 ms tick times the speed (1 kt = 0.514444 m/s), multiplied by **Time x** (1 to 100) so a long leg can be flown quickly. Nothing moves while the speed is 0; change the speed, heading or altitude while flying to change course.
- **Date / time (UTC)** with a *PC clock* button and a *Send date/time* check box (on by default). Type `yyyy-MM-dd HH:mm:ss` and press Enter; the clock then runs in real time from that value and is sent as the simulator time.
- **Back to the selected airport** returns to the selected airport.

Movement uses a great-circle destination-point calculation, so a step or a leg is the same distance on any bearing and stays accurate over long flights.

## Testing

Run together with the GPS interface on one PC. The GPS interface showed the tester's position; a North click moved latitude about 0.0009 degrees (100 m) and set heading 0, holding the Left arrow moved the position west with heading 270, choosing Japan and then Osaka moved it to Kansai (RJBB), Fly at 120 kt, heading 90 and Time x10 moved the longitude about 2.8 km in 4.5 seconds, and with the clock set to 2027-03-05 12:30:00 the GPS interface's RMC sentence carried time 123001 and date 050327 and advanced in real time.
