EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 900  600  0    50   ~ 0
\n
$Comp
L Connector:Conn_01x08_Female J1
U 1 1 5FE0AB8F
P 1100 1450
F 0 "J1" H 1300 1400 50  0000 C CNN
F 1 "GND POWER " H 1050 1850 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 1100 1450 50  0001 C CNN
F 3 "~" H 1100 1450 50  0001 C CNN
	1    1100 1450
	-1   0    0    -1  
$EndComp
Wire Notes Line
	600  800  600  2150
Wire Notes Line
	600  2150 1700 2150
Wire Notes Line
	1700 2150 1700 800 
Wire Notes Line
	1700 800  600  800 
Text Notes 950  900  0    50   ~ 0
GND POWER
$Comp
L Connector:Conn_01x04_Female J2
U 1 1 5FE1082E
P 1100 2750
F 0 "J2" H 1000 3100 50  0000 C CNN
F 1 "EXT LIGHTS ANALOG" H 1000 3000 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1100 2750 50  0001 C CNN
F 3 "~" H 1100 2750 50  0001 C CNN
	1    1100 2750
	-1   0    0    -1  
$EndComp
Text Notes 1100 2350 0    50   ~ 0
EXT LIGHTS
Text Label 1300 2650 0    50   ~ 0
ANALOG_5V
Text Label 1300 2750 0    50   ~ 0
ANALOG_GND
Text Label 1300 2850 0    50   ~ 0
POSITION_DIMMER
Text Label 1300 2950 0    50   ~ 0
FORMATION_DIMMER
$Comp
L Connector:Conn_01x04_Female J3
U 1 1 5FE1268F
P 1100 3350
F 0 "J3" H 1250 3350 50  0000 C CNN
F 1 "EXT LIGHTS" H 1000 3550 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1100 3350 50  0001 C CNN
F 3 "~" H 1100 3350 50  0001 C CNN
	1    1100 3350
	-1   0    0    -1  
$EndComp
Wire Notes Line
	700  2250 700  3800
Wire Notes Line
	700  3800 2100 3800
Wire Notes Line
	2100 3800 2100 2250
Wire Notes Line
	2100 2250 700  2250
Text Notes 1000 4150 0    50   ~ 0
FLIGHT CONTROL SYSTEMS
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 5FE1638F
P 1300 5050
F 0 "J5" H 1400 5050 50  0000 C CNN
F 1 "FCS" H 1200 5300 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1300 5050 50  0001 C CNN
F 3 "~" H 1300 5050 50  0001 C CNN
	1    1300 5050
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J4
U 1 1 5FE18797
P 1300 4600
F 0 "J4" H 1400 4600 50  0000 C CNN
F 1 "FCS ANALOG" H 1200 4850 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 1300 4600 50  0001 C CNN
F 3 "~" H 1300 4600 50  0001 C CNN
	1    1300 4600
	-1   0    0    -1  
$EndComp
Text Label 1500 4500 0    50   ~ 0
ANALOG_5V
Text Label 1500 4600 0    50   ~ 0
RUD_TRIM
Text Label 1500 4700 0    50   ~ 0
ANALOG_GND
Wire Notes Line
	600  4000 600  5500
Wire Notes Line
	600  5500 2200 5500
Wire Notes Line
	2200 5500 2200 4000
Wire Notes Line
	2200 4000 600  4000
$Comp
L Connector:Conn_01x10_Female J7
U 1 1 5FDEA1B4
P 2600 1600
F 0 "J7" H 2500 2250 50  0000 C CNN
F 1 "COM ANALOG" H 2450 2150 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 2600 1600 50  0001 C CNN
F 3 "~" H 2600 1600 50  0001 C CNN
	1    2600 1600
	-1   0    0    -1  
$EndComp
Text Label 2800 1200 0    50   ~ 0
ANALOG_5V
Text Label 2800 1300 0    50   ~ 0
ANALOG_GND
Text Label 2800 1400 0    50   ~ 0
COM_VOX
Text Label 2800 1500 0    50   ~ 0
COM_ICS
Text Label 2800 1600 0    50   ~ 0
COM_RWR
Text Label 2800 1700 0    50   ~ 0
COM_MIDS_A
Text Label 2800 1800 0    50   ~ 0
COM_MIDS_B
Text Label 2800 1900 0    50   ~ 0
COM_TACAN
Text Label 2800 2000 0    50   ~ 0
COM_AUX
Text Label 2800 2100 0    50   ~ 0
COM_WPN
$Comp
L Connector:Conn_01x10_Female J9
U 1 1 5FDF1152
P 2600 3350
F 0 "J9" H 2492 3935 50  0000 C CNN
F 1 "COM ROWS" H 2492 3844 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 2600 3350 50  0001 C CNN
F 3 "~" H 2600 3350 50  0001 C CNN
	1    2600 3350
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J8
U 1 1 5FDF3E63
P 2600 2500
F 0 "J8" H 2492 2785 50  0000 C CNN
F 1 "COM COLUMNS" H 2492 2694 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 2600 2500 50  0001 C CNN
F 3 "~" H 2600 2500 50  0001 C CNN
	1    2600 2500
	-1   0    0    -1  
$EndComp
Text Notes 2600 900  0    50   ~ 0
COM
Wire Notes Line
	2300 800  2300 3950
Wire Notes Line
	2300 3950 3300 3950
Wire Notes Line
	3300 3950 3300 800 
Wire Notes Line
	3300 800  2300 800 
$Comp
L Connector:Conn_01x03_Female J10
U 1 1 5FDE7EBB
P 2700 4550
F 0 "J10" H 2800 4550 50  0000 C CNN
F 1 "LOX ANALOG" H 2600 4800 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 2700 4550 50  0001 C CNN
F 3 "~" H 2700 4550 50  0001 C CNN
	1    2700 4550
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J11
U 1 1 5FDE8E05
P 2700 5100
F 0 "J11" H 2800 5100 50  0000 C CNN
F 1 "LOX " H 2550 5350 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 2700 5100 50  0001 C CNN
F 3 "~" H 2700 5100 50  0001 C CNN
	1    2700 5100
	-1   0    0    -1  
$EndComp
Text Label 2900 4450 0    50   ~ 0
ANALOG_5V
Text Label 2900 4650 0    50   ~ 0
ANALOG_GND
Text Label 2900 4550 0    50   ~ 0
OXY_FLOW
Text Label 2900 5000 0    50   ~ 0
COL10
Text Label 2900 5100 0    50   ~ 0
ROW4
Text Notes 2700 4200 0    50   ~ 0
LOX
Wire Notes Line
	2350 4000 2350 5500
Wire Notes Line
	2350 5500 3400 5500
Wire Notes Line
	3400 5500 3400 4000
Wire Notes Line
	3400 4000 2350 4000
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 5FDEB4FC
P 1300 6300
F 0 "J6" H 1400 6300 50  0000 C CNN
F 1 "MISSION COMPUTER" H 1200 6600 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1300 6300 50  0001 C CNN
F 3 "~" H 1300 6300 50  0001 C CNN
	1    1300 6300
	-1   0    0    -1  
$EndComp
Text Notes 1050 5900 0    50   ~ 0
MISSION COMPUTER
Wire Notes Line
	650  6800 2150 6800
Wire Notes Line
	2150 6800 2150 5700
Wire Notes Line
	2150 5700 650  5700
Wire Notes Line
	650  5700 650  6800
$Comp
L Connector:Conn_01x04_Female J12
U 1 1 5FDED7E7
P 3050 6300
F 0 "J12" H 3150 6300 50  0000 C CNN
F 1 "ANTENNA SEL" H 2950 6600 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 3050 6300 50  0001 C CNN
F 3 "~" H 3050 6300 50  0001 C CNN
	1    3050 6300
	-1   0    0    -1  
$EndComp
Text Notes 2800 5900 0    50   ~ 0
ANTENNA SEL
Wire Notes Line
	2400 6800 3900 6800
Wire Notes Line
	3900 6800 3900 5700
Wire Notes Line
	3900 5700 2400 5700
Wire Notes Line
	2400 5700 2400 6800
Text Notes 3850 950  0    50   ~ 0
APU
$Comp
L Connector:Conn_01x04_Female J13
U 1 1 5FDEF5D2
P 3800 1400
F 0 "J13" H 3950 1300 50  0000 C CNN
F 1 "APU" H 3700 1600 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 3800 1400 50  0001 C CNN
F 3 "~" H 3800 1400 50  0001 C CNN
	1    3800 1400
	-1   0    0    -1  
$EndComp
Wire Notes Line
	3450 800  3450 1900
Wire Notes Line
	3450 1900 4350 1900
Wire Notes Line
	4350 1900 4350 800 
Wire Notes Line
	4350 800  3450 800 
$Comp
L Connector:Conn_01x06_Female J14
U 1 1 5FDF3E24
P 3900 2550
F 0 "J14" H 4000 2550 50  0000 C CNN
F 1 "ESS BREAKER" H 3800 2900 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 3900 2550 50  0001 C CNN
F 3 "~" H 3900 2550 50  0001 C CNN
	1    3900 2550
	-1   0    0    -1  
$EndComp
Text Notes 3650 2150 0    50   ~ 0
ESSENTIAL BREAKER
Wire Notes Line
	3400 2000 3400 3250
Wire Notes Line
	3400 3250 4650 3250
Wire Notes Line
	4650 3250 4650 2000
Wire Notes Line
	4650 2000 3400 2000
Text Notes 3850 3600 0    50   ~ 0
EJECTION SEAT
$Comp
L Connector:Conn_01x06_Female J15
U 1 1 5FDF65F7
P 3950 4100
F 0 "J15" H 4050 4050 50  0000 C CNN
F 1 "EJECTION SEAT" H 3850 4400 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 3950 4100 50  0001 C CNN
F 3 "~" H 3950 4100 50  0001 C CNN
	1    3950 4100
	-1   0    0    -1  
$EndComp
Wire Notes Line
	3450 3400 4650 3400
Wire Notes Line
	4650 3400 4650 4700
Wire Notes Line
	4650 4700 3450 4700
Wire Notes Line
	3450 4700 3450 3400
$Comp
L Connector:Conn_01x06_Female J16
U 1 1 5FDF8986
P 4400 5500
F 0 "J16" H 4550 5450 50  0000 C CNN
F 1 "FUEL" H 4300 5800 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 4400 5500 50  0001 C CNN
F 3 "~" H 4400 5500 50  0001 C CNN
	1    4400 5500
	-1   0    0    -1  
$EndComp
Text Notes 4400 5000 0    50   ~ 0
FUEL
Wire Notes Line
	4050 4850 4050 6150
Wire Notes Line
	4050 6150 4950 6150
Wire Notes Line
	4950 6150 4950 4850
Wire Notes Line
	4950 4850 4050 4850
Text Notes 5150 950  0    50   ~ 0
SELECTIVE JET
$Comp
L Connector:Conn_01x08_Female J17
U 1 1 5FE1953B
P 5300 1550
F 0 "J17" H 5450 1400 50  0000 C CNN
F 1 "SEL JET" H 5200 2000 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 5300 1550 50  0001 C CNN
F 3 "~" H 5300 1550 50  0001 C CNN
	1    5300 1550
	-1   0    0    -1  
$EndComp
Wire Notes Line
	4850 800  4850 2700
Wire Notes Line
	4850 2700 5950 2700
Wire Notes Line
	5950 2700 5950 800 
Wire Notes Line
	5950 800  4850 800 
Text Notes 5200 3000 0    50   ~ 0
Landing Gear
$Comp
L Connector:Conn_01x04_Female J19
U 1 1 5FDF7415
P 5350 3450
F 0 "J19" H 5500 3350 50  0000 C CNN
F 1 "LANDING GEAR" H 5250 3700 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 5350 3450 50  0001 C CNN
F 3 "~" H 5350 3450 50  0001 C CNN
	1    5350 3450
	-1   0    0    -1  
$EndComp
Wire Notes Line
	4850 2850 4850 4150
Wire Notes Line
	4850 4150 6150 4150
Wire Notes Line
	6150 4150 6150 2850
Wire Notes Line
	6150 2850 4850 2850
Text Label 2800 2400 0    50   ~ 0
COL0
Text Label 2800 2500 0    50   ~ 0
COL1
Text Label 2800 2600 0    50   ~ 0
COL2
Text Label 2800 2950 0    50   ~ 0
ROW0
Text Label 2800 3050 0    50   ~ 0
ROW1
Text Label 2800 3150 0    50   ~ 0
ROW2
Text Label 2800 3250 0    50   ~ 0
ROW3
Text Label 2800 3350 0    50   ~ 0
ROW4
Text Label 2800 3450 0    50   ~ 0
ROW5
Text Label 2800 3550 0    50   ~ 0
ROW6
Text Label 2800 3650 0    50   ~ 0
ROW7
Text Label 2800 3750 0    50   ~ 0
ROW8
Text Label 2800 3850 0    50   ~ 0
ROW9
Text Label 1300 1150 0    50   ~ 0
COL3
Text Label 1300 1250 0    50   ~ 0
COL4
Text Label 1300 1350 0    50   ~ 0
COL5
Text Label 1300 1450 0    50   ~ 0
COL6
Text Label 1300 1550 0    50   ~ 0
COL7
Text Label 1300 1650 0    50   ~ 0
COL8
Text Label 1300 1750 0    50   ~ 0
ROW0
Text Label 1300 1850 0    50   ~ 0
ROW1
Text Label 1300 3250 0    50   ~ 0
COL9
Text Label 1300 3350 0    50   ~ 0
COL10
Text Label 1300 3450 0    50   ~ 0
ROW0
Text Label 1300 3550 0    50   ~ 0
ROW1
Text Label 5500 1250 0    50   ~ 0
COL3
Text Label 5500 1350 0    50   ~ 0
COL4
Text Label 5500 1450 0    50   ~ 0
COL5
Text Label 5500 1550 0    50   ~ 0
ROW2
Text Label 5500 1650 0    50   ~ 0
ROW3
Text Label 5500 1750 0    50   ~ 0
ROW4
Text Label 5500 1850 0    50   ~ 0
ROW5
Text Label 5500 1950 0    50   ~ 0
ROW6
Text Label 4150 3900 0    50   ~ 0
COL3
Text Label 4150 4000 0    50   ~ 0
COL4
Text Label 4150 4100 0    50   ~ 0
ROW7
Text Label 4150 4200 0    50   ~ 0
ROW8
Text Label 4150 4300 0    50   ~ 0
ROW9
Text Label 4600 5300 0    50   ~ 0
COL0
Text Label 4600 5400 0    50   ~ 0
COL1
Text Label 4600 5500 0    50   ~ 0
COL2
Text Label 4600 5600 0    50   ~ 0
COL3
Text Label 5550 3350 0    50   ~ 0
COL4
Text Label 5550 3450 0    50   ~ 0
COL5
Text Label 5550 3550 0    50   ~ 0
ROW10
Text Label 5550 3650 0    50   ~ 0
ROW11
Text Label 3250 6200 0    50   ~ 0
COL0
Text Label 3250 6300 0    50   ~ 0
COL1
Text Label 3250 6400 0    50   ~ 0
ROW12
Text Label 3250 6500 0    50   ~ 0
ROW13
Text Label 4100 2350 0    50   ~ 0
COL6
Text Label 4100 2450 0    50   ~ 0
COL7
Text Label 4100 2550 0    50   ~ 0
COL8
Text Label 4100 2650 0    50   ~ 0
ROW2
Text Label 4100 2750 0    50   ~ 0
ROW3
Text Label 1500 4950 0    50   ~ 0
COL9
Text Label 1500 5050 0    50   ~ 0
COL10
Text Label 1500 5150 0    50   ~ 0
ROW2
Text Label 1500 5250 0    50   ~ 0
ROW3
Text Label 4000 1300 0    50   ~ 0
COL6
Text Label 4000 1400 0    50   ~ 0
COL7
Text Label 4000 1500 0    50   ~ 0
ROW4
Text Label 4000 1600 0    50   ~ 0
ROW5
Text Label 1500 6200 0    50   ~ 0
COL8
Text Label 1500 6300 0    50   ~ 0
COL9
Text Label 1500 6400 0    50   ~ 0
ROW4
Text Label 1500 6500 0    50   ~ 0
ROW5
$Comp
L Connector:Conn_01x08_Female J22
U 1 1 5FE42075
P 7350 4950
F 0 "J22" H 7378 4926 50  0000 L CNN
F 1 "ANALOG A" H 7378 4835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 7350 4950 50  0001 C CNN
F 3 "~" H 7350 4950 50  0001 C CNN
	1    7350 4950
	1    0    0    -1  
$EndComp
Text Label 7150 4650 2    50   ~ 0
ANALOG_5V
Text Label 7150 4750 2    50   ~ 0
ANALOG_GND
Text Label 7150 4850 2    50   ~ 0
COM_VOX
Text Label 7150 4950 2    50   ~ 0
COM_ICS
Text Label 7150 5050 2    50   ~ 0
COM_RWR
Text Label 7150 5150 2    50   ~ 0
COM_MIDS_A
Text Label 7150 5250 2    50   ~ 0
COM_MIDS_B
Text Label 7150 5350 2    50   ~ 0
COM_TACAN
Text Label 7650 5750 2    50   ~ 0
ANALOG_5V
Text Label 6500 5850 2    50   ~ 0
ANALOG_GND
Text Label 7650 5950 2    50   ~ 0
COM_AUX
Text Label 7650 6050 2    50   ~ 0
COM_WPN
Text Label 7650 6150 2    50   ~ 0
RUD_TRIM
Text Label 7650 6250 2    50   ~ 0
POSITION_DIMMER
Text Label 6500 5950 2    50   ~ 0
FORMATION_DIMMER
Text Label 6500 6050 2    50   ~ 0
OXY_FLOW
$Comp
L Connector:Conn_01x34_Female J21
U 1 1 5FDEE968
P 7150 2450
F 0 "J21" H 7178 2426 50  0000 L CNN
F 1 "DIGITAL_IN" H 7178 2335 50  0000 L CNN
F 2 "Connector_IDC:IDC-Header_2x17_P2.54mm_Vertical" H 7150 2450 50  0001 C CNN
F 3 "~" H 7150 2450 50  0001 C CNN
	1    7150 2450
	1    0    0    -1  
$EndComp
Text Label 6950 950  2    50   ~ 0
ROW0
Text Label 6950 1050 2    50   ~ 0
ROW1
Text Label 6950 1150 2    50   ~ 0
ROW2
Text Label 6950 1250 2    50   ~ 0
ROW3
Text Label 6950 1350 2    50   ~ 0
ROW4
Text Label 6950 1450 2    50   ~ 0
ROW5
Text Label 6950 1550 2    50   ~ 0
ROW6
Text Label 6950 1650 2    50   ~ 0
ROW7
Text Label 6950 1750 2    50   ~ 0
ROW8
Text Label 6950 1850 2    50   ~ 0
ROW9
Text Label 6950 1950 2    50   ~ 0
ROW10
Text Label 6950 2050 2    50   ~ 0
ROW11
Text Label 6950 2150 2    50   ~ 0
ROW12
Text Label 6950 2250 2    50   ~ 0
ROW13
Text Label 6950 2350 2    50   ~ 0
ROW14
Text Label 6950 2450 2    50   ~ 0
ROW15
Text Label 6950 2550 2    50   ~ 0
COL0
Text Label 6950 2650 2    50   ~ 0
COL1
Text Label 6950 2750 2    50   ~ 0
COL2
Text Label 6950 2850 2    50   ~ 0
COL3
Text Label 6950 2950 2    50   ~ 0
COL4
Text Label 6950 3050 2    50   ~ 0
COL5
Text Label 6950 3150 2    50   ~ 0
COL6
Text Label 6950 3250 2    50   ~ 0
COL7
Text Label 6950 3350 2    50   ~ 0
COL8
Text Label 6950 3450 2    50   ~ 0
COL9
Text Label 6950 3550 2    50   ~ 0
COL10
NoConn ~ 6950 3650
NoConn ~ 6950 3750
NoConn ~ 6950 3850
NoConn ~ 6950 3950
NoConn ~ 6950 4050
NoConn ~ 6950 4150
$Comp
L Connector:Conn_01x16_Female J32
U 1 1 5FE0BAEB
P 10250 1650
F 0 "J32" H 10278 1626 50  0000 L CNN
F 1 "ROWS" H 10278 1535 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 10250 1650 50  0001 C CNN
F 3 "~" H 10250 1650 50  0001 C CNN
	1    10250 1650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x11_Female J33
U 1 1 5FE0E409
P 10250 3200
F 0 "J33" H 10278 3226 50  0000 L CNN
F 1 "COLUMNS" H 10278 3135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x11_P2.54mm_Vertical" H 10250 3200 50  0001 C CNN
F 3 "~" H 10250 3200 50  0001 C CNN
	1    10250 3200
	1    0    0    -1  
$EndComp
NoConn ~ 6950 850 
Text Label 10050 950  2    50   ~ 0
ROW0
Text Label 10050 1050 2    50   ~ 0
ROW1
Text Label 10050 1150 2    50   ~ 0
ROW2
Text Label 10050 1250 2    50   ~ 0
ROW3
Text Label 10050 1350 2    50   ~ 0
ROW4
Text Label 10050 1450 2    50   ~ 0
ROW5
Text Label 10050 1550 2    50   ~ 0
ROW6
Text Label 10050 1650 2    50   ~ 0
ROW7
Text Label 10050 1750 2    50   ~ 0
ROW8
Text Label 10050 1850 2    50   ~ 0
ROW9
Text Label 10050 1950 2    50   ~ 0
ROW10
Text Label 10050 2050 2    50   ~ 0
ROW11
Text Label 10050 2150 2    50   ~ 0
ROW12
Text Label 10050 2250 2    50   ~ 0
ROW13
Text Label 10050 2350 2    50   ~ 0
ROW14
Text Label 10050 2450 2    50   ~ 0
ROW15
Text Label 10050 2700 2    50   ~ 0
COL0
Text Label 10050 2800 2    50   ~ 0
COL1
Text Label 10050 2900 2    50   ~ 0
COL2
Text Label 10050 3000 2    50   ~ 0
COL3
Text Label 10050 3100 2    50   ~ 0
COL4
Text Label 10050 3200 2    50   ~ 0
COL5
Text Label 10050 3300 2    50   ~ 0
COL6
Text Label 10050 3400 2    50   ~ 0
COL7
Text Label 10050 3500 2    50   ~ 0
COL8
Text Label 10050 3600 2    50   ~ 0
COL9
Text Label 10050 3700 2    50   ~ 0
COL10
$Comp
L Connector:Conn_01x16_Female J26
U 1 1 5FE17DE3
P 9300 1650
F 0 "J26" H 9328 1626 50  0000 L CNN
F 1 "ROWS" H 9328 1535 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 9300 1650 50  0001 C CNN
F 3 "~" H 9300 1650 50  0001 C CNN
	1    9300 1650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x11_Female J27
U 1 1 5FE17DE9
P 9300 3200
F 0 "J27" H 9328 3226 50  0000 L CNN
F 1 "COLUMNS" H 9328 3135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x11_P2.54mm_Vertical" H 9300 3200 50  0001 C CNN
F 3 "~" H 9300 3200 50  0001 C CNN
	1    9300 3200
	1    0    0    -1  
$EndComp
Text Label 9100 950  2    50   ~ 0
ROW0
Text Label 9100 1050 2    50   ~ 0
ROW1
Text Label 9100 1150 2    50   ~ 0
ROW2
Text Label 9100 1250 2    50   ~ 0
ROW3
Text Label 9100 1350 2    50   ~ 0
ROW4
Text Label 9100 1450 2    50   ~ 0
ROW5
Text Label 9100 1550 2    50   ~ 0
ROW6
Text Label 9100 1650 2    50   ~ 0
ROW7
Text Label 9100 1750 2    50   ~ 0
ROW8
Text Label 9100 1850 2    50   ~ 0
ROW9
Text Label 9100 1950 2    50   ~ 0
ROW10
Text Label 9100 2050 2    50   ~ 0
ROW11
Text Label 9100 2150 2    50   ~ 0
ROW12
Text Label 9100 2250 2    50   ~ 0
ROW13
Text Label 9100 2350 2    50   ~ 0
ROW14
Text Label 9100 2450 2    50   ~ 0
ROW15
Text Label 9100 2700 2    50   ~ 0
COL0
Text Label 9100 2800 2    50   ~ 0
COL1
Text Label 9100 2900 2    50   ~ 0
COL2
Text Label 9100 3000 2    50   ~ 0
COL3
Text Label 9100 3100 2    50   ~ 0
COL4
Text Label 9100 3200 2    50   ~ 0
COL5
Text Label 9100 3300 2    50   ~ 0
COL6
Text Label 9100 3400 2    50   ~ 0
COL7
Text Label 9100 3500 2    50   ~ 0
COL8
Text Label 9100 3600 2    50   ~ 0
COL9
Text Label 9100 3700 2    50   ~ 0
COL10
$Comp
L Connector:Conn_01x16_Female J24
U 1 1 5FE18A01
P 8550 1650
F 0 "J24" H 8578 1626 50  0000 L CNN
F 1 "ROWS" H 8578 1535 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 8550 1650 50  0001 C CNN
F 3 "~" H 8550 1650 50  0001 C CNN
	1    8550 1650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x11_Female J25
U 1 1 5FE18A0B
P 8550 3200
F 0 "J25" H 8578 3226 50  0000 L CNN
F 1 "COLUMNS" H 8578 3135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x11_P2.54mm_Vertical" H 8550 3200 50  0001 C CNN
F 3 "~" H 8550 3200 50  0001 C CNN
	1    8550 3200
	1    0    0    -1  
$EndComp
Text Label 8350 950  2    50   ~ 0
ROW0
Text Label 8350 1050 2    50   ~ 0
ROW1
Text Label 8350 1150 2    50   ~ 0
ROW2
Text Label 8350 1250 2    50   ~ 0
ROW3
Text Label 8350 1350 2    50   ~ 0
ROW4
Text Label 8350 1450 2    50   ~ 0
ROW5
Text Label 8350 1550 2    50   ~ 0
ROW6
Text Label 8350 1650 2    50   ~ 0
ROW7
Text Label 8350 1750 2    50   ~ 0
ROW8
Text Label 8350 1850 2    50   ~ 0
ROW9
Text Label 8350 1950 2    50   ~ 0
ROW10
Text Label 8350 2050 2    50   ~ 0
ROW11
Text Label 8350 2150 2    50   ~ 0
ROW12
Text Label 8350 2250 2    50   ~ 0
ROW13
Text Label 8350 2350 2    50   ~ 0
ROW14
Text Label 8350 2450 2    50   ~ 0
ROW15
Text Label 8350 2700 2    50   ~ 0
COL0
Text Label 8350 2800 2    50   ~ 0
COL1
Text Label 8350 2900 2    50   ~ 0
COL2
Text Label 8350 3000 2    50   ~ 0
COL3
Text Label 8350 3100 2    50   ~ 0
COL4
Text Label 8350 3200 2    50   ~ 0
COL5
Text Label 8350 3300 2    50   ~ 0
COL6
Text Label 8350 3400 2    50   ~ 0
COL7
Text Label 8350 3500 2    50   ~ 0
COL8
Text Label 8350 3600 2    50   ~ 0
COL9
Text Label 8350 3700 2    50   ~ 0
COL10
Text Label 2900 5200 0    50   ~ 0
ROW5
Text Notes 2700 7600 0    118  ~ 0
For 3 Pin Analog analog input is in centre, \notherwise order is +5V, GND, Analog Inputs\n
Text Label 4600 5700 0    50   ~ 0
ROW10
Text Label 4600 5800 0    50   ~ 0
ROW11
$Comp
L Device:D D1
U 1 1 60592261
P 5950 4750
F 0 "D1" H 5950 4966 50  0000 C CNN
F 1 "D" H 5950 4875 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 5950 4750 50  0001 C CNN
F 3 "~" H 5950 4750 50  0001 C CNN
	1    5950 4750
	-1   0    0    -1  
$EndComp
Text Notes 5600 4400 0    50   ~ 0
Fire Test
$Comp
L Connector:Conn_01x03_Female J18
U 1 1 60593DBA
P 5300 4850
F 0 "J18" H 5192 5135 50  0000 C CNN
F 1 "FIRETEST" H 5192 5044 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 5300 4850 50  0001 C CNN
F 3 "~" H 5300 4850 50  0001 C CNN
	1    5300 4850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5800 4750 5500 4750
$Comp
L Device:D D2
U 1 1 60598EEA
P 5950 4950
F 0 "D2" H 5950 4750 50  0000 C CNN
F 1 "D" H 5950 4850 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 5950 4950 50  0001 C CNN
F 3 "~" H 5950 4950 50  0001 C CNN
	1    5950 4950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5800 4950 5500 4950
NoConn ~ 4150 4400
NoConn ~ 4100 2850
Text Label 5500 4850 0    50   ~ 0
COL5
Text Label 6100 4750 0    50   ~ 0
ROW7
Text Label 6100 4950 0    50   ~ 0
ROW8
Wire Notes Line
	5150 4300 5150 5400
Wire Notes Line
	5150 5400 6400 5400
Wire Notes Line
	6400 5400 6400 4300
Wire Notes Line
	5150 4300 6400 4300
$Comp
L Mechanical:MountingHole H1
U 1 1 604EB466
P 8450 4800
F 0 "H1" H 8550 4846 50  0000 L CNN
F 1 "MountingHole" H 8550 4755 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 4800 50  0001 C CNN
F 3 "~" H 8450 4800 50  0001 C CNN
	1    8450 4800
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 604EB842
P 8450 5050
F 0 "H2" H 8550 5096 50  0000 L CNN
F 1 "MountingHole" H 8550 5005 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 5050 50  0001 C CNN
F 3 "~" H 8450 5050 50  0001 C CNN
	1    8450 5050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 604EC2BE
P 8450 5300
F 0 "H3" H 8550 5346 50  0000 L CNN
F 1 "MountingHole" H 8550 5255 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 5300 50  0001 C CNN
F 3 "~" H 8450 5300 50  0001 C CNN
	1    8450 5300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 604ECD46
P 8450 5600
F 0 "H4" H 8550 5646 50  0000 L CNN
F 1 "MountingHole" H 8550 5555 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 5600 50  0001 C CNN
F 3 "~" H 8450 5600 50  0001 C CNN
	1    8450 5600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H5
U 1 1 604ECD50
P 8450 5850
F 0 "H5" H 8550 5896 50  0000 L CNN
F 1 "MountingHole" H 8550 5805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 5850 50  0001 C CNN
F 3 "~" H 8450 5850 50  0001 C CNN
	1    8450 5850
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H6
U 1 1 604ECD5A
P 8450 6100
F 0 "H6" H 8550 6146 50  0000 L CNN
F 1 "MountingHole" H 8550 6055 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 6100 50  0001 C CNN
F 3 "~" H 8450 6100 50  0001 C CNN
	1    8450 6100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J28
U 1 1 60542417
P 9350 4400
F 0 "J28" H 9242 4985 50  0000 C CNN
F 1 "SpareA1" H 9242 4894 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 9350 4400 50  0001 C CNN
F 3 "~" H 9350 4400 50  0001 C CNN
	1    9350 4400
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J30
U 1 1 60548F78
P 9900 4400
F 0 "J30" H 9792 4985 50  0000 C CNN
F 1 "SpareA2" H 9792 4894 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 9900 4400 50  0001 C CNN
F 3 "~" H 9900 4400 50  0001 C CNN
	1    9900 4400
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J34
U 1 1 60549EB3
P 10450 4400
F 0 "J34" H 10342 4985 50  0000 C CNN
F 1 "SpareA3" H 10342 4894 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 10450 4400 50  0001 C CNN
F 3 "~" H 10450 4400 50  0001 C CNN
	1    10450 4400
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J29
U 1 1 6054B29A
P 9350 5700
F 0 "J29" H 9242 6285 50  0000 C CNN
F 1 "SpareB1" H 9242 6194 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 9350 5700 50  0001 C CNN
F 3 "~" H 9350 5700 50  0001 C CNN
	1    9350 5700
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J31
U 1 1 6054CC0E
P 9900 5700
F 0 "J31" H 9792 6285 50  0000 C CNN
F 1 "SpareB2" H 9792 6194 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 9900 5700 50  0001 C CNN
F 3 "~" H 9900 5700 50  0001 C CNN
	1    9900 5700
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J35
U 1 1 6054DF0C
P 10450 5700
F 0 "J35" H 10342 6285 50  0000 C CNN
F 1 "SpareB3" H 10342 6194 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 10450 5700 50  0001 C CNN
F 3 "~" H 10450 5700 50  0001 C CNN
	1    10450 5700
	-1   0    0    -1  
$EndComp
Text Label 9550 4000 0    50   ~ 0
A1
Text Label 10650 4000 0    50   ~ 0
A1
Text Label 9550 4100 0    50   ~ 0
A2
Text Label 9550 4200 0    50   ~ 0
A3
Text Label 9550 4300 0    50   ~ 0
A4
Text Label 9550 4400 0    50   ~ 0
A5
Text Label 9550 4500 0    50   ~ 0
A6
Text Label 9550 4600 0    50   ~ 0
A7
Text Label 9550 4700 0    50   ~ 0
A8
Text Label 9550 4800 0    50   ~ 0
A9
Text Label 9550 4900 0    50   ~ 0
A10
Text Label 10100 4000 0    50   ~ 0
A1
Text Label 10100 4100 0    50   ~ 0
A2
Text Label 10100 4200 0    50   ~ 0
A3
Text Label 10100 4300 0    50   ~ 0
A4
Text Label 10100 4400 0    50   ~ 0
A5
Text Label 10100 4500 0    50   ~ 0
A6
Text Label 10100 4600 0    50   ~ 0
A7
Text Label 10100 4700 0    50   ~ 0
A8
Text Label 10100 4800 0    50   ~ 0
A9
Text Label 10100 4900 0    50   ~ 0
A10
Text Label 10650 4100 0    50   ~ 0
A2
Text Label 10650 4200 0    50   ~ 0
A3
Text Label 10650 4300 0    50   ~ 0
A4
Text Label 10650 4400 0    50   ~ 0
A5
Text Label 10650 4500 0    50   ~ 0
A6
Text Label 10650 4600 0    50   ~ 0
A7
Text Label 10650 4700 0    50   ~ 0
A8
Text Label 10650 4800 0    50   ~ 0
A9
Text Label 10650 4900 0    50   ~ 0
A10
Text Label 9550 5300 0    50   ~ 0
B1
Text Label 9550 5400 0    50   ~ 0
B2
Text Label 9550 5500 0    50   ~ 0
B3
Text Label 9550 5600 0    50   ~ 0
B4
Text Label 9550 5700 0    50   ~ 0
B5
Text Label 9550 5800 0    50   ~ 0
B6
Text Label 9550 5900 0    50   ~ 0
B7
Text Label 9550 6000 0    50   ~ 0
B8
Text Label 9550 6100 0    50   ~ 0
B9
Text Label 9550 6200 0    50   ~ 0
B10
Text Label 10100 5300 0    50   ~ 0
B1
Text Label 10100 5400 0    50   ~ 0
B2
Text Label 10100 5500 0    50   ~ 0
B3
Text Label 10100 5600 0    50   ~ 0
B4
Text Label 10100 5700 0    50   ~ 0
B5
Text Label 10100 5800 0    50   ~ 0
B6
Text Label 10100 5900 0    50   ~ 0
B7
Text Label 10100 6000 0    50   ~ 0
B8
Text Label 10100 6100 0    50   ~ 0
B9
Text Label 10100 6200 0    50   ~ 0
B10
Text Label 10650 5300 0    50   ~ 0
B1
Text Label 10650 5400 0    50   ~ 0
B2
Text Label 10650 5500 0    50   ~ 0
B3
Text Label 10650 5600 0    50   ~ 0
B4
Text Label 10650 5700 0    50   ~ 0
B5
Text Label 10650 5800 0    50   ~ 0
B6
Text Label 10650 5900 0    50   ~ 0
B7
Text Label 10650 6000 0    50   ~ 0
B8
Text Label 10650 6100 0    50   ~ 0
B9
Text Label 10650 6200 0    50   ~ 0
B10
$Comp
L Mechanical:MountingHole H7
U 1 1 605F60C5
P 8450 6350
F 0 "H7" H 8550 6396 50  0000 L CNN
F 1 "MountingHole" H 8550 6305 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8450 6350 50  0001 C CNN
F 3 "~" H 8450 6350 50  0001 C CNN
	1    8450 6350
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H8
U 1 1 605F6C77
P 9250 6350
F 0 "H8" H 9350 6396 50  0000 L CNN
F 1 "MountingHole" H 9350 6305 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9250 6350 50  0001 C CNN
F 3 "~" H 9250 6350 50  0001 C CNN
	1    9250 6350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J20
U 1 1 606B6DC2
P 6700 5950
F 0 "J20" H 6728 5926 50  0000 L CNN
F 1 "ANALOG B" H 6728 5835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 6700 5950 50  0001 C CNN
F 3 "~" H 6700 5950 50  0001 C CNN
	1    6700 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J23
U 1 1 606B8169
P 7850 5950
F 0 "J23" H 7878 5926 50  0000 L CNN
F 1 "ANALOG C" H 7878 5835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7850 5950 50  0001 C CNN
F 3 "~" H 7850 5950 50  0001 C CNN
	1    7850 5950
	1    0    0    -1  
$EndComp
Text Label 6500 5750 2    50   ~ 0
ANALOG_5V
Text Label 7650 5850 2    50   ~ 0
ANALOG_GND
NoConn ~ 6500 6150
NoConn ~ 6500 6250
$EndSCHEMATC