EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "mar. 31 mars 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
NoConn ~ 9350 1350
Text Label 9250 1200 1    60   ~ 0
IOREF
Text Label 9050 2450 0    60   ~ 0
A0
Text Label 9050 2550 0    60   ~ 0
A1
Text Label 9050 2650 0    60   ~ 0
A2
Text Label 9050 2750 0    60   ~ 0
A3
Text Label 9050 2850 0    60   ~ 0
A4
Text Label 9050 2950 0    60   ~ 0
A5
Text Label 9050 3400 0    60   ~ 0
A8
Text Label 9050 3500 0    60   ~ 0
A9
Text Label 9050 3600 0    60   ~ 0
A10
Text Label 9050 3700 0    60   ~ 0
A11
Text Label 9050 3800 0    60   ~ 0
A12
Text Label 9050 3900 0    60   ~ 0
A13
Text Label 9050 4000 0    60   ~ 0
A14
Text Label 9050 4100 0    60   ~ 0
A15
Text Label 10550 5000 1    60   ~ 0
22
Text Label 10450 5000 1    60   ~ 0
24
Text Label 10350 5000 1    60   ~ 0
26
Text Label 10250 5000 1    60   ~ 0
28
Text Label 10150 5000 1    60   ~ 0
30
Text Label 10050 5000 1    60   ~ 0
32
Text Label 9950 5000 1    60   ~ 0
34
Text Label 9850 5000 1    60   ~ 0
36
Text Label 9750 5000 1    60   ~ 0
38
Text Label 9650 5000 1    60   ~ 0
40
Text Label 9550 5000 1    60   ~ 0
42
Text Label 9450 5000 1    60   ~ 0
44
Text Label 9350 5000 1    60   ~ 0
46
Text Label 9250 5000 1    60   ~ 0
48
Text Label 10550 5750 1    60   ~ 0
23
Text Label 10450 5750 1    60   ~ 0
25
Text Label 10350 5750 1    60   ~ 0
27
Text Label 10150 5750 1    60   ~ 0
31
Text Label 10250 5750 1    60   ~ 0
29
Text Label 10050 5750 1    60   ~ 0
33
Text Label 9950 5750 1    60   ~ 0
35
Text Label 9850 5750 1    60   ~ 0
37
Text Label 9750 5750 1    60   ~ 0
39
Text Label 9650 5750 1    60   ~ 0
41
Text Label 9550 5750 1    60   ~ 0
43
Text Label 9450 5750 1    60   ~ 0
45
Text Label 9350 5750 1    60   ~ 0
47
Text Label 9250 5750 1    60   ~ 0
49
Text Label 10200 4100 0    60   ~ 0
21(SCL)
Text Label 10200 4000 0    60   ~ 0
20(SDA)
Text Label 10200 3900 0    60   ~ 0
19(Rx1)
Text Label 10200 3800 0    60   ~ 0
18(Tx1)
Text Label 10200 3700 0    60   ~ 0
17(Rx2)
Text Label 10200 3600 0    60   ~ 0
16(Tx2)
Text Label 10200 3500 0    60   ~ 0
15(Rx3)
Text Label 10200 3400 0    60   ~ 0
14(Tx3)
Text Label 10700 1350 0    60   ~ 0
AREF
Text Notes 8375 575  0    60   ~ 0
Shield for Arduino Mega Rev 3
Text Notes 9650 1350 0    60   ~ 0
1
$Comp
L power:+3V3 #PWR020
U 1 1 56D71AA9
P 9100 1200
F 0 "#PWR020" H 9100 1050 50  0001 C CNN
F 1 "+3.3V" V 9100 1450 50  0000 C CNN
F 2 "" H 9100 1200 50  0000 C CNN
F 3 "" H 9100 1200 50  0000 C CNN
	1    9100 1200
	1    0    0    -1  
$EndComp
Text Label 8600 1550 0    60   ~ 0
Reset
Wire Wire Line
	9100 1200 9100 1650
Wire Wire Line
	9250 1450 9250 1200
Wire Wire Line
	9350 1450 9250 1450
Wire Notes Line
	9850 650  9850 475 
Wire Notes Line
	8350 650  9850 650 
Wire Wire Line
	9100 1650 9350 1650
Wire Wire Line
	9000 1050 9000 1750
Wire Wire Line
	9000 1750 9350 1750
Wire Wire Line
	9350 2050 8900 2050
Wire Wire Line
	8900 2050 8900 1200
Wire Wire Line
	8600 1550 9350 1550
Wire Wire Line
	9350 1850 9250 1850
Wire Wire Line
	9250 1850 9250 1950
Wire Wire Line
	9250 1950 9250 2150
Wire Wire Line
	9350 1950 9250 1950
Connection ~ 9250 1950
Wire Wire Line
	10150 1350 10400 1350
Wire Wire Line
	10150 1750 10600 1750
Wire Wire Line
	10600 1950 10150 1950
Wire Wire Line
	10150 2050 10600 2050
Wire Wire Line
	10250 1450 10150 1450
Wire Wire Line
	9350 2450 8900 2450
Wire Wire Line
	8900 2550 9350 2550
Wire Wire Line
	9350 2650 8900 2650
Wire Wire Line
	8900 2750 9350 2750
Wire Wire Line
	9350 2850 8900 2850
Wire Wire Line
	8900 2950 9350 2950
Wire Wire Line
	10400 2450 10150 2450
Wire Wire Line
	10150 2550 10400 2550
Wire Wire Line
	10400 2650 10150 2650
Wire Wire Line
	10400 2850 10150 2850
Wire Wire Line
	10150 2950 10400 2950
Wire Wire Line
	10400 3050 10150 3050
Wire Wire Line
	10150 3150 10400 3150
Wire Wire Line
	9350 3400 8900 3400
Wire Wire Line
	8900 3500 9350 3500
Wire Wire Line
	9350 3600 8900 3600
Wire Wire Line
	8900 3700 9350 3700
Wire Wire Line
	9350 3800 8900 3800
Wire Wire Line
	8900 3900 9350 3900
Wire Wire Line
	9350 4000 8900 4000
Wire Wire Line
	8900 4100 9350 4100
Wire Wire Line
	10600 3400 10150 3400
Wire Wire Line
	10150 3500 10600 3500
Wire Wire Line
	10600 3600 10150 3600
Wire Wire Line
	10150 3700 10600 3700
Wire Wire Line
	10600 3800 10150 3800
Wire Wire Line
	10150 3900 10600 3900
Wire Wire Line
	10600 4000 10150 4000
Wire Wire Line
	10150 4100 10600 4100
Wire Wire Line
	10550 5050 10550 4850
Wire Wire Line
	10450 5050 10450 4850
Wire Wire Line
	10350 5050 10350 4850
Wire Wire Line
	10250 5050 10250 4850
Wire Wire Line
	10150 5050 10150 4850
Wire Wire Line
	10050 5050 10050 4850
Wire Wire Line
	9950 5050 9950 4850
Wire Wire Line
	9850 5050 9850 4850
Wire Wire Line
	9750 5050 9750 4850
Wire Wire Line
	9650 5050 9650 4850
Wire Wire Line
	9550 5050 9550 4850
Wire Wire Line
	9450 5050 9450 4850
Wire Wire Line
	9350 5050 9350 4850
Wire Wire Line
	9250 5050 9250 4850
Wire Wire Line
	10550 5550 10550 5850
Wire Wire Line
	10450 5550 10450 5850
Wire Wire Line
	10350 5550 10350 5850
Wire Wire Line
	10250 5550 10250 5850
Wire Wire Line
	10150 5550 10150 5850
Wire Wire Line
	10050 5550 10050 5850
Wire Wire Line
	9950 5550 9950 5850
Wire Wire Line
	9850 5550 9850 5850
Wire Wire Line
	9750 5550 9750 5850
Wire Wire Line
	9650 5550 9650 5850
Wire Wire Line
	9550 5550 9550 5850
Wire Wire Line
	9450 5550 9450 5850
Wire Wire Line
	9350 5550 9350 5850
Wire Wire Line
	9250 5550 9250 5850
Wire Wire Line
	8950 5050 8700 5050
$Comp
L power:GND #PWR018
U 1 1 56D758F6
P 8700 5950
F 0 "#PWR018" H 8700 5700 50  0001 C CNN
F 1 "GND" H 8700 5800 50  0000 C CNN
F 2 "" H 8700 5950 50  0000 C CNN
F 3 "" H 8700 5950 50  0000 C CNN
	1    8700 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 5550 8700 5550
Connection ~ 8700 5550
Wire Wire Line
	10800 5550 10650 5550
Wire Wire Line
	10800 5050 10650 5050
$Comp
L power:+5V #PWR023
U 1 1 56D75AB8
P 10800 4750
F 0 "#PWR023" H 10800 4600 50  0001 C CNN
F 1 "+5V" H 10800 4890 50  0000 C CNN
F 2 "" H 10800 4750 50  0000 C CNN
F 3 "" H 10800 4750 50  0000 C CNN
	1    10800 4750
	1    0    0    -1  
$EndComp
Connection ~ 10800 5050
Wire Wire Line
	10800 4750 10800 5050
Wire Wire Line
	10800 5050 10800 5550
Wire Wire Line
	8700 5050 8700 5550
Wire Wire Line
	8700 5550 8700 5950
Wire Notes Line
	8350 6050 8350 500 
NoConn ~ 10150 1850
Text Notes 10300 1850 0    50   ~ 0
USED BY ETHERNET SHIELD
NoConn ~ 10150 2750
Text Notes 10250 2750 0    50   ~ 0
USED BY ETHERNET SHIELD
NoConn ~ 9150 5050
NoConn ~ 9050 5050
NoConn ~ 9050 5550
NoConn ~ 9150 5550
Text Notes 9150 5850 2    50   ~ 0
USED BY\nETHERNET\nSHIELD
NoConn ~ -7400 5900
Text Label 8900 4100 2    50   ~ 0
A15
Text Label 8900 4000 2    50   ~ 0
A14
Text Label 8900 3900 2    50   ~ 0
A13
Text Label 8900 3800 2    50   ~ 0
A12
Text Label 8900 3700 2    50   ~ 0
A11
Text Label 8900 3600 2    50   ~ 0
A10
Text Label 8900 3500 2    50   ~ 0
A9
Text Label 8900 3400 2    50   ~ 0
A8
NoConn ~ 10400 1350
Text Label 8900 1200 1    60   ~ 0
Vin
$Comp
L power:+5VL #PWR019
U 1 1 616FD070
P 9000 1050
F 0 "#PWR019" H 9000 900 50  0001 C CNN
F 1 "+5VL" H 9015 1223 50  0000 C CNN
F 2 "" H 9000 1050 50  0001 C CNN
F 3 "" H 9000 1050 50  0001 C CNN
	1    9000 1050
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR021
U 1 1 616FDC1F
P 9250 2150
F 0 "#PWR021" H 9250 1900 50  0001 C CNN
F 1 "GNDD" H 9254 1995 50  0000 C CNN
F 2 "" H 9250 2150 50  0001 C CNN
F 3 "" H 9250 2150 50  0001 C CNN
	1    9250 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR022
U 1 1 616FEA6D
P 10250 2150
F 0 "#PWR022" H 10250 1900 50  0001 C CNN
F 1 "GNDD" H 10254 1995 50  0000 C CNN
F 2 "" H 10250 2150 50  0001 C CNN
F 3 "" H 10250 2150 50  0001 C CNN
	1    10250 2150
	1    0    0    -1  
$EndComp
NoConn ~ 8900 1200
NoConn ~ 9250 1200
NoConn ~ 8600 1550
Text Label 8900 2450 2    50   ~ 0
A0
Text Label 8900 2550 2    50   ~ 0
A1
Text Label 8900 2650 2    50   ~ 0
A2
Text Label 8900 2750 2    50   ~ 0
A3
Text Label 8900 2850 2    50   ~ 0
A4
Text Label 8900 2950 2    50   ~ 0
A5
Text Label 10300 1550 0    50   ~ 0
D13
Text Label 10300 1750 0    50   ~ 0
D11
Text Label 10300 1950 0    50   ~ 0
D9
Text Label 10300 2050 0    50   ~ 0
D8
Text Label 10250 2450 0    50   ~ 0
D7
Text Label 10250 2550 0    50   ~ 0
D6
Text Label 10250 2650 0    50   ~ 0
D5
Text Label 10250 2850 0    50   ~ 0
D3
Text Label 10250 2950 0    50   ~ 0
D2
Text Label 10400 3050 0    50   ~ 0
D1
Text Label 10400 3150 0    50   ~ 0
D0
Wire Wire Line
	4150 6450 4150 6600
Wire Wire Line
	4750 6450 4750 6600
Wire Wire Line
	4750 6900 4750 6950
Wire Wire Line
	4150 6900 4150 6950
Text Notes 4050 7500 0    50   ~ 0
Red LED - solid\non until a packet\nis succesfully \nsent to controller
Wire Wire Line
	2400 6550 2050 6550
NoConn ~ 2400 6950
Text Notes 2650 6650 0    50   ~ 0
DATA IN - ORANGE
Text Notes 2650 6750 0    50   ~ 0
LOAD/CS - YELLOW
Text Notes 2650 6850 0    50   ~ 0
CLOCK - GREEN
Text Notes 1800 7500 0    50   ~ 0
LedControl lc=LedControl(16,14,15,2);\n16 - DATA IN\n14 - CLK\n15 - LOAD/CS
$Comp
L Connector_Generic:Conn_01x08 P1
U 1 1 56D71773
P 9550 1650
F 0 "P1" H 9550 2050 50  0000 C CNN
F 1 "Power" V 9650 1650 50  0000 C CNN
F 2 "Socket_Arduino_Mega:Socket_Strip_Arduino_1x08" H 9550 1650 50  0001 C CNN
F 3 "" H 9550 1650 50  0000 C CNN
	1    9550 1650
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 P5
U 1 1 56D72368
P 9950 1650
F 0 "P5" H 9950 2150 50  0000 C CNN
F 1 "PWM" V 10050 1650 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 9950 1650 50  0001 C CNN
F 3 "" H 9950 1650 50  0000 C CNN
	1    9950 1650
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 P6
U 1 1 56D734D0
P 9950 2750
F 0 "P6" H 9950 3150 50  0000 C CNN
F 1 "PWM" V 10050 2750 50  0000 C CNN
F 2 "Socket_Arduino_Mega:Socket_Strip_Arduino_1x08" H 9950 2750 50  0001 C CNN
F 3 "" H 9950 2750 50  0000 C CNN
	1    9950 2750
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 P3
U 1 1 56D73A0E
P 9550 3700
F 0 "P3" H 9550 4100 50  0000 C CNN
F 1 "Analog" V 9650 3700 50  0000 C CNN
F 2 "Socket_Arduino_Mega:Socket_Strip_Arduino_1x08" H 9550 3700 50  0001 C CNN
F 3 "" H 9550 3700 50  0000 C CNN
	1    9550 3700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 P7
U 1 1 56D73F2C
P 9950 3700
F 0 "P7" H 9950 4100 50  0000 C CNN
F 1 "Communication" V 10050 3700 50  0000 C CNN
F 2 "Socket_Arduino_Mega:Socket_Strip_Arduino_1x08" H 9950 3700 50  0001 C CNN
F 3 "" H 9950 3700 50  0000 C CNN
	1    9950 3700
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x18_Odd_Even P4
U 1 1 56D743B5
P 9750 5250
F 0 "P4" H 9750 6200 50  0000 C CNN
F 1 "Digital" V 9750 5250 50  0000 C CNN
F 2 "Socket_Arduino_Mega:Socket_Strip_Arduino_2x18" H 9750 4200 50  0001 C CNN
F 3 "" H 9750 4200 50  0000 C CNN
	1    9750 5250
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x06 P2
U 1 1 56D72F1C
P 9550 2650
F 0 "P2" H 9550 3050 50  0000 C CNN
F 1 "Analog" V 9650 2650 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 9550 2650 50  0001 C CNN
F 3 "" H 9550 2650 50  0000 C CNN
	1    9550 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 616EE56B
P 4150 6750
F 0 "D1" V 4189 6632 50  0000 R CNN
F 1 "GREEN" V 4098 6632 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 4150 6750 50  0001 C CNN
F 3 "~" H 4150 6750 50  0001 C CNN
	1    4150 6750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R1
U 1 1 616EFF2F
P 4150 6300
F 0 "R1" H 4218 6346 50  0000 L CNN
F 1 "330" H 4218 6255 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 4190 6290 50  0001 C CNN
F 3 "~" H 4150 6300 50  0001 C CNN
	1    4150 6300
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 616F7F07
P 4750 6750
F 0 "D2" V 4789 6632 50  0000 R CNN
F 1 "RED" V 4698 6632 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 4750 6750 50  0001 C CNN
F 3 "~" H 4750 6750 50  0001 C CNN
	1    4750 6750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R2
U 1 1 616F7F11
P 4750 6300
F 0 "R2" H 4818 6346 50  0000 L CNN
F 1 "330" H 4818 6255 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 4790 6290 50  0001 C CNN
F 3 "~" H 4750 6300 50  0001 C CNN
	1    4750 6300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J8
U 1 1 61726457
P 2600 6650
F 0 "J8" H 2250 6950 50  0000 L CNN
F 1 "7219 OUT" H 2400 6950 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 2600 6650 50  0001 C CNN
F 3 "~" H 2600 6650 50  0001 C CNN
	1    2600 6650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 61728B28
P 1550 5200
F 0 "J5" H 1578 5176 50  0000 L CNN
F 1 "LOWER 5V IN" H 1578 5085 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 1550 5200 50  0001 C CNN
F 3 "~" H 1550 5200 50  0001 C CNN
	1    1550 5200
	1    0    0    -1  
$EndComp
Wire Notes Line
	1700 5850 1700 7650
Wire Notes Line
	1700 7650 3450 7650
Text Notes 4250 6000 0    50   ~ 0
STATUS AND ADDRESS SELECT
Wire Notes Line
	3850 5850 3850 7650
Wire Notes Line
	3850 7650 6750 7650
Wire Notes Line
	6750 7650 6750 5850
Wire Notes Line
	6750 5850 3850 5850
Wire Notes Line
	3450 5850 1700 5850
Wire Notes Line
	3450 7650 3450 5850
Text Notes 9150 5000 2    50   ~ 0
USED BY\nETHERNET\nSHIELD
Text Label 1250 4100 2    50   ~ 0
UPPER_OUTPUT_5V
Text Label 1250 4250 2    50   ~ 0
SERVO_GND
Wire Wire Line
	1250 4250 1400 4250
Wire Wire Line
	10250 2150 10250 1450
Text Label 10300 1650 0    50   ~ 0
D12
Wire Wire Line
	10600 1650 10150 1650
Text Label 10600 3400 0    50   ~ 0
D14
Text Label 10600 3500 0    50   ~ 0
D15
Text Label 10600 3600 0    50   ~ 0
D16
Text Label 2400 6850 2    50   ~ 0
D14
Text Label 2400 6750 2    50   ~ 0
D15
Text Label 2400 6650 2    50   ~ 0
D16
$Comp
L power:GNDD #PWR09
U 1 1 617C6762
P 1500 4250
F 0 "#PWR09" H 1500 4000 50  0001 C CNN
F 1 "GNDD" H 1504 4095 50  0000 C CNN
F 2 "" H 1500 4250 50  0001 C CNN
F 3 "" H 1500 4250 50  0001 C CNN
	1    1500 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 617D4C60
P 2050 6550
F 0 "#PWR06" H 2050 6300 50  0001 C CNN
F 1 "GND" H 2055 6377 50  0000 C CNN
F 2 "" H 2050 6550 50  0001 C CNN
F 3 "" H 2050 6550 50  0001 C CNN
	1    2050 6550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J7
U 1 1 617A3D34
P 1800 4750
F 0 "J7" H 1828 4726 50  0000 L CNN
F 1 "NEXTION" H 1828 4635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1800 4750 50  0001 C CNN
F 3 "~" H 1800 4750 50  0001 C CNN
	1    1800 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1050 4750 1600 4750
$Comp
L power:GND #PWR03
U 1 1 617AE279
P 1050 4750
F 0 "#PWR03" H 1050 4500 50  0001 C CNN
F 1 "GND" H 1055 4577 50  0000 C CNN
F 2 "" H 1050 4750 50  0001 C CNN
F 3 "" H 1050 4750 50  0001 C CNN
	1    1050 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10150 1550 10600 1550
Text Label 10600 1550 0    50   ~ 0
RED_LED
Text Label 4150 6150 0    50   ~ 0
RED_LED
Text Label 10400 2650 0    50   ~ 0
GREEN_LED
Text Label 4750 6150 0    50   ~ 0
GREEN_LED
$Comp
L power:GNDD #PWR011
U 1 1 617DC479
P 4750 6950
F 0 "#PWR011" H 4750 6700 50  0001 C CNN
F 1 "GNDD" H 4754 6795 50  0000 C CNN
F 2 "" H 4750 6950 50  0001 C CNN
F 3 "" H 4750 6950 50  0001 C CNN
	1    4750 6950
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR010
U 1 1 617DD1EC
P 4150 6950
F 0 "#PWR010" H 4150 6700 50  0001 C CNN
F 1 "GNDD" H 4154 6795 50  0000 C CNN
F 2 "" H 4150 6950 50  0001 C CNN
F 3 "" H 4150 6950 50  0001 C CNN
	1    4150 6950
	1    0    0    -1  
$EndComp
Text Label 1600 4850 2    50   ~ 0
NEXTION_TX
Text Label 1600 4950 2    50   ~ 0
NEXTION_RX
Text Label 10600 3800 0    50   ~ 0
NEXTION_TX
Text Label 10600 3900 0    50   ~ 0
NEXTION_RX
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 6173779C
P 1950 4100
F 0 "J11" H 1978 4076 50  0000 L CNN
F 1 "5V IN" H 1978 3985 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 1950 4100 50  0001 C CNN
F 3 "~" H 1950 4100 50  0001 C CNN
	1    1950 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 4100 1750 4100
Wire Wire Line
	1750 4200 1400 4200
Wire Wire Line
	1400 4200 1400 4250
Connection ~ 1400 4250
Wire Wire Line
	1400 4250 1500 4250
Text Notes 2800 5650 0    50   ~ 0
Powering the Servos and LEDs and Nextron is done through a barrel connect to the upper board and then carried across to \nthe lower board through stand headers.  As boards are split, separate 5V and 12V buses are used in this diagram.
Text Label 1600 4650 2    50   ~ 0
LOWER_OUTPUT_5V
Text Label 2400 6450 2    50   ~ 0
LOWER_OUTPUT_5V
$Comp
L power:GND #PWR04
U 1 1 61778DBA
P 1350 5300
F 0 "#PWR04" H 1350 5050 50  0001 C CNN
F 1 "GND" H 1355 5127 50  0000 C CNN
F 2 "" H 1350 5300 50  0001 C CNN
F 3 "" H 1350 5300 50  0001 C CNN
	1    1350 5300
	1    0    0    -1  
$EndComp
Text Label 1350 5200 2    50   ~ 0
LOWER_OUTPUT_5V
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 6177A219
P 1600 5650
F 0 "J6" H 1628 5626 50  0000 L CNN
F 1 "UPPER 5V OUT" H 1628 5535 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 1600 5650 50  0001 C CNN
F 3 "~" H 1600 5650 50  0001 C CNN
	1    1600 5650
	1    0    0    -1  
$EndComp
Text Label 1400 5650 2    50   ~ 0
UPPER_OUTPUT_5V
$Comp
L power:GNDD #PWR05
U 1 1 61782A2B
P 1400 5750
F 0 "#PWR05" H 1400 5500 50  0001 C CNN
F 1 "GNDD" H 1404 5595 50  0000 C CNN
F 2 "" H 1400 5750 50  0001 C CNN
F 3 "" H 1400 5750 50  0001 C CNN
	1    1400 5750
	1    0    0    -1  
$EndComp
NoConn ~ 8900 2950
NoConn ~ 10400 3150
NoConn ~ 10600 3700
NoConn ~ 8900 3400
NoConn ~ 8900 3500
NoConn ~ 8900 3600
NoConn ~ 8900 3700
NoConn ~ 8900 3800
NoConn ~ 8900 3900
NoConn ~ 8900 4000
NoConn ~ 8900 4100
NoConn ~ 10400 3050
$Comp
L Connector:Conn_01x03_Female J18
U 1 1 61766056
P 950 6700
F 0 "J18" H 978 6726 50  0000 L CNN
F 1 "NEXTION ANA" H 978 6635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 950 6700 50  0001 C CNN
F 3 "~" H 950 6700 50  0001 C CNN
	1    950  6700
	1    0    0    -1  
$EndComp
$Comp
L power:+5VL #PWR016
U 1 1 617677BF
P 750 6600
F 0 "#PWR016" H 750 6450 50  0001 C CNN
F 1 "+5VL" H 765 6773 50  0000 C CNN
F 2 "" H 750 6600 50  0001 C CNN
F 3 "" H 750 6600 50  0001 C CNN
	1    750  6600
	1    0    0    -1  
$EndComp
Text Label 750  6700 2    50   ~ 0
A4
$Comp
L power:GNDD #PWR017
U 1 1 6176CA45
P 750 6800
F 0 "#PWR017" H 750 6550 50  0001 C CNN
F 1 "GNDD" H 754 6645 50  0000 C CNN
F 2 "" H 750 6800 50  0001 C CNN
F 3 "" H 750 6800 50  0001 C CNN
	1    750  6800
	1    0    0    -1  
$EndComp
$Comp
L Hornet-Forward-Output-Upper-Shield-rescue:IRF1405PBF-Mosfet Q1
U 1 1 624444DF
P 5350 3950
F 0 "Q1" H 5878 3996 50  0000 L CNN
F 1 "IRF1405PBF" H 5878 3905 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 5350 4450 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf1405pbf.pdf?fileId=5546d462533600a4015355db084a18bb" H 5350 4550 50  0001 L CNN
F 4 "No" H 5350 4650 50  0001 L CNN "automotive"
F 5 "Trans" H 5350 4750 50  0001 L CNN "category"
F 6 "169A" H 5350 4850 50  0001 L CNN "continuous drain current"
F 7 "False" H 5350 4950 50  0001 L CNN "depletion mode"
F 8 "Discrete Semiconductors" H 5350 5050 50  0001 L CNN "device class L1"
F 9 "Transistors" H 5350 5150 50  0001 L CNN "device class L2"
F 10 "MOSFETs" H 5350 5250 50  0001 L CNN "device class L3"
F 11 "MOSFET N-CH 55V 169A TO-220AB" H 5350 5350 50  0001 L CNN "digikey description"
F 12 "IRF1405PBF-ND" H 5350 5450 50  0001 L CNN "digikey part number"
F 13 "55V" H 5350 5550 50  0001 L CNN "drain to source breakdown voltage"
F 14 "4.6mΩ" H 5350 5650 50  0001 L CNN "drain to source resistance"
F 15 "55V" H 5350 5750 50  0001 L CNN "drain to source voltage"
F 16 "https://www.infineon.com/dgdl/po-to220ab-fp.pdf?fileId=5546d462580663ef0158068cfbee01be" H 5350 5850 50  0001 L CNN "footprint url"
F 17 "170nC @ 10V" H 5350 5950 50  0001 L CNN "gate charge at vgs"
F 18 "20V" H 5350 6050 50  0001 L CNN "gate to source voltage"
F 19 "19.8mm" H 5350 6150 50  0001 L CNN "height"
F 20 "5480pF @ 25V" H 5350 6250 50  0001 L CNN "input capacitace at vds"
F 21 "TO-220" H 5350 6350 50  0001 L CNN "ipc land pattern name"
F 22 "Yes" H 5350 6450 50  0001 L CNN "lead free"
F 23 "802d71249bb95e7c" H 5350 6550 50  0001 L CNN "library id"
F 24 "International Rectifier" H 5350 6650 50  0001 L CNN "manufacturer"
F 25 "1.3V" H 5350 6750 50  0001 L CNN "max forward diode voltage"
F 26 "+175°C" H 5350 6850 50  0001 L CNN "max junction temp"
F 27 "MOSFET Operating temperature: -55...+175 °C Housing type: TO-220AB Polarity: N Power dissipation: 330 W" H 5350 6950 50  0001 L CNN "mouser description"
F 28 "942-IRF1405PBF" H 5350 7050 50  0001 L CNN "mouser part number"
F 29 "1" H 5350 7150 50  0001 L CNN "number of N channels"
F 30 "1" H 5350 7250 50  0001 L CNN "number of channels"
F 31 "TO-220AB" H 5350 7350 50  0001 L CNN "package"
F 32 "330W" H 5350 7450 50  0001 L CNN "power dissipation"
F 33 "680A" H 5350 7550 50  0001 L CNN "pulse drain current"
F 34 "250nC" H 5350 7650 50  0001 L CNN "reverse recovery charge"
F 35 "88ns" H 5350 7750 50  0001 L CNN "reverse recovery time"
F 36 "Yes" H 5350 7850 50  0001 L CNN "rohs"
F 37 "62°C/W" H 5350 7950 50  0001 L CNN "rthja max"
F 38 "3.37mm" H 5350 8050 50  0001 L CNN "standoff height"
F 39 "+175°C" H 5350 8150 50  0001 L CNN "temperature range high"
F 40 "-55°C" H 5350 8250 50  0001 L CNN "temperature range low"
F 41 "4V" H 5350 8350 50  0001 L CNN "threshold vgs max"
F 42 "2V" H 5350 8450 50  0001 L CNN "threshold vgs min"
F 43 "130ns" H 5350 8550 50  0001 L CNN "turn off delay time"
F 44 "13ns" H 5350 8650 50  0001 L CNN "turn on delay time"
	1    5350 3950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 62445EE7
P 5750 4150
F 0 "#PWR0101" H 5750 3900 50  0001 C CNN
F 1 "GND" H 5755 3977 50  0000 C CNN
F 2 "" H 5750 4150 50  0001 C CNN
F 3 "" H 5750 4150 50  0001 C CNN
	1    5750 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R3
U 1 1 62447217
P 5100 3950
F 0 "R3" H 5168 3996 50  0000 L CNN
F 1 "330" H 5168 3905 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5140 3940 50  0001 C CNN
F 3 "~" H 5100 3950 50  0001 C CNN
	1    5100 3950
	0    1    -1   0   
$EndComp
Wire Wire Line
	5450 3950 5400 3950
$Comp
L Connector:Conn_01x04_Female J13
U 1 1 62451B06
P 7450 4150
F 0 "J13" H 7478 4126 50  0000 L CNN
F 1 "PORT SIDE" H 7478 4035 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7450 4150 50  0001 C CNN
F 3 "~" H 7450 4150 50  0001 C CNN
	1    7450 4150
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 62452691
P 1450 950
F 0 "J1" H 1478 926 50  0000 L CNN
F 1 "LOWER 12V IN" H 1478 835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 1450 950 50  0001 C CNN
F 3 "~" H 1450 950 50  0001 C CNN
	1    1450 950 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 62454164
P 1250 1050
F 0 "#PWR0102" H 1250 800 50  0001 C CNN
F 1 "GND" H 1255 877 50  0000 C CNN
F 2 "" H 1250 1050 50  0001 C CNN
F 3 "" H 1250 1050 50  0001 C CNN
	1    1250 1050
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0103
U 1 1 6245528D
P 1250 950
F 0 "#PWR0103" H 1250 800 50  0001 C CNN
F 1 "+12V" H 1265 1123 50  0000 C CNN
F 2 "" H 1250 950 50  0001 C CNN
F 3 "" H 1250 950 50  0001 C CNN
	1    1250 950 
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0104
U 1 1 62455923
P 7150 4050
F 0 "#PWR0104" H 7150 3900 50  0001 C CNN
F 1 "+12V" H 7165 4223 50  0000 C CNN
F 2 "" H 7150 4050 50  0001 C CNN
F 3 "" H 7150 4050 50  0001 C CNN
	1    7150 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 4050 7150 4050
Wire Wire Line
	7250 4250 7150 4250
Wire Wire Line
	7150 4250 7150 4050
Connection ~ 7150 4050
Text Label 7250 4150 2    50   ~ 0
NAV_LIGHTS_NEG
Text Label 7250 4350 2    50   ~ 0
STROBE_LIGHTS_NEG
Text Label 5750 3750 2    50   ~ 0
NAV_LIGHTS_NEG
$Comp
L Hornet-Forward-Output-Upper-Shield-rescue:IRF1405PBF-Mosfet Q2
U 1 1 62465D20
P 5350 4700
F 0 "Q2" H 5878 4746 50  0000 L CNN
F 1 "IRF1405PBF" H 5878 4655 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 5350 5200 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf1405pbf.pdf?fileId=5546d462533600a4015355db084a18bb" H 5350 5300 50  0001 L CNN
F 4 "No" H 5350 5400 50  0001 L CNN "automotive"
F 5 "Trans" H 5350 5500 50  0001 L CNN "category"
F 6 "169A" H 5350 5600 50  0001 L CNN "continuous drain current"
F 7 "False" H 5350 5700 50  0001 L CNN "depletion mode"
F 8 "Discrete Semiconductors" H 5350 5800 50  0001 L CNN "device class L1"
F 9 "Transistors" H 5350 5900 50  0001 L CNN "device class L2"
F 10 "MOSFETs" H 5350 6000 50  0001 L CNN "device class L3"
F 11 "MOSFET N-CH 55V 169A TO-220AB" H 5350 6100 50  0001 L CNN "digikey description"
F 12 "IRF1405PBF-ND" H 5350 6200 50  0001 L CNN "digikey part number"
F 13 "55V" H 5350 6300 50  0001 L CNN "drain to source breakdown voltage"
F 14 "4.6mΩ" H 5350 6400 50  0001 L CNN "drain to source resistance"
F 15 "55V" H 5350 6500 50  0001 L CNN "drain to source voltage"
F 16 "https://www.infineon.com/dgdl/po-to220ab-fp.pdf?fileId=5546d462580663ef0158068cfbee01be" H 5350 6600 50  0001 L CNN "footprint url"
F 17 "170nC @ 10V" H 5350 6700 50  0001 L CNN "gate charge at vgs"
F 18 "20V" H 5350 6800 50  0001 L CNN "gate to source voltage"
F 19 "19.8mm" H 5350 6900 50  0001 L CNN "height"
F 20 "5480pF @ 25V" H 5350 7000 50  0001 L CNN "input capacitace at vds"
F 21 "TO-220" H 5350 7100 50  0001 L CNN "ipc land pattern name"
F 22 "Yes" H 5350 7200 50  0001 L CNN "lead free"
F 23 "802d71249bb95e7c" H 5350 7300 50  0001 L CNN "library id"
F 24 "International Rectifier" H 5350 7400 50  0001 L CNN "manufacturer"
F 25 "1.3V" H 5350 7500 50  0001 L CNN "max forward diode voltage"
F 26 "+175°C" H 5350 7600 50  0001 L CNN "max junction temp"
F 27 "MOSFET Operating temperature: -55...+175 °C Housing type: TO-220AB Polarity: N Power dissipation: 330 W" H 5350 7700 50  0001 L CNN "mouser description"
F 28 "942-IRF1405PBF" H 5350 7800 50  0001 L CNN "mouser part number"
F 29 "1" H 5350 7900 50  0001 L CNN "number of N channels"
F 30 "1" H 5350 8000 50  0001 L CNN "number of channels"
F 31 "TO-220AB" H 5350 8100 50  0001 L CNN "package"
F 32 "330W" H 5350 8200 50  0001 L CNN "power dissipation"
F 33 "680A" H 5350 8300 50  0001 L CNN "pulse drain current"
F 34 "250nC" H 5350 8400 50  0001 L CNN "reverse recovery charge"
F 35 "88ns" H 5350 8500 50  0001 L CNN "reverse recovery time"
F 36 "Yes" H 5350 8600 50  0001 L CNN "rohs"
F 37 "62°C/W" H 5350 8700 50  0001 L CNN "rthja max"
F 38 "3.37mm" H 5350 8800 50  0001 L CNN "standoff height"
F 39 "+175°C" H 5350 8900 50  0001 L CNN "temperature range high"
F 40 "-55°C" H 5350 9000 50  0001 L CNN "temperature range low"
F 41 "4V" H 5350 9100 50  0001 L CNN "threshold vgs max"
F 42 "2V" H 5350 9200 50  0001 L CNN "threshold vgs min"
F 43 "130ns" H 5350 9300 50  0001 L CNN "turn off delay time"
F 44 "13ns" H 5350 9400 50  0001 L CNN "turn on delay time"
	1    5350 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 62465D2A
P 5750 4900
F 0 "#PWR0105" H 5750 4650 50  0001 C CNN
F 1 "GND" H 5755 4727 50  0000 C CNN
F 2 "" H 5750 4900 50  0001 C CNN
F 3 "" H 5750 4900 50  0001 C CNN
	1    5750 4900
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R4
U 1 1 62465D34
P 5100 4700
F 0 "R4" H 5168 4746 50  0000 L CNN
F 1 "330" H 5168 4655 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5140 4690 50  0001 C CNN
F 3 "~" H 5100 4700 50  0001 C CNN
	1    5100 4700
	0    1    -1   0   
$EndComp
Text Label 5750 4500 2    50   ~ 0
STROBE_LIGHTS_NEG
$Comp
L Hornet-Forward-Output-Upper-Shield-rescue:IRF1405PBF-Mosfet Q3
U 1 1 6246E3EC
P 5500 1250
F 0 "Q3" H 6028 1296 50  0000 L CNN
F 1 "IRF1405PBF" H 6028 1205 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 5500 1750 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf1405pbf.pdf?fileId=5546d462533600a4015355db084a18bb" H 5500 1850 50  0001 L CNN
F 4 "No" H 5500 1950 50  0001 L CNN "automotive"
F 5 "Trans" H 5500 2050 50  0001 L CNN "category"
F 6 "169A" H 5500 2150 50  0001 L CNN "continuous drain current"
F 7 "False" H 5500 2250 50  0001 L CNN "depletion mode"
F 8 "Discrete Semiconductors" H 5500 2350 50  0001 L CNN "device class L1"
F 9 "Transistors" H 5500 2450 50  0001 L CNN "device class L2"
F 10 "MOSFETs" H 5500 2550 50  0001 L CNN "device class L3"
F 11 "MOSFET N-CH 55V 169A TO-220AB" H 5500 2650 50  0001 L CNN "digikey description"
F 12 "IRF1405PBF-ND" H 5500 2750 50  0001 L CNN "digikey part number"
F 13 "55V" H 5500 2850 50  0001 L CNN "drain to source breakdown voltage"
F 14 "4.6mΩ" H 5500 2950 50  0001 L CNN "drain to source resistance"
F 15 "55V" H 5500 3050 50  0001 L CNN "drain to source voltage"
F 16 "https://www.infineon.com/dgdl/po-to220ab-fp.pdf?fileId=5546d462580663ef0158068cfbee01be" H 5500 3150 50  0001 L CNN "footprint url"
F 17 "170nC @ 10V" H 5500 3250 50  0001 L CNN "gate charge at vgs"
F 18 "20V" H 5500 3350 50  0001 L CNN "gate to source voltage"
F 19 "19.8mm" H 5500 3450 50  0001 L CNN "height"
F 20 "5480pF @ 25V" H 5500 3550 50  0001 L CNN "input capacitace at vds"
F 21 "TO-220" H 5500 3650 50  0001 L CNN "ipc land pattern name"
F 22 "Yes" H 5500 3750 50  0001 L CNN "lead free"
F 23 "802d71249bb95e7c" H 5500 3850 50  0001 L CNN "library id"
F 24 "International Rectifier" H 5500 3950 50  0001 L CNN "manufacturer"
F 25 "1.3V" H 5500 4050 50  0001 L CNN "max forward diode voltage"
F 26 "+175°C" H 5500 4150 50  0001 L CNN "max junction temp"
F 27 "MOSFET Operating temperature: -55...+175 °C Housing type: TO-220AB Polarity: N Power dissipation: 330 W" H 5500 4250 50  0001 L CNN "mouser description"
F 28 "942-IRF1405PBF" H 5500 4350 50  0001 L CNN "mouser part number"
F 29 "1" H 5500 4450 50  0001 L CNN "number of N channels"
F 30 "1" H 5500 4550 50  0001 L CNN "number of channels"
F 31 "TO-220AB" H 5500 4650 50  0001 L CNN "package"
F 32 "330W" H 5500 4750 50  0001 L CNN "power dissipation"
F 33 "680A" H 5500 4850 50  0001 L CNN "pulse drain current"
F 34 "250nC" H 5500 4950 50  0001 L CNN "reverse recovery charge"
F 35 "88ns" H 5500 5050 50  0001 L CNN "reverse recovery time"
F 36 "Yes" H 5500 5150 50  0001 L CNN "rohs"
F 37 "62°C/W" H 5500 5250 50  0001 L CNN "rthja max"
F 38 "3.37mm" H 5500 5350 50  0001 L CNN "standoff height"
F 39 "+175°C" H 5500 5450 50  0001 L CNN "temperature range high"
F 40 "-55°C" H 5500 5550 50  0001 L CNN "temperature range low"
F 41 "4V" H 5500 5650 50  0001 L CNN "threshold vgs max"
F 42 "2V" H 5500 5750 50  0001 L CNN "threshold vgs min"
F 43 "130ns" H 5500 5850 50  0001 L CNN "turn off delay time"
F 44 "13ns" H 5500 5950 50  0001 L CNN "turn on delay time"
	1    5500 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R5
U 1 1 6246E400
P 5250 1250
F 0 "R5" H 5318 1296 50  0000 L CNN
F 1 "330" H 5318 1205 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5290 1240 50  0001 C CNN
F 3 "~" H 5250 1250 50  0001 C CNN
	1    5250 1250
	0    1    -1   0   
$EndComp
Wire Wire Line
	5600 1250 5550 1250
Text Label 5900 1050 2    50   ~ 0
FORMATION_LIGHTS_NEG
$Comp
L Connector:Conn_01x04_Female J19
U 1 1 6247473C
P 7500 4700
F 0 "J19" H 7528 4676 50  0000 L CNN
F 1 "STARBOARD SIDE" H 7528 4585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7500 4700 50  0001 C CNN
F 3 "~" H 7500 4700 50  0001 C CNN
	1    7500 4700
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0107
U 1 1 62474746
P 7200 4600
F 0 "#PWR0107" H 7200 4450 50  0001 C CNN
F 1 "+12V" H 7215 4773 50  0000 C CNN
F 2 "" H 7200 4600 50  0001 C CNN
F 3 "" H 7200 4600 50  0001 C CNN
	1    7200 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 4600 7200 4600
Wire Wire Line
	7300 4800 7200 4800
Wire Wire Line
	7200 4800 7200 4600
Connection ~ 7200 4600
Text Label 7300 4700 2    50   ~ 0
NAV_LIGHTS_NEG
Text Label 7300 4900 2    50   ~ 0
STROBE_LIGHTS_NEG
$Comp
L Connector:Conn_01x02_Female J14
U 1 1 624799C6
P 7550 1000
F 0 "J14" H 7578 976 50  0000 L CNN
F 1 "LEFT FORM" H 7578 885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7550 1000 50  0001 C CNN
F 3 "~" H 7550 1000 50  0001 C CNN
	1    7550 1000
	1    0    0    -1  
$EndComp
Text Label 7350 1100 2    50   ~ 0
FORMATION_LIGHTS_NEG
$Comp
L Connector:Conn_01x02_Female J12
U 1 1 624A3E85
P 7500 1500
F 0 "J12" H 7528 1476 50  0000 L CNN
F 1 "RIGHT FORM" H 7528 1385 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7500 1500 50  0001 C CNN
F 3 "~" H 7500 1500 50  0001 C CNN
	1    7500 1500
	1    0    0    -1  
$EndComp
Text Label 7300 1600 2    50   ~ 0
FORMATION_LIGHTS_NEG
$Comp
L Hornet-Forward-Output-Upper-Shield-rescue:IRF1405PBF-Mosfet Q5
U 1 1 62469FCB
P 3300 2500
F 0 "Q5" H 3828 2546 50  0000 L CNN
F 1 "IRF1405PBF" H 3828 2455 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 3300 3000 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf1405pbf.pdf?fileId=5546d462533600a4015355db084a18bb" H 3300 3100 50  0001 L CNN
F 4 "No" H 3300 3200 50  0001 L CNN "automotive"
F 5 "Trans" H 3300 3300 50  0001 L CNN "category"
F 6 "169A" H 3300 3400 50  0001 L CNN "continuous drain current"
F 7 "False" H 3300 3500 50  0001 L CNN "depletion mode"
F 8 "Discrete Semiconductors" H 3300 3600 50  0001 L CNN "device class L1"
F 9 "Transistors" H 3300 3700 50  0001 L CNN "device class L2"
F 10 "MOSFETs" H 3300 3800 50  0001 L CNN "device class L3"
F 11 "MOSFET N-CH 55V 169A TO-220AB" H 3300 3900 50  0001 L CNN "digikey description"
F 12 "IRF1405PBF-ND" H 3300 4000 50  0001 L CNN "digikey part number"
F 13 "55V" H 3300 4100 50  0001 L CNN "drain to source breakdown voltage"
F 14 "4.6mΩ" H 3300 4200 50  0001 L CNN "drain to source resistance"
F 15 "55V" H 3300 4300 50  0001 L CNN "drain to source voltage"
F 16 "https://www.infineon.com/dgdl/po-to220ab-fp.pdf?fileId=5546d462580663ef0158068cfbee01be" H 3300 4400 50  0001 L CNN "footprint url"
F 17 "170nC @ 10V" H 3300 4500 50  0001 L CNN "gate charge at vgs"
F 18 "20V" H 3300 4600 50  0001 L CNN "gate to source voltage"
F 19 "19.8mm" H 3300 4700 50  0001 L CNN "height"
F 20 "5480pF @ 25V" H 3300 4800 50  0001 L CNN "input capacitace at vds"
F 21 "TO-220" H 3300 4900 50  0001 L CNN "ipc land pattern name"
F 22 "Yes" H 3300 5000 50  0001 L CNN "lead free"
F 23 "802d71249bb95e7c" H 3300 5100 50  0001 L CNN "library id"
F 24 "International Rectifier" H 3300 5200 50  0001 L CNN "manufacturer"
F 25 "1.3V" H 3300 5300 50  0001 L CNN "max forward diode voltage"
F 26 "+175°C" H 3300 5400 50  0001 L CNN "max junction temp"
F 27 "MOSFET Operating temperature: -55...+175 °C Housing type: TO-220AB Polarity: N Power dissipation: 330 W" H 3300 5500 50  0001 L CNN "mouser description"
F 28 "942-IRF1405PBF" H 3300 5600 50  0001 L CNN "mouser part number"
F 29 "1" H 3300 5700 50  0001 L CNN "number of N channels"
F 30 "1" H 3300 5800 50  0001 L CNN "number of channels"
F 31 "TO-220AB" H 3300 5900 50  0001 L CNN "package"
F 32 "330W" H 3300 6000 50  0001 L CNN "power dissipation"
F 33 "680A" H 3300 6100 50  0001 L CNN "pulse drain current"
F 34 "250nC" H 3300 6200 50  0001 L CNN "reverse recovery charge"
F 35 "88ns" H 3300 6300 50  0001 L CNN "reverse recovery time"
F 36 "Yes" H 3300 6400 50  0001 L CNN "rohs"
F 37 "62°C/W" H 3300 6500 50  0001 L CNN "rthja max"
F 38 "3.37mm" H 3300 6600 50  0001 L CNN "standoff height"
F 39 "+175°C" H 3300 6700 50  0001 L CNN "temperature range high"
F 40 "-55°C" H 3300 6800 50  0001 L CNN "temperature range low"
F 41 "4V" H 3300 6900 50  0001 L CNN "threshold vgs max"
F 42 "2V" H 3300 7000 50  0001 L CNN "threshold vgs min"
F 43 "130ns" H 3300 7100 50  0001 L CNN "turn off delay time"
F 44 "13ns" H 3300 7200 50  0001 L CNN "turn on delay time"
	1    3300 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R7
U 1 1 62469FDF
P 3050 2500
F 0 "R7" H 3118 2546 50  0000 L CNN
F 1 "330" H 3118 2455 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 3090 2490 50  0001 C CNN
F 3 "~" H 3050 2500 50  0001 C CNN
	1    3050 2500
	0    1    -1   0   
$EndComp
Wire Wire Line
	3400 2500 3300 2500
$Comp
L Hornet-Forward-Output-Upper-Shield-rescue:IRF1405PBF-Mosfet Q4
U 1 1 62475E29
P 3150 4100
F 0 "Q4" H 3678 4146 50  0000 L CNN
F 1 "IRF1405PBF" H 3678 4055 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 3150 4600 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf1405pbf.pdf?fileId=5546d462533600a4015355db084a18bb" H 3150 4700 50  0001 L CNN
F 4 "No" H 3150 4800 50  0001 L CNN "automotive"
F 5 "Trans" H 3150 4900 50  0001 L CNN "category"
F 6 "169A" H 3150 5000 50  0001 L CNN "continuous drain current"
F 7 "False" H 3150 5100 50  0001 L CNN "depletion mode"
F 8 "Discrete Semiconductors" H 3150 5200 50  0001 L CNN "device class L1"
F 9 "Transistors" H 3150 5300 50  0001 L CNN "device class L2"
F 10 "MOSFETs" H 3150 5400 50  0001 L CNN "device class L3"
F 11 "MOSFET N-CH 55V 169A TO-220AB" H 3150 5500 50  0001 L CNN "digikey description"
F 12 "IRF1405PBF-ND" H 3150 5600 50  0001 L CNN "digikey part number"
F 13 "55V" H 3150 5700 50  0001 L CNN "drain to source breakdown voltage"
F 14 "4.6mΩ" H 3150 5800 50  0001 L CNN "drain to source resistance"
F 15 "55V" H 3150 5900 50  0001 L CNN "drain to source voltage"
F 16 "https://www.infineon.com/dgdl/po-to220ab-fp.pdf?fileId=5546d462580663ef0158068cfbee01be" H 3150 6000 50  0001 L CNN "footprint url"
F 17 "170nC @ 10V" H 3150 6100 50  0001 L CNN "gate charge at vgs"
F 18 "20V" H 3150 6200 50  0001 L CNN "gate to source voltage"
F 19 "19.8mm" H 3150 6300 50  0001 L CNN "height"
F 20 "5480pF @ 25V" H 3150 6400 50  0001 L CNN "input capacitace at vds"
F 21 "TO-220" H 3150 6500 50  0001 L CNN "ipc land pattern name"
F 22 "Yes" H 3150 6600 50  0001 L CNN "lead free"
F 23 "802d71249bb95e7c" H 3150 6700 50  0001 L CNN "library id"
F 24 "International Rectifier" H 3150 6800 50  0001 L CNN "manufacturer"
F 25 "1.3V" H 3150 6900 50  0001 L CNN "max forward diode voltage"
F 26 "+175°C" H 3150 7000 50  0001 L CNN "max junction temp"
F 27 "MOSFET Operating temperature: -55...+175 °C Housing type: TO-220AB Polarity: N Power dissipation: 330 W" H 3150 7100 50  0001 L CNN "mouser description"
F 28 "942-IRF1405PBF" H 3150 7200 50  0001 L CNN "mouser part number"
F 29 "1" H 3150 7300 50  0001 L CNN "number of N channels"
F 30 "1" H 3150 7400 50  0001 L CNN "number of channels"
F 31 "TO-220AB" H 3150 7500 50  0001 L CNN "package"
F 32 "330W" H 3150 7600 50  0001 L CNN "power dissipation"
F 33 "680A" H 3150 7700 50  0001 L CNN "pulse drain current"
F 34 "250nC" H 3150 7800 50  0001 L CNN "reverse recovery charge"
F 35 "88ns" H 3150 7900 50  0001 L CNN "reverse recovery time"
F 36 "Yes" H 3150 8000 50  0001 L CNN "rohs"
F 37 "62°C/W" H 3150 8100 50  0001 L CNN "rthja max"
F 38 "3.37mm" H 3150 8200 50  0001 L CNN "standoff height"
F 39 "+175°C" H 3150 8300 50  0001 L CNN "temperature range high"
F 40 "-55°C" H 3150 8400 50  0001 L CNN "temperature range low"
F 41 "4V" H 3150 8500 50  0001 L CNN "threshold vgs max"
F 42 "2V" H 3150 8600 50  0001 L CNN "threshold vgs min"
F 43 "130ns" H 3150 8700 50  0001 L CNN "turn off delay time"
F 44 "13ns" H 3150 8800 50  0001 L CNN "turn on delay time"
	1    3150 4100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 62475E33
P 3550 4300
F 0 "#PWR01" H 3550 4050 50  0001 C CNN
F 1 "GND" H 3555 4127 50  0000 C CNN
F 2 "" H 3550 4300 50  0001 C CNN
F 3 "" H 3550 4300 50  0001 C CNN
	1    3550 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R6
U 1 1 62475E3D
P 2900 4100
F 0 "R6" H 2968 4146 50  0000 L CNN
F 1 "330" H 2968 4055 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 2940 4090 50  0001 C CNN
F 3 "~" H 2900 4100 50  0001 C CNN
	1    2900 4100
	0    1    -1   0   
$EndComp
Text Label 4950 3950 2    50   ~ 0
NAV_LIGHT
Text Label 2900 2500 2    50   ~ 0
FLOOD_LIGHTS
Text Label 2750 4100 2    50   ~ 0
BACKLIGHT
Text Label 4950 4700 2    50   ~ 0
STROBE_LIGHT
$Comp
L Connector:Conn_01x02_Female J10
U 1 1 624459AE
P 3900 2100
F 0 "J10" H 3928 2076 50  0000 L CNN
F 1 "FLOOD" H 3928 1985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3900 2100 50  0001 C CNN
F 3 "~" H 3900 2100 50  0001 C CNN
	1    3900 2100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 624459C3
P 3750 3650
F 0 "J9" H 3778 3626 50  0000 L CNN
F 1 "BACKLIGHT" H 3778 3535 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3750 3650 50  0001 C CNN
F 3 "~" H 3750 3650 50  0001 C CNN
	1    3750 3650
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0111
U 1 1 624459CD
P 3550 3650
F 0 "#PWR0111" H 3550 3500 50  0001 C CNN
F 1 "+12V" H 3565 3823 50  0000 C CNN
F 2 "" H 3550 3650 50  0001 C CNN
F 3 "" H 3550 3650 50  0001 C CNN
	1    3550 3650
	1    0    0    -1  
$EndComp
NoConn ~ 9850 4850
NoConn ~ 9750 4850
NoConn ~ 9650 4850
NoConn ~ 9550 4850
NoConn ~ 9250 4850
NoConn ~ 9250 5850
NoConn ~ 9350 5850
NoConn ~ 9550 5850
NoConn ~ 9650 5850
NoConn ~ 9750 5850
NoConn ~ 9850 5850
NoConn ~ 10250 4850
NoConn ~ 10350 5850
NoConn ~ 10250 5850
NoConn ~ 10350 4850
NoConn ~ 10600 4000
NoConn ~ 10600 4100
Text Label 5100 1250 2    50   ~ 0
FORMATION_LIGHTS
$Comp
L Device:C C2
U 1 1 624FD5F5
P 1000 1000
F 0 "C2" H 1115 1046 50  0000 L CNN
F 1 "0.1uF" H 1115 955 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D4.3mm_W1.9mm_P5.00mm" H 1038 850 50  0001 C CNN
F 3 "~" H 1000 1000 50  0001 C CNN
	1    1000 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C1
U 1 1 624FE41F
P 650 1000
F 0 "C1" H 768 1046 50  0000 L CNN
F 1 "100uF" H 768 955 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.50mm" H 688 850 50  0001 C CNN
F 3 "~" H 650 1000 50  0001 C CNN
	1    650  1000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 624FF265
P 1000 1150
F 0 "#PWR0112" H 1000 900 50  0001 C CNN
F 1 "GND" H 1005 977 50  0000 C CNN
F 2 "" H 1000 1150 50  0001 C CNN
F 3 "" H 1000 1150 50  0001 C CNN
	1    1000 1150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 624FFC3F
P 650 1150
F 0 "#PWR0113" H 650 900 50  0001 C CNN
F 1 "GND" H 655 977 50  0000 C CNN
F 2 "" H 650 1150 50  0001 C CNN
F 3 "" H 650 1150 50  0001 C CNN
	1    650  1150
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0114
U 1 1 625018E0
P 650 850
F 0 "#PWR0114" H 650 700 50  0001 C CNN
F 1 "+12V" H 665 1023 50  0000 C CNN
F 2 "" H 650 850 50  0001 C CNN
F 3 "" H 650 850 50  0001 C CNN
	1    650  850 
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0115
U 1 1 62502A8D
P 1000 850
F 0 "#PWR0115" H 1000 700 50  0001 C CNN
F 1 "+12V" H 1015 1023 50  0000 C CNN
F 2 "" H 1000 850 50  0001 C CNN
F 3 "" H 1000 850 50  0001 C CNN
	1    1000 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R9
U 1 1 62463EEC
P 3300 2650
F 0 "R9" H 3368 2696 50  0000 L CNN
F 1 "10K" H 3368 2605 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 3340 2640 50  0001 C CNN
F 3 "~" H 3300 2650 50  0001 C CNN
	1    3300 2650
	1    0    0    1   
$EndComp
Connection ~ 3300 2500
Wire Wire Line
	3300 2500 3200 2500
$Comp
L Device:R_US R12
U 1 1 62466E4F
P 5550 1400
F 0 "R12" H 5618 1446 50  0000 L CNN
F 1 "10K" H 5618 1355 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5590 1390 50  0001 C CNN
F 3 "~" H 5550 1400 50  0001 C CNN
	1    5550 1400
	1    0    0    1   
$EndComp
Connection ~ 5550 1250
Wire Wire Line
	5550 1250 5400 1250
$Comp
L power:GND #PWR08
U 1 1 6246F490
P 3150 4400
F 0 "#PWR08" H 3150 4150 50  0001 C CNN
F 1 "GND" H 3155 4227 50  0000 C CNN
F 2 "" H 3150 4400 50  0001 C CNN
F 3 "" H 3150 4400 50  0001 C CNN
	1    3150 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R10
U 1 1 62473EF9
P 5350 4850
F 0 "R10" H 5418 4896 50  0000 L CNN
F 1 "10K" H 5418 4805 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5390 4840 50  0001 C CNN
F 3 "~" H 5350 4850 50  0001 C CNN
	1    5350 4850
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR025
U 1 1 62473EFF
P 5350 5000
F 0 "#PWR025" H 5350 4750 50  0001 C CNN
F 1 "GND" H 5355 4827 50  0000 C CNN
F 2 "" H 5350 5000 50  0001 C CNN
F 3 "" H 5350 5000 50  0001 C CNN
	1    5350 5000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R11
U 1 1 62479292
P 5400 4100
F 0 "R11" H 5468 4146 50  0000 L CNN
F 1 "10K" H 5468 4055 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5440 4090 50  0001 C CNN
F 3 "~" H 5400 4100 50  0001 C CNN
	1    5400 4100
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR026
U 1 1 62479298
P 5400 4250
F 0 "#PWR026" H 5400 4000 50  0001 C CNN
F 1 "GND" H 5405 4077 50  0000 C CNN
F 2 "" H 5400 4250 50  0001 C CNN
F 3 "" H 5400 4250 50  0001 C CNN
	1    5400 4250
	1    0    0    -1  
$EndComp
NoConn ~ 9950 4850
NoConn ~ 10050 4850
NoConn ~ 10150 4850
NoConn ~ 10150 5850
NoConn ~ 10050 5850
Text Label 9450 5850 3    50   ~ 0
NAV_LIGHT
Text Label 9450 4850 1    50   ~ 0
STROBE_LIGHT
Text Label 9350 4850 1    50   ~ 0
BACKLIGHT
Text Label 10600 2050 0    50   ~ 0
FORMATION_LIGHTS
Text Label 10400 2450 0    50   ~ 0
FLOOD_LIGHTS
$Comp
L Hornet-Forward-Output-Upper-Shield-rescue:IRF1405PBF-Mosfet Q6
U 1 1 6252C1BD
P 3250 1250
F 0 "Q6" H 3778 1296 50  0000 L CNN
F 1 "IRF1405PBF" H 3778 1205 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 3250 1750 50  0001 L CNN
F 3 "https://www.infineon.com/dgdl/irf1405pbf.pdf?fileId=5546d462533600a4015355db084a18bb" H 3250 1850 50  0001 L CNN
F 4 "No" H 3250 1950 50  0001 L CNN "automotive"
F 5 "Trans" H 3250 2050 50  0001 L CNN "category"
F 6 "169A" H 3250 2150 50  0001 L CNN "continuous drain current"
F 7 "False" H 3250 2250 50  0001 L CNN "depletion mode"
F 8 "Discrete Semiconductors" H 3250 2350 50  0001 L CNN "device class L1"
F 9 "Transistors" H 3250 2450 50  0001 L CNN "device class L2"
F 10 "MOSFETs" H 3250 2550 50  0001 L CNN "device class L3"
F 11 "MOSFET N-CH 55V 169A TO-220AB" H 3250 2650 50  0001 L CNN "digikey description"
F 12 "IRF1405PBF-ND" H 3250 2750 50  0001 L CNN "digikey part number"
F 13 "55V" H 3250 2850 50  0001 L CNN "drain to source breakdown voltage"
F 14 "4.6mΩ" H 3250 2950 50  0001 L CNN "drain to source resistance"
F 15 "55V" H 3250 3050 50  0001 L CNN "drain to source voltage"
F 16 "https://www.infineon.com/dgdl/po-to220ab-fp.pdf?fileId=5546d462580663ef0158068cfbee01be" H 3250 3150 50  0001 L CNN "footprint url"
F 17 "170nC @ 10V" H 3250 3250 50  0001 L CNN "gate charge at vgs"
F 18 "20V" H 3250 3350 50  0001 L CNN "gate to source voltage"
F 19 "19.8mm" H 3250 3450 50  0001 L CNN "height"
F 20 "5480pF @ 25V" H 3250 3550 50  0001 L CNN "input capacitace at vds"
F 21 "TO-220" H 3250 3650 50  0001 L CNN "ipc land pattern name"
F 22 "Yes" H 3250 3750 50  0001 L CNN "lead free"
F 23 "802d71249bb95e7c" H 3250 3850 50  0001 L CNN "library id"
F 24 "International Rectifier" H 3250 3950 50  0001 L CNN "manufacturer"
F 25 "1.3V" H 3250 4050 50  0001 L CNN "max forward diode voltage"
F 26 "+175°C" H 3250 4150 50  0001 L CNN "max junction temp"
F 27 "MOSFET Operating temperature: -55...+175 °C Housing type: TO-220AB Polarity: N Power dissipation: 330 W" H 3250 4250 50  0001 L CNN "mouser description"
F 28 "942-IRF1405PBF" H 3250 4350 50  0001 L CNN "mouser part number"
F 29 "1" H 3250 4450 50  0001 L CNN "number of N channels"
F 30 "1" H 3250 4550 50  0001 L CNN "number of channels"
F 31 "TO-220AB" H 3250 4650 50  0001 L CNN "package"
F 32 "330W" H 3250 4750 50  0001 L CNN "power dissipation"
F 33 "680A" H 3250 4850 50  0001 L CNN "pulse drain current"
F 34 "250nC" H 3250 4950 50  0001 L CNN "reverse recovery charge"
F 35 "88ns" H 3250 5050 50  0001 L CNN "reverse recovery time"
F 36 "Yes" H 3250 5150 50  0001 L CNN "rohs"
F 37 "62°C/W" H 3250 5250 50  0001 L CNN "rthja max"
F 38 "3.37mm" H 3250 5350 50  0001 L CNN "standoff height"
F 39 "+175°C" H 3250 5450 50  0001 L CNN "temperature range high"
F 40 "-55°C" H 3250 5550 50  0001 L CNN "temperature range low"
F 41 "4V" H 3250 5650 50  0001 L CNN "threshold vgs max"
F 42 "2V" H 3250 5750 50  0001 L CNN "threshold vgs min"
F 43 "130ns" H 3250 5850 50  0001 L CNN "turn off delay time"
F 44 "13ns" H 3250 5950 50  0001 L CNN "turn on delay time"
	1    3250 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R13
U 1 1 6252C1D1
P 3000 1250
F 0 "R13" H 3068 1296 50  0000 L CNN
F 1 "330" H 3068 1205 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 3040 1240 50  0001 C CNN
F 3 "~" H 3000 1250 50  0001 C CNN
	1    3000 1250
	0    1    -1   0   
$EndComp
Wire Wire Line
	3350 1250 3250 1250
Text Label 2850 1250 2    50   ~ 0
MAP_LIGHT
$Comp
L Device:R_US R14
U 1 1 6252C1DE
P 3250 1400
F 0 "R14" H 3318 1446 50  0000 L CNN
F 1 "10K" H 3318 1355 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 3290 1390 50  0001 C CNN
F 3 "~" H 3250 1400 50  0001 C CNN
	1    3250 1400
	1    0    0    1   
$EndComp
Connection ~ 3250 1250
Wire Wire Line
	3250 1250 3150 1250
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 62538AD3
P 3850 850
F 0 "J3" H 3878 826 50  0000 L CNN
F 1 "MAP" H 3878 735 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3850 850 50  0001 C CNN
F 3 "~" H 3850 850 50  0001 C CNN
	1    3850 850 
	1    0    0    -1  
$EndComp
Text Label 10400 2550 0    50   ~ 0
MAP_LIGHT
$Comp
L Device:R_US R8
U 1 1 6246F48A
P 3150 4250
F 0 "R8" H 3218 4296 50  0000 L CNN
F 1 "10K" H 3218 4205 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 3190 4240 50  0001 C CNN
F 3 "~" H 3150 4250 50  0001 C CNN
	1    3150 4250
	1    0    0    1   
$EndComp
Wire Wire Line
	3050 4100 3150 4100
Connection ~ 3150 4100
Wire Wire Line
	3150 4100 3250 4100
Wire Wire Line
	5250 4700 5350 4700
Connection ~ 5350 4700
Wire Wire Line
	5350 4700 5450 4700
Wire Wire Line
	5400 4000 5400 3950
Connection ~ 5400 3950
Wire Wire Line
	5400 3950 5250 3950
NoConn ~ 8250 5400
NoConn ~ 8900 2450
NoConn ~ 8900 2550
NoConn ~ 8900 2650
NoConn ~ 8900 2750
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 626850B3
P 1000 1800
F 0 "J2" H 1028 1776 50  0000 L CNN
F 1 "UPPER 12V IN" H 1028 1685 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 1000 1800 50  0001 C CNN
F 3 "~" H 1000 1800 50  0001 C CNN
	1    1000 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 6268A880
P 2350 750
F 0 "J4" H 2378 726 50  0000 L CNN
F 1 "LOWER 12V OUT" H 2378 635 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2350 750 50  0001 C CNN
F 3 "~" H 2350 750 50  0001 C CNN
	1    2350 750 
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0106
U 1 1 62691514
P 800 1900
F 0 "#PWR0106" H 800 1650 50  0001 C CNN
F 1 "GNDD" H 804 1745 50  0000 C CNN
F 2 "" H 800 1900 50  0001 C CNN
F 3 "" H 800 1900 50  0001 C CNN
	1    800  1900
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0109
U 1 1 6269A6B5
P 3700 2700
F 0 "#PWR0109" H 3700 2450 50  0001 C CNN
F 1 "GNDD" H 3704 2545 50  0000 C CNN
F 2 "" H 3700 2700 50  0001 C CNN
F 3 "" H 3700 2700 50  0001 C CNN
	1    3700 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0110
U 1 1 6269B3E1
P 3300 2800
F 0 "#PWR0110" H 3300 2550 50  0001 C CNN
F 1 "GNDD" H 3304 2645 50  0000 C CNN
F 2 "" H 3300 2800 50  0001 C CNN
F 3 "" H 3300 2800 50  0001 C CNN
	1    3300 2800
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0116
U 1 1 6269BFD8
P 5900 1450
F 0 "#PWR0116" H 5900 1200 50  0001 C CNN
F 1 "GNDD" H 5904 1295 50  0000 C CNN
F 2 "" H 5900 1450 50  0001 C CNN
F 3 "" H 5900 1450 50  0001 C CNN
	1    5900 1450
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0117
U 1 1 6269D02B
P 5550 1550
F 0 "#PWR0117" H 5550 1300 50  0001 C CNN
F 1 "GNDD" H 5554 1395 50  0000 C CNN
F 2 "" H 5550 1550 50  0001 C CNN
F 3 "" H 5550 1550 50  0001 C CNN
	1    5550 1550
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0118
U 1 1 6269DD5B
P 3650 1450
F 0 "#PWR0118" H 3650 1200 50  0001 C CNN
F 1 "GNDD" H 3654 1295 50  0000 C CNN
F 2 "" H 3650 1450 50  0001 C CNN
F 3 "" H 3650 1450 50  0001 C CNN
	1    3650 1450
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0119
U 1 1 6269EBFA
P 3250 1550
F 0 "#PWR0119" H 3250 1300 50  0001 C CNN
F 1 "GNDD" H 3254 1395 50  0000 C CNN
F 2 "" H 3250 1550 50  0001 C CNN
F 3 "" H 3250 1550 50  0001 C CNN
	1    3250 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 3750 3550 3900
Wire Wire Line
	3650 950  3650 1050
Wire Wire Line
	3700 2200 3700 2300
$Comp
L power:+12VA #PWR0120
U 1 1 627200B0
P 800 1800
F 0 "#PWR0120" H 800 1650 50  0001 C CNN
F 1 "+12VA" H 815 1973 50  0000 C CNN
F 2 "" H 800 1800 50  0001 C CNN
F 3 "" H 800 1800 50  0001 C CNN
	1    800  1800
	1    0    0    -1  
$EndComp
$Comp
L power:+12VA #PWR0121
U 1 1 6272064C
P 3650 850
F 0 "#PWR0121" H 3650 700 50  0001 C CNN
F 1 "+12VA" H 3665 1023 50  0000 C CNN
F 2 "" H 3650 850 50  0001 C CNN
F 3 "" H 3650 850 50  0001 C CNN
	1    3650 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+12VA #PWR0122
U 1 1 62725C0C
P 3700 2100
F 0 "#PWR0122" H 3700 1950 50  0001 C CNN
F 1 "+12VA" H 3715 2273 50  0000 C CNN
F 2 "" H 3700 2100 50  0001 C CNN
F 3 "" H 3700 2100 50  0001 C CNN
	1    3700 2100
	1    0    0    -1  
$EndComp
$Comp
L power:+12VA #PWR0123
U 1 1 6272FC69
P 7300 1500
F 0 "#PWR0123" H 7300 1350 50  0001 C CNN
F 1 "+12VA" H 7315 1673 50  0000 C CNN
F 2 "" H 7300 1500 50  0001 C CNN
F 3 "" H 7300 1500 50  0001 C CNN
	1    7300 1500
	1    0    0    -1  
$EndComp
$Comp
L power:+12VA #PWR0124
U 1 1 62730AB5
P 7350 1000
F 0 "#PWR0124" H 7350 850 50  0001 C CNN
F 1 "+12VA" H 7365 1173 50  0000 C CNN
F 2 "" H 7350 1000 50  0001 C CNN
F 3 "" H 7350 1000 50  0001 C CNN
	1    7350 1000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 62765917
P 2150 850
F 0 "#PWR0108" H 2150 600 50  0001 C CNN
F 1 "GND" H 2155 677 50  0000 C CNN
F 2 "" H 2150 850 50  0001 C CNN
F 3 "" H 2150 850 50  0001 C CNN
	1    2150 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0125
U 1 1 62766EFE
P 2150 750
F 0 "#PWR0125" H 2150 600 50  0001 C CNN
F 1 "+12V" H 2165 923 50  0000 C CNN
F 2 "" H 2150 750 50  0001 C CNN
F 3 "" H 2150 750 50  0001 C CNN
	1    2150 750 
	1    0    0    -1  
$EndComp
NoConn ~ 9950 5850
Text Label 10550 4850 1    50   ~ 0
RELAY_1
Text Label 10550 5850 3    50   ~ 0
RELAY_2
Text Label 10450 4850 1    50   ~ 0
RELAY_3
Text Label 10450 5850 3    50   ~ 0
RELAY_4
$Comp
L Connector:Conn_01x06_Female J15
U 1 1 627F3B3E
P 6250 6750
F 0 "J15" H 5900 7050 50  0000 L CNN
F 1 "RELAY OUT" H 6050 7050 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 6250 6750 50  0001 C CNN
F 3 "~" H 6250 6750 50  0001 C CNN
	1    6250 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 627F66A5
P 6050 7050
F 0 "#PWR02" H 6050 6800 50  0001 C CNN
F 1 "GND" H 6055 6877 50  0000 C CNN
F 2 "" H 6050 7050 50  0001 C CNN
F 3 "" H 6050 7050 50  0001 C CNN
	1    6050 7050
	1    0    0    -1  
$EndComp
Text Label 6050 6550 2    50   ~ 0
RELAY_1
Text Label 6050 6650 2    50   ~ 0
RELAY_2
Text Label 6050 6750 2    50   ~ 0
RELAY_3
Text Label 6050 6850 2    50   ~ 0
RELAY_4
NoConn ~ 6050 6950
$Comp
L Connector:Conn_01x08_Female J16
U 1 1 62BA5FC7
P 6450 2700
F 0 "J16" H 6478 2676 50  0000 L CNN
F 1 "Compass" H 6478 2585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 6450 2700 50  0001 C CNN
F 3 "~" H 6450 2700 50  0001 C CNN
	1    6450 2700
	1    0    0    -1  
$EndComp
Text Label 6250 2400 2    50   ~ 0
Compass_Stepper_A
Text Label 6250 2500 2    50   ~ 0
Compass_Stepper_B
Text Label 6250 2600 2    50   ~ 0
Compass_Stepper_C
Text Label 6250 2700 2    50   ~ 0
Compass_Stepper_D
NoConn ~ 6250 3100
Text Label 6250 2800 2    50   ~ 0
Compass_Zero_Sensor
Text Label 6250 2900 2    50   ~ 0
UPPER_OUTPUT_5V
$Comp
L power:GNDD #PWR07
U 1 1 62BB8EAE
P 5950 3000
F 0 "#PWR07" H 5950 2750 50  0001 C CNN
F 1 "GNDD" H 5954 2845 50  0000 C CNN
F 2 "" H 5950 3000 50  0001 C CNN
F 3 "" H 5950 3000 50  0001 C CNN
	1    5950 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3000 5950 3000
Text Label 10600 1650 0    50   ~ 0
Compass_Stepper_A
Text Label 10600 1750 0    50   ~ 0
Compass_Stepper_B
Text Label 10400 2850 0    50   ~ 0
Compass_Stepper_C
Text Label 10400 2950 0    50   ~ 0
Compass_Stepper_D
Text Label 10600 1950 0    50   ~ 0
Compass_Zero_Sensor
$EndSCHEMATC