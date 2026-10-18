# Colour codes and cabling

This chapter explains the colours used in the diagrams and programs, and the colour codes used for cables and connectors.

## Reading the diagrams

Each diagram box is shaded to say what kind of item it is. The same shades are used in every diagram.

| Colour | What it means in the diagrams |
|---|---|
| [[#FFFFFF]] White | An ordinary item: a board, program, step or detail |
| [[#E8F0FB]] Blue | The subject of the figure or the main step: the bridge, the Health Monitor, the code that decides |
| [[#E7F4EA]] Green | Running or healthy, power on, or an output: connected, the gauge or backlight being driven |
| [[#FFF3DC]] Amber | Waiting, a warning or a special path: no data, the default position, the USB HID path |
| [[#FDECEA]] Red | Lost, off, failed or a reset |
| [[#F1F1EE]] Gray | Supporting or inactive: notes, items that stay off |
| [[#F0EAFB]] Purple | Tells boards or groups apart where the figure says so (for example the Dual Stepper in the Swing ALL sequence, the Upper controller in the PCB map) |
| [[#E3F5F4]] Teal | The same, for the ADF controller in the PCB map |
| [[#2B2F36]] Dark | Column headers and the Ethernet switch |

Arrows follow the same idea: blue is data going to a board over UDP, green is commands or events coming back, amber is the USB HID path, gray is an ordinary link, and a dashed line is an optional or recovery path.

## Colours in the programs

| Where | Colour | Meaning |
|---|---|---|
| Health Monitor light | [[#2E9E4F]] Green | A heartbeat was heard within the last 15 seconds |
| Health Monitor light | [[#F28C00]] Orange | 15 seconds without a heartbeat (connection warning) |
| Health Monitor light | [[#D32F2F]] Red | 30 seconds without a heartbeat (connection lost) |
| Bridge status bar | [[#D32F2F]] Red | Disconnected from the simulator |
| Bridge status bar | [[#F28C00]] Orange | Connected but no data (the simulator is paused or loading) |
| Bridge status bar | [[#2E9E4F]] Green | Connected, with the aircraft name |
| Board status LEDs (pins 14 green, 15 red) | Green and red | Alternate every 200 ms while the sketch runs; red flashes fast while Ethernet starts |
| SwingTester | [[#F2C200]] Gold | The gauge row being swung, or a warning lamp that is on |
| GPS bench tester | [[#FFE4E1]] Pink | A box whose value could not be read (it is not used) |
| Upper controller emulator | [[#2E9E4F]] Green | A switch that is on |
| GPS interface | [[#2E9E4F]] Green | The position comes from aircraft data over UDP |
| GPS interface | [[#F28C00]] Orange | No data for 30 seconds: the Brisbane Airport default is being sent |

<!-- pagebreak -->

## Cabling colour codes

### Round power connectors

| Colour | Supply |
|---|---|
| [[#D32F2F]] Red | 12 V |
| [[#9E9E9E]] Gray | 5 V |

### PCB connectors

| Colour | Use | Example |
|---|---|---|
| [[#FFFFFF]] Uncoloured | Digital input | |
| [[#1E63D6]] Blue | Analog | |
| [[#D32F2F]] Red | 12 V | Magnetic coils |
| [[#2E9E4F]] Green | 12 V backlighting | |
| [[#F2C200]] Yellow | OLED | |
| [[#F28C00]] Orange | LEDs | |

The two 12 V colours are different on purpose: red is 12 V for coils, green is 12 V for backlighting.

## OLED connectors

![The two OLED connection types](diagrams/oled_connectors.svg)

The OLED connectors are **not the same** on every display:

- **Traditional OLED connection:** +5 V on pin 1 and GND on pin 2.
- **Directly connected SSD1306 OLEDs** (the Temperature and Time displays): GND on pin 1 and +3.3 V on pin 2.

The SD and SC (data and clock) pin positions of the directly connected displays have not been validated. Do not plug one type of cable into the other type of connector: pins 1 and 2 are reversed and the supply voltages differ.
