EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
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
L OH---UIP---Led-Distribution-rescue:MAX7219ENG-MAX7219ENG-Max7219-rescue U2
U 1 1 5D2333EB
P 3600 3150
F 0 "U2" H 3600 4920 50  0000 C CNN
F 1 "MAX7219-A" H 3600 4829 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 3600 3150 50  0001 L BNN
F 3 "" H 3600 3150 50  0001 L BNN
F 4 "MAX7219ENG+" H 3600 3150 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 3600 3150 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 3600 3150 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 3600 3150 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 3600 3150 50  0001 L BNN "Field8"
	1    3600 3150
	-1   0    0    -1  
$EndComp
Text Label 4300 1950 0    50   ~ 0
DIN
Text Label 2900 1850 2    50   ~ 0
A-DOUT-B-DIN
Text Label 4300 1850 0    50   ~ 0
CLK
Text Label 4300 2050 0    50   ~ 0
LOAD
Text Notes 3250 1000 0    50   ~ 0
Digits = Columns = Cathodes\n\nSegments = Rows = Anodes
Text Label 4300 2250 0    50   ~ 0
ACol0
Text Label 4300 2350 0    50   ~ 0
ACol1
Text Label 4300 2450 0    50   ~ 0
ACol2
Text Label 4300 2550 0    50   ~ 0
ACol3
Text Label 2900 2250 2    50   ~ 0
ACol4
Text Label 2900 2350 2    50   ~ 0
ACol5
Text Label 2900 2450 2    50   ~ 0
ACol6
Text Label 2900 2550 2    50   ~ 0
ACol7
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5D30502B
P 1950 1200
F 0 "J2" H 2030 1192 50  0000 L CNN
F 1 " " H 2030 1101 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 1950 1200 50  0001 C CNN
F 3 "~" H 1950 1200 50  0001 C CNN
	1    1950 1200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5D3057CB
P 1950 2150
F 0 "J3" H 2030 2142 50  0000 L CNN
F 1 " " H 2030 2051 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 1950 2150 50  0001 C CNN
F 3 "~" H 1950 2150 50  0001 C CNN
	1    1950 2150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR09
U 1 1 5D306791
P 1750 1000
F 0 "#PWR09" H 1750 850 50  0001 C CNN
F 1 "+5V" H 1765 1173 50  0000 C CNN
F 2 "" H 1750 1000 50  0001 C CNN
F 3 "" H 1750 1000 50  0001 C CNN
	1    1750 1000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR010
U 1 1 5D306B64
P 1750 1950
F 0 "#PWR010" H 1750 1800 50  0001 C CNN
F 1 "+5V" H 1765 2123 50  0000 C CNN
F 2 "" H 1750 1950 50  0001 C CNN
F 3 "" H 1750 1950 50  0001 C CNN
	1    1750 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5D306D11
P 1050 1100
F 0 "#PWR03" H 1050 850 50  0001 C CNN
F 1 "GND" H 1055 927 50  0000 C CNN
F 2 "" H 1050 1100 50  0001 C CNN
F 3 "" H 1050 1100 50  0001 C CNN
	1    1050 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5D306E49
P 1050 2050
F 0 "#PWR04" H 1050 1800 50  0001 C CNN
F 1 "GND" H 1055 1877 50  0000 C CNN
F 2 "" H 1050 2050 50  0001 C CNN
F 3 "" H 1050 2050 50  0001 C CNN
	1    1050 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 1100 1050 1100
Wire Wire Line
	1050 2050 1750 2050
Text Label 1750 1200 2    50   ~ 0
DIN
Text Label 1750 2150 2    50   ~ 0
DOUT
Text Label 1750 1400 2    50   ~ 0
CLK
Text Label 1750 2350 2    50   ~ 0
CLK
Text Label 1750 1300 2    50   ~ 0
LOAD
Text Label 1750 2250 2    50   ~ 0
LOAD
NoConn ~ 1750 1500
NoConn ~ 1750 2450
Text Label 4300 3750 0    50   ~ 0
ARow0
Text Label 4300 2950 0    50   ~ 0
ARow1
Text Label 4300 3150 0    50   ~ 0
ARow2
Text Label 4300 3350 0    50   ~ 0
ARow3
Text Label 4300 3550 0    50   ~ 0
ARow4
Text Label 4300 3950 0    50   ~ 0
ARow5
Text Label 4300 4150 0    50   ~ 0
ARow6
Text Label 4300 4350 0    50   ~ 0
ARow7
$Comp
L power:GND #PWR011
U 1 1 5D309D4D
P 2550 4500
F 0 "#PWR011" H 2550 4250 50  0001 C CNN
F 1 "GND" H 2555 4327 50  0000 C CNN
F 2 "" H 2550 4500 50  0001 C CNN
F 3 "" H 2550 4500 50  0001 C CNN
	1    2550 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 4450 2550 4450
$Comp
L power:+5V #PWR012
U 1 1 5D30A028
P 2650 1650
F 0 "#PWR012" H 2650 1500 50  0001 C CNN
F 1 "+5V" H 2665 1823 50  0000 C CNN
F 2 "" H 2650 1650 50  0001 C CNN
F 3 "" H 2650 1650 50  0001 C CNN
	1    2650 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 1650 2900 1650
$Comp
L Device:C C1
U 1 1 5D30A52C
P 1750 3450
F 0 "C1" H 1865 3496 50  0000 L CNN
F 1 "0.1uF" H 1865 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm" H 1788 3300 50  0001 C CNN
F 3 "~" H 1750 3450 50  0001 C CNN
	1    1750 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C2
U 1 1 5D30A807
P 2150 3450
F 0 "C2" H 2268 3496 50  0000 L CNN
F 1 "100uF" H 2268 3405 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D8.0mm_P5.00mm" H 2188 3300 50  0001 C CNN
F 3 "~" H 2150 3450 50  0001 C CNN
	1    2150 3450
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 5D30B9F5
P 1550 3150
F 0 "#PWR05" H 1550 3000 50  0001 C CNN
F 1 "+5V" H 1565 3323 50  0000 C CNN
F 2 "" H 1550 3150 50  0001 C CNN
F 3 "" H 1550 3150 50  0001 C CNN
	1    1550 3150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5D30BEF7
P 1550 3700
F 0 "#PWR06" H 1550 3450 50  0001 C CNN
F 1 "GND" H 1555 3527 50  0000 C CNN
F 2 "" H 1550 3700 50  0001 C CNN
F 3 "" H 1550 3700 50  0001 C CNN
	1    1550 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 3300 1750 3300
Wire Wire Line
	1550 3150 1550 3300
Wire Wire Line
	1550 3300 1750 3300
Connection ~ 1750 3300
Wire Wire Line
	2150 3600 1750 3600
Wire Wire Line
	1750 3600 1550 3600
Wire Wire Line
	1550 3600 1550 3700
Connection ~ 1750 3600
$Comp
L Connector:Conn_01x08_Female J5
U 1 1 5D30CF47
P 3400 6350
F 0 "J5" H 3428 6326 50  0000 L CNN
F 1 " " H 3428 6235 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 3400 6350 50  0001 C CNN
F 3 "~" H 3400 6350 50  0001 C CNN
	1    3400 6350
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J4
U 1 1 5D30D8BE
P 3350 7450
F 0 "J4" H 3378 7426 50  0000 L CNN
F 1 " " H 3378 7335 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 3350 7450 50  0001 C CNN
F 3 "~" H 3350 7450 50  0001 C CNN
	1    3350 7450
	-1   0    0    -1  
$EndComp
Text Label 3600 6050 0    50   ~ 0
CCol0
Text Label 3600 6150 0    50   ~ 0
CCol1
Text Label 3600 6250 0    50   ~ 0
CCol2
Text Label 3600 6350 0    50   ~ 0
CCol3
Text Label 3600 6450 0    50   ~ 0
CCol4
Text Label 3600 6550 0    50   ~ 0
CCol5
Text Label 3600 6650 0    50   ~ 0
CCol6
Text Label 3600 6750 0    50   ~ 0
CCol7
Text Label 3550 7150 0    50   ~ 0
CRow0
Text Label 3550 7250 0    50   ~ 0
CRow1
Text Label 3550 7350 0    50   ~ 0
CRow2
Text Label 3550 7450 0    50   ~ 0
CRow3
Text Label 3550 7550 0    50   ~ 0
CRow4
Text Label 3550 7650 0    50   ~ 0
CRow5
Text Label 3550 7750 0    50   ~ 0
CRow6
Text Label 3550 7850 0    50   ~ 0
CRow7
$Comp
L Device:R_Small R2
U 1 1 5D32B4BD
P 4600 2750
F 0 "R2" V 4404 2750 50  0000 C CNN
F 1 "10K" V 4495 2750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 4600 2750 50  0001 C CNN
F 3 "~" H 4600 2750 50  0001 C CNN
	1    4600 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	4300 2750 4500 2750
$Comp
L power:+5V #PWR016
U 1 1 5D32C649
P 4850 2550
F 0 "#PWR016" H 4850 2400 50  0001 C CNN
F 1 "+5V" H 4865 2723 50  0000 C CNN
F 2 "" H 4850 2550 50  0001 C CNN
F 3 "" H 4850 2550 50  0001 C CNN
	1    4850 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 2750 4850 2750
Wire Wire Line
	4850 2750 4850 2550
$Comp
L Connector:Conn_01x08_Female J35
U 1 1 5D2A5E5D
P 14900 1300
F 0 "J35" H 14928 1276 50  0000 L CNN
F 1 " " H 14928 1185 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 14900 1300 50  0001 C CNN
F 3 "~" H 14900 1300 50  0001 C CNN
	1    14900 1300
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J36
U 1 1 5D2A709D
P 15400 1300
F 0 "J36" H 15428 1276 50  0000 L CNN
F 1 " " H 15428 1185 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 15400 1300 50  0001 C CNN
F 3 "~" H 15400 1300 50  0001 C CNN
	1    15400 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	15100 1000 15200 1000
Wire Wire Line
	15100 1100 15200 1100
Wire Wire Line
	15100 1200 15200 1200
Wire Wire Line
	15100 1300 15200 1300
Wire Wire Line
	15100 1400 15200 1400
Wire Wire Line
	15100 1500 15200 1500
Wire Wire Line
	15100 1600 15200 1600
Wire Wire Line
	15100 1700 15200 1700
$Comp
L Connector:Conn_01x08_Female J30
U 1 1 5D2B0943
P 13000 1250
F 0 "J30" H 13028 1226 50  0000 L CNN
F 1 " " H 13028 1135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 13000 1250 50  0001 C CNN
F 3 "~" H 13000 1250 50  0001 C CNN
	1    13000 1250
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J31
U 1 1 5D2B094D
P 13500 1250
F 0 "J31" H 13528 1226 50  0000 L CNN
F 1 " " H 13528 1135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 13500 1250 50  0001 C CNN
F 3 "~" H 13500 1250 50  0001 C CNN
	1    13500 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	13200 950  13300 950 
Wire Wire Line
	13200 1050 13300 1050
Wire Wire Line
	13200 1150 13300 1150
Wire Wire Line
	13200 1250 13300 1250
Wire Wire Line
	13200 1350 13300 1350
Wire Wire Line
	13200 1450 13300 1450
Wire Wire Line
	13200 1550 13300 1550
Wire Wire Line
	13200 1650 13300 1650
Wire Notes Line
	12750 800  12750 1900
Wire Notes Line
	12750 1900 15800 1900
Wire Notes Line
	15800 1900 15800 800 
Wire Notes Line
	15800 800  12750 800 
Text Notes 14050 1250 0    50   ~ 0
SCRATCH PAD
$Comp
L OH---UIP---Led-Distribution-rescue:MAX7219ENG-MAX7219ENG-Max7219-rescue U5
U 1 1 61653072
P 9950 3200
F 0 "U5" H 9950 4970 50  0000 C CNN
F 1 "MAX7219-B" H 9950 4879 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 9950 3200 50  0001 L BNN
F 3 "" H 9950 3200 50  0001 L BNN
F 4 "MAX7219ENG+" H 9950 3200 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 9950 3200 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 9950 3200 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 9950 3200 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 9950 3200 50  0001 L BNN "Field8"
	1    9950 3200
	-1   0    0    -1  
$EndComp
Text Label 10650 2000 0    50   ~ 0
A-DOUT-B-DIN
Text Label 10650 1900 0    50   ~ 0
CLK
Text Label 10650 2100 0    50   ~ 0
LOAD
Text Label 10650 2300 0    50   ~ 0
BCol0
Text Label 10650 2400 0    50   ~ 0
BCol1
Text Label 10650 2500 0    50   ~ 0
BCol2
Text Label 10650 2600 0    50   ~ 0
BCol3
Text Label 9250 2300 2    50   ~ 0
BCol4
Text Label 9250 2400 2    50   ~ 0
BCol5
Text Label 9250 2500 2    50   ~ 0
BCol6
Text Label 9250 2600 2    50   ~ 0
BCol7
Text Label 10650 3800 0    50   ~ 0
BRow0
Text Label 10650 3000 0    50   ~ 0
BRow1
Text Label 10650 3200 0    50   ~ 0
BRow2
Text Label 10650 3400 0    50   ~ 0
BRow3
Text Label 10650 3600 0    50   ~ 0
BRow4
Text Label 10650 4000 0    50   ~ 0
BRow5
Text Label 10650 4200 0    50   ~ 0
BRow6
Text Label 10650 4400 0    50   ~ 0
BRow7
$Comp
L power:GND #PWR020
U 1 1 61653090
P 8900 4500
F 0 "#PWR020" H 8900 4250 50  0001 C CNN
F 1 "GND" H 8905 4327 50  0000 C CNN
F 2 "" H 8900 4500 50  0001 C CNN
F 3 "" H 8900 4500 50  0001 C CNN
	1    8900 4500
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR021
U 1 1 6165309B
P 9000 1700
F 0 "#PWR021" H 9000 1550 50  0001 C CNN
F 1 "+5V" H 9015 1873 50  0000 C CNN
F 2 "" H 9000 1700 50  0001 C CNN
F 3 "" H 9000 1700 50  0001 C CNN
	1    9000 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 1700 9250 1700
$Comp
L Device:R_Small R5
U 1 1 616530A6
P 10950 2800
F 0 "R5" V 10754 2800 50  0000 C CNN
F 1 "10K" V 10845 2800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 10950 2800 50  0001 C CNN
F 3 "~" H 10950 2800 50  0001 C CNN
	1    10950 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	10650 2800 10850 2800
$Comp
L power:+5V #PWR025
U 1 1 616530B1
P 11200 2600
F 0 "#PWR025" H 11200 2450 50  0001 C CNN
F 1 "+5V" H 11215 2773 50  0000 C CNN
F 2 "" H 11200 2600 50  0001 C CNN
F 3 "" H 11200 2600 50  0001 C CNN
	1    11200 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	11050 2800 11200 2800
Wire Wire Line
	11200 2800 11200 2600
$Comp
L OH---UIP---Led-Distribution-rescue:MAX7219ENG-MAX7219ENG-Max7219-rescue U3
U 1 1 616563B5
P 5350 7500
F 0 "U3" H 5350 9270 50  0000 C CNN
F 1 "MAX7219-D" H 5350 9179 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 5350 7500 50  0001 L BNN
F 3 "" H 5350 7500 50  0001 L BNN
F 4 "MAX7219ENG+" H 5350 7500 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 5350 7500 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 5350 7500 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 5350 7500 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 5350 7500 50  0001 L BNN "Field8"
	1    5350 7500
	-1   0    0    -1  
$EndComp
Text Label 6050 6300 0    50   ~ 0
C-DOUT-D-DIN
Text Label 4650 6200 2    50   ~ 0
D-DOUT-E-DIN
Text Label 6050 6200 0    50   ~ 0
CLK
Text Label 6050 6400 0    50   ~ 0
LOAD
Text Label 6050 6600 0    50   ~ 0
DCol0
Text Label 6050 6700 0    50   ~ 0
DCol1
Text Label 6050 6800 0    50   ~ 0
DCol2
Text Label 6050 6900 0    50   ~ 0
DCol3
Text Label 4650 6600 2    50   ~ 0
DCol4
Text Label 4650 6700 2    50   ~ 0
DCol5
Text Label 4650 6800 2    50   ~ 0
DCol6
Text Label 4650 6900 2    50   ~ 0
DCol7
Text Label 6050 8100 0    50   ~ 0
DRow0
Text Label 6050 7300 0    50   ~ 0
DRow1
Text Label 6050 7500 0    50   ~ 0
DRow2
Text Label 6050 7700 0    50   ~ 0
DRow3
Text Label 6050 7900 0    50   ~ 0
DRow4
Text Label 6050 8300 0    50   ~ 0
DRow5
Text Label 6050 8500 0    50   ~ 0
DRow6
Text Label 6050 8700 0    50   ~ 0
DRow7
$Comp
L power:GND #PWR014
U 1 1 616563D3
P 4300 8800
F 0 "#PWR014" H 4300 8550 50  0001 C CNN
F 1 "GND" H 4305 8627 50  0000 C CNN
F 2 "" H 4300 8800 50  0001 C CNN
F 3 "" H 4300 8800 50  0001 C CNN
	1    4300 8800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 8800 4300 8800
$Comp
L power:+5V #PWR015
U 1 1 616563DE
P 4400 6000
F 0 "#PWR015" H 4400 5850 50  0001 C CNN
F 1 "+5V" H 4415 6173 50  0000 C CNN
F 2 "" H 4400 6000 50  0001 C CNN
F 3 "" H 4400 6000 50  0001 C CNN
	1    4400 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 6000 4650 6000
$Comp
L Device:R_Small R3
U 1 1 616563E9
P 6350 7100
F 0 "R3" V 6154 7100 50  0000 C CNN
F 1 "10K" V 6245 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 6350 7100 50  0001 C CNN
F 3 "~" H 6350 7100 50  0001 C CNN
	1    6350 7100
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 7100 6250 7100
$Comp
L power:+5V #PWR017
U 1 1 616563F4
P 6600 6900
F 0 "#PWR017" H 6600 6750 50  0001 C CNN
F 1 "+5V" H 6615 7073 50  0000 C CNN
F 2 "" H 6600 6900 50  0001 C CNN
F 3 "" H 6600 6900 50  0001 C CNN
	1    6600 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 7100 6600 7100
Wire Wire Line
	6600 7100 6600 6900
$Comp
L OH---UIP---Led-Distribution-rescue:MAX7219ENG-MAX7219ENG-Max7219-rescue U1
U 1 1 6165B7FE
P 1950 7550
F 0 "U1" H 1950 9320 50  0000 C CNN
F 1 "MAX7219-C" H 1950 9229 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 1950 7550 50  0001 L BNN
F 3 "" H 1950 7550 50  0001 L BNN
F 4 "MAX7219ENG+" H 1950 7550 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 1950 7550 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 1950 7550 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 1950 7550 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 1950 7550 50  0001 L BNN "Field8"
	1    1950 7550
	-1   0    0    -1  
$EndComp
Text Label 2650 6350 0    50   ~ 0
B-DOUT-C-DIN
Text Label 1250 6250 2    50   ~ 0
C-DOUT-D-DIN
Text Label 2650 6250 0    50   ~ 0
CLK
Text Label 2650 6450 0    50   ~ 0
LOAD
Text Label 2650 6650 0    50   ~ 0
CCol0
Text Label 2650 6750 0    50   ~ 0
CCol1
Text Label 2650 6850 0    50   ~ 0
CCol2
Text Label 2650 6950 0    50   ~ 0
CCol3
Text Label 1250 6650 2    50   ~ 0
CCol4
Text Label 1250 6750 2    50   ~ 0
CCol5
Text Label 1250 6850 2    50   ~ 0
CCol6
Text Label 1250 6950 2    50   ~ 0
CCol7
Text Label 2650 8150 0    50   ~ 0
CRow0
Text Label 2650 7350 0    50   ~ 0
CRow1
Text Label 2650 7550 0    50   ~ 0
CRow2
Text Label 2650 7750 0    50   ~ 0
CRow3
Text Label 2650 7950 0    50   ~ 0
CRow4
Text Label 2650 8350 0    50   ~ 0
CRow5
Text Label 2650 8550 0    50   ~ 0
CRow6
Text Label 2650 8750 0    50   ~ 0
CRow7
$Comp
L power:GND #PWR01
U 1 1 6165B81C
P 900 8850
F 0 "#PWR01" H 900 8600 50  0001 C CNN
F 1 "GND" H 905 8677 50  0000 C CNN
F 2 "" H 900 8850 50  0001 C CNN
F 3 "" H 900 8850 50  0001 C CNN
	1    900  8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 8850 900  8850
$Comp
L power:+5V #PWR02
U 1 1 6165B827
P 1000 6050
F 0 "#PWR02" H 1000 5900 50  0001 C CNN
F 1 "+5V" H 1015 6223 50  0000 C CNN
F 2 "" H 1000 6050 50  0001 C CNN
F 3 "" H 1000 6050 50  0001 C CNN
	1    1000 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 6050 1250 6050
$Comp
L Device:R_Small R1
U 1 1 6165B832
P 2950 7150
F 0 "R1" V 2754 7150 50  0000 C CNN
F 1 "10K" V 2845 7150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 2950 7150 50  0001 C CNN
F 3 "~" H 2950 7150 50  0001 C CNN
	1    2950 7150
	0    1    1    0   
$EndComp
Wire Wire Line
	2650 7150 2850 7150
$Comp
L power:+5V #PWR013
U 1 1 6165B83D
P 3200 6950
F 0 "#PWR013" H 3200 6800 50  0001 C CNN
F 1 "+5V" H 3215 7123 50  0000 C CNN
F 2 "" H 3200 6950 50  0001 C CNN
F 3 "" H 3200 6950 50  0001 C CNN
	1    3200 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 7150 3200 7150
Wire Wire Line
	3200 7150 3200 6950
$Comp
L Connector:Conn_01x08_Female J10
U 1 1 6162DC25
P 5150 2250
F 0 "J10" H 5178 2226 50  0000 L CNN
F 1 " " H 5178 2135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 5150 2250 50  0001 C CNN
F 3 "~" H 5150 2250 50  0001 C CNN
	1    5150 2250
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J9
U 1 1 6162DC2F
P 5050 3450
F 0 "J9" H 5078 3426 50  0000 L CNN
F 1 " " H 5078 3335 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 5050 3450 50  0001 C CNN
F 3 "~" H 5050 3450 50  0001 C CNN
	1    5050 3450
	-1   0    0    -1  
$EndComp
Text Label 5350 1950 0    50   ~ 0
ACol0
Text Label 5350 2050 0    50   ~ 0
ACol1
Text Label 5350 2150 0    50   ~ 0
ACol2
Text Label 5350 2250 0    50   ~ 0
ACol3
Text Label 5350 2350 0    50   ~ 0
ACol4
Text Label 5350 2450 0    50   ~ 0
ACol5
Text Label 5350 2550 0    50   ~ 0
ACol6
Text Label 5350 2650 0    50   ~ 0
ACol7
Text Label 5250 3150 0    50   ~ 0
ARow0
Text Label 5250 3250 0    50   ~ 0
ARow1
Text Label 5250 3350 0    50   ~ 0
ARow2
Text Label 5250 3450 0    50   ~ 0
ARow3
Text Label 5250 3550 0    50   ~ 0
ARow4
Text Label 5250 3650 0    50   ~ 0
ARow5
Text Label 5250 3750 0    50   ~ 0
ARow6
Text Label 5250 3850 0    50   ~ 0
ARow7
$Comp
L Connector:Conn_01x08_Female J16
U 1 1 616271A3
P 6850 6300
F 0 "J16" H 6878 6276 50  0000 L CNN
F 1 " " H 6878 6185 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 6850 6300 50  0001 C CNN
F 3 "~" H 6850 6300 50  0001 C CNN
	1    6850 6300
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J17
U 1 1 616271AD
P 6850 7150
F 0 "J17" H 6878 7126 50  0000 L CNN
F 1 " " H 6878 7035 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 6850 7150 50  0001 C CNN
F 3 "~" H 6850 7150 50  0001 C CNN
	1    6850 7150
	-1   0    0    -1  
$EndComp
Text Label 7050 6000 0    50   ~ 0
DCol0
Text Label 7050 6100 0    50   ~ 0
DCol1
Text Label 7050 6200 0    50   ~ 0
DCol2
Text Label 7050 6300 0    50   ~ 0
DCol3
Text Label 7050 6400 0    50   ~ 0
DCol4
Text Label 7050 6500 0    50   ~ 0
DCol5
Text Label 7050 6600 0    50   ~ 0
DCol6
Text Label 7050 6700 0    50   ~ 0
DCol7
Text Label 7050 6850 0    50   ~ 0
DRow0
Text Label 7050 6950 0    50   ~ 0
DRow1
Text Label 7050 7050 0    50   ~ 0
DRow2
Text Label 7050 7150 0    50   ~ 0
DRow3
Text Label 7050 7250 0    50   ~ 0
DRow4
Text Label 7050 7350 0    50   ~ 0
DRow5
Text Label 7050 7450 0    50   ~ 0
DRow6
Text Label 7050 7550 0    50   ~ 0
DRow7
$Comp
L Connector:Conn_01x08_Female J27
U 1 1 6162F9D6
P 11500 2250
F 0 "J27" H 11528 2226 50  0000 L CNN
F 1 " " H 11528 2135 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 11500 2250 50  0001 C CNN
F 3 "~" H 11500 2250 50  0001 C CNN
	1    11500 2250
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J26
U 1 1 6162F9E0
P 11400 3450
F 0 "J26" H 11428 3426 50  0000 L CNN
F 1 " " H 11428 3335 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 11400 3450 50  0001 C CNN
F 3 "~" H 11400 3450 50  0001 C CNN
	1    11400 3450
	-1   0    0    -1  
$EndComp
Text Label 11700 1950 0    50   ~ 0
BCol0
Text Label 11700 2050 0    50   ~ 0
BCol1
Text Label 11700 2150 0    50   ~ 0
BCol2
Text Label 11700 2250 0    50   ~ 0
BCol3
Text Label 11700 2350 0    50   ~ 0
BCol4
Text Label 11700 2450 0    50   ~ 0
BCol5
Text Label 11700 2550 0    50   ~ 0
BCol6
Text Label 11700 2650 0    50   ~ 0
BCol7
Text Label 11600 3150 0    50   ~ 0
BRow0
Text Label 11600 3250 0    50   ~ 0
BRow1
Text Label 11600 3350 0    50   ~ 0
BRow2
Text Label 11600 3450 0    50   ~ 0
BRow3
Text Label 11600 3550 0    50   ~ 0
BRow4
Text Label 11600 3650 0    50   ~ 0
BRow5
Text Label 11600 3750 0    50   ~ 0
BRow6
Text Label 11600 3850 0    50   ~ 0
BRow7
Wire Wire Line
	2550 4450 2550 4500
$Comp
L Connector:Conn_01x06_Female J12
U 1 1 6166C126
P 5750 2200
F 0 "J12" H 5642 2585 50  0000 C CNN
F 1 "LEFT EWI COL" H 5642 2494 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 5750 2200 50  0001 C CNN
F 3 "~" H 5750 2200 50  0001 C CNN
	1    5750 2200
	-1   0    0    -1  
$EndComp
Text Label 5950 2000 0    50   ~ 0
ACol0
Text Label 5950 2100 0    50   ~ 0
ACol1
Text Label 5950 2200 0    50   ~ 0
ACol2
Text Label 5950 2300 0    50   ~ 0
ACol3
Text Label 5950 2400 0    50   ~ 0
ACol4
Text Label 5950 2500 0    50   ~ 0
ACol5
$Comp
L Connector:Conn_01x08_Female J11
U 1 1 6167124F
P 5700 3450
F 0 "J11" H 5592 3935 50  0000 C CNN
F 1 "LEFT EWI ROW" H 5592 3844 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 5700 3450 50  0001 C CNN
F 3 "~" H 5700 3450 50  0001 C CNN
	1    5700 3450
	-1   0    0    -1  
$EndComp
Text Label 5900 3150 0    50   ~ 0
ARow0
Text Label 5900 3250 0    50   ~ 0
ARow1
Text Label 5900 3350 0    50   ~ 0
ARow2
Text Label 5900 3450 0    50   ~ 0
ARow3
Text Label 5900 3550 0    50   ~ 0
ARow4
Text Label 5900 3650 0    50   ~ 0
ARow5
Text Label 5900 3750 0    50   ~ 0
ARow6
Text Label 5900 3850 0    50   ~ 0
ARow7
$Comp
L Connector:Conn_01x03_Female J13
U 1 1 6167CDB7
P 6350 2100
F 0 "J13" H 6242 2385 50  0000 C CNN
F 1 "UFC OPT COL" H 6242 2294 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 6350 2100 50  0001 C CNN
F 3 "~" H 6350 2100 50  0001 C CNN
	1    6350 2100
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J14
U 1 1 6167EA31
P 6350 2550
F 0 "J14" H 6242 2835 50  0000 C CNN
F 1 "UFC OPT ROW" H 6242 2744 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 6350 2550 50  0001 C CNN
F 3 "~" H 6350 2550 50  0001 C CNN
	1    6350 2550
	-1   0    0    -1  
$EndComp
Text Label 6550 2000 0    50   ~ 0
ACol3
Text Label 6550 2100 0    50   ~ 0
ACol4
Text Label 6550 2200 0    50   ~ 0
ACol5
Text Label 6550 2450 0    50   ~ 0
ARow4
Text Label 6550 2550 0    50   ~ 0
ARow5
Text Label 6550 2650 0    50   ~ 0
ARow6
Text Label 6550 2750 0    50   ~ 0
ARow7
Text Notes 4600 1350 0    79   ~ 0
LEFT EWI\nUFC\nBIT\nLEFT DIST
Text Notes 10650 1450 0    79   ~ 0
RIGHT EWI
$Comp
L Connector:Conn_01x06_Female J29
U 1 1 61690B49
P 12100 2150
F 0 "J29" H 11992 2535 50  0000 C CNN
F 1 "RIGHT EWI COL" H 11992 2444 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 12100 2150 50  0001 C CNN
F 3 "~" H 12100 2150 50  0001 C CNN
	1    12100 2150
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J28
U 1 1 61690B59
P 12050 3400
F 0 "J28" H 11942 3885 50  0000 C CNN
F 1 "RIGHT EWI ROW" H 11942 3794 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 12050 3400 50  0001 C CNN
F 3 "~" H 12050 3400 50  0001 C CNN
	1    12050 3400
	-1   0    0    -1  
$EndComp
Text Label 12250 3100 0    50   ~ 0
BRow0
Text Label 12250 3200 0    50   ~ 0
BRow1
Text Label 12250 3300 0    50   ~ 0
BRow2
Text Label 12250 3400 0    50   ~ 0
BRow3
Text Label 12250 3500 0    50   ~ 0
BRow4
Text Label 12250 3600 0    50   ~ 0
BRow5
Text Label 12250 3700 0    50   ~ 0
BRow6
Text Label 12250 3800 0    50   ~ 0
BRow7
Text Label 12300 1950 0    50   ~ 0
BCol0
Text Label 12300 2050 0    50   ~ 0
BCol1
Text Label 12300 2150 0    50   ~ 0
BCol2
Text Label 12300 2250 0    50   ~ 0
BCol3
Text Label 12300 2350 0    50   ~ 0
BCol4
Text Label 12300 2450 0    50   ~ 0
BCol5
$Comp
L Connector:Conn_01x03_Female J15
U 1 1 616C6E1E
P 6850 2050
F 0 "J15" H 6742 2335 50  0000 C CNN
F 1 "BIT LED" H 6742 2244 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 6850 2050 50  0001 C CNN
F 3 "~" H 6850 2050 50  0001 C CNN
	1    6850 2050
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J21
U 1 1 616C0C9C
P 6900 10000
F 0 "J21" H 6792 10385 50  0000 C CNN
F 1 "Caution Rows" H 6792 10294 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 6900 10000 50  0001 C CNN
F 3 "~" H 6900 10000 50  0001 C CNN
	1    6900 10000
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J20
U 1 1 616C8E36
P 6900 9350
F 0 "J20" H 6792 9635 50  0000 C CNN
F 1 "Caution Columns" H 6792 9544 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 6900 9350 50  0001 C CNN
F 3 "~" H 6900 9350 50  0001 C CNN
	1    6900 9350
	-1   0    0    -1  
$EndComp
Text Label 7100 9250 0    50   ~ 0
DCol0
Text Label 7100 9350 0    50   ~ 0
DCol1
Text Label 7100 9450 0    50   ~ 0
DCol2
Text Label 7100 9550 0    50   ~ 0
DCol3
Text Label 7100 9800 0    50   ~ 0
DRow0
Text Label 7100 9900 0    50   ~ 0
DRow1
Text Label 7100 10000 0    50   ~ 0
DRow2
Text Label 7100 10100 0    50   ~ 0
DRow3
Text Label 7100 10200 0    50   ~ 0
DRow4
$Comp
L Connector:Conn_01x04_Female J18
U 1 1 616D1CC0
P 6850 7950
F 0 "J18" H 6742 8235 50  0000 C CNN
F 1 "Right Distribution" H 6742 8144 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 6850 7950 50  0001 C CNN
F 3 "~" H 6850 7950 50  0001 C CNN
	1    6850 7950
	-1   0    0    -1  
$EndComp
Text Label 7050 7850 0    50   ~ 0
DRow0
Text Label 7050 7950 0    50   ~ 0
DRow1
Text Label 7050 8050 0    50   ~ 0
DCol5
Text Label 7050 8150 0    50   ~ 0
DCol6
Text Notes 4400 650  0    50   ~ 0
Unlike inputs - connectors with Rows and Coumns have Rows first as the rows as the annodes
Text Label 7650 2150 0    50   ~ 0
ACol5
NoConn ~ 9200 2900
Text Label 7650 1950 0    50   ~ 0
ARow2
Text Label 7650 2050 0    50   ~ 0
ARow3
Text Label 7050 2050 0    50   ~ 0
ACol5
Text Label 7050 2150 0    50   ~ 0
ACol6
Text Label 7050 1950 0    50   ~ 0
ARow5
$Comp
L Connector:Conn_01x06_Female J6
U 1 1 61727743
P 3400 8400
F 0 "J6" H 3292 8785 50  0000 C CNN
F 1 "FLAPS GEAR COLUMNS" H 3292 8694 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 3400 8400 50  0001 C CNN
F 3 "~" H 3400 8400 50  0001 C CNN
	1    3400 8400
	-1   0    0    -1  
$EndComp
Text Label 3600 8200 0    50   ~ 0
CCol0
Text Label 3600 8300 0    50   ~ 0
CCol1
Text Label 3600 8400 0    50   ~ 0
CCol2
Text Label 3600 8500 0    50   ~ 0
CCol3
Text Label 3600 8600 0    50   ~ 0
CCol4
Text Label 3600 8700 0    50   ~ 0
CCol5
$Comp
L Connector:Conn_01x03_Female J7
U 1 1 6172D93E
P 3400 9100
F 0 "J7" H 3292 9385 50  0000 C CNN
F 1 "FLAPS GEAR ROWS" H 3292 9294 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 3400 9100 50  0001 C CNN
F 3 "~" H 3400 9100 50  0001 C CNN
	1    3400 9100
	-1   0    0    -1  
$EndComp
Text Label 3600 9000 0    50   ~ 0
CRow0
Text Label 3600 9100 0    50   ~ 0
CRow1
Text Label 3600 9200 0    50   ~ 0
CRow2
$Comp
L Connector:Conn_01x06_Female J19
U 1 1 61737271
P 6900 8650
F 0 "J19" H 6792 9035 50  0000 C CNN
F 1 "LOCK SHOOT" H 6792 8944 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 6900 8650 50  0001 C CNN
F 3 "~" H 6900 8650 50  0001 C CNN
	1    6900 8650
	-1   0    0    -1  
$EndComp
Text Label 7100 8450 0    50   ~ 0
DRow2
Text Label 7100 8550 0    50   ~ 0
DRow3
Text Label 7100 8650 0    50   ~ 0
DRow4
Text Label 7100 8750 0    50   ~ 0
DCol5
Text Label 7100 8850 0    50   ~ 0
DCol6
NoConn ~ 7100 8950
$Comp
L Connector:Conn_01x06_Female J8
U 1 1 617402DC
P 3400 9650
F 0 "J8" H 3292 10035 50  0000 C CNN
F 1 "MASTER ARM" H 3292 9944 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 3400 9650 50  0001 C CNN
F 3 "~" H 3400 9650 50  0001 C CNN
	1    3400 9650
	-1   0    0    -1  
$EndComp
Text Label 3600 9450 0    50   ~ 0
CRow5
Text Label 3600 9550 0    50   ~ 0
CRow6
Text Label 3600 9650 0    50   ~ 0
CRow7
Text Label 3600 9750 0    50   ~ 0
CCol0
Text Label 3600 9850 0    50   ~ 0
CCol1
Text Label 3600 9950 0    50   ~ 0
CCol2
Text Label 9250 1900 2    50   ~ 0
B-DOUT-C-DIN
Wire Wire Line
	9250 4500 8900 4500
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 61759ED0
P 1850 4950
F 0 "J1" H 1878 4926 50  0000 L CNN
F 1 "Conn_01x02_Female" H 1878 4835 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 1850 4950 50  0001 C CNN
F 3 "~" H 1850 4950 50  0001 C CNN
	1    1850 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 6175B6A4
P 1650 5050
F 0 "#PWR08" H 1650 4800 50  0001 C CNN
F 1 "GND" H 1655 4877 50  0000 C CNN
F 2 "" H 1650 5050 50  0001 C CNN
F 3 "" H 1650 5050 50  0001 C CNN
	1    1650 5050
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR07
U 1 1 6175BF7F
P 1650 4950
F 0 "#PWR07" H 1650 4800 50  0001 C CNN
F 1 "+5V" H 1665 5123 50  0000 C CNN
F 2 "" H 1650 4950 50  0001 C CNN
F 3 "" H 1650 4950 50  0001 C CNN
	1    1650 4950
	1    0    0    -1  
$EndComp
$Comp
L OH---UIP---Led-Distribution-rescue:MAX7219ENG-MAX7219ENG-Max7219-rescue U4
U 1 1 6177AC31
P 8700 7550
F 0 "U4" H 8700 9320 50  0000 C CNN
F 1 "MAX7219-E" H 8700 9229 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 8700 7550 50  0001 L BNN
F 3 "" H 8700 7550 50  0001 L BNN
F 4 "MAX7219ENG+" H 8700 7550 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 8700 7550 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 8700 7550 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 8700 7550 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 8700 7550 50  0001 L BNN "Field8"
	1    8700 7550
	-1   0    0    -1  
$EndComp
Text Label 9400 6350 0    50   ~ 0
D-DOUT-E-DIN
Text Label 9400 6250 0    50   ~ 0
CLK
Text Label 9400 6450 0    50   ~ 0
LOAD
Text Label 9400 6650 0    50   ~ 0
ECol0
Text Label 9400 6750 0    50   ~ 0
ECol1
Text Label 9400 6850 0    50   ~ 0
ECol2
Text Label 9400 6950 0    50   ~ 0
ECol3
Text Label 8000 6650 2    50   ~ 0
ECol4
Text Label 8000 6750 2    50   ~ 0
ECol5
Text Label 8000 6850 2    50   ~ 0
ECol6
Text Label 8000 6950 2    50   ~ 0
ECol7
Text Label 9400 8150 0    50   ~ 0
ERow0
Text Label 9400 7350 0    50   ~ 0
ERow1
Text Label 9400 7550 0    50   ~ 0
ERow2
Text Label 9400 7750 0    50   ~ 0
ERow3
Text Label 9400 7950 0    50   ~ 0
ERow4
Text Label 9400 8350 0    50   ~ 0
ERow5
Text Label 9400 8550 0    50   ~ 0
ERow6
Text Label 9400 8750 0    50   ~ 0
ERow7
$Comp
L power:GND #PWR018
U 1 1 6177AC4F
P 7650 8850
F 0 "#PWR018" H 7650 8600 50  0001 C CNN
F 1 "GND" H 7655 8677 50  0000 C CNN
F 2 "" H 7650 8850 50  0001 C CNN
F 3 "" H 7650 8850 50  0001 C CNN
	1    7650 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 8850 7650 8850
$Comp
L power:+5V #PWR019
U 1 1 6177AC5A
P 7750 6050
F 0 "#PWR019" H 7750 5900 50  0001 C CNN
F 1 "+5V" H 7765 6223 50  0000 C CNN
F 2 "" H 7750 6050 50  0001 C CNN
F 3 "" H 7750 6050 50  0001 C CNN
	1    7750 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 6050 8000 6050
$Comp
L Device:R_Small R4
U 1 1 6177AC65
P 9700 7150
F 0 "R4" V 9504 7150 50  0000 C CNN
F 1 "10K" V 9595 7150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 9700 7150 50  0001 C CNN
F 3 "~" H 9700 7150 50  0001 C CNN
	1    9700 7150
	0    1    1    0   
$EndComp
Wire Wire Line
	9400 7150 9600 7150
$Comp
L power:+5V #PWR022
U 1 1 6177AC70
P 9950 6950
F 0 "#PWR022" H 9950 6800 50  0001 C CNN
F 1 "+5V" H 9965 7123 50  0000 C CNN
F 2 "" H 9950 6950 50  0001 C CNN
F 3 "" H 9950 6950 50  0001 C CNN
	1    9950 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 7150 9950 7150
Wire Wire Line
	9950 7150 9950 6950
$Comp
L Connector:Conn_01x08_Female J23
U 1 1 6177AC7C
P 10250 6650
F 0 "J23" H 10278 6626 50  0000 L CNN
F 1 " " H 10278 6535 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 10250 6650 50  0001 C CNN
F 3 "~" H 10250 6650 50  0001 C CNN
	1    10250 6650
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J24
U 1 1 6177AC86
P 10250 7800
F 0 "J24" H 10278 7776 50  0000 L CNN
F 1 " " H 10278 7685 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 10250 7800 50  0001 C CNN
F 3 "~" H 10250 7800 50  0001 C CNN
	1    10250 7800
	-1   0    0    -1  
$EndComp
Text Label 10450 6350 0    50   ~ 0
ECol0
Text Label 10450 6450 0    50   ~ 0
ECol1
Text Label 10450 6550 0    50   ~ 0
ECol2
Text Label 10450 6650 0    50   ~ 0
ECol3
Text Label 10450 6750 0    50   ~ 0
ECol4
Text Label 10450 6850 0    50   ~ 0
ECol5
Text Label 10450 6950 0    50   ~ 0
ECol6
Text Label 10450 7050 0    50   ~ 0
ECol7
Text Label 10450 7500 0    50   ~ 0
ERow0
Text Label 10450 7600 0    50   ~ 0
ERow1
Text Label 10450 7700 0    50   ~ 0
ERow2
Text Label 10450 7800 0    50   ~ 0
ERow3
Text Label 10450 7900 0    50   ~ 0
ERow4
Text Label 10450 8000 0    50   ~ 0
ERow5
Text Label 10450 8100 0    50   ~ 0
ERow6
Text Label 10450 8200 0    50   ~ 0
ERow7
$Comp
L Connector:Conn_01x04_Female J25
U 1 1 6179100D
P 10250 8700
F 0 "J25" H 10142 8985 50  0000 C CNN
F 1 "AOA - SEP DIM" H 10142 8894 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 10250 8700 50  0001 C CNN
F 3 "~" H 10250 8700 50  0001 C CNN
	1    10250 8700
	-1   0    0    -1  
$EndComp
Text Label 10450 8600 0    50   ~ 0
ERow0
Text Label 10450 8700 0    50   ~ 0
ERow1
Text Label 10450 8800 0    50   ~ 0
ECol0
Text Label 10450 8900 0    50   ~ 0
ECol1
Text Notes 8350 5650 0    50   ~ 0
THIS ALLOWS FOR SPEARATE DIMMING OF AOA 
$Comp
L Connector:Conn_01x03_Female J22
U 1 1 61723E00
P 7450 2050
F 0 "J22" H 7342 2335 50  0000 C CNN
F 1 "LEFT DIST" H 7342 2244 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x03_P2.54mm_Vertical" H 7450 2050 50  0001 C CNN
F 3 "~" H 7450 2050 50  0001 C CNN
	1    7450 2050
	-1   0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H6
U 1 1 6179AA80
P 1000 10900
F 0 "H6" H 1100 10946 50  0000 L CNN
F 1 "MountingHole" H 1100 10855 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1000 10900 50  0001 C CNN
F 3 "~" H 1000 10900 50  0001 C CNN
	1    1000 10900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H5
U 1 1 6179AA76
P 1000 10600
F 0 "H5" H 1100 10646 50  0000 L CNN
F 1 "MountingHole" H 1100 10555 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1000 10600 50  0001 C CNN
F 3 "~" H 1000 10600 50  0001 C CNN
	1    1000 10600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 61798C1B
P 1000 10300
F 0 "H4" H 1100 10346 50  0000 L CNN
F 1 "MountingHole" H 1100 10255 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1000 10300 50  0001 C CNN
F 3 "~" H 1000 10300 50  0001 C CNN
	1    1000 10300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 61798C11
P 1000 10000
F 0 "H3" H 1100 10046 50  0000 L CNN
F 1 "MountingHole" H 1100 9955 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1000 10000 50  0001 C CNN
F 3 "~" H 1000 10000 50  0001 C CNN
	1    1000 10000
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 61796F37
P 1000 9750
F 0 "H2" H 1100 9796 50  0000 L CNN
F 1 "MountingHole" H 1100 9705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1000 9750 50  0001 C CNN
F 3 "~" H 1000 9750 50  0001 C CNN
	1    1000 9750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 61796008
P 1000 9450
F 0 "H1" H 1100 9496 50  0000 L CNN
F 1 "MountingHole" H 1100 9405 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1000 9450 50  0001 C CNN
F 3 "~" H 1000 9450 50  0001 C CNN
	1    1000 9450
	1    0    0    -1  
$EndComp
$Comp
L OH---UIP---Led-Distribution-rescue:MAX7219ENG-MAX7219ENG-Max7219-rescue U6
U 1 1 617976A7
P 12050 7550
F 0 "U6" H 12050 9320 50  0000 C CNN
F 1 "MAX7219-F" H 12050 9229 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 12050 7550 50  0001 L BNN
F 3 "" H 12050 7550 50  0001 L BNN
F 4 "MAX7219ENG+" H 12050 7550 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 12050 7550 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 12050 7550 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 12050 7550 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 12050 7550 50  0001 L BNN "Field8"
	1    12050 7550
	-1   0    0    -1  
$EndComp
Text Label 12750 6350 0    50   ~ 0
E-DOUT-F-DIN
Text Label 11350 6250 2    50   ~ 0
DOUT
Text Label 12750 6250 0    50   ~ 0
CLK
Text Label 12750 6450 0    50   ~ 0
LOAD
Text Label 12750 6650 0    50   ~ 0
FCol0
Text Label 12750 6750 0    50   ~ 0
FCol1
Text Label 12750 6850 0    50   ~ 0
FCol2
Text Label 12750 6950 0    50   ~ 0
FCol3
Text Label 11350 6650 2    50   ~ 0
FCol4
Text Label 11350 6750 2    50   ~ 0
FCol5
Text Label 11350 6850 2    50   ~ 0
FCol6
Text Label 11350 6950 2    50   ~ 0
FCol7
Text Label 12750 8150 0    50   ~ 0
FRow0
Text Label 12750 7350 0    50   ~ 0
FRow1
Text Label 12750 7550 0    50   ~ 0
FRow2
Text Label 12750 7750 0    50   ~ 0
FRow3
Text Label 12750 7950 0    50   ~ 0
FRow4
Text Label 12750 8350 0    50   ~ 0
FRow5
Text Label 12750 8550 0    50   ~ 0
FRow6
Text Label 12750 8750 0    50   ~ 0
FRow7
$Comp
L power:GND #PWR023
U 1 1 617976C5
P 11000 8850
F 0 "#PWR023" H 11000 8600 50  0001 C CNN
F 1 "GND" H 11005 8677 50  0000 C CNN
F 2 "" H 11000 8850 50  0001 C CNN
F 3 "" H 11000 8850 50  0001 C CNN
	1    11000 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	11350 8850 11000 8850
$Comp
L power:+5V #PWR024
U 1 1 617976D0
P 11100 6050
F 0 "#PWR024" H 11100 5900 50  0001 C CNN
F 1 "+5V" H 11115 6223 50  0000 C CNN
F 2 "" H 11100 6050 50  0001 C CNN
F 3 "" H 11100 6050 50  0001 C CNN
	1    11100 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	11100 6050 11350 6050
$Comp
L Device:R_Small R6
U 1 1 617976DB
P 13050 7150
F 0 "R6" V 12854 7150 50  0000 C CNN
F 1 "10K" V 12945 7150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 13050 7150 50  0001 C CNN
F 3 "~" H 13050 7150 50  0001 C CNN
	1    13050 7150
	0    1    1    0   
$EndComp
Wire Wire Line
	12750 7150 12950 7150
$Comp
L power:+5V #PWR026
U 1 1 617976E6
P 13300 6950
F 0 "#PWR026" H 13300 6800 50  0001 C CNN
F 1 "+5V" H 13315 7123 50  0000 C CNN
F 2 "" H 13300 6950 50  0001 C CNN
F 3 "" H 13300 6950 50  0001 C CNN
	1    13300 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	13150 7150 13300 7150
Wire Wire Line
	13300 7150 13300 6950
$Comp
L Connector:Conn_01x08_Female J32
U 1 1 617976F2
P 13600 6650
F 0 "J32" H 13628 6626 50  0000 L CNN
F 1 " " H 13628 6535 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 13600 6650 50  0001 C CNN
F 3 "~" H 13600 6650 50  0001 C CNN
	1    13600 6650
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J33
U 1 1 617976FC
P 13600 7800
F 0 "J33" H 13628 7776 50  0000 L CNN
F 1 " " H 13628 7685 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 13600 7800 50  0001 C CNN
F 3 "~" H 13600 7800 50  0001 C CNN
	1    13600 7800
	-1   0    0    -1  
$EndComp
Text Label 13800 6350 0    50   ~ 0
FCol0
Text Label 13800 6450 0    50   ~ 0
FCol1
Text Label 13800 6550 0    50   ~ 0
FCol2
Text Label 13800 6650 0    50   ~ 0
FCol3
Text Label 13800 6750 0    50   ~ 0
FCol4
Text Label 13800 6850 0    50   ~ 0
FCol5
Text Label 13800 6950 0    50   ~ 0
FCol6
Text Label 13800 7050 0    50   ~ 0
FCol7
Text Label 13800 7500 0    50   ~ 0
FRow0
Text Label 13800 7600 0    50   ~ 0
FRow1
Text Label 13800 7700 0    50   ~ 0
FRow2
Text Label 13800 7800 0    50   ~ 0
FRow3
Text Label 13800 7900 0    50   ~ 0
FRow4
Text Label 13800 8000 0    50   ~ 0
FRow5
Text Label 13800 8100 0    50   ~ 0
FRow6
Text Label 13800 8200 0    50   ~ 0
FRow7
Text Notes 11700 5650 0    50   ~ 0
THIS ALLOWS FOR SPEARATE DIMMING OF RWR 
Text Label 8000 6250 2    50   ~ 0
E-DOUT-F-DIN
$Comp
L Connector:Conn_01x08_Female J34
U 1 1 617A87DD
P 13600 8750
F 0 "J34" H 13492 9235 50  0000 C CNN
F 1 "RWR" H 13492 9144 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 13600 8750 50  0001 C CNN
F 3 "~" H 13600 8750 50  0001 C CNN
	1    13600 8750
	-1   0    0    -1  
$EndComp
Text Label 13800 8450 0    50   ~ 0
FCol0
Text Label 13800 8550 0    50   ~ 0
FCol1
Text Label 13800 8650 0    50   ~ 0
FRow0
Text Label 13800 8750 0    50   ~ 0
FRow1
Text Label 13800 8850 0    50   ~ 0
FRow2
Text Label 13800 8950 0    50   ~ 0
FRow3
Text Label 13800 9050 0    50   ~ 0
FRow4
NoConn ~ 13800 9150
NoConn ~ 1100 650 
$Comp
L Device:C C3
U 1 1 617D70DC
P 14300 5250
F 0 "C3" H 14415 5296 50  0000 L CNN
F 1 "0.1uF" H 14415 5205 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm" H 14338 5100 50  0001 C CNN
F 3 "~" H 14300 5250 50  0001 C CNN
	1    14300 5250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR027
U 1 1 617D70E6
P 14100 4950
F 0 "#PWR027" H 14100 4800 50  0001 C CNN
F 1 "+5V" H 14115 5123 50  0000 C CNN
F 2 "" H 14100 4950 50  0001 C CNN
F 3 "" H 14100 4950 50  0001 C CNN
	1    14100 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 617D70F0
P 14100 5500
F 0 "#PWR028" H 14100 5250 50  0001 C CNN
F 1 "GND" H 14105 5327 50  0000 C CNN
F 2 "" H 14100 5500 50  0001 C CNN
F 3 "" H 14100 5500 50  0001 C CNN
	1    14100 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	14100 4950 14100 5100
Wire Wire Line
	14100 5100 14300 5100
Wire Wire Line
	14300 5400 14100 5400
Wire Wire Line
	14100 5400 14100 5500
Text Label 7100 10300 0    50   ~ 0
DRow5
$EndSCHEMATC