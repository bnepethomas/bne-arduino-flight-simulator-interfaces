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
L Connector:Conn_01x02_Female J1
U 1 1 5EED210C
P 1400 1600
F 0 "J1" H 1428 1576 50  0000 L CNN
F 1 "12V" H 1428 1485 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 1400 1600 50  0001 C CNN
F 3 "~" H 1400 1600 50  0001 C CNN
	1    1400 1600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 5EED23E2
P 1400 1900
F 0 "J2" H 1428 1876 50  0000 L CNN
F 1 "5V" H 1428 1785 50  0000 L CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 1400 1900 50  0001 C CNN
F 3 "~" H 1400 1900 50  0001 C CNN
	1    1400 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J3
U 1 1 5EED2E0B
P 1400 2300
F 0 "J3" H 1428 2276 50  0000 L CNN
F 1 "MULTI" H 1428 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1400 2300 50  0001 C CNN
F 3 "~" H 1400 2300 50  0001 C CNN
	1    1400 2300
	1    0    0    -1  
$EndComp
Text Label 1200 2200 2    50   ~ 0
Green12V+
Text Label 1200 2300 2    50   ~ 0
GreenGND
Text Label 1200 2400 2    50   ~ 0
White+12V
Text Label 1200 2500 2    50   ~ 0
WhiteGND
Text Label 1200 1900 2    50   ~ 0
+5V
Text Label 1200 2000 2    50   ~ 0
5VGND
Text Label 1200 1600 2    50   ~ 0
+12V
Text Label 1200 1700 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J6
U 1 1 5EED8B2E
P 5700 2250
F 0 "J6" H 5728 2226 50  0000 L CNN
F 1 "12V" H 5728 2135 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5700 2250 50  0001 C CNN
F 3 "~" H 5700 2250 50  0001 C CNN
	1    5700 2250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J19
U 1 1 5EED8B38
P 7450 4850
F 0 "J19" H 7478 4826 50  0000 L CNN
F 1 "5V" H 7478 4735 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7450 4850 50  0001 C CNN
F 3 "~" H 7450 4850 50  0001 C CNN
	1    7450 4850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J4
U 1 1 5EED8B42
P 2450 4000
F 0 "J4" H 2478 3976 50  0000 L CNN
F 1 "MULTI" H 2478 3885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 2450 4000 50  0001 C CNN
F 3 "~" H 2450 4000 50  0001 C CNN
	1    2450 4000
	1    0    0    -1  
$EndComp
Text Label 2250 3900 2    50   ~ 0
Green12V+
Text Label 2250 4000 2    50   ~ 0
GreenGND
Text Label 2250 4100 2    50   ~ 0
White+12V
Text Label 2250 4200 2    50   ~ 0
WhiteGND
Text Label 7250 4850 2    50   ~ 0
+5V
Text Label 7250 4950 2    50   ~ 0
5VGND
Text Label 5500 2250 2    50   ~ 0
+12V
Text Label 5500 2350 2    50   ~ 0
12VGND
$Comp
L PT_Symbol_Library_v001:PT_SmallPCB SPCB1
U 1 1 5EEE094C
P 1550 6950
F 0 "SPCB1" H 1778 6959 50  0000 L CNN
F 1 "PT_SmallPCB" H 1778 6868 50  0000 L CNN
F 2 "PT_Library_v001:PT_General_PCB_Outline" H 1525 6950 50  0001 C CNN
F 3 "" H 1525 6950 50  0001 C CNN
	1    1550 6950
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 5EEE0D9A
P 2750 6750
F 0 "H1" H 2850 6796 50  0000 L CNN
F 1 "MountingHole" H 2850 6705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 6750 50  0001 C CNN
F 3 "~" H 2750 6750 50  0001 C CNN
	1    2750 6750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5EEE14AA
P 2750 7000
F 0 "H2" H 2850 7046 50  0000 L CNN
F 1 "MountingHole" H 2850 6955 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 7000 50  0001 C CNN
F 3 "~" H 2750 7000 50  0001 C CNN
	1    2750 7000
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5EEE1A18
P 2750 7250
F 0 "H3" H 2850 7296 50  0000 L CNN
F 1 "MountingHole" H 2850 7205 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 7250 50  0001 C CNN
F 3 "~" H 2750 7250 50  0001 C CNN
	1    2750 7250
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5EEE1A22
P 2750 7500
F 0 "H4" H 2850 7546 50  0000 L CNN
F 1 "MountingHole" H 2850 7455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2750 7500 50  0001 C CNN
F 3 "~" H 2750 7500 50  0001 C CNN
	1    2750 7500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J17
U 1 1 5EEE4478
P 7250 2550
F 0 "J17" H 7278 2526 50  0000 L CNN
F 1 "12V" H 7278 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7250 2550 50  0001 C CNN
F 3 "~" H 7250 2550 50  0001 C CNN
	1    7250 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J25
U 1 1 5EEE4482
P 8100 4600
F 0 "J25" H 8128 4576 50  0000 L CNN
F 1 "5V" H 8128 4485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8100 4600 50  0001 C CNN
F 3 "~" H 8100 4600 50  0001 C CNN
	1    8100 4600
	1    0    0    -1  
$EndComp
Text Label 7900 4600 2    50   ~ 0
+5V
Text Label 7900 4700 2    50   ~ 0
5VGND
Text Label 7050 2550 2    50   ~ 0
+12V
Text Label 7050 2650 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J10
U 1 1 5EEE6572
P 5800 4050
F 0 "J10" H 5828 4026 50  0000 L CNN
F 1 "WHITE" H 5828 3935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5800 4050 50  0001 C CNN
F 3 "~" H 5800 4050 50  0001 C CNN
	1    5800 4050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J7
U 1 1 5EEE8D94
P 5800 2550
F 0 "J7" H 5828 2526 50  0000 L CNN
F 1 "12V" H 5828 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5800 2550 50  0001 C CNN
F 3 "~" H 5800 2550 50  0001 C CNN
	1    5800 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J39
U 1 1 5EEE8D9E
P 10250 5200
F 0 "J39" H 10278 5176 50  0000 L CNN
F 1 "5V" H 10278 5085 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10250 5200 50  0001 C CNN
F 3 "~" H 10250 5200 50  0001 C CNN
	1    10250 5200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 5EEE8DA8
P 3950 4000
F 0 "J5" H 3978 3976 50  0000 L CNN
F 1 "MULTI" H 3978 3885 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 3950 4000 50  0001 C CNN
F 3 "~" H 3950 4000 50  0001 C CNN
	1    3950 4000
	1    0    0    -1  
$EndComp
Text Label 3750 3900 2    50   ~ 0
Green12V+
Text Label 3750 4000 2    50   ~ 0
GreenGND
Text Label 3750 4100 2    50   ~ 0
White+12V
Text Label 3750 4200 2    50   ~ 0
WhiteGND
Text Label 10050 5200 2    50   ~ 0
+5V
Text Label 10050 5300 2    50   ~ 0
5VGND
Text Label 5600 2550 2    50   ~ 0
+12V
Text Label 5600 2650 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J16
U 1 1 5EEEAEEE
P 7250 2250
F 0 "J16" H 7278 2226 50  0000 L CNN
F 1 "12V" H 7278 2135 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7250 2250 50  0001 C CNN
F 3 "~" H 7250 2250 50  0001 C CNN
	1    7250 2250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J26
U 1 1 5EEEAEF8
P 8100 4850
F 0 "J26" H 8128 4826 50  0000 L CNN
F 1 "5V" H 8128 4735 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8100 4850 50  0001 C CNN
F 3 "~" H 8100 4850 50  0001 C CNN
	1    8100 4850
	1    0    0    -1  
$EndComp
Text Label 7900 4850 2    50   ~ 0
+5V
Text Label 7900 4950 2    50   ~ 0
5VGND
Text Label 7050 2250 2    50   ~ 0
+12V
Text Label 7050 2350 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J8
U 1 1 5EEEAF06
P 5800 3150
F 0 "J8" H 5828 3126 50  0000 L CNN
F 1 "GREEN" H 5828 3035 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5800 3150 50  0001 C CNN
F 3 "~" H 5800 3150 50  0001 C CNN
	1    5800 3150
	1    0    0    -1  
$EndComp
Text Label 5600 3150 2    50   ~ 0
Green12V+
Text Label 5600 3250 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J24
U 1 1 5EEEEE17
P 8000 2550
F 0 "J24" H 8028 2526 50  0000 L CNN
F 1 "12V" H 8028 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 2550 50  0001 C CNN
F 3 "~" H 8000 2550 50  0001 C CNN
	1    8000 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J31
U 1 1 5EEEEE21
P 8800 4900
F 0 "J31" H 8828 4876 50  0000 L CNN
F 1 "5V" H 8828 4785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8800 4900 50  0001 C CNN
F 3 "~" H 8800 4900 50  0001 C CNN
	1    8800 4900
	1    0    0    -1  
$EndComp
Text Label 8600 4900 2    50   ~ 0
+5V
Text Label 8600 5000 2    50   ~ 0
5VGND
Text Label 7800 2550 2    50   ~ 0
+12V
Text Label 7800 2650 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J13
U 1 1 5EEEEE2F
P 6650 4050
F 0 "J13" H 6678 4026 50  0000 L CNN
F 1 "WHITE" H 6678 3935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6650 4050 50  0001 C CNN
F 3 "~" H 6650 4050 50  0001 C CNN
	1    6650 4050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J23
U 1 1 5EEEEE3B
P 8000 2300
F 0 "J23" H 8028 2276 50  0000 L CNN
F 1 "12V" H 8028 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8000 2300 50  0001 C CNN
F 3 "~" H 8000 2300 50  0001 C CNN
	1    8000 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J30
U 1 1 5EEEEE45
P 8800 4600
F 0 "J30" H 8828 4576 50  0000 L CNN
F 1 "5V" H 8828 4485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8800 4600 50  0001 C CNN
F 3 "~" H 8800 4600 50  0001 C CNN
	1    8800 4600
	1    0    0    -1  
$EndComp
Text Label 8600 4600 2    50   ~ 0
+5V
Text Label 8600 4700 2    50   ~ 0
5VGND
Text Label 7800 2300 2    50   ~ 0
+12V
Text Label 7800 2400 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J14
U 1 1 5EEEEE53
P 6700 3150
F 0 "J14" H 6728 3126 50  0000 L CNN
F 1 "GREEN" H 6728 3035 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6700 3150 50  0001 C CNN
F 3 "~" H 6700 3150 50  0001 C CNN
	1    6700 3150
	1    0    0    -1  
$EndComp
Text Label 6500 3150 2    50   ~ 0
Green12V+
Text Label 6500 3250 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J29
U 1 1 5EEF4671
P 8750 2550
F 0 "J29" H 8778 2526 50  0000 L CNN
F 1 "12V" H 8778 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8750 2550 50  0001 C CNN
F 3 "~" H 8750 2550 50  0001 C CNN
	1    8750 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J33
U 1 1 5EEF467B
P 9500 4900
F 0 "J33" H 9528 4876 50  0000 L CNN
F 1 "5V" H 9528 4785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 9500 4900 50  0001 C CNN
F 3 "~" H 9500 4900 50  0001 C CNN
	1    9500 4900
	1    0    0    -1  
$EndComp
Text Label 9300 4900 2    50   ~ 0
+5V
Text Label 9300 5000 2    50   ~ 0
5VGND
Text Label 8550 2550 2    50   ~ 0
+12V
Text Label 8550 2650 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J20
U 1 1 5EEF4689
P 7550 4050
F 0 "J20" H 7578 4026 50  0000 L CNN
F 1 "WHITE" H 7578 3935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7550 4050 50  0001 C CNN
F 3 "~" H 7550 4050 50  0001 C CNN
	1    7550 4050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J28
U 1 1 5EEF4695
P 8750 2300
F 0 "J28" H 8778 2276 50  0000 L CNN
F 1 "12V" H 8778 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8750 2300 50  0001 C CNN
F 3 "~" H 8750 2300 50  0001 C CNN
	1    8750 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J32
U 1 1 5EEF469F
P 9500 4600
F 0 "J32" H 9528 4576 50  0000 L CNN
F 1 "5V" H 9528 4485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 9500 4600 50  0001 C CNN
F 3 "~" H 9500 4600 50  0001 C CNN
	1    9500 4600
	1    0    0    -1  
$EndComp
Text Label 9300 4600 2    50   ~ 0
+5V
Text Label 9300 4700 2    50   ~ 0
5VGND
Text Label 8550 2300 2    50   ~ 0
+12V
Text Label 8550 2400 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J27
U 1 1 5EEF46AD
P 8450 4050
F 0 "J27" H 8478 4026 50  0000 L CNN
F 1 "WHITE" H 8478 3935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8450 4050 50  0001 C CNN
F 3 "~" H 8450 4050 50  0001 C CNN
	1    8450 4050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J35
U 1 1 5EEF46B9
P 9550 2550
F 0 "J35" H 9578 2526 50  0000 L CNN
F 1 "12V" H 9578 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 9550 2550 50  0001 C CNN
F 3 "~" H 9550 2550 50  0001 C CNN
	1    9550 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J38
U 1 1 5EEF46C3
P 10200 4900
F 0 "J38" H 10228 4876 50  0000 L CNN
F 1 "5V" H 10228 4785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10200 4900 50  0001 C CNN
F 3 "~" H 10200 4900 50  0001 C CNN
	1    10200 4900
	1    0    0    -1  
$EndComp
Text Label 10000 4900 2    50   ~ 0
+5V
Text Label 10000 5000 2    50   ~ 0
5VGND
Text Label 9350 2550 2    50   ~ 0
+12V
Text Label 9350 2650 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J22
U 1 1 5EEF46D1
P 7650 3450
F 0 "J22" H 7678 3426 50  0000 L CNN
F 1 "GREEN" H 7678 3335 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7650 3450 50  0001 C CNN
F 3 "~" H 7650 3450 50  0001 C CNN
	1    7650 3450
	1    0    0    -1  
$EndComp
Text Label 7450 3450 2    50   ~ 0
Green12V+
Text Label 7450 3550 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J34
U 1 1 5EEF46DD
P 9550 2300
F 0 "J34" H 9578 2276 50  0000 L CNN
F 1 "12V" H 9578 2185 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 9550 2300 50  0001 C CNN
F 3 "~" H 9550 2300 50  0001 C CNN
	1    9550 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J37
U 1 1 5EEF46E7
P 10200 4600
F 0 "J37" H 10228 4576 50  0000 L CNN
F 1 "5V" H 10228 4485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 10200 4600 50  0001 C CNN
F 3 "~" H 10200 4600 50  0001 C CNN
	1    10200 4600
	1    0    0    -1  
$EndComp
Text Label 10000 4600 2    50   ~ 0
+5V
Text Label 10000 4700 2    50   ~ 0
5VGND
Text Label 9350 2300 2    50   ~ 0
+12V
Text Label 9350 2400 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J21
U 1 1 5EEF46F5
P 7600 3200
F 0 "J21" H 7628 3176 50  0000 L CNN
F 1 "GREEN" H 7628 3085 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7600 3200 50  0001 C CNN
F 3 "~" H 7600 3200 50  0001 C CNN
	1    7600 3200
	1    0    0    -1  
$EndComp
Text Label 7400 3200 2    50   ~ 0
Green12V+
Text Label 7400 3300 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J12
U 1 1 5EF1B893
P 6500 2550
F 0 "J12" H 6528 2526 50  0000 L CNN
F 1 "12V" H 6528 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6500 2550 50  0001 C CNN
F 3 "~" H 6500 2550 50  0001 C CNN
	1    6500 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J18
U 1 1 5EF1B89D
P 7450 4600
F 0 "J18" H 7478 4576 50  0000 L CNN
F 1 "5V" H 7478 4485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7450 4600 50  0001 C CNN
F 3 "~" H 7450 4600 50  0001 C CNN
	1    7450 4600
	1    0    0    -1  
$EndComp
Text Label 7250 4600 2    50   ~ 0
+5V
Text Label 7250 4700 2    50   ~ 0
5VGND
Text Label 6300 2550 2    50   ~ 0
+12V
Text Label 6300 2650 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J15
U 1 1 5EF1B8AB
P 6700 3400
F 0 "J15" H 6728 3376 50  0000 L CNN
F 1 "GREEN" H 6728 3285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6700 3400 50  0001 C CNN
F 3 "~" H 6700 3400 50  0001 C CNN
	1    6700 3400
	1    0    0    -1  
$EndComp
Text Label 6500 3400 2    50   ~ 0
Green12V+
Text Label 6500 3500 2    50   ~ 0
GreenGND
$Comp
L Connector:Conn_01x02_Female J11
U 1 1 5EF1B8B7
P 6450 2250
F 0 "J11" H 6478 2226 50  0000 L CNN
F 1 "12V" H 6478 2135 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 6450 2250 50  0001 C CNN
F 3 "~" H 6450 2250 50  0001 C CNN
	1    6450 2250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J36
U 1 1 5EF1B8C1
P 9550 5200
F 0 "J36" H 9578 5176 50  0000 L CNN
F 1 "5V" H 9578 5085 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 9550 5200 50  0001 C CNN
F 3 "~" H 9550 5200 50  0001 C CNN
	1    9550 5200
	1    0    0    -1  
$EndComp
Text Label 9350 5200 2    50   ~ 0
+5V
Text Label 9350 5300 2    50   ~ 0
5VGND
Text Label 6250 2250 2    50   ~ 0
+12V
Text Label 6250 2350 2    50   ~ 0
12VGND
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 5EF1B8CF
P 5800 3400
F 0 "J9" H 5828 3376 50  0000 L CNN
F 1 "GREEN" H 5828 3285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 5800 3400 50  0001 C CNN
F 3 "~" H 5800 3400 50  0001 C CNN
	1    5800 3400
	1    0    0    -1  
$EndComp
Text Label 5600 3400 2    50   ~ 0
Green12V+
Text Label 5600 3500 2    50   ~ 0
GreenGND
$Comp
L Device:R R1
U 1 1 5EF187D2
P 3000 2500
F 0 "R1" H 3070 2546 50  0000 L CNN
F 1 "1K" H 3070 2455 50  0000 L CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2930 2500 50  0001 C CNN
F 3 "~" H 3000 2500 50  0001 C CNN
	1    3000 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5EF190EC
P 3600 2500
F 0 "R2" H 3670 2546 50  0000 L CNN
F 1 "330" H 3670 2455 50  0000 L CNN
F 2 "PT_Library_v001:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3530 2500 50  0001 C CNN
F 3 "~" H 3600 2500 50  0001 C CNN
	1    3600 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5EF19DD9
P 3000 2800
F 0 "D1" V 3039 2683 50  0000 R CNN
F 1 "LED" V 2948 2683 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 3000 2800 50  0001 C CNN
F 3 "~" H 3000 2800 50  0001 C CNN
	1    3000 2800
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 5EF1C533
P 3600 2800
F 0 "D2" V 3639 2683 50  0000 R CNN
F 1 "LED" V 3548 2683 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 3600 2800 50  0001 C CNN
F 3 "~" H 3600 2800 50  0001 C CNN
	1    3600 2800
	0    -1   -1   0   
$EndComp
Text Label 3000 2950 3    50   ~ 0
12VGND
Text Label 3000 2350 0    50   ~ 0
+12V
Text Label 3600 2350 0    50   ~ 0
+5V
Text Label 3600 2950 3    50   ~ 0
5VGND
Text Label 7350 4050 2    50   ~ 0
White+12V
Text Label 7350 4150 2    50   ~ 0
WhiteGND
Text Label 8250 4050 2    50   ~ 0
White+12V
Text Label 8250 4150 2    50   ~ 0
WhiteGND
Text Label 6450 4050 2    50   ~ 0
White+12V
Text Label 6450 4150 2    50   ~ 0
WhiteGND
Text Label 5600 4050 2    50   ~ 0
White+12V
Text Label 5600 4150 2    50   ~ 0
WhiteGND
$EndSCHEMATC
