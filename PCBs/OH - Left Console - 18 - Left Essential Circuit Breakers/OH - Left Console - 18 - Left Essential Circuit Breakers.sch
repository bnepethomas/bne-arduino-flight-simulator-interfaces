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
-- 16. ECM Dispenser Button\ndefinePushButton("CMSD_DISPENSE_BTN", 54, 3002, 380, "ECM Dispenser Button", "Dispense Button - Push to dispense flares and chaff")\n\n-- 18. Left Essential Circuit Breakers\ndefinePushButton("CB_FCS_CHAN1", 3, 3017, 381, "Left Essential Circuit Breakers", "CB FCS CHAN 1, ON/OFF")\ndefinePushButton("CB_FCS_CHAN2", 3, 3018, 382, "Left Essential Circuit Breakers", "CB FCS CHAN 2, ON/OFF")\ndefinePushButton("CB_SPD_BRK", 3, 3019, 383, "Left Essential Circuit Breakers", "CB SPD BRK, ON/OFF")\ndefinePushButton("CB_LAUNCH_BAR", 3, 3020, 384, "Left Essential Circuit Breakers", "CB LAUNCH BAR, ON/OFF")
$Comp
L Device:D D1
U 1 1 5FC2589C
P 2450 2600
F 0 "D1" H 2450 2816 50  0000 C CNN
F 1 "D" H 2450 2725 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2450 2600 50  0001 C CNN
F 3 "~" H 2450 2600 50  0001 C CNN
	1    2450 2600
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 5FC37DA5
P 4850 3550
F 0 "J2" H 4878 3526 50  0000 L CNN
F 1 "Input Rows" H 4878 3435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 4850 3550 50  0001 C CNN
F 3 "~" H 4850 3550 50  0001 C CNN
	1    4850 3550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5FC6A958
P 3250 2600
F 0 "SW1" H 3250 2885 50  0000 C CNN
F 1 "SW_Push" H 3250 2794 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3250 2800 50  0001 C CNN
F 3 "~" H 3250 2800 50  0001 C CNN
	1    3250 2600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5FCBDA4D
P 3250 3300
F 0 "SW2" H 3250 3585 50  0000 C CNN
F 1 "SW_Push" H 3250 3494 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3250 3500 50  0001 C CNN
F 3 "~" H 3250 3500 50  0001 C CNN
	1    3250 3300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5FCBEC5D
P 3250 3950
F 0 "SW3" H 3250 4235 50  0000 C CNN
F 1 "SW_Push" H 3250 4144 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3250 4150 50  0001 C CNN
F 3 "~" H 3250 4150 50  0001 C CNN
	1    3250 3950
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5FCBF386
P 3250 4550
F 0 "SW4" H 3250 4835 50  0000 C CNN
F 1 "SW_Push" H 3250 4744 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3250 4750 50  0001 C CNN
F 3 "~" H 3250 4750 50  0001 C CNN
	1    3250 4550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW5
U 1 1 5FCBFB32
P 3250 5100
F 0 "SW5" H 3250 5385 50  0000 C CNN
F 1 "SW_Push" H 3250 5294 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 3250 5300 50  0001 C CNN
F 3 "~" H 3250 5300 50  0001 C CNN
	1    3250 5100
	1    0    0    -1  
$EndComp
$Comp
L Device:D D2
U 1 1 5FCC08D9
P 2450 3300
F 0 "D2" H 2450 3516 50  0000 C CNN
F 1 "D" H 2450 3425 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2450 3300 50  0001 C CNN
F 3 "~" H 2450 3300 50  0001 C CNN
	1    2450 3300
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D3
U 1 1 5FCC1171
P 2450 3950
F 0 "D3" H 2450 4166 50  0000 C CNN
F 1 "D" H 2450 4075 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2450 3950 50  0001 C CNN
F 3 "~" H 2450 3950 50  0001 C CNN
	1    2450 3950
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D4
U 1 1 5FCC18D9
P 2450 4550
F 0 "D4" H 2450 4766 50  0000 C CNN
F 1 "D" H 2450 4675 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2450 4550 50  0001 C CNN
F 3 "~" H 2450 4550 50  0001 C CNN
	1    2450 4550
	-1   0    0    -1  
$EndComp
$Comp
L Device:D D5
U 1 1 5FCC1FC0
P 2450 5100
F 0 "D5" H 2450 5316 50  0000 C CNN
F 1 "D" H 2450 5225 50  0000 C CNN
F 2 "PT_Library_v001:D_Signal_P7.62mm_Horizontal" H 2450 5100 50  0001 C CNN
F 3 "~" H 2450 5100 50  0001 C CNN
	1    2450 5100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3050 2600 2600 2600
Wire Wire Line
	3050 3300 2600 3300
Wire Wire Line
	3050 3950 2600 3950
Wire Wire Line
	3050 4550 2600 4550
Wire Wire Line
	3050 5100 2600 5100
$Comp
L Connector:Conn_01x03_Female J1
U 1 1 5FCC4B59
P 1250 3450
F 0 "J1" H 1142 3735 50  0000 C CNN
F 1 "Input Columns" H 1142 3644 50  0000 C CNN
F 2 "" H 1250 3450 50  0001 C CNN
F 3 "~" H 1250 3450 50  0001 C CNN
	1    1250 3450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2300 2600 1900 2600
Wire Wire Line
	1900 2600 1900 3300
Wire Wire Line
	1900 3350 1450 3350
Wire Wire Line
	2300 3300 1900 3300
Connection ~ 1900 3300
Wire Wire Line
	1900 3300 1900 3350
Wire Wire Line
	1450 3450 2100 3450
Wire Wire Line
	2100 3450 2100 3950
Wire Wire Line
	2100 4550 2300 4550
Wire Wire Line
	2300 3950 2100 3950
Connection ~ 2100 3950
Wire Wire Line
	2100 3950 2100 4550
Wire Wire Line
	1450 3550 1900 3550
Wire Wire Line
	1900 3550 1900 5100
Wire Wire Line
	1900 5100 2300 5100
Wire Wire Line
	3450 3950 4000 3950
Wire Wire Line
	4000 3950 4000 3550
Wire Wire Line
	4000 3550 4650 3550
Wire Wire Line
	3500 2600 4000 2600
Wire Wire Line
	4000 2600 4000 3550
Connection ~ 4000 3550
Wire Wire Line
	3450 5100 4000 5100
Wire Wire Line
	4000 5100 4000 3950
Connection ~ 4000 3950
Wire Wire Line
	3450 3300 3700 3300
Wire Wire Line
	3700 4550 3450 4550
Wire Wire Line
	3700 3300 3700 3650
Wire Wire Line
	4650 3650 3700 3650
Connection ~ 3700 3650
Wire Wire Line
	3700 3650 3700 4550
$EndSCHEMATC