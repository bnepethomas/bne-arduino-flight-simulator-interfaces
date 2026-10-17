# LowranceAirMapGps — Program Summary

WinForms (.NET 10, `net10.0-windows`) program that makes a PC act as the GPS source for a
**Lowrance AirMap 2000C**. It sends NMEA 0183 sentences (latitude, longitude, altitude,
airspeed, heading) over a serial port every 500 ms. Aircraft information arrives over UDP; with no data for
30 seconds the position falls back to the start of the runway at Brisbane Airport.

Build: `dotnet build -c Release` in this folder; the executable is
`bin\Release\net10.0-windows\LowranceAirMapGps.exe`. Uses the `System.IO.Ports` NuGet package.

## Behaviour

1. **Serial port:** 115200 bps, 8 data bits, no parity, 1 stop bit, no handshake. The chosen port is saved in
   `%AppData%\LowranceAirMapGps\settings.json`. If none is saved (or the saved port no longer exists) a dialog asks
   the user to pick one at start-up; *Change serial port...* reopens it. If the port cannot be opened or a write
   fails the program retries every 3 seconds (so a USB adapter can be plugged in later).
2. **UDP input:** listens on UDP port **27002** (`UdpPort` in the settings file). Two packet layouts:
   - key/value: `D,LAT:-33.9461,LON:151.1772,ALT:1500,IAS:110,HDG:90` (any subset of keys; missing keys keep the previous value;
     aliases `LONG`/`LNG`, `ALTITUDE`, `AIRSPEED`/`SPD`/`SPEED`/`GS`, `HEADING`/`TRK`/`COURSE`);
   - CSV: `lat,lon,altitude_ft,airspeed_kt,heading_deg`.

   Units: degrees (south and west negative), feet, knots, degrees true. Packets that are not aircraft data (for
   example other `D,` packets on the network) or are out of range are ignored and do not reset the 30 second timer.
3. **Default position:** with no accepted packet for 30 seconds (or since start-up) the program sends Brisbane Airport:
   latitude -27.3830, longitude 153.1068, 13 ft, speed 0, heading 027 degrees true, which is the threshold of runway
   01L (016 magnetic). Override with `DefaultLatitude`, `DefaultLongitude`, `DefaultAltitudeFt` and `DefaultHeadingDeg` in the
   settings file. These coordinates come from SkyVector and were not checked against the AIP; check them before relying on them.
4. **Output rate:** a timer sends one set of sentences every 500 ms (two updates a second).

## NMEA sentences sent each update

`$GPGGA` (position, altitude in metres), `$GPRMC` (position, speed in knots, track, date), `$GPGLL`, `$GPVTG` (track, speed
in knots and km/h), `$GPGSA` (fixed 3D fix, 8 satellites) and `$GPHDT` (heading). NMEA has no airspeed sentence a GPS accepts, so
the airspeed is sent as the GPS ground speed in RMC and VTG, and the heading as the track and as HDT. Time and date are the
PC's UTC clock; fix quality is 1 with 8 satellites. Each sentence ends with its `*CS` checksum and CR LF.

## Files

| File | Role |
|---|---|
| `MainForm.cs` | Window, serial port handling, UDP listener task, 500 ms timer and the 30 second fallback |
| `NmeaBuilder.cs` | Builds the sentences and checksums |
| `AircraftData.cs` | `AircraftData` record and the UDP packet parser (`AircraftParser`) |
| `AppSettings.cs` | JSON settings (serial port, UDP port, default position) |
| `SerialPortDialog.cs` | The serial port picker |

## Status

Built clean. The NMEA builder (checksums checked against two reference sentences) and the parser passed a scratch test
program. The running window was exercised with UDP packets: it switched to the packet position, and 30 seconds after the
last packet went back to Brisbane Airport. **Not yet tested against a real GPS or serial port**, so the serial write path
and the AirMap 2000C's acceptance of these sentences and the 115200 bps rate are unverified.
