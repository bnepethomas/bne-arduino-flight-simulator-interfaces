# ServoTuner — Program Summary

Standalone WinForms calibration tool used to find/verify each gauge
servo's min/max/zero pulse-position values on the Servo Controller board,
without needing a flight sim running. An operator picks a servo from a
list, either drags a scrollbar or types a raw/"converted" test value, and
the tool sends the equivalent `"D,<CODE>:<value>"` packet the sim-bridge
apps would normally generate.

## Program flow

1. **Constructor**: populates the servo list box from the `Servos` enum,
   selects `EngTorquePercent1` by default, calls `UpdateLabels()`, and
   connects `frontPanelClient` to the Servo Controller board.
2. **`lstServos_SelectedIndexChanged`** → `UpdateLabels()` shows the
   selected servo's short code and its configured min/max position.
3. **Scrollbar (`vScrollBar1_Scroll`)**: clamps the new value to the
   selected servo's min/max range (handling either min>max or min<max
   orientation), updates the label, and immediately sends
   `"D,<CODE>:<rawValue>"` to the board — i.e. drives the servo directly by
   raw position, bypassing any unit conversion.
4. **"Send Manual Value" (`SendManualValue`)**: parses `txtRawInput`, checks
   it's within range, and sends it the same way as the scrollbar.
5. **"Send Converted Value" (`SendConvertedValue`)**: parses
   `txtConvertedInput` as an instrument-unit value (knots, %, °, PSI, etc.),
   runs it through the matching `<CODE>_Process()` mapping function
   (identical formulas to the ones in `JET_RANGER_SERVO_CONTROLLER` and the
   three sim-bridge apps), and sends the resulting raw position — i.e. this
   button simulates what the sim bridge would compute for a given
   real-world instrument reading.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.102` (Servo Controller) | 13136 | Manual/converted `"D,<CODE>:<value>"` test packets |

## Programs this communicates with

- **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)**
  (`172.16.1.102:13136`) — the sole target; this tool is meant to be run
  *instead of* the sim-bridge apps (`P3D_to_UDP`/`SimConnect_to_UDP`/
  `MSFSSimConnectExtractor`) while calibrating, since both would otherwise
  fight over which value is currently driving each servo.
