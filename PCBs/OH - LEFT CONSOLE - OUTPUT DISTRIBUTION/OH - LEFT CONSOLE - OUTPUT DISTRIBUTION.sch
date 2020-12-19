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
Text Notes 1400 900  0    50   ~ 0
APU
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 5FDF1416
P 1400 1450
F 0 "J1" H 1292 1635 50  0000 C CNN
F 1 "Conn_01x02_Female" H 1292 1544 50  0000 C CNN
F 2 "" H 1400 1450 50  0001 C CNN
F 3 "~" H 1400 1450 50  0001 C CNN
	1    1400 1450
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 5FDF1F94
P 1400 1850
F 0 "J2" H 1292 2035 50  0000 C CNN
F 1 "Conn_01x02_Female" H 1292 1944 50  0000 C CNN
F 2 "" H 1400 1850 50  0001 C CNN
F 3 "~" H 1400 1850 50  0001 C CNN
	1    1400 1850
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 5FDF243B
P 1400 2250
F 0 "J3" H 1292 2435 50  0000 C CNN
F 1 "Conn_01x02_Female" H 1292 2344 50  0000 C CNN
F 2 "" H 1400 2250 50  0001 C CNN
F 3 "~" H 1400 2250 50  0001 C CNN
	1    1400 2250
	-1   0    0    -1  
$EndComp
Text Label 1600 1550 0    50   ~ 0
COILGND
Text Label 1600 1950 0    50   ~ 0
COILGND
Text Label 1600 1450 0    50   ~ 0
COIL_A_12V
Text Label 1600 1850 0    50   ~ 0
COIL_B_12V
Text Notes 1200 1200 0    50   ~ 0
REAL HONEYWELL \nHAS ONLY A SINGLE \nCOIL
Text Label 1600 2250 0    50   ~ 0
APU_LED_ANNODE
Text Label 1600 2350 0    50   ~ 0
APU_LED_CATHODE
Wire Notes Line
	950  750  950  2450
Wire Notes Line
	950  2450 2400 2450
Wire Notes Line
	2400 2450 2400 750 
Wire Notes Line
	2400 750  950  750 
Text Notes 1250 2700 0    50   ~ 0
SELECTIVE JET
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 5FE20A6E
P 1400 3150
F 0 "J?" H 1292 3435 50  0000 C CNN
F 1 "32 HYD_IND_BRAKE_OUT" H 1292 3344 50  0000 C CNN
F 2 "" H 1400 3150 50  0001 C CNN
F 3 "~" H 1400 3150 50  0001 C CNN
	1    1400 3150
	-1   0    0    -1  
$EndComp
Wire Notes Line
	950  2550 2050 2550
Wire Notes Line
	2050 2550 2050 3450
Wire Notes Line
	2050 3450 950  3450
Wire Notes Line
	950  3450 950  2550
$EndSCHEMATC
