v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {LIGHTNING DETECTOR} 5740 -5900 2 1 0.333333 0.333333 {}
T {techman@dingoblue.net.au} 6110 -5830 2 1 0.4 0.4 {}
T {1.00} 6100 -5860 2 1 0.333333 0.333333 {}
T {lightning.sch} 5720 -5860 2 1 0.333333 0.333333 {}
N 5320 -6320 5320 -6160 {lab=#net1}
N 5320 -6070 5320 -6010 {lab=bat(0v)}
N 5390 -6170 5390 -6120 {lab=#net1}
N 5390 -6030 5390 -6010 {lab=bat(0v)}
N 5320 -6170 5390 -6170 {lab=#net1}
N 5480 -6170 5490 -6170 {lab=#net2}
N 5560 -6400 5560 -6220 {lab=#net3}
N 5320 -6480 5320 -6410 {lab=A1}
N 5320 -6010 6240 -6010 {lab=bat(0v)}
N 5560 -6120 5560 -6010 {lab=bat(0v)}
N 5560 -6010 6060 -6010 {lab=bat(0v)}
N 5480 -6360 5480 -6340 {lab=#net3}
N 5480 -6250 5480 -6170 {lab=#net2}
N 6210 -6060 6210 -6010 {lab=bat(0v)}
N 5560 -6610 6240 -6610 {lab=bat(+3v)}
N 5560 -6610 5560 -6490 {lab=bat(+3v)}
N 6190 -6610 6190 -6590 {lab=bat(+3v)}
N 6190 -6500 6190 -6310 {lab=lamp(1)}
N 6190 -6310 6230 -6310 {lab=lamp(1)}
N 5480 -6360 5560 -6360 {lab=#net3}
N 5690 -6110 6140 -6110 {lab=#net4}
N 5690 -6170 5690 -6110 {lab=#net4}
N 6060 -6110 6060 -6100 {lab=#net4}
N 6060 -6210 6060 -6110 {lab=#net4}
N 6060 -6490 6060 -6310 {lab=lamp(1)}
N 5560 -6310 5600 -6310 {lab=#net3}
N 5690 -6370 5690 -6260 {lab=#net5}
N 5640 -6490 5640 -6420 {lab=lamp(1)}
N 5640 -6490 6190 -6490 {lab=lamp(1)}
N 5790 -6260 5860 -6260 {lab=#net6}
N 6210 -6210 6230 -6210 {lab=lamp(2)}
N 6210 -6210 6210 -6160 {lab=lamp(2)}
N 5980 -6260 5980 -6220 {lab=#net7}
N 5980 -6130 5980 -6110 {lab=#net4}
N 5790 -6360 5840 -6360 {lab=#net8}
N 5840 -6360 5840 -6220 {lab=#net8}
N 5840 -6130 5840 -6110 {lab=#net4}
N 6120 -6610 6120 -6420 {lab=bat(+3v)}
N 6120 -6330 6120 -6010 {lab=bat(0v)}
N 5950 -6260 5990 -6260 {lab=#net7}
N 5690 -6310 5720 -6310 {lab=#net5}
N 5690 -6490 5690 -6460 {lab=lamp(1)}
N 5790 -6490 5790 -6480 {lab=lamp(1)}
N 5790 -6390 5790 -6360 {lab=#net8}
C {inductor-1.sym} 5330 -6070 3 0 {name=L1
value=10mH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 5330 -6320 3 0 {name=L2
value=10mH
device=INDUCTOR
symversion=0.1
}
C {title-A4.sym} 5250 -5820 0 0 {}
C {capacitor-2.sym} 5390 -6150 0 0 {name=C2
value=0.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5370 -6120 1 0 {name=C1
value=680pf
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5600 -6290 0 0 {name=C3
value=.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5820 -6220 1 0 {name=C4
value=100uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5960 -6220 1 0 {name=C5
value=.005uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {resistor-1.sym} 5470 -6340 1 0 {name=R1
value=180k
device=RESISTOR
}
C {resistor-1.sym} 5550 -6490 1 0 {name=R2
value=3.9k
device=RESISTOR
}
C {resistor-1.sym} 6200 -6500 3 0 {name=R7
value=47R
device=RESISTOR
}
C {resistor-1.sym} 5700 -6170 3 0 {name=R3
value=22k
device=RESISTOR
}
C {resistor-1.sym} 5860 -6250 0 0 {name=R5
value=2.2K
device=RESISTOR
}
C {resistor-1.sym} 6070 -6010 3 0 {name=R6
value=2.7k
device=RESISTOR
}
C {resistor-variable-1.sym} 5700 -6370 3 0 {name=R4
value=20k
device=VARIABLE_RESISTOR
}
C {out-1.sym} 6230 -6300 0 0 {name=lamp(1)
lab=lamp(1)
device=OUTPUT
}
C {out-1.sym} 6230 -6200 0 0 {name=lamp(2)
lab=lamp(2)
device=OUTPUT
}
C {in-1.sym} 6300 -6620 2 0 {name=bat(+3v)
lab=bat(+3v)
device=INPUT
}
C {in-1.sym} 6300 -6020 2 0 {name=bat(0v)
lab=bat(0v)
device=INPUT
}
C {in-1.sym} 5310 -6540 1 0 {name=A1
lab=A1
device=INPUT
}
C {capacitor-2.sym} 6100 -6420 1 0 {name=C6
value=1uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {diode-1.sym} 5770 -6480 1 0 {name=D1
value=1N914
device=DIODE
}
C {2N4401.sym} 5490 -6120 0 0 {name=Q1
value=2N4401
}
C {2N4403.sym} 5720 -6360 2 1 {name=Q2
value=2N4403
}
C {2N4401.sym} 5990 -6210 0 0 {name=Q3
value=2N4401
}
C {2N4401.sym} 6140 -6060 0 0 {name=Q4
value=2N4401
}
