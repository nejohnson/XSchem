v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {LIGHTNING DETECTOR} 5740 -5900 2 1 0.333333 0.333333 {}
T {techman@dingoblue.net.au} 6110 -5830 2 1 0.4 0.4 {}
T {1.00} 6100 -5860 2 1 0.333333 0.333333 {}
T {lightning.sch} 5720 -5860 2 1 0.333333 0.333333 {}






C {inductor-1.sym} 5310 -6060 3 0 {name=L1
value=10mH
device=INDUCTOR
symversion=0.1
}
C {inductor-1.sym} 5310 -6310 3 0 {name=L2
value=10mH
device=INDUCTOR
symversion=0.1
}
C {title-A4.sym} 5250 -5820 0 0 {}
C {capacitor-2.sym} 5350 -6140 0 0 {name=C2
value=0.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5330 -6110 1 0 {name=C1
value=680pf
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5560 -6280 0 0 {name=C3
value=.01uF
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5780 -6210 1 0 {value=100uF
name=C4
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {capacitor-2.sym} 5920 -6210 1 0 {value=.005uF
name=C5
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {resistor-1.sym} 5430 -6330 1 0 {name=R1
value=180k
device=RESISTOR
}
C {resistor-1.sym} 5510 -6480 1 0 {name=R2
value=3.9k
device=RESISTOR
}
C {resistor-1.sym} 6160 -6490 3 0 {name=R7
value=47R
device=RESISTOR
}
C {resistor-1.sym} 5660 -6160 3 0 {name=R3
value=22k
device=RESISTOR
}
C {resistor-1.sym} 5820 -6240 0 0 {name=R5
value=2.2K
device=RESISTOR
}
C {resistor-1.sym} 6030 -6000 3 0 {name=R6
value=2.7k
device=RESISTOR
}
C {resistor-variable-1.sym} 5660 -6360 3 0 {name=R4
value=20k
device=VARIABLE_RESISTOR
}
C {out-1.sym} 6190 -6290 0 0 {name=lamp(1)
device=OUTPUT
}
C {out-1.sym} 6190 -6190 0 0 {name=lamp(2)
device=OUTPUT
}
C {in-1.sym} 6260 -6610 2 0 {name=bat(+3v)
device=INPUT
}
C {in-1.sym} 6260 -6010 2 0 {name=bat(0v)
device=INPUT
}
C {in-1.sym} 5290 -6530 1 0 {name=A1
device=INPUT
}
C {capacitor-2.sym} 6060 -6410 1 0 {value=1uF
name=C6
device=POLARIZED_CAPACITOR
symversion=0.1
}
C {diode-1.sym} 5730 -6470 1 0 {name=D1
value=1N914
device=DIODE
}
C {2N4401.sym} 5450 -6110 0 0 {name=Q1
value=2N4401
}
C {2N4403.sym} 5680 -6350 2 1 {name=Q2
value=2N4403
}
C {2N4401.sym} 5950 -6200 0 0 {value=2N4401
name=Q3
}
C {2N4401.sym} 6100 -6050 0 0 {name=Q4
value=2N4401
}

N 5300 -6310 5300 -6150 {}
N 5300 -6060 5300 -6000 {}
N 5350 -6110 5350 -6160 {}
N 5350 -6020 5350 -6000 {}
N 5350 -6160 5300 -6160 {}
N 5440 -6160 5450 -6160 {}
N 5520 -6390 5520 -6210 {}
N 5300 -6470 5300 -6400 {}
N 6200 -6000 5300 -6000 {}
N 5520 -6110 5520 -6000 {}
N 5520 -6000 6020 -6000 {}
N 5440 -6350 5440 -6330 {}
N 5440 -6240 5440 -6160 {}
N 6170 -6050 6170 -6000 {}
N 6200 -6600 5520 -6600 {}
N 5520 -6600 5520 -6480 {}
N 6150 -6580 6150 -6600 {}
N 6150 -6300 6150 -6490 {}
N 6150 -6300 6190 -6300 {}
N 5440 -6350 5520 -6350 {}
N 6100 -6100 5650 -6100 {}
N 5650 -6100 5650 -6160 {}
N 6020 -6090 6020 -6100 {}
N 6020 -6200 6020 -6100 {}
N 6020 -6300 6020 -6480 {}
N 5520 -6300 5560 -6300 {}
N 5650 -6250 5650 -6360 {}
N 5600 -6410 5600 -6480 {}
N 5600 -6480 6150 -6480 {}
N 5820 -6250 5750 -6250 {}
N 6190 -6200 6170 -6200 {}
N 6170 -6200 6170 -6150 {}
N 5940 -6250 5940 -6210 {}
N 5940 -6120 5940 -6100 {}
N 5750 -6350 5800 -6350 {}
N 5800 -6350 5800 -6210 {}
N 5800 -6120 5800 -6100 {}
N 6080 -6410 6080 -6600 {}
N 6080 -6320 6080 -6000 {}
N 5910 -6250 5950 -6250 {}
N 5650 -6300 5680 -6300 {}
N 5650 -6450 5650 -6480 {}
N 5750 -6470 5750 -6480 {}
N 5750 -6350 5750 -6380 {}

