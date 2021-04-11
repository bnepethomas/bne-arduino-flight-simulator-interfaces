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
Text Notes 3250 2350 0    50   ~ 0
Right Console\n\n2003\nHook Led - include Resistor\nHook Led 2 - include Resistor \n\nPitot Heat- Mag Switch\nBleed Air Pull - Solenoid\nLaser Arm - Mag Switch\nCanopy Switch - Mag Switch\n\nHyd Press 1 - Servo\nHyd Press 2 - Servo\n\nBattery Left - Servo\nBattery Right - Servo\n
Text Notes 1100 2350 0    50   ~ 0
Left Console\n\nAPU - LED - Include Resistor\nLanding Gear Handle - LED - Include Resistor\n\nLanding Gear - Solenoid\n\n\nLaunch Bar - Mag Switch\nHook Bypass - Mag Switch\n\nAPU - Mag Switch\nEngine Crank - Mag Switch\n\nFuel Dump - Mag Switch\n\n\nTrim - Servo\n\nBrake Pressure - Stepper\n
$Comp
L Transistor_Array:ULN2003A U?
U 1 1 6073B19B
P 4050 4100
F 0 "U?" H 4050 4767 50  0000 C CNN
F 1 "ULN2003A" H 4050 4676 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 4100 3550 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 4150 3900 50  0001 C CNN
	1    4050 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 6073DAF2
P 5350 2300
F 0 "J?" H 5242 2485 50  0000 C CNN
F 1 "Conn_01x02_Female" H 5242 2394 50  0000 C CNN
F 2 "" H 5350 2300 50  0001 C CNN
F 3 "~" H 5350 2300 50  0001 C CNN
	1    5350 2300
	-1   0    0    -1  
$EndComp
Text Label 5550 2300 0    50   ~ 0
+12V
Text Label 5550 2400 0    50   ~ 0
GND
Text Label 4450 3700 0    50   ~ 0
+12V
Text Label 4500 5100 0    50   ~ 0
+12V
Text Label 4050 4700 0    50   ~ 0
GND
Text Label 4100 6100 0    50   ~ 0
GND
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 60740F05
P 8000 1700
F 0 "J?" H 8028 1676 50  0000 L CNN
F 1 "Landing Gear Sol" H 8028 1585 50  0000 L CNN
F 2 "" H 8000 1700 50  0001 C CNN
F 3 "~" H 8000 1700 50  0001 C CNN
	1    8000 1700
	1    0    0    -1  
$EndComp
Text Label 2700 4800 0    50   ~ 0
GND
Wire Wire Line
	2700 3900 3650 3900
Wire Wire Line
	2700 4000 3650 4000
Wire Wire Line
	2700 4100 3650 4100
Wire Wire Line
	2700 4200 3650 4200
Wire Wire Line
	2700 4300 3650 4300
Wire Wire Line
	3650 4400 2700 4400
Wire Wire Line
	3650 4500 2700 4500
$Comp
L Transistor_Array:ULN2003A U?
U 1 1 6073C2C9
P 4100 5500
F 0 "U?" H 4100 6167 50  0000 C CNN
F 1 "ULN2003A" H 4100 6076 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 4150 4950 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 4200 5300 50  0001 C CNN
	1    4100 5500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x10_Female J?
U 1 1 6074D9D3
P 2500 4300
F 0 "J?" H 2392 4885 50  0000 C CNN
F 1 "Conn_01x10_Female" H 2392 4794 50  0000 C CNN
F 2 "" H 2500 4300 50  0001 C CNN
F 3 "~" H 2500 4300 50  0001 C CNN
	1    2500 4300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2700 4600 3150 4600
Wire Wire Line
	3150 4600 3150 5300
Wire Wire Line
	3150 5300 3700 5300
Wire Wire Line
	2700 4700 3050 4700
Wire Wire Line
	3050 4700 3050 5400
Wire Wire Line
	3050 5400 3700 5400
Text Label 2850 3900 0    50   ~ 0
In_Landing_Gear_Sol
Text Label 4450 3900 0    50   ~ 0
Out_Landing_Gear_Sol
Text Label 7800 1800 2    50   ~ 0
Out_Landing_Gear_Sol
Text Label 7800 1700 2    50   ~ 0
+12V
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 607556D0
P 8000 2100
F 0 "J?" H 8028 2076 50  0000 L CNN
F 1 "Landing Gear Handle" H 8028 1985 50  0000 L CNN
F 2 "" H 8000 2100 50  0001 C CNN
F 3 "~" H 8000 2100 50  0001 C CNN
	1    8000 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R?
U 1 1 60756564
P 7350 2100
F 0 "R?" V 7555 2100 50  0000 C CNN
F 1 "560" V 7464 2100 50  0000 C CNN
F 2 "" V 7390 2090 50  0001 C CNN
F 3 "~" H 7350 2100 50  0001 C CNN
	1    7350 2100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7800 2100 7500 2100
Text Label 7200 2100 2    50   ~ 0
+12V
Text Label 7800 2200 2    50   ~ 0
Out_Landing_Gear_LED
Text Label 4500 5300 0    50   ~ 0
Out_Landing_Gear_LED
Text Label 3150 4800 0    50   ~ 0
In_Landing_Gear_LED
Text Label 3100 5400 0    50   ~ 0
In_APU_Led
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 6075AC33
P 8300 5400
F 0 "J?" H 8328 5376 50  0000 L CNN
F 1 "APU_LED" H 8328 5285 50  0000 L CNN
F 2 "" H 8300 5400 50  0001 C CNN
F 3 "~" H 8300 5400 50  0001 C CNN
	1    8300 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R?
U 1 1 6075AC3D
P 7650 5400
F 0 "R?" V 7855 5400 50  0000 C CNN
F 1 "560" V 7764 5400 50  0000 C CNN
F 2 "" V 7690 5390 50  0001 C CNN
F 3 "~" H 7650 5400 50  0001 C CNN
	1    7650 5400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8100 5400 7800 5400
Text Label 7500 5400 2    50   ~ 0
+12V
Text Label 8100 5500 2    50   ~ 0
Out_APU_LED
Text Label 4500 5400 0    50   ~ 0
Out_APU_LED
Text Notes 7350 5050 0    50   ~ 0
LED Values\n560 to 12V (to be validated!)
Text Label 2850 4000 0    50   ~ 0
In_Launch_Bar_Mag
Text Label 4450 4000 0    50   ~ 0
Out_Launch_Bar_Mag
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 6075F258
P 8000 2900
F 0 "J?" H 8028 2876 50  0000 L CNN
F 1 "Select_Jet" H 8028 2785 50  0000 L CNN
F 2 "" H 8000 2900 50  0001 C CNN
F 3 "~" H 8000 2900 50  0001 C CNN
	1    8000 2900
	1    0    0    -1  
$EndComp
Text Label 7800 2800 2    50   ~ 0
+12V
Text Label 7800 3000 2    50   ~ 0
+12V
Text Label 7800 2900 2    50   ~ 0
Out_Launch_Bar_Mag
Text Label 4450 4100 0    50   ~ 0
Out_Hook_Bypass
Text Label 7800 3100 2    50   ~ 0
Out_Hook_Bypass
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 60762D4E
P 7200 5900
F 0 "J?" H 7228 5876 50  0000 L CNN
F 1 "Braker_Pressure_Out" H 7228 5785 50  0000 L CNN
F 2 "" H 7200 5900 50  0001 C CNN
F 3 "~" H 7200 5900 50  0001 C CNN
	1    7200 5900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J?
U 1 1 60765586
P 6600 5900
F 0 "J?" H 6492 6185 50  0000 C CNN
F 1 "Braker_Pressure_Out" H 6492 6094 50  0000 C CNN
F 2 "" H 6600 5900 50  0001 C CNN
F 3 "~" H 6600 5900 50  0001 C CNN
	1    6600 5900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7000 5800 6800 5800
Wire Wire Line
	7000 5900 6800 5900
Wire Wire Line
	7000 6000 6800 6000
Wire Wire Line
	7000 6100 6800 6100
$EndSCHEMATC
