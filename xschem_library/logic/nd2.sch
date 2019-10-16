v {xschem version=2.9.2 file_version=1.1}
G {
Y <= A nand B after delay ;}
V {assign #del Y = ~(A & B);
}
S {}
E {}
C {devices/opin.sym} 690 -320 0 0 {name=p1 lab=Y verilog_type=wire}
C {devices/ipin.sym} 150 -320 0 0 {name=p2 lab=A}
C {devices/ipin.sym} 150 -370 0 0 {name=p3 lab=B}
C {devices/use.sym} 640 -570 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {devices/title.sym} 160 -30 0 0 {name=l2}
