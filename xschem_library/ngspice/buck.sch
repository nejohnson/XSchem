v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {Buck Regulator
This circuit is a simplified buck regulator.
Instead of a digital logic block controlling the regulator,
a simple triangle wave and comparator generates the switch pulses. } 20 -790 0 0 0.6 0.6 {}
T {Copyright (C) 2011 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General Public License,
either verion 2 or (at your choice) any later version.} 20 -150 0 0 0.4 0.4 {}
N 60 -470 180 -470 {lab=nin}
N 60 -470 60 -340 {lab=nin}
N 210 -230 210 -220 {lab=0}
N 60 -220 210 -220 {lab=0}
N 60 -280 60 -220 {lab=0}
N 210 -430 210 -350 {lab=ntriangle}
N 350 -360 350 -220 {lab=0}
N 210 -220 350 -220 {lab=0}
N 350 -470 350 -420 {lab=ndiode}
N 240 -470 350 -470 {lab=ndiode}
N 350 -470 400 -470 {lab=ndiode}
N 350 -220 570 -220 {lab=0}
N 570 -300 570 -220 {lab=0}
N 520 -470 570 -470 {lab=nout}
N 570 -470 570 -420 {lab=nout}
N 690 -470 690 -420 {lab=nout}
N 570 -470 690 -470 {lab=nout}
N 690 -300 690 -220 {lab=0}
N 570 -220 690 -220 {lab=0}
N 230 -430 570 -430 {lab=nout}
C {vsource.sym} 60 -310 0 0 {name=Vin value="DC pwl 0 0 50u 5V"}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {gnd.sym} 60 -220 0 0 {name=l2 lab=0}
C {switch_ngspice.sym} 210 -470 3 0 {name=S1 model=swmod}
C {diode.sym} 350 -390 2 0 {name=D1 model=DIODE area=1}
C {vsource.sym} 210 -320 0 0 {name=Vpulse value="pulse -0.06 0.14 0 3.32u 3.32u 1f 6.67u"}
C {vsource.sym} 210 -260 0 0 {name=Vset value=3.3}
C {ammeter.sym} 490 -470 3 0 {name=Vmeas}
C {capa.sym} 570 -330 0 0 {name=C1
m=1
value=47u
footprint=1206
device="ceramic capacitor"}
C {ammeter.sym} 570 -390 0 0 {name=Vcap}
C {ammeter.sym} 690 -390 0 0 {name=Vload}
C {res.sym} 690 -330 0 0 {name=Rload
value=13.2
footprint=1206
device=resistor
m=1}
C {ind.sym} 430 -470 3 1 {name=L1
m=1
value=18u
footprint=1206
device=inductor}
C {code.sym} 780 -470 0 0 {name=MODELS value=".MODEL DIODE D(IS=1.139e-08 RS=0.99 CJO=9.3e-12 VJ=1.6 M=0.411 BV=30 EG=0.7 ) 
.MODEL swmod SW(VT=0 VH=0.01 RON=1 ROFF=100000)
"}
C {code_shown.sym} 790 -320 0 0 {name=COMMANDS value=".save all
.tran 0.001us 0.25ms
"}
C {lab_wire.sym} 650 -470 0 0 {name=l3 sig_type=std_logic lab=nout}
C {lab_wire.sym} 340 -470 0 0 {name=l4 sig_type=std_logic lab=ndiode}
C {lab_wire.sym} 140 -470 0 0 {name=l5 sig_type=std_logic lab=nin}
C {lab_wire.sym} 210 -360 0 0 {name=l6 sig_type=std_logic lab=ntriangle}
C {launcher.sym} 90 -580 0 0 {name=h1
descr="Ctrl-click to go to Delorie's 
project page for info"
url="http://www.delorie.com/electronics/spice-stuff"}
