v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
N 660 -190 660 -170 { lab=0}
N 440 -220 620 -220 { lab=GN1}
N 400 -270 400 -250 { lab=GN1}
N 400 -350 400 -330 { lab=VCC}
N 400 -190 400 -170 { lab=0}
N 580 -280 740 -280 { lab=#net1}
N 660 -280 660 -250 { lab=#net1}
N 580 -380 580 -340 { lab=#net2}
N 740 -380 740 -340 { lab=#net3}
N 620 -450 620 -420 { lab=#net2}
N 580 -420 620 -420 { lab=#net2}
N 580 -420 580 -380 { lab=#net2}
N 580 -500 580 -480 { lab=VCC}
N 940 -330 1060 -330 { lab=OUT}
N 940 -500 940 -440 { lab=VCC}
N 740 -410 900 -410 { lab=#net3}
N 400 -250 440 -250 { lab=GN1}
N 440 -250 440 -220 { lab=GN1}
N 740 -500 740 -480 { lab=VCC}
N 620 -450 710 -450 { lab=#net2}
N 940 -190 940 -170 { lab=0}
N 940 -380 940 -250 { lab=OUT}
N 620 -220 620 -210 { lab=GN1}
N 620 -210 730 -210 { lab=GN1}
N 730 -220 730 -210 { lab=GN1}
N 730 -220 900 -220 { lab=GN1}
N 740 -420 740 -380 { lab=#net3}
N 790 -410 790 -340 { lab=#net3}
N 910 -340 940 -340 { lab=OUT}
C {nmos4.sym} 640 -220 0 0 {name=M1 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 660 -220 0 1 {name=p2 lab=0}
C {lab_pin.sym} 660 -170 0 0 {name=p6 lab=0}
C {nmos4.sym} 420 -220 0 1 {name=M2 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 400 -220 0 0 {name=p7 lab=0}
C {isource.sym} 400 -300 0 0 {name=I0 value=30u}
C {lab_pin.sym} 400 -350 0 0 {name=p9 lab=VCC}
C {lab_pin.sym} 400 -170 0 0 {name=p16 lab=0}
C {nmos4.sym} 560 -310 0 0 {name=M3 model=nmos w=1.5u l=0.2u m=1}
C {lab_pin.sym} 580 -310 0 1 {name=p17 lab=0}
C {nmos4.sym} 760 -310 0 1 {name=M4 model=nmos w=1.5u l=0.2u m=1}
C {lab_pin.sym} 740 -310 0 0 {name=p18 lab=0 l=0.2u}
C {pmos4.sym} 600 -450 0 1 {name=M5 model=pmos w=6u l=0.3u m=1}
C {lab_pin.sym} 580 -500 0 0 {name=p19 lab=VCC}
C {pmos4.sym} 720 -450 0 0 {name=M6 model=pmos w=6u l=0.3u m=1}
C {lab_pin.sym} 740 -500 0 0 {name=p21 lab=VCC}
C {lab_pin.sym} 580 -450 0 0 {name=p23 lab=VCC}
C {lab_pin.sym} 740 -450 0 1 {name=p33 lab=VCC}
C {lab_wire.sym} 570 -220 0 0 {name=l2 lab=GN1}
C {ipin.sym} 100 -310 0 0 {name=p161 lab=PLUS}
C {ipin.sym} 100 -260 0 0 {name=p1 lab=MINUS}
C {opin.sym} 180 -290 0 0 {name=p20 lab=OUT}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 780 -310 0 1 {name=p3 lab=PLUS}
C {lab_pin.sym} 540 -310 0 0 {name=p4 lab=MINUS}
C {lab_pin.sym} 1060 -330 0 1 {name=p14 lab=OUT}
C {pmos4.sym} 920 -410 0 0 {name=M8 model=pmos w=6u l=0.4u m=1}
C {lab_pin.sym} 940 -410 0 1 {name=p15 lab=VCC}
C {lab_pin.sym} 940 -500 0 0 {name=p22 lab=VCC}
C {spice_probe.sym} 490 -220 0 0 {name=p27 attrs=""}
C {spice_probe.sym} 640 -450 0 0 {name=p28 attrs=""}
C {nmos4.sym} 920 -220 0 0 {name=M7 model=nmos w=4u l=0.4u m=1}
C {lab_pin.sym} 940 -220 0 1 {name=p5 lab=0}
C {lab_pin.sym} 940 -170 0 1 {name=p8 lab=0}
C {res.sym} 880 -340 1 0 {name=R1
value=5k
footprint=1206
device=resistor
m=1}
C {capa.sym} 820 -340 3 0 {name=C1
m=1
value=150f
footprint=1206
device="ceramic capacitor"}
C {parax_cap.sym} 460 -210 0 0 {name=C2 gnd=0 value=200f m=1}
C {spice_probe.sym} 630 -280 0 0 {name=p10 attrs=""}
