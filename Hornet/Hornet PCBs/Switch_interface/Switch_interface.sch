EESchema Schematic File Version 4
LIBS:Switcih_interface-cache
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
L Connector_Generic:Conn_01x36 J1
U 1 1 5D25A90C
P 2050 3550
F 0 "J1" H 2100 4567 50  0000 C CNN
F 1 "Conn_02x18_Odd_Even" H 2100 4476 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x18_P2.54mm_Vertical" H 2050 3550 50  0001 C CNN
F 3 "~" H 2050 3550 50  0001 C CNN
	1    2050 3550
	-1   0    0    -1  
$EndComp
Text Label 2250 3750 0    50   ~ 0
Col1
Text Label 2250 3850 0    50   ~ 0
Col2
Text Label 2250 3950 0    50   ~ 0
Col3
Text Label 2250 4050 0    50   ~ 0
Col4
Text Label 2250 4150 0    50   ~ 0
Col5
Text Label 2250 4250 0    50   ~ 0
Col6
Text Label 2250 4350 0    50   ~ 0
Col7
Text Label 2250 4450 0    50   ~ 0
Col8
Text Label 2250 4550 0    50   ~ 0
Col9
Text Label 2250 4650 0    50   ~ 0
Col10
Text Label 2250 2050 0    50   ~ 0
Row0
Text Label 2250 2150 0    50   ~ 0
Row1
Text Label 2250 2250 0    50   ~ 0
Row2
Text Label 2250 2450 0    50   ~ 0
Row4
Text Label 2250 2550 0    50   ~ 0
Row5
Text Label 2250 2650 0    50   ~ 0
Row6
Text Label 2250 2750 0    50   ~ 0
Row7
Text Label 2250 2850 0    50   ~ 0
Row8
Text Label 2250 2950 0    50   ~ 0
Row9
Text Label 2250 3050 0    50   ~ 0
Row10
Text Label 2250 3150 0    50   ~ 0
Row11
Text Label 2250 3250 0    50   ~ 0
Row12
Text Label 2250 3350 0    50   ~ 0
Row13
Text Notes 6600 1000 0    50   ~ 0
digitalPin 22~ 37 used as row0 ~ row 15, \ndigital pin 38~53 used as column 0 ~ 15,\nWhen an Ethernet shield is connected Columns 13 and 14 are pulled up to 1.5V - so do not use\nIt's a 16 * 14  matrix (due to loss of two columns) \n
Wire Wire Line
	2250 5250 2250 5350
Text Label 2250 2350 0    50   ~ 0
Row3
Text Label 2250 3450 0    50   ~ 0
Row14
Text Label 2250 3550 0    50   ~ 0
Row15
NoConn ~ 2250 5150
NoConn ~ 2250 5050
NoConn ~ 2250 4950
NoConn ~ 2250 4750
NoConn ~ 2250 4850
Text Label 2250 3650 0    50   ~ 0
Col0
$Comp
L Connector_Generic:Conn_01x34 J2
U 1 1 5D265C93
P 3100 3600
F 0 "J2" H 3150 4617 50  0000 C CNN
F 1 "Conn_02x18_Odd_Even" H 3150 4526 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x17_P2.54mm_Vertical" H 3100 3600 50  0001 C CNN
F 3 "~" H 3100 3600 50  0001 C CNN
	1    3100 3600
	-1   0    0    -1  
$EndComp
Text Label 3300 3800 0    50   ~ 0
Col1
Text Label 3300 3900 0    50   ~ 0
Col2
Text Label 3300 4000 0    50   ~ 0
Col3
Text Label 3300 4100 0    50   ~ 0
Col4
Text Label 3300 4200 0    50   ~ 0
Col5
Text Label 3300 4300 0    50   ~ 0
Col6
Text Label 3300 4400 0    50   ~ 0
Col7
Text Label 3300 4500 0    50   ~ 0
Col8
Text Label 3300 4600 0    50   ~ 0
Col9
Text Label 3300 4700 0    50   ~ 0
Col10
Text Label 3300 2100 0    50   ~ 0
Row0
Text Label 3300 2200 0    50   ~ 0
Row1
Text Label 3300 2300 0    50   ~ 0
Row2
Text Label 3300 2500 0    50   ~ 0
Row4
Text Label 3300 2600 0    50   ~ 0
Row5
Text Label 3300 2700 0    50   ~ 0
Row6
Text Label 3300 2800 0    50   ~ 0
Row7
Text Label 3300 2900 0    50   ~ 0
Row8
Text Label 3300 3000 0    50   ~ 0
Row9
Text Label 3300 3100 0    50   ~ 0
Row10
Text Label 3300 3200 0    50   ~ 0
Row11
Text Label 3300 3300 0    50   ~ 0
Row12
Text Label 3300 3400 0    50   ~ 0
Row13
Text Label 3300 2400 0    50   ~ 0
Row3
Text Label 3300 3500 0    50   ~ 0
Row14
Text Label 3300 3600 0    50   ~ 0
Row15
NoConn ~ 3300 5200
NoConn ~ 3300 5100
NoConn ~ 3300 5000
NoConn ~ 3300 4800
NoConn ~ 3300 4900
Text Label 3300 3700 0    50   ~ 0
Col0
$Comp
L Connector_Generic:Conn_01x34 J3
U 1 1 5D272068
P 4050 3600
F 0 "J3" H 4100 4617 50  0000 C CNN
F 1 "Conn_02x18_Odd_Even" H 4100 4526 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x17_P2.54mm_Vertical" H 4050 3600 50  0001 C CNN
F 3 "~" H 4050 3600 50  0001 C CNN
	1    4050 3600
	-1   0    0    -1  
$EndComp
Text Label 4250 3800 0    50   ~ 0
Col1
Text Label 4250 3900 0    50   ~ 0
Col2
Text Label 4250 4000 0    50   ~ 0
Col3
Text Label 4250 4100 0    50   ~ 0
Col4
Text Label 4250 4200 0    50   ~ 0
Col5
Text Label 4250 4300 0    50   ~ 0
Col6
Text Label 4250 4400 0    50   ~ 0
Col7
Text Label 4250 4500 0    50   ~ 0
Col8
Text Label 4250 4600 0    50   ~ 0
Col9
Text Label 4250 4700 0    50   ~ 0
Col10
Text Label 4250 2100 0    50   ~ 0
Row0
Text Label 4250 2200 0    50   ~ 0
Row1
Text Label 4250 2300 0    50   ~ 0
Row2
Text Label 4250 2500 0    50   ~ 0
Row4
Text Label 4250 2600 0    50   ~ 0
Row5
Text Label 4250 2700 0    50   ~ 0
Row6
Text Label 4250 2800 0    50   ~ 0
Row7
Text Label 4250 2900 0    50   ~ 0
Row8
Text Label 4250 3000 0    50   ~ 0
Row9
Text Label 4250 3100 0    50   ~ 0
Row10
Text Label 4250 3200 0    50   ~ 0
Row11
Text Label 4250 3300 0    50   ~ 0
Row12
Text Label 4250 3400 0    50   ~ 0
Row13
Text Label 4250 2400 0    50   ~ 0
Row3
Text Label 4250 3500 0    50   ~ 0
Row14
Text Label 4250 3600 0    50   ~ 0
Row15
NoConn ~ 4250 5200
NoConn ~ 4250 5100
NoConn ~ 4250 5000
NoConn ~ 4250 4800
NoConn ~ 4250 4900
Text Label 4250 3700 0    50   ~ 0
Col0
Text Label 5750 3500 0    50   ~ 0
Row15
Text Label 5750 3400 0    50   ~ 0
Row14
Text Label 5750 2300 0    50   ~ 0
Row3
Text Label 5750 3300 0    50   ~ 0
Row13
Text Label 5750 3200 0    50   ~ 0
Row12
Text Label 5750 3100 0    50   ~ 0
Row11
Text Label 5750 3000 0    50   ~ 0
Row10
Text Label 5750 2900 0    50   ~ 0
Row9
Text Label 5750 2800 0    50   ~ 0
Row8
Text Label 5750 2700 0    50   ~ 0
Row7
Text Label 5750 2600 0    50   ~ 0
Row6
Text Label 5750 2500 0    50   ~ 0
Row5
Text Label 5750 2400 0    50   ~ 0
Row4
Text Label 5750 2200 0    50   ~ 0
Row2
Text Label 5750 2100 0    50   ~ 0
Row1
Text Label 5750 2000 0    50   ~ 0
Row0
Text Label 5850 4350 0    50   ~ 0
Col0
Text Label 5850 5350 0    50   ~ 0
Col10
Text Label 5850 5250 0    50   ~ 0
Col9
Text Label 5850 5150 0    50   ~ 0
Col8
Text Label 5850 5050 0    50   ~ 0
Col7
Text Label 5850 4950 0    50   ~ 0
Col6
Text Label 5850 4850 0    50   ~ 0
Col5
Text Label 5850 4750 0    50   ~ 0
Col4
Text Label 5850 4650 0    50   ~ 0
Col3
Text Label 5850 4550 0    50   ~ 0
Col2
Text Label 5850 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J7
U 1 1 5D282A4A
P 5650 4850
F 0 "J7" H 5730 4842 50  0000 L CNN
F 1 "Conn_01x13" H 5730 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 5650 4850 50  0001 C CNN
F 3 "~" H 5650 4850 50  0001 C CNN
	1    5650 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J6
U 1 1 5D282A40
P 5550 2700
F 0 "J6" H 5630 2692 50  0000 L CNN
F 1 "Conn_01x13" H 5630 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 5550 2700 50  0001 C CNN
F 3 "~" H 5550 2700 50  0001 C CNN
	1    5550 2700
	-1   0    0    -1  
$EndComp
Text Label 5150 3500 0    50   ~ 0
Row15
Text Label 5150 3400 0    50   ~ 0
Row14
Text Label 5150 2300 0    50   ~ 0
Row3
Text Label 5150 3300 0    50   ~ 0
Row13
Text Label 5150 3200 0    50   ~ 0
Row12
Text Label 5150 3100 0    50   ~ 0
Row11
Text Label 5150 3000 0    50   ~ 0
Row10
Text Label 5150 2900 0    50   ~ 0
Row9
Text Label 5150 2800 0    50   ~ 0
Row8
Text Label 5150 2700 0    50   ~ 0
Row7
Text Label 5150 2600 0    50   ~ 0
Row6
Text Label 5150 2500 0    50   ~ 0
Row5
Text Label 5150 2400 0    50   ~ 0
Row4
Text Label 5150 2200 0    50   ~ 0
Row2
Text Label 5150 2100 0    50   ~ 0
Row1
Text Label 5150 2000 0    50   ~ 0
Row0
Text Label 5250 4350 0    50   ~ 0
Col0
Text Label 5250 5350 0    50   ~ 0
Col10
Text Label 5250 5250 0    50   ~ 0
Col9
Text Label 5250 5150 0    50   ~ 0
Col8
Text Label 5250 5050 0    50   ~ 0
Col7
Text Label 5250 4950 0    50   ~ 0
Col6
Text Label 5250 4850 0    50   ~ 0
Col5
Text Label 5250 4750 0    50   ~ 0
Col4
Text Label 5250 4650 0    50   ~ 0
Col3
Text Label 5250 4550 0    50   ~ 0
Col2
Text Label 5250 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J5
U 1 1 5D27653E
P 5050 4850
F 0 "J5" H 5130 4842 50  0000 L CNN
F 1 "Conn_01x13" H 5130 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 5050 4850 50  0001 C CNN
F 3 "~" H 5050 4850 50  0001 C CNN
	1    5050 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J4
U 1 1 5D2649E7
P 4950 2700
F 0 "J4" H 5030 2692 50  0000 L CNN
F 1 "Conn_01x13" H 5030 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 4950 2700 50  0001 C CNN
F 3 "~" H 4950 2700 50  0001 C CNN
	1    4950 2700
	-1   0    0    -1  
$EndComp
Text Label 7250 3500 0    50   ~ 0
Row15
Text Label 7250 3400 0    50   ~ 0
Row14
Text Label 7250 2300 0    50   ~ 0
Row3
Text Label 7250 3300 0    50   ~ 0
Row13
Text Label 7250 3200 0    50   ~ 0
Row12
Text Label 7250 3100 0    50   ~ 0
Row11
Text Label 7250 3000 0    50   ~ 0
Row10
Text Label 7250 2900 0    50   ~ 0
Row9
Text Label 7250 2800 0    50   ~ 0
Row8
Text Label 7250 2700 0    50   ~ 0
Row7
Text Label 7250 2600 0    50   ~ 0
Row6
Text Label 7250 2500 0    50   ~ 0
Row5
Text Label 7250 2400 0    50   ~ 0
Row4
Text Label 7250 2200 0    50   ~ 0
Row2
Text Label 7250 2100 0    50   ~ 0
Row1
Text Label 7250 2000 0    50   ~ 0
Row0
Text Label 7350 4350 0    50   ~ 0
Col0
Text Label 7350 5350 0    50   ~ 0
Col10
Text Label 7350 5250 0    50   ~ 0
Col9
Text Label 7350 5150 0    50   ~ 0
Col8
Text Label 7350 5050 0    50   ~ 0
Col7
Text Label 7350 4950 0    50   ~ 0
Col6
Text Label 7350 4850 0    50   ~ 0
Col5
Text Label 7350 4750 0    50   ~ 0
Col4
Text Label 7350 4650 0    50   ~ 0
Col3
Text Label 7350 4550 0    50   ~ 0
Col2
Text Label 7350 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J11
U 1 1 5D29352F
P 7150 4850
F 0 "J11" H 7230 4842 50  0000 L CNN
F 1 "Conn_01x13" H 7230 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 7150 4850 50  0001 C CNN
F 3 "~" H 7150 4850 50  0001 C CNN
	1    7150 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J10
U 1 1 5D293539
P 7050 2700
F 0 "J10" H 7130 2692 50  0000 L CNN
F 1 "Conn_01x13" H 7130 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 7050 2700 50  0001 C CNN
F 3 "~" H 7050 2700 50  0001 C CNN
	1    7050 2700
	-1   0    0    -1  
$EndComp
Text Label 6500 3500 0    50   ~ 0
Row15
Text Label 6500 3400 0    50   ~ 0
Row14
Text Label 6500 2300 0    50   ~ 0
Row3
Text Label 6500 3300 0    50   ~ 0
Row13
Text Label 6500 3200 0    50   ~ 0
Row12
Text Label 6500 3100 0    50   ~ 0
Row11
Text Label 6500 3000 0    50   ~ 0
Row10
Text Label 6500 2900 0    50   ~ 0
Row9
Text Label 6500 2800 0    50   ~ 0
Row8
Text Label 6500 2700 0    50   ~ 0
Row7
Text Label 6500 2600 0    50   ~ 0
Row6
Text Label 6500 2500 0    50   ~ 0
Row5
Text Label 6500 2400 0    50   ~ 0
Row4
Text Label 6500 2200 0    50   ~ 0
Row2
Text Label 6500 2100 0    50   ~ 0
Row1
Text Label 6500 2000 0    50   ~ 0
Row0
Text Label 6600 4350 0    50   ~ 0
Col0
Text Label 6600 5350 0    50   ~ 0
Col10
Text Label 6600 5250 0    50   ~ 0
Col9
Text Label 6600 5150 0    50   ~ 0
Col8
Text Label 6600 5050 0    50   ~ 0
Col7
Text Label 6600 4950 0    50   ~ 0
Col6
Text Label 6600 4850 0    50   ~ 0
Col5
Text Label 6600 4750 0    50   ~ 0
Col4
Text Label 6600 4650 0    50   ~ 0
Col3
Text Label 6600 4550 0    50   ~ 0
Col2
Text Label 6600 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J9
U 1 1 5D29355E
P 6400 4850
F 0 "J9" H 6480 4842 50  0000 L CNN
F 1 "Conn_01x13" H 6480 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 6400 4850 50  0001 C CNN
F 3 "~" H 6400 4850 50  0001 C CNN
	1    6400 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J8
U 1 1 5D293568
P 6300 2700
F 0 "J8" H 6380 2692 50  0000 L CNN
F 1 "Conn_01x13" H 6380 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 6300 2700 50  0001 C CNN
F 3 "~" H 6300 2700 50  0001 C CNN
	1    6300 2700
	-1   0    0    -1  
$EndComp
Text Label 8650 3500 0    50   ~ 0
Row15
Text Label 8650 3400 0    50   ~ 0
Row14
Text Label 8650 2300 0    50   ~ 0
Row3
Text Label 8650 3300 0    50   ~ 0
Row13
Text Label 8650 3200 0    50   ~ 0
Row12
Text Label 8650 3100 0    50   ~ 0
Row11
Text Label 8650 3000 0    50   ~ 0
Row10
Text Label 8650 2900 0    50   ~ 0
Row9
Text Label 8650 2800 0    50   ~ 0
Row8
Text Label 8650 2700 0    50   ~ 0
Row7
Text Label 8650 2600 0    50   ~ 0
Row6
Text Label 8650 2500 0    50   ~ 0
Row5
Text Label 8650 2400 0    50   ~ 0
Row4
Text Label 8650 2200 0    50   ~ 0
Row2
Text Label 8650 2100 0    50   ~ 0
Row1
Text Label 8650 2000 0    50   ~ 0
Row0
Text Label 8750 4350 0    50   ~ 0
Col0
Text Label 8750 5350 0    50   ~ 0
Col10
Text Label 8750 5250 0    50   ~ 0
Col9
Text Label 8750 5150 0    50   ~ 0
Col8
Text Label 8750 5050 0    50   ~ 0
Col7
Text Label 8750 4950 0    50   ~ 0
Col6
Text Label 8750 4850 0    50   ~ 0
Col5
Text Label 8750 4750 0    50   ~ 0
Col4
Text Label 8750 4650 0    50   ~ 0
Col3
Text Label 8750 4550 0    50   ~ 0
Col2
Text Label 8750 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J15
U 1 1 5D2B5740
P 8550 4850
F 0 "J15" H 8630 4842 50  0000 L CNN
F 1 "Conn_01x13" H 8630 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 8550 4850 50  0001 C CNN
F 3 "~" H 8550 4850 50  0001 C CNN
	1    8550 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J14
U 1 1 5D2B574A
P 8450 2700
F 0 "J14" H 8530 2692 50  0000 L CNN
F 1 "Conn_01x13" H 8530 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 8450 2700 50  0001 C CNN
F 3 "~" H 8450 2700 50  0001 C CNN
	1    8450 2700
	-1   0    0    -1  
$EndComp
Text Label 8050 3500 0    50   ~ 0
Row15
Text Label 8050 3400 0    50   ~ 0
Row14
Text Label 8050 2300 0    50   ~ 0
Row3
Text Label 8050 3300 0    50   ~ 0
Row13
Text Label 8050 3200 0    50   ~ 0
Row12
Text Label 8050 3100 0    50   ~ 0
Row11
Text Label 8050 3000 0    50   ~ 0
Row10
Text Label 8050 2900 0    50   ~ 0
Row9
Text Label 8050 2800 0    50   ~ 0
Row8
Text Label 8050 2700 0    50   ~ 0
Row7
Text Label 8050 2600 0    50   ~ 0
Row6
Text Label 8050 2500 0    50   ~ 0
Row5
Text Label 8050 2400 0    50   ~ 0
Row4
Text Label 8050 2200 0    50   ~ 0
Row2
Text Label 8050 2100 0    50   ~ 0
Row1
Text Label 8050 2000 0    50   ~ 0
Row0
Text Label 8150 4350 0    50   ~ 0
Col0
Text Label 8150 5350 0    50   ~ 0
Col10
Text Label 8150 5250 0    50   ~ 0
Col9
Text Label 8150 5150 0    50   ~ 0
Col8
Text Label 8150 5050 0    50   ~ 0
Col7
Text Label 8150 4950 0    50   ~ 0
Col6
Text Label 8150 4850 0    50   ~ 0
Col5
Text Label 8150 4750 0    50   ~ 0
Col4
Text Label 8150 4650 0    50   ~ 0
Col3
Text Label 8150 4550 0    50   ~ 0
Col2
Text Label 8150 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J13
U 1 1 5D2B576F
P 7950 4850
F 0 "J13" H 8030 4842 50  0000 L CNN
F 1 "Conn_01x13" H 8030 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 7950 4850 50  0001 C CNN
F 3 "~" H 7950 4850 50  0001 C CNN
	1    7950 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J12
U 1 1 5D2B5779
P 7850 2700
F 0 "J12" H 7930 2692 50  0000 L CNN
F 1 "Conn_01x13" H 7930 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 7850 2700 50  0001 C CNN
F 3 "~" H 7850 2700 50  0001 C CNN
	1    7850 2700
	-1   0    0    -1  
$EndComp
Text Label 10150 3500 0    50   ~ 0
Row15
Text Label 10150 3400 0    50   ~ 0
Row14
Text Label 10150 2300 0    50   ~ 0
Row3
Text Label 10150 3300 0    50   ~ 0
Row13
Text Label 10150 3200 0    50   ~ 0
Row12
Text Label 10150 3100 0    50   ~ 0
Row11
Text Label 10150 3000 0    50   ~ 0
Row10
Text Label 10150 2900 0    50   ~ 0
Row9
Text Label 10150 2800 0    50   ~ 0
Row8
Text Label 10150 2700 0    50   ~ 0
Row7
Text Label 10150 2600 0    50   ~ 0
Row6
Text Label 10150 2500 0    50   ~ 0
Row5
Text Label 10150 2400 0    50   ~ 0
Row4
Text Label 10150 2200 0    50   ~ 0
Row2
Text Label 10150 2100 0    50   ~ 0
Row1
Text Label 10150 2000 0    50   ~ 0
Row0
Text Label 10250 4350 0    50   ~ 0
Col0
Text Label 10250 5350 0    50   ~ 0
Col10
Text Label 10250 5250 0    50   ~ 0
Col9
Text Label 10250 5150 0    50   ~ 0
Col8
Text Label 10250 5050 0    50   ~ 0
Col7
Text Label 10250 4950 0    50   ~ 0
Col6
Text Label 10250 4850 0    50   ~ 0
Col5
Text Label 10250 4750 0    50   ~ 0
Col4
Text Label 10250 4650 0    50   ~ 0
Col3
Text Label 10250 4550 0    50   ~ 0
Col2
Text Label 10250 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J19
U 1 1 5D2B579E
P 10050 4850
F 0 "J19" H 10130 4842 50  0000 L CNN
F 1 "Conn_01x13" H 10130 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 10050 4850 50  0001 C CNN
F 3 "~" H 10050 4850 50  0001 C CNN
	1    10050 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J18
U 1 1 5D2B57A8
P 9950 2700
F 0 "J18" H 10030 2692 50  0000 L CNN
F 1 "Conn_01x13" H 10030 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 9950 2700 50  0001 C CNN
F 3 "~" H 9950 2700 50  0001 C CNN
	1    9950 2700
	-1   0    0    -1  
$EndComp
Text Label 9400 3500 0    50   ~ 0
Row15
Text Label 9400 3400 0    50   ~ 0
Row14
Text Label 9400 2300 0    50   ~ 0
Row3
Text Label 9400 3300 0    50   ~ 0
Row13
Text Label 9400 3200 0    50   ~ 0
Row12
Text Label 9400 3100 0    50   ~ 0
Row11
Text Label 9400 3000 0    50   ~ 0
Row10
Text Label 9400 2900 0    50   ~ 0
Row9
Text Label 9400 2800 0    50   ~ 0
Row8
Text Label 9400 2700 0    50   ~ 0
Row7
Text Label 9400 2600 0    50   ~ 0
Row6
Text Label 9400 2500 0    50   ~ 0
Row5
Text Label 9400 2400 0    50   ~ 0
Row4
Text Label 9400 2200 0    50   ~ 0
Row2
Text Label 9400 2100 0    50   ~ 0
Row1
Text Label 9400 2000 0    50   ~ 0
Row0
Text Label 9500 4350 0    50   ~ 0
Col0
Text Label 9500 5350 0    50   ~ 0
Col10
Text Label 9500 5250 0    50   ~ 0
Col9
Text Label 9500 5150 0    50   ~ 0
Col8
Text Label 9500 5050 0    50   ~ 0
Col7
Text Label 9500 4950 0    50   ~ 0
Col6
Text Label 9500 4850 0    50   ~ 0
Col5
Text Label 9500 4750 0    50   ~ 0
Col4
Text Label 9500 4650 0    50   ~ 0
Col3
Text Label 9500 4550 0    50   ~ 0
Col2
Text Label 9500 4450 0    50   ~ 0
Col1
$Comp
L Connector_Generic:Conn_01x11 J17
U 1 1 5D2B57CD
P 9300 4850
F 0 "J17" H 9380 4842 50  0000 L CNN
F 1 "Conn_01x13" H 9380 4751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 9300 4850 50  0001 C CNN
F 3 "~" H 9300 4850 50  0001 C CNN
	1    9300 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x16 J16
U 1 1 5D2B57D7
P 9200 2700
F 0 "J16" H 9280 2692 50  0000 L CNN
F 1 "Conn_01x13" H 9280 2601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x16_P2.54mm_Vertical" H 9200 2700 50  0001 C CNN
F 3 "~" H 9200 2700 50  0001 C CNN
	1    9200 2700
	-1   0    0    -1  
$EndComp
Text Label 3900 6500 0    50   ~ 0
Col1
Text Label 3900 6600 0    50   ~ 0
Col2
Text Label 3900 6700 0    50   ~ 0
Col3
Text Label 3900 6800 0    50   ~ 0
Col4
Text Label 3900 6900 0    50   ~ 0
Col5
Text Label 3900 7000 0    50   ~ 0
Col6
Text Label 3900 7100 0    50   ~ 0
Col7
Text Label 3900 7200 0    50   ~ 0
Col8
Text Label 3900 7300 0    50   ~ 0
Col9
Text Label 3900 7400 0    50   ~ 0
Col10
Text Label 3900 6400 0    50   ~ 0
Col0
$Comp
L power:+5V #PWR01
U 1 1 5D2D2118
P 2400 1400
F 0 "#PWR01" H 2400 1250 50  0001 C CNN
F 1 "+5V" H 2415 1573 50  0000 C CNN
F 2 "" H 2400 1400 50  0001 C CNN
F 3 "" H 2400 1400 50  0001 C CNN
	1    2400 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 1950 2400 1950
Wire Wire Line
	2400 1950 2400 1850
Wire Wire Line
	2250 1850 2400 1850
Connection ~ 2400 1850
Wire Wire Line
	2400 1850 2400 1400
$Comp
L Device:R_US R2
U 1 1 5D2D3BDC
P 1250 6000
F 0 "R2" H 1318 6046 50  0000 L CNN
F 1 "R_US" H 1318 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 1290 5990 50  0001 C CNN
F 3 "~" H 1250 6000 50  0001 C CNN
	1    1250 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R3
U 1 1 5D2D65F2
P 1500 6000
F 0 "R3" H 1568 6046 50  0000 L CNN
F 1 "R_US" H 1568 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 1540 5990 50  0001 C CNN
F 3 "~" H 1500 6000 50  0001 C CNN
	1    1500 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R4
U 1 1 5D2D7508
P 1800 6000
F 0 "R4" H 1868 6046 50  0000 L CNN
F 1 "R_US" H 1868 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 1840 5990 50  0001 C CNN
F 3 "~" H 1800 6000 50  0001 C CNN
	1    1800 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R5
U 1 1 5D2D7512
P 2050 6000
F 0 "R5" H 2118 6046 50  0000 L CNN
F 1 "R_US" H 2118 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2090 5990 50  0001 C CNN
F 3 "~" H 2050 6000 50  0001 C CNN
	1    2050 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R6
U 1 1 5D2D8ED2
P 2300 6000
F 0 "R6" H 2368 6046 50  0000 L CNN
F 1 "R_US" H 2368 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2340 5990 50  0001 C CNN
F 3 "~" H 2300 6000 50  0001 C CNN
	1    2300 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R7
U 1 1 5D2D8EDC
P 2550 6000
F 0 "R7" H 2618 6046 50  0000 L CNN
F 1 "R_US" H 2618 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2590 5990 50  0001 C CNN
F 3 "~" H 2550 6000 50  0001 C CNN
	1    2550 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R8
U 1 1 5D2D8EE6
P 2800 6000
F 0 "R8" H 2868 6046 50  0000 L CNN
F 1 "R_US" H 2868 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 2840 5990 50  0001 C CNN
F 3 "~" H 2800 6000 50  0001 C CNN
	1    2800 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R9
U 1 1 5D2D8EF0
P 3050 6000
F 0 "R9" H 3118 6046 50  0000 L CNN
F 1 "R_US" H 3118 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 3090 5990 50  0001 C CNN
F 3 "~" H 3050 6000 50  0001 C CNN
	1    3050 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R10
U 1 1 5D2DCE33
P 3300 6000
F 0 "R10" H 3368 6046 50  0000 L CNN
F 1 "R_US" H 3368 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 3340 5990 50  0001 C CNN
F 3 "~" H 3300 6000 50  0001 C CNN
	1    3300 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R11
U 1 1 5D2DCE3D
P 3550 6000
F 0 "R11" H 3618 6046 50  0000 L CNN
F 1 "R_US" H 3618 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 3590 5990 50  0001 C CNN
F 3 "~" H 3550 6000 50  0001 C CNN
	1    3550 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 5850 1250 5700
Wire Wire Line
	3550 5700 3550 5850
Wire Wire Line
	3300 5850 3300 5700
Connection ~ 3300 5700
Wire Wire Line
	3300 5700 3550 5700
Wire Wire Line
	3050 5850 3050 5700
Connection ~ 3050 5700
Wire Wire Line
	3050 5700 3300 5700
Wire Wire Line
	2800 5850 2800 5700
Connection ~ 2800 5700
Wire Wire Line
	2800 5700 3050 5700
Wire Wire Line
	2550 5850 2550 5700
Connection ~ 2550 5700
Wire Wire Line
	2550 5700 2800 5700
Wire Wire Line
	2300 5850 2300 5700
Connection ~ 2300 5700
Wire Wire Line
	2300 5700 2550 5700
Wire Wire Line
	1800 5700 1800 5850
Wire Wire Line
	1250 5700 1500 5700
Connection ~ 1800 5700
Wire Wire Line
	1800 5700 2050 5700
Wire Wire Line
	1500 5850 1500 5700
Connection ~ 1500 5700
Wire Wire Line
	1500 5700 1800 5700
Wire Wire Line
	2050 5850 2050 5700
Connection ~ 2050 5700
Wire Wire Line
	2050 5700 2300 5700
Wire Wire Line
	3550 6150 3550 6400
Wire Wire Line
	3550 6400 3900 6400
Wire Wire Line
	3300 6150 3300 6500
Wire Wire Line
	3300 6500 3900 6500
Wire Wire Line
	3050 6150 3050 6600
Wire Wire Line
	3050 6600 3900 6600
Wire Wire Line
	2800 6150 2800 6700
Wire Wire Line
	2800 6700 3900 6700
Wire Wire Line
	2550 6150 2550 6800
Wire Wire Line
	2550 6800 3900 6800
Wire Wire Line
	2300 6150 2300 6900
Wire Wire Line
	2300 6900 3900 6900
Wire Wire Line
	2050 6150 2050 7000
Wire Wire Line
	2050 7000 3900 7000
Wire Wire Line
	1800 6150 1800 7100
Wire Wire Line
	1800 7100 3900 7100
Wire Wire Line
	1500 6150 1500 7200
Wire Wire Line
	1500 7200 3900 7200
Wire Wire Line
	1250 6150 1250 7300
Wire Wire Line
	1250 7300 3900 7300
$Comp
L Device:R_US R1
U 1 1 5D2ECFAD
P 1050 6000
F 0 "R1" H 1118 6046 50  0000 L CNN
F 1 "R_US" H 1118 5955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 1090 5990 50  0001 C CNN
F 3 "~" H 1050 6000 50  0001 C CNN
	1    1050 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1050 5850 1050 5700
Wire Wire Line
	1050 5700 1250 5700
Connection ~ 1250 5700
Wire Wire Line
	3900 7400 1050 7400
Wire Wire Line
	1050 7400 1050 6150
$Comp
L power:+5V #PWR?
U 1 1 5D2F4901
P 1550 5350
F 0 "#PWR?" H 1550 5200 50  0001 C CNN
F 1 "+5V" H 1565 5523 50  0000 C CNN
F 2 "" H 1550 5350 50  0001 C CNN
F 3 "" H 1550 5350 50  0001 C CNN
	1    1550 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 5700 1500 5350
Wire Wire Line
	1500 5350 1550 5350
$EndSCHEMATC
