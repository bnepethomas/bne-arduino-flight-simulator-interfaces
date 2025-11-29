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
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 5EED210C
P 1400 1600
F 0 "J1" H 1428 1576 50  0000 L CNN
F 1 "12V in" H 1428 1485 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 1400 1600 50  0001 C CNN
F 3 "~" H 1400 1600 50  0001 C CNN
	1    1400 1600
	1    0    0    -1  
$EndComp
Text Label 1850 2250 2    50   ~ 0
+12Vin
Text Label 1850 2350 2    50   ~ 0
12VGND
$Comp
L OH---Relay-and-Backlight-Distribution-rescue:PT_SmallPCB-PT_Symbol_Library_v001 SPCB1
U 1 1 5EEE094C
P 1550 6950
F 0 "SPCB1" H 1778 6959 50  0000 L CNN
F 1 "PT_SmallPCB" H 1778 6868 50  0000 L CNN
F 2 "PT_Library_v001:PT_General_PCB_Outline" H 1525 6950 50  0001 C CNN
F 3 "" H 1525 6950 50  0001 C CNN
	1    1550 6950
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 5EEE0D9A
P 2750 6750
F 0 "H1" H 2850 6796 50  0000 L CNN
F 1 "MountingHole" H 2850 6705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 6750 50  0001 C CNN
F 3 "~" H 2750 6750 50  0001 C CNN
	1    2750 6750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5EEE14AA
P 2750 7000
F 0 "H2" H 2850 7046 50  0000 L CNN
F 1 "MountingHole" H 2850 6955 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 7000 50  0001 C CNN
F 3 "~" H 2750 7000 50  0001 C CNN
	1    2750 7000
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5EEE1A18
P 2750 7250
F 0 "H3" H 2850 7296 50  0000 L CNN
F 1 "MountingHole" H 2850 7205 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 7250 50  0001 C CNN
F 3 "~" H 2750 7250 50  0001 C CNN
	1    2750 7250
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5EEE1A22
P 2750 7500
F 0 "H4" H 2850 7546 50  0000 L CNN
F 1 "MountingHole" H 2850 7455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 7500 50  0001 C CNN
F 3 "~" H 2750 7500 50  0001 C CNN
	1    2750 7500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5EF187D2
P 3000 2500
F 0 "R1" H 3070 2546 50  0000 L CNN
F 1 "1K" H 3070 2455 50  0000 L CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2930 2500 50  0001 C CNN
F 3 "~" H 3000 2500 50  0001 C CNN
	1    3000 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5EF190EC
P 3600 2500
F 0 "R2" H 3670 2546 50  0000 L CNN
F 1 "330" H 3670 2455 50  0000 L CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3530 2500 50  0001 C CNN
F 3 "~" H 3600 2500 50  0001 C CNN
	1    3600 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5EF19DD9
P 3000 2800
F 0 "D1" V 3039 2683 50  0000 R CNN
F 1 "LED" V 2948 2683 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 3000 2800 50  0001 C CNN
F 3 "~" H 3000 2800 50  0001 C CNN
	1    3000 2800
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 5EF1C533
P 3600 2800
F 0 "D2" V 3639 2683 50  0000 R CNN
F 1 "LED" V 3548 2683 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 3600 2800 50  0001 C CNN
F 3 "~" H 3600 2800 50  0001 C CNN
	1    3600 2800
	0    -1   -1   0   
$EndComp
Text Label 3000 2950 3    50   ~ 0
12VGND
Text Label 3000 2350 0    50   ~ 0
+12Vin
Text Label 3600 2350 0    50   ~ 0
+5Vin
Text Label 3600 2950 3    50   ~ 0
5VGND
Text Notes 7150 1250 0    50   ~ 0
12v in, 5v in\nNight Vis pwm bus (3*2)\nAMPCD 12V Barel, \nLIP Backlight PWM
$Comp
L Connector:Conn_01x02_Female J12
U 1 1 63B6B0B6
P 5550 1150
F 0 "J12" H 5578 1126 50  0000 L CNN
F 1 "NVG In" H 5578 1035 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 5550 1150 50  0001 C CNN
F 3 "~" H 5550 1150 50  0001 C CNN
	1    5550 1150
	1    0    0    -1  
$EndComp
Text Label 5350 1150 2    50   ~ 0
NVG+
Text Label 5350 1250 2    50   ~ 0
NVG-
$Comp
L Connector:Conn_01x02_Female J7
U 1 1 63B6FA31
P 4900 1150
F 0 "J7" H 4928 1126 50  0000 L CNN
F 1 "Rght NVG" H 4928 1035 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4900 1150 50  0001 C CNN
F 3 "~" H 4900 1150 50  0001 C CNN
	1    4900 1150
	1    0    0    -1  
$EndComp
Text Label 4700 1150 2    50   ~ 0
NVG+
Text Label 4700 1250 2    50   ~ 0
NVG-
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 63B70D9F
P 4300 1150
F 0 "J6" H 4328 1126 50  0000 L CNN
F 1 "Left NVG" H 4328 1035 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4300 1150 50  0001 C CNN
F 3 "~" H 4300 1150 50  0001 C CNN
	1    4300 1150
	1    0    0    -1  
$EndComp
Text Label 4100 1150 2    50   ~ 0
NVG+
Text Label 4100 1250 2    50   ~ 0
NVG-
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 63B89A3E
P 2950 1250
F 0 "J5" H 2978 1226 50  0000 L CNN
F 1 "5V In" H 2978 1135 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 2950 1250 50  0001 C CNN
F 3 "~" H 2950 1250 50  0001 C CNN
	1    2950 1250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J16
U 1 1 63B89EBD
P 9650 2450
F 0 "J16" H 9678 2426 50  0000 L CNN
F 1 "4 * RELAY" H 9678 2335 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-08A_1x08_P2.54mm_Vertical" H 9650 2450 50  0001 C CNN
F 3 "~" H 9650 2450 50  0001 C CNN
	1    9650 2450
	1    0    0    -1  
$EndComp
Text Label 9450 2250 2    50   ~ 0
+5Vin
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 63B8BAFE
P 2150 1250
F 0 "J4" H 2178 1226 50  0000 L CNN
F 1 "5V Stndby" H 2178 1135 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 2150 1250 50  0001 C CNN
F 3 "~" H 2150 1250 50  0001 C CNN
	1    2150 1250
	1    0    0    -1  
$EndComp
Text Label 1950 1350 2    50   ~ 0
5VGND
Text Label 1850 2750 2    50   ~ 0
5VGND
Text Label 1850 2650 2    50   ~ 0
+5Vin
Text Label 1950 1250 2    50   ~ 0
+5VSwitched
Text Label 9450 2150 2    50   ~ 0
+5VSwitched
Text Label 9450 2450 2    50   ~ 0
+12Vin
Text Label 9450 2350 2    50   ~ 0
+12VLeftDDI
Text Label 9450 2550 2    50   ~ 0
+12VRightDDI
Text Label 9450 2650 2    50   ~ 0
+12Vin
Text Label 9450 2750 2    50   ~ 0
+12VChartDimmer
Text Label 9450 2850 2    50   ~ 0
+12Vin
$Comp
L Connector:Conn_01x02_Female J13
U 1 1 63B98E71
P 6700 2000
F 0 "J13" H 6728 1976 50  0000 L CNN
F 1 "Left DDI" H 6728 1885 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 6700 2000 50  0001 C CNN
F 3 "~" H 6700 2000 50  0001 C CNN
	1    6700 2000
	1    0    0    -1  
$EndComp
Text Label 6500 2000 2    50   ~ 0
+12VLeftDDI
Text Label 6500 2100 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J14
U 1 1 63B9A191
P 6700 2450
F 0 "J14" H 6728 2426 50  0000 L CNN
F 1 "Right DDI" H 6728 2335 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 6700 2450 50  0001 C CNN
F 3 "~" H 6700 2450 50  0001 C CNN
	1    6700 2450
	1    0    0    -1  
$EndComp
Text Label 6500 2550 2    50   ~ 0
12VGND
Text Label 6500 2450 2    50   ~ 0
+12VRightDDI
$Comp
L Connector:Conn_01x02_Female J15
U 1 1 63B9BC51
P 6800 3000
F 0 "J15" H 6828 2976 50  0000 L CNN
F 1 "Chart Dimmer" H 6828 2885 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 6800 3000 50  0001 C CNN
F 3 "~" H 6800 3000 50  0001 C CNN
	1    6800 3000
	1    0    0    -1  
$EndComp
Text Label 6600 3100 2    50   ~ 0
12VGND
Text Label 6600 3000 2    50   ~ 0
+12VChartDimmer
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 63BA7B0F
P 5150 2000
F 0 "J11" H 5178 1976 50  0000 L CNN
F 1 "AMPCD" H 5178 1885 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 5150 2000 50  0001 C CNN
F 3 "~" H 5150 2000 50  0001 C CNN
	1    5150 2000
	1    0    0    -1  
$EndComp
Text Label 4950 2100 2    50   ~ 0
12VGND
Text Label 4950 2000 2    50   ~ 0
+12VLeftDDI
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 63BAE346
P 2050 2250
F 0 "J2" H 2078 2226 50  0000 L CNN
F 1 "12V in" H 2078 2135 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2050 2250 50  0001 C CNN
F 3 "~" H 2050 2250 50  0001 C CNN
	1    2050 2250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 63BAECCE
P 2050 2650
F 0 "J3" H 2078 2626 50  0000 L CNN
F 1 "5V in" H 2078 2535 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 2050 2650 50  0001 C CNN
F 3 "~" H 2050 2650 50  0001 C CNN
	1    2050 2650
	1    0    0    -1  
$EndComp
Text Label 2750 1350 2    50   ~ 0
5VGND
Text Label 2750 1250 2    50   ~ 0
+5Vin
Text Label 1200 1600 2    50   ~ 0
+12Vin
Text Label 1200 1700 2    50   ~ 0
12VGND
Text Notes 6150 4150 0    50   ~ 0
#define BACKLIGHTING_RELAY_PORT RELAY_PORT_1\n#define RIGHT_SCREEN_RELAY_PORT RELAY_PORT_2\n#define LEFT_AND_AMPCD_SCREENS_RELAY_PORT RELAY_PORT_3\n
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 63B74703
P 4950 3950
F 0 "J8" H 4978 3926 50  0000 L CNN
F 1 "Spare" H 4978 3835 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4950 3950 50  0001 C CNN
F 3 "~" H 4950 3950 50  0001 C CNN
	1    4950 3950
	1    0    0    -1  
$EndComp
Text Label 4750 4050 2    50   ~ 0
A1
Text Label 4750 3950 2    50   ~ 0
A2
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 63B76141
P 4950 4200
F 0 "J9" H 4978 4176 50  0000 L CNN
F 1 "Spare" H 4978 4085 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4950 4200 50  0001 C CNN
F 3 "~" H 4950 4200 50  0001 C CNN
	1    4950 4200
	1    0    0    -1  
$EndComp
Text Label 4750 4300 2    50   ~ 0
A1
Text Label 4750 4200 2    50   ~ 0
A2
$Comp
L Connector:Conn_01x02_Female J10
U 1 1 63B77B75
P 4950 4500
F 0 "J10" H 4978 4476 50  0000 L CNN
F 1 "Spare" H 4978 4385 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4950 4500 50  0001 C CNN
F 3 "~" H 4950 4500 50  0001 C CNN
	1    4950 4500
	1    0    0    -1  
$EndComp
Text Label 4750 4600 2    50   ~ 0
A1
Text Label 4750 4500 2    50   ~ 0
A2
$EndSCHEMATC
