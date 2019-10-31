v {xschem version=2.9.2 file_version=1.1}
G {
Y <= not ( A or B ) after delay ;}
V {assign #del Y = ~(A | B ) ;}
S {}
E {}
C {opin.sym} 490 -270 0 0 {name=p1 lab=Y verilog_type=wire}
C {ipin.sym} 250 -300 0 0 {name=p2 lab=A}
C {ipin.sym} 250 -270 0 0 {name=p3 lab=B}
C {use.sym} 560 -420 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {title.sym} 160 -30 0 0 {name=l2}
