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
Text Notes 1400 2450 0    50   ~ 0
Right Console\n\n2003\nHook Led - include Resistor\nHook Led 2 - include Resistor \n\nPitot Heat- Mag Switch\nBleed Air Pull - Solenoid\nLaser Arm - Mag Switch\nCanopy Switch - Mag Switch\n\nHyd Press 1 - Servo\nHyd Press 2 - Servo\n\nBattery Left - Servo\nBattery Right - Servo\n
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
Text Label 4050 4700 0    50   ~ 0
GND
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 60740F05
P 8000 1700
F 0 "J5" H 8028 1676 50  0000 L CNN
F 1 "Hook Led 1" H 8028 1585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 1700 50  0001 C CNN
F 3 "~" H 8000 1700 50  0001 C CNN
	1    8000 1700
	1    0    0    -1  
$EndComp
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
L Connector:Conn_01x08_Female J1
U 1 1 6074D9D3
P 2500 4200
F 0 "J1" H 2392 4785 50  0000 C CNN
F 1 "Central Output" H 2392 4694 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 2500 4200 50  0001 C CNN
F 3 "~" H 2500 4200 50  0001 C CNN
	1    2500 4200
	-1   0    0    -1  
$EndComp
Text Label 2850 3900 0    50   ~ 0
In_Bleed_Air_Sol
Text Label 4450 3900 0    50   ~ 0
Out_Bleed_Air_Sol
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 607556D0
P 8000 2100
F 0 "J6" H 8028 2076 50  0000 L CNN
F 1 "Hook Led 2" H 8028 1985 50  0000 L CNN
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
Text Label 4450 4300 0    50   ~ 0
Out_Hook_Led_1
Text Label 2850 4300 0    50   ~ 0
In_Hook_Led_1
Text Label 2850 4400 0    50   ~ 0
In_Hook_Led_2
Text Label 4450 4400 0    50   ~ 0
Out_Hook_Led_2
Text Label 2850 4000 0    50   ~ 0
In_Pitot_Heat_Mag
Text Label 4450 4000 0    50   ~ 0
Out_Pitot_Heat_Mag
$Comp
L Connector:Conn_01x04_Female J7
U 1 1 6075F258
P 8000 2900
F 0 "J7" H 8028 2876 50  0000 L CNN
F 1 "ECS" H 8028 2785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 2900 50  0001 C CNN
F 3 "~" H 8000 2900 50  0001 C CNN
	1    8000 2900
	1    0    0    -1  
$EndComp
Text Label 7800 2800 2    50   ~ 0
+12V
Text Label 7800 3000 2    50   ~ 0
+12V
Text Label 7800 2900 2    50   ~ 0
Out_Bleed_Air_Sol
Text Label 7800 3100 2    50   ~ 0
Out_Pitot_Heat_Mag
$Comp
L Connector:Conn_01x03_Female J15
U 1 1 60769A26
P 9850 5950
F 0 "J15" H 9878 5976 50  0000 L CNN
F 1 "Hyd Press 2 Out" H 9878 5885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9850 5950 50  0001 C CNN
F 3 "~" H 9850 5950 50  0001 C CNN
	1    9850 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J13
U 1 1 6076A1A4
P 9150 5950
F 0 "J13" H 9042 6235 50  0000 C CNN
F 1 "Hyd Press 2 In" H 9042 6144 50  0000 C CNN
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
$Comp
L Device:R_US R1
U 1 1 6075CCE6
P 7350 1700
F 0 "R1" V 7555 1700 50  0000 C CNN
F 1 "560" V 7464 1700 50  0000 C CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 7390 1690 50  0001 C CNN
F 3 "~" H 7350 1700 50  0001 C CNN
	1    7350 1700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7800 1700 7500 1700
Text Label 7200 1700 2    50   ~ 0
+12V
Text Label 7800 1800 2    50   ~ 0
Out_Hook_Led_1
Text Label 7800 2200 2    50   ~ 0
Out_Hook_Led_2
Wire Notes Line
	6800 1400 8550 1400
Wire Notes Line
	8550 1400 8550 2300
Wire Notes Line
	8550 2300 6800 2300
Wire Notes Line
	6800 2300 6800 1400
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 60760DE9
P 8000 3500
F 0 "J8" H 8028 3476 50  0000 L CNN
F 1 "SNSR" H 8028 3385 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 3500 50  0001 C CNN
F 3 "~" H 8000 3500 50  0001 C CNN
	1    8000 3500
	1    0    0    -1  
$EndComp
Text Label 7800 3500 2    50   ~ 0
+12V
Text Label 2850 4100 0    50   ~ 0
In_Laser_Arm_Mag
Text Label 4450 4100 0    50   ~ 0
Out_Laser_Arm_Mag
Text Label 7800 3600 2    50   ~ 0
Out_Laser_Arm_Mag
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 60762C83
P 8000 3900
F 0 "J9" H 8028 3876 50  0000 L CNN
F 1 "CANOPY" H 8028 3785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 3900 50  0001 C CNN
F 3 "~" H 8000 3900 50  0001 C CNN
	1    8000 3900
	1    0    0    -1  
$EndComp
Text Label 7800 3900 2    50   ~ 0
+12V
Text Label 7800 4000 2    50   ~ 0
Out_Canopy_Mag
Text Label 2850 4200 0    50   ~ 0
In_Canopy_Mag
Text Label 4450 4200 0    50   ~ 0
Out_Canopy_Mag
NoConn ~ 2700 4500
Text Label 2700 4600 0    50   ~ 0
GND
Text Label 3650 4500 2    50   ~ 0
GND
NoConn ~ 4450 4500
$Comp
L Connector:Conn_01x03_Female J14
U 1 1 60769955
P 9850 5300
F 0 "J14" H 9878 5326 50  0000 L CNN
F 1 "Hyd Press 1 Out" H 9878 5235 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9850 5300 50  0001 C CNN
F 3 "~" H 9850 5300 50  0001 C CNN
	1    9850 5300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J12
U 1 1 6076995F
P 9150 5300
F 0 "J12" H 9042 5585 50  0000 C CNN
F 1 "Hyd Press 1 In" H 9042 5494 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 9150 5300 50  0001 C CNN
F 3 "~" H 9150 5300 50  0001 C CNN
	1    9150 5300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9650 5200 9350 5200
Wire Wire Line
	9350 5300 9650 5300
Wire Wire Line
	9650 5400 9350 5400
$Comp
L Connector:Conn_01x04_Female J11
U 1 1 6076C0DC
P 8000 6000
F 0 "J11" H 8028 6026 50  0000 L CNN
F 1 "Batt E Out" H 8028 5935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 6000 50  0001 C CNN
F 3 "~" H 8000 6000 50  0001 C CNN
	1    8000 6000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J4
U 1 1 6076C0E6
P 7300 6000
F 0 "J4" H 7192 6285 50  0000 C CNN
F 1 "Batt E In" H 7192 6194 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7300 6000 50  0001 C CNN
F 3 "~" H 7300 6000 50  0001 C CNN
	1    7300 6000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7800 5900 7500 5900
Wire Wire Line
	7500 6000 7800 6000
Wire Wire Line
	7800 6100 7500 6100
$Comp
L Connector:Conn_01x04_Female J10
U 1 1 6076C0F3
P 8000 5350
F 0 "J10" H 8028 5376 50  0000 L CNN
F 1 "Batt U Out" H 8028 5285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8000 5350 50  0001 C CNN
F 3 "~" H 8000 5350 50  0001 C CNN
	1    8000 5350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J3
U 1 1 6076C0FD
P 7300 5350
F 0 "J3" H 7192 5635 50  0000 C CNN
F 1 "Batt U In" H 7192 5544 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7300 5350 50  0001 C CNN
F 3 "~" H 7300 5350 50  0001 C CNN
	1    7300 5350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7800 5250 7500 5250
Wire Wire Line
	7500 5350 7800 5350
Wire Wire Line
	7800 5450 7500 5450
Wire Wire Line
	7800 5550 7500 5550
Wire Wire Line
	7800 6200 7500 6200
$Comp
L Mechanical:MountingHole H1
U 1 1 60771FD8
P 9600 1850
F 0 "H1" H 9700 1896 50  0000 L CNN
F 1 "MountingHole" H 9700 1805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9600 1850 50  0001 C CNN
F 3 "~" H 9600 1850 50  0001 C CNN
	1    9600 1850
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 60772046
P 9600 2150
F 0 "H2" H 9700 2196 50  0000 L CNN
F 1 "MountingHole" H 9700 2105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9600 2150 50  0001 C CNN
F 3 "~" H 9600 2150 50  0001 C CNN
	1    9600 2150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 6077240F
P 9600 2450
F 0 "H3" H 9700 2496 50  0000 L CNN
F 1 "MountingHole" H 9700 2405 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9600 2450 50  0001 C CNN
F 3 "~" H 9600 2450 50  0001 C CNN
	1    9600 2450
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 607727C3
P 9600 2850
F 0 "H4" H 9700 2896 50  0000 L CNN
F 1 "MountingHole" H 9700 2805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9600 2850 50  0001 C CNN
F 3 "~" H 9600 2850 50  0001 C CNN
	1    9600 2850
	1    0    0    -1  
$EndComp
$EndSCHEMATC
