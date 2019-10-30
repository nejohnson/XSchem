v {xschem version=2.9.2 file_version=1.1}
G {process
begin
A<='0';
B<='0';
wait for 1 ns;
A<='1';
wait for 1 ns;
B<='1';
wait for 1 ns;
A<='0';
wait for 1 ns;
B<='0';
wait for 1 ns;
B<= '1'; 
wait for 1 ns;
B<= '0'; 
A<= '0'; 
wait for 1 ns;
B<= '1'; 
wait for 1 ns;
A <='1';
wait for 20 ns;
A <= '0';
wait for 1 ns;
A<='Z';
B<='Z';
wait;
end process;

process 
variable n : integer := 0;
begin
  if n = 0 then 
    CK <= '0';
    n := n + 1;
  end if;
  if n = 16 then
    wait;
  end if;
  n := n + 1;
  wait for 5 ns;
  CK <= not CK;
end process;

process begin
wait for 2 ns;
WEN <='0';
CEN <='0';
OEN <='1';
M <= x"00";

ADD <="00000";
DIN <=x"11";
wait for 10 ns;
ADD <="00001";
DIN <=x"22";
wait for 10 ns;
ADD <="00010";
DIN <=x"33";
wait for 10 ns;
ADD <="00011";
DIN <=x"44";
wait for 10 ns;
WEN <='1';
OEN <='0';

ADD<="00000";
wait for 10 ns;
ADD<="00001";
wait for 10 ns;
ADD<="00010";
wait for 10 ns;
ADD<="00011";
wait for 10 ns;
wait;
end process;
}
V {integer n = 0;

initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars;
  A=0;
  B=0;
  #1000;
  A=1;
  #1000;
  B=1;
  #1000;
  A=0;
  #1000;
  B=0;
  #1000;
  B=1;
  #1000;
  B=0;
  A=0;
  #1000;
  B=1;
  #1000;
  A=1;
  #20000;
  A=0;
end

always begin
  if(n ==0 ) CK = 0;
  if(n == 16) $finish;
  n = n + 1;
  #5000;
  CK = !CK;
end

initial begin
#2000;
WEN=0;
CEN=0;
OEN=1;
M ='h00;

ADD=0;
DIN='h11;
#10000;
ADD=1;
DIN='h22;
#10000;
ADD=2;
DIN='h33;
#10000;
ADD=3;
DIN='h44;
#10000;
WEN=1;
OEN=0;

ADD=0;
#10000;
ADD=1;
#10000;
ADD=2;
#10000;
ADD=3;
#10000;
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
N 780 -90 810 -90 {lab=B}
N 890 -90 920 -90 {lab=BN}
C {devices/title.sym} 160 -30 0 0 {name=l2}
C {logic/nr2.sym} 510 -330 0 0 {name=x1 }
C {devices/lab_pin.sym} 440 -350 2 1 {name=p20 lab=A}
C {devices/lab_pin.sym} 440 -310 2 1 {name=p1 lab=B}
C {devices/lab_pin.sym} 600 -330 2 0 {name=p2 lab=Y_NOR}
C {logic/nd2.sym} 510 -440 0 0 {name=x2 }
C {devices/lab_pin.sym} 440 -460 2 1 {name=p3 lab=A}
C {devices/lab_pin.sym} 440 -420 2 1 {name=p4 lab=B}
C {devices/lab_pin.sym} 600 -440 2 0 {name=p5 lab=Y_NAND}
C {devices/verilog_timescale.sym} 40 -570 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {devices/lab_pin.sym} 220 -390 2 0 {name=p6 lab=A verilog_type=reg}
C {devices/lab_pin.sym} 220 -350 2 0 {name=p7 lab=B verilog_type=reg}
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
C {logic/ff.sym} 870 -470 0 0 {name=x6}
C {devices/lab_pin.sym} 800 -490 2 1 {name=p15 lab=A}
C {devices/lab_pin.sym} 800 -450 2 1 {name=p16 lab=B}
C {devices/lab_pin.sym} 870 -420 2 1 {name=p17 lab=Y_NOR}
C {devices/lab_pin.sym} 940 -490 2 0 {name=p18 lab=Y_FF}
C {logic/latch.sym} 870 -250 0 0 {name=x7 del=200}
C {devices/lab_pin.sym} 800 -270 2 1 {name=p19 lab=A}
C {devices/lab_pin.sym} 800 -230 2 1 {name=p21 lab=B}
C {devices/lab_pin.sym} 870 -200 2 1 {name=p22 lab=Y_NOR}
C {devices/lab_pin.sym} 940 -270 2 0 {name=p23 lab=Y_LATCH}
C {devices/lab_pin.sym} 940 -230 2 0 {name=p24 lab=YN_LATCH}
C {devices/use.sym} 40 -670 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {logic/ram.sym} 840 -670 0 0 {name=xcoderam   dim=5 width=8 hex=0 datafile=ram.list}
C {devices/lab_pin.sym} 950 -750 0 1 {name=p25 lab=DOUT[7:0]}
C {devices/lab_pin.sym} 730 -710 0 0 {name=p26 lab=ADD[4:0]}
C {devices/lab_pin.sym} 730 -750 0 0 {name=p27 lab=DIN[7:0]}
C {devices/lab_pin.sym} 730 -670 0 0 {name=p28 lab=WEN}
C {devices/lab_pin.sym} 730 -650 0 0 {name=p29 lab=OEN}
C {devices/lab_pin.sym} 730 -630 0 0 {name=p30 lab=CK}
C {devices/lab_pin.sym} 730 -690 0 0 {name=p31 lab=CEN}
C {devices/lab_pin.sym} 730 -730 0 0 {name=p32 lab=M[7:0]}
C {devices/lab_pin.sym} 220 -280 0 1 {name=p34 lab=ADD[4:0] verilog_type=reg}
C {devices/lab_pin.sym} 220 -320 0 1 {name=p35 lab=DIN[7:0] verilog_type=reg}
C {devices/lab_pin.sym} 220 -240 0 1 {name=p36 lab=WEN verilog_type=reg}
C {devices/lab_pin.sym} 220 -220 0 1 {name=p37 lab=OEN verilog_type=reg}
C {devices/lab_pin.sym} 220 -200 0 1 {name=p38 lab=CK verilog_type=reg}
C {devices/lab_pin.sym} 220 -260 0 1 {name=p39 lab=CEN verilog_type=reg}
C {devices/lab_pin.sym} 220 -300 0 1 {name=p40 lab=M[7:0] verilog_type=reg}
C {logic/sync_reg.sym} 840 -850 0 0 {name=x8 width=8}
C {devices/lab_pin.sym} 740 -880 0 0 {name=p33 lab=DIN[7:0]}
C {devices/lab_pin.sym} 740 -820 0 0 {name=p41 lab=CK}
C {devices/lab_pin.sym} 740 -840 0 0 {name=p42 lab=BN}
C {devices/lab_pin.sym} 740 -860 2 1 {name=p43 lab=A}
C {devices/lab_pin.sym} 940 -880 0 1 {name=p44 lab=DATA_OUT[7:0]}
C {logic/iv.sym} 850 -90 0 0 {name=x9 }
C {devices/lab_pin.sym} 780 -90 2 1 {name=p45 lab=B}
C {devices/lab_pin.sym} 920 -90 2 0 {name=p46 lab=BN}
C {devices/lab_pin.sym} 160 -390 2 1 {name=p47 lab=A_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -350 2 1 {name=p48 lab=B_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -280 0 0 {name=p49 lab=ADD_A[4:0] verilog_type=reg}
C {devices/lab_pin.sym} 160 -320 0 0 {name=p50 lab=DIN_A[7:0] verilog_type=reg}
C {devices/lab_pin.sym} 160 -240 0 0 {name=p51 lab=WEN_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -220 0 0 {name=p52 lab=OEN_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -200 0 0 {name=p53 lab=CK_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -260 0 0 {name=p54 lab=CEN_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -300 0 0 {name=p55 lab=M_A[7:0] verilog_type=reg}
C {devices/adc_bridge.sym} 190 -390 0 0 {name=v1 delay=1}
C {devices/adc_bridge.sym} 190 -350 0 0 {name=v2 delay=1}
C {devices/adc_bridge.sym} 190 -320 0 0 {name=v3 delay=1}
C {devices/adc_bridge.sym} 190 -300 0 0 {name=v4 delay=1}
C {devices/adc_bridge.sym} 190 -280 0 0 {name=v5 delay=1}
C {devices/adc_bridge.sym} 190 -260 0 0 {name=v6 delay=1}
C {devices/adc_bridge.sym} 190 -240 0 0 {name=v7 delay=1}
C {devices/adc_bridge.sym} 190 -220 0 0 {name=v8 delay=1}
C {devices/adc_bridge.sym} 190 -200 0 0 {name=v9 delay=1}