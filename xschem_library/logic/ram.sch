v {xschem version=2.9.2 file_version=1.1}
G {

    process(data, CEN, OEN) begin
      if CEN='0' and OEN='0' then
        DOUT <= data after 3 ns;
      else
        DOUT <= (others=>'Z') after 3 ns;
      end if;
    end process;

    READWRITE:
    process (CK) begin
       if(CK'event and CK = '1') then
         if (CEN = '0' and WEN = '0') then
           mem(to_integer(unsigned(ADD))) <= DIN;
           data <= DIN;
         end if;
         if (CEN = '0' and WEN = '1') then
           data <= mem(to_integer(unsigned(ADD))) ;
         end if;
       end if;
    end process;
}
V {
integer i;
reg [width-1:0] mem[0:(1<<dim)-1] ;
reg [width-1:0] idata;
wire [width-1:0] iidata;


// initial begin
//   $display("filling code ram");
//   if(hex==1) $readmemh(datafile, mem);
//   else       $readmemb(datafile, mem);
// end

always @(posedge CK) begin : writeread
  reg [width-1:0] tmp;
  if(!CEN && !WEN) begin
    tmp = (DIN & ~M) | (mem[ADD] & M);
    mem[ADD] <= tmp;
    idata <= tmp; //write thru option
    $display("%s write: addr=%h data=%h time=%t",modulename, ADD,tmp, $time);
  end
  if(!CEN && WEN) begin
      idata <= mem[ADD];
      $display("%s read: addr=%h, data=%h, time=%t", modulename, ADD, mem[ADD], $time);
  end
end

assign iidata = idata;

assign #3000 DOUT = OEN ? 'bz: iidata;
//always@(DOUT) $display("code ram2 read: addr=%h, data=%h, time=%t", ADD, DOUT, $time);



}
S {}
E {}
L 4 290 -560 290 -260 {}
L 4 210 -560 210 -260 {}
L 4 450 -560 450 -260 {}
L 4 370 -560 370 -260 {}
L 4 610 -560 610 -260 {}
L 4 530 -560 530 -260 {}
L 4 770 -560 770 -260 {}
L 4 690 -560 690 -260 {}
L 15 170 -430 840 -430 {}
L 15 170 -410 840 -410 {}
L 15 210 -430 215 -410 {}
L 15 210 -410 215 -430 {}
L 15 170 -470 210 -470 {}
L 15 210 -490 210 -470 {}
L 15 210 -490 250 -490 {}
L 15 250 -490 250 -470 {}
L 15 250 -470 290 -470 {}
L 15 290 -490 290 -470 {}
L 15 290 -490 330 -490 {}
L 15 330 -490 330 -470 {}
L 15 330 -470 370 -470 {}
L 15 370 -490 370 -470 {}
L 15 370 -490 410 -490 {}
L 15 410 -490 410 -470 {}
L 15 410 -470 450 -470 {}
L 15 450 -490 450 -470 {}
L 15 450 -490 490 -490 {}
L 15 490 -490 490 -470 {}
L 15 490 -470 530 -470 {}
L 15 530 -490 530 -470 {}
L 15 530 -490 570 -490 {}
L 15 570 -490 570 -470 {}
L 15 570 -470 610 -470 {}
L 15 610 -490 610 -470 {}
L 15 610 -490 650 -490 {}
L 15 650 -490 650 -470 {}
L 15 650 -470 690 -470 {}
L 15 690 -490 690 -470 {}
L 15 690 -490 730 -490 {}
L 15 730 -490 730 -470 {}
L 15 730 -470 770 -470 {}
L 15 770 -490 770 -470 {}
L 15 770 -490 810 -490 {}
L 15 810 -490 810 -470 {}
L 15 810 -470 840 -470 {}
L 15 290 -430 295 -410 {}
L 15 290 -410 295 -430 {}
L 15 170 -370 840 -370 {}
L 15 170 -350 840 -350 {}
L 15 370 -370 375 -350 {}
L 15 370 -350 375 -370 {}
L 15 290 -370 295 -350 {}
L 15 290 -350 295 -370 {}
T {add0} 230 -425 0 0 0.3 0.3 {}
T {data0} 310 -365 0 0 0.3 0.3 {}
T {prech} 215 -550 0 0 0.18 0.3 {}
T {sense} 260 -550 0 0 0.18 0.3 {}
C {devices/opin.sym} 340 -220 0 0 {name=p10 lab=DOUT[width-1:0] verilog_type=wire}
C {devices/ipin.sym} 200 -220 0 0 {name=p8 lab=DIN[width-1:0]}
C {devices/ipin.sym} 200 -110 0 0 {name=p12 lab=CK}
C {devices/ipin.sym} 200 -140 0 0 {name=p1 lab=OEN}
C {devices/ipin.sym} 200 -260 0 0 {name=p3 lab=ADD[dim-1:0]}
C {devices/ipin.sym} 200 -190 0 0 {name=p2 lab=WEN}
C {devices/ipin.sym} 200 -170 0 0 {name=p4 lab=CEN}
C {devices/ipin.sym} 200 -240 0 0 {name=p5 lab=M[width-1:0]}
C {devices/verilog_timescale.sym} 710 -197.5 0 0 {name=s1 timestep="1ps" precision="1ps" }
C {devices/title.sym} 160 -30 0 0 {name=l2}
C {devices/use.sym} 360 -120 0 0 {------------------------------------------------
library ieee;
        use ieee.std_logic_1164.all;
        use ieee.numeric_std.all;}
C {devices/arch_declarations.sym} 750 -630 0 0 {    constant RAM_DEPTH :integer := 2**dim;


    type RAM is array (integer range <>)of std_logic_vector (width-1 downto 0);
    signal mem : RAM (0 to RAM_DEPTH-1);
    signal data : std_logic_vector(width-1 downto 0);
}
