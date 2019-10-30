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
S {.model adc_buff adc_bridge(in_low = 0.3 in_high = 0.7)
}
E {}
C {devices/title.sym} 160 -30 0 0 {name=l2}
C {devices/verilog_timescale.sym} 40 -570 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {devices/lab_pin.sym} 220 -390 2 0 {name=p6 lab=A verilog_type=reg}
C {devices/lab_pin.sym} 220 -350 2 0 {name=p7 lab=B verilog_type=reg}
C {devices/use.sym} 40 -670 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {devices/lab_pin.sym} 220 -280 0 1 {name=p34 lab=ADD[4:0] verilog_type=reg}
C {devices/lab_pin.sym} 220 -320 0 1 {name=p35 lab=DIN[7:0] verilog_type=reg}
C {devices/lab_pin.sym} 220 -240 0 1 {name=p36 lab=WEN verilog_type=reg}
C {devices/lab_pin.sym} 220 -220 0 1 {name=p37 lab=OEN verilog_type=reg}
C {devices/lab_pin.sym} 220 -200 0 1 {name=p38 lab=CK verilog_type=reg}
C {devices/lab_pin.sym} 220 -260 0 1 {name=p39 lab=CEN verilog_type=reg}
C {devices/lab_pin.sym} 220 -300 0 1 {name=p40 lab=M[7:0] verilog_type=reg}
C {devices/lab_pin.sym} 160 -390 2 1 {name=p47 lab=A_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -350 2 1 {name=p48 lab=B_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -280 0 0 {name=p49 lab=ADD_A[4:0] verilog_type=reg}
C {devices/lab_pin.sym} 160 -320 0 0 {name=p50 lab=DIN_A[7:0] verilog_type=reg}
C {devices/lab_pin.sym} 160 -240 0 0 {name=p51 lab=WEN_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -220 0 0 {name=p52 lab=OEN_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -200 0 0 {name=p53 lab=CK_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -260 0 0 {name=p54 lab=CEN_A verilog_type=reg}
C {devices/lab_pin.sym} 160 -300 0 0 {name=p55 lab=M_A[7:0] verilog_type=reg}
C {devices/adc_bridge.sym} 190 -390 0 0 {name=a1 delay=1}
C {devices/adc_bridge.sym} 190 -350 0 0 {name=a2 delay=1}
C {devices/adc_bridge.sym} 190 -320 0 0 {name=a3[7:0] delay=1}
C {devices/adc_bridge.sym} 190 -300 0 0 {name=a4[7:0] delay=1}
C {devices/adc_bridge.sym} 190 -280 0 0 {name=a5[4:0] delay=1}
C {devices/adc_bridge.sym} 190 -260 0 0 {name=a6 delay=1}
C {devices/adc_bridge.sym} 190 -240 0 0 {name=a7 delay=1}
C {devices/adc_bridge.sym} 190 -220 0 0 {name=a8 delay=1}
C {devices/adc_bridge.sym} 190 -200 0 0 {name=a9 delay=1}
C {devices/code.sym} 380 -210 0 0 {name=STIMULI
place=end
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.include stimuli.cir

.control
tran 100n 6u
eprvcd A B din_5_ > zzzz.vcd
.endc
"}
