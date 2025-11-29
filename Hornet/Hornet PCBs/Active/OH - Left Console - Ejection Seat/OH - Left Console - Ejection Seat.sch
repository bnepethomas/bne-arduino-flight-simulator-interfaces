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
Text Notes 900  1650 0    50   ~ 0
-- 1. Ejection Seat\n\ndefineEjectionHandleSwitch("EJECTION_HANDLE_SW", 7, 3008, 510, "Ejection Seat", "Ejection Control Handle")\ndefineToggleSwitch("EJECTION_SEAT_ARMED", 7, 3006, 511, "Ejection Seat", "Ejection Seat SAFE/ARMED Handle, SAFE/ARMED")\ndefineToggleSwitch("EJECTION_SEAT_MNL_OVRD", 7, 3007, 512, "Ejection Seat", "Ejection Seat Manual Override Handle, PULL/PUSH")\ndefineToggleSwitch("SHLDR_HARNESS_SW", 7, 3009, 513, "Ejection Seat", "Shoulder Harness Control Handle, LOCK/UNLOCK")\ndefineRockerSwitch("SEAT_HEIGHT_SW", 7, 3011, 3011, 3010, 3010, 514, "Ejection Seat", "Seat Height Adjustment Switch, UP/HOLD/DOWN")\ndefineToggleSwitch("HIDE_STICK_TOGGLE", 7, 3013, 575, "Ejection Seat", "Hide Stick toggle")
$Comp
L Connector:Conn_01x06_Female J1
U 1 1 5FC37DA5
P 1150 2850
F 0 "J1" H 1178 2826 50  0000 L CNN
F 1 "INPUTS" H 1178 2735 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 1150 2850 50  0001 C CNN
F 3 "~" H 1150 2850 50  0001 C CNN
	1    1150 2850
	-1   0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5FDFC6A7
P 2700 2600
F 0 "SW1" H 2700 2885 50  0000 C CNN
F 1 "EJECTION HANDLE" H 2700 2794 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2700 2800 50  0001 C CNN
F 3 "~" H 2700 2800 50  0001 C CNN
	1    2700 2600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW2
U 1 1 5FDFD1B2
P 2700 3200
F 0 "SW2" H 2700 3435 50  0000 C CNN
F 1 "SEAT ARMED" H 2700 3344 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2700 3200 50  0001 C CNN
F 3 "~" H 2700 3200 50  0001 C CNN
	1    2700 3200
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW3
U 1 1 5FDFDA32
P 2700 3800
F 0 "SW3" H 2700 4035 50  0000 C CNN
F 1 "MNL OVRD" H 2700 3944 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2700 3800 50  0001 C CNN
F 3 "~" H 2700 3800 50  0001 C CNN
	1    2700 3800
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW5
U 1 1 5FDFE364
P 4700 3150
F 0 "SW5" H 4700 3385 50  0000 C CNN
F 1 "SHLDR HARNESS" H 4700 3294 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4700 3150 50  0001 C CNN
F 3 "~" H 4700 3150 50  0001 C CNN
	1    4700 3150
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPDT_MSM SW4
U 1 1 5FDFE7F5
P 4700 2500
F 0 "SW4" H 4700 2785 50  0000 C CNN
F 1 "SEAT HEIGHT" H 4700 2694 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 4700 2500 50  0001 C CNN
F 3 "~" H 4700 2500 50  0001 C CNN
	1    4700 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:D D4
U 1 1 5FE003FE
P 4150 2500
F 0 "D4" H 4150 2716 50  0000 C CNN
F 1 "D" H 4150 2625 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 4150 2500 50  0001 C CNN
F 3 "~" H 4150 2500 50  0001 C CNN
	1    4150 2500
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D1
U 1 1 5FE00CA2
P 2100 2600
F 0 "D1" H 2100 2816 50  0000 C CNN
F 1 "D" H 2100 2725 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2100 2600 50  0001 C CNN
F 3 "~" H 2100 2600 50  0001 C CNN
	1    2100 2600
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D2
U 1 1 5FE01707
P 2100 3200
F 0 "D2" H 2100 3416 50  0000 C CNN
F 1 "D" H 2100 3325 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2100 3200 50  0001 C CNN
F 3 "~" H 2100 3200 50  0001 C CNN
	1    2100 3200
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D3
U 1 1 5FE01BA5
P 2100 3800
F 0 "D3" H 2100 4016 50  0000 C CNN
F 1 "D" H 2100 3925 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2100 3800 50  0001 C CNN
F 3 "~" H 2100 3800 50  0001 C CNN
	1    2100 3800
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D5
U 1 1 5FE0248B
P 4150 3150
F 0 "D5" H 4150 3366 50  0000 C CNN
F 1 "D" H 4150 3275 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 4150 3150 50  0001 C CNN
F 3 "~" H 4150 3150 50  0001 C CNN
	1    4150 3150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2250 2600 2500 2600
Wire Wire Line
	2250 3200 2500 3200
Wire Wire Line
	2250 3800 2500 3800
Wire Wire Line
	4500 2500 4300 2500
Wire Wire Line
	4500 3150 4300 3150
Wire Wire Line
	4000 2500 3850 2500
Wire Wire Line
	3850 2500 3850 3150
Wire Wire Line
	3850 3150 4000 3150
Wire Wire Line
	1950 2600 1800 2600
Wire Wire Line
	1800 2600 1800 2750
Wire Wire Line
	1800 3800 1950 3800
Wire Wire Line
	1950 3200 1800 3200
Connection ~ 1800 3200
Wire Wire Line
	1800 3200 1800 3800
Connection ~ 1800 2750
Wire Wire Line
	1800 2750 1800 3200
Text Label 1350 2650 0    50   ~ 0
Col1
Text Label 3850 2500 2    50   ~ 0
Col1
Text Label 4900 2600 0    50   ~ 0
Row2
Text Label 4900 3150 0    50   ~ 0
Row3
Text Label 2900 2600 0    50   ~ 0
Row1
Text Label 2900 3200 0    50   ~ 0
Row2
Text Label 2900 3800 0    50   ~ 0
Row3
Wire Wire Line
	1350 2750 1800 2750
Text Notes 4900 3900 0    50   ~ 0
add hide stick toggle
$Comp
L Mechanical:MountingHole H1
U 1 1 6174D020
P 4150 4250
F 0 "H1" H 4250 4296 50  0000 L CNN
F 1 "MountingHole" H 4250 4205 50  0000 L CNN
F 2 "MountingHole:MountingHole_4.3mm_M4" H 4150 4250 50  0001 C CNN
F 3 "~" H 4150 4250 50  0001 C CNN
	1    4150 4250
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 6174D7A8
P 4150 4450
F 0 "H2" H 4250 4496 50  0000 L CNN
F 1 "MountingHole" H 4250 4405 50  0000 L CNN
F 2 "MountingHole:MountingHole_4.3mm_M4" H 4150 4450 50  0001 C CNN
F 3 "~" H 4150 4450 50  0001 C CNN
	1    4150 4450
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 6174DF62
P 4150 4650
F 0 "H3" H 4250 4696 50  0000 L CNN
F 1 "MountingHole" H 4250 4605 50  0000 L CNN
F 2 "MountingHole:MountingHole_4.3mm_M4" H 4150 4650 50  0001 C CNN
F 3 "~" H 4150 4650 50  0001 C CNN
	1    4150 4650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 6174DF6C
P 4150 4850
F 0 "H4" H 4250 4896 50  0000 L CNN
F 1 "MountingHole" H 4250 4805 50  0000 L CNN
F 2 "MountingHole:MountingHole_4.3mm_M4" H 4150 4850 50  0001 C CNN
F 3 "~" H 4150 4850 50  0001 C CNN
	1    4150 4850
	1    0    0    -1  
$EndComp
Text Label 1350 2850 0    50   ~ 0
Row1
Text Label 1350 2950 0    50   ~ 0
Row2
Text Label 1350 3050 0    50   ~ 0
Row3
NoConn ~ 1350 3150
Text Label 4900 2400 0    50   ~ 0
Row1
$EndSCHEMATC
