v {xschem version=2.9.2 file_version=1.1}
G {
a: process( CK , RST )
begin
if ( RST = '1' ) then
  Q <= '0' after delay ;
elsif  CK'event AND CK = '1'  then 
  Q <= D after delay ;
end if ;
end process ;

 }
V {reg iQ;
always @(posedge CK or posedge RST) begin
  if(RST)    iQ=0;
  else     iQ=D;
end

assign #del Q=iQ;
}
S {}
E {}
C {devices/ipin.sym} 60 -150 0 0 {name=p1 lab=D}
C {devices/opin.sym} 130 -130 0 0 {name=p2 verilog_type=wire lab=Q}
C {devices/ipin.sym} 60 -130 0 0 {name=p3 lab=CK}
C {devices/ipin.sym} 60 -110 0 0 {name=p5 lab=RST}
C {devices/use.sym} 820 -190 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
--         use ieee.std_logic_arith.all;
--         use ieee.std_logic_unsigned.all;

-- library SYNOPSYS;
--         use SYNOPSYS.ATTRIBUTES.ALL;
}
C {devices/title.sym} 160 -30 0 0 {name=l2}
