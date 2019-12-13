v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {LIGHTNING DETECTOR} 5740 -5900 2 1 0.333333 0.333333 {}
T {techman@dingoblue.net.au} 6110 -5830 2 1 0.4 0.4 {}
T {1.00} 6100 -5860 2 1 0.333333 0.333333 {}
T {lightning.sch} 5720 -5860 2 1 0.333333 0.333333 {}
N 5310 -6310 5310 -6150 {lab=#net1}
N 5310 -6060 5310 -6000 {lab=bat(0v)}
N 5400 -6160 5400 -6110 {lab=#net1}
N 5400 -6020 5400 -6000 {lab=bat(0v)}
N 5310 -6160 5400 -6160 {lab=#net1}
N 5490 -6160 5500 -6160 {lab=#net2}
N 5570 -6390 5570 -6210 {lab=#net3}
N 5310 -6470 5310 -6400 {lab=A1}
N 5310 -6000 6250 -6000 {lab=bat(0v)}
N 5570 -6110 5570 -6000 {lab=bat(0v)}
N 5570 -6000 6070 -6000 {lab=bat(0v)}
N 5490 -6350 5490 -6330 {lab=#net3}
N 5490 -6240 5490 -6160 {lab=#net2}
N 6220 -6050 6220 -6000 {lab=bat(0v)}
N 5570 -6600 6250 -6600 {lab=bat(+3v)}
N 5570 -6600 5570 -6480 {lab=bat(+3v)}
N 6200 -6600 6200 -6580 {lab=bat(+3v)}
N 6200 -6490 6200 -6300 {lab=lamp(1)}
N 6200 -6300 6240 -6300 {lab=lamp(1)}
N 5490 -6350 5570 -6350 {lab=#net3}
N 5700 -6100 6150 -6100 {lab=#net4}
N 5700 -6160 5700 -6100 {lab=#net4}
N 6070 -6100 6070 -6090 {lab=#net4}
N 6070 -6200 6070 -6100 {lab=#net4}
N 6070 -6480 6070 -6300 {lab=lamp(1)}
N 5570 -6300 5610 -6300 {lab=#net3}
N 5700 -6360 5700 -6250 {lab=#net5}
N 5650 -6480 5650 -6410 {lab=lamp(1)}
N 5650 -6480 6200 -6480 {lab=lamp(1)}
N 5800 -6250 5870 -6250 {lab=#net6}
N 6220 -6200 6240 -6200 {lab=lamp(2)}
N 6220 -6200 6220 -6150 {lab=lamp(2)}
N 5990 -6250 5990 -6210 {lab=#net7}
N 5990 -6120 5990 -6100 {lab=#net4}
N 5800 -6350 5850 -6350 {lab=#net8}
N 5850 -6350 5850 -6210 {lab=#net8}
N 5850 -6120 5850 -6100 {lab=#net4}
N 6130 -6600 6130 -6410 {lab=bat(+3v)}
N 6130 -6320 6130 -6000 {lab=bat(0v)}
N 5960 -6250 6000 -6250 {lab=#net7}
N 5700 -6300 5730 -6300 {lab=#net5}
N 5700 -6480 5700 -6450 {lab=lamp(1)}
N 5800 -6480 5800 -6470 {lab=lamp(1)}
N 5800 -6380 5800 -6350 {lab=#net8}
C {inductor-1.sym} 5320 -6060 3 0 {name=L1
value=10mH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 5320 -6310 3 0 {name=L2
value=10mH
device=INDUCTOR
symversion=0.1
}
C {title-A4.sym} 5250 -5820 0 0 {}
C {capacitor-2.sym} 5400 -6140 0 0 {name=C2
value=0.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5420 -6110 3 1 {name=C1
value=680pf
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5610 -6280 0 0 {name=C3
value=.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5830 -6210 1 0 {value=100uF
name=C4
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5970 -6210 1 0 {value=.005uF
name=C5
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {resistor-1.sym} 5480 -6330 1 0 {name=R1
value=180k
device=RESISTOR
}
C {resistor-1.sym} 5560 -6480 1 0 {name=R2
value=3.9k
device=RESISTOR
}
C {resistor-1.sym} 6210 -6490 3 0 {name=R7
value=47R
device=RESISTOR
}
C {resistor-1.sym} 5710 -6160 3 0 {name=R3
value=22k
device=RESISTOR
}
C {resistor-1.sym} 5870 -6240 0 0 {name=R5
value=2.2K
device=RESISTOR
}
C {resistor-1.sym} 6080 -6000 3 0 {name=R6
value=2.7k
device=RESISTOR
}
C {resistor-variable-1.sym} 5710 -6360 3 0 {name=R4
value=20k
device=VARIABLE_RESISTOR
}
C {out-1.sym} 6240 -6290 0 0 {name=lamp(1)
lab=lamp(1)
device=OUTPUT
}
C {out-1.sym} 6240 -6190 0 0 {name=lamp(2)
lab=lamp(2)
device=OUTPUT
}
C {in-1.sym} 6310 -6610 2 0 {name=bat(+3v)
lab=bat(+3v)
device=INPUT
}
C {in-1.sym} 6310 -6010 2 0 {name=bat(0v)
lab=bat(0v)
device=INPUT
}
C {in-1.sym} 5300 -6530 1 0 {name=A1
lab=A1
device=INPUT
}
C {capacitor-2.sym} 6110 -6410 1 0 {value=1uF
name=C6
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {diode-1.sym} 5780 -6470 1 0 {name=D1
value=1N914
device=DIODE
}
C {2N4401.sym} 5500 -6110 0 0 {name=Q1
value=2N4401
}
C {2N4403.sym} 5730 -6350 2 1 {name=Q2
value=2N4403
}
C {2N4401.sym} 6000 -6200 0 0 {value=2N4401
name=Q3
}
C {2N4401.sym} 6150 -6050 0 0 {name=Q4
value=2N4401
}
