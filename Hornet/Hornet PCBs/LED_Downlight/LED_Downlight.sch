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
L Device:LED D1
U 1 1 5E9F5CDB
P 3400 2800
F 0 "D1" H 3393 3016 50  0000 C CNN
F 1 "LED" H 3393 2925 50  0000 C CNN
F 2 "PT_Library_v001:LED_D8.0mm-LargePins" H 3400 2800 50  0001 C CNN
F 3 "~" H 3400 2800 50  0001 C CNN
	1    3400 2800
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 5E9F6574
P 4150 2800
F 0 "D2" H 4143 3016 50  0000 C CNN
F 1 "LED" H 4143 2925 50  0000 C CNN
F 2 "PT_Library_v001:LED_D8.0mm-LargePins" H 4150 2800 50  0001 C CNN
F 3 "~" H 4150 2800 50  0001 C CNN
	1    4150 2800
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 5E9F70E1
P 4900 2800
F 0 "D3" H 4893 3016 50  0000 C CNN
F 1 "LED" H 4893 2925 50  0000 C CNN
F 2 "PT_Library_v001:LED_D8.0mm-LargePins" H 4900 2800 50  0001 C CNN
F 3 "~" H 4900 2800 50  0001 C CNN
	1    4900 2800
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_US R1
U 1 1 5E9F83C0
P 2500 2800
F 0 "R1" V 2705 2800 50  0000 C CNN
F 1 "R_US" V 2614 2800 50  0000 C CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2540 2790 50  0001 C CNN
F 3 "~" H 2500 2800 50  0001 C CNN
	1    2500 2800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2650 2800 3250 2800
Wire Wire Line
	3550 2800 4000 2800
Wire Wire Line
	4300 2800 4750 2800
Wire Wire Line
	5000 2800 5050 2800
$Comp
L Device:LED D4
U 1 1 5EE55F24
P 3350 4550
F 0 "D4" H 3343 4766 50  0000 C CNN
F 1 "LED" H 3343 4675 50  0000 C CNN
F 2 "PT_Library_v001:LED_D8.0mm-LargePins" H 3350 4550 50  0001 C CNN
F 3 "~" H 3350 4550 50  0001 C CNN
	1    3350 4550
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D5
U 1 1 5EE55F2E
P 4100 4550
F 0 "D5" H 4093 4766 50  0000 C CNN
F 1 "LED" H 4093 4675 50  0000 C CNN
F 2 "PT_Library_v001:LED_D8.0mm-LargePins" H 4100 4550 50  0001 C CNN
F 3 "~" H 4100 4550 50  0001 C CNN
	1    4100 4550
	-1   0    0    -1  
$EndComp
$Comp
L Device:LED D6
U 1 1 5EE55F38
P 4850 4550
F 0 "D6" H 4843 4766 50  0000 C CNN
F 1 "LED" H 4843 4675 50  0000 C CNN
F 2 "PT_Library_v001:LED_D8.0mm-LargePins" H 4850 4550 50  0001 C CNN
F 3 "~" H 4850 4550 50  0001 C CNN
	1    4850 4550
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_US R2
U 1 1 5EE55F42
P 2450 4550
F 0 "R2" V 2655 4550 50  0000 C CNN
F 1 "R_US" V 2564 4550 50  0000 C CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2490 4540 50  0001 C CNN
F 3 "~" H 2450 4550 50  0001 C CNN
	1    2450 4550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2600 4550 3200 4550
Wire Wire Line
	3500 4550 3950 4550
Wire Wire Line
	4250 4550 4700 4550
Wire Wire Line
	4950 4550 5000 4550
$Comp
L Connector:Conn_01x04_Female J1
U 1 1 5EE5878F
P 6600 3250
F 0 "J1" H 6628 3226 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6628 3135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Horizontal" H 6600 3250 50  0001 C CNN
F 3 "~" H 6600 3250 50  0001 C CNN
	1    6600 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J2
U 1 1 5EE59171
P 6650 4250
F 0 "J2" H 6678 4226 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6678 4135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Horizontal" H 6650 4250 50  0001 C CNN
F 3 "~" H 6650 4250 50  0001 C CNN
	1    6650 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 2800 2350 3150
Wire Wire Line
	2350 3150 5700 3150
Wire Wire Line
	5050 2800 5250 2800
Wire Wire Line
	5250 2800 5250 3250
Wire Wire Line
	5250 3250 5950 3250
Connection ~ 5050 2800
Wire Wire Line
	6450 4350 6050 4350
Wire Wire Line
	6400 3450 6150 3450
Wire Wire Line
	6150 3450 6150 4450
Wire Wire Line
	6150 4450 6450 4450
Wire Wire Line
	6050 3350 6050 4350
Wire Wire Line
	5950 3250 5950 4250
Wire Wire Line
	5950 4250 6450 4250
Connection ~ 5950 3250
Wire Wire Line
	5950 3250 6400 3250
Wire Wire Line
	5700 3150 5700 4150
Wire Wire Line
	5700 4150 6450 4150
Connection ~ 5700 3150
Wire Wire Line
	5700 3150 6400 3150
Wire Wire Line
	2300 4550 2300 3350
Wire Wire Line
	2300 3350 6050 3350
Connection ~ 6050 3350
Wire Wire Line
	6050 3350 6400 3350
Wire Wire Line
	5000 4550 5000 3450
Wire Wire Line
	5000 3450 6150 3450
Connection ~ 5000 4550
Connection ~ 6150 3450
$Comp
L Mechanical:MountingHole H1
U 1 1 5EE5B363
P 8200 2800
F 0 "H1" H 8300 2846 50  0000 L CNN
F 1 "MountingHole" H 8300 2755 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8200 2800 50  0001 C CNN
F 3 "~" H 8200 2800 50  0001 C CNN
	1    8200 2800
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5EE5B85B
P 8200 3100
F 0 "H2" H 8300 3146 50  0000 L CNN
F 1 "MountingHole" H 8300 3055 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8200 3100 50  0001 C CNN
F 3 "~" H 8200 3100 50  0001 C CNN
	1    8200 3100
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5EE5BD02
P 8200 3350
F 0 "H3" H 8300 3396 50  0000 L CNN
F 1 "MountingHole" H 8300 3305 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8200 3350 50  0001 C CNN
F 3 "~" H 8200 3350 50  0001 C CNN
	1    8200 3350
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5EE5C31B
P 8200 3600
F 0 "H4" H 8300 3646 50  0000 L CNN
F 1 "MountingHole" H 8300 3555 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8200 3600 50  0001 C CNN
F 3 "~" H 8200 3600 50  0001 C CNN
	1    8200 3600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H5
U 1 1 5EE5F4D4
P 8200 3850
F 0 "H5" H 8300 3896 50  0000 L CNN
F 1 "MountingHole" H 8300 3805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8200 3850 50  0001 C CNN
F 3 "~" H 8200 3850 50  0001 C CNN
	1    8200 3850
	1    0    0    -1  
$EndComp
Text Label 2900 3350 0    50   ~ 0
+12VWhite
Text Label 5100 3450 0    50   ~ 0
GNDWhite
Text Label 3400 3150 0    50   ~ 0
+12VGreen
Text Label 5350 3250 0    50   ~ 0
GNDGreen
$EndSCHEMATC
