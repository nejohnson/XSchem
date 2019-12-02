v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
L 4 120 -420 120 -410 {}
L 4 120 -180 120 -170 {}
L 4 690 -420 690 -410 {}
L 4 690 -180 690 -170 {}
B 7 115 -410 125 -400 {}
B 7 115 -170 125 -160 {}
B 7 685 -410 695 -400 {}
B 7 685 -170 695 -160 {}
P 4 5 120 -440 110 -430 120 -420 130 -430 120 -440 {}
P 4 5 120 -200 110 -190 120 -180 130 -190 120 -200 {}
P 4 5 690 -440 680 -430 690 -420 700 -430 690 -440 {}
P 4 5 690 -200 680 -190 690 -180 700 -190 690 -200 {}
P 4 5 70 -460 70 -150 740 -150 740 -460 70 -460 {}
T {MANUFACTURING NOTES
REFERENCE: CONN1 IS IN THE LOWER LEFT

LED8 (POWER) IS ALIGNED POS LEFT NEG RIGHT
LED2 AND LED1 ARE ALIGNED POS DOWN NEG UP
LED3/4/5/6 ARE ALIGNED POS LEFT NEG RIGHT
LED7 IS ALIGNED POS LEFT NEG RIGHT
I.E. BEVELED EDGE ON SILKSCREEN = PIN1 = ANODE

U1 PIN 1 IS UPPER RIGHT
U2 PIN 1 IS UPPER LEFT
X1 MAY BE INSTALLED EITHER ORIENTATION

ALL CAPACITORS, RESISTORS, AND L1 ARE NONPOLAR
AND MAY BE INSTALLED EITHER ORIENTATION} 1475 -545 0 0 0.5 0.5 {}
T {PCB MATERIAL FR4 OR EQUIV 0.031 OR 0.062 INCH
COPPER ANY FROM 0.5 to 2.0 OZ
SOLDERMASK BOTH SIDES DIFFERENT
SILKSCREEN BOTH SIDES DIFFERENT
SOLDERMASK AND SILKSCREEN ANY COLOR 
AS LONG AS THEY CONTRAST
DRILL TOLERANCE +- 0.002 INCH

PCB FINAL SIZE IS 1.0 INCH BY 2.0 INCH} 795 -545 0 0 0.5 0.5 {}
T {FIDUCIAL LOCATIONS ARE IN MILS (0.001 INCH) RELATIVE
TO THE LOWER LEFT (CONN1 LOCATION) OF THE PCB

FIDUCIALS ARE EXPOSED 0.025 INCH SQUARE COPPER
WITH CENTERPOINTS NOTED AS BELOW} 40 -640 0 0 0.5 0.5 {}
T {X = 1890
Y = 840} 550 -430 0 0 0.5 0.5 {}
T {X = 1950
Y = 50} 550 -220 0 0 0.5 0.5 {}
T {X = 60
Y = 910} 140 -440 0 0 0.5 0.5 {}
T {X = 50
Y = 50} 140 -230 0 0 0.5 0.5 {}
T {(CONN1)} 90 -130 0 0 0.5 0.5 {}
T {TP1} 105 -455 0 0 0.3 0.3 {layer=8}
T {TP2} 105 -215 0 0 0.3 0.3 {layer=8}
T {TP4} 675 -215 0 0 0.3 0.3 {layer=8}
T {TP3} 675 -455 0 0 0.3 0.3 {layer=8}
T {ALL RESISTORS ARE 5% OR BETTER
ALL CAPACITORS ARE 10V X5R OR BETTER
SUBSTITUTIONS TO SIMILAR OR HIGHER
SPEC PARTS IS ALLOWED.
DNP MEANS DO NOT POPULATE} 795 -245 0 0 0.5 0.5 {}
T {1 x 2 in R8C/33C Lab/Breadboard helper. P0 and P3 drive the breadboard, 
giving GPIO, ADC, DAC, SPI, I2C, and UART on the breadboard. 
P1 is used for serial mode 2 programming and UART communications via USB
(with HW flow control), as well as driving four discrete LEDs. 
P2's PWM outputs drive an RGB LED. } 385 -2215 0 0 0.7 0.7 {}
T {R8C/33c DevCon Lab Board} 830 -2375 0 0 1 1 {}
T {Copyright (C) 2010 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General Public License,
either verion 2 or (at your choice) any later version.} 2340 -340 0 0 0.7 0.7 {layer=8}
N 290 -1470 640 -1470 {lab=USBDM}
N 420 -1430 640 -1430 {lab=USBDP}
N 420 -1440 420 -1430 {lab=USBDP}
N 290 -1440 420 -1440 {lab=USBDP}
N 290 -1880 290 -1500 {lab=RAW_5V}
N 290 -1880 410 -1880 {lab=RAW_5V}
N 470 -1880 470 -1820 {lab=USB_5V}
N 410 -1880 410 -1820 {lab=RAW_5V}
N 580 -1880 670 -1880 {lab=USB_5V}
N 700 -1830 700 -1820 {lab=#net1}
N 700 -1830 770 -1830 {lab=#net1}
N 1080 -1310 1150 -1310 {lab=PWREN}
N 1150 -1830 1150 -1310 {lab=PWREN}
N 700 -1840 700 -1830 {lab=#net1}
N 830 -1830 1150 -1830 {lab=PWREN}
N 730 -1880 970 -1880 {lab=+5V}
N 970 -1910 970 -1880 {lab=+5V}
N 1250 -1880 1370 -1880 {lab=+5V}
N 1250 -1790 1250 -1770 {lab=#net2}
N 1250 -1880 1250 -1850 {lab=+5V}
N 1080 -1470 1250 -1470 {lab=URX}
N 1250 -1710 1250 -1470 {lab=URX}
N 1370 -1790 1370 -1770 {lab=#net3}
N 1370 -1880 1370 -1850 {lab=+5V}
N 1370 -1710 1370 -1430 {lab=UTX}
N 1080 -1430 1370 -1430 {lab=UTX}
N 970 -1880 1250 -1880 {lab=+5V}
N 1490 -1910 1490 -1880 {lab=+5V}
N 1630 -1830 1700 -1830 {lab=+5V}
N 1630 -1910 1630 -1830 {lab=+5V}
N 1700 -1830 1700 -1800 {lab=+5V}
N 1700 -1800 1760 -1800 {lab=+5V}
N 1760 -1830 1760 -1800 {lab=+5V}
N 1760 -1830 1920 -1830 {lab=+5V}
N 1920 -1160 2050 -1160 {lab=+5V}
N 1920 -1400 1920 -1160 {lab=+5V}
N 1920 -1400 2050 -1400 {lab=+5V}
N 1920 -1830 1920 -1400 {lab=+5V}
N 1080 -1390 1500 -1390 {lab=MODE}
N 1500 -1360 2050 -1360 {lab=MODE}
N 1500 -1390 1500 -1360 {lab=MODE}
N 1080 -1230 1520 -1230 {lab=RESET}
N 1520 -1320 1520 -1230 {lab=RESET}
N 1520 -1320 2050 -1320 {lab=RESET}
N 2150 -2140 2150 -2120 {lab=+5V}
N 2060 -2120 2150 -2120 {lab=+5V}
N 2060 -2170 2060 -2120 {lab=+5V}
N 2170 -2140 2170 -2100 {lab=GND}
N 2190 -2140 2190 -1890 {lab=#net4}
N 2170 -1890 2190 -1890 {lab=#net4}
N 2170 -1890 2170 -1600 {lab=#net4}
N 2210 -2140 2210 -1600 {lab=#net5}
N 2250 -1890 2250 -1600 {lab=#net6}
N 2230 -1890 2250 -1890 {lab=#net6}
N 2230 -2140 2230 -1890 {lab=#net6}
N 2290 -1910 2290 -1600 {lab=#net7}
N 2250 -1910 2290 -1910 {lab=#net7}
N 2250 -2140 2250 -1910 {lab=#net7}
N 2330 -1930 2330 -1600 {lab=#net8}
N 2270 -1930 2330 -1930 {lab=#net8}
N 2270 -2140 2270 -1930 {lab=#net8}
N 2370 -1950 2370 -1600 {lab=#net9}
N 2290 -1950 2370 -1950 {lab=#net9}
N 2290 -2140 2290 -1950 {lab=#net9}
N 2410 -1970 2410 -1600 {lab=#net10}
N 2310 -1970 2410 -1970 {lab=#net10}
N 2310 -2140 2310 -1970 {lab=#net10}
N 2450 -1990 2450 -1600 {lab=#net11}
N 2330 -1990 2450 -1990 {lab=#net11}
N 2330 -2140 2330 -1990 {lab=#net11}
N 2170 -920 2170 -890 {lab=P3.5}
N 2090 -890 2170 -890 {lab=P3.5}
N 2210 -920 2210 -860 {lab=P3.4}
N 2090 -860 2210 -860 {lab=P3.4}
N 2250 -920 2250 -830 {lab=P3.3}
N 2090 -830 2250 -830 {lab=P3.3}
N 2020 -1120 2050 -1120 {lab=P3.7}
N 2350 -2010 2580 -2010 {lab=P3.3}
N 2350 -2140 2350 -2010 {lab=P3.3}
N 2370 -2040 2580 -2040 {lab=P3.4}
N 2370 -2140 2370 -2040 {lab=P3.4}
N 2490 -2070 2580 -2070 {lab=P3.5}
N 2390 -2140 2390 -2070 {lab=P3.5}
N 2550 -2100 2580 -2100 {lab=P3.7}
N 2410 -2140 2410 -2100 {lab=P3.7}
N 2490 -2170 2490 -2070 {lab=P3.5}
N 2550 -2170 2550 -2100 {lab=P3.7}
N 80 -930 80 -900 {lab=+5V}
N 270 -1270 640 -1270 {lab=GND}
N 270 -1270 270 -890 {lab=GND}
N 340 -1230 640 -1230 {lab=3V3OUT}
N 340 -1230 340 -1180 {lab=3V3OUT}
N 340 -1120 340 -890 {lab=GND}
N 410 -1190 640 -1190 {lab=GND}
N 410 -1190 410 -890 {lab=GND}
N 580 -1880 580 -1610 {lab=USB_5V}
N 720 -1610 800 -1610 {lab=USB_5V}
N 800 -1610 800 -1570 {lab=USB_5V}
N 720 -1610 720 -1570 {lab=USB_5V}
N 2570 -1200 2680 -1200 {lab=TXD}
N 2680 -1710 2680 -1200 {lab=TXD}
N 1670 -1710 2680 -1710 {lab=TXD}
N 1670 -1710 1670 -950 {lab=TXD}
N 1080 -950 1670 -950 {lab=TXD}
N 2570 -1160 2720 -1160 {lab=RTS}
N 2720 -1740 2720 -1160 {lab=RTS}
N 1610 -1740 2720 -1740 {lab=RTS}
N 1610 -1740 1610 -1030 {lab=RTS}
N 1080 -1030 1610 -1030 {lab=RTS}
N 2570 -1120 2750 -1120 {lab=CTS}
N 2750 -1770 2750 -1120 {lab=CTS}
N 1580 -1770 2750 -1770 {lab=CTS}
N 1580 -1770 1580 -1190 {lab=CTS}
N 1080 -1190 1580 -1190 {lab=CTS}
N 2570 -1240 2640 -1240 {lab=RXD}
N 2640 -1680 2640 -1240 {lab=RXD}
N 1720 -1680 2640 -1680 {lab=RXD}
N 1720 -1680 1720 -990 {lab=RXD}
N 1080 -990 1720 -990 {lab=RXD}
N 2570 -1400 2880 -1400 {lab=#net12}
N 3000 -1400 3030 -1400 {lab=GND}
N 3030 -1070 3030 -1040 {lab=GND}
N 3000 -1290 3030 -1290 {lab=GND}
N 3000 -1180 3030 -1180 {lab=GND}
N 3000 -1070 3030 -1070 {lab=GND}
N 2860 -1290 2880 -1290 {lab=#net13}
N 2860 -1360 2860 -1290 {lab=#net13}
N 2570 -1360 2860 -1360 {lab=#net13}
N 2840 -1180 2880 -1180 {lab=#net14}
N 2840 -1320 2840 -1180 {lab=#net14}
N 2570 -1320 2840 -1320 {lab=#net14}
N 2810 -1070 2880 -1070 {lab=#net15}
N 2810 -1280 2810 -1070 {lab=#net15}
N 2570 -1280 2810 -1280 {lab=#net15}
N 1960 -1240 2050 -1240 {lab=GND}
N 1770 -1280 2050 -1280 {lab=XOUT}
N 1770 -1060 1770 -920 {lab=XOUT}
N 1960 -1200 2050 -1200 {lab=XIN}
N 1960 -1060 1960 -920 {lab=XIN}
N 1890 -1060 1960 -1060 {lab=XIN}
N 1770 -1060 1830 -1060 {lab=XOUT}
N 470 -1880 580 -1880 {lab=USB_5V}
N 2390 -2070 2490 -2070 {lab=P3.5}
N 2410 -2100 2550 -2100 {lab=P3.7}
N 580 -1610 720 -1610 {lab=USB_5V}
N 3030 -1400 3030 -1290 {lab=GND}
N 1770 -1280 1770 -1060 {lab=XOUT}
N 1960 -1200 1960 -1060 {lab=XIN}
N 3030 -1290 3030 -1180 {lab=GND}
N 3030 -1180 3030 -1070 {lab=GND}
N 1890 -920 1960 -920 {lab=XIN}
N 1770 -920 1830 -920 {lab=XOUT}
N 2540 -790 2620 -790 {lab=#net16}
N 2540 -710 2570 -710 {lab=#net17}
N 2570 -760 2570 -710 {lab=#net17}
N 2570 -760 2620 -760 {lab=#net17}
N 2540 -860 2570 -860 {lab=#net18}
N 2570 -860 2570 -820 {lab=#net18}
N 2570 -820 2620 -820 {lab=#net18}
N 2370 -860 2480 -860 {lab=#net19}
N 2370 -920 2370 -860 {lab=#net19}
N 2330 -790 2480 -790 {lab=#net20}
N 2330 -920 2330 -790 {lab=#net20}
N 2290 -710 2480 -710 {lab=#net21}
N 2290 -920 2290 -710 {lab=#net21}
N 2680 -820 2750 -820 {lab=GND}
N 2750 -760 2750 -700 {lab=GND}
N 2680 -790 2750 -790 {lab=GND}
N 2680 -760 2750 -760 {lab=GND}
N 2750 -820 2750 -790 {lab=GND}
N 2750 -790 2750 -760 {lab=GND}
C {title-2.sym} 160 -30 0 0 {name=l1 
author="Stefan Schippers"
page=1
pages=1
title="R8C/R33C RULZ LAB BOARD"
lock=true}
C {usb-minib.sym} 210 -1440 0 0 {name=CONN1
model=usb-minib
device=UX60SC-MB-5ST(80)
footprint=hirose-UX60SC
comptag="manufacturer_part_number UX60SC-MB-5ST(80)
manifacturer Hirose
vendor_part_number H11671CT-ND
vendor digikey"
}
C {conn_14x1.sym} 2280 -2160 3 1 {name=CONN2
footprint=JUMPER_14
value=DNP
comptag="manufacturer_part_number DNP
manifacturer DNP
vendor_part_number DNP
vendor DNP" device=CONNECTOR_14}
C {jumper.sym} 1730 -1830 1 1 {name=JP1
footprint=JUMPER2
device=JUMPER}
C {r8c-33c.sym} 2310 -1260 0 0 {name=U2 device=r8c-33c
footprint=TQFP32_7
comptag="manufacturer_part_number R8C/33C
manifacturer Renesas
vendor_part_number R5F21336CNFP
vendor Renesas"}
C {ft232rl.sym} 860 -1210 0 0 {name=U1 
device="USB to Serial converter"
footprint=SSOP28
comptag="manufacturer_part_number FT232RL\\ R
manifacturer FTDI
vendor_part_number 768-1007-1-ND
vendor digikey"
}
C {gnd.sym} 290 -1380 0 0 {name=l2 lab=GND}
C {noconn.sym} 290 -1410 1 0 {name=l3}
C {noconn.sym} 210 -1320 3 0 {name=l4}
C {vdd.sym} 160 -1690 0 0 {name=l5 lab=+5V}
C {led.sym} 160 -1660 0 0 {name=LED8 
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 160 -1600 0 0 {name=R5
value=3.3k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX3301
manifacturer Rohm
vendor_part_number RHM3.30KHCT-ND
vendor digikey"}
C {gnd.sym} 160 -1570 0 0 {name=l6 lab=GND}
C {ind.sym} 440 -1880 3 1 {name=L1
value="600@100"
footprint=0603
device=INDUCTOR
comptag="manufacturer_part_number BLM18AG601SN1D
manifacturer Murata
vendor_part_number 490-1014-1-ND
vendor digikey"}
C {capa-2.sym} 470 -1790 0 0 {name=C2
value=4.7uF
footprint=0603
device=polarized_capacitor
comptag="manufacturer_part_number C0603C475K9PACTU
manifacturer Kemet
vendor_part_number 399-3482-1-ND
vendor digikey"}
C {gnd.sym} 410 -1760 0 0 {name=l7 lab=GND}
C {gnd.sym} 470 -1760 0 0 {name=l8 lab=GND}
C {lab_pin.sym} 290 -1880 0 0 {name=l9 sig_type=std_logic lab=RAW_5V}
C {capa.sym} 410 -1790 0 0 {name=C1
value=10nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C103K5RACTU
manifacturer Kemet
vendor_part_number 399-1091-1-ND
vendor digikey"}
C {pmos.sym} 700 -1860 1 1 {name=Q1 
model=DMP2035U 
device=DMP2035U
footprint SOT-523
comptag="manufacturer_part_number RZE002P02TL
manifacturer Rohm
vendor_part_number RZE002P02TLCT-ND
vendor digikey" 
pinnumber(G)=1 
pinnumber(S)=3 
pinnumber(D)=2}
C {capa.sym} 700 -1790 0 0 {name=C3
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 700 -1760 0 0 {name=l10 lab=GND}
C {res.sym} 800 -1830 3 1 {name=R1
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {vdd.sym} 970 -1910 0 0 {name=l11 lab=+5V}
C {led.sym} 1250 -1820 0 0 {name=LED1
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 1250 -1740 0 0 {name=R2
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {lab_wire.sym} 1210 -1470 0 0 {name=l12 sig_type=std_logic lab=URX}
C {led.sym} 1370 -1820 0 0 {name=LED2
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 1370 -1740 0 0 {name=R3
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {lab_wire.sym} 1210 -1430 0 0 {name=l13 sig_type=std_logic lab=UTX}
C {vdd.sym} 1490 -1910 0 0 {name=l14 lab=+5V}
C {capa.sym} 1490 -1850 0 0 {name=C5
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 1490 -1820 0 0 {name=l15 lab=GND}
C {vdd.sym} 1630 -1910 0 0 {name=l16 lab=+5V}
C {lab_wire.sym} 1210 -1390 0 0 {name=l17 sig_type=std_logic lab=MODE}
C {lab_wire.sym} 1150 -1590 0 0 {name=l18 sig_type=std_logic lab=PWREN}
C {noconn.sym} 1080 -1350 1 0 {name=l19}
C {lab_wire.sym} 1210 -1230 0 0 {name=l20 sig_type=std_logic lab=RESET}
C {vdd.sym} 2060 -2170 0 0 {name=l21 lab=+5V}
C {gnd.sym} 2170 -2100 0 1 {name=l22 lab=GND}
C {lab_pin.sym} 2090 -890 0 0 {name=p25 lab=P3.5}
C {lab_pin.sym} 2090 -860 0 0 {name=p26 lab=P3.4}
C {lab_pin.sym} 2090 -830 0 0 {name=p27 lab=P3.3}
C {lab_pin.sym} 2020 -1120 0 0 {name=p1 lab=P3.7}
C {lab_pin.sym} 2580 -2070 0 1 {name=p2 lab=P3.5}
C {lab_pin.sym} 2580 -2040 0 1 {name=p3 lab=P3.4}
C {lab_pin.sym} 2580 -2010 0 1 {name=p4 lab=P3.3}
C {lab_pin.sym} 2580 -2100 0 1 {name=p5 lab=P3.7}
C {res.sym} 2490 -2200 0 0 {name=R4
value=3.3k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX3301
manifacturer Rohm
vendor_part_number RHM3.30KHCT-ND
vendor digikey"}
C {vdd.sym} 2490 -2230 0 0 {name=l23 lab=+5V}
C {res.sym} 2550 -2200 0 0 {name=R6
value=3.3k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX3301
manifacturer Rohm
vendor_part_number RHM3.30KHCT-ND
vendor digikey"}
C {vdd.sym} 2550 -2230 0 0 {name=l24 lab=+5V}
C {vdd.sym} 80 -930 0 0 {name=l25 lab=+5V}
C {capa.sym} 80 -870 0 0 {name=C6
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 80 -840 0 0 {name=l26 lab=GND}
C {gnd.sym} 270 -890 0 0 {name=l27 lab=GND}
C {lab_wire.sym} 590 -1230 0 0 {name=l28 sig_type=std_logic lab=3V3OUT}
C {capa.sym} 340 -1150 0 0 {name=C4
value=100nF
footprint=0603
device="capacitor"
comptag="manufacturer_part_number C0603C104K4RACTU
manifacturer Kemet
vendor_part_number 399-1096-1-ND
vendor digikey"}
C {gnd.sym} 340 -890 0 0 {name=l29 lab=GND}
C {gnd.sym} 410 -890 0 0 {name=l30 lab=GND}
C {lab_wire.sym} 560 -1470 0 0 {name=l35 lab=USBDM}
C {lab_wire.sym} 560 -1430 0 0 {name=l36 lab=USBDP}
C {noconn.sym} 640 -1310 3 0 {name=l31}
C {noconn.sym} 640 -1350 3 0 {name=l32}
C {noconn.sym} 640 -1390 3 0 {name=l33}
C {noconn.sym} 1080 -1150 1 1 {name=l34}
C {noconn.sym} 1080 -1110 1 1 {name=l37}
C {noconn.sym} 1080 -1070 1 1 {name=l38}
C {gnd.sym} 720 -810 0 0 {name=l39 lab=GND}
C {gnd.sym} 780 -810 0 0 {name=l40 lab=GND}
C {gnd.sym} 840 -810 0 0 {name=l41 lab=GND}
C {lab_wire.sym} 570 -1880 0 0 {name=l42 lab=USB_5V}
C {res.sym} 2910 -1400 3 1 {name=R7
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1400 3 0 {name=LED3
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 2910 -1290 3 1 {name=R8
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1290 3 0 {name=LED4
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 2910 -1180 3 1 {name=R9
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1180 3 0 {name=LED5
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {res.sym} 2910 -1070 3 1 {name=R10
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {led.sym} 2970 -1070 3 0 {name=LED6
model=LED 
device=LTST-C190KGKT
footprint=0603 
value=GREEN
comptag="manufacturer_part_number LTST-C190KGKT
manifacturer Lite-on
vendor_part_number 160-1435-1-ND
vendor digikey"}
C {gnd.sym} 3030 -1040 0 0 {name=l43 lab=GND}
C {gnd.sym} 1960 -1240 0 0 {name=l44 lab=GND}
C {res.sym} 1860 -1060 3 1 {name=R15
value=1MEG
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1004
manifacturer Rohm
vendor_part_number RHM1.00MHCT-ND
vendor digikey"}
C {rgb_led.sym} 2650 -790 0 0 {name=LED7 model=XXX device=RGB_LED value=RGB footprint=LRTB-R98G comptag="manufacturer_part_number LRTB\\ R98G-R7T5-1+S7U-37+P7R-26
manifacturer OSRAM
vendor digikey"}
C {res.sym} 2510 -860 3 1 {name=R11
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {res.sym} 2510 -790 3 1 {name=R12
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {res.sym} 2510 -710 3 1 {name=R13
value=1k
footprint=0603
device=resistor
comptag="manufacturer_part_number MCR03EZPFX1001
manifacturer Rohm
vendor_part_number RHM1.00KHCT-ND
vendor digikey"}
C {gnd.sym} 2750 -700 0 0 {name=l45 lab=GND}
C {noconn.sym} 2410 -920 1 1 {name=l46}
C {noconn.sym} 2450 -920 1 1 {name=l47}
C {lab_wire.sym} 1210 -1190 0 0 {name=l48 sig_type=std_logic lab=CTS}
C {lab_wire.sym} 1190 -1030 0 1 {name=l49 lab=RTS}
C {lab_wire.sym} 1190 -990 0 1 {name=l50 lab=RXD}
C {lab_wire.sym} 1190 -950 0 1 {name=l51 lab=TXD}
C {lab_wire.sym} 2000 -1280 0 1 {name=l52 lab=XOUT}
C {lab_wire.sym} 2010 -1200 0 1 {name=l53 lab=XIN}
C {crystal-2.sym} 1860 -920 3 0 {name=X1
value=18.43MHz
footprint=res3-e15mm 300
device=CRYSTAL_module comptag="manufacturer_part_number AWSCR-18.43CV-T
manifacturer Abracon"}
C {gnd.sym} 1860 -890 0 0 {name=l54 lab=GND}
