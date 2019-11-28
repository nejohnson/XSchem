v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {Ligthning
simulation} 320 -180 0 0 0.4 0.4 {}
N 120 -440 120 -360 {lab=IN}
N 120 -440 220 -440 {lab=IN}
N 220 -440 220 -360 {lab=IN}
N 120 -540 120 -440 {lab=IN}
N 120 -300 120 -280 {lab=BAT0V}
N 1100 -280 1160 -280 {lab=BAT0V}
N 220 -300 220 -280 {lab=BAT0V}
N 220 -440 250 -440 {lab=IN}
N 360 -440 400 -440 {lab=B1}
N 120 -640 120 -600 {lab=A1}
N 440 -410 440 -280 {lab=BAT0V}
N 440 -500 440 -470 {lab=C1}
N 360 -520 440 -520 {lab=C1}
N 360 -460 360 -440 {lab=B1}
N 440 -660 440 -640 {lab=BAT3V}
N 440 -500 500 -500 {lab=C1}
N 580 -500 620 -500 {lab=B2}
N 580 -500 580 -440 {lab=B2}
N 660 -540 660 -530 {lab=E2}
N 660 -660 660 -640 {lab=LAMP1}
N 580 -660 580 -640 {lab=LAMP1}
N 580 -380 580 -340 {lab=E3}
N 880 -340 960 -340 {lab=E3}
N 660 -540 720 -540 {lab=E2}
N 720 -540 720 -440 {lab=E2}
N 720 -380 720 -340 {lab=E3}
N 660 -470 660 -460 {lab=C2}
N 660 -460 760 -460 {lab=C2}
N 880 -460 920 -460 {lab=B3}
N 880 -460 880 -440 {lab=B3}
N 880 -380 880 -340 {lab=E3}
N 960 -660 960 -490 {lab=LAMP1}
N 660 -660 960 -660 {lab=LAMP1}
N 960 -430 960 -340 {lab=E3}
N 960 -340 1060 -340 {lab=E3}
N 1100 -310 1100 -280 {lab=BAT0V}
N 1100 -420 1100 -370 {lab=LAMP2}
N 1100 -420 1160 -420 {lab=LAMP2}
N 1040 -760 1040 -640 {lab=BAT3V}
N 1040 -580 1040 -280 {lab=BAT0V}
N 120 -280 220 -280 {lab=BAT0V}
N 220 -280 440 -280 {lab=BAT0V}
N 310 -440 360 -440 {lab=B1}
N 440 -580 440 -520 {lab=C1}
N 440 -520 440 -500 {lab=C1}
N 560 -500 580 -500 {lab=B2}
N 660 -580 660 -540 {lab=E2}
N 580 -660 660 -660 {lab=LAMP1}
N 580 -340 720 -340 {lab=E3}
N 720 -340 880 -340 {lab=E3}
N 820 -460 880 -460 {lab=B3}
N 1040 -280 1100 -280 {lab=BAT0V}
N 580 -580 580 -500 {lab=B2}
N 440 -280 1040 -280 {lab=BAT0V}
N 440 -760 440 -660 {lab=BAT3V}
N 440 -760 1160 -760 {lab=BAT3V}
N 960 -660 1100 -660 {lab=LAMP1}
N 1100 -680 1100 -660 {lab=LAMP1}
N 1100 -760 1100 -740 {lab=BAT3V}
N 1100 -660 1100 -540 {lab=LAMP1}
N 1100 -540 1160 -540 {lab=LAMP1}
N 550 -660 550 -610 {lab=LAMP1}
N 550 -660 580 -660 {lab=LAMP1}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ind.sym} 120 -570 0 0 {name=L2 
m=1 
value=10m
footprint=1206 
device="inductor"}
C {ind.sym} 120 -330 0 0 {name=L1
m=1 
value=10m
footprint=1206 
device="inductor"}
C {capa.sym} 280 -440 3 1 {name=C2 
m=1 
value=10n
footprint=1206 
device="ceramic capacitor"}
C {capa.sym} 220 -330 0 1 {name=C1 
m=1 
value=680p
footprint=1206 
device="ceramic capacitor"}
C {npn.sym} 420 -440 0 0 {name=Q1 
model=Q2N4401 
device=2N4401 
footprint=TO92 
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {res.sym} 440 -610 0 0 {name=R2 
value=3.9K 
footprint=1206 
device=resistor 
m=1}
C {ipin.sym} 120 -640 1 0 {name=p1 lab=A1}
C {res.sym} 360 -490 0 0 {name=R1 
value=180K 
footprint=1206 
device=resistor 
m=1}
C {capa.sym} 530 -500 3 1 {name=C3
m=1 
value=10n
footprint=1206 
device="ceramic capacitor"}
C {pnp.sym} 640 -500 0 0 {name=Q2
model=Q2N4403
device=2N4403
footprint=TO92
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {diode.sym} 660 -610 0 0 {name=D1 model=D1N914 area=1}
C {res.sym} 580 -410 0 0 {name=R3 
value=22K
footprint=1206 
device=resistor 
m=1}
C {capa.sym} 720 -410 0 1 {name=C4 
m=1 
value=100u
footprint=1206 
device="ceramic capacitor"}
C {res.sym} 790 -460 3 1 {name=R5 
value=2.2K
footprint=1206 
device=resistor 
m=1}
C {capa.sym} 880 -410 0 1 {name=C5 
m=1 
value=5n
footprint=1206 
device="ceramic capacitor"}
C {npn.sym} 940 -460 0 0 {name=Q3 
model=Q2N4401 
device=2N4401 
footprint=TO92 
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {res.sym} 960 -310 0 0 {name=R6 
value=2.7K
footprint=1206 
device=resistor 
m=1}
C {npn.sym} 1080 -340 0 0 {name=Q4 
model=Q2N4401 
device=2N4401 
footprint=TO92 
area=1
pinnumber(C)=1
pinnumber(B)=2
pinnumber(E)=3}
C {capa.sym} 1040 -610 0 1 {name=C6 
m=1 
value=1u
footprint=1206 
device="ceramic capacitor"}
C {res.sym} 1100 -710 0 0 {name=R7 
value=47
footprint=1206 
device=resistor 
m=1}
C {ipin.sym} 1160 -760 0 1 {name=p2 lab=BAT3V}
C {ipin.sym} 1160 -280 0 1 {name=p3 lab=BAT0V}
C {opin.sym} 1160 -540 0 0 {name=p4 lab=LAMP1}
C {opin.sym} 1160 -420 0 0 {name=p5 lab=LAMP2}
C {var_res.sym} 580 -610 0 0 {name=R8
value=9.5K
footprint=1206
m=1}
C {code.sym} 40 -210 0 0 {name=MODELS value="
** you need to get the spice models for Q2N4401, Q2N4403 and D1N914
** and put them in file referenced below.
.include models_lightning.txt
"}
C {code_shown.sym} 190 -210 0 0 {name=COMMANDS value=".OP
*.save all
*.tran 4n 1m
"}
C {spice_probe.sym} 900 -760 0 0 {name=p7 analysis=tran voltage=3}
C {spice_probe.sym} 170 -440 0 0 {name=p9 analysis=tran voltage=0.0000e+00}
C {spice_probe.sym} 340 -440 0 1 {name=p10 analysis=tran voltage=0.6363}
C {spice_probe.sym} 670 -340 0 1 {name=p11 analysis=tran voltage=0.5154}
C {spice_probe.sym} 850 -460 0 1 {name=p12 analysis=tran voltage=1.116}
C {spice_probe.sym} 580 -520 0 1 {name=p13 analysis=tran voltage=2.244}
C {spice_probe.sym} 620 -280 0 1 {name=p14 analysis=tran voltage=0.0000e+00}
C {vsource.sym} 1060 -150 0 0 {name=V1 value=0}
C {gnd.sym} 1060 -120 0 0 {name=l2 lab=GND}
C {lab_pin.sym} 1060 -180 0 0 {name=l3 sig_type=std_logic lab=BAT0V}
C {vsource.sym} 940 -150 0 0 {name=V2 value=3}
C {gnd.sym} 940 -120 0 0 {name=l4 lab=GND}
C {lab_pin.sym} 940 -180 0 0 {name=l5 sig_type=std_logic lab=BAT3V}
C {spice_probe.sym} 490 -500 0 1 {name=p6 analysis=tran voltage=1.317}
C {spice_probe.sym} 730 -460 0 0 {name=p15 analysis=tran voltage=1.12}
C {spice_probe.sym} 1100 -420 0 0 {name=p16 analysis=tran voltage=2.99}
C {spice_probe.sym} 1100 -540 2 1 {name=p17 analysis=tran voltage=2.991}
C {lab_pin.sym} 800 -120 0 0 {name=l6 sig_type=std_logic lab=LAMP2}
C {lab_pin.sym} 800 -180 0 0 {name=l7 sig_type=std_logic lab=LAMP1}
C {res.sym} 800 -150 0 0 {name=Rlamp 
value=300
footprint=1206 
device=resistor 
m=1}
C {spice_probe.sym} 680 -540 0 0 {name=p8 analysis=tran voltage=2.718}
C {isource.sym} 440 -160 0 0 {name=I0 value="dc 0
+ pwl 0 0 
+ 1u 0 
+ 1.01u 1m
+ 2u 1m
+ 2.01u 0" }
C {lab_pin.sym} 440 -190 0 1 {name=l8 sig_type=std_logic lab=A1}
C {gnd.sym} 440 -70 0 0 {name=l9 lab=GND}
C {res.sym} 440 -100 0 1 {name=RS
value=1
footprint=1206 
device=resistor 
m=1}
C {lab_wire.sym} 400 -440 0 0 {name=l10 sig_type=std_logic lab=B1}
C {lab_wire.sym} 440 -470 0 1 {name=l11 sig_type=std_logic lab=C1}
C {lab_wire.sym} 620 -500 0 0 {name=l12 sig_type=std_logic lab=B2}
C {lab_wire.sym} 700 -460 0 0 {name=l13 sig_type=std_logic lab=C2}
C {lab_wire.sym} 660 -550 0 0 {name=l14 sig_type=std_logic lab=E2}
C {lab_wire.sym} 160 -440 0 0 {name=l15 sig_type=std_logic lab=IN}
C {lab_wire.sym} 910 -460 0 0 {name=l16 sig_type=std_logic lab=B3}
C {lab_wire.sym} 960 -390 0 0 {name=l18 sig_type=std_logic lab=E3}
