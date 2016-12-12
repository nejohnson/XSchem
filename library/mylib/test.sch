G {} 
V {} 
S {} 
N 220 -460 260 -460 {lab=SET_BAR} 
N 220 -300 260 -300 {lab=CLEAR_BAR} 
N 360 -440 390 -440 {lab=Q} 
N 360 -320 390 -320 {lab=QBAR} 
N 260 -420 260 -400 {lab=QBAR} 
N 260 -400 360 -370 {lab=QBAR} 
N 360 -370 360 -320 {lab=QBAR} 
N 260 -360 260 -340 {lab=Q} 
N 260 -360 360 -390 {lab=Q} 
N 360 -440 360 -390 {lab=Q} 
C {mylib/nand2} 310 -440 0 0 {name=x1} 
C {devices/lab_pin} 390 -440 0 1 {name=p1 lab=Q} 
C {devices/lab_pin} 220 -460 0 0 {name=p2 lab=SET_BAR} 
C {devices/lab_pin} 220 -300 0 0 {name=p3 lab=CLEAR_BAR} 
C {mylib/nand2} 310 -320 2 1 {name=x2} 
C {devices/lab_pin} 390 -320 0 1 {name=p4 lab=QBAR} 
