v {xschem version=2.9.5 file_version=1.1}
G {}
V {}
S {}
E {}
N 170 -170 170 -110 {lab=nLC}
N 170 -270 170 -230 {lab=nRL}
N 40 -270 170 -270 {lab=nRL}
N 40 -270 40 -250 {lab=nRL}
N 40 -190 40 -130 {lab=#net1}
N 170 -70 170 -50 {lab=#net2}
N 40 -50 170 -50 {lab=#net2}
N 40 -70 40 -50 {lab=#net2}
C {spice/r.sym} 40 -220 0 0 {name=R1 value=10}
C {spice/c.sym} 170 -90 0 0 {name=C1 value=1u}
C {spice/l.sym} 170 -200 0 0 {name=L1 value=1m}
C {spice/v.sym} 40 -100 0 0 {name=V1 value=1V}
C {lab_wire.sym} 90 -270 0 0 {name=l2 sig_type=std_logic lab=nRL}
C {lab_wire.sym} 170 -140 0 0 {name=l3 sig_type=std_logic lab=nLC}
C {code_shown.sym} 280 -230 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="
.tran 10n 1m uic
.save all
.control
run
set color0=white
set color1=black
set xbrushwidth=2
plot nRL nLC
.endc
"}
C {spice/gnd.sym} 40 -50 0 0 {name=l1 lab=0}
