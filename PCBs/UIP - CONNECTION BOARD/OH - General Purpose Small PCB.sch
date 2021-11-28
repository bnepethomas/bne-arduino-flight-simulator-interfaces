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
L Connector:Conn_01x02_Female J24
U 1 1 61A52F51
P 8350 2100
F 0 "J24" H 8378 2076 50  0000 L CNN
F 1 "L DDI BL OUT" H 8378 1985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8350 2100 50  0001 C CNN
F 3 "~" H 8350 2100 50  0001 C CNN
	1    8350 2100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J22
U 1 1 61A52F57
P 7700 2100
F 0 "J22" H 8000 1950 50  0000 C CNN
F 1 "L DDI BL IN" H 8000 2050 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7700 2100 50  0001 C CNN
F 3 "~" H 7700 2100 50  0001 C CNN
	1    7700 2100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8150 2100 7900 2100
Wire Wire Line
	8150 2200 7900 2200
Wire Wire Line
	8150 2750 7900 2750
Wire Wire Line
	8150 2650 7900 2650
Wire Wire Line
	8150 2550 7900 2550
Wire Wire Line
	8150 2450 7900 2450
$Comp
L Connector:Conn_01x04_Female J11
U 1 1 604F03C2
P 7700 2550
F 0 "J11" H 8000 2450 50  0000 C CNN
F 1 "L DDI ANLOG IN" H 8050 2550 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7700 2550 50  0001 C CNN
F 3 "~" H 7700 2550 50  0001 C CNN
	1    7700 2550
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J15
U 1 1 604EF775
P 8350 2550
F 0 "J15" H 8378 2526 50  0000 L CNN
F 1 "L DDI ANLOG OUT" H 8378 2435 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8350 2550 50  0001 C CNN
F 3 "~" H 8350 2550 50  0001 C CNN
	1    8350 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 2500 1800 2500
Wire Wire Line
	1900 2400 1750 2400
$Comp
L Connector:Conn_01x02_Female J9
U 1 1 604E3762
P 1450 2400
F 0 "J9" H 1342 2585 50  0000 C CNN
F 1 "AMPCD BL IN" H 1342 2494 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 1450 2400 50  0001 C CNN
F 3 "~" H 1450 2400 50  0001 C CNN
	1    1450 2400
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J13
U 1 1 604E3006
P 2100 2400
F 0 "J13" H 2128 2376 50  0000 L CNN
F 1 "AMPCD BL OUT" H 2128 2285 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2100 2400 50  0001 C CNN
F 3 "~" H 2100 2400 50  0001 C CNN
	1    2100 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 1500 1600 1500
Wire Wire Line
	1900 1400 1600 1400
Wire Wire Line
	1900 1300 1600 1300
Wire Wire Line
	1900 1200 1600 1200
Wire Wire Line
	1900 1100 1600 1100
Wire Wire Line
	1900 1000 1600 1000
Wire Wire Line
	1900 900  1600 900 
Wire Wire Line
	1900 800  1600 800 
$Comp
L Connector:Conn_01x08_Female J17
U 1 1 61A3A34E
P 1400 1100
F 0 "J17" H 1292 1585 50  0000 C CNN
F 1 "AMPCD ROW IN" H 1292 1494 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 1400 1100 50  0001 C CNN
F 3 "~" H 1400 1100 50  0001 C CNN
	1    1400 1100
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Female J19
U 1 1 61A3A348
P 2100 1100
F 0 "J19" H 2128 1076 50  0000 L CNN
F 1 "AMPCD ROW OUT" H 2128 985 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x08_P2.54mm_Vertical" H 2100 1100 50  0001 C CNN
F 3 "~" H 2100 1100 50  0001 C CNN
	1    2100 1100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J25
U 1 1 61A61779
P 8350 950
F 0 "J25" H 8378 926 50  0000 L CNN
F 1 "L DDI ROW OUT" H 8378 835 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 8350 950 50  0001 C CNN
F 3 "~" H 8350 950 50  0001 C CNN
	1    8350 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 1250 8150 1250
Wire Wire Line
	7900 1150 8150 1150
Wire Wire Line
	7900 1050 8150 1050
Wire Wire Line
	7900 950  8150 950 
Wire Wire Line
	7900 850  8150 850 
Wire Wire Line
	7900 750  8150 750 
$Comp
L Connector:Conn_01x06_Female J32
U 1 1 61ACFD97
P 8350 1600
F 0 "J32" H 8378 1576 50  0000 L CNN
F 1 "L DDI COL OUT" H 8378 1485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 8350 1600 50  0001 C CNN
F 3 "~" H 8350 1600 50  0001 C CNN
	1    8350 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 1900 8150 1900
Wire Wire Line
	7900 1800 8150 1800
Wire Wire Line
	7900 1700 8150 1700
Wire Wire Line
	7900 1600 8150 1600
Wire Wire Line
	7900 1400 8150 1400
$Comp
L Connector:Conn_01x02_Female J35
U 1 1 61AF6DB6
P 8400 5050
F 0 "J35" H 8428 5026 50  0000 L CNN
F 1 "R DDI BL OUT" H 8428 4935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 8400 5050 50  0001 C CNN
F 3 "~" H 8400 5050 50  0001 C CNN
	1    8400 5050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J30
U 1 1 61AF6DBC
P 7750 5050
F 0 "J30" H 8050 5100 50  0000 C CNN
F 1 "R DDI BL IN" H 8050 5000 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 7750 5050 50  0001 C CNN
F 3 "~" H 7750 5050 50  0001 C CNN
	1    7750 5050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 5050 7950 5050
Wire Wire Line
	8200 5150 7950 5150
Wire Wire Line
	8200 5700 7950 5700
Wire Wire Line
	8200 5600 7950 5600
Wire Wire Line
	8200 5500 7950 5500
Wire Wire Line
	8200 5400 7950 5400
$Comp
L Connector:Conn_01x04_Female J31
U 1 1 61AF6DC8
P 7750 5500
F 0 "J31" H 8050 5550 50  0000 C CNN
F 1 "R DDI ANLOG IN" H 8100 5400 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 7750 5500 50  0001 C CNN
F 3 "~" H 7750 5500 50  0001 C CNN
	1    7750 5500
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J36
U 1 1 61AF6DCE
P 8400 5500
F 0 "J36" H 8428 5476 50  0000 L CNN
F 1 "R DDI ANLOG OUT" H 8428 5385 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 8400 5500 50  0001 C CNN
F 3 "~" H 8400 5500 50  0001 C CNN
	1    8400 5500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J26
U 1 1 61A671FB
P 7700 950
F 0 "J26" H 8000 1100 50  0000 C CNN
F 1 "L DDI ROW IN" H 8000 950 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7700 950 50  0001 C CNN
F 3 "~" H 7700 950 50  0001 C CNN
	1    7700 950 
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7900 1500 8150 1500
$Comp
L Connector:Conn_01x06_Female J21
U 1 1 61B70786
P 7700 1600
F 0 "J21" H 8000 1750 50  0000 C CNN
F 1 "L DDI ROW IN" H 8000 1600 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7700 1600 50  0001 C CNN
F 3 "~" H 7700 1600 50  0001 C CNN
	1    7700 1600
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Female J28
U 1 1 61B800A6
P 8400 3800
F 0 "J28" H 8428 3776 50  0000 L CNN
F 1 "R DDI ROW OUT" H 8428 3685 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 8400 3800 50  0001 C CNN
F 3 "~" H 8400 3800 50  0001 C CNN
	1    8400 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 4100 8200 4100
Wire Wire Line
	7950 4000 8200 4000
Wire Wire Line
	7950 3900 8200 3900
Wire Wire Line
	7950 3800 8200 3800
Wire Wire Line
	7950 3700 8200 3700
Wire Wire Line
	7950 3600 8200 3600
$Comp
L Connector:Conn_01x06_Female J29
U 1 1 61B800B2
P 8400 4450
F 0 "J29" H 8428 4426 50  0000 L CNN
F 1 "R DDI COL OUT" H 8428 4335 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 8400 4450 50  0001 C CNN
F 3 "~" H 8400 4450 50  0001 C CNN
	1    8400 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 4750 8200 4750
Wire Wire Line
	7950 4650 8200 4650
Wire Wire Line
	7950 4550 8200 4550
Wire Wire Line
	7950 4450 8200 4450
Wire Wire Line
	7950 4250 8200 4250
$Comp
L Connector:Conn_01x06_Female J23
U 1 1 61B800BD
P 7750 3800
F 0 "J23" H 8050 3950 50  0000 C CNN
F 1 "R DDI ROW IN" H 8050 3800 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7750 3800 50  0001 C CNN
F 3 "~" H 7750 3800 50  0001 C CNN
	1    7750 3800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7950 4350 8200 4350
$Comp
L Connector:Conn_01x06_Female J27
U 1 1 61B800C4
P 7750 4450
F 0 "J27" H 8050 4600 50  0000 C CNN
F 1 "R DDI ROW IN" H 8050 4450 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x06_P2.54mm_Vertical" H 7750 4450 50  0001 C CNN
F 3 "~" H 7750 4450 50  0001 C CNN
	1    7750 4450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1900 2100 1650 2100
Wire Wire Line
	1900 2000 1650 2000
Wire Wire Line
	1900 1900 1650 1900
Wire Wire Line
	1900 1800 1650 1800
$Comp
L Connector:Conn_01x04_Female J1
U 1 1 61BA6EA7
P 1450 1900
F 0 "J1" H 1342 2185 50  0000 C CNN
F 1 "AMPCD COL IN" H 1342 2094 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1450 1900 50  0001 C CNN
F 3 "~" H 1450 1900 50  0001 C CNN
	1    1450 1900
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J2
U 1 1 61BA6EAD
P 2100 1900
F 0 "J2" H 2128 1876 50  0000 L CNN
F 1 "AMPCD COL OUT" H 2128 1785 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 2100 1900 50  0001 C CNN
F 3 "~" H 2100 1900 50  0001 C CNN
	1    2100 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 61BAD2D0
P 2100 2650
F 0 "J3" H 2128 2626 50  0000 L CNN
F 1 "H/C BL OUT" H 2128 2535 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x02_P2.54mm_Vertical" H 2100 2650 50  0001 C CNN
F 3 "~" H 2100 2650 50  0001 C CNN
	1    2100 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 2400 1750 2650
Wire Wire Line
	1750 2650 1900 2650
Connection ~ 1750 2400
Wire Wire Line
	1750 2400 1650 2400
Wire Wire Line
	1800 2500 1800 2750
Wire Wire Line
	1800 2750 1900 2750
Connection ~ 1800 2500
Wire Wire Line
	1800 2500 1650 2500
Wire Wire Line
	1900 3250 1650 3250
Wire Wire Line
	1900 3150 1650 3150
Wire Wire Line
	1900 3050 1650 3050
Wire Wire Line
	1900 2950 1650 2950
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 61BC3DA8
P 1450 3050
F 0 "J5" H 1342 3335 50  0000 C CNN
F 1 "AMPCD CRS IN" H 1342 3244 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1450 3050 50  0001 C CNN
F 3 "~" H 1450 3050 50  0001 C CNN
	1    1450 3050
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J20
U 1 1 61BC3DAE
P 2100 3050
F 0 "J20" H 2128 3026 50  0000 L CNN
F 1 "AMPCD CRS OUT" H 2128 2935 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 2100 3050 50  0001 C CNN
F 3 "~" H 2100 3050 50  0001 C CNN
	1    2100 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3800 1650 3800
Wire Wire Line
	1900 3700 1650 3700
Wire Wire Line
	1900 3600 1650 3600
Wire Wire Line
	1900 3500 1650 3500
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 61BC7908
P 1450 3600
F 0 "J6" H 1342 3885 50  0000 C CNN
F 1 "AMPCD HDG IN" H 1342 3794 50  0000 C CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 1450 3600 50  0001 C CNN
F 3 "~" H 1450 3600 50  0001 C CNN
	1    1450 3600
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J33
U 1 1 61BC790E
P 2100 3600
F 0 "J33" H 2128 3576 50  0000 L CNN
F 1 "AMPCD HDG OUT" H 2128 3485 50  0000 L CNN
F 2 "PT_Library_v001:Molex_1x04_P2.54mm_Vertical" H 2100 3600 50  0001 C CNN
F 3 "~" H 2100 3600 50  0001 C CNN
	1    2100 3600
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 604D4B6F
P 1650 4100
F 0 "H1" H 1750 4146 50  0000 L CNN
F 1 "MountingHole" H 1750 4055 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1650 4100 50  0001 C CNN
F 3 "~" H 1650 4100 50  0001 C CNN
	1    1650 4100
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 604D50C8
P 1650 4300
F 0 "H2" H 1750 4346 50  0000 L CNN
F 1 "MountingHole" H 1750 4255 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1650 4300 50  0001 C CNN
F 3 "~" H 1650 4300 50  0001 C CNN
	1    1650 4300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 604D550A
P 1650 4500
F 0 "H3" H 1750 4546 50  0000 L CNN
F 1 "MountingHole" H 1750 4455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1650 4500 50  0001 C CNN
F 3 "~" H 1650 4500 50  0001 C CNN
	1    1650 4500
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 604D596A
P 1650 4700
F 0 "H4" H 1750 4746 50  0000 L CNN
F 1 "MountingHole" H 1750 4655 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1650 4700 50  0001 C CNN
F 3 "~" H 1650 4700 50  0001 C CNN
	1    1650 4700
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H9
U 1 1 604D923B
P 9500 1300
F 0 "H9" H 9600 1346 50  0000 L CNN
F 1 "MountingHole" H 9600 1255 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9500 1300 50  0001 C CNN
F 3 "~" H 9500 1300 50  0001 C CNN
	1    9500 1300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H10
U 1 1 604D9245
P 9500 1500
F 0 "H10" H 9600 1546 50  0000 L CNN
F 1 "MountingHole" H 9600 1455 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9500 1500 50  0001 C CNN
F 3 "~" H 9500 1500 50  0001 C CNN
	1    9500 1500
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H11
U 1 1 604D924F
P 9500 1700
F 0 "H11" H 9600 1746 50  0000 L CNN
F 1 "MountingHole" H 9600 1655 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9500 1700 50  0001 C CNN
F 3 "~" H 9500 1700 50  0001 C CNN
	1    9500 1700
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H12
U 1 1 604D9259
P 9500 1900
F 0 "H12" H 9600 1946 50  0000 L CNN
F 1 "MountingHole" H 9600 1855 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9500 1900 50  0001 C CNN
F 3 "~" H 9500 1900 50  0001 C CNN
	1    9500 1900
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H13
U 1 1 604D927F
P 9550 4550
F 0 "H13" H 9650 4596 50  0000 L CNN
F 1 "MountingHole" H 9650 4505 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9550 4550 50  0001 C CNN
F 3 "~" H 9550 4550 50  0001 C CNN
	1    9550 4550
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H14
U 1 1 604D9289
P 9550 4750
F 0 "H14" H 9650 4796 50  0000 L CNN
F 1 "MountingHole" H 9650 4705 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9550 4750 50  0001 C CNN
F 3 "~" H 9550 4750 50  0001 C CNN
	1    9550 4750
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H15
U 1 1 604D9293
P 9550 4950
F 0 "H15" H 9650 4996 50  0000 L CNN
F 1 "MountingHole" H 9650 4905 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9550 4950 50  0001 C CNN
F 3 "~" H 9550 4950 50  0001 C CNN
	1    9550 4950
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H16
U 1 1 604D929D
P 9550 5150
F 0 "H16" H 9650 5196 50  0000 L CNN
F 1 "MountingHole" H 9650 5105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 9550 5150 50  0001 C CNN
F 3 "~" H 9550 5150 50  0001 C CNN
	1    9550 5150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H5
U 1 1 61ACA3BE
P 4950 5000
F 0 "H5" H 5050 5046 50  0000 L CNN
F 1 "MountingHole" H 5050 4955 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4950 5000 50  0001 C CNN
F 3 "~" H 4950 5000 50  0001 C CNN
	1    4950 5000
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H6
U 1 1 61ACA3C8
P 4950 5200
F 0 "H6" H 5050 5246 50  0000 L CNN
F 1 "MountingHole" H 5050 5155 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4950 5200 50  0001 C CNN
F 3 "~" H 4950 5200 50  0001 C CNN
	1    4950 5200
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H7
U 1 1 61ACA3D2
P 4950 5400
F 0 "H7" H 5050 5446 50  0000 L CNN
F 1 "MountingHole" H 5050 5355 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4950 5400 50  0001 C CNN
F 3 "~" H 4950 5400 50  0001 C CNN
	1    4950 5400
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H8
U 1 1 61ACA3DC
P 4950 5600
F 0 "H8" H 5050 5646 50  0000 L CNN
F 1 "MountingHole" H 5050 5555 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 4950 5600 50  0001 C CNN
F 3 "~" H 4950 5600 50  0001 C CNN
	1    4950 5600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
