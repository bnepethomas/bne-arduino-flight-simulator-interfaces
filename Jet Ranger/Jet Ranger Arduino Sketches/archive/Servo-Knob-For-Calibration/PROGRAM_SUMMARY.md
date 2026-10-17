# Servo-Knob-For-Calibration — Program Summary

Small standalone bench-test sketch (based on the classic Arduino "Knob"
example) used to manually drive a single servo with a potentiometer while
working out the min/max pulse values for a gauge — it is not part of the
networked Jet Ranger panel fleet and has no Ethernet code at all.

## Program flow

1. **Setup**: starts Serial at 9600 baud and attaches one `Servo` object to
   pin 9.
2. **Main loop** (`loop()`): reads the potentiometer on `A0`, maps its
   0–1023 range to a 0–180° servo angle (inverted: 1023→0°, 0→180°),
   writes it to the servo, and prints the raw pot value to Serial whenever
   it changes.
3. A large block of unused helper functions (`EngineTorque()`,
   `TransmissionPressure()`, `EGT()`, `Airspeed()`, `VSI()`, etc.) is
   defined below `loop()`. These reproduce the min/max servo-position
   mapping tables also found in `JET_RANGER_SERVO_CONTROLLER`,
   `ServoTuner`, and the various SimConnect-to-UDP bridge apps, but are
   never called from anywhere in this sketch — they exist purely as
   reference/scratch code for deriving those calibration constants by hand.

## Pin usage

| Pin(s) | Function |
|---|---|
| A0 | Potentiometer input (`potpin`) driving the servo position |
| 9 | Servo signal output (`myservo`) |

## Local network configuration / IP addresses

None — this sketch does not include the Ethernet/SPI libraries and performs
no network communication of any kind.

## C# programs this sketch communicates with

None. It's a serial-only bench-calibration tool; it doesn't talk to any of
the Jet Ranger C# WinForms applications. Its unused helper functions encode
the same value ranges as **ServoTuner** and the SimConnect-to-UDP bridge
apps (`P3D_to_UDP` / `SimConnect_to_UDP` / `MSFSSimConnectExtractor`), which
is presumably where those constants were later ported to.
