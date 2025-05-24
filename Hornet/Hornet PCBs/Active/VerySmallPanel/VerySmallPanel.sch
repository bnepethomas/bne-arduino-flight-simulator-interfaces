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
L Diode:1N4148 D1
U 1 1 5EEFED0C
P 3500 2000
F 0 "D1" H 3500 1784 50  0000 C CNN
F 1 "1N4148" H 3500 1875 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" H 3500 1825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 3500 2000 50  0001 C CNN
	1    3500 2000
	-1   0    0    1   
$EndComp
$Comp
L Diode:1N4148 D2
U 1 1 5EEFF5AA
P 3500 2300
F 0 "D2" H 3500 2084 50  0000 C CNN
F 1 "1N4148" H 3500 2175 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" H 3500 2125 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 3500 2300 50  0001 C CNN
	1    3500 2300
	-1   0    0    1   
$EndComp
$Comp
L Diode:1N4148 D3
U 1 1 5EF01EF3
P 3500 2600
F 0 "D3" H 3500 2384 50  0000 C CNN
F 1 "1N4148" H 3500 2475 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" H 3500 2425 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 3500 2600 50  0001 C CNN
	1    3500 2600
	-1   0    0    1   
$EndComp
$Comp
L Diode:1N4148 D4
U 1 1 5EF033E0
P 3500 2900
F 0 "D4" H 3500 2684 50  0000 C CNN
F 1 "1N4148" H 3500 2775 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" H 3500 2725 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 3500 2900 50  0001 C CNN
	1    3500 2900
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 5EF05497
P 4200 1400
F 0 "J4" H 4228 1376 50  0000 L CNN
F 1 "SwitchRow" H 4228 1285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4200 1400 50  0001 C CNN
F 3 "~" H 4200 1400 50  0001 C CNN
	1    4200 1400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 5EF08C7F
P 3150 3500
F 0 "J3" H 3178 3476 50  0000 L CNN
F 1 "BackLight" H 3178 3385 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3150 3500 50  0001 C CNN
F 3 "~" H 3150 3500 50  0001 C CNN
	1    3150 3500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 5EF0A654
P 2350 3500
F 0 "J1" H 2242 3685 50  0000 C CNN
F 1 "BackLight" H 2242 3594 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2350 3500 50  0001 C CNN
F 3 "~" H 2350 3500 50  0001 C CNN
	1    2350 3500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2950 3500 2550 3500
Wire Wire Line
	2950 3600 2550 3600
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 5EF0B9E6
P 2400 2000
F 0 "J2" H 2292 2185 50  0000 C CNN
F 1 "SwitchCol" H 2292 2094 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2400 2000 50  0001 C CNN
F 3 "~" H 2400 2000 50  0001 C CNN
	1    2400 2000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3350 2000 2950 2000
Wire Wire Line
	2600 2100 2800 2100
Wire Wire Line
	3150 2100 3150 2300
Wire Wire Line
	3150 2300 3350 2300
Wire Wire Line
	2950 2000 2950 2600
Wire Wire Line
	2950 2600 3350 2600
Connection ~ 2950 2000
Wire Wire Line
	2950 2000 2600 2000
Wire Wire Line
	2800 2100 2800 2900
Wire Wire Line
	2800 2900 3350 2900
Connection ~ 2800 2100
Wire Wire Line
	2800 2100 3150 2100
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 5EF0DF75
P 5300 2900
F 0 "J5" H 5328 2876 50  0000 L CNN
F 1 "Conn_01x04_Female" H 5328 2785 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 5300 2900 50  0001 C CNN
F 3 "~" H 5300 2900 50  0001 C CNN
	1    5300 2900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 5EF0E5D4
P 5700 2900
F 0 "J6" H 5728 2876 50  0000 L CNN
F 1 "Conn_01x04_Female" H 5728 2785 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 5700 2900 50  0001 C CNN
F 3 "~" H 5700 2900 50  0001 C CNN
	1    5700 2900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J7
U 1 1 5EF0F421
P 6050 2900
F 0 "J7" H 6078 2876 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6078 2785 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 6050 2900 50  0001 C CNN
F 3 "~" H 6050 2900 50  0001 C CNN
	1    6050 2900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 5EF14483
P 5200 3600
F 0 "H1" H 5300 3646 50  0000 L CNN
F 1 "MountingHole" H 5300 3555 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 5200 3600 50  0001 C CNN
F 3 "~" H 5200 3600 50  0001 C CNN
	1    5200 3600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5EF144D9
P 5200 3850
F 0 "H2" H 5300 3896 50  0000 L CNN
F 1 "MountingHole" H 5300 3805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 5200 3850 50  0001 C CNN
F 3 "~" H 5200 3850 50  0001 C CNN
	1    5200 3850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 5EF166E8
P 4550 2000
F 0 "J8" H 4578 1976 50  0000 L CNN
F 1 "SW1" H 4578 1885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4550 2000 50  0001 C CNN
F 3 "~" H 4550 2000 50  0001 C CNN
	1    4550 2000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 5EF19880
P 4550 2900
F 0 "J11" H 4578 2876 50  0000 L CNN
F 1 "SW4" H 4578 2785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4550 2900 50  0001 C CNN
F 3 "~" H 4550 2900 50  0001 C CNN
	1    4550 2900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 5EF1A50A
P 4550 2300
F 0 "J9" H 4578 2276 50  0000 L CNN
F 1 "SW2" H 4578 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4550 2300 50  0001 C CNN
F 3 "~" H 4550 2300 50  0001 C CNN
	1    4550 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J10
U 1 1 5EF1AC89
P 4550 2600
F 0 "J10" H 4578 2576 50  0000 L CNN
F 1 "SW3" H 4578 2485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4550 2600 50  0001 C CNN
F 3 "~" H 4550 2600 50  0001 C CNN
	1    4550 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 2000 4350 2000
Wire Wire Line
	3650 2300 4350 2300
Wire Wire Line
	3650 2600 4350 2600
Wire Wire Line
	3650 2900 4350 2900
Wire Wire Line
	4000 1400 3850 1400
Wire Wire Line
	3850 1400 3850 2100
Wire Wire Line
	4350 2100 3850 2100
Wire Wire Line
	3850 2100 3850 2400
Wire Wire Line
	3850 2400 4350 2400
Connection ~ 3850 2100
Wire Wire Line
	4000 1500 4000 2700
Wire Wire Line
	4000 3000 4350 3000
Wire Wire Line
	4350 2700 4000 2700
Connection ~ 4000 2700
Wire Wire Line
	4000 2700 4000 3000
$EndSCHEMATC
