v {xschem version=2.9.2 file_version=1.1}
G {process
begin
A<='0';
B<='0';
wait for 100 ns;
A<='1';
wait for 100 ns;
B<='1';
wait for 100 ns;
A<='0';
wait for 100 ns;
B<='0';
wait for 100 ns;
B<= '1'; 
wait for 100 ns;
B<= '0'; 
A<= '0'; 
wait for 100 ns;
B<= '1'; 
wait for 100 ns;
A<='Z';
B<='Z';
wait;
end process;
}
V {initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars;
  A=0;
  B=0;
  #100000;
  A=1;
  #100000;
  B=1;
  #100000;
  A=0;
  #100000;
  B=0;
  #100000;
  B=1;
  #100000;
  B=0;
  A=0;
  #100000;
  B=1;
  #100000;
  
end
}
S {}
E {}
N 440 -350 470 -350 {lab=A}
N 440 -310 470 -310 {lab=B}
N 570 -330 600 -330 {lab=Y_NOR}
N 440 -460 470 -460 {lab=A}
N 440 -420 470 -420 {lab=B}
N 570 -440 600 -440 {lab=Y_NAND}
N 440 -230 470 -230 {lab=A}
N 550 -230 580 -230 {lab=Y_INV}
N 580 -130 610 -130 {lab=Y_XOR}
N 450 -150 480 -150 {lab=A}
N 450 -110 480 -110 {lab=B}
N 810 -360 840 -360 {lab=A}
N 920 -360 950 -360 {lab=Y_BUF}
C {devices/title.sym} 160 -30 0 0 {name=l2}
C {logic/nr2.sym} 510 -330 0 0 {name=x1 }
C {devices/lab_pin.sym} 440 -350 2 1 {name=p20 lab=A}
C {devices/lab_pin.sym} 440 -310 2 1 {name=p1 lab=B}
C {devices/lab_pin.sym} 600 -330 2 0 {name=p2 lab=Y_NOR}
C {devices/use.sym} 370 -610 0 0 {library ieee;
use ieee.std_logic_1164.all;



}
C {logic/nd2.sym} 510 -440 0 0 {name=x2 }
C {devices/lab_pin.sym} 440 -460 2 1 {name=p3 lab=A}
C {devices/lab_pin.sym} 440 -420 2 1 {name=p4 lab=B}
C {devices/lab_pin.sym} 600 -440 2 0 {name=p5 lab=Y_NAND}
C {devices/verilog_timescale.sym} 70 -200 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {devices/lab_pin.sym} 260 -430 2 1 {name=p6 lab=A verilog_type=reg}
C {devices/lab_pin.sym} 260 -390 2 1 {name=p7 lab=B verilog_type=reg}
C {logic/iv.sym} 510 -230 0 0 {name=x3 }
C {devices/lab_pin.sym} 440 -230 2 1 {name=p8 lab=A}
C {devices/lab_pin.sym} 580 -230 2 0 {name=p9 lab=Y_INV}
C {logic/eo.sym} 520 -130 0 0 {name=x4}
C {devices/lab_pin.sym} 610 -130 2 0 {name=p10 lab=Y_XOR}
C {devices/lab_pin.sym} 450 -150 2 1 {name=p11 lab=A}
C {devices/lab_pin.sym} 450 -110 2 1 {name=p12 lab=B}
C {logic/bf.sym} 880 -360 0 0 {name=x5}
C {devices/lab_pin.sym} 810 -360 2 1 {name=p13 lab=A}
C {devices/lab_pin.sym} 950 -360 2 0 {name=p14 lab=Y_BUF}
C {logic/ff.sym} 840 -520 0 0 {name=x6}
C {devices/lab_pin.sym} 770 -540 2 1 {name=p15 lab=A}
C {devices/lab_pin.sym} 770 -500 2 1 {name=p16 lab=B}
C {devices/lab_pin.sym} 840 -470 2 1 {name=p17 lab=Y_NOR}
C {devices/lab_pin.sym} 910 -540 2 0 {name=p18 lab=Y_FF}
