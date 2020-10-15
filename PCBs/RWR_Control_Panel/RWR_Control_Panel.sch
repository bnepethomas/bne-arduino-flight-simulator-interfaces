EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "RWR CONTROL PANEL"
Date "2020-10-16"
Rev "A"
Comp "F/A-18C SimPit"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 8400 3900 0    50   ~ 0
ROTARY SWITCH CONNECTIONs
Wire Notes Line
	9950 1050 9950 500 
Wire Notes Line
	10500 1050 9950 1050
$Comp
L Mechanical:MountingHole P6
U 1 1 56D73DD9
P 10400 650
F 0 "P6" V 10500 650 50  0000 C CNN
F 1 "CONN_01X01" V 10500 650 50  0001 C CNN
F 2 "Mounting_Holes:MountingHole_4.3mm_M4" H 10400 650 50  0001 C CNN
F 3 "" H 10400 650 50  0000 C CNN
	1    10400 650 
	0    -1   -1   0   
$EndComp
$Comp
L Mechanical:MountingHole P5
U 1 1 56D73DAE
P 10300 650
F 0 "P5" V 10400 650 50  0000 C CNN
F 1 "CONN_01X01" V 10400 650 50  0001 C CNN
F 2 "Mounting_Holes:MountingHole_4.3mm_M4" H 10300 650 50  0001 C CNN
F 3 "" H 10300 650 50  0000 C CNN
	1    10300 650 
	0    -1   -1   0   
$EndComp
$Comp
L Mechanical:MountingHole P4
U 1 1 56D73D86
P 10200 650
F 0 "P4" V 10300 650 50  0000 C CNN
F 1 "CONN_01X01" V 10300 650 50  0001 C CNN
F 2 "Mounting_Holes:MountingHole_4.3mm_M4" H 10200 650 50  0001 C CNN
F 3 "" H 10200 650 50  0000 C CNN
	1    10200 650 
	0    -1   -1   0   
$EndComp
$Comp
L Mechanical:MountingHole P3
U 1 1 56D73ADD
P 10100 650
F 0 "P3" V 10200 650 50  0000 C CNN
F 1 "CONN_01X01" V 10200 650 50  0001 C CNN
F 2 "Mounting_Holes:MountingHole_4.3mm_M4" H 10100 650 50  0001 C CNN
F 3 "" H 10100 650 50  0000 C CNN
	1    10100 650 
	0    -1   -1   0   
$EndComp
Text Notes 10100 1000 0    60   ~ 0
Holes
$Comp
L PT_Symbol_Library_v001:PT_Tactile_Switch_Led SW?
U 1 1 5F88CBEF
P 1700 2050
F 0 "SW?" H 1625 2483 50  0000 C CNN
F 1 "PT_Tactile_Switch_Led" H 1650 1900 50  0001 C CNN
F 2 "PT_Library_v001:PT_Small_Tactile_Switch_With_LED" H 1600 1975 50  0001 C CNN
F 3 "" H 1675 2050 50  0001 C CNN
	1    1700 2050
	1    0    0    -1  
$EndComp
$Comp
L PT_Symbol_Library_v001:PT_Tactile_Switch_Led SW?
U 1 1 5F88E3AB
P 2850 2100
F 0 "SW?" H 2775 2533 50  0000 C CNN
F 1 "PT_Tactile_Switch_Led" H 2800 1950 50  0001 C CNN
F 2 "PT_Library_v001:PT_Small_Tactile_Switch_With_LED" H 2750 2025 50  0001 C CNN
F 3 "" H 2825 2100 50  0001 C CNN
	1    2850 2100
	1    0    0    -1  
$EndComp
$Comp
L PT_Symbol_Library_v001:PT_Tactile_Switch_Led SW?
U 1 1 5F88F068
P 4000 2100
F 0 "SW?" H 3925 2533 50  0000 C CNN
F 1 "PT_Tactile_Switch_Led" H 3950 1950 50  0001 C CNN
F 2 "PT_Library_v001:PT_Small_Tactile_Switch_With_LED" H 3900 2025 50  0001 C CNN
F 3 "" H 3975 2100 50  0001 C CNN
	1    4000 2100
	1    0    0    -1  
$EndComp
$Comp
L PT_Symbol_Library_v001:PT_Tactile_Switch_Led SW?
U 1 1 5F88FB69
P 5450 2100
F 0 "SW?" H 5375 2533 50  0000 C CNN
F 1 "PT_Tactile_Switch_Led" H 5400 1950 50  0001 C CNN
F 2 "PT_Library_v001:PT_Small_Tactile_Switch_With_LED" H 5350 2025 50  0001 C CNN
F 3 "" H 5425 2100 50  0001 C CNN
	1    5450 2100
	1    0    0    -1  
$EndComp
$Comp
L PT_Symbol_Library_v001:PT_Tactile_Switch_Led SW?
U 1 1 5F89081E
P 6650 2100
F 0 "SW?" H 6575 2533 50  0000 C CNN
F 1 "PT_Tactile_Switch_Led" H 6600 1950 50  0001 C CNN
F 2 "PT_Library_v001:PT_Small_Tactile_Switch_With_LED" H 6550 2025 50  0001 C CNN
F 3 "" H 6625 2100 50  0001 C CNN
	1    6650 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D?
U 1 1 5F892A3D
P 7750 2350
F 0 "D?" H 7743 2095 50  0000 C CNN
F 1 "LED" H 7743 2186 50  0000 C CNN
F 2 "" H 7750 2350 50  0001 C CNN
F 3 "~" H 7750 2350 50  0001 C CNN
	1    7750 2350
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D?
U 1 1 5F894F21
P 8850 2350
F 0 "D?" H 8843 2095 50  0000 C CNN
F 1 "LED" H 8843 2186 50  0000 C CNN
F 2 "" H 8850 2350 50  0001 C CNN
F 3 "~" H 8850 2350 50  0001 C CNN
	1    8850 2350
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D?
U 1 1 5F895DE2
P 10100 2350
F 0 "D?" H 10093 2095 50  0000 C CNN
F 1 "LED" H 10093 2186 50  0000 C CNN
F 2 "" H 10100 2350 50  0001 C CNN
F 3 "~" H 10100 2350 50  0001 C CNN
	1    10100 2350
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D?
U 1 1 5F896786
P 8750 2900
F 0 "D?" H 8743 2645 50  0000 C CNN
F 1 "LED" H 8743 2736 50  0000 C CNN
F 2 "" H 8750 2900 50  0001 C CNN
F 3 "~" H 8750 2900 50  0001 C CNN
	1    8750 2900
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D?
U 1 1 5F897126
P 10050 2900
F 0 "D?" H 10043 2645 50  0000 C CNN
F 1 "LED" H 10043 2736 50  0000 C CNN
F 2 "" H 10050 2900 50  0001 C CNN
F 3 "~" H 10050 2900 50  0001 C CNN
	1    10050 2900
	-1   0    0    1   
$EndComp
$Comp
L Device:D D?
U 1 1 5F89CD39
P 3750 3250
F 0 "D?" H 3750 3034 50  0000 C CNN
F 1 "D" H 3750 3125 50  0000 C CNN
F 2 "" H 3750 3250 50  0001 C CNN
F 3 "~" H 3750 3250 50  0001 C CNN
	1    3750 3250
	-1   0    0    1   
$EndComp
$Comp
L PT_Symbol_Library_v001:PT_Conn_01x06 J?
U 1 1 5F95307A
P 9250 4450
F 0 "J?" H 9330 4442 50  0000 L CNN
F 1 "PT_Conn_01x06" H 9330 4351 50  0000 L CNN
F 2 "" H 9250 4450 50  0001 C CNN
F 3 "~" H 9250 4450 50  0001 C CNN
	1    9250 4450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 5F954C66
P 5300 4200
F 0 "J?" H 5380 4242 50  0000 L CNN
F 1 "Conn_01x03" H 5380 4151 50  0000 L CNN
F 2 "" H 5300 4200 50  0001 C CNN
F 3 "~" H 5300 4200 50  0001 C CNN
	1    5300 4200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 5F9552AF
P 6300 4200
F 0 "J?" H 6380 4242 50  0000 L CNN
F 1 "Conn_01x03" H 6380 4151 50  0000 L CNN
F 2 "" H 6300 4200 50  0001 C CNN
F 3 "~" H 6300 4200 50  0001 C CNN
	1    6300 4200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 5F955FCD
P 5300 5250
F 0 "J?" H 5380 5242 50  0000 L CNN
F 1 "Conn_01x04" H 5380 5151 50  0000 L CNN
F 2 "" H 5300 5250 50  0001 C CNN
F 3 "~" H 5300 5250 50  0001 C CNN
	1    5300 5250
	1    0    0    -1  
$EndComp
Text Notes 5300 3800 0    50   ~ 0
ROTARY POT CONNECTIONS
$Comp
L PT_Symbol_Library_v001:PT_Conn_01x06 J?
U 1 1 5F959C2D
P 1950 6300
F 0 "J?" H 2030 6292 50  0000 L CNN
F 1 "PT_Conn_01x06" H 2030 6201 50  0000 L CNN
F 2 "" H 1950 6300 50  0001 C CNN
F 3 "~" H 1950 6300 50  0001 C CNN
	1    1950 6300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 5F962C07
P 1550 2950
F 0 "J?" H 1468 3167 50  0000 C CNN
F 1 "Conn_01x02" H 1468 3076 50  0000 C CNN
F 2 "" H 1550 2950 50  0001 C CNN
F 3 "~" H 1550 2950 50  0001 C CNN
	1    1550 2950
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 5F96AF99
P 1950 5550
F 0 "J?" H 2030 5542 50  0000 L CNN
F 1 "Conn_01x02" H 2030 5451 50  0000 L CNN
F 2 "" H 1950 5550 50  0001 C CNN
F 3 "~" H 1950 5550 50  0001 C CNN
	1    1950 5550
	1    0    0    -1  
$EndComp
Text Notes 1700 5250 0    50   ~ 0
Digital Inputs
Text Notes 5100 4900 0    50   ~ 0
Analog Input
Text Notes 1350 2550 0    50   ~ 0
Backlighting
Text Notes 8950 1800 0    50   ~ 0
Led Outputs
Text Notes 6300 5550 0    50   ~ 0
For Analog inputs - \n3 Pin connectors \n1: +5V\n2: Input for pot\n3: GND\n\nFor aggregation connector\n1: +5V\n2: GND\n3: Input 1 \n4: Input 2 \n5: etc
$Comp
L Device:R R?
U 1 1 5F96D7EC
P 1100 2050
F 0 "R?" V 893 2050 50  0000 C CNN
F 1 "R" V 984 2050 50  0000 C CNN
F 2 "" V 1030 2050 50  0001 C CNN
F 3 "~" H 1100 2050 50  0001 C CNN
	1    1100 2050
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5F96E623
P 4750 2100
F 0 "R?" V 4543 2100 50  0000 C CNN
F 1 "R" V 4634 2100 50  0000 C CNN
F 2 "" V 4680 2100 50  0001 C CNN
F 3 "~" H 4750 2100 50  0001 C CNN
	1    4750 2100
	0    1    1    0   
$EndComp
$EndSCHEMATC
