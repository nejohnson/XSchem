v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {

initial begin
  $dumpfile("dumpfile.vcd");
  $dumpvars;
  A=0;
end

integer i = 0;
always begin
  i = i + 1;
  #100000;
  $display("time= %t: A= %08b   Y= %08b", $time, A, Y);
  A=~A;
  if(i==20) $finish;
end}
S {va a 0 pwl 0 0 100n 0 101n 3
vvcc vcc 0 dc 3
vvss vss 0 dc 0

.tran 1n 200n}
E {}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {ipin.sym} 210 -220 0 0 {name=p1 lab=A}
C {ipin.sym} 210 -190 0 0 {name=p3 lab=B}
C {opin.sym} 520 -240 0 0 {name=p4 lab=Z}
C {ipin.sym} 210 -260 0 0 {name=p5 lab=RST}
C {ipin.sym} 210 -300 0 0 {name=p7 lab=CK}
C {noconn.sym} 210 -300 2 0 {name=l2}
C {noconn.sym} 210 -260 2 0 {name=l3}
C {noconn.sym} 210 -220 2 0 {name=l4}
C {noconn.sym} 210 -190 2 0 {name=l5}
C {noconn.sym} 520 -240 2 1 {name=l6}
