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
L MCU_RaspberryPi_and_Boards:Pico U7
U 1 1 60EC66CE
P 8700 2750
F 0 "U7" H 8700 3965 50  0000 C CNN
F 1 "Pico" H 8700 3874 50  0000 C CNN
F 2 "MCU_RaspberryPi_and_Boards:RPi_Pico_SMD_TH" V 8700 2750 50  0001 C CNN
F 3 "" H 8700 2750 50  0001 C CNN
	1    8700 2750
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS280 U1
U 1 1 60EC8B5C
P 5150 1600
F 0 "U1" H 4900 2300 50  0000 C CNN
F 1 "74LS280" H 4900 2200 50  0000 C CNN
F 2 "Package_SO:SO-14_3.9x8.65mm_P1.27mm" H 5150 1600 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS280" H 5150 1600 50  0001 C CNN
	1    5150 1600
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS244 U2
U 1 1 60EC987B
P 5150 3750
F 0 "U2" H 4850 4550 50  0000 C CNN
F 1 "74LS244" H 4850 4450 50  0000 C CNN
F 2 "Package_SO:SOIC-20W_7.5x12.8mm_P1.27mm" H 5150 3750 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74ls244.pdf" H 5150 3750 50  0001 C CNN
	1    5150 3750
	-1   0    0    -1  
$EndComp
$Comp
L 74xx:74LS244 U3
U 1 1 60ECAAC7
P 5200 5900
F 0 "U3" H 4900 6700 50  0000 C CNN
F 1 "74LVC244" H 4900 6600 50  0000 C CNN
F 2 "Package_SO:SOIC-20W_7.5x12.8mm_P1.27mm" H 5200 5900 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74ls244.pdf" H 5200 5900 50  0001 C CNN
	1    5200 5900
	1    0    0    -1  
$EndComp
$Comp
L 74xGxx:74LVC2G240 U4
U 2 1 60ECFB03
P 10550 6100
F 0 "U4" H 10300 5950 50  0000 C CNN
F 1 "74LVC2G240" H 10250 6300 50  0000 C CNN
F 2 "Package_SO:TSSOP-8_3x3mm_P0.65mm" H 10550 6100 50  0001 C CNN
F 3 "http://www.ti.com/lit/sg/scyt129e/scyt129e.pdf" H 10550 6100 50  0001 C CNN
	2    10550 6100
	-1   0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 60ED2254
P 8000 1000
F 0 "C3" H 8115 1046 50  0000 L CNN
F 1 "10uF" H 8100 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8038 850 50  0001 C CNN
F 3 "~" H 8000 1000 50  0001 C CNN
	1    8000 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 60ED293A
P 9350 1000
F 0 "C5" H 9465 1046 50  0000 L CNN
F 1 "10uF" H 9450 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 9388 850 50  0001 C CNN
F 3 "~" H 9350 1000 50  0001 C CNN
	1    9350 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 60ED30A9
P 9750 1000
F 0 "C6" H 9865 1046 50  0000 L CNN
F 1 "100nF" H 9850 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 9788 850 50  0001 C CNN
F 3 "~" H 9750 1000 50  0001 C CNN
	1    9750 1000
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x13_Top_Bottom J1
U 1 1 60ED6C71
P 1400 1400
F 0 "J1" H 1450 2217 50  0000 C CNN
F 1 "Parallel" H 1450 2126 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x13_P2.54mm_Vertical" H 1400 1400 50  0001 C CNN
F 3 "~" H 1400 1400 50  0001 C CNN
	1    1400 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR029
U 1 1 60EDC12D
P 9450 750
F 0 "#PWR029" H 9450 600 50  0001 C CNN
F 1 "+3V3" H 9465 923 50  0000 C CNN
F 2 "" H 9450 750 50  0001 C CNN
F 3 "" H 9450 750 50  0001 C CNN
	1    9450 750 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 60EDDEBD
P 8950 1300
F 0 "#PWR028" H 8950 1050 50  0001 C CNN
F 1 "GND" H 8955 1127 50  0000 C CNN
F 2 "" H 8950 1300 50  0001 C CNN
F 3 "" H 8950 1300 50  0001 C CNN
	1    8950 1300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR033
U 1 1 60EE23E4
P 10000 3800
F 0 "#PWR033" H 10000 3550 50  0001 C CNN
F 1 "GND" H 10005 3627 50  0000 C CNN
F 2 "" H 10000 3800 50  0001 C CNN
F 3 "" H 10000 3800 50  0001 C CNN
	1    10000 3800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR024
U 1 1 60EE3E7D
P 7750 3750
F 0 "#PWR024" H 7750 3500 50  0001 C CNN
F 1 "GND" H 7755 3577 50  0000 C CNN
F 2 "" H 7750 3750 50  0001 C CNN
F 3 "" H 7750 3750 50  0001 C CNN
	1    7750 3750
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR026
U 1 1 60EE9BE3
P 8700 4500
F 0 "#PWR026" H 8700 4350 50  0001 C CNN
F 1 "+3V3" H 8715 4673 50  0000 C CNN
F 2 "" H 8700 4500 50  0001 C CNN
F 3 "" H 8700 4500 50  0001 C CNN
	1    8700 4500
	1    0    0    -1  
$EndComp
Text Label 4150 3600 0    50   ~ 0
PD3
Text Label 4150 3700 0    50   ~ 0
PD4
$Comp
L power:GND #PWR014
U 1 1 60F114CF
P 5150 2300
F 0 "#PWR014" H 5150 2050 50  0001 C CNN
F 1 "GND" H 5155 2127 50  0000 C CNN
F 2 "" H 5150 2300 50  0001 C CNN
F 3 "" H 5150 2300 50  0001 C CNN
	1    5150 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 60F117C9
P 5150 4600
F 0 "#PWR016" H 5150 4350 50  0001 C CNN
F 1 "GND" H 5155 4427 50  0000 C CNN
F 2 "" H 5150 4600 50  0001 C CNN
F 3 "" H 5150 4600 50  0001 C CNN
	1    5150 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 60F11A56
P 5200 6800
F 0 "#PWR018" H 5200 6550 50  0001 C CNN
F 1 "GND" H 5205 6627 50  0000 C CNN
F 2 "" H 5200 6800 50  0001 C CNN
F 3 "" H 5200 6800 50  0001 C CNN
	1    5200 6800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 60F3AE49
P 3950 3300
F 0 "R6" V 3900 3150 50  0000 C CNN
F 1 "100" V 3950 3300 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3300 50  0001 C CNN
F 3 "~" H 3950 3300 50  0001 C CNN
	1    3950 3300
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 60F3C150
P 3950 3400
F 0 "R7" V 3900 3250 50  0000 C CNN
F 1 "100" V 3950 3400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3400 50  0001 C CNN
F 3 "~" H 3950 3400 50  0001 C CNN
	1    3950 3400
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 60F3C6F3
P 3950 3500
F 0 "R8" V 3900 3350 50  0000 C CNN
F 1 "100" V 3950 3500 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3500 50  0001 C CNN
F 3 "~" H 3950 3500 50  0001 C CNN
	1    3950 3500
	0    1    1    0   
$EndComp
$Comp
L Device:R R11
U 1 1 60F3C956
P 3950 3800
F 0 "R11" V 3900 3650 50  0000 C CNN
F 1 "100" V 3950 3800 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3800 50  0001 C CNN
F 3 "~" H 3950 3800 50  0001 C CNN
	1    3950 3800
	0    1    1    0   
$EndComp
$Comp
L Device:R R12
U 1 1 60F3CBC8
P 3950 3900
F 0 "R12" V 3900 3750 50  0000 C CNN
F 1 "100" V 3950 3900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3900 50  0001 C CNN
F 3 "~" H 3950 3900 50  0001 C CNN
	1    3950 3900
	0    1    1    0   
$EndComp
$Comp
L Device:R R13
U 1 1 60F3CD57
P 3950 4000
F 0 "R13" V 3900 3850 50  0000 C CNN
F 1 "100" V 3950 4000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 4000 50  0001 C CNN
F 3 "~" H 3950 4000 50  0001 C CNN
	1    3950 4000
	0    1    1    0   
$EndComp
Entry Wire Line
	4300 3600 4400 3500
Entry Wire Line
	4300 3700 4400 3600
Entry Wire Line
	4400 1300 4500 1200
Text Label 4500 1200 0    50   ~ 0
PD0
Entry Wire Line
	4400 1400 4500 1300
Entry Wire Line
	4400 1500 4500 1400
Entry Wire Line
	4400 1600 4500 1500
Entry Wire Line
	4400 1700 4500 1600
Entry Wire Line
	4400 1800 4500 1700
Entry Wire Line
	4400 1900 4500 1800
Entry Wire Line
	4400 2000 4500 1900
Text Label 4500 1300 0    50   ~ 0
PD1
Text Label 4500 1400 0    50   ~ 0
PD2
Text Label 4500 1500 0    50   ~ 0
PD3
Text Label 4500 1600 0    50   ~ 0
PD4
Text Label 4500 1700 0    50   ~ 0
PD5
Text Label 4500 1800 0    50   ~ 0
PD6
Text Label 4500 1900 0    50   ~ 0
PD7
$Comp
L power:GND #PWR012
U 1 1 60F763DC
P 4650 2000
F 0 "#PWR012" H 4650 1750 50  0001 C CNN
F 1 "GND" H 4655 1827 50  0000 C CNN
F 2 "" H 4650 2000 50  0001 C CNN
F 3 "" H 4650 2000 50  0001 C CNN
	1    4650 2000
	1    0    0    -1  
$EndComp
Entry Wire Line
	4400 3350 4500 3250
Entry Wire Line
	4400 3450 4500 3350
Entry Wire Line
	4400 3550 4500 3450
Entry Wire Line
	4400 3650 4500 3550
Entry Wire Line
	4400 3750 4500 3650
Entry Wire Line
	4400 3850 4500 3750
Entry Wire Line
	4400 3950 4500 3850
Entry Wire Line
	4400 4050 4500 3950
Text Label 4500 3250 0    50   ~ 0
PD4
Text Label 4500 3350 0    50   ~ 0
PD3
Text Label 4500 3450 0    50   ~ 0
PD2
Text Label 4500 3550 0    50   ~ 0
PD1
Text Label 4500 3650 0    50   ~ 0
PD0
Text Label 4500 3750 0    50   ~ 0
PD5
Text Label 4500 3850 0    50   ~ 0
PD6
Text Label 4500 3950 0    50   ~ 0
PD7
Entry Wire Line
	4400 5500 4500 5400
Entry Wire Line
	4400 5600 4500 5500
Entry Wire Line
	4400 5700 4500 5600
Entry Wire Line
	4400 5800 4500 5700
Entry Wire Line
	4400 5900 4500 5800
Entry Wire Line
	4400 6000 4500 5900
Entry Wire Line
	4400 6100 4500 6000
Entry Wire Line
	4400 6200 4500 6100
Text Label 4500 5400 0    50   ~ 0
PD4
Text Label 4500 5500 0    50   ~ 0
PD3
Text Label 4500 5600 0    50   ~ 0
PD2
Text Label 4500 5700 0    50   ~ 0
PD1
Text Label 4500 5800 0    50   ~ 0
PD0
Text Label 4500 5900 0    50   ~ 0
PD5
Text Label 4500 6000 0    50   ~ 0
PD6
Text Label 4500 6100 0    50   ~ 0
PD7
$Comp
L power:GND #PWR02
U 1 1 60FA2C14
P 1150 900
F 0 "#PWR02" H 1150 650 50  0001 C CNN
F 1 "GND" H 1155 727 50  0000 C CNN
F 2 "" H 1150 900 50  0001 C CNN
F 3 "" H 1150 900 50  0001 C CNN
	1    1150 900 
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 60FA889A
P 1200 1650
F 0 "#PWR03" H 1200 1400 50  0001 C CNN
F 1 "GND" H 1205 1477 50  0000 C CNN
F 2 "" H 1200 1650 50  0001 C CNN
F 3 "" H 1200 1650 50  0001 C CNN
	1    1200 1650
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 60FB41A5
P 1750 2050
F 0 "#PWR06" H 1750 1800 50  0001 C CNN
F 1 "GND" H 1755 1877 50  0000 C CNN
F 2 "" H 1750 2050 50  0001 C CNN
F 3 "" H 1750 2050 50  0001 C CNN
	1    1750 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 60FB9C6C
P 1700 1350
F 0 "#PWR05" H 1700 1100 50  0001 C CNN
F 1 "GND" H 1705 1177 50  0000 C CNN
F 2 "" H 1700 1350 50  0001 C CNN
F 3 "" H 1700 1350 50  0001 C CNN
	1    1700 1350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 60FC0D49
P 1700 800
F 0 "#PWR04" H 1700 550 50  0001 C CNN
F 1 "GND" H 1705 627 50  0000 C CNN
F 2 "" H 1700 800 50  0001 C CNN
F 3 "" H 1700 800 50  0001 C CNN
	1    1700 800 
	0    -1   -1   0   
$EndComp
Text Label 2000 900  0    50   ~ 0
~PSTRB
Text Label 2000 1000 0    50   ~ 0
~PBSY
Text Label 2000 1100 0    50   ~ 0
~PCMD
Text Label 2000 1200 0    50   ~ 0
~PPARITY
Text Label 2000 1500 0    50   ~ 0
~PRES
Text Label 700  1000 0    50   ~ 0
PR~W
Text Label 6800 1400 0    50   ~ 0
~PPARITY
$Comp
L power:GND #PWR019
U 1 1 60FF8BF4
P 6000 1150
F 0 "#PWR019" H 6000 900 50  0001 C CNN
F 1 "GND" H 6005 977 50  0000 C CNN
F 2 "" H 6000 1150 50  0001 C CNN
F 3 "" H 6000 1150 50  0001 C CNN
	1    6000 1150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR034
U 1 1 61003705
P 10550 5900
F 0 "#PWR034" H 10550 5650 50  0001 C CNN
F 1 "GND" H 10555 5727 50  0000 C CNN
F 2 "" H 10550 5900 50  0001 C CNN
F 3 "" H 10550 5900 50  0001 C CNN
	1    10550 5900
	-1   0    0    1   
$EndComp
Text Label 9650 6100 0    50   ~ 0
~PBSY
Text Label 6850 4150 0    50   ~ 0
PR~W
Wire Wire Line
	9450 800  9450 750 
Wire Wire Line
	8000 1150 8000 1250
Wire Wire Line
	8000 1250 8550 1250
Connection ~ 8950 1250
Wire Wire Line
	8950 1250 8950 1300
Wire Wire Line
	8950 1250 9350 1250
Wire Wire Line
	9350 1250 9350 1150
Wire Wire Line
	9350 1250 9750 1250
Wire Wire Line
	9750 1250 9750 1150
Connection ~ 9350 1250
Wire Wire Line
	10000 2000 10000 2500
Connection ~ 10000 2500
Wire Wire Line
	10000 2500 10000 3000
Wire Wire Line
	10000 3500 10000 3000
Connection ~ 10000 3000
Connection ~ 10000 3500
Wire Wire Line
	7750 3750 7750 3500
Wire Wire Line
	7750 3500 8000 3500
Wire Wire Line
	8000 3000 7750 3000
Wire Wire Line
	7750 3000 7750 3500
Connection ~ 7750 3500
Wire Wire Line
	8000 2500 7750 2500
Wire Wire Line
	7750 2500 7750 3000
Connection ~ 7750 3000
Wire Wire Line
	8000 2000 7750 2000
Wire Wire Line
	7750 2000 7750 2500
Connection ~ 7750 2500
Wire Wire Line
	9400 2200 9500 2200
Wire Wire Line
	10000 3500 10000 3800
Wire Wire Line
	8700 4700 8700 4500
Wire Wire Line
	5150 900  5150 800 
Wire Wire Line
	4100 3600 4300 3600
Wire Wire Line
	4300 3700 4100 3700
Wire Wire Line
	4500 1200 4650 1200
Wire Wire Line
	4500 1300 4650 1300
Wire Wire Line
	4500 1400 4650 1400
Wire Wire Line
	4500 1500 4650 1500
Wire Wire Line
	4500 1600 4650 1600
Wire Wire Line
	4500 1700 4650 1700
Wire Wire Line
	4500 1800 4650 1800
Wire Wire Line
	4500 1900 4650 1900
Wire Wire Line
	4500 3250 4650 3250
Wire Wire Line
	4500 3350 4650 3350
Wire Wire Line
	4500 3450 4650 3450
Wire Wire Line
	4500 3550 4650 3550
Wire Wire Line
	4500 3650 4650 3650
Wire Wire Line
	4500 3750 4650 3750
Wire Wire Line
	4500 3850 4650 3850
Wire Wire Line
	4500 3950 4650 3950
Wire Wire Line
	4500 5400 4700 5400
Wire Wire Line
	4500 5500 4700 5500
Wire Wire Line
	4500 5600 4700 5600
Wire Wire Line
	4500 5700 4700 5700
Wire Wire Line
	4500 5800 4700 5800
Wire Wire Line
	4500 5900 4700 5900
Wire Wire Line
	4500 6000 4700 6000
Wire Wire Line
	4500 6100 4700 6100
Wire Wire Line
	1150 900  1200 900 
Wire Wire Line
	1150 900  1150 1100
Wire Wire Line
	1150 1100 1200 1100
Connection ~ 1150 900 
Wire Wire Line
	1200 1600 1200 1650
Wire Wire Line
	1200 1650 1200 1700
Connection ~ 1200 1650
Wire Wire Line
	1700 1800 1750 1800
Wire Wire Line
	1750 1800 1750 2000
Wire Wire Line
	1700 2000 1750 2000
Connection ~ 1750 2000
Wire Wire Line
	1750 2000 1750 2050
Wire Wire Line
	1700 1400 1700 1350
Wire Wire Line
	1700 900  2000 900 
Wire Wire Line
	1700 1000 2000 1000
Wire Wire Line
	1700 1100 2000 1100
Connection ~ 1700 1350
Wire Wire Line
	1700 1350 1700 1300
Wire Wire Line
	1700 1200 2000 1200
Wire Wire Line
	1700 1500 2000 1500
Wire Wire Line
	1200 1000 700  1000
Wire Wire Line
	5650 1400 5700 1400
Wire Wire Line
	6000 1150 6000 1200
Wire Wire Line
	10300 6100 10250 6100
Wire Wire Line
	5150 4550 5150 4600
Wire Wire Line
	5650 4250 5650 4150
Wire Wire Line
	4700 6300 4700 6400
Wire Wire Line
	5200 6700 5200 6800
Wire Wire Line
	5200 6700 4700 6700
Wire Wire Line
	4700 6700 4700 6400
Connection ~ 5200 6700
Connection ~ 4700 6400
$Comp
L Device:R R9
U 1 1 60F3D522
P 3950 3600
F 0 "R9" V 3900 3450 50  0000 C CNN
F 1 "100" V 3950 3600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3600 50  0001 C CNN
F 3 "~" H 3950 3600 50  0001 C CNN
	1    3950 3600
	0    1    1    0   
$EndComp
$Comp
L Device:R R10
U 1 1 60F3CF7C
P 3950 3700
F 0 "R10" V 3900 3550 50  0000 C CNN
F 1 "100" V 3950 3700 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3880 3700 50  0001 C CNN
F 3 "~" H 3950 3700 50  0001 C CNN
	1    3950 3700
	0    1    1    0   
$EndComp
Entry Wire Line
	4300 3300 4400 3200
Wire Wire Line
	4100 3300 4300 3300
Entry Wire Line
	4300 3400 4400 3300
Wire Wire Line
	4100 3400 4300 3400
Entry Wire Line
	4300 3500 4400 3400
Wire Wire Line
	4100 3500 4300 3500
Entry Wire Line
	4300 3800 4400 3700
Entry Wire Line
	4300 3900 4400 3800
Entry Wire Line
	4300 4000 4400 3900
Wire Wire Line
	4100 3800 4300 3800
Wire Wire Line
	4100 3900 4300 3900
Wire Wire Line
	4100 4000 4300 4000
Text Label 4150 3300 0    50   ~ 0
PD0
Text Label 4150 3400 0    50   ~ 0
PD1
Text Label 4150 3500 0    50   ~ 0
PD2
Text Label 4150 3800 0    50   ~ 0
PD5
Text Label 4150 3900 0    50   ~ 0
PD6
Text Label 4150 4000 0    50   ~ 0
PD7
Entry Wire Line
	3450 3300 3550 3400
Entry Wire Line
	3450 3400 3550 3500
Entry Wire Line
	3450 3500 3550 3600
Entry Wire Line
	3450 3600 3550 3700
Entry Wire Line
	3450 3700 3550 3800
Entry Wire Line
	3450 3800 3550 3900
Entry Wire Line
	3450 3900 3550 4000
Wire Wire Line
	3550 3400 3800 3400
Wire Wire Line
	3550 3500 3800 3500
Wire Wire Line
	3550 3600 3800 3600
Wire Wire Line
	3550 3700 3800 3700
Wire Wire Line
	3550 3800 3800 3800
Wire Wire Line
	3550 3900 3800 3900
Wire Wire Line
	3550 4000 3800 4000
Wire Wire Line
	3550 3300 3800 3300
Entry Wire Line
	3450 3200 3550 3300
Text Label 3600 3300 0    50   ~ 0
D0
Text Label 3600 3400 0    50   ~ 0
D1
Text Label 3600 3500 0    50   ~ 0
D2
Text Label 3600 3600 0    50   ~ 0
D3
Text Label 3600 3700 0    50   ~ 0
D4
Text Label 3600 3800 0    50   ~ 0
D5
Text Label 3600 3900 0    50   ~ 0
D6
Text Label 3600 4000 0    50   ~ 0
D7
Text Label 3450 1350 0    50   ~ 0
ADATA
Text Label 700  1150 0    50   ~ 0
ADATA
Entry Wire Line
	700  1300 800  1200
Entry Wire Line
	700  1400 800  1300
Entry Wire Line
	700  1600 800  1500
Entry Wire Line
	700  1900 800  1800
Entry Wire Line
	700  2000 800  1900
Entry Wire Line
	700  2100 800  2000
Wire Wire Line
	1200 1200 800  1200
Wire Wire Line
	800  1300 1200 1300
Wire Wire Line
	800  1500 1200 1500
Wire Wire Line
	1200 1800 800  1800
Wire Wire Line
	800  1900 1200 1900
Wire Wire Line
	1200 2000 800  2000
Text Label 950  1200 0    50   ~ 0
D0
Text Label 950  1300 0    50   ~ 0
D1
Text Label 950  1500 0    50   ~ 0
D2
Text Label 950  1800 0    50   ~ 0
D5
Text Label 950  1900 0    50   ~ 0
D6
Text Label 950  2000 0    50   ~ 0
D7
Entry Wire Line
	3350 1600 3450 1500
Entry Wire Line
	3350 1700 3450 1600
Wire Wire Line
	1700 1600 3350 1600
Wire Wire Line
	1700 1700 3350 1700
Text Label 1800 1600 0    50   ~ 0
D3
Text Label 1800 1700 0    50   ~ 0
D4
$Comp
L Connector:DB25_Female J2
U 1 1 612044DA
P 1450 4000
F 0 "J2" H 1368 5492 50  0000 C CNN
F 1 "DB25_Female" H 1368 5401 50  0000 C CNN
F 2 "Connector_Dsub:DSUB-25_Female_Horizontal_P2.77x2.84mm_EdgePinOffset7.70mm_Housed_MountingHolesOffset9.12mm" H 1450 4000 50  0001 C CNN
F 3 " ~" H 1450 4000 50  0001 C CNN
	1    1450 4000
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 612139C7
P 1900 5400
F 0 "#PWR07" H 1900 5150 50  0001 C CNN
F 1 "GND" H 1905 5227 50  0000 C CNN
F 2 "" H 1900 5400 50  0001 C CNN
F 3 "" H 1900 5400 50  0001 C CNN
	1    1900 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 2900 1900 2900
Wire Wire Line
	1900 2900 1900 3000
Wire Wire Line
	1750 3000 1900 3000
Connection ~ 1900 3000
Wire Wire Line
	1900 3000 1900 3400
Wire Wire Line
	1750 3400 1900 3400
Connection ~ 1900 3400
Wire Wire Line
	1900 3400 1900 3900
Wire Wire Line
	1750 3900 1900 3900
Connection ~ 1900 3900
Wire Wire Line
	1900 3900 1900 4100
Wire Wire Line
	1750 4100 1900 4100
Connection ~ 1900 4100
Wire Wire Line
	1900 4100 1900 4400
Wire Wire Line
	1750 4900 1900 4900
Connection ~ 1900 4900
Wire Wire Line
	1900 4900 1900 5400
Wire Wire Line
	1750 4400 1900 4400
Connection ~ 1900 4400
Wire Wire Line
	1900 4400 1900 4600
Wire Wire Line
	1750 4600 1900 4600
Connection ~ 1900 4600
Wire Wire Line
	1900 4600 1900 4900
Wire Wire Line
	1750 3100 2050 3100
Wire Wire Line
	1750 3200 2050 3200
Wire Wire Line
	1750 3300 2050 3300
Wire Wire Line
	1750 3500 2050 3500
Wire Wire Line
	1750 3700 2050 3700
Wire Wire Line
	1750 4300 2050 4300
Entry Wire Line
	3350 3600 3450 3500
Entry Wire Line
	3350 3800 3450 3700
Entry Wire Line
	3350 4200 3450 4100
Entry Wire Line
	3350 4500 3450 4400
Entry Wire Line
	3350 4700 3450 4600
Entry Wire Line
	3350 4800 3450 4700
Entry Wire Line
	3350 5000 3450 4900
Entry Wire Line
	3350 5200 3450 5100
Wire Wire Line
	1750 3600 3350 3600
Wire Wire Line
	1750 3800 3350 3800
Wire Wire Line
	1750 4200 3350 4200
Wire Wire Line
	1750 4500 3350 4500
Wire Wire Line
	1750 4700 3350 4700
Wire Wire Line
	1750 4800 3350 4800
Wire Wire Line
	1750 5000 3350 5000
Wire Wire Line
	1750 5200 3350 5200
Wire Bus Line
	700  2350 3450 2350
Connection ~ 3450 2350
Wire Wire Line
	8400 4800 8300 4800
Text Label 7650 4800 0    50   ~ 0
~PCMD
Wire Wire Line
	8950 6250 9050 6250
Text Label 9050 6250 0    50   ~ 0
RUN
Text Label 7500 6250 0    50   ~ 0
~PRES
Text Label 9150 4800 0    50   ~ 0
CMD
Connection ~ 5650 4150
Wire Wire Line
	6450 4150 6500 4150
Wire Wire Line
	5650 4150 5900 4150
Text Label 5950 2850 0    50   ~ 0
DOUT
Entry Wire Line
	5800 3950 5900 3850
Entry Wire Line
	5800 3850 5900 3750
Entry Wire Line
	5800 3750 5900 3650
Entry Wire Line
	5800 3650 5900 3550
Entry Wire Line
	5800 3550 5900 3450
Entry Wire Line
	5800 3450 5900 3350
Entry Wire Line
	5800 3350 5900 3250
Wire Bus Line
	5900 2850 7200 2850
Entry Wire Line
	5800 3250 5900 3150
Wire Wire Line
	5650 3250 5800 3250
Wire Wire Line
	5650 3350 5800 3350
Wire Wire Line
	5650 3450 5800 3450
Wire Wire Line
	5650 3550 5800 3550
Wire Wire Line
	5650 3650 5800 3650
Wire Wire Line
	5650 3750 5800 3750
Wire Wire Line
	5650 3850 5800 3850
Wire Wire Line
	5650 3950 5800 3950
Text Label 5650 3250 0    50   ~ 0
OD4
Text Label 5650 3350 0    50   ~ 0
OD3
Text Label 5650 3450 0    50   ~ 0
OD2
Text Label 5650 3550 0    50   ~ 0
OD1
Text Label 5650 3650 0    50   ~ 0
OD0
Text Label 5650 3750 0    50   ~ 0
OD5
Text Label 5650 3850 0    50   ~ 0
OD6
Text Label 5650 3950 0    50   ~ 0
OD7
Entry Wire Line
	7200 1900 7300 1800
Entry Wire Line
	7200 2000 7300 1900
Entry Wire Line
	7200 2200 7300 2100
Entry Wire Line
	7200 2300 7300 2200
Entry Wire Line
	7200 2400 7300 2300
Entry Wire Line
	7200 2500 7300 2400
Entry Wire Line
	7200 2700 7300 2600
Entry Wire Line
	7200 2800 7300 2700
Wire Wire Line
	7300 1800 8000 1800
Wire Wire Line
	7300 1900 8000 1900
Wire Wire Line
	7300 2100 8000 2100
Wire Wire Line
	7300 2200 8000 2200
Wire Wire Line
	7300 2300 8000 2300
Wire Wire Line
	7300 2400 8000 2400
Wire Wire Line
	7300 2600 8000 2600
Wire Wire Line
	7300 2700 8000 2700
Text Label 7850 1800 0    50   ~ 0
OD0
Text Label 7850 1900 0    50   ~ 0
OD1
Text Label 7850 2100 0    50   ~ 0
OD2
Text Label 7850 2200 0    50   ~ 0
OD3
Text Label 7850 2300 0    50   ~ 0
OD4
Text Label 7850 2400 0    50   ~ 0
OD5
Text Label 7850 2600 0    50   ~ 0
OD6
Text Label 7850 2700 0    50   ~ 0
OD7
Wire Wire Line
	8000 2800 7800 2800
Wire Wire Line
	10850 6100 10900 6100
Text Label 7800 2800 0    50   ~ 0
BSY
Entry Wire Line
	5850 6100 5950 6000
Entry Wire Line
	5850 6000 5950 5900
Entry Wire Line
	5850 5900 5950 5800
Entry Wire Line
	5850 5800 5950 5700
Entry Wire Line
	5850 5700 5950 5600
Entry Wire Line
	5850 5600 5950 5500
Entry Wire Line
	5850 5500 5950 5400
Entry Wire Line
	5850 5400 5950 5300
Wire Wire Line
	5700 5400 5850 5400
Wire Wire Line
	5700 5500 5850 5500
Wire Wire Line
	5700 5600 5850 5600
Wire Wire Line
	5700 5700 5850 5700
Wire Wire Line
	5700 5800 5850 5800
Wire Wire Line
	5700 5900 5850 5900
Wire Wire Line
	5700 6000 5850 6000
Wire Wire Line
	5700 6100 5850 6100
Text Label 7650 5450 0    50   ~ 0
~PSTRB
Text Label 9150 5450 0    50   ~ 0
STRB
Text Label 5700 5400 0    50   ~ 0
ID4
Text Label 5700 5500 0    50   ~ 0
ID3
Wire Bus Line
	5950 5150 7200 5150
Text Label 5700 5600 0    50   ~ 0
ID2
Text Label 5700 5700 0    50   ~ 0
ID1
Text Label 5700 5800 0    50   ~ 0
ID0
Text Label 5700 5900 0    50   ~ 0
ID5
Text Label 5700 6000 0    50   ~ 0
ID6
Text Label 5700 6100 0    50   ~ 0
ID7
Wire Wire Line
	9400 2000 10000 2000
Wire Wire Line
	9400 2500 10000 2500
Wire Wire Line
	9400 3500 10000 3500
Wire Wire Line
	9400 2800 9500 2800
Text Label 9500 2800 0    50   ~ 0
RUN
Text Label 7800 2900 0    50   ~ 0
~R~W
Text Label 9500 2600 0    50   ~ 0
CMD
Text Label 9500 2400 0    50   ~ 0
STRB
$Comp
L power:GND #PWR01
U 1 1 61817E21
P 1050 7200
F 0 "#PWR01" H 1050 6950 50  0001 C CNN
F 1 "GND" H 1055 7027 50  0000 C CNN
F 2 "" H 1050 7200 50  0001 C CNN
F 3 "" H 1050 7200 50  0001 C CNN
	1    1050 7200
	1    0    0    -1  
$EndComp
Text Label 3000 6750 0    50   ~ 0
SD_MISO
$Comp
L power:+3.3V #PWR011
U 1 1 61847408
P 3750 5650
F 0 "#PWR011" H 3750 5500 50  0001 C CNN
F 1 "+3.3V" H 3765 5823 50  0000 C CNN
F 2 "" H 3750 5650 50  0001 C CNN
F 3 "" H 3750 5650 50  0001 C CNN
	1    3750 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 6450 3750 6450
Wire Wire Line
	3750 6450 3750 6350
Text Label 3000 6550 0    50   ~ 0
SD_SCK
Text Label 3000 6350 0    50   ~ 0
SD_MOSI
$Comp
L Device:R R2
U 1 1 61889C95
P 3600 6250
F 0 "R2" V 3550 6100 50  0000 C CNN
F 1 "4.7k" V 3600 6250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3530 6250 50  0001 C CNN
F 3 "~" H 3600 6250 50  0001 C CNN
	1    3600 6250
	0    1    1    0   
$EndComp
Connection ~ 3750 6250
Wire Wire Line
	3750 6250 3750 5650
Text Label 3000 6250 0    50   ~ 0
SD_CS
Wire Wire Line
	2750 6650 2900 6650
Wire Wire Line
	2900 6650 2900 7000
$Comp
L power:GND #PWR010
U 1 1 618BEE66
P 2900 7000
F 0 "#PWR010" H 2900 6750 50  0001 C CNN
F 1 "GND" H 2905 6827 50  0000 C CNN
F 2 "" H 2900 7000 50  0001 C CNN
F 3 "" H 2900 7000 50  0001 C CNN
	1    2900 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 2400 9500 2400
Wire Wire Line
	9400 2600 9500 2600
Wire Bus Line
	7200 4200 7200 5150
Wire Bus Line
	7200 4200 10250 4200
Entry Wire Line
	10150 2700 10250 2800
Wire Wire Line
	9400 3000 10000 3000
Entry Wire Line
	10150 3100 10250 3200
Entry Wire Line
	10150 3200 10250 3300
Entry Wire Line
	10150 3300 10250 3400
Entry Wire Line
	10150 3400 10250 3500
Entry Wire Line
	10150 3600 10250 3700
Entry Wire Line
	10150 3700 10250 3800
Wire Wire Line
	9400 3100 10150 3100
Wire Wire Line
	9400 3200 10150 3200
Wire Wire Line
	9400 3300 10150 3300
Wire Wire Line
	9400 3400 10150 3400
Wire Wire Line
	9400 3600 10150 3600
Wire Wire Line
	7800 2900 8000 2900
Entry Wire Line
	10150 2900 10250 3000
Wire Wire Line
	9400 2700 10150 2700
Wire Wire Line
	9400 2900 10150 2900
Wire Wire Line
	9400 3700 10150 3700
Text Label 9500 3700 0    50   ~ 0
ID0
Text Label 9500 3600 0    50   ~ 0
ID1
Text Label 9500 3400 0    50   ~ 0
ID2
Text Label 9500 3300 0    50   ~ 0
ID3
Text Label 9500 3200 0    50   ~ 0
ID4
Text Label 9500 3100 0    50   ~ 0
ID5
Text Label 9500 2900 0    50   ~ 0
ID6
Text Label 9500 2700 0    50   ~ 0
ID7
Wire Wire Line
	8000 3700 7250 3700
Text Label 7250 3700 0    50   ~ 0
SD_MOSI
Wire Wire Line
	8000 3600 7250 3600
Text Label 7250 3600 0    50   ~ 0
SD_SCK
Wire Wire Line
	8000 3400 7250 3400
Text Label 7250 3400 0    50   ~ 0
SD_CS
Wire Wire Line
	8000 3300 7250 3300
Text Label 7250 3300 0    50   ~ 0
SD_MISO
$Comp
L Connector:Conn_01x04_Male J4
U 1 1 61ADE97B
P 6550 3350
F 0 "J4" H 6658 3631 50  0000 C CNN
F 1 "I2C" H 6658 3540 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 6550 3350 50  0001 C CNN
F 3 "~" H 6550 3350 50  0001 C CNN
	1    6550 3350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 61ADF636
P 6800 3550
F 0 "#PWR022" H 6800 3300 50  0001 C CNN
F 1 "GND" H 6805 3377 50  0000 C CNN
F 2 "" H 6800 3550 50  0001 C CNN
F 3 "" H 6800 3550 50  0001 C CNN
	1    6800 3550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR021
U 1 1 61ADFD93
P 6800 3150
F 0 "#PWR021" H 6800 3000 50  0001 C CNN
F 1 "+3.3V" H 6815 3323 50  0000 C CNN
F 2 "" H 6800 3150 50  0001 C CNN
F 3 "" H 6800 3150 50  0001 C CNN
	1    6800 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 3250 6750 3250
Wire Wire Line
	6800 3250 6800 3150
Wire Wire Line
	6750 3550 6800 3550
Wire Wire Line
	8000 3100 7000 3100
Wire Wire Line
	7000 3100 7000 3350
Wire Wire Line
	7000 3350 6750 3350
Wire Wire Line
	6750 3450 7050 3450
Wire Wire Line
	7050 3450 7050 3200
Wire Wire Line
	7050 3200 8000 3200
Text Label 6800 3350 0    50   ~ 0
SDA
Text Label 6800 3450 0    50   ~ 0
SCL
$Comp
L Device:C C4
U 1 1 61B3ED89
P 8950 1000
F 0 "C4" H 9065 1046 50  0000 L CNN
F 1 "10uF" H 9050 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8988 850 50  0001 C CNN
F 3 "~" H 8950 1000 50  0001 C CNN
	1    8950 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 1150 8950 1250
$Comp
L Device:C C7
U 1 1 61B7A67C
P 10200 1000
F 0 "C7" H 10315 1046 50  0000 L CNN
F 1 "100nF" H 10300 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10238 850 50  0001 C CNN
F 3 "~" H 10200 1000 50  0001 C CNN
	1    10200 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 61B7A9FF
P 10650 1000
F 0 "C8" H 10765 1046 50  0000 L CNN
F 1 "100nF" H 10750 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10688 850 50  0001 C CNN
F 3 "~" H 10650 1000 50  0001 C CNN
	1    10650 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 1250 10200 1250
Wire Wire Line
	10650 1250 10650 1150
Connection ~ 9750 1250
Wire Wire Line
	10200 1150 10200 1250
Connection ~ 10200 1250
Wire Wire Line
	10200 1250 10650 1250
$Comp
L Device:C C2
U 1 1 61BB4F16
P 8550 1000
F 0 "C2" H 8665 1046 50  0000 L CNN
F 1 "100nF" H 8650 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8588 850 50  0001 C CNN
F 3 "~" H 8550 1000 50  0001 C CNN
	1    8550 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 61BB534A
P 7600 1000
F 0 "C1" H 7715 1046 50  0000 L CNN
F 1 "100nF" H 7650 900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7638 850 50  0001 C CNN
F 3 "~" H 7600 1000 50  0001 C CNN
	1    7600 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 1150 7600 1250
Connection ~ 8000 1250
Wire Wire Line
	7600 850  7600 800 
Wire Wire Line
	7600 800  8000 800 
Wire Wire Line
	8950 850  8950 800 
Wire Wire Line
	8950 800  9350 800 
Wire Wire Line
	9450 800  9750 800 
Wire Wire Line
	10650 800  10650 850 
Connection ~ 9450 800 
Wire Wire Line
	10200 850  10200 800 
Connection ~ 10200 800 
Wire Wire Line
	10200 800  10650 800 
Wire Wire Line
	9750 850  9750 800 
Connection ~ 9750 800 
Wire Wire Line
	9750 800  10200 800 
Wire Wire Line
	9350 850  9350 800 
Connection ~ 9350 800 
Wire Wire Line
	9350 800  9450 800 
$Comp
L Connector:Micro_SD_Card J3
U 1 1 61D6791C
P 1850 6450
F 0 "J3" H 1800 7167 50  0000 C CNN
F 1 "Micro_SD_Card" H 1800 7076 50  0000 C CNN
F 2 "Connector_Card:microSD_HC_Molex_104031-0811" H 3000 6750 50  0001 C CNN
F 3 "http://katalog.we-online.de/em/datasheet/693072010801.pdf" H 1850 6450 50  0001 C CNN
	1    1850 6450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1050 7050 1050 7200
Wire Wire Line
	2750 6750 3450 6750
$Comp
L Device:R R3
U 1 1 61E22D42
P 3600 6350
F 0 "R3" V 3550 6200 50  0000 C CNN
F 1 "4.7k" V 3600 6350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3530 6350 50  0001 C CNN
F 3 "~" H 3600 6350 50  0001 C CNN
	1    3600 6350
	0    1    1    0   
$EndComp
Connection ~ 3750 6350
Wire Wire Line
	3750 6350 3750 6250
Wire Wire Line
	2750 6250 3450 6250
Wire Wire Line
	2750 6350 3450 6350
$Comp
L Device:R R4
U 1 1 61E3729F
P 3600 6550
F 0 "R4" V 3550 6400 50  0000 C CNN
F 1 "4.7k" V 3600 6550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3530 6550 50  0001 C CNN
F 3 "~" H 3600 6550 50  0001 C CNN
	1    3600 6550
	0    1    1    0   
$EndComp
Wire Wire Line
	3750 6550 3750 6450
Connection ~ 3750 6450
$Comp
L Device:R R5
U 1 1 61E5F3C7
P 3600 6750
F 0 "R5" V 3550 6600 50  0000 C CNN
F 1 "4.7k" V 3600 6750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3530 6750 50  0001 C CNN
F 3 "~" H 3600 6750 50  0001 C CNN
	1    3600 6750
	0    1    1    0   
$EndComp
Wire Wire Line
	3750 6750 3750 6550
Connection ~ 3750 6550
Wire Wire Line
	2750 6550 3450 6550
Wire Wire Line
	8000 800  8000 850 
$Comp
L power:+3V3 #PWR017
U 1 1 60EEDF4C
P 5200 5050
F 0 "#PWR017" H 5200 4900 50  0001 C CNN
F 1 "+3V3" H 5215 5223 50  0000 C CNN
F 2 "" H 5200 5050 50  0001 C CNN
F 3 "" H 5200 5050 50  0001 C CNN
	1    5200 5050
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR030
U 1 1 60EE8B1F
P 9500 2200
F 0 "#PWR030" H 9500 2050 50  0001 C CNN
F 1 "+3V3" H 9515 2373 50  0000 C CNN
F 2 "" H 9500 2200 50  0001 C CNN
F 3 "" H 9500 2200 50  0001 C CNN
	1    9500 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2850 5150 2950
Wire Wire Line
	8000 700  8000 800 
$Comp
L power:+5VL #PWR025
U 1 1 6213EE5C
P 8000 700
F 0 "#PWR025" H 8000 550 50  0001 C CNN
F 1 "+5VL" H 8015 873 50  0000 C CNN
F 2 "" H 8000 700 50  0001 C CNN
F 3 "" H 8000 700 50  0001 C CNN
	1    8000 700 
	1    0    0    -1  
$EndComp
$Comp
L power:+5VL #PWR013
U 1 1 62153AC0
P 5150 800
F 0 "#PWR013" H 5150 650 50  0001 C CNN
F 1 "+5VL" H 5165 973 50  0000 C CNN
F 2 "" H 5150 800 50  0001 C CNN
F 3 "" H 5150 800 50  0001 C CNN
	1    5150 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 5050 5200 5100
$Comp
L power:+5VL #PWR015
U 1 1 621A2F44
P 5150 2850
F 0 "#PWR015" H 5150 2700 50  0001 C CNN
F 1 "+5VL" H 5165 3023 50  0000 C CNN
F 2 "" H 5150 2850 50  0001 C CNN
F 3 "" H 5150 2850 50  0001 C CNN
	1    5150 2850
	1    0    0    -1  
$EndComp
$Comp
L power:+5VL #PWR031
U 1 1 621B83FC
P 9750 1850
F 0 "#PWR031" H 9750 1700 50  0001 C CNN
F 1 "+5VL" H 9765 2023 50  0000 C CNN
F 2 "" H 9750 1850 50  0001 C CNN
F 3 "" H 9750 1850 50  0001 C CNN
	1    9750 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 1900 9750 1900
Wire Wire Line
	9750 1900 9750 1850
$Comp
L 74xGxx:74LVC2G240 U4
U 1 1 60ED0A89
P 6000 1400
F 0 "U4" H 5750 1250 50  0000 C CNN
F 1 "74LVC2G240" H 5750 1150 50  0000 C CNN
F 2 "Package_SO:TSSOP-8_3x3mm_P0.65mm" H 6150 2100 50  0001 C CNN
F 3 "http://www.ti.com/lit/sg/scyt129e/scyt129e.pdf" H 6000 1400 50  0001 C CNN
	1    6000 1400
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR023
U 1 1 623B73C5
P 7600 700
F 0 "#PWR023" H 7600 550 50  0001 C CNN
F 1 "VCC" H 7615 873 50  0000 C CNN
F 2 "" H 7600 700 50  0001 C CNN
F 3 "" H 7600 700 50  0001 C CNN
	1    7600 700 
	1    0    0    -1  
$EndComp
Text Label 10900 6100 0    50   ~ 0
BSY
Wire Wire Line
	7600 800  7600 700 
Connection ~ 7600 800 
$Comp
L pico-profile-lib:74LVC1G07 U8
U 1 1 6120C67E
P 8700 6250
F 0 "U8" H 8900 6400 50  0000 C CNN
F 1 "74LVC1G07" H 8950 6150 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 8700 6250 50  0001 C CNN
F 3 "http://www.ti.com/lit/sg/scyt129e/scyt129e.pdf" H 8700 6250 50  0001 C CNN
	1    8700 6250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR032
U 1 1 6125C165
P 8700 6000
F 0 "#PWR032" H 8700 5850 50  0001 C CNN
F 1 "+3V3" H 8715 6173 50  0000 C CNN
F 2 "" H 8700 6000 50  0001 C CNN
F 3 "" H 8700 6000 50  0001 C CNN
	1    8700 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 6000 8700 6150
$Comp
L pico-profile-lib:74LVC1G04 U5
U 1 1 612B2C66
P 6150 4150
F 0 "U5" H 5950 4300 50  0000 C CNN
F 1 "74LVC1G04" H 5900 3950 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 6150 4150 50  0001 C CNN
F 3 "http://www.ti.com/lit/sg/scyt129e/scyt129e.pdf" H 6150 4150 50  0001 C CNN
	1    6150 4150
	-1   0    0    -1  
$EndComp
Text Label 5750 4150 0    50   ~ 0
~R~W
$Comp
L power:+3V3 #PWR020
U 1 1 612E12FD
P 6150 3900
F 0 "#PWR020" H 6150 3750 50  0001 C CNN
F 1 "+3V3" H 6165 4073 50  0000 C CNN
F 2 "" H 6150 3900 50  0001 C CNN
F 3 "" H 6150 3900 50  0001 C CNN
	1    6150 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3900 6150 4050
Text Label 2050 5200 0    50   ~ 0
D7
Text Label 2050 5000 0    50   ~ 0
D6
Text Label 2050 4800 0    50   ~ 0
D5
Text Label 2050 4700 0    50   ~ 0
D4
Text Label 2050 4500 0    50   ~ 0
D3
Text Label 2050 4300 0    50   ~ 0
~PRES
Text Label 2050 4200 0    50   ~ 0
D2
Text Label 2050 3800 0    50   ~ 0
D1
Text Label 2050 3700 0    50   ~ 0
~PPARITY
Text Label 2050 3600 0    50   ~ 0
D0
Text Label 2050 3500 0    50   ~ 0
~PCMD
Text Label 2050 3300 0    50   ~ 0
~PBSY
Text Label 2050 3200 0    50   ~ 0
PR~W
Text Label 2050 3100 0    50   ~ 0
~PSTRB
$Comp
L Device:R R14
U 1 1 612B4C09
P 6450 1400
F 0 "R14" V 6400 1250 50  0000 C CNN
F 1 "100" V 6450 1400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6380 1400 50  0001 C CNN
F 3 "~" H 6450 1400 50  0001 C CNN
	1    6450 1400
	0    1    1    0   
$EndComp
Wire Wire Line
	6250 1400 6300 1400
Wire Wire Line
	6600 1400 6800 1400
$Comp
L Device:R R16
U 1 1 6133D368
P 6650 4150
F 0 "R16" V 6550 4150 50  0000 C CNN
F 1 "100" V 6650 4150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6580 4150 50  0001 C CNN
F 3 "~" H 6650 4150 50  0001 C CNN
	1    6650 4150
	0    1    1    0   
$EndComp
$Comp
L Device:R R17
U 1 1 6133D9F7
P 8150 4800
F 0 "R17" V 8050 4800 50  0000 C CNN
F 1 "100" V 8150 4800 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8080 4800 50  0001 C CNN
F 3 "~" H 8150 4800 50  0001 C CNN
	1    8150 4800
	0    1    1    0   
$EndComp
$Comp
L Device:R R18
U 1 1 6133DE3C
P 8150 5450
F 0 "R18" V 8050 5450 50  0000 C CNN
F 1 "100" V 8150 5450 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8080 5450 50  0001 C CNN
F 3 "~" H 8150 5450 50  0001 C CNN
	1    8150 5450
	0    1    1    0   
$EndComp
Wire Wire Line
	6800 4150 6850 4150
Wire Wire Line
	8000 4800 7650 4800
Wire Wire Line
	8000 5450 7650 5450
$Comp
L Device:R R15
U 1 1 614C1119
P 10100 6100
F 0 "R15" V 10050 5950 50  0000 C CNN
F 1 "100" V 10100 6100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10030 6100 50  0001 C CNN
F 3 "~" H 10100 6100 50  0001 C CNN
	1    10100 6100
	0    1    1    0   
$EndComp
Wire Wire Line
	9650 6100 9950 6100
Wire Wire Line
	7500 6250 8050 6250
$Comp
L pico-profile-lib:74LVC1G04 U6
U 1 1 6134DF7E
P 8700 4800
F 0 "U6" H 8500 4950 50  0000 C CNN
F 1 "74LVC1G04" H 8450 4600 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 8700 4800 50  0001 C CNN
F 3 "http://www.ti.com/lit/sg/scyt129e/scyt129e.pdf" H 8700 4800 50  0001 C CNN
	1    8700 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 4800 9150 4800
$Comp
L pico-profile-lib:74LVC1G04 U9
U 1 1 615394C7
P 8700 5450
F 0 "U9" H 8500 5600 50  0000 C CNN
F 1 "74LVC1G04" H 8450 5250 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 8700 5450 50  0001 C CNN
F 3 "http://www.ti.com/lit/sg/scyt129e/scyt129e.pdf" H 8700 5450 50  0001 C CNN
	1    8700 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 5450 9150 5450
Wire Wire Line
	8300 5450 8400 5450
$Comp
L power:+3V3 #PWR0101
U 1 1 615B1879
P 8700 5250
F 0 "#PWR0101" H 8700 5100 50  0001 C CNN
F 1 "+3V3" H 8715 5423 50  0000 C CNN
F 2 "" H 8700 5250 50  0001 C CNN
F 3 "" H 8700 5250 50  0001 C CNN
	1    8700 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 5250 8700 5350
$Comp
L Device:R R1
U 1 1 612A2938
P 2400 5100
F 0 "R1" V 2350 4950 50  0000 C CNN
F 1 "0" V 2400 5100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2330 5100 50  0001 C CNN
F 3 "~" H 2400 5100 50  0001 C CNN
	1    2400 5100
	0    1    1    0   
$EndComp
Text Label 2000 1900 0    50   ~ 0
PCHK
Wire Wire Line
	1700 1900 2000 1900
Text Label 1950 5100 0    50   ~ 0
PCHK
Wire Wire Line
	1750 5100 2250 5100
$Comp
L power:GND #PWR0102
U 1 1 612F4789
P 2650 5100
F 0 "#PWR0102" H 2650 4850 50  0001 C CNN
F 1 "GND" H 2655 4927 50  0000 C CNN
F 2 "" H 2650 5100 50  0001 C CNN
F 3 "" H 2650 5100 50  0001 C CNN
	1    2650 5100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2550 5100 2650 5100
Wire Wire Line
	7600 1250 8000 1250
Connection ~ 8000 800 
Wire Wire Line
	8550 1150 8550 1250
Connection ~ 8550 1250
Wire Wire Line
	8550 1250 8950 1250
Wire Wire Line
	8950 800  8550 800 
Wire Wire Line
	8550 800  8550 850 
Connection ~ 8950 800 
$Comp
L Connector:Screw_Terminal_01x02 J5
U 1 1 61277E40
P 7050 1050
F 0 "J5" H 6968 1267 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 6968 1176 50  0000 C CNN
F 2 "Connector_Phoenix_MSTB:PhoenixContact_MSTBA_2,5_2-G-5,08_1x02_P5.08mm_Horizontal" H 7050 1050 50  0001 C CNN
F 3 "~" H 7050 1050 50  0001 C CNN
	1    7050 1050
	-1   0    0    1   
$EndComp
Wire Wire Line
	7250 950  7250 800 
Wire Wire Line
	7250 1050 7250 1250
Wire Wire Line
	7250 1250 7600 1250
Connection ~ 7600 1250
$Comp
L Diode:MBR1020VL D1
U 1 1 614028B7
P 7400 800
F 0 "D1" H 7400 584 50  0000 C CNN
F 1 "MBR1020VL" H 7400 675 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123F" H 7400 625 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/MBR1020VL-D.PDF" H 7400 800 50  0001 C CNN
	1    7400 800 
	-1   0    0    1   
$EndComp
Wire Wire Line
	7550 800  7600 800 
$Comp
L Device:R R19
U 1 1 6129DB38
P 8050 6100
F 0 "R19" V 7950 6100 50  0000 C CNN
F 1 "10k" V 8050 6100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7980 6100 50  0001 C CNN
F 3 "~" H 8050 6100 50  0001 C CNN
	1    8050 6100
	-1   0    0    1   
$EndComp
Connection ~ 8050 6250
Wire Wire Line
	8050 6250 8400 6250
$Comp
L power:+5VL #PWR0103
U 1 1 612B3D43
P 8050 5900
F 0 "#PWR0103" H 8050 5750 50  0001 C CNN
F 1 "+5VL" H 8065 6073 50  0000 C CNN
F 2 "" H 8050 5900 50  0001 C CNN
F 3 "" H 8050 5900 50  0001 C CNN
	1    8050 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 5950 8050 5900
Wire Bus Line
	3450 1350 3450 2350
Wire Bus Line
	700  1150 700  2350
Wire Bus Line
	10250 2800 10250 4200
Wire Bus Line
	5950 5150 5950 6000
Wire Bus Line
	7200 1900 7200 2850
Wire Bus Line
	3450 2350 3450 5100
Wire Bus Line
	5900 2850 5900 3900
Wire Bus Line
	4400 1050 4400 6200
$EndSCHEMATC
