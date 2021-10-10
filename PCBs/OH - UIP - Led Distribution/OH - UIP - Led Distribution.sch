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
L Max7219-rescue:MAX7219ENG-MAX7219ENG U1
U 1 1 5D2333EB
P 3600 3150
F 0 "U1" H 3600 4920 50  0000 C CNN
F 1 "MAX7219ENG-A" H 3600 4829 50  0000 C CNN
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
L Connector_Generic:Conn_01x06 J1
U 1 1 5D30502B
P 1950 1200
F 0 "J1" H 2030 1192 50  0000 L CNN
F 1 " " H 2030 1101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 1950 1200 50  0001 C CNN
F 3 "~" H 1950 1200 50  0001 C CNN
	1    1950 1200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5D3057CB
P 1950 2150
F 0 "J2" H 2030 2142 50  0000 L CNN
F 1 " " H 2030 2051 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 1950 2150 50  0001 C CNN
F 3 "~" H 1950 2150 50  0001 C CNN
	1    1950 2150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 5D306791
P 1750 1000
F 0 "#PWR05" H 1750 850 50  0001 C CNN
F 1 "+5V" H 1765 1173 50  0000 C CNN
F 2 "" H 1750 1000 50  0001 C CNN
F 3 "" H 1750 1000 50  0001 C CNN
	1    1750 1000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 5D306B64
P 1750 1950
F 0 "#PWR06" H 1750 1800 50  0001 C CNN
F 1 "+5V" H 1765 2123 50  0000 C CNN
F 2 "" H 1750 1950 50  0001 C CNN
F 3 "" H 1750 1950 50  0001 C CNN
	1    1750 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5D306D11
P 1050 1100
F 0 "#PWR01" H 1050 850 50  0001 C CNN
F 1 "GND" H 1055 927 50  0000 C CNN
F 2 "" H 1050 1100 50  0001 C CNN
F 3 "" H 1050 1100 50  0001 C CNN
	1    1050 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5D306E49
P 1050 2050
F 0 "#PWR02" H 1050 1800 50  0001 C CNN
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
Text Label 1600 1200 0    50   ~ 0
DIN
Text Label 1550 2150 0    50   ~ 0
DOUT
Text Label 1600 1400 0    50   ~ 0
CLK
Text Label 1600 2350 0    50   ~ 0
CLK
Text Label 1550 1300 0    50   ~ 0
LOAD
Text Label 1550 2250 0    50   ~ 0
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
L power:GND #PWR07
U 1 1 5D309D4D
P 2550 4450
F 0 "#PWR07" H 2550 4200 50  0001 C CNN
F 1 "GND" H 2555 4277 50  0000 C CNN
F 2 "" H 2550 4450 50  0001 C CNN
F 3 "" H 2550 4450 50  0001 C CNN
	1    2550 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 4450 2550 4450
$Comp
L power:+5V #PWR08
U 1 1 5D30A028
P 2650 1650
F 0 "#PWR08" H 2650 1500 50  0001 C CNN
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
L power:+5V #PWR03
U 1 1 5D30B9F5
P 1550 3150
F 0 "#PWR03" H 1550 3000 50  0001 C CNN
F 1 "+5V" H 1565 3323 50  0000 C CNN
F 2 "" H 1550 3150 50  0001 C CNN
F 3 "" H 1550 3150 50  0001 C CNN
	1    1550 3150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5D30BEF7
P 1550 3700
F 0 "#PWR04" H 1550 3450 50  0001 C CNN
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
L Connector:Conn_01x08_Female J3
U 1 1 5D30CF47
P 7850 2350
F 0 "J3" H 7878 2326 50  0000 L CNN
F 1 " " H 7878 2235 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 7850 2350 50  0001 C CNN
F 3 "~" H 7850 2350 50  0001 C CNN
	1    7850 2350
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J4
U 1 1 5D30D8BE
P 7750 3550
F 0 "J4" H 7778 3526 50  0000 L CNN
F 1 " " H 7778 3435 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 7750 3550 50  0001 C CNN
F 3 "~" H 7750 3550 50  0001 C CNN
	1    7750 3550
	-1   0    0    -1  
$EndComp
Text Label 8050 2050 0    50   ~ 0
Col0
Text Label 8050 2150 0    50   ~ 0
Col1
Text Label 8050 2250 0    50   ~ 0
Col2
Text Label 8050 2350 0    50   ~ 0
Col3
Text Label 8050 2450 0    50   ~ 0
Col4
Text Label 8050 2550 0    50   ~ 0
Col5
Text Label 8050 2650 0    50   ~ 0
Col6
Text Label 8050 2750 0    50   ~ 0
Col7
Text Label 7950 3250 0    50   ~ 0
Row0
Text Label 7950 3350 0    50   ~ 0
Row1
Text Label 7950 3450 0    50   ~ 0
Row2
Text Label 7950 3550 0    50   ~ 0
Row3
Text Label 7950 3650 0    50   ~ 0
Row4
Text Label 7950 3750 0    50   ~ 0
Row5
Text Label 7950 3850 0    50   ~ 0
Row6
Text Label 7950 3950 0    50   ~ 0
Row7
$Comp
L Device:R_Small R1
U 1 1 5D32B4BD
P 4600 2750
F 0 "R1" V 4404 2750 50  0000 C CNN
F 1 "10K" V 4495 2750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 4600 2750 50  0001 C CNN
F 3 "~" H 4600 2750 50  0001 C CNN
	1    4600 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	4300 2750 4500 2750
$Comp
L power:+5V #PWR0101
U 1 1 5D32C649
P 4850 2550
F 0 "#PWR0101" H 4850 2400 50  0001 C CNN
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
L Connector_Generic:Conn_01x03 J16
U 1 1 5D361879
P 6850 8850
F 0 "J16" H 6768 9167 50  0000 C CNN
F 1 " " H 6768 9076 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 6850 8850 50  0001 C CNN
F 3 "~" H 6850 8850 50  0001 C CNN
	1    6850 8850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x10 J15
U 1 1 5D3620E4
P 6000 9100
F 0 "J15" H 5918 9717 50  0000 C CNN
F 1 " " H 5918 9626 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x10_P2.54mm_Vertical" H 6000 9100 50  0001 C CNN
F 3 "~" H 6000 9100 50  0001 C CNN
	1    6000 9100
	-1   0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0102
U 1 1 5D3664F5
P 6200 9600
F 0 "#PWR0102" H 6200 9350 50  0001 C CNN
F 1 "GNDA" H 6205 9427 50  0000 C CNN
F 2 "" H 6200 9600 50  0001 C CNN
F 3 "" H 6200 9600 50  0001 C CNN
	1    6200 9600
	1    0    0    -1  
$EndComp
Text Label 6200 9600 0    50   ~ 0
GNDA
Text Label 7050 8750 0    50   ~ 0
GNDA
Text Label 7050 8850 0    50   ~ 0
5VA
Text Label 6200 8700 0    50   ~ 0
Servo0
Text Label 6200 8800 0    50   ~ 0
Servo1
Text Label 6200 8900 0    50   ~ 0
Servo2
Text Label 6200 9000 0    50   ~ 0
Servo3
Text Label 6200 9100 0    50   ~ 0
Servo4
Text Label 6200 9200 0    50   ~ 0
Servo5
Text Label 6200 9300 0    50   ~ 0
Servo6
Text Label 6200 9400 0    50   ~ 0
Servo7
Text Label 6200 9500 0    50   ~ 0
Servo8
$Comp
L Connector_Generic:Conn_01x03 J17
U 1 1 5D370500
P 6850 9450
F 0 "J17" H 6768 9767 50  0000 C CNN
F 1 " " H 6768 9676 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 6850 9450 50  0001 C CNN
F 3 "~" H 6850 9450 50  0001 C CNN
	1    6850 9450
	-1   0    0    -1  
$EndComp
Text Label 7050 9350 0    50   ~ 0
GNDA
Text Label 7050 9450 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J18
U 1 1 5D372863
P 7450 8850
F 0 "J18" H 7368 9167 50  0000 C CNN
F 1 " " H 7368 9076 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 7450 8850 50  0001 C CNN
F 3 "~" H 7450 8850 50  0001 C CNN
	1    7450 8850
	-1   0    0    -1  
$EndComp
Text Label 7650 8750 0    50   ~ 0
GNDA
Text Label 7650 8850 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J19
U 1 1 5D37286F
P 7450 9450
F 0 "J19" H 7368 9767 50  0000 C CNN
F 1 " " H 7368 9676 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 7450 9450 50  0001 C CNN
F 3 "~" H 7450 9450 50  0001 C CNN
	1    7450 9450
	-1   0    0    -1  
$EndComp
Text Label 7650 9350 0    50   ~ 0
GNDA
Text Label 7650 9450 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J20
U 1 1 5D3743D9
P 8050 8850
F 0 "J20" H 7968 9167 50  0000 C CNN
F 1 " " H 7968 9076 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8050 8850 50  0001 C CNN
F 3 "~" H 8050 8850 50  0001 C CNN
	1    8050 8850
	-1   0    0    -1  
$EndComp
Text Label 8250 8750 0    50   ~ 0
GNDA
Text Label 8250 8850 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J21
U 1 1 5D3743E5
P 8050 9450
F 0 "J21" H 7968 9767 50  0000 C CNN
F 1 " " H 7968 9676 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8050 9450 50  0001 C CNN
F 3 "~" H 8050 9450 50  0001 C CNN
	1    8050 9450
	-1   0    0    -1  
$EndComp
Text Label 8250 9350 0    50   ~ 0
GNDA
Text Label 8250 9450 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J22
U 1 1 5D3743F1
P 8650 8850
F 0 "J22" H 8568 9167 50  0000 C CNN
F 1 " " H 8568 9076 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8650 8850 50  0001 C CNN
F 3 "~" H 8650 8850 50  0001 C CNN
	1    8650 8850
	-1   0    0    -1  
$EndComp
Text Label 8850 8750 0    50   ~ 0
GNDA
Text Label 8850 8850 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J23
U 1 1 5D3743FD
P 8650 9450
F 0 "J23" H 8568 9767 50  0000 C CNN
F 1 " " H 8568 9676 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8650 9450 50  0001 C CNN
F 3 "~" H 8650 9450 50  0001 C CNN
	1    8650 9450
	-1   0    0    -1  
$EndComp
Text Label 8850 9350 0    50   ~ 0
GNDA
Text Label 8850 9450 0    50   ~ 0
5VA
$Comp
L Connector_Generic:Conn_01x03 J24
U 1 1 5D377526
P 9400 8850
F 0 "J24" H 9318 9167 50  0000 C CNN
F 1 " " H 9318 9076 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 9400 8850 50  0001 C CNN
F 3 "~" H 9400 8850 50  0001 C CNN
	1    9400 8850
	-1   0    0    -1  
$EndComp
Text Label 9600 8750 0    50   ~ 0
GNDA
Text Label 9600 8850 0    50   ~ 0
5VA
Text Label 7050 8950 0    50   ~ 0
Servo0
Text Label 7650 8950 0    50   ~ 0
Servo1
Text Label 8250 8950 0    50   ~ 0
Servo2
Text Label 8850 8950 0    50   ~ 0
Servo3
Text Label 9600 8950 0    50   ~ 0
Servo4
Text Label 7050 9550 0    50   ~ 0
Servo5
Text Label 7650 9550 0    50   ~ 0
Servo6
Text Label 8250 9550 0    50   ~ 0
Servo7
Text Label 8850 9550 0    50   ~ 0
Servo8
$Comp
L Transistor_Array:ULN2003A U2
U 1 1 5D384716
P 2300 9900
F 0 "U2" H 2300 10567 50  0000 C CNN
F 1 "ULN2003A" H 2300 10476 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2350 9350 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 2400 9700 50  0001 C CNN
	1    2300 9900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J25
U 1 1 5D386AC6
P 1000 9800
F 0 "J25" H 892 10285 50  0000 C CNN
F 1 " " H 892 10194 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 1000 9800 50  0001 C CNN
F 3 "~" H 1000 9800 50  0001 C CNN
	1    1000 9800
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J27
U 1 1 5D38812F
P 3700 9850
F 0 "J27" H 3592 10335 50  0000 C CNN
F 1 " " H 3592 10244 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 3700 9850 50  0001 C CNN
F 3 "~" H 3700 9850 50  0001 C CNN
	1    3700 9850
	-1   0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0103
U 1 1 5D38A859
P 2300 10500
F 0 "#PWR0103" H 2300 10250 50  0001 C CNN
F 1 "GNDD" H 2304 10345 50  0000 C CNN
F 2 "" H 2300 10500 50  0001 C CNN
F 3 "" H 2300 10500 50  0001 C CNN
	1    2300 10500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0104
U 1 1 5D38B8BE
P 1200 10200
F 0 "#PWR0104" H 1200 9950 50  0001 C CNN
F 1 "GNDD" H 1204 10045 50  0000 C CNN
F 2 "" H 1200 10200 50  0001 C CNN
F 3 "" H 1200 10200 50  0001 C CNN
	1    1200 10200
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0105
U 1 1 5D38BFA3
P 3300 10350
F 0 "#PWR0105" H 3300 10200 50  0001 C CNN
F 1 "+12V" H 3315 10523 50  0000 C CNN
F 2 "" H 3300 10350 50  0001 C CNN
F 3 "" H 3300 10350 50  0001 C CNN
	1    3300 10350
	1    0    0    -1  
$EndComp
Text Label 1750 9700 0    50   ~ 0
In0
Text Label 1200 9500 0    50   ~ 0
In0
Text Label 1200 9600 0    50   ~ 0
In1
Text Label 1200 9700 0    50   ~ 0
In2
Text Label 1200 9800 0    50   ~ 0
In3
Text Label 1200 9900 0    50   ~ 0
In4
Text Label 1200 10000 0    50   ~ 0
In5
Text Label 1200 10100 0    50   ~ 0
In6
Text Label 1750 10300 0    50   ~ 0
In6
Text Label 1750 10200 0    50   ~ 0
In5
Text Label 1750 10100 0    50   ~ 0
In4
Text Label 1750 10000 0    50   ~ 0
In3
Text Label 1750 9900 0    50   ~ 0
In2
Text Label 1750 9800 0    50   ~ 0
In1
Text Label 2700 9700 0    50   ~ 0
Out0
Text Label 2700 9800 0    50   ~ 0
Out1
Text Label 2700 9900 0    50   ~ 0
Out2
Text Label 2700 10000 0    50   ~ 0
Out3
Text Label 2700 10100 0    50   ~ 0
Out4
Text Label 2700 10200 0    50   ~ 0
Out5
Text Label 2700 10300 0    50   ~ 0
Out6
Text Label 3900 9550 0    50   ~ 0
Out0
Text Label 3900 9650 0    50   ~ 0
Out1
Text Label 3900 9750 0    50   ~ 0
Out2
Text Label 3900 9850 0    50   ~ 0
Out3
Text Label 3900 9950 0    50   ~ 0
Out4
Text Label 3900 10050 0    50   ~ 0
Out5
Text Label 3900 10150 0    50   ~ 0
Out6
Wire Wire Line
	3900 10250 3900 10350
Wire Wire Line
	3300 10350 3900 10350
$Comp
L Connector:Conn_01x02_Male J26
U 1 1 5D39C3C7
P 3150 10700
F 0 "J26" H 3258 10881 50  0000 C CNN
F 1 " " H 3258 10790 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 3150 10700 50  0001 C CNN
F 3 "~" H 3150 10700 50  0001 C CNN
	1    3150 10700
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J28
U 1 1 5D25D4CE
P 9750 9450
F 0 "J28" H 9858 9631 50  0000 C CNN
F 1 " " H 9858 9540 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 9750 9450 50  0001 C CNN
F 3 "~" H 9750 9450 50  0001 C CNN
	1    9750 9450
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0106
U 1 1 5D26E366
P 3750 10600
F 0 "#PWR0106" H 3750 10450 50  0001 C CNN
F 1 "+12V" H 3765 10773 50  0000 C CNN
F 2 "" H 3750 10600 50  0001 C CNN
F 3 "" H 3750 10600 50  0001 C CNN
	1    3750 10600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 10600 3350 10600
Wire Wire Line
	3350 10600 3350 10700
$Comp
L power:GNDD #PWR0107
U 1 1 5D26F5F7
P 3500 10950
F 0 "#PWR0107" H 3500 10700 50  0001 C CNN
F 1 "GNDD" H 3504 10795 50  0000 C CNN
F 2 "" H 3500 10950 50  0001 C CNN
F 3 "" H 3500 10950 50  0001 C CNN
	1    3500 10950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 10800 3500 10800
Wire Wire Line
	3500 10800 3500 10950
Text Label 9950 9450 0    50   ~ 0
5VA
Text Label 9950 9550 0    50   ~ 0
GNDA
$Comp
L Connector:Conn_01x08_Female J31
U 1 1 5D2A5E5D
P 14650 8900
F 0 "J31" H 14678 8876 50  0000 L CNN
F 1 " " H 14678 8785 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 14650 8900 50  0001 C CNN
F 3 "~" H 14650 8900 50  0001 C CNN
	1    14650 8900
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J32
U 1 1 5D2A709D
P 15150 8900
F 0 "J32" H 15178 8876 50  0000 L CNN
F 1 " " H 15178 8785 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 15150 8900 50  0001 C CNN
F 3 "~" H 15150 8900 50  0001 C CNN
	1    15150 8900
	1    0    0    -1  
$EndComp
Wire Wire Line
	14850 8600 14950 8600
Wire Wire Line
	14850 8700 14950 8700
Wire Wire Line
	14850 8800 14950 8800
Wire Wire Line
	14850 8900 14950 8900
Wire Wire Line
	14850 9000 14950 9000
Wire Wire Line
	14850 9100 14950 9100
Wire Wire Line
	14850 9200 14950 9200
Wire Wire Line
	14850 9300 14950 9300
$Comp
L Connector:Conn_01x08_Female J29
U 1 1 5D2B0943
P 12750 8850
F 0 "J29" H 12778 8826 50  0000 L CNN
F 1 " " H 12778 8735 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 12750 8850 50  0001 C CNN
F 3 "~" H 12750 8850 50  0001 C CNN
	1    12750 8850
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J30
U 1 1 5D2B094D
P 13250 8850
F 0 "J30" H 13278 8826 50  0000 L CNN
F 1 " " H 13278 8735 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 13250 8850 50  0001 C CNN
F 3 "~" H 13250 8850 50  0001 C CNN
	1    13250 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	12950 8550 13050 8550
Wire Wire Line
	12950 8650 13050 8650
Wire Wire Line
	12950 8750 13050 8750
Wire Wire Line
	12950 8850 13050 8850
Wire Wire Line
	12950 8950 13050 8950
Wire Wire Line
	12950 9050 13050 9050
Wire Wire Line
	12950 9150 13050 9150
Wire Wire Line
	12950 9250 13050 9250
Text Notes 4550 10100 0    50   ~ 0
There are 3 different grounds in this diagram as they are 3 separate functions which should be electrically ioslated from one another
Text Notes -50  9150 0    50   ~ 0
If inductive loads such as relays or magnetically switches are used, either use a fallback diode on the coil or tie pin 9 to the coil supply rail
Wire Notes Line
	12500 8400 12500 9500
Wire Notes Line
	12500 9500 15550 9500
Wire Notes Line
	15550 9500 15550 8400
Wire Notes Line
	15550 8400 12500 8400
Text Notes 13800 8850 0    50   ~ 0
SCRATCH PAD
$Comp
L Max7219-rescue:MAX7219ENG-MAX7219ENG U4
U 1 1 61653072
P 9950 3200
F 0 "U4" H 9950 4970 50  0000 C CNN
F 1 "MAX7219ENG-B" H 9950 4879 50  0000 C CNN
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
DIN
Text Label 9000 1900 0    50   ~ 0
DOUT
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
L power:GND #PWR0108
U 1 1 61653090
P 8900 4500
F 0 "#PWR0108" H 8900 4250 50  0001 C CNN
F 1 "GND" H 8905 4327 50  0000 C CNN
F 2 "" H 8900 4500 50  0001 C CNN
F 3 "" H 8900 4500 50  0001 C CNN
	1    8900 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 4500 8900 4500
$Comp
L power:+5V #PWR0109
U 1 1 6165309B
P 9000 1700
F 0 "#PWR0109" H 9000 1550 50  0001 C CNN
F 1 "+5V" H 9015 1873 50  0000 C CNN
F 2 "" H 9000 1700 50  0001 C CNN
F 3 "" H 9000 1700 50  0001 C CNN
	1    9000 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 1700 9250 1700
$Comp
L Device:R_Small R3
U 1 1 616530A6
P 10950 2800
F 0 "R3" V 10754 2800 50  0000 C CNN
F 1 "10K" V 10845 2800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 10950 2800 50  0001 C CNN
F 3 "~" H 10950 2800 50  0001 C CNN
	1    10950 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	10650 2800 10850 2800
$Comp
L power:+5V #PWR0110
U 1 1 616530B1
P 11200 2600
F 0 "#PWR0110" H 11200 2450 50  0001 C CNN
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
L Max7219-rescue:MAX7219ENG-MAX7219ENG U3
U 1 1 616563B5
P 3600 6950
F 0 "U3" H 3600 8720 50  0000 C CNN
F 1 "MAX7219ENG-D" H 3600 8629 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 3600 6950 50  0001 L BNN
F 3 "" H 3600 6950 50  0001 L BNN
F 4 "MAX7219ENG+" H 3600 6950 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 3600 6950 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 3600 6950 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 3600 6950 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 3600 6950 50  0001 L BNN "Field8"
	1    3600 6950
	-1   0    0    -1  
$EndComp
Text Label 4300 5750 0    50   ~ 0
DIN
Text Label 2650 5650 0    50   ~ 0
DOUT
Text Label 4300 5650 0    50   ~ 0
CLK
Text Label 4300 5850 0    50   ~ 0
LOAD
Text Label 4300 6050 0    50   ~ 0
DCol0
Text Label 4300 6150 0    50   ~ 0
DCol1
Text Label 4300 6250 0    50   ~ 0
DCol2
Text Label 4300 6350 0    50   ~ 0
DCol3
Text Label 2900 6050 2    50   ~ 0
DCol4
Text Label 2900 6150 2    50   ~ 0
DCol5
Text Label 2900 6250 2    50   ~ 0
DCol6
Text Label 2900 6350 2    50   ~ 0
DCol7
Text Label 4300 7550 0    50   ~ 0
DRow0
Text Label 4300 6750 0    50   ~ 0
DRow1
Text Label 4300 6950 0    50   ~ 0
DRow2
Text Label 4300 7150 0    50   ~ 0
DRow3
Text Label 4300 7350 0    50   ~ 0
DRow4
Text Label 4300 7750 0    50   ~ 0
DRow5
Text Label 4300 7950 0    50   ~ 0
DRow6
Text Label 4300 8150 0    50   ~ 0
DRow7
$Comp
L power:GND #PWR0111
U 1 1 616563D3
P 2550 8250
F 0 "#PWR0111" H 2550 8000 50  0001 C CNN
F 1 "GND" H 2555 8077 50  0000 C CNN
F 2 "" H 2550 8250 50  0001 C CNN
F 3 "" H 2550 8250 50  0001 C CNN
	1    2550 8250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 8250 2550 8250
$Comp
L power:+5V #PWR0112
U 1 1 616563DE
P 2650 5450
F 0 "#PWR0112" H 2650 5300 50  0001 C CNN
F 1 "+5V" H 2665 5623 50  0000 C CNN
F 2 "" H 2650 5450 50  0001 C CNN
F 3 "" H 2650 5450 50  0001 C CNN
	1    2650 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 5450 2900 5450
$Comp
L Device:R_Small R2
U 1 1 616563E9
P 4600 6550
F 0 "R2" V 4404 6550 50  0000 C CNN
F 1 "10K" V 4495 6550 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 4600 6550 50  0001 C CNN
F 3 "~" H 4600 6550 50  0001 C CNN
	1    4600 6550
	0    1    1    0   
$EndComp
Wire Wire Line
	4300 6550 4500 6550
$Comp
L power:+5V #PWR0113
U 1 1 616563F4
P 4850 6350
F 0 "#PWR0113" H 4850 6200 50  0001 C CNN
F 1 "+5V" H 4865 6523 50  0000 C CNN
F 2 "" H 4850 6350 50  0001 C CNN
F 3 "" H 4850 6350 50  0001 C CNN
	1    4850 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 6550 4850 6550
Wire Wire Line
	4850 6550 4850 6350
$Comp
L Max7219-rescue:MAX7219ENG-MAX7219ENG U5
U 1 1 6165B7FE
P 13500 5950
F 0 "U5" H 13500 7720 50  0000 C CNN
F 1 "MAX7219ENG-C" H 13500 7629 50  0000 C CNN
F 2 "Package_DIP:DIP-24_W7.62mm_Socket" H 13500 5950 50  0001 L BNN
F 3 "" H 13500 5950 50  0001 L BNN
F 4 "MAX7219ENG+" H 13500 5950 50  0001 L BNN "Field4"
F 5 "https://www.digikey.com.au/product-detail/en/maxim-integrated/MAX7219ENG-/MAX7219ENG--ND/774127?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 13500 5950 50  0001 L BNN "Field5"
F 6 "Maxim Integrated" H 13500 5950 50  0001 L BNN "Field6"
F 7 "MAX7219ENG+-ND" H 13500 5950 50  0001 L BNN "Field7"
F 8 "Driver; LED controller; 330mA; Channels: 2; DIP24" H 13500 5950 50  0001 L BNN "Field8"
	1    13500 5950
	-1   0    0    -1  
$EndComp
Text Label 14200 4750 0    50   ~ 0
DIN
Text Label 12550 4650 0    50   ~ 0
DOUT
Text Label 14200 4650 0    50   ~ 0
CLK
Text Label 14200 4850 0    50   ~ 0
LOAD
Text Label 14200 5050 0    50   ~ 0
CCol0
Text Label 14200 5150 0    50   ~ 0
CCol1
Text Label 14200 5250 0    50   ~ 0
CCol2
Text Label 14200 5350 0    50   ~ 0
CCol3
Text Label 12600 5050 0    50   ~ 0
CCol4
Text Label 12600 5150 0    50   ~ 0
CCol5
Text Label 12600 5250 0    50   ~ 0
CCol6
Text Label 12600 5350 0    50   ~ 0
CCol7
Text Label 14200 6550 0    50   ~ 0
CRow0
Text Label 14200 5750 0    50   ~ 0
CRow1
Text Label 14200 5950 0    50   ~ 0
CRow2
Text Label 14200 6150 0    50   ~ 0
CRow3
Text Label 14200 6350 0    50   ~ 0
CRow4
Text Label 14200 6750 0    50   ~ 0
CRow5
Text Label 14200 6950 0    50   ~ 0
CRow6
Text Label 14200 7150 0    50   ~ 0
CRow7
$Comp
L power:GND #PWR0114
U 1 1 6165B81C
P 12450 7250
F 0 "#PWR0114" H 12450 7000 50  0001 C CNN
F 1 "GND" H 12455 7077 50  0000 C CNN
F 2 "" H 12450 7250 50  0001 C CNN
F 3 "" H 12450 7250 50  0001 C CNN
	1    12450 7250
	1    0    0    -1  
$EndComp
Wire Wire Line
	12800 7250 12450 7250
$Comp
L power:+5V #PWR0115
U 1 1 6165B827
P 12550 4450
F 0 "#PWR0115" H 12550 4300 50  0001 C CNN
F 1 "+5V" H 12565 4623 50  0000 C CNN
F 2 "" H 12550 4450 50  0001 C CNN
F 3 "" H 12550 4450 50  0001 C CNN
	1    12550 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	12550 4450 12800 4450
$Comp
L Device:R_Small R4
U 1 1 6165B832
P 14500 5550
F 0 "R4" V 14304 5550 50  0000 C CNN
F 1 "10K" V 14395 5550 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P5.08mm_Vertical" H 14500 5550 50  0001 C CNN
F 3 "~" H 14500 5550 50  0001 C CNN
	1    14500 5550
	0    1    1    0   
$EndComp
Wire Wire Line
	14200 5550 14400 5550
$Comp
L power:+5V #PWR0116
U 1 1 6165B83D
P 14750 5350
F 0 "#PWR0116" H 14750 5200 50  0001 C CNN
F 1 "+5V" H 14765 5523 50  0000 C CNN
F 2 "" H 14750 5350 50  0001 C CNN
F 3 "" H 14750 5350 50  0001 C CNN
	1    14750 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	14600 5550 14750 5550
Wire Wire Line
	14750 5550 14750 5350
$Comp
L Connector:Conn_01x08_Female J6
U 1 1 6162DC25
P 5600 2350
F 0 "J6" H 5628 2326 50  0000 L CNN
F 1 " " H 5628 2235 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 5600 2350 50  0001 C CNN
F 3 "~" H 5600 2350 50  0001 C CNN
	1    5600 2350
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J5
U 1 1 6162DC2F
P 5500 3550
F 0 "J5" H 5528 3526 50  0000 L CNN
F 1 " " H 5528 3435 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 5500 3550 50  0001 C CNN
F 3 "~" H 5500 3550 50  0001 C CNN
	1    5500 3550
	-1   0    0    -1  
$EndComp
Text Label 5800 2050 0    50   ~ 0
Col0
Text Label 5800 2150 0    50   ~ 0
Col1
Text Label 5800 2250 0    50   ~ 0
Col2
Text Label 5800 2350 0    50   ~ 0
Col3
Text Label 5800 2450 0    50   ~ 0
Col4
Text Label 5800 2550 0    50   ~ 0
Col5
Text Label 5800 2650 0    50   ~ 0
Col6
Text Label 5800 2750 0    50   ~ 0
Col7
Text Label 5700 3250 0    50   ~ 0
Row0
Text Label 5700 3350 0    50   ~ 0
Row1
Text Label 5700 3450 0    50   ~ 0
Row2
Text Label 5700 3550 0    50   ~ 0
Row3
Text Label 5700 3650 0    50   ~ 0
Row4
Text Label 5700 3750 0    50   ~ 0
Row5
Text Label 5700 3850 0    50   ~ 0
Row6
Text Label 5700 3950 0    50   ~ 0
ARow7
$EndSCHEMATC
