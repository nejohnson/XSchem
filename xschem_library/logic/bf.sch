v {xschem version=2.9.2 file_version=1.1}
G {
Y <= A after delay ;}
V {assign #del Y=A;
}
S {}
E {}
C {devices/opin.sym} 670 -290 0 0 {name=p0 lab=Y}
C {devices/ipin.sym} 250 -290 0 0 {name=p3 lab=A}
C {devices/use.sym} 670 -450 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {devices/title.sym} 160 -30 0 0 {name=l2}
