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
L PT_Symbol_Library_v001:PT_Tactile_Switch_Led SW1
U 1 1 5A7BCF73
P 6200 1650
F 0 "SW1" H 6250 1750 50  0000 L CNN
F 1 "push" H 6200 1590 50  0000 C CNN
F 2 "PT_Library_v001:PT_Small_Tactile_Switch_With_LED" H 6200 1850 50  0001 C CNN
F 3 "" H 6200 1850 50  0001 C CNN
F 4 "GX06400" H 6200 1650 60  0001 C CNN "PN"
	1    6200 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5A7BDB8B
P 4150 2250
F 0 "D1" H 4150 2350 50  0000 C CNN
F 1 "LED" H 4150 2150 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 4150 2250 50  0001 C CNN
F 3 "http://inolux-corp.com/datasheet/SMDLED/Mono%20Color%20Top%20View/IN-S63AT%20Series_V1.1.pdf" H 4150 2250 50  0001 C CNN
F 4 "IN-S63AT" H 4150 2250 60  0001 C CNN "PN"
	1    4150 2250
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 5A7BDE34
P 4150 2550
F 0 "D2" H 4150 2650 50  0000 C CNN
F 1 "LED" H 4150 2450 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 4150 2550 50  0001 C CNN
F 3 "http://inolux-corp.com/datasheet/SMDLED/Mono%20Color%20Top%20View/IN-S63AT%20Series_V1.1.pdf" H 4150 2550 50  0001 C CNN
F 4 "IN-S63AT" H 4150 2550 60  0001 C CNN "PN"
	1    4150 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D3
U 1 1 5A7BDE66
P 4150 2850
F 0 "D3" H 4150 2950 50  0000 C CNN
F 1 "LED" H 4150 2750 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 4150 2850 50  0001 C CNN
F 3 "http://inolux-corp.com/datasheet/SMDLED/Mono%20Color%20Top%20View/IN-S63AT%20Series_V1.1.pdf" H 4150 2850 50  0001 C CNN
F 4 "IN-S63AT" H 4150 2850 60  0001 C CNN "PN"
	1    4150 2850
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D4
U 1 1 5A7BDE93
P 4150 3150
F 0 "D4" H 4150 3250 50  0000 C CNN
F 1 "LED" H 4150 3050 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 4150 3150 50  0001 C CNN
F 3 "http://inolux-corp.com/datasheet/SMDLED/Mono%20Color%20Top%20View/IN-S63AT%20Series_V1.1.pdf" H 4150 3150 50  0001 C CNN
F 4 "IN-S63AT" H 4150 3150 60  0001 C CNN "PN"
	1    4150 3150
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 5EE5A9D9
P 5050 1300
F 0 "J3" H 4942 1485 50  0000 C CNN
F 1 "SwitchCol" H 4942 1394 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5050 1300 50  0001 C CNN
F 3 "~" H 5050 1300 50  0001 C CNN
	1    5050 1300
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 5EE5B359
P 5050 1750
F 0 "J4" H 4942 1935 50  0000 C CNN
F 1 "SwitchRow" H 4942 1844 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5050 1750 50  0001 C CNN
F 3 "~" H 5050 1750 50  0001 C CNN
	1    5050 1750
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 5EE5BA11
P 2850 3450
F 0 "J2" H 2742 3635 50  0000 C CNN
F 1 "LedCathode" H 2742 3544 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2850 3450 50  0001 C CNN
F 3 "~" H 2850 3450 50  0001 C CNN
	1    2850 3450
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 5EE5C1EE
P 2850 2600
F 0 "J1" H 2742 2785 50  0000 C CNN
F 1 "LedAnode" H 2742 2694 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2850 2600 50  0001 C CNN
F 3 "~" H 2850 2600 50  0001 C CNN
	1    2850 2600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5850 1350 5850 1450
Wire Wire Line
	6400 1550 6400 1400
Wire Wire Line
	6400 1400 6550 1400
Wire Wire Line
	6550 1400 6550 1100
Wire Wire Line
	6550 1100 5650 1100
Wire Wire Line
	5650 1100 5650 1750
Wire Wire Line
	5650 1750 5250 1750
Connection ~ 6400 1400
Wire Wire Line
	6400 1400 6400 1350
Wire Wire Line
	4300 2250 4300 2400
Wire Wire Line
	4300 2850 4300 3000
Wire Wire Line
	3050 3450 4450 3450
Wire Wire Line
	4450 3450 4450 3000
Wire Wire Line
	4450 3000 4300 3000
Connection ~ 4300 3000
Wire Wire Line
	4300 3000 4300 3150
Wire Wire Line
	4300 2400 4600 2400
Wire Wire Line
	4600 2400 4600 3550
Wire Wire Line
	4600 3550 3050 3550
Connection ~ 4300 2400
Wire Wire Line
	4300 2400 4300 2550
Wire Wire Line
	3550 2250 3550 2600
Wire Wire Line
	3550 2600 3050 2600
Wire Wire Line
	4000 2550 3700 2550
Wire Wire Line
	3700 2550 3700 2700
Wire Wire Line
	3550 2250 3800 2250
Wire Wire Line
	3800 2250 3800 2850
Wire Wire Line
	3800 2850 4000 2850
Connection ~ 3800 2250
Wire Wire Line
	3800 2250 4000 2250
Wire Wire Line
	3700 2700 3550 2700
Wire Wire Line
	3550 2700 3550 3150
Wire Wire Line
	3550 3150 4000 3150
Connection ~ 3550 2700
Wire Wire Line
	3550 2700 3050 2700
$Comp
L Diode:1N4148 D5
U 1 1 5EEF607D
P 5400 1300
F 0 "D5" H 5400 1084 50  0000 C CNN
F 1 "1N4148" H 5400 1175 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5400 1125 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 5400 1300 50  0001 C CNN
	1    5400 1300
	-1   0    0    1   
$EndComp
Wire Wire Line
	5550 1300 5550 1450
Wire Wire Line
	5550 1450 5850 1450
Connection ~ 5850 1450
Wire Wire Line
	5850 1450 5850 1550
$Comp
L Mechanical:MountingHole H1
U 1 1 5EEECC41
P 8050 3550
F 0 "H1" H 8150 3596 50  0000 L CNN
F 1 "MountingHole" H 8150 3505 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 8050 3550 50  0001 C CNN
F 3 "~" H 8050 3550 50  0001 C CNN
	1    8050 3550
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5EEED081
P 8050 3800
F 0 "H2" H 8150 3846 50  0000 L CNN
F 1 "MountingHole" H 8150 3755 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 8050 3800 50  0001 C CNN
F 3 "~" H 8050 3800 50  0001 C CNN
	1    8050 3800
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5EEED377
P 8050 4050
F 0 "H3" H 8150 4096 50  0000 L CNN
F 1 "MountingHole" H 8150 4005 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8050 4050 50  0001 C CNN
F 3 "~" H 8050 4050 50  0001 C CNN
	1    8050 4050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5EEED746
P 8050 4350
F 0 "H4" H 8150 4396 50  0000 L CNN
F 1 "MountingHole" H 8150 4305 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 8050 4350 50  0001 C CNN
F 3 "~" H 8050 4350 50  0001 C CNN
	1    8050 4350
	1    0    0    -1  
$EndComp
$EndSCHEMATC
