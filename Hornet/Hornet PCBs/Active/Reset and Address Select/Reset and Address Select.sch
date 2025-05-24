EESchema Schematic File Version 4
LIBS:Reset and Address Select-cache
EELAYER 29 0
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
L Device:R_Small R1
U 1 1 5E5D8064
P 2900 1850
F 0 "R1" H 2959 1896 50  0000 L CNN
F 1 "R_Small" H 2959 1805 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" H 2900 1850 50  0001 C CNN
F 3 "~" H 2900 1850 50  0001 C CNN
	1    2900 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C1
U 1 1 5E5D8757
P 2900 2350
F 0 "C1" H 3018 2396 50  0000 L CNN
F 1 "CP" H 3018 2305 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.00mm" H 2938 2200 50  0001 C CNN
F 3 "~" H 2900 2350 50  0001 C CNN
	1    2900 2350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5E5D8279
P 1450 1600
F 0 "J1" H 1558 1981 50  0000 C CNN
F 1 "Conn_01x06_Male" H 1558 1890 50  0000 C CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_1x09_P2.00mm_Horizontal" H 1450 1600 50  0001 C CNN
F 3 "~" H 1450 1600 50  0001 C CNN
	1    1450 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 1400 2900 1400
Wire Wire Line
	2900 1400 2900 1750
Wire Wire Line
	2900 1950 2900 2200
Wire Wire Line
	2900 2500 1900 2500
Wire Wire Line
	1900 2500 1900 1700
Wire Wire Line
	1900 1700 1650 1700
$EndSCHEMATC
