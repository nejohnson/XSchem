G {} 
V {} 
S {} 
N 160 -280 240 -280 {lab=#net1} 
N 300 -280 400 -280 {lab=#net2} 
C {devices/ipin} 100 -150 0 0 {name=p0 lab=D} 
C {devices/opin} 400 -150 0 0 {name=p2 lab=Q} 
C {devices/ipin} 100 -100 0 0 {name=p4 lab=VSS} 
C {devices/switch} 270 -280 1 0 {name=G5 TABLE="'VTH-0.1' 10G 'VTH+0.1' 10"} 
C {devices/capa} 300 -250 0 0 {name=c1 m=1 value=1p} 
C {devices/lab_pin} 250 -320 0 0 {name=p6 lab=VSS} 
C {devices/lab_pin} 300 -220 0 1 {name=p7 lab=VSS} 
C {devices/ipin} 100 -130 0 0 {name=p1 lab=G} 
C {devices/lab_pin} 270 -320 0 1 {name=p3 lab=G} 
C {examples/buf} 120 -280 0 0 {name=E5 TABLE="'VTH-0.1' 0 'VTH+0.1' VHI"} 
C {examples/buf} 440 -280 0 0 {name=E1 TABLE="'VTH-0.1' 0 'VTH+0.1' VHI"} 
C {devices/lab_pin} 120 -250 0 0 {name=p5 lab=VSS} 
C {devices/lab_pin} 440 -250 0 0 {name=p8 lab=VSS} 
C {devices/lab_pin} 480 -280 0 1 {name=p9 lab=Q} 
C {devices/lab_pin} 80 -280 0 0 {name=p10 lab=D} 
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"} 
