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
F 1 "12V ULN2003A" H 4050 4676 50  0000 C CNN
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
F 1 "12V Power In" H 5242 2394 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 5350 2300 50  0001 C CNN
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
Text Label 4050 4700 0    50   ~ 0
GND
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 60740F05
P 8000 1700
F 0 "J9" H 8028 1676 50  0000 L CNN
F 1 "12V Landing Gear Sol" H 8028 1585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 1700 50  0001 C CNN
F 3 "~" H 8000 1700 50  0001 C CNN
	1    8000 1700
	1    0    0    -1  
$EndComp
Text Label 2700 4600 0    50   ~ 0
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
Text Label 2850 3900 0    50   ~ 0
In_Landing_Gear_Sol
Text Label 4450 3900 0    50   ~ 0
12V_Landing_Gear_Sol
Text Label 7800 1800 2    50   ~ 0
12V_Landing_Gear_Sol
Text Label 7800 1700 2    50   ~ 0
+12V
Text Label 2850 4000 0    50   ~ 0
In_Launch_Bar_Mag
Text Label 4450 4000 0    50   ~ 0
12V_Launch_Bar_Mag
$Comp
L Connector:Conn_01x04_Female J10
U 1 1 6075F258
P 8000 2750
F 0 "J10" H 8028 2726 50  0000 L CNN
F 1 "12V Select Jet" H 8028 2635 50  0000 L CNN
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
12V_Launch_Bar_Mag
Text Label 4450 4100 0    50   ~ 0
12V_Hook_Bypass_Mag
Text Label 7800 2950 2    50   ~ 0
12V_Hook_Bypass_Mag
$Comp
L Connector:Conn_01x04_Female J12
U 1 1 60762D4E
P 8000 3900
F 0 "J12" H 8028 3876 50  0000 L CNN
F 1 "Brake Pressure" H 8028 3785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 3900 50  0001 C CNN
F 3 "~" H 8000 3900 50  0001 C CNN
	1    8000 3900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J7
U 1 1 60765586
P 7400 3900
F 0 "J7" H 7292 4185 50  0000 C CNN
F 1 "Brake Pressure" H 7292 4094 50  0000 C CNN
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
12V_Fuel_Dump_Mag
Text Label 2850 4100 0    50   ~ 0
In_Hook_Bypass_Mag
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 60768592
P 8000 3300
F 0 "J11" H 8028 3276 50  0000 L CNN
F 1 "12V Fuel Dump" H 8028 3185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 3300 50  0001 C CNN
F 3 "~" H 8000 3300 50  0001 C CNN
	1    8000 3300
	1    0    0    -1  
$EndComp
Text Label 7800 3400 2    50   ~ 0
12V_Fuel_Dump_Mag
Text Label 7800 3300 2    50   ~ 0
+12V
$Comp
L Connector:Conn_01x03_Female J17
U 1 1 60769A26
P 10000 5950
F 0 "J17" H 10028 5976 50  0000 L CNN
F 1 "Trim Servo" H 10028 5885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 10000 5950 50  0001 C CNN
F 3 "~" H 10000 5950 50  0001 C CNN
	1    10000 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J15
U 1 1 6076A1A4
P 9150 5950
F 0 "J15" H 9042 6235 50  0000 C CNN
F 1 "Trim Servo" H 9042 6144 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9150 5950 50  0001 C CNN
F 3 "~" H 9150 5950 50  0001 C CNN
	1    9150 5950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9800 5850 9350 5850
Wire Wire Line
	9350 5950 9800 5950
Wire Wire Line
	9800 6050 9350 6050
Text Label 2850 4300 0    50   ~ 0
In_APU_Mag
Text Label 4450 4300 0    50   ~ 0
12V_APU_Mag
$Comp
L Connector:Conn_01x04_Female J13
U 1 1 6076D2F2
P 8000 5100
F 0 "J13" H 8028 5076 50  0000 L CNN
F 1 "12V APU" H 8028 4985 50  0000 L CNN
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
12V_APU_Mag
Text Label 7800 5300 2    50   ~ 0
12V_Engine_Crank_Mag
Text Label 2850 4400 0    50   ~ 0
In_Engine_Crank_Mag
Text Label 4450 4400 0    50   ~ 0
12V_Engine_Crank_Mag
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
Text Label 3650 4500 2    50   ~ 0
GND
Text Label 9350 5850 0    50   ~ 0
SERVO_PWM
Text Label 9350 5950 0    50   ~ 0
SERVO_5V
Text Label 9350 6050 0    50   ~ 0
GND
Text Label 10000 4100 0    50   ~ 0
LEDCOL1
Text Label 10000 3900 0    50   ~ 0
LEDROW1
Text Label 10000 4000 0    50   ~ 0
LEDROW2
Text Label 6250 6700 0    50   ~ 0
LEDCOL1
Text Label 6250 6600 0    50   ~ 0
LEDROW1
Text Label 8000 2250 0    50   ~ 0
LEDCOL1
Text Label 8000 2150 0    50   ~ 0
LEDROW2
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 61709A1F
P 6050 6600
F 0 "J4" H 5942 6785 50  0000 C CNN
F 1 "APU LED" H 5942 6694 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6050 6600 50  0001 C CNN
F 3 "~" H 6050 6600 50  0001 C CNN
	1    6050 6600
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 6170C2D5
P 7800 2150
F 0 "J8" H 7692 2335 50  0000 C CNN
F 1 "LANDING GEAR LED" H 7692 2244 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7800 2150 50  0001 C CNN
F 3 "~" H 7800 2150 50  0001 C CNN
	1    7800 2150
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J16
U 1 1 6170EF5B
P 9800 4000
F 0 "J16" H 9692 4285 50  0000 C CNN
F 1 "LED IN" H 9692 4194 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9800 4000 50  0001 C CNN
F 3 "~" H 9800 4000 50  0001 C CNN
	1    9800 4000
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J1
U 1 1 61714426
P 2500 4200
F 0 "J1" H 2392 4685 50  0000 C CNN
F 1 "CENTRAL OUTPUT" H 2392 4594 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 2500 4200 50  0001 C CNN
F 3 "~" H 2500 4200 50  0001 C CNN
	1    2500 4200
	-1   0    0    -1  
$EndComp
NoConn ~ 2700 4500
$Comp
L Transistor_Array:ULN2003A U2
U 1 1 617DBE7C
P 4050 5850
F 0 "U2" H 4050 6517 50  0000 C CNN
F 1 "5V ULN2003A" H 4050 6426 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 4100 5300 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 4150 5650 50  0001 C CNN
	1    4050 5850
	1    0    0    -1  
$EndComp
Text Label 4450 5450 0    50   ~ 0
SERVO_5V
Text Label 4050 6450 0    50   ~ 0
GND
Wire Wire Line
	2700 5650 3650 5650
Wire Wire Line
	2700 5750 3650 5750
Wire Wire Line
	2700 5850 3650 5850
Wire Wire Line
	2700 5950 3650 5950
Wire Wire Line
	2700 6050 3650 6050
Wire Wire Line
	3650 6150 2700 6150
Text Label 2850 5650 0    50   ~ 0
In_Landing_Gear_Sol
Text Label 4450 5650 0    50   ~ 0
5V_Landing_Gear_Sol
Text Label 2850 5750 0    50   ~ 0
In_Launch_Bar_Mag
Text Label 4450 5750 0    50   ~ 0
5V_Launch_Bar_Mag
Text Label 4450 5850 0    50   ~ 0
5V_Hook_Bypass_Mag
Text Label 2850 5950 0    50   ~ 0
In_Fuel_Dump_Mag
Text Label 4450 5950 0    50   ~ 0
5V_Fuel_Dump_Mag
Text Label 2850 5850 0    50   ~ 0
In_Hook_Bypass_Mag
Text Label 2850 6050 0    50   ~ 0
In_APU_Mag
Text Label 4450 6050 0    50   ~ 0
5V_APU_Mag
Text Label 2850 6150 0    50   ~ 0
In_Engine_Crank_Mag
Text Label 4450 6150 0    50   ~ 0
5V_Engine_Crank_Mag
NoConn ~ 4450 6250
Text Label 3650 6250 2    50   ~ 0
GND
$Comp
L Connector:Conn_01x04_Female J14
U 1 1 617E9460
P 8050 5750
F 0 "J14" H 8078 5726 50  0000 L CNN
F 1 "5V APU" H 8078 5635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8050 5750 50  0001 C CNN
F 3 "~" H 8050 5750 50  0001 C CNN
	1    8050 5750
	1    0    0    -1  
$EndComp
Text Label 7850 5650 2    50   ~ 0
SERVO_5V
Text Label 7850 5850 2    50   ~ 0
SERVO_5V
Text Label 7850 5750 2    50   ~ 0
5V_APU_Mag
Text Label 7850 5950 2    50   ~ 0
5V_Engine_Crank_Mag
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 617EF045
P 6100 3350
F 0 "J5" H 6128 3326 50  0000 L CNN
F 1 "5V Fuel Dump" H 6128 3235 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6100 3350 50  0001 C CNN
F 3 "~" H 6100 3350 50  0001 C CNN
	1    6100 3350
	1    0    0    -1  
$EndComp
Text Label 5900 3450 2    50   ~ 0
5V_Fuel_Dump_Mag
Text Label 5900 3350 2    50   ~ 0
SERVO_5V
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 617F018A
P 6400 2750
F 0 "J6" H 6428 2726 50  0000 L CNN
F 1 "5V Select Jet" H 6428 2635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 6400 2750 50  0001 C CNN
F 3 "~" H 6400 2750 50  0001 C CNN
	1    6400 2750
	1    0    0    -1  
$EndComp
Text Label 6200 2650 2    50   ~ 0
SERVO_5V
Text Label 6200 2850 2    50   ~ 0
SERVO_5V
Text Label 6200 2750 2    50   ~ 0
5V_Launch_Bar_Mag
Text Label 6200 2950 2    50   ~ 0
5V_Hook_Bypass_Mag
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 617F125C
P 5850 1700
F 0 "J3" H 5878 1676 50  0000 L CNN
F 1 "5V Landing Gear Sol" H 5878 1585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5850 1700 50  0001 C CNN
F 3 "~" H 5850 1700 50  0001 C CNN
	1    5850 1700
	1    0    0    -1  
$EndComp
Text Label 5650 1800 2    50   ~ 0
5V_Landing_Gear_Sol
Text Label 5650 1700 2    50   ~ 0
+12V
$EndSCHEMATC
