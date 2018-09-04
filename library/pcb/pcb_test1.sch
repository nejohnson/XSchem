G {} 
V {} 
S {} 
E {} 
B 20 270 -550 860 -290 {} 
T {3 of 4 NANDS of a 74ls00} 500 -580 0 0 0.4 0.4 {} 
T {EXPERIMENTAL schematic for generating a tEDAx netlist
1) set netlist mode to 'tEDAx' (Options menu -> tEDAx netlist)
2) press 'Netlist' button on the right
3) resulting netlist is in pcb_test1.tdx } 230 -760 0 0 0.5 0.5 {} 
N 230 -330 300 -330 {lab=INPUT_B} 
N 230 -370 300 -370 {lab=INPUT_A} 
N 680 -420 750 -420 {lab=B} 
N 680 -460 750 -460 {lab=A} 
N 400 -350 440 -350 {lab=B} 
N 850 -440 890 -440 {lab=OUTPUT_Y} 
N 230 -440 300 -440 {lab=INPUT_F} 
N 230 -480 300 -480 {lab=INPUT_E} 
N 400 -460 440 -460 {lab=A} 
N 550 -190 670 -190 {lab=VCCFILT} 
N 590 -130 590 -110 {lab=ANALOG_GND} 
N 790 -190 940 -190 {lab=VCC5} 
N 890 -130 890 -110 {lab=ANALOG_GND} 
N 730 -110 890 -110 {lab=ANALOG_GND} 
N 730 -160 730 -110 {lab=ANALOG_GND} 
N 590 -110 730 -110 {lab=ANALOG_GND} 
N 440 -460 680 -460 {lab=A} 
N 500 -420 680 -420 {lab=B} 
N 500 -420 500 -350 {lab=B} 
N 440 -350 500 -350 {lab=B} 
C {devices/title} 160 -30 0 0 {name=l2 author="Stefan"} 
C {pcb/74ls00} 340 -350 0 0 {name=U1:2  risedel=100 falldel=200} 
C {pcb/74ls00} 790 -440 0 0 {name=U1:1  risedel=100 falldel=200} 
C {devices/lab_pin} 890 -440 0 1 {name=p0 lab=OUTPUT_Y} 
C {pcb/capa} 590 -160 0 0 {name=C0 m=1 value=100u device="electrolitic capacitor"} 
C {pcb/74ls00} 340 -460 0 0 {name=U1:4 risedel=100 falldel=200 power=VCC5} 
C {pcb/7805} 730 -190 0 0 {name=U0} 
C {devices/lab_pin} 490 -190 0 0 {name=p20 lab=VCC12} 
C {devices/lab_pin} 940 -190 0 1 {name=p22 lab=VCC5} 
C {devices/lab_pin} 590 -110 0 0 {name=p23 lab=ANALOG_GND} 
C {pcb/capa} 890 -160 0 0 {name=C4 m=1 value=10u device="tantalium capacitor"} 
C {pcb/res} 520 -190 1 0 {name=R0 m=1 value=4.7 device="carbon resistor"} 
C {devices/lab_wire} 620 -460 0 0 {name=l3 lab=A} 
C {devices/lab_wire} 620 -420 0 0 {name=l0 lab=B} 
C {devices/lab_wire} 650 -190 0 0 {name=l1 lab=VCCFILT} 
C {pcb/connector} 230 -370 0 0 {name=CONN1 lab=INPUT_A} 
C {pcb/connector} 230 -330 0 0 {name=CONN2 lab=INPUT_B} 
C {pcb/connector} 240 -160 0 0 { name=CONN3 lab=OUTPUT_Y } 
C {pcb/connector} 230 -480 0 0 {name=CONN6 lab=INPUT_E} 
C {pcb/connector} 230 -440 0 0 {name=CONN8 lab=INPUT_F} 
C {pcb/connector} 240 -140 0 0 { name=CONN9 lab=VCC12 } 
C {pcb/connector} 240 -120 0 0 { name=CONN14 lab=ANALOG_GND } 
C {pcb/connector} 240 -100 0 0 { name=CONN15 lab=GND } 
