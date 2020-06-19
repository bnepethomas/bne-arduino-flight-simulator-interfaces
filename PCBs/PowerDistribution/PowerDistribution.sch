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
F 1 "Conn_01x02_Female" H 1428 1485 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 1400 1600 50  0001 C CNN
F 3 "~" H 1400 1600 50  0001 C CNN
	1    1400 1600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 5EED23E2
P 1400 1900
F 0 "J2" H 1428 1876 50  0000 L CNN
F 1 "Conn_01x02_Female" H 1428 1785 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 1400 1900 50  0001 C CNN
F 3 "~" H 1400 1900 50  0001 C CNN
	1    1400 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J3
U 1 1 5EED2E0B
P 1400 2300
F 0 "J3" H 1428 2276 50  0000 L CNN
F 1 "Conn_01x04_Female" H 1428 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1400 2300 50  0001 C CNN
F 3 "~" H 1400 2300 50  0001 C CNN
	1    1400 2300
	1    0    0    -1  
$EndComp
Text Label 1200 2200 2    50   ~ 0
Green12V+
Text Label 1200 2300 2    50   ~ 0
GreenGND
Text Label 1200 2400 2    50   ~ 0
White+12V
Text Label 1200 2500 2    50   ~ 0
WhiteGND
Text Label 1200 1900 2    50   ~ 0
+5V
Text Label 1200 2000 2    50   ~ 0
5VGND
Text Label 1200 1600 2    50   ~ 0
+12V
Text Label 1200 1700 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 5EED8B2E
P 2450 3300
F 0 "J4" H 2478 3276 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2478 3185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2450 3300 50  0001 C CNN
F 3 "~" H 2450 3300 50  0001 C CNN
	1    2450 3300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 5EED8B38
P 2450 3600
F 0 "J5" H 2478 3576 50  0000 L CNN
F 1 "Conn_01x02_Female" H 2478 3485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2450 3600 50  0001 C CNN
F 3 "~" H 2450 3600 50  0001 C CNN
	1    2450 3600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 5EED8B42
P 2450 4000
F 0 "J6" H 2478 3976 50  0000 L CNN
F 1 "Conn_01x04_Female" H 2478 3885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 2450 4000 50  0001 C CNN
F 3 "~" H 2450 4000 50  0001 C CNN
	1    2450 4000
	1    0    0    -1  
$EndComp
Text Label 2250 3900 2    50   ~ 0
Green12V+
Text Label 2250 4000 2    50   ~ 0
GreenGND
Text Label 2250 4100 2    50   ~ 0
White+12V
Text Label 2250 4200 2    50   ~ 0
WhiteGND
Text Label 2250 3600 2    50   ~ 0
+5V
Text Label 2250 3700 2    50   ~ 0
5VGND
Text Label 2250 3300 2    50   ~ 0
+12V
Text Label 2250 3400 2    50   ~ 0
12VGND
$Comp
L PT_Symbol_Library_v001:PT_SmallPCB SPCB1
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
L Connector:Conn_01x02_Female J13
U 1 1 5EEE4478
P 5900 3350
F 0 "J13" H 5928 3326 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5928 3235 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5900 3350 50  0001 C CNN
F 3 "~" H 5900 3350 50  0001 C CNN
	1    5900 3350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J14
U 1 1 5EEE4482
P 5900 3650
F 0 "J14" H 5928 3626 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5928 3535 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5900 3650 50  0001 C CNN
F 3 "~" H 5900 3650 50  0001 C CNN
	1    5900 3650
	1    0    0    -1  
$EndComp
Text Label 5700 3650 2    50   ~ 0
+5V
Text Label 5700 3750 2    50   ~ 0
5VGND
Text Label 5700 3350 2    50   ~ 0
+12V
Text Label 5700 3450 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J15
U 1 1 5EEE6572
P 5900 4050
F 0 "J15" H 5928 4026 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5928 3935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5900 4050 50  0001 C CNN
F 3 "~" H 5900 4050 50  0001 C CNN
	1    5900 4050
	1    0    0    -1  
$EndComp
Text Label 5700 4050 2    50   ~ 0
Green12V+
Text Label 5700 4150 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J7
U 1 1 5EEE8D94
P 3950 3300
F 0 "J7" H 3978 3276 50  0000 L CNN
F 1 "Conn_01x02_Female" H 3978 3185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3950 3300 50  0001 C CNN
F 3 "~" H 3950 3300 50  0001 C CNN
	1    3950 3300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 5EEE8D9E
P 3950 3600
F 0 "J8" H 3978 3576 50  0000 L CNN
F 1 "Conn_01x02_Female" H 3978 3485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3950 3600 50  0001 C CNN
F 3 "~" H 3950 3600 50  0001 C CNN
	1    3950 3600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J9
U 1 1 5EEE8DA8
P 3950 4000
F 0 "J9" H 3978 3976 50  0000 L CNN
F 1 "Conn_01x04_Female" H 3978 3885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 3950 4000 50  0001 C CNN
F 3 "~" H 3950 4000 50  0001 C CNN
	1    3950 4000
	1    0    0    -1  
$EndComp
Text Label 3750 3900 2    50   ~ 0
Green12V+
Text Label 3750 4000 2    50   ~ 0
GreenGND
Text Label 3750 4100 2    50   ~ 0
White+12V
Text Label 3750 4200 2    50   ~ 0
WhiteGND
Text Label 3750 3600 2    50   ~ 0
+5V
Text Label 3750 3700 2    50   ~ 0
5VGND
Text Label 3750 3300 2    50   ~ 0
+12V
Text Label 3750 3400 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J10
U 1 1 5EEEAEEE
P 5850 2250
F 0 "J10" H 5878 2226 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5878 2135 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5850 2250 50  0001 C CNN
F 3 "~" H 5850 2250 50  0001 C CNN
	1    5850 2250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 5EEEAEF8
P 5850 2550
F 0 "J11" H 5878 2526 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5878 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5850 2550 50  0001 C CNN
F 3 "~" H 5850 2550 50  0001 C CNN
	1    5850 2550
	1    0    0    -1  
$EndComp
Text Label 5650 2550 2    50   ~ 0
+5V
Text Label 5650 2650 2    50   ~ 0
5VGND
Text Label 5650 2250 2    50   ~ 0
+12V
Text Label 5650 2350 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J12
U 1 1 5EEEAF06
P 5850 2950
F 0 "J12" H 5878 2926 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5878 2835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5850 2950 50  0001 C CNN
F 3 "~" H 5850 2950 50  0001 C CNN
	1    5850 2950
	1    0    0    -1  
$EndComp
Text Label 5650 2950 2    50   ~ 0
Green12V+
Text Label 5650 3050 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J19
U 1 1 5EEEEE17
P 7350 3400
F 0 "J19" H 7378 3376 50  0000 L CNN
F 1 "Conn_01x02_Female" H 7378 3285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7350 3400 50  0001 C CNN
F 3 "~" H 7350 3400 50  0001 C CNN
	1    7350 3400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J20
U 1 1 5EEEEE21
P 7350 3700
F 0 "J20" H 7378 3676 50  0000 L CNN
F 1 "Conn_01x02_Female" H 7378 3585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7350 3700 50  0001 C CNN
F 3 "~" H 7350 3700 50  0001 C CNN
	1    7350 3700
	1    0    0    -1  
$EndComp
Text Label 7150 3700 2    50   ~ 0
+5V
Text Label 7150 3800 2    50   ~ 0
5VGND
Text Label 7150 3400 2    50   ~ 0
+12V
Text Label 7150 3500 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J21
U 1 1 5EEEEE2F
P 7350 4100
F 0 "J21" H 7378 4076 50  0000 L CNN
F 1 "Conn_01x02_Female" H 7378 3985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7350 4100 50  0001 C CNN
F 3 "~" H 7350 4100 50  0001 C CNN
	1    7350 4100
	1    0    0    -1  
$EndComp
Text Label 7150 4100 2    50   ~ 0
Green12V+
Text Label 7150 4200 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J16
U 1 1 5EEEEE3B
P 7300 2300
F 0 "J16" H 7328 2276 50  0000 L CNN
F 1 "Conn_01x02_Female" H 7328 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7300 2300 50  0001 C CNN
F 3 "~" H 7300 2300 50  0001 C CNN
	1    7300 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J17
U 1 1 5EEEEE45
P 7300 2600
F 0 "J17" H 7328 2576 50  0000 L CNN
F 1 "Conn_01x02_Female" H 7328 2485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7300 2600 50  0001 C CNN
F 3 "~" H 7300 2600 50  0001 C CNN
	1    7300 2600
	1    0    0    -1  
$EndComp
Text Label 7100 2600 2    50   ~ 0
+5V
Text Label 7100 2700 2    50   ~ 0
5VGND
Text Label 7100 2300 2    50   ~ 0
+12V
Text Label 7100 2400 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J18
U 1 1 5EEEEE53
P 7300 3000
F 0 "J18" H 7328 2976 50  0000 L CNN
F 1 "Conn_01x02_Female" H 7328 2885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7300 3000 50  0001 C CNN
F 3 "~" H 7300 3000 50  0001 C CNN
	1    7300 3000
	1    0    0    -1  
$EndComp
Text Label 7100 3000 2    50   ~ 0
Green12V+
Text Label 7100 3100 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J25
U 1 1 5EEF4671
P 8800 3400
F 0 "J25" H 8828 3376 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8828 3285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8800 3400 50  0001 C CNN
F 3 "~" H 8800 3400 50  0001 C CNN
	1    8800 3400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J26
U 1 1 5EEF467B
P 8800 3700
F 0 "J26" H 8828 3676 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8828 3585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8800 3700 50  0001 C CNN
F 3 "~" H 8800 3700 50  0001 C CNN
	1    8800 3700
	1    0    0    -1  
$EndComp
Text Label 8600 3700 2    50   ~ 0
+5V
Text Label 8600 3800 2    50   ~ 0
5VGND
Text Label 8600 3400 2    50   ~ 0
+12V
Text Label 8600 3500 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J27
U 1 1 5EEF4689
P 8800 4100
F 0 "J27" H 8828 4076 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8828 3985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8800 4100 50  0001 C CNN
F 3 "~" H 8800 4100 50  0001 C CNN
	1    8800 4100
	1    0    0    -1  
$EndComp
Text Label 8600 4100 2    50   ~ 0
Green12V+
Text Label 8600 4200 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J22
U 1 1 5EEF4695
P 8750 2300
F 0 "J22" H 8778 2276 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8778 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8750 2300 50  0001 C CNN
F 3 "~" H 8750 2300 50  0001 C CNN
	1    8750 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J23
U 1 1 5EEF469F
P 8750 2600
F 0 "J23" H 8778 2576 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8778 2485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8750 2600 50  0001 C CNN
F 3 "~" H 8750 2600 50  0001 C CNN
	1    8750 2600
	1    0    0    -1  
$EndComp
Text Label 8550 2600 2    50   ~ 0
+5V
Text Label 8550 2700 2    50   ~ 0
5VGND
Text Label 8550 2300 2    50   ~ 0
+12V
Text Label 8550 2400 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J24
U 1 1 5EEF46AD
P 8750 3000
F 0 "J24" H 8778 2976 50  0000 L CNN
F 1 "Conn_01x02_Female" H 8778 2885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8750 3000 50  0001 C CNN
F 3 "~" H 8750 3000 50  0001 C CNN
	1    8750 3000
	1    0    0    -1  
$EndComp
Text Label 8550 3000 2    50   ~ 0
Green12V+
Text Label 8550 3100 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J31
U 1 1 5EEF46B9
P 10250 3450
F 0 "J31" H 10278 3426 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10278 3335 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10250 3450 50  0001 C CNN
F 3 "~" H 10250 3450 50  0001 C CNN
	1    10250 3450
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J32
U 1 1 5EEF46C3
P 10250 3750
F 0 "J32" H 10278 3726 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10278 3635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10250 3750 50  0001 C CNN
F 3 "~" H 10250 3750 50  0001 C CNN
	1    10250 3750
	1    0    0    -1  
$EndComp
Text Label 10050 3750 2    50   ~ 0
+5V
Text Label 10050 3850 2    50   ~ 0
5VGND
Text Label 10050 3450 2    50   ~ 0
+12V
Text Label 10050 3550 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J33
U 1 1 5EEF46D1
P 10250 4150
F 0 "J33" H 10278 4126 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10278 4035 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10250 4150 50  0001 C CNN
F 3 "~" H 10250 4150 50  0001 C CNN
	1    10250 4150
	1    0    0    -1  
$EndComp
Text Label 10050 4150 2    50   ~ 0
Green12V+
Text Label 10050 4250 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J28
U 1 1 5EEF46DD
P 10200 2350
F 0 "J28" H 10228 2326 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10228 2235 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10200 2350 50  0001 C CNN
F 3 "~" H 10200 2350 50  0001 C CNN
	1    10200 2350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J29
U 1 1 5EEF46E7
P 10200 2650
F 0 "J29" H 10228 2626 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10228 2535 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10200 2650 50  0001 C CNN
F 3 "~" H 10200 2650 50  0001 C CNN
	1    10200 2650
	1    0    0    -1  
$EndComp
Text Label 10000 2650 2    50   ~ 0
+5V
Text Label 10000 2750 2    50   ~ 0
5VGND
Text Label 10000 2350 2    50   ~ 0
+12V
Text Label 10000 2450 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J30
U 1 1 5EEF46F5
P 10200 3050
F 0 "J30" H 10228 3026 50  0000 L CNN
F 1 "Conn_01x02_Female" H 10228 2935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10200 3050 50  0001 C CNN
F 3 "~" H 10200 3050 50  0001 C CNN
	1    10200 3050
	1    0    0    -1  
$EndComp
Text Label 10000 3050 2    50   ~ 0
Green12V+
Text Label 10000 3150 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J37
U 1 1 5EF1B893
P 6050 5500
F 0 "J37" H 6078 5476 50  0000 L CNN
F 1 "Conn_01x02_Female" H 6078 5385 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6050 5500 50  0001 C CNN
F 3 "~" H 6050 5500 50  0001 C CNN
	1    6050 5500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J38
U 1 1 5EF1B89D
P 6050 5800
F 0 "J38" H 6078 5776 50  0000 L CNN
F 1 "Conn_01x02_Female" H 6078 5685 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6050 5800 50  0001 C CNN
F 3 "~" H 6050 5800 50  0001 C CNN
	1    6050 5800
	1    0    0    -1  
$EndComp
Text Label 5850 5800 2    50   ~ 0
+5V
Text Label 5850 5900 2    50   ~ 0
5VGND
Text Label 5850 5500 2    50   ~ 0
+12V
Text Label 5850 5600 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J39
U 1 1 5EF1B8AB
P 6050 6200
F 0 "J39" H 6078 6176 50  0000 L CNN
F 1 "Conn_01x02_Female" H 6078 6085 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6050 6200 50  0001 C CNN
F 3 "~" H 6050 6200 50  0001 C CNN
	1    6050 6200
	1    0    0    -1  
$EndComp
Text Label 5850 6200 2    50   ~ 0
Green12V+
Text Label 5850 6300 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J34
U 1 1 5EF1B8B7
P 6000 4400
F 0 "J34" H 6028 4376 50  0000 L CNN
F 1 "Conn_01x02_Female" H 6028 4285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6000 4400 50  0001 C CNN
F 3 "~" H 6000 4400 50  0001 C CNN
	1    6000 4400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J35
U 1 1 5EF1B8C1
P 6000 4700
F 0 "J35" H 6028 4676 50  0000 L CNN
F 1 "Conn_01x02_Female" H 6028 4585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6000 4700 50  0001 C CNN
F 3 "~" H 6000 4700 50  0001 C CNN
	1    6000 4700
	1    0    0    -1  
$EndComp
Text Label 5800 4700 2    50   ~ 0
+5V
Text Label 5800 4800 2    50   ~ 0
5VGND
Text Label 5800 4400 2    50   ~ 0
+12V
Text Label 5800 4500 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J36
U 1 1 5EF1B8CF
P 6000 5100
F 0 "J36" H 6028 5076 50  0000 L CNN
F 1 "Conn_01x02_Female" H 6028 4985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6000 5100 50  0001 C CNN
F 3 "~" H 6000 5100 50  0001 C CNN
	1    6000 5100
	1    0    0    -1  
$EndComp
Text Label 5800 5100 2    50   ~ 0
Green12V+
Text Label 5800 5200 2    50   ~ 0
GreenGND
$EndSCHEMATC
