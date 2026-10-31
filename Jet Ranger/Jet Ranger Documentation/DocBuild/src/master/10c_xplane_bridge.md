# X-Plane bridge (XPlaneBridge)

`XPlaneBridge` is the bridge for **X-Plane 11 and 12**. It reads **datarefs over X-Plane's built-in UDP interface** (no plugin), converts them with a **profile**, and sends them to the boards in the same `D,TOKEN:value` packets (UDP 13136) as the other bridges, so the board firmware does not change. It is a Windows Forms program (`net10.0-windows`) and is **output only**. Run only one bridge at a time.

It builds with no warnings and passes `tests\test_bridge.py` (a fake X-Plane answering the dataref subscriptions, 11 checks, with the generic profile and the Robinson R22 profile). It has **not been run against a real X-Plane yet**. The dataref names in the profiles were checked against X-Plane 12's `Resources\plugins\DataRefs.txt`.

## How it works

1. **Dataref link (`XPlaneLink.cs`).** One UDP socket on an ephemeral port. For every dataref the profile needs, the bridge sends an `RREF` request (`RREF` and a zero byte, an int32 frequency in Hz, an int32 index and a 400 byte dataref path; an array element is written `name[0]`) to X-Plane, by default `127.0.0.1:49000`. X-Plane answers to the request's source address and port with `RREF,` followed by 8 byte records of an int32 index and a float32 value. The requests are repeated every 5 seconds, at 10 Hz, so the bridge can start before X-Plane and survives X-Plane restarting. On exit the bridge cancels them (frequency 0).
2. **Finding X-Plane.** With no `--xplane-host` the bridge listens for X-Plane's beacon (multicast 239.255.1.1 port 49707) and uses the host and port it announces; with no beacon it keeps trying `127.0.0.1:49000`. Use `--xplane-host` and `--xplane-port` when X-Plane is on another PC or its receive port was changed. X-Plane must be allowed to receive UDP data (Settings, Network).
3. **Profiles.** `profiles\_common.json` holds the values nearly every aircraft gives through the standard datarefs and is added to every aircraft profile (the aircraft's own value for a token wins). On its own, as *Generic (common values, any aircraft)* in the Profile box, it drives any aircraft. X-Plane gives no reliable aircraft name over this interface, so the **profile is chosen by hand** (or with `--module <name>`). A value has a `token`, a `dataref` (or `datarefs` for the clock), an optional `divisor` dataref (a ratio, for torque), a `type` (`analog`, `lamp` or `hhmm`), `scale`, `offset`, `decimals`, `suffix`, `threshold`, `invert`, `hint` and `note`.
4. **Boards, power rules and cadence** are those of the DCS bridge: one packet per board through `Payloads.Build`, `BPWR` to the ADF and Upper board (.107), the 20 V and 24 V thresholds when a `MAINBUS` or `AVIONICSBUS` voltage is mapped (otherwise the aircraft counts as powered while data flows), an unpowered aircraft sends `AGL:0` to the Stepper, and nothing is sent for 3 seconds after X-Plane's data stops, so the boards' 30 second watchdogs return the gauges to zero.

## The common values

| Boards | Tokens and datarefs |
|---|---|
| Stepper | `IAS` (`airspeed_kts_pilot`), `VSI` (`vvi_fpm_pilot`), `AGL` (`radio_altimeter_height_ft_pilot`), `N1` (`N1_percent[0]`), `ITT` (`ITT_deg_C[0]`), `TQ` (`torque_n_mtr[0]` over `POINT_max_TRQ[0]`, as a percent), `FUEL` (`m_fuel_total` in kg times 0.3303, as US gallons of jet fuel) |
| Dual Stepper | `ALT` (`altitude_ft_pilot`), `RPME` (`N2_percent[0]`), `OILP`, `OILT` (`oil_pressure_psi[0]`, `oil_temperature_deg_C[0]`), `FUELLOAD` (`fuel_pressure_psi[0]`), `ZULU` (zulu hours and minutes), `TEMP` (outside air temperature with the C suffix) |
| Servo | `PITCH` (`pitch_AHARS_deg_pilot`), `BANK` (`roll_AHARS_deg_pilot`), and the lamps `RLOW`, `GENF`, `LOWF`, `EC`, `FPMP` and `EOUT` (engine 1 not running) from X-Plane's generic annunciators |
| Radio | Ten Huey Warning Panel LEDs from the nearest generic annunciator (approximations: X-Plane has no Huey caution panel) |
| GPS interface | `LAT`, `LON` (6 decimals), `ALT`, `IAS`, `HDG` (`true_psi`), and `UTC`: the simulator's date and time from `sim/time/zulu_time_sec` with `sim/cockpit2/clock_timer/zulu_month` and `zulu_day` (X-Plane has no year, so the PC's UTC year is used), sent as `UTC:yyyyMMddHHmmss.fff` so the Lowrance GPS follows the simulator's clock |

Some aircraft leave datarefs at 0, and that gauge then sits at zero; use **Search datarefs...** to find the right ones and put them in an aircraft profile.

## Aircraft profiles

- **Robinson R22** (`profiles\Robinson R22.json`): rotor speed from `prop_speed_rpm[0]` (530 rpm = 100 percent) and the 14 V bus doubled to `MAINBUS`, so the boards' 20 V and 24 V thresholds work.
- **Sikorsky S-76** (`profiles\Sikorsky S-76.json`): rotor speed with 293 rpm = 100 percent and a 28 V bus.

The rotor speeds and bus scales are first guesses: check them against the cockpit gauges.

## Window and command line

![XPlaneBridge against a fake X-Plane with the Robinson R22 profile (scroll for the Huey LEDs, rotor speed and bus voltage)](screenshots/xplane_bridge.png)

The window shows the X-Plane address and whether data is arriving, the **Profile** box, **Reload**, **Search datarefs...** (words matched against the names and descriptions in `DataRefs.txt`, best first, in the log), **Send to the boards**, a table of tokens with dataref, raw value, value sent and board, and a log. Options: `--board-host <ip>` (send every board packet there, for testing), `--xplane-host`, `--xplane-port`, `--datarefs <DataRefs.txt>` (default `C:\X-Plane 12\Resources\plugins\DataRefs.txt`), `--profile-dir`, `--module <name>`, `--no-send` and `--logfile`.

## Testing without X-Plane

`python tests\test_bridge.py` starts a fake X-Plane on UDP 49100, runs the bridge against it first with the generic profile and then with the Robinson R22 profile (switching the bus voltage off half way), and checks the packets on UDP 13136 and 27002. It stops only the processes it started.

## Known gaps

- Not yet run against a real X-Plane.
- No inputs (the switches already reach X-Plane through the USB HID board as a joystick), no radio commands on port 27001, port 27003 is not read, and the landing gear lamps are not sent.
- Only the Robinson R22 and Sikorsky S-76 have aircraft profiles; the Huey LEDs are approximations.
