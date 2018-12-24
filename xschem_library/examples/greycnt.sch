v {xschem version=2.8.2_RC1 file_version=1.0}
G {}
V {}
S {}
E {}
T {BINARY} 500 -780 0 0 0.4 0.4 {}
T {GRAY} 830 -780 0 0 0.4 0.4 {}
T {BINARY} 1210 -780 0 0 0.4 0.4 {}
N 720 -630 830 -630 {lab=B[6]}
N 570 -610 620 -610 {lab=A[6]}
N 570 -530 620 -530 {lab=A[5]}
N 570 -450 620 -450 {lab=A[4]}
N 720 -550 830 -550 {lab=B[5]}
N 720 -470 830 -470 {lab=B[4]}
N 570 -690 630 -690 {lab=A[7]}
N 720 -390 830 -390 {lab=B[3]}
N 570 -370 620 -370 {lab=A[3]}
N 570 -290 620 -290 {lab=A[2]}
N 570 -210 620 -210 {lab=A[1]}
N 720 -310 830 -310 {lab=B[2]}
N 720 -230 830 -230 {lab=B[1]}
N 570 -130 620 -130 {lab=A[0]}
N 720 -150 830 -150 {lab=B[0]}
N 830 -130 1020 -130 {lab=B[0]}
N 830 -150 830 -130 {lab=B[0]}
N 830 -210 1020 -210 {lab=B[1]}
N 830 -230 830 -210 {lab=B[1]}
N 830 -290 1020 -290 {lab=B[2]}
N 830 -310 830 -290 {lab=B[2]}
N 830 -370 1020 -370 {lab=B[3]}
N 830 -390 830 -370 {lab=B[3]}
N 830 -450 1020 -450 {lab=B[4]}
N 830 -470 830 -450 {lab=B[4]}
N 830 -530 1020 -530 {lab=B[5]}
N 830 -550 830 -530 {lab=B[5]}
N 830 -610 1020 -610 {lab=B[6]}
N 830 -630 830 -610 {lab=B[6]}
N 690 -690 830 -690 {lab=B[7]}
N 830 -690 1030 -690 {lab=B[7]}
N 830 -710 830 -690 {lab=B[7]}
N 1120 -630 1230 -630 {lab=C[6]}
N 1120 -550 1230 -550 {lab=C[5]}
N 1120 -470 1230 -470 {lab=C[4]}
N 1120 -390 1230 -390 {lab=C[3]}
N 1120 -310 1230 -310 {lab=C[2]}
N 1120 -230 1230 -230 {lab=C[1]}
N 1120 -150 1230 -150 {lab=C[0]}
N 1090 -690 1130 -690 {lab=C[7]}
N 1130 -710 1130 -690 {lab=C[7]}
N 1130 -710 1230 -710 {lab=C[7]}
N 1120 -630 1120 -590 {lab=C[6]}
N 1020 -590 1120 -590 {lab=C[6]}
N 1020 -590 1020 -570 {lab=C[6]}
N 1120 -550 1120 -510 {lab=C[5]}
N 1020 -510 1120 -510 {lab=C[5]}
N 1020 -510 1020 -490 {lab=C[5]}
N 1120 -470 1120 -430 {lab=C[4]}
N 1020 -430 1120 -430 {lab=C[4]}
N 1020 -430 1020 -410 {lab=C[4]}
N 1120 -390 1120 -350 {lab=C[3]}
N 1020 -350 1120 -350 {lab=C[3]}
N 1020 -350 1020 -330 {lab=C[3]}
N 1120 -310 1120 -270 {lab=C[2]}
N 1020 -270 1120 -270 {lab=C[2]}
N 1020 -270 1020 -250 {lab=C[2]}
N 1120 -230 1120 -190 {lab=C[1]}
N 1020 -190 1120 -190 {lab=C[1]}
N 1020 -190 1020 -170 {lab=C[1]}
N 1130 -690 1130 -670 {lab=C[7]}
N 1020 -670 1130 -670 {lab=C[7]}
N 1020 -670 1020 -650 {lab=C[7]}
N 620 -690 620 -650 {lab=A[7]}
N 620 -610 620 -570 {lab=A[6]}
N 620 -530 620 -490 {lab=A[5]}
N 620 -450 620 -410 {lab=A[4]}
N 620 -370 620 -330 {lab=A[3]}
N 620 -290 620 -250 {lab=A[2]}
N 620 -210 620 -170 {lab=A[1]}
C {title} 160 -30 0 0 {name=l3 author="Stefan Schippers"}
C {verilog_timescale} 30 -110 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {xnor} 660 -470 0 0 {name=x2 risedel=1 falldel=1}
C {xnor} 660 -550 0 0 {name=x3 risedel=1 falldel=1}
C {xnor} 660 -630 0 0 {name=x14 risedel=1 falldel=1}
C {lab_pin} 570 -690 0 0 {name=p9 lab=A[7] }
C {lab_pin} 570 -610 0 0 {name=p1 lab=A[6] }
C {lab_pin} 570 -530 0 0 {name=p2 lab=A[5] }
C {lab_pin} 570 -450 0 0 {name=p3 lab=A[4] }
C {lab_pin} 830 -710 0 1 {name=p4 lab=B[7]}
C {lab_pin} 830 -630 0 1 {name=p0 lab=B[6]}
C {lab_pin} 830 -550 0 1 {name=p5 lab=B[5]}
C {lab_pin} 830 -470 0 1 {name=p6 lab=B[4]}
C {assign} 660 -690 0 0 {name=v1 delay=1}
C {xnor} 660 -230 0 0 {name=x1 risedel=1 falldel=1}
C {xnor} 660 -310 0 0 {name=x4 risedel=1 falldel=1}
C {xnor} 660 -390 0 0 {name=x5 risedel=1 falldel=1}
C {lab_pin} 570 -370 0 0 {name=p7 lab=A[3] }
C {lab_pin} 570 -290 0 0 {name=p8 lab=A[2] }
C {lab_pin} 570 -210 0 0 {name=p10 lab=A[1]}
C {lab_pin} 830 -390 0 1 {name=p11 lab=B[3]}
C {lab_pin} 830 -310 0 1 {name=p12 lab=B[2]}
C {lab_pin} 830 -230 0 1 {name=p13 lab=B[1]}
C {xnor} 660 -150 0 0 {name=x6 risedel=1 falldel=1}
C {lab_pin} 570 -130 0 0 {name=p14 lab=A[0]}
C {lab_pin} 830 -150 0 1 {name=p15 lab=B[0]}
C {opin} 150 -220 0 0 { name=p16 lab=B[7:0] }
C {lab_pin} 90 -200 0 0 { name=l17 lab=A[7:0]  verilog_type=reg}
C {xnor} 1060 -470 0 0 {name=x7 risedel=1 falldel=1}
C {xnor} 1060 -550 0 0 {name=x8 risedel=1 falldel=1}
C {xnor} 1060 -630 0 0 {name=x9 risedel=1 falldel=1}
C {assign} 1060 -690 0 0 {name=v0 delay=1}
C {xnor} 1060 -230 0 0 {name=x10 risedel=1 falldel=1}
C {xnor} 1060 -310 0 0 {name=x11 risedel=1 falldel=1}
C {xnor} 1060 -390 0 0 {name=x12 risedel=1 falldel=1}
C {xnor} 1060 -150 0 0 {name=x13 risedel=1 falldel=1}
C {lab_pin} 1230 -710 0 1 {name=p18 lab=C[7]}
C {lab_pin} 1230 -630 0 1 {name=p19 lab=C[6]}
C {lab_pin} 1230 -550 0 1 {name=p20 lab=C[5]}
C {lab_pin} 1230 -470 0 1 {name=p21 lab=C[4]}
C {lab_pin} 1230 -390 0 1 {name=p22 lab=C[3]}
C {lab_pin} 1230 -310 0 1 {name=p23 lab=C[2]}
C {lab_pin} 1230 -230 0 1 {name=p24 lab=C[1]}
C {lab_pin} 1230 -150 0 1 {name=p25 lab=C[0]}
C {opin} 150 -190 0 0 { name=p26 lab=C[7:0] }
C {code} 330 -250 0 0 {name=TESTBENCH only_toplevel=false value="initial begin
  $dumpfile(\\"dumpfile.vcd\\");
  $dumpvars;
  A=0;
end

always begin
  #1000;
  $display(\\"%08b %08b\\", A, B);
  A=A + 1;
  if(A==0) $finish;
end
"}
