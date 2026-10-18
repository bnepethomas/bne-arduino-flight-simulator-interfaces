# LowranceAirMapGpsBenchTester — Program Summary

WinForms (.NET 10, `net10.0-windows`) bench tool for [`LowranceAirMapGps`](../LowranceAirMapGps/PROGRAM_SUMMARY.md).
It sends aircraft information over UDP twice a second so the GPS interface can be exercised without a simulator.
Build: `dotnet build -c Release` in this folder; the executable is
`bin\Release\net10.0-windows\LowranceAirMapGpsBenchTester.exe`.

## Window

- **Send to UDP** host and port (default `127.0.0.1:27002`, the GPS interface's port) and a *Send twice a second* check box.
- **Country, City and Airport** drop-down lists (start on Australia, Brisbane, YBBN). Choosing an airport puts the aircraft on the
  ground there (its reference point and field elevation, speed 0; the heading is left as it is). The lists hold 10,133 airports that
  have an ICAO code, in 236 countries, from the embedded `Data/airports.tsv`. Country names come from .NET's region names.
- **Latitude, longitude, altitude (ft), speed (kt) and heading (degrees true)** boxes. Type a value and press Enter; an
  unreadable value turns the box pink and is not used. The boxes follow the cursor while it moves.
- **8-way cursor:** a 3 by 3 pad of buttons (N, NE, E, SE, S, SW, W, NW) moves the position by the **Cursor step**
  (metres, default 100). A press moves one step at once; holding it repeats every 100 ms after 400 ms. The **arrow keys** do the
  same (two arrows together give a diagonal) while the window is active and no box is being edited.
- **Fly** check box: while ticked the position advances along the heading at the current speed (altitude, speed and heading stay
  constant). Distance is the real time since the last 100 ms tick times the speed (1 kt = 0.514444 m/s), multiplied by the **Time x**
  setting (1 to 100) so a long leg can be flown quickly on the bench. Nothing moves while the speed is 0. Edit the speed, heading
  or altitude while flying to change course, or use the cursor to nudge the position (with *Point heading* ticked the cursor also
  steers by setting the heading).
- **Point heading in direction of travel** (on by default) sets the heading to the cursor direction (0, 45, 90 ... 315).
- **Date / time (UTC)** box with a *PC clock* button and a *Send date/time* check box (on by default). Type a date and time as
  `yyyy-MM-dd HH:mm:ss` and press Enter; the clock then runs forward in real time from that value and is sent as the simulator time.
  (It does not follow the Time x setting.)
- **Back to the selected airport** puts the position back at the selected airport. At start-up the position is the GPS interface's
  Brisbane default (-27.3830, 153.1068, 13 ft, heading 27, the start of the runway) with Brisbane selected in the lists.
- The packet being sent is shown at the bottom.

## Packet

`D,LAT:-27.383000,LON:153.106800,ALT:13,IAS:0,HDG:27,UTC:20261018031205.250` — latitude and longitude in degrees (south and west negative),
altitude in feet, `IAS` airspeed in knots, heading in degrees true, `UTC` the date and time (left out when *Send date/time* is off).

## Cursor and flight movement

`GeoMove.MoveBearing()` is a great-circle destination-point calculation (earth radius 6,371,008.8 m), so a step or a leg is the
same distance on any bearing and stays accurate over long flights. Longitude wraps at plus and minus 180 degrees and latitude is
limited to plus and minus 90.

## Files

| File | Role |
|---|---|
| `MainForm.cs` | Window, 500 ms UDP send timer, 100 ms cursor timer, key polling (`GetAsyncKeyState`) |
| `AirportDb.cs` | Loads the embedded airport list and answers country / city / airport queries |
| `Data/airports.tsv` | Airport list trimmed from [OurAirports](https://ourairports.com/data/) `airports.csv` (public domain, downloaded 2026-10-18, 12.7 MB reduced to 0.7 MB): airports of type large, medium or small with a 4-letter ICAO code |
| `GeoMove.cs` | Compass directions, move-by-metres and move-along-a-bearing (used by Fly) and arrow-key to direction mapping |
| `Program.cs` | Entry point |

## Status

Built clean. Run together with `LowranceAirMapGps` on one PC: the GPS interface showed the Brisbane default, then the tester's
position; a North click moved latitude about 0.0009 degrees (100 m) and set heading 0, a North-East click moved both axes and
set heading 45, and holding the Left arrow moved the position west several steps with heading 270.

Airport selection was tested through the window: Australia / Brisbane / YBBN at start-up (position unchanged at the runway start),
then Japan (first airport Aguni, RORA) and Osaka (Kansai, RJBB) moved the GPS interface to the matching coordinates.
Airport positions are the OurAirports reference points, not runway thresholds, because the runway file was not downloaded.

Fly mode was tested live: speed 120 kt, heading 90, Time x10, about 4.5 s of flying moved the GPS interface's longitude by 0.0284
degrees (about 2.8 km, as expected) with latitude, altitude, speed and heading unchanged, and un-ticking Fly stopped the movement.

Date and time were tested live: with the tester clock set to 2027-03-05 12:30:00 the GPS interface's RMC sentence carried time 123001
and date 050327 and advanced with real time; with *Send date/time* off it returned to the PC clock.
