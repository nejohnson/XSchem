G {} 
V {// This is a transport (non inertial) delay
reg x;
always @(inp) x <= #del inp;
assign outp = x;

} 
S {} 
C {devices/ipin} 60 -50 0 0 {name=p1 lab=inp} 
C {devices/opin} 160 -50 0 0 {name=p2 lab=outp verilog_type=wire} 
