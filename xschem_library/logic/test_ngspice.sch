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
}
S {.model adc_buff adc_bridge(in_low = 0.3 in_high = 0.7)


.model nand d_nand(rise_delay = 0.5e-9 fall_delay = 0.3e-9
+ input_load = 5e-15)

.model dac_buff  dac_bridge(out_low = 0 out_high = 1.2 out_undef = 0.6
+ input_load = 5.0e-15 t_rise = 0.5e-9
+ t_fall = 0.2e-9)}
E {}
B 21 450 -380 810 -280 {}
T {XSPICE DOMAIN} 450 -420 0 0 0.6 0.6 {}
T {A --> D} 300 -280 0 0 0.6 0.6 {}
T {D --> A} 660 -230 0 0 0.6 0.6 {}
N 520 -350 550 -350 {lab=A}
N 520 -310 550 -310 {lab=B}
N 650 -330 680 -330 {lab=Y_NAND}
N 650 -330 650 -250 {lab=Y_NAND}
N 650 -250 680 -250 {lab=Y_NAND}
N 740 -250 770 -250 {lab=Y_NAND_A}
C {title.sym} 160 -30 0 0 {name=l2}
C {verilog_timescale.sym} 50 -320 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {lab_pin.sym} 380 -350 2 0 {name=p6 lab=A verilog_type=reg}
C {lab_pin.sym} 380 -310 2 0 {name=p7 lab=B verilog_type=reg}
C {use.sym} 50 -420 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {lab_pin.sym} 320 -350 2 1 {name=p47 lab=A_A verilog_type=reg}
C {lab_pin.sym} 320 -310 2 1 {name=p48 lab=B_A verilog_type=reg}
C {adc_bridge.sym} 350 -350 0 0 {name=a1 delay=1}
C {adc_bridge.sym} 350 -310 0 0 {name=a2 delay=1}
C {code.sym} 380 -210 0 0 {name=STIMULI
place=end
vhdl_ignore=true
verilog_ignore=true
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.include stimuli.cir

.control
tran 100n 26u
eprvcd A B Y_NAND  > zzzz.vcd
.endc
"}
C {lab_pin.sym} 520 -350 2 1 {name=p3 lab=A}
C {lab_pin.sym} 520 -310 2 1 {name=p4 lab=B}
C {lab_pin.sym} 680 -330 2 0 {name=p5 lab=Y_NAND}
C {nd2.sym} 590 -330 0 0 {name=a3 delay="120 ps" del=120}
C {dac_bridge.sym} 710 -250 0 0 {name=a4 }
C {lab_pin.sym} 770 -250 2 0 {name=p1 lab=Y_NAND_A}
C {/mnt/x/home/schippes/xschem-repo/trunk/xschem_library/devices/netlist_options.sym} 50 -480 0 0 {bus_replacement_char="xx"}
