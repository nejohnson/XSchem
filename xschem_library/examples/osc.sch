v {xschem version=2.8.2_RC1 file_version=1.0}
G {}
V {}
S {}
E {}
T {COLPITTS OSCILLATOR
EXAMPLE} 460 -480 0 0 0.4 0.4 {}
N 240 -270 340 -270 {lab=B}
N 380 -300 690 -300 {lab=OUT}
N 250 -460 380 -460 {lab=VCC}
N 380 -100 380 -80 {lab=VSS}
N 380 -100 550 -100 {lab=VSS}
N 380 -340 380 -300 {lab=OUT}
N 380 -240 380 -220 {lab=E}
N 380 -220 380 -160 {lab=E}
N 550 -240 550 -200 {lab=E}
N 550 -140 550 -100 {lab=VSS}
N 380 -210 400 -210 {lab=E}
N 400 -210 550 -210 {lab=E}
C {code} 750 -190 0 0 {
name=STIMULI 
value=".option RUNLVL=6 post 
vvss vss 0 dc 0

.save all
.temp 30
.tran 10n 100u 
.probe tran i1(Q1) i2(Q1) i3(Q1) i(L1) i(c1) i(c2) i(r1) i(r2) i(r3)
"}
C {npn} 360 -270 0 0 {name=Q1 model=q2n2222a area=1
}
C {vsource} 250 -430 0 1 {name=v2 value="5 pwl 0 5 1n 6 2n 6 3n 5"}
C {lab_pin} 250 -400 0 0 {name=p0 lab=VSS}
C {lab_pin} 690 -300 0 1 {name=p3 lab=OUT}
C {lab_pin} 380 -80 0 0 {name=p5 lab=VSS}
C {lab_pin} 250 -460 0 0 {name=p6 lab=VCC}
C {ind} 380 -430 2 0 {name=L1 value=100u}
C {capa} 550 -270 0 1 {name=c1 m=1 value=100p}
C {lab_pin} 380 -230 0 0 {name=p2 lab=E}
C {vsource} 240 -240 0 1 {name=v1 value=1}
C {lab_pin} 240 -210 0 0 {name=p1 lab=VSS}
C {lab_pin} 240 -270 0 0 {name=p4 lab=B}
C {res} 380 -130 0 1 {name=r1 m=1 value=1k}
C {res} 380 -370 0 1 {name=r2 m=1 value=2}
C {capa} 550 -170 0 1 {name=c2 m=1 value=100p}
C {code} 750 -340 0 0 {name=MODELS 
only_toplevel=true
value=".MODEL Q2N2222A NPN IS =3.0611E-14 NF =1.00124 BF =220 IKF=0.52 
+              VAF=104 ISE=7.5E-15 NE =1.41 NR =1.005 BR =4 IKR=0.24 
+              VAR=28 ISC=1.06525E-11 NC =1.3728 RB =0.13 RE =0.22 
+              RC =0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 
+              CJE=27.01E-12 TF =0.325E-9 TR =100E-9
+              vce_max=45 vbe_max=6

"}
