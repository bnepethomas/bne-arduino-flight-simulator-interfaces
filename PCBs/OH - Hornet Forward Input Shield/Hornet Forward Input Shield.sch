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
Text Label 10500 4800 1    60   ~ 0
22
Text Label 10400 4800 1    60   ~ 0
24
Text Label 10300 4800 1    60   ~ 0
26
Text Label 10200 4800 1    60   ~ 0
28
Text Label 10100 4800 1    60   ~ 0
30
Text Label 10000 4800 1    60   ~ 0
32
Text Label 9900 4800 1    60   ~ 0
34
Text Label 9800 4800 1    60   ~ 0
36
Text Label 9700 4800 1    60   ~ 0
38
Text Label 9600 4800 1    60   ~ 0
40
Text Label 9500 4800 1    60   ~ 0
42
Text Label 9400 4800 1    60   ~ 0
44
Text Label 9300 4800 1    60   ~ 0
46
Text Label 9200 4800 1    60   ~ 0
48
Text Label 10500 5550 1    60   ~ 0
23
Text Label 10400 5550 1    60   ~ 0
25
Text Label 10300 5550 1    60   ~ 0
27
Text Label 10100 5550 1    60   ~ 0
31
Text Label 10200 5550 1    60   ~ 0
29
Text Label 10000 5550 1    60   ~ 0
33
Text Label 9900 5550 1    60   ~ 0
35
Text Label 9800 5550 1    60   ~ 0
37
Text Label 9700 5550 1    60   ~ 0
39
Text Label 9600 5550 1    60   ~ 0
41
Text Label 9500 5550 1    60   ~ 0
43
Text Label 9400 5550 1    60   ~ 0
45
Text Label 9300 5550 1    60   ~ 0
47
Text Label 9200 5550 1    60   ~ 0
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
Text Label 10700 1550 0    60   ~ 0
13(**)
Text Label 10700 1650 0    60   ~ 0
12(**)
Text Label 10700 1750 0    60   ~ 0
11(**)
Text Label 10700 1950 0    60   ~ 0
9(**)
Text Label 10700 2050 0    60   ~ 0
8(**)
Text Label 10700 2450 0    60   ~ 0
7(**)
Text Label 10700 2550 0    60   ~ 0
6(**)
Text Label 10700 2650 0    60   ~ 0
5(**)
Text Label 10700 2850 0    60   ~ 0
3(**)
Text Label 10700 2950 0    60   ~ 0
2(**)
Text Label 10700 3050 0    60   ~ 0
1(Tx0)
Text Label 10700 3150 0    60   ~ 0
0(Rx0)
Text Label 10700 1350 0    60   ~ 0
AREF
Text Notes 8375 575  0    60   ~ 0
Shield for Arduino Mega Rev 3
Text Notes 9650 1350 0    60   ~ 0
1
$Comp
L power:+3V3 #PWR023
U 1 1 56D71AA9
P 9100 1200
F 0 "#PWR023" H 9100 1050 50  0001 C CNN
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
	10150 1550 10400 1550
Wire Wire Line
	10400 1650 10150 1650
Wire Wire Line
	10150 1750 10400 1750
Wire Wire Line
	10400 1950 10150 1950
Wire Wire Line
	10150 2050 10400 2050
Wire Wire Line
	10250 2150 10250 1450
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
	10500 4850 10500 4650
Wire Wire Line
	10400 4850 10400 4650
Wire Wire Line
	10300 4850 10300 4650
Wire Wire Line
	10200 4850 10200 4650
Wire Wire Line
	10100 4850 10100 4650
Wire Wire Line
	10000 4850 10000 4650
Wire Wire Line
	9900 4850 9900 4650
Wire Wire Line
	9800 4850 9800 4650
Wire Wire Line
	9700 4850 9700 4650
Wire Wire Line
	9600 4850 9600 4650
Wire Wire Line
	9500 4850 9500 4650
Wire Wire Line
	9400 4850 9400 4650
Wire Wire Line
	9300 4850 9300 4650
Wire Wire Line
	9200 4850 9200 4650
Wire Wire Line
	10500 5350 10500 5650
Wire Wire Line
	10400 5350 10400 5650
Wire Wire Line
	10300 5350 10300 5650
Wire Wire Line
	10200 5350 10200 5650
Wire Wire Line
	10100 5350 10100 5650
Wire Wire Line
	10000 5350 10000 5650
Wire Wire Line
	9900 5350 9900 5650
Wire Wire Line
	9800 5350 9800 5650
Wire Wire Line
	9700 5350 9700 5650
Wire Wire Line
	9600 5350 9600 5650
Wire Wire Line
	9500 5350 9500 5650
Wire Wire Line
	9400 5350 9400 5650
Wire Wire Line
	9300 5350 9300 5650
Wire Wire Line
	9200 5350 9200 5650
Wire Wire Line
	8900 4850 8650 4850
$Comp
L power:GND #PWR021
U 1 1 56D758F6
P 8650 5750
F 0 "#PWR021" H 8650 5500 50  0001 C CNN
F 1 "GND" H 8650 5600 50  0000 C CNN
F 2 "" H 8650 5750 50  0000 C CNN
F 3 "" H 8650 5750 50  0000 C CNN
	1    8650 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 5350 8650 5350
Connection ~ 8650 5350
Wire Wire Line
	10750 5350 10600 5350
Wire Wire Line
	10750 4850 10600 4850
$Comp
L power:+5V #PWR026
U 1 1 56D75AB8
P 10750 4550
F 0 "#PWR026" H 10750 4400 50  0001 C CNN
F 1 "+5V" H 10750 4690 50  0000 C CNN
F 2 "" H 10750 4550 50  0000 C CNN
F 3 "" H 10750 4550 50  0000 C CNN
	1    10750 4550
	1    0    0    -1  
$EndComp
Connection ~ 10750 4850
Wire Wire Line
	10750 4550 10750 4850
Wire Wire Line
	10750 4850 10750 5350
Wire Wire Line
	8650 4850 8650 5350
Wire Wire Line
	8650 5350 8650 5750
Wire Notes Line
	11200 6050 8350 6050
Wire Notes Line
	8350 6050 8350 500 
NoConn ~ 10150 1850
Text Notes 10300 1850 0    50   ~ 0
USED BY ETHERNET SHIELD
NoConn ~ 10150 2750
Text Notes 10250 2750 0    50   ~ 0
USED BY ETHERNET SHIELD
NoConn ~ 9100 4850
NoConn ~ 9000 4850
NoConn ~ 9000 5350
NoConn ~ 9100 5350
Text Notes 9100 4800 2    50   ~ 0
USED BY\nETHERNET\nSHIELD
Text Notes 9100 5650 2    50   ~ 0
USED BY\nETHERNET\nSHIELD
NoConn ~ 2400 800 
NoConn ~ 2400 4100
NoConn ~ 2400 4000
NoConn ~ 2400 3900
NoConn ~ 2400 3800
NoConn ~ 2400 3700
NoConn ~ 2400 3600
Text Label 2400 3500 2    50   ~ 0
COL10
Text Label 2400 3400 2    50   ~ 0
COL9
Text Label 2400 3300 2    50   ~ 0
COL8
Text Label 2400 3200 2    50   ~ 0
COL7
Text Label 2400 3100 2    50   ~ 0
COL6
Text Label 2400 3000 2    50   ~ 0
COL5
Text Label 2400 2900 2    50   ~ 0
COL4
Text Label 2400 2800 2    50   ~ 0
COL3
Text Label 2400 2700 2    50   ~ 0
COL2
Text Label 2400 2600 2    50   ~ 0
COL1
Text Label 2400 2500 2    50   ~ 0
COL0
Text Label 2400 2400 2    50   ~ 0
ROW15
Text Label 2400 2300 2    50   ~ 0
ROW14
Text Label 2400 2200 2    50   ~ 0
ROW13
Text Label 2400 2100 2    50   ~ 0
ROW12
Text Label 2400 2000 2    50   ~ 0
ROW11
Text Label 2400 1900 2    50   ~ 0
ROW10
Text Label 2400 1800 2    50   ~ 0
ROW9
Text Label 2400 1600 2    50   ~ 0
ROW7
Text Label 2400 1700 2    50   ~ 0
ROW8
Text Label 2400 1500 2    50   ~ 0
ROW6
Text Label 2400 1400 2    50   ~ 0
ROW5
Text Label 2400 1300 2    50   ~ 0
ROW4
Text Label 2400 1200 2    50   ~ 0
ROW3
Text Label 2400 1100 2    50   ~ 0
ROW2
Text Label 2400 1000 2    50   ~ 0
ROW1
Text Label 2400 900  2    50   ~ 0
ROW0
Wire Wire Line
	2050 2500 2400 2500
Wire Wire Line
	2050 2600 2400 2600
Wire Wire Line
	2050 2700 2400 2700
Wire Wire Line
	2050 2800 2400 2800
Wire Wire Line
	2050 2900 2400 2900
Wire Wire Line
	2050 3000 2400 3000
Wire Wire Line
	2050 3100 2400 3100
Wire Wire Line
	2050 3200 2400 3200
Wire Wire Line
	2050 3300 2400 3300
Wire Wire Line
	2050 3400 2400 3400
Wire Wire Line
	2050 3500 2400 3500
Wire Wire Line
	1500 2350 1500 2500
Wire Wire Line
	1500 3500 1750 3500
Wire Wire Line
	1750 3400 1500 3400
Connection ~ 1500 3400
Wire Wire Line
	1500 3400 1500 3500
Wire Wire Line
	1750 3300 1500 3300
Connection ~ 1500 3300
Wire Wire Line
	1500 3300 1500 3400
Wire Wire Line
	1750 3200 1500 3200
Connection ~ 1500 3200
Wire Wire Line
	1500 3200 1500 3300
Wire Wire Line
	1750 3100 1500 3100
Connection ~ 1500 3100
Wire Wire Line
	1500 3100 1500 3200
Wire Wire Line
	1750 3000 1500 3000
Connection ~ 1500 3000
Wire Wire Line
	1500 3000 1500 3100
Wire Wire Line
	1750 2900 1500 2900
Connection ~ 1500 2900
Wire Wire Line
	1500 2900 1500 3000
Wire Wire Line
	1750 2800 1500 2800
Connection ~ 1500 2800
Wire Wire Line
	1500 2800 1500 2900
Wire Wire Line
	1750 2700 1500 2700
Connection ~ 1500 2700
Wire Wire Line
	1500 2700 1500 2800
Wire Wire Line
	1750 2600 1500 2600
Connection ~ 1500 2600
Wire Wire Line
	1500 2600 1500 2700
Wire Wire Line
	1750 2500 1500 2500
Connection ~ 1500 2500
Wire Wire Line
	1500 2500 1500 2600
$Comp
L power:+5V #PWR01
U 1 1 61742883
P 1500 2350
F 0 "#PWR01" H 1500 2200 50  0001 C CNN
F 1 "+5V" H 1515 2523 50  0000 C CNN
F 2 "" H 1500 2350 50  0001 C CNN
F 3 "" H 1500 2350 50  0001 C CNN
	1    1500 2350
	1    0    0    -1  
$EndComp
Text Label 9200 4650 1    50   ~ 0
COL10
Text Label 9300 5650 3    50   ~ 0
COL9
Text Label 9300 4650 1    50   ~ 0
COL8
Text Label 9400 5650 3    50   ~ 0
COL7
Text Label 9400 4650 1    50   ~ 0
COL6
Text Label 9500 5650 3    50   ~ 0
COL5
Text Label 9500 4650 1    50   ~ 0
COL4
Text Label 9600 5650 3    50   ~ 0
COL3
Text Label 9600 4650 1    50   ~ 0
COL2
Text Label 9700 5650 3    50   ~ 0
COL1
Text Label 9700 4650 1    50   ~ 0
COL0
Text Label 9800 4650 1    50   ~ 0
ROW14
Text Label 9900 5650 3    50   ~ 0
ROW13
Text Label 9900 4650 1    50   ~ 0
ROW12
Text Label 10000 5650 3    50   ~ 0
ROW11
Text Label 10000 4650 1    50   ~ 0
ROW10
Text Label 10100 5650 3    50   ~ 0
ROW9
Text Label 10200 5650 3    50   ~ 0
ROW7
Text Label 10100 4650 1    50   ~ 0
ROW8
Text Label 10200 4650 1    50   ~ 0
ROW6
Text Label 10300 5650 3    50   ~ 0
ROW5
Text Label 10300 4650 1    50   ~ 0
ROW4
Text Label 10400 5650 3    50   ~ 0
ROW3
Text Label 10400 4650 1    50   ~ 0
ROW2
Text Label 10500 5650 3    50   ~ 0
ROW1
Text Label 10500 4650 1    50   ~ 0
ROW0
Text Label 9800 5650 3    50   ~ 0
ROW15
NoConn ~ 9200 5650
NoConn ~ -7400 5900
$Comp
L power:+5V #PWR06
U 1 1 617FDCF0
P 3250 950
F 0 "#PWR06" H 3250 800 50  0001 C CNN
F 1 "+5V" H 3265 1123 50  0000 C CNN
F 2 "" H 3250 950 50  0001 C CNN
F 3 "" H 3250 950 50  0001 C CNN
	1    3250 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 1050 2900 1050
$Comp
L power:GND #PWR04
U 1 1 618041CD
P 2900 1050
F 0 "#PWR04" H 2900 800 50  0001 C CNN
F 1 "GND" H 2905 877 50  0000 C CNN
F 2 "" H 2900 1050 50  0001 C CNN
F 3 "" H 2900 1050 50  0001 C CNN
	1    2900 1050
	1    0    0    -1  
$EndComp
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
Text Label 3250 1850 2    50   ~ 0
A15
Text Label 3250 1750 2    50   ~ 0
A14
Text Label 3250 1650 2    50   ~ 0
A13
Text Label 3250 1550 2    50   ~ 0
A12
Text Label 3250 1450 2    50   ~ 0
A11
Text Label 3250 1350 2    50   ~ 0
A10
Text Label 3250 1250 2    50   ~ 0
A9
Text Label 3250 1150 2    50   ~ 0
A8
Wire Wire Line
	2950 2950 3500 2950
$Comp
L power:GND #PWR05
U 1 1 6185E380
P 2950 2950
F 0 "#PWR05" H 2950 2700 50  0001 C CNN
F 1 "GND" H 2955 2777 50  0000 C CNN
F 2 "" H 2950 2950 50  0001 C CNN
F 3 "" H 2950 2950 50  0001 C CNN
	1    2950 2950
	1    0    0    -1  
$EndComp
Text Label 3500 3050 2    50   ~ 0
ENC-1-A
Text Label 3500 3150 2    50   ~ 0
ENC-1-B
Text Label 3500 3250 2    50   ~ 0
ENC-2-A
Text Label 3500 3350 2    50   ~ 0
ENC-2-B
Text Label 3500 3450 2    50   ~ 0
ENC-3-A
Text Label 3500 3550 2    50   ~ 0
ENC-3-B
Text Label 3500 3650 2    50   ~ 0
ENC-4-A
Text Label 3500 3750 2    50   ~ 0
ENC-4-B
NoConn ~ 3500 3850
Text Label 10600 3400 0    50   ~ 0
ENC-1-A
Text Label 10600 3500 0    50   ~ 0
ENC-1-B
Text Label 10600 3600 0    50   ~ 0
ENC-2-A
Text Label 10600 3700 0    50   ~ 0
ENC-2-B
Text Label 10600 3800 0    50   ~ 0
ENC-3-A
Text Label 10600 3900 0    50   ~ 0
ENC-3-B
Text Label 10600 4000 0    50   ~ 0
ENC-4-A
Text Label 10600 4100 0    50   ~ 0
ENC-4-B
NoConn ~ 10400 1350
Text Label 8900 1200 1    60   ~ 0
Vin
$Comp
L power:+5VL #PWR022
U 1 1 616FD070
P 9000 1050
F 0 "#PWR022" H 9000 900 50  0001 C CNN
F 1 "+5VL" H 9015 1223 50  0000 C CNN
F 2 "" H 9000 1050 50  0001 C CNN
F 3 "" H 9000 1050 50  0001 C CNN
	1    9000 1050
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR024
U 1 1 616FDC1F
P 9250 2150
F 0 "#PWR024" H 9250 1900 50  0001 C CNN
F 1 "GNDD" H 9254 1995 50  0000 C CNN
F 2 "" H 9250 2150 50  0001 C CNN
F 3 "" H 9250 2150 50  0001 C CNN
	1    9250 2150
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR025
U 1 1 616FEA6D
P 10250 2150
F 0 "#PWR025" H 10250 1900 50  0001 C CNN
F 1 "GNDD" H 10254 1995 50  0000 C CNN
F 2 "" H 10250 2150 50  0001 C CNN
F 3 "" H 10250 2150 50  0001 C CNN
	1    10250 2150
	1    0    0    -1  
$EndComp
NoConn ~ 8900 1200
NoConn ~ 9250 1200
NoConn ~ 8600 1550
$Comp
L power:+5VL #PWR09
U 1 1 616D8965
P 5050 1050
F 0 "#PWR09" H 5050 900 50  0001 C CNN
F 1 "+5VL" H 5065 1223 50  0000 C CNN
F 2 "" H 5050 1050 50  0001 C CNN
F 3 "" H 5050 1050 50  0001 C CNN
	1    5050 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1150 5050 1150
$Comp
L power:GNDD #PWR07
U 1 1 616DEB3A
P 4450 1150
F 0 "#PWR07" H 4450 900 50  0001 C CNN
F 1 "GNDD" H 4454 995 50  0000 C CNN
F 2 "" H 4450 1150 50  0001 C CNN
F 3 "" H 4450 1150 50  0001 C CNN
	1    4450 1150
	1    0    0    -1  
$EndComp
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
Text Label 5050 1250 2    50   ~ 0
A0
Text Label 5050 1350 2    50   ~ 0
A1
Text Label 5050 1450 2    50   ~ 0
A2
Text Label 5050 1550 2    50   ~ 0
A3
Text Label 5050 1650 2    50   ~ 0
A4
Text Label 5050 1750 2    50   ~ 0
A5
Wire Wire Line
	6100 2300 5800 2300
Wire Wire Line
	5500 2300 5800 2300
Connection ~ 5800 2300
Wire Wire Line
	5500 2300 5350 2300
Connection ~ 5500 2300
Wire Wire Line
	5200 2300 4900 2300
Connection ~ 5200 2300
Wire Wire Line
	4900 2300 4600 2300
Connection ~ 4900 2300
Wire Wire Line
	6700 3100 6400 3100
Wire Wire Line
	6400 3100 6100 3100
Connection ~ 6400 3100
Wire Wire Line
	6100 3100 5800 3100
Connection ~ 6100 3100
Wire Wire Line
	5800 3100 5650 3100
Connection ~ 5800 3100
Wire Wire Line
	5500 3100 5200 3100
Connection ~ 5500 3100
Wire Wire Line
	5200 3100 4900 3100
Connection ~ 5200 3100
Wire Wire Line
	4900 3100 4600 3100
Connection ~ 4900 3100
$Comp
L power:GNDD #PWR011
U 1 1 6175428E
P 5350 2300
F 0 "#PWR011" H 5350 2050 50  0001 C CNN
F 1 "GNDD" H 5354 2145 50  0000 C CNN
F 2 "" H 5350 2300 50  0001 C CNN
F 3 "" H 5350 2300 50  0001 C CNN
	1    5350 2300
	1    0    0    -1  
$EndComp
Connection ~ 5350 2300
Wire Wire Line
	5350 2300 5200 2300
$Comp
L power:GND #PWR012
U 1 1 61754F34
P 5650 3100
F 0 "#PWR012" H 5650 2850 50  0001 C CNN
F 1 "GND" H 5655 2927 50  0000 C CNN
F 2 "" H 5650 3100 50  0001 C CNN
F 3 "" H 5650 3100 50  0001 C CNN
	1    5650 3100
	1    0    0    -1  
$EndComp
Connection ~ 5650 3100
Wire Wire Line
	5650 3100 5500 3100
Text Label 6100 2000 1    50   ~ 0
A0
Text Label 5800 2000 1    50   ~ 0
A1
Text Label 5500 2000 1    50   ~ 0
A2
Text Label 5200 2000 1    50   ~ 0
A3
Text Label 4900 2000 1    50   ~ 0
A4
Text Label 4600 2000 1    50   ~ 0
A5
Text Label 4600 2800 1    50   ~ 0
A15
Text Label 4900 2800 1    50   ~ 0
A14
Text Label 5200 2800 1    50   ~ 0
A13
Text Label 5500 2800 1    50   ~ 0
A12
Text Label 5800 2800 1    50   ~ 0
A11
Text Label 6100 2800 1    50   ~ 0
A10
Text Label 6400 2800 1    50   ~ 0
A9
Text Label 6700 2800 1    50   ~ 0
A8
Text Label 10400 1550 0    50   ~ 0
D13
Text Label 10400 1650 0    50   ~ 0
D12
Text Label 10400 1750 0    50   ~ 0
D11
Text Label 10400 1950 0    50   ~ 0
D9
Text Label 10400 2050 0    50   ~ 0
D8
Text Label 10400 2450 0    50   ~ 0
D7
Text Label 10400 2550 0    50   ~ 0
D6
Text Label 10400 2650 0    50   ~ 0
D5
Text Label 10400 2850 0    50   ~ 0
D3
Text Label 10400 2950 0    50   ~ 0
D2
Text Label 10400 3050 0    50   ~ 0
D1
Text Label 10400 3150 0    50   ~ 0
D0
Text Label 7550 2750 2    50   ~ 0
D13
Text Label 7550 2650 2    50   ~ 0
D12
Text Label 7550 2550 2    50   ~ 0
D11
Text Label 7550 2450 2    50   ~ 0
D9
Text Label 7500 2000 2    50   ~ 0
D8
Text Label 7500 1900 2    50   ~ 0
D7
Text Label 7500 1800 2    50   ~ 0
D6
Text Label 7500 1700 2    50   ~ 0
D5
Text Label 7500 1250 2    50   ~ 0
D3
Text Label 7500 1150 2    50   ~ 0
D2
Text Label 7500 1050 2    50   ~ 0
D1
Text Label 7500 950  2    50   ~ 0
D0
$Comp
L power:+5VL #PWR018
U 1 1 6182A45A
P 7500 750
F 0 "#PWR018" H 7500 600 50  0001 C CNN
F 1 "+5VL" H 7515 923 50  0000 C CNN
F 2 "" H 7500 750 50  0001 C CNN
F 3 "" H 7500 750 50  0001 C CNN
	1    7500 750 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 850  7150 850 
$Comp
L power:GNDD #PWR015
U 1 1 61831204
P 7150 850
F 0 "#PWR015" H 7150 600 50  0001 C CNN
F 1 "GNDD" H 7154 695 50  0000 C CNN
F 2 "" H 7150 850 50  0001 C CNN
F 3 "" H 7150 850 50  0001 C CNN
	1    7150 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+5VL #PWR019
U 1 1 61838201
P 7500 1500
F 0 "#PWR019" H 7500 1350 50  0001 C CNN
F 1 "+5VL" H 7515 1673 50  0000 C CNN
F 2 "" H 7500 1500 50  0001 C CNN
F 3 "" H 7500 1500 50  0001 C CNN
	1    7500 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 1600 7150 1600
$Comp
L power:GNDD #PWR016
U 1 1 6183820C
P 7150 1600
F 0 "#PWR016" H 7150 1350 50  0001 C CNN
F 1 "GNDD" H 7154 1445 50  0000 C CNN
F 2 "" H 7150 1600 50  0001 C CNN
F 3 "" H 7150 1600 50  0001 C CNN
	1    7150 1600
	1    0    0    -1  
$EndComp
$Comp
L power:+5VL #PWR020
U 1 1 61851AB5
P 7550 2250
F 0 "#PWR020" H 7550 2100 50  0001 C CNN
F 1 "+5VL" H 7565 2423 50  0000 C CNN
F 2 "" H 7550 2250 50  0001 C CNN
F 3 "" H 7550 2250 50  0001 C CNN
	1    7550 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 2350 7200 2350
$Comp
L power:GNDD #PWR017
U 1 1 61851AC0
P 7200 2350
F 0 "#PWR017" H 7200 2100 50  0001 C CNN
F 1 "GNDD" H 7204 2195 50  0000 C CNN
F 2 "" H 7200 2350 50  0001 C CNN
F 3 "" H 7200 2350 50  0001 C CNN
	1    7200 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 4750 5150 4900
$Comp
L power:GNDD #PWR08
U 1 1 616F7A06
P 5150 5250
F 0 "#PWR08" H 5150 5000 50  0001 C CNN
F 1 "GNDD" H 5154 5095 50  0000 C CNN
F 2 "" H 5150 5250 50  0001 C CNN
F 3 "" H 5150 5250 50  0001 C CNN
	1    5150 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 4750 5750 4900
$Comp
L power:GNDD #PWR010
U 1 1 616F7F1C
P 5750 5250
F 0 "#PWR010" H 5750 5000 50  0001 C CNN
F 1 "GNDD" H 5754 5095 50  0000 C CNN
F 2 "" H 5750 5250 50  0001 C CNN
F 3 "" H 5750 5250 50  0001 C CNN
	1    5750 5250
	1    0    0    -1  
$EndComp
Text Label 5150 4450 0    50   ~ 0
D5
$Comp
L power:GNDD #PWR013
U 1 1 61703EDF
P 6650 4750
F 0 "#PWR013" H 6650 4500 50  0001 C CNN
F 1 "GNDD" H 6654 4595 50  0000 C CNN
F 2 "" H 6650 4750 50  0001 C CNN
F 3 "" H 6650 4750 50  0001 C CNN
	1    6650 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR014
U 1 1 61704042
P 6650 5150
F 0 "#PWR014" H 6650 4900 50  0001 C CNN
F 1 "GNDD" H 6654 4995 50  0000 C CNN
F 2 "" H 6650 5150 50  0001 C CNN
F 3 "" H 6650 5150 50  0001 C CNN
	1    6650 5150
	1    0    0    -1  
$EndComp
Text Label 6650 4650 2    50   ~ 0
D2
Text Label 6650 5050 2    50   ~ 0
D3
Text Label 5750 4450 0    50   ~ 0
D6
Wire Wire Line
	5750 5200 5750 5250
Wire Wire Line
	5150 5200 5150 5250
Text Notes 5050 5800 0    50   ~ 0
Red LED - solid\non until a packet\nis succesfully \nsent to controller
$Comp
L power:+5VL #PWR02
U 1 1 6172A180
P 2850 4400
F 0 "#PWR02" H 2850 4250 50  0001 C CNN
F 1 "+5VL" H 2865 4573 50  0000 C CNN
F 2 "" H 2850 4400 50  0001 C CNN
F 3 "" H 2850 4400 50  0001 C CNN
	1    2850 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 4500 2850 4800
Wire Wire Line
	2850 4800 3200 4800
$Comp
L power:GNDD #PWR03
U 1 1 61731E38
P 2850 4900
F 0 "#PWR03" H 2850 4650 50  0001 C CNN
F 1 "GNDD" H 2854 4745 50  0000 C CNN
F 2 "" H 2850 4900 50  0001 C CNN
F 3 "" H 2850 4900 50  0001 C CNN
	1    2850 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 4900 2850 4900
NoConn ~ 3200 5300
Text Notes 3450 5000 0    50   ~ 0
DATA IN - ORANGE
Text Notes 3450 5100 0    50   ~ 0
LOAD/CS - YELLOW
Text Notes 3450 5200 0    50   ~ 0
CLOCK - GREEN
Text Notes 2800 5800 0    50   ~ 0
LEDCONTROL Ic_2=LEDCONTROL(9,8,7,1);\n9 - DATA IN\n8 - CLK\n7 - LOAD/CS
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
P 9700 5050
F 0 "P4" H 9700 6000 50  0000 C CNN
F 1 "Digital" V 9700 5050 50  0000 C CNN
F 2 "Socket_Arduino_Mega:Socket_Strip_Arduino_2x18" H 9700 4000 50  0001 C CNN
F 3 "" H 9700 4000 50  0000 C CNN
	1    9700 5050
	0    -1   1    0   
$EndComp
$Comp
L Connector:Conn_01x34_Female J1
U 1 1 616BC0F0
P 2600 2400
F 0 "J1" H 2628 2376 50  0000 L CNN
F 1 "MATRIX INPUT" H 2628 2285 50  0000 L CNN
F 2 "Connector_IDC:IDC-Header_2x17_P2.54mm_Vertical" H 2600 2400 50  0001 C CNN
F 3 "~" H 2600 2400 50  0001 C CNN
	1    2600 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 616E2AC0
P 1900 2500
F 0 "R1" V 1950 2700 50  0000 C CNN
F 1 "10K" V 1950 2300 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 2500 50  0001 C CNN
F 3 "~" H 1900 2500 50  0001 C CNN
	1    1900 2500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 616E7E06
P 1900 2600
F 0 "R2" V 1950 2800 50  0000 C CNN
F 1 "10K" V 1950 2400 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 2600 50  0001 C CNN
F 3 "~" H 1900 2600 50  0001 C CNN
	1    1900 2600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 616E93B5
P 1900 2700
F 0 "R3" V 1950 2900 50  0000 C CNN
F 1 "10K" V 1950 2500 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 2700 50  0001 C CNN
F 3 "~" H 1900 2700 50  0001 C CNN
	1    1900 2700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 616E93BF
P 1900 2800
F 0 "R4" V 1950 3000 50  0000 C CNN
F 1 "10K" V 1950 2600 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 2800 50  0001 C CNN
F 3 "~" H 1900 2800 50  0001 C CNN
	1    1900 2800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R5
U 1 1 616ED032
P 1900 2900
F 0 "R5" V 1950 3100 50  0000 C CNN
F 1 "10K" V 1950 2700 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 2900 50  0001 C CNN
F 3 "~" H 1900 2900 50  0001 C CNN
	1    1900 2900
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R6
U 1 1 616ED03C
P 1900 3000
F 0 "R6" V 1950 3200 50  0000 C CNN
F 1 "10K" V 1950 2800 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 3000 50  0001 C CNN
F 3 "~" H 1900 3000 50  0001 C CNN
	1    1900 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R7
U 1 1 616ED046
P 1900 3100
F 0 "R7" V 1950 3300 50  0000 C CNN
F 1 "10K" V 1950 2900 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 3100 50  0001 C CNN
F 3 "~" H 1900 3100 50  0001 C CNN
	1    1900 3100
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R8
U 1 1 616ED050
P 1900 3200
F 0 "R8" V 1950 3400 50  0000 C CNN
F 1 "10K" V 1950 3000 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 3200 50  0001 C CNN
F 3 "~" H 1900 3200 50  0001 C CNN
	1    1900 3200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R9
U 1 1 616F3089
P 1900 3300
F 0 "R9" V 1950 3500 50  0000 C CNN
F 1 "10K" V 1950 3100 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 3300 50  0001 C CNN
F 3 "~" H 1900 3300 50  0001 C CNN
	1    1900 3300
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R10
U 1 1 616F3093
P 1900 3400
F 0 "R10" V 1950 3600 50  0000 C CNN
F 1 "10K" V 1950 3200 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 3400 50  0001 C CNN
F 3 "~" H 1900 3400 50  0001 C CNN
	1    1900 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R11
U 1 1 616F309D
P 1900 3500
F 0 "R11" V 1950 3700 50  0000 C CNN
F 1 "10K" V 1950 3300 50  0000 C CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1830 3500 50  0001 C CNN
F 3 "~" H 1900 3500 50  0001 C CNN
	1    1900 3500
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x10_Female J4
U 1 1 617FBCF0
P 3450 1350
F 0 "J4" H 3550 1900 50  0000 L CNN
F 1 "ANALOG B" H 3500 1800 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 3450 1350 50  0001 C CNN
F 3 "~" H 3450 1350 50  0001 C CNN
	1    3450 1350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J5
U 1 1 6185764C
P 3700 3350
F 0 "J5" H 3728 3326 50  0000 L CNN
F 1 "ENCODERS" H 3728 3235 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x10_P2.54mm_Vertical" H 3700 3350 50  0001 C CNN
F 3 "~" H 3700 3350 50  0001 C CNN
	1    3700 3350
	1    0    0    -1  
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
L Connector:Conn_01x08_Female J6
U 1 1 616C0788
P 5250 1350
F 0 "J6" H 5250 1900 50  0000 L CNN
F 1 "ANALOG A" H 5200 1750 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 5250 1350 50  0001 C CNN
F 3 "~" H 5250 1350 50  0001 C CNN
	1    5250 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 616FB1D4
P 4600 2150
F 0 "C1" H 4715 2196 50  0000 L CNN
F 1 "0.1uF" H 4715 2105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 4638 2000 50  0001 C CNN
F 3 "~" H 4600 2150 50  0001 C CNN
	1    4600 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 616FB807
P 4900 2150
F 0 "C3" H 5015 2196 50  0000 L CNN
F 1 "0.1uF" H 5015 2105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 4938 2000 50  0001 C CNN
F 3 "~" H 4900 2150 50  0001 C CNN
	1    4900 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 616FC067
P 5200 2150
F 0 "C5" H 5315 2196 50  0000 L CNN
F 1 "0.1uF" H 5315 2105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 5238 2000 50  0001 C CNN
F 3 "~" H 5200 2150 50  0001 C CNN
	1    5200 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C7
U 1 1 616FCAB3
P 5500 2150
F 0 "C7" H 5615 2196 50  0000 L CNN
F 1 "0.1uF" H 5615 2105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 5538 2000 50  0001 C CNN
F 3 "~" H 5500 2150 50  0001 C CNN
	1    5500 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 616FCABD
P 5800 2150
F 0 "C9" H 5915 2196 50  0000 L CNN
F 1 "0.1uF" H 5915 2105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 5838 2000 50  0001 C CNN
F 3 "~" H 5800 2150 50  0001 C CNN
	1    5800 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C11
U 1 1 616FCAC7
P 6100 2150
F 0 "C11" H 6215 2196 50  0000 L CNN
F 1 "0.1uF" H 6215 2105 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 6138 2000 50  0001 C CNN
F 3 "~" H 6100 2150 50  0001 C CNN
	1    6100 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 61702F78
P 4600 2950
F 0 "C2" H 4715 2996 50  0000 L CNN
F 1 "0.1uF" H 4715 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 4638 2800 50  0001 C CNN
F 3 "~" H 4600 2950 50  0001 C CNN
	1    4600 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 61702F82
P 4900 2950
F 0 "C4" H 5015 2996 50  0000 L CNN
F 1 "0.1uF" H 5015 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 4938 2800 50  0001 C CNN
F 3 "~" H 4900 2950 50  0001 C CNN
	1    4900 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 61702F8C
P 5200 2950
F 0 "C6" H 5315 2996 50  0000 L CNN
F 1 "0.1uF" H 5315 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 5238 2800 50  0001 C CNN
F 3 "~" H 5200 2950 50  0001 C CNN
	1    5200 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 61702F96
P 5500 2950
F 0 "C8" H 5615 2996 50  0000 L CNN
F 1 "0.1uF" H 5615 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 5538 2800 50  0001 C CNN
F 3 "~" H 5500 2950 50  0001 C CNN
	1    5500 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C10
U 1 1 61702FA0
P 5800 2950
F 0 "C10" H 5915 2996 50  0000 L CNN
F 1 "0.1uF" H 5915 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 5838 2800 50  0001 C CNN
F 3 "~" H 5800 2950 50  0001 C CNN
	1    5800 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C12
U 1 1 61702FAA
P 6100 2950
F 0 "C12" H 6215 2996 50  0000 L CNN
F 1 "0.1uF" H 6215 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 6138 2800 50  0001 C CNN
F 3 "~" H 6100 2950 50  0001 C CNN
	1    6100 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C13
U 1 1 6170B3C2
P 6400 2950
F 0 "C13" H 6515 2996 50  0000 L CNN
F 1 "0.1uF" H 6515 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 6438 2800 50  0001 C CNN
F 3 "~" H 6400 2950 50  0001 C CNN
	1    6400 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C14
U 1 1 6170B3CC
P 6700 2950
F 0 "C14" H 6815 2996 50  0000 L CNN
F 1 "0.1uF" H 6815 2905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 6738 2800 50  0001 C CNN
F 3 "~" H 6700 2950 50  0001 C CNN
	1    6700 2950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J9
U 1 1 61828261
P 7700 950
F 0 "J9" H 7728 926 50  0000 L CNN
F 1 "DIGITAL A" H 7728 835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7700 950 50  0001 C CNN
F 3 "~" H 7700 950 50  0001 C CNN
	1    7700 950 
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J10
U 1 1 618381F7
P 7700 1700
F 0 "J10" H 7728 1676 50  0000 L CNN
F 1 "DIGITAL B" H 7728 1585 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7700 1700 50  0001 C CNN
F 3 "~" H 7700 1700 50  0001 C CNN
	1    7700 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J11
U 1 1 61851AAB
P 7750 2450
F 0 "J11" H 7778 2426 50  0000 L CNN
F 1 "DIGITAL C" H 7778 2335 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7750 2450 50  0001 C CNN
F 3 "~" H 7750 2450 50  0001 C CNN
	1    7750 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 616EE56B
P 5150 5050
F 0 "D1" V 5189 4932 50  0000 R CNN
F 1 "GREEN" V 5098 4932 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 5150 5050 50  0001 C CNN
F 3 "~" H 5150 5050 50  0001 C CNN
	1    5150 5050
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R12
U 1 1 616EFF2F
P 5150 4600
F 0 "R12" H 5218 4646 50  0000 L CNN
F 1 "330" H 5218 4555 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5190 4590 50  0001 C CNN
F 3 "~" H 5150 4600 50  0001 C CNN
	1    5150 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 616F7F07
P 5750 5050
F 0 "D2" V 5789 4932 50  0000 R CNN
F 1 "RED" V 5698 4932 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 5750 5050 50  0001 C CNN
F 3 "~" H 5750 5050 50  0001 C CNN
	1    5750 5050
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R13
U 1 1 616F7F11
P 5750 4600
F 0 "R13" H 5818 4646 50  0000 L CNN
F 1 "330" H 5818 4555 50  0000 L CNN
F 2 "PT_Library_v001:PT_R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 5790 4590 50  0001 C CNN
F 3 "~" H 5750 4600 50  0001 C CNN
	1    5750 4600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J7
U 1 1 61703257
P 6850 4650
F 0 "J7" H 6878 4626 50  0000 L CNN
F 1 "Address Select 0" H 6878 4535 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6850 4650 50  0001 C CNN
F 3 "~" H 6850 4650 50  0001 C CNN
	1    6850 4650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 61704038
P 6850 5050
F 0 "J8" H 6878 5026 50  0000 L CNN
F 1 "Address Select 1" H 6878 4935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6850 5050 50  0001 C CNN
F 3 "~" H 6850 5050 50  0001 C CNN
	1    6850 5050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J3
U 1 1 61726457
P 3400 5000
F 0 "J3" H 3050 5300 50  0000 L CNN
F 1 "7219 OUT" H 3200 5300 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 3400 5000 50  0001 C CNN
F 3 "~" H 3400 5000 50  0001 C CNN
	1    3400 5000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 61728B28
P 3050 4400
F 0 "J2" H 3078 4376 50  0000 L CNN
F 1 "5V to Max7219" H 3078 4285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3050 4400 50  0001 C CNN
F 3 "~" H 3050 4400 50  0001 C CNN
	1    3050 4400
	1    0    0    -1  
$EndComp
Text Label 3200 5000 2    50   ~ 0
D9
Text Label 3200 5100 2    50   ~ 0
D8
Text Label 3200 5200 2    50   ~ 0
D7
Wire Notes Line
	2700 4150 2700 5950
Wire Notes Line
	2700 5950 4450 5950
Wire Notes Line
	4450 5950 4450 4150
Wire Notes Line
	4450 4150 2700 4150
Text Notes 2950 4250 0    50   ~ 0
Output block which can be removed
Text Notes 6050 4350 0    50   ~ 0
STATUS AND ADDRESS SELECT
Wire Notes Line
	4850 4150 4850 5950
Wire Notes Line
	4850 5950 7750 5950
Wire Notes Line
	7750 5950 7750 4150
Wire Notes Line
	7750 4150 4850 4150
$EndSCHEMATC