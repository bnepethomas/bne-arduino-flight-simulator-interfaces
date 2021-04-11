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
Text Notes 1100 2350 0    50   ~ 0
Left Console\n\nAPU - LED - Include Resistor\nLanding Gear Handle - LED - Include Resistor\n\nLanding Gear - Solenoid\n\n\nLaunch Bar - Mag Switch\nHook Bypass - Mag Switch\n\nAPU - Mag Switch\nEngine Crank - Mag Switch\n\nFuel Dump - Mag Switch\n\n\nTrim - Servo\n\nBrake Pressure - Stepper\n
$Comp
L Transistor_Array:ULN2003A U1
U 1 1 6073B19B
P 4050 4100
F 0 "U1" H 4050 4767 50  0000 C CNN
F 1 "ULN2003A" H 4050 4676 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 4100 3550 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 4150 3900 50  0001 C CNN
	1    4050 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 6073DAF2
P 5350 2300
F 0 "J2" H 5242 2485 50  0000 C CNN
F 1 "Power In" H 5242 2394 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5350 2300 50  0001 C CNN
F 3 "~" H 5350 2300 50  0001 C CNN
	1    5350 2300
	-1   0    0    -1  
$EndComp
Text Label 5550 2300 0    50   ~ 0
+12V
Text Label 5550 2400 0    50   ~ 0
GND
Text Label 4450 3700 0    50   ~ 0
+12V
Text Label 4500 5100 0    50   ~ 0
+12V
Text Label 4050 4700 0    50   ~ 0
GND
Text Label 4100 6100 0    50   ~ 0
GND
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 60740F05
P 8000 1700
F 0 "J5" H 8028 1676 50  0000 L CNN
F 1 "Landing Gear Sol" H 8028 1585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 1700 50  0001 C CNN
F 3 "~" H 8000 1700 50  0001 C CNN
	1    8000 1700
	1    0    0    -1  
$EndComp
Text Label 2700 4800 0    50   ~ 0
GND
Wire Wire Line
	2700 3900 3650 3900
Wire Wire Line
	2700 4000 3650 4000
Wire Wire Line
	2700 4100 3650 4100
Wire Wire Line
	2700 4200 3650 4200
Wire Wire Line
	2700 4300 3650 4300
Wire Wire Line
	3650 4400 2700 4400
$Comp
L Connector:Conn_01x10_Female J1
U 1 1 6074D9D3
P 2500 4300
F 0 "J1" H 2392 4885 50  0000 C CNN
F 1 "Central Output" H 2392 4794 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 2500 4300 50  0001 C CNN
F 3 "~" H 2500 4300 50  0001 C CNN
	1    2500 4300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2700 4600 3150 4600
Wire Wire Line
	3150 4600 3150 5300
Wire Wire Line
	3150 5300 3700 5300
Wire Wire Line
	2700 4700 3050 4700
Wire Wire Line
	3050 4700 3050 5400
Wire Wire Line
	3050 5400 3700 5400
Text Label 2850 3900 0    50   ~ 0
In_Landing_Gear_Sol
Text Label 4450 3900 0    50   ~ 0
Out_Landing_Gear_Sol
Text Label 7800 1800 2    50   ~ 0
Out_Landing_Gear_Sol
Text Label 7800 1700 2    50   ~ 0
+12V
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 607556D0
P 8000 2100
F 0 "J6" H 8028 2076 50  0000 L CNN
F 1 "Landing Gear Handle" H 8028 1985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 2100 50  0001 C CNN
F 3 "~" H 8000 2100 50  0001 C CNN
	1    8000 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R2
U 1 1 60756564
P 7350 2100
F 0 "R2" V 7555 2100 50  0000 C CNN
F 1 "560" V 7464 2100 50  0000 C CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 7390 2090 50  0001 C CNN
F 3 "~" H 7350 2100 50  0001 C CNN
	1    7350 2100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7800 2100 7500 2100
Text Label 7200 2100 2    50   ~ 0
+12V
Text Label 7800 2200 2    50   ~ 0
Out_Landing_Gear_LED
Text Label 4500 5300 0    50   ~ 0
Out_Landing_Gear_LED
Text Label 3150 4800 0    50   ~ 0
In_Landing_Gear_LED
Text Label 3100 5400 0    50   ~ 0
In_APU_Led
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 6075AC33
P 7900 5950
F 0 "J4" H 7928 5926 50  0000 L CNN
F 1 "APU LED" H 7928 5835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7900 5950 50  0001 C CNN
F 3 "~" H 7900 5950 50  0001 C CNN
	1    7900 5950
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R1
U 1 1 6075AC3D
P 7250 5950
F 0 "R1" V 7455 5950 50  0000 C CNN
F 1 "560" V 7364 5950 50  0000 C CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 7290 5940 50  0001 C CNN
F 3 "~" H 7250 5950 50  0001 C CNN
	1    7250 5950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7700 5950 7400 5950
Text Label 7100 5950 2    50   ~ 0
+12V
Text Label 7700 6050 2    50   ~ 0
Out_APU_LED
Text Label 4500 5400 0    50   ~ 0
Out_APU_LED
Text Notes 6950 5600 0    50   ~ 0
LED Values\n560 to 12V (to be validated!)
Text Label 2850 4000 0    50   ~ 0
In_Launch_Bar_Mag
Text Label 4450 4000 0    50   ~ 0
Out_Launch_Bar_Mag
$Comp
L Connector:Conn_01x04_Female J7
U 1 1 6075F258
P 8000 2750
F 0 "J7" H 8028 2726 50  0000 L CNN
F 1 "Select Jet" H 8028 2635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 2750 50  0001 C CNN
F 3 "~" H 8000 2750 50  0001 C CNN
	1    8000 2750
	1    0    0    -1  
$EndComp
Text Label 7800 2650 2    50   ~ 0
+12V
Text Label 7800 2850 2    50   ~ 0
+12V
Text Label 7800 2750 2    50   ~ 0
Out_Launch_Bar_Mag
Text Label 4450 4100 0    50   ~ 0
Out_Hook_Bypass_Mag
Text Label 7800 2950 2    50   ~ 0
Out_Hook_Bypass_Mag
$Comp
L Connector:Conn_01x04_Female J9
U 1 1 60762D4E
P 8000 3900
F 0 "J9" H 8028 3876 50  0000 L CNN
F 1 "Brake Pressure Out" H 8028 3785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 3900 50  0001 C CNN
F 3 "~" H 8000 3900 50  0001 C CNN
	1    8000 3900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J3
U 1 1 60765586
P 7400 3900
F 0 "J3" H 7292 4185 50  0000 C CNN
F 1 "Brake Pressure In" H 7292 4094 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7400 3900 50  0001 C CNN
F 3 "~" H 7400 3900 50  0001 C CNN
	1    7400 3900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7800 3800 7600 3800
Wire Wire Line
	7800 3900 7600 3900
Wire Wire Line
	7800 4000 7600 4000
Wire Wire Line
	7800 4100 7600 4100
Text Label 2850 4200 0    50   ~ 0
In_Fuel_Dump_Mag
Text Label 4450 4200 0    50   ~ 0
Out_Fuel_Dump_Mag
Text Label 2850 4100 0    50   ~ 0
In_Hook_Bypass_Mag
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 60768592
P 8000 3300
F 0 "J8" H 8028 3276 50  0000 L CNN
F 1 "Fuel Dump" H 8028 3185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 3300 50  0001 C CNN
F 3 "~" H 8000 3300 50  0001 C CNN
	1    8000 3300
	1    0    0    -1  
$EndComp
Text Label 7800 3400 2    50   ~ 0
Out_Fuel_Dump_Mag
Text Label 7800 3300 2    50   ~ 0
+12V
$Comp
L Connector:Conn_01x03_Female J12
U 1 1 60769A26
P 9850 5950
F 0 "J12" H 9878 5976 50  0000 L CNN
F 1 "Trim Servo Out" H 9878 5885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9850 5950 50  0001 C CNN
F 3 "~" H 9850 5950 50  0001 C CNN
	1    9850 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J11
U 1 1 6076A1A4
P 9150 5950
F 0 "J11" H 9042 6235 50  0000 C CNN
F 1 "Trim Servo Out" H 9042 6144 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9150 5950 50  0001 C CNN
F 3 "~" H 9150 5950 50  0001 C CNN
	1    9150 5950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9650 5850 9350 5850
Wire Wire Line
	9350 5950 9650 5950
Wire Wire Line
	9650 6050 9350 6050
Text Label 2850 4300 0    50   ~ 0
In_APU_Mag
Text Label 4450 4300 0    50   ~ 0
Out_APU_Mag
$Comp
L Connector:Conn_01x04_Female J10
U 1 1 6076D2F2
P 8000 5100
F 0 "J10" H 8028 5076 50  0000 L CNN
F 1 "APU" H 8028 4985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 5100 50  0001 C CNN
F 3 "~" H 8000 5100 50  0001 C CNN
	1    8000 5100
	1    0    0    -1  
$EndComp
Text Label 7800 5000 2    50   ~ 0
+12V
Text Label 7800 5200 2    50   ~ 0
+12V
Text Label 7800 5100 2    50   ~ 0
Out_APU_Mag
Text Label 7800 5300 2    50   ~ 0
Out_Engine_Crank_Mag
Text Label 2850 4400 0    50   ~ 0
In_Engine_Crank_Mag
Text Label 4450 4400 0    50   ~ 0
Out_Engine_Crank_Mag
Wire Notes Line
	6600 4800 8350 4800
Wire Notes Line
	8350 4800 8350 6200
Wire Notes Line
	8350 6200 6600 6200
Wire Notes Line
	6600 6200 6600 4800
Wire Notes Line
	6850 1450 9000 1450
Wire Notes Line
	9000 1450 9000 2400
Wire Notes Line
	9000 2400 6850 2400
Wire Notes Line
	6850 2400 6850 1450
Wire Notes Line
	8900 5500 10550 5500
Wire Notes Line
	10550 5500 10550 6200
Wire Notes Line
	10550 6200 8900 6200
Wire Notes Line
	8900 6200 8900 5500
Wire Notes Line
	6850 2550 9000 2550
Wire Notes Line
	9000 2550 9000 4350
Wire Notes Line
	9000 4350 6850 4350
Wire Notes Line
	6850 4350 6850 2550
$Comp
L Mechanical:MountingHole H1
U 1 1 6073BD38
P 9850 1850
F 0 "H1" H 9950 1896 50  0000 L CNN
F 1 "MountingHole" H 9950 1805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9850 1850 50  0001 C CNN
F 3 "~" H 9850 1850 50  0001 C CNN
	1    9850 1850
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 6073C29F
P 9850 2150
F 0 "H2" H 9950 2196 50  0000 L CNN
F 1 "MountingHole" H 9950 2105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9850 2150 50  0001 C CNN
F 3 "~" H 9850 2150 50  0001 C CNN
	1    9850 2150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 6073C71F
P 9850 2400
F 0 "H3" H 9950 2446 50  0000 L CNN
F 1 "MountingHole" H 9950 2355 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9850 2400 50  0001 C CNN
F 3 "~" H 9850 2400 50  0001 C CNN
	1    9850 2400
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 6073CBBD
P 9850 2750
F 0 "H4" H 9950 2796 50  0000 L CNN
F 1 "MountingHole" H 9950 2705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9850 2750 50  0001 C CNN
F 3 "~" H 9850 2750 50  0001 C CNN
	1    9850 2750
	1    0    0    -1  
$EndComp
NoConn ~ 4450 4500
NoConn ~ 4500 5500
NoConn ~ 4500 5600
NoConn ~ 4500 5700
NoConn ~ 4500 5800
NoConn ~ 4500 5900
$Comp
L Transistor_Array:ULN2003A U2
U 1 1 6073C2C9
P 4100 5500
F 0 "U2" H 4100 6167 50  0000 C CNN
F 1 "ULN2003A" H 4100 6076 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 4150 4950 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 4200 5300 50  0001 C CNN
	1    4100 5500
	1    0    0    -1  
$EndComp
Text Label 3700 5500 2    50   ~ 0
GND
Text Label 3700 5600 2    50   ~ 0
GND
Text Label 3700 5700 2    50   ~ 0
GND
Text Label 3700 5800 2    50   ~ 0
GND
Text Label 3700 5900 2    50   ~ 0
GND
Text Label 3650 4500 2    50   ~ 0
GND
NoConn ~ 2700 4500
$EndSCHEMATC
