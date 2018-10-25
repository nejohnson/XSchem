G {}
V {}
S {}
E {}
L 18 845 -530 880 -530 {}
L 18 845 -530 845 -450 {}
L 18 845 -450 880 -450 {}
L 18 880 -450 900 -400 {}
L 18 900 -400 910 -400 {}
L 18 910 -580 910 -400 {}
L 18 900 -580 910 -580 {}
L 18 880 -530 900 -580 {}
L 18 880 -530 880 -450 {}
L 18 900 -580 900 -400 {}
N 160 -1250 160 -1230 {lab=#net1}
N 160 -1110 160 -1090 {lab=#net2}
N 280 -1170 340 -1170 {lab=VSS}
N 710 -700 860 -700 {lab=OUTM}
N 510 -1200 580 -1200 {lab=VSS}
N 580 -1200 580 -1190 {lab=VSS}
N 580 -1190 620 -1190 {lab=VSS}
N 550 -1240 580 -1240 {lab=IN}
N 620 -1250 710 -1250 {lab=REFP}
N 280 -1180 280 -1170 {lab=VSS}
N 280 -1250 280 -1240 {lab=VPP}
N 270 -1100 270 -1090 {lab=VNN}
N 270 -1170 270 -1160 {lab=VSS}
N 160 -1090 190 -1090 {lab=#net2}
N 270 -1090 340 -1090 {lab=VNN}
N 160 -1250 190 -1250 {lab=#net1}
N 280 -1250 340 -1250 {lab=VPP}
N 270 -1170 280 -1170 {lab=VSS}
N 250 -1250 280 -1250 {lab=VPP}
N 250 -1090 270 -1090 {lab=VNN}
N 160 -1170 270 -1170 {lab=VSS}
N 550 -950 710 -950 {lab=OUTM}
N 400 -890 550 -890 {lab=FBN}
N 550 -770 550 -750 {lab=IN}
N 350 -890 350 -700 {lab=FBN}
N 710 -950 710 -700 {lab=OUTM}
N 710 -240 860 -240 {lab=OUTP}
N 260 -220 350 -220 {lab=INX}
N 550 -490 710 -490 {lab=OUTP}
N 400 -430 550 -430 {lab=FB}
N 350 -430 350 -240 {lab=FB}
N 710 -490 710 -240 {lab=OUTP}
N 240 -370 240 -350 {lab=VPP}
N 240 -290 240 -220 {lab=INX}
N 260 -220 260 -190 {lab=INX}
N 260 -130 260 -110 {lab=VSS}
N 510 -1120 580 -1120 {lab=VSS}
N 550 -1160 580 -1160 {lab=IN}
N 620 -1170 710 -1170 {lab=REFM}
N 200 -220 240 -220 {lab=INX}
N 550 -310 550 -290 {lab=VSS}
N 650 -700 710 -700 {lab=OUTM}
N 650 -240 710 -240 {lab=OUTP}
N 240 -220 260 -220 {lab=INX}
N 260 -680 350 -680 {lab=VSSX}
N 240 -830 240 -810 {lab=VPP}
N 240 -750 240 -680 {lab=VSSX}
N 260 -680 260 -650 {lab=VSSX}
N 260 -590 260 -570 {lab=VSS}
N 240 -680 260 -680 {lab=VSSX}
N 180 -680 240 -680 {lab=VSSX}
N 880 -1250 900 -1250 {lab=#net3}
N 880 -1250 880 -1220 {lab=#net3}
N 400 -1000 400 -980 {lab=VPP}
N 400 -920 400 -890 {lab=FBN}
N 400 -540 400 -520 {lab=VPP}
N 400 -460 400 -430 {lab=FB}
N 860 -700 860 -520 {lab=OUTM}
N 860 -460 860 -240 {lab=OUTP}
N 350 -890 400 -890 {lab=FBN}
N 350 -430 400 -430 {lab=FB}
N 580 -1110 620 -1110 {lab=VSS}
N 580 -1120 580 -1110 {lab=VSS}
C {devices/code} 1050 -540 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value=".option PARHIER=LOCAL RUNLVL=6 post MODMONTE=1 warn maxwarns=400
.option sampling_method = SRS 
.option method=gear
vvss vss 0 dc 0 
.temp 30

.param freq=20k
.param gain=48
.tran '0.2m/freq' '3m + 80/freq'
.FOUR 20k v(outm,outp)
.probe i(*) 
.probe p(r*) p(v*)
"}
C {devices/code} 1050 -690 0 0 {name=MODELS 
only_toplevel=true
value=".MODEL bd139_2 npn
+IS=1e-09 BF=222.664 NF=0.85 VAF=36.4079
+IKF=0.166126 ISE=5.03418e-09 NE=1.45313 BR=1.35467
+NR=1.33751 VAR=142.931 IKR=1.66126 ISC=5.02557e-09
+NC=3.10227 RB=26.9143 IRB=0.1 RBM=0.1
+RE=0.000472454 RC=1.04109 XTB=0.727762 XTI=1.04311
+EG=1.05 CJE=1e-11 VJE=0.75 MJE=0.33
+TF=1e-09 XTF=1 VTF=10 ITF=0.01
+CJC=1e-11 VJC=0.75 MJC=0.33 XCJC=0.9
+FC=0.5 CJS=0 VJS=0.75 MJS=0.5
+TR=1e-07 PTF=0 KF=0 AF=1


.MODEL BD139 NPN (
+IS=2.3985E-13 Vceo=80 Icrating=1500m
+BF=244.9 NF=1.0 BR=78.11 NR=1.007 ISE=1.0471E-14
+NE=1.2 ISC=1.9314E-11 NC=1.45 VAF=98.5 VAR=7.46
+IKF=1.1863 IKR=0.1445 RB=2.14 RBM=0.001 IRB=0.031
+RE=0.0832 RC=0.01 CJE=2.92702E-10 VJE=0.67412
+MJE=0.3300 FC=0.5 CJC=4.8831E-11 VJC=0.5258
+MJC=0.3928 XCJC=0.5287 XTB=1.1398 EG=1.2105 XTI=3.0) 


.MODEL BD140  PNP(IS=1e-09 BF=650.842 NF=0.85 VAF=10
+IKF=0.0950125 ISE=1e-08 NE=1.54571 BR=56.177
+NR=1.5 VAR=2.11267 IKR=0.950125 ISC=1e-08
+NC=3.58527 RB=41.7566 IRB=0.1 RBM=0.108893
+RE=0.000347052 RC=1.32566 XTB=19.5239 XTI=1
+EG=1.05 CJE=1e-11 VJE=0.75 MJE=0.33
+TF=1e-09 XTF=1 VTF=10 ITF=0.01
+CJC=1e-11 VJC=0.75 MJC=0.33 XCJC=0.9
+FC=0.5 CJS=0 VJS=0.75 MJS=0.5 TR=1e-07 PTF=0 KF=0 AF=10
+ VCEO=80 ICRATING=1A )


.MODEL Q2N2222A NPN IS =3.0611E-14 NF =1.00124 BF =220 IKF=0.52 
+              VAF=104 ISE=7.5E-15 NE =1.41 NR =1.005 BR =4 IKR=0.24 
+              VAR=28 ISC=1.06525E-11 NC =1.3728 RB =0.13 RE =0.22 
+              RC =0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 
+              CJE=27.01E-12 TF =0.325E-9 TR =100E-9
+              vce_max=45 vbe_max=6

.MODEL Q2N2222 NPN (
+ IS = 3.97589E-14 BF = 195.3412 NF = 1.0040078 VAF = 53.081 IKF = 0.976 ISE = 1.60241E-14
+ NE = 1.4791931 BR = 1.1107942 NR = 0.9928261 VAR = 11.3571702 IKR = 2.4993953 ISC = 1.88505E-12
+ NC = 1.1838278 RB = 56.5826472 IRB = 1.50459E-4 RBM = 5.2592283 RE = 0.0402974 RC = 0.4208
+ CJE = 2.56E-11 VJE = 0.682256 MJE = 0.3358856 TF = 3.3E-10 XTF = 6 VTF = 0.574
+ ITF = 0.32 PTF = 25.832 CJC = 1.40625E-11 VJC = 0.5417393 MJC = 0.4547893 XCJC = 1
+ TR = 3.2E-7 CJS = 0 VJS = .75 MJS = 0 XTB = 1.6486 EG = 1.11
+ XTI = 5.8315 KF = 0 AF = 1 FC = 0.83
+              vce_max=45 vbe_max=6 )


* 2n2222 p complementary
.MODEL Q2N2907P PNP(IS=650.6E-18 ISE=54.81E-15 ISC=0 XTI=3
+ BF=231.7 BR=3.563 IKF=1.079 IKR=0 XTB=1.5
+ VAF=115.7 VAR=35 VJE=0.65 VJC=0.65
+ RE=0.15 RC=0.715 RB=10
+ CJE=19.82E-12 CJC=14.76E-12 XCJC=0.75 FC=0.5
+ NF=1 NR=1 NE=1.829 NC=2 MJE=0.3357 MJC=0.5383
+ TF=603.7E-12 TR=111.3E-9 ITF=0.65 VTF=5 XTF=1.7
+ EG=1.11 KF=1E-9 AF=1
+ VCEO=45 ICRATING=100M 
+              vce_max=45 vbe_max=6 )

.MODEL Q2N2907S PNP(IS=2.32E-13 ISE=6.69E-16 ISC=1.65E-13 XTI=3.00
+ BF=3.08E2 BR=2.18E1 IKF=8.42E-1 IKR=1.00 XTB=1.5
+ VAF=1.41E2 VAR=1.35E1 VJE=3.49E-1 VJC=3.00E-1
+ RE=1.00E-2 RC=8.46E-1 RB=4.02E1 RBM=1.00E-2 IRB=1.25E-2
+ CJE=2.66E-11 CJC=1.93E-11 XCJC=1.00 FC=5.00E-1
+ NF=1.04 NR=1.12 NE=1.09 NC=1.13 MJE=4.60E-1 MJC=4.65E-1
+ TF=4.95E-10 TR=0 PTF=0 ITF=3.36E-1 VTF=6.54 XTF=1.87E1
+ EG=1.11 KF=1E-9 AF=1
+ VCEO=40 ICRATING=500M 
+              vce_max=45 vbe_max=6 )

*MM3725 MCE 5/13/95
*Si    1W  40V 500mA 307MHz pkg:TO-39 3,2,1
.MODEL QMM3725 NPN (IS=50.7F NF=1 BF=162 VAF=113 IKF=.45 ISE=38.2P NE=2
+ BR=4 NR=1 VAR=24 IKR=.675 RE=.263 RB=1.05 RC=.105 XTB=1.5
+ CJE=62.2P VJE=1.1 MJE=.5 CJC=14.6P VJC=.3 MJC=.3 TF=518P TR=45.9N)

*MPS651 MCE 5/12/95
*Si 625mW  60V    2A 110MHz pkg:TO-92 1,2,3
.MODEL QMPS651 NPN (IS=72.1F NF=1 BF=260 VAF=139 IKF=2.5 ISE=67.1P NE=2
+ BR=4 NR=1 VAR=20 IKR=3.75 RE=28.2M RB=.113 RC=11.3M XTB=1.5
+ CJE=212P VJE=1.1 MJE=.5 CJC=68.6P VJC=.3 MJC=.3 TF=1.44N TR=1U)

*FCX649 ZETEX Spice model   Last revision  17/7/90   Medium Power
*ZTX Si  1.5W  25V    2A 240MHz pkg:SOT-89 2,1,3
.MODEL QFCX649 NPN(IS=3E-13 BF=225 VAF=80 IKF=2.8 ISE=1.1E-13 NE=1.37 
+ BR=110 NR=0.972 VAR=28 IKR=0.8 ISC=6.5E-13 NC=1.372 RB=0.3 RE=0.063 
+ RC=0.07 CJE=3.25E-10 TF=1E-9 CJC=7E-11 TR=1E-8 )

*MPSW01A MCE 5/12/95
*Si    1W  40V    1A 210MHz pkg:TO-92 1,2,3
.MODEL QMPSW01A NPN (IS=18.1F NF=1 BF=273 VAF=113 IKF=.6 ISE=15.7P NE=2
+ BR=4 NR=1 VAR=20 IKR=.9 RE=96.5M RB=.386 RC=38.6M XTB=1.5
+ CJE=78.7P VJE=1.1 MJE=.5 CJC=17.3P VJC=.3 MJC=.3 TF=757P TR=526N)

*BC546 ZETEX Spice model     Last revision  4/90   General Purpose
*ZTX Si 500mW  65V 200mA 300MHz pkg:TO-92 1,2,3
.MODEL BC546 NPN(IS=1.8E-14 BF=400 NF=0.9955 VAF=80 IKF=0.14 ISE=5E-14 
+ NE=1.46 BR=35.5 NR=1.005 VAR=12.5 IKR=0.03 ISC=1.72E-13 NC=1.27 RB=0.56 
+ RE=0.6 RC=0.25 CJE=1.3E-11 TF=6.4E-10 CJC=4E-12 VJC=0.54 TR=5.072E-8 
+     vce_max=65 vbe_max=6 )


.MODEL BC556 PNP( 
+     IS=2.059E-14
+     NF=1.003
+     ISE=2.971E-15
+     NE=1.316
+     BF=227.3
+     IKF=0.08719
+     VAF=37.2
+     NR=1.007
+     ISC=1.339E-14
+     NC=1.15
+     BR=7.69
+     IKR=0.07646
+     VAR=11.42
+     RB=1
+     IRB=1E-06
+     RBM=1
+     RE=0.688
+     RC=0.6437
+     XTB=0
+     EG=1.11
+     XTI=3
+     CJE=1.4E-11
+     VJE=0.5912
+     MJE=0.3572
+     TF=7.046E-10
+     XTF=4.217
+     VTF=5.367
+     ITF=0.1947
+     PTF=0
+     CJC=1.113E-11
+     VJC=0.1
+     MJC=0.3414
+     XCJC=0.6288
+     TR=1E-32
+     CJS=0
+     VJS=0.75
+     MJS=0.333
+     FC=0.7947
+     vce_max=65 vbe_max=6 )
*

* NXP Semiconductors
*
* Medium power NPN transistor
* IC   = 1 A
* VCEO = 20 V 
* hFE  = 85 - 375 @ 2V/500mA
* 
*
*
*
* Package pinning does not match Spice model pinning.
* Package: SOT 223
* 
* Package Pin 1: Base  
* Package Pin 2: Collector
* Package Pin 3: Emitter
* Package Pin 4: Collector
* 
* Extraction date (week/year): 13/2008 
* Simulator: Spice 3 
*
**********************************************************
*#
.SUBCKT BCP68 1 2 3
* 
Q1 1 2 3 BCP68 
D1 2 1 DIODE
*
*The diode does not reflect a 
*physical device but improves 
*only modeling in the reverse 
*mode of operation.
*
.MODEL BCP68 NPN 
+ IS = 2.312E-013 
+ NF = 0.988 
+ ISE = 8.851E-014 
+ NE = 2.191 
+ BF = 273 
+ IKF = 5.5 
+ VAF = 50 
+ NR = 0.9885 
+ ISC = 6.808E-014 
+ NC = 3 
+ BR = 155.6 
+ IKR = 4 
+ VAR = 17 
+ RB = 15 
+ IRB = 2E-006 
+ RBM = 0.65 
+ RE = 0.073 
+ RC = 0.073 
+ XTB = 0 
+ EG = 1.11 
+ XTI = 3 
+ CJE = 2.678E-010 
+ VJE = 0.732 
+ MJE = 0.3484 
+ TF = 5.8E-010 
+ XTF = 1.5 
+ VTF = 2.5 
+ ITF = 1 
+ PTF = 0 
+ CJC = 3.406E-011 
+ VJC = 2 
+ MJC = 0.3142 
+ XCJC = 1 
+ TR = 6.5E-009 
+ CJS = 0 
+ VJS = 0.75 
+ MJS = 0.333 
+ FC = 0.95 
.MODEL DIODE D 
+ IS = 2.702E-015 
+ N = 1.2 
+ BV = 1000 
+ IBV = 0.001 
+ RS = 0.1 
+ CJO = 0 
+ VJ = 1 
+ M = 0.5 
+ FC = 0 
+ TT = 0 
+ EG = 1.11 
+ XTI = 3 
.ENDS

.model D1N4007 D(IS=7.02767n RS=0.0341512 N=1.80803 EG=1.05743 XTI=5 BV=1000 IBV=5e-08 CJO=1e-11 VJ=0.7 M=0.5 FC=0.5 TT=1e-07 )

.MODEL D1N4148 D 
+ IS = 4.352E-9 
+ N = 1.906 
+ BV = 110 
+ IBV = 0.0001 
+ RS = 0.6458 
+ CJO = 7.048E-13 
+ VJ = 0.869 
+ M = 0.03 
+ FC = 0.5 
+ TT = 3.48E-9 

.MODEL D1n5400 d
+IS=2.61339e-12 RS=0.0110501 N=1.20576 EG=0.6
+XTI=3.1271 BV=50 IBV=1e-05 CJO=1e-11
+VJ=0.7 M=0.5 FC=0.5 TT=1e-09
+KF=0 AF=1
* Model generated on October 12, 2003
* Model format: PSpice

*1N758
*Ref: Motorola
*10V 500mW Si Zener pkg:DIODE0.4 1,2
.MODEL D1N758 D(IS=1E-11 RS=8.483 N=1.27 TT=5E-8 CJO=2.334E-10 VJ=0.75 
+ M=0.33 BV=9.83 IBV=0.01 )

*1N4744
*Ref: Motorola
*15V 1W Si Zener pkg:DIODE0.4 1,2
.MODEL D1N4744 D(IS=5.32E-14 RS=6.47 TT=5.01E-8 CJO=7.83E-11 M=0.33 
+ BV=14.89 IBV=0.017 )

*1N755
*Ref: Motorola
*7.5V 500mW Si Zener pkg:DIODE0.4 1,2
.MODEL D1N755 D(IS=1E-11 RS=3.359 N=1.27 TT=5E-8 CJO=2.959E-10 VJ=0.75 
+ M=0.33 BV=7.433 IBV=0.01 )

** node order
* 1: gnd
* 2: trig
* 3: out
* 4: reset#
* 5: ctrl
* 6: thres
* 7: dis
* 8: vcc
.SUBCKT ne555  34 32 30 19 23 33 1  21
*              G  TR O  R  F  TH D  V
Q4 25 2 3 QP
Q5 34 6 3 QP
Q6 6 6 8 QP
R1 9 21 4.7K
R2 3 21 830
R3 8 21 4.7K
Q7 2 33 5 QN
Q8 2 5 17 QN
Q9 6 4 17 QN
Q10 6 23 4 QN
Q11 12 20 10 QP
R4 10 21 1K
Q12 22 11 12 QP
Q13 14 13 12 QP
Q14 34 32 11 QP
Q15 14 18 13 QP
R5 14 34 100K
R6 22 34 100K
R7 17 34 10K
Q16 1 15 34 QN
Q17 15 19 31 QP
R8 18 23 5K
R9 18 34 5K
R10 21 23 5K
Q18 27 20 21 QP
Q19 20 20 21 QP
R11 20 31 5K
D1 31 24 DA
Q20 24 25 34 QN
Q21 25 22 34 QN
Q22 27 24 34 QN
R12 25 27 4.7K
R13 21 29 6.8K
Q23 21 29 28 QN
Q24 29 27 16 QN
Q25 30 26 34 QN
Q26 21 28 30 QN
D2 30 29 DA
R14 16 15 100
R15 16 26 220
R16 16 34 4.7K
R17 28 30 3.9K
Q3 2 2 9 QP
.MODEL DA D (RS=40 IS=1.0E-14 CJO=1PF)
.MODEL QP PNP (level=1 BF=20 BR=0.02 RC=4 RB=25 IS=1.0E-14 VA=50 NE=2)
+ CJE=12.4P VJE=1.1 MJE=.5 CJC=4.02P VJC=.3 MJC=.3 TF=229P TR=159N)
.MODEL QN NPN (level=1 IS=5.07F NF=1 BF=100 VAF=161 IKF=30M ISE=3.9P NE=2
+ BR=4 NR=1 VAR=16 IKR=45M RE=1.03 RB=4.12 RC=.412 XTB=1.5
+ CJE=12.4P VJE=1.1 MJE=.5 CJC=4.02P VJC=.3 MJC=.3 TF=229P TR=959P)
.ENDS

.SUBCKT BS250P 3 4 5
*              D G S
M1 3 2 5 5 MBS250
RG 4 2 160
RL 3 5 1.2E8
C1 2 5 47E-12
C2 3 2 10E-12
D1 3 5 DBS250
*
.MODEL MBS250 PMOS VTO=-3.193 RS=2.041 RD=0.697 IS=1E-15 KP=0.277
+CBD=105E-12 PB=1 LAMBDA=1.2E-2
.MODEL DBS250 D IS=2E-13 RS=0.309
.ENDS BS250P

.SUBCKT BS170 3 4 5
*             D G S
M1 3 2 5 5 N3306M
RG 4 2 270
RL 3 5 1.2E8
C1 2 5 28E-12
C2 3 2 3E-12 
D1 5 3 N3306D
*
.MODEL N3306M NMOS VTO=1.824 RS=1.572 RD=1.436 IS=1E-15 KP=.1233
+CBD=35E-12 PB=1
.MODEL N3306D D IS=5E-12 RS=.768
.ENDS BS170
*

.SUBCKT irf540 1 2 3
**************************************
* Model Generated by MODPEX *
*Copyright(c) Symmetry Design Systems*
* All Rights Reserved *
* UNPUBLISHED LICENSED SOFTWARE *
* Contains Proprietary Information *
* Which is The Property of *
* SYMMETRY OR ITS LICENSORS *
*Commercial Use or Resale Restricted *
* by Symmetry License Agreement *
**************************************
* Model generated on Apr 24, 96
* Model format: SPICE3
* Symmetry POWER MOS Model (Version 1.0)
* External Node Designations
* Node 1 -> Drain
* Node 2 -> Gate
* Node 3 -> Source
M1 9 7 8 8 MM L=100u W=100u
* Default values used in MM:
* The voltage-dependent capacitances are
* not included. Other default values are:
* RS=0 RD=0 LD=0 CBD=0 CBS=0 CGBO=0
.MODEL MM NMOS LEVEL=1 IS=1e-32
+VTO=3.56362 LAMBDA=0.00291031 KP=25.0081
+CGSO=1.60584e-05 CGDO=4.25919e-07
RS 8 3 0.0317085
D1 3 1 MD
.MODEL MD D IS=1.02194e-10 RS=0.00968022 N=1.21527 BV=100
+IBV=0.00025 EG=1.2 XTI=3.03885 TT=1e-07
+CJO=1.81859e-09 VJ=1.1279 M=0.449161 FC=0.5
RDS 3 1 4e+06
RD 9 1 0.0135649
RG 2 7 5.11362
D2 4 5 MD1
* Default values used in MD1:
* RS=0 EG=1.11 XTI=3.0 TT=0
* BV=infinite IBV=1mA
.MODEL MD1 D IS=1e-32 N=50
+CJO=2.49697e-09 VJ=0.5 M=0.9 FC=1e-08
D3 0 5 MD2
* Default values used in MD2:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* BV=infinite IBV=1mA
.MODEL MD2 D IS=1e-10 N=0.4 RS=3e-06
RL 5 10 1
FI2 7 9 VFI2 -1
VFI2 4 0 0
EV16 10 0 9 7 1
CAP 11 10 2.49697e-09
FI1 7 9 VFI1 -1
VFI1 11 6 0
RCAP 6 10 1
D4 0 6 MD3
* Default values used in MD3:
* EG=1.11 XTI=3.0 TT=0 CJO=0
* RS=0 BV=infinite IBV=1mA
.MODEL MD3 D IS=1e-10 N=0.4
.ENDS




*$
*LM5134A
*****************************************************************************
* (C) Copyright 2016 Texas Instruments Incorporated. All rights reserved.
*****************************************************************************
** This model is designed as an aid for customers of Texas Instruments.
** TI and its licensors and suppliers make no warranties, either expressed
** or implied, with respect to this model, including the warranties of 
** merchantability or fitness for a particular purpose.  The model is
** provided solely on an \\"as is\\" basis.  The entire risk as to its quality
** and performance is with the customer.
*****************************************************************************
*
** Released by: WEBENCH Design Center,Texas Instruments Inc.
* Part: LM5134A
* Date: 23SEP2016
* Model Type: TRANSIENT
* Simulator: PSPICE
* Simulator Version: 16.2.0.p001
* EVM Order Number: NA
* EVM User's Guide: NA
* Datasheet: SNVS808CMAY 2012REVISED FEBRURARY 2016
*
* Model Version: Final 1.00
*
*****************************************************************************
*
* Updates:
*
* Final 1.00
* Release to Web
*
*****************************************************************************
*
* Model Usage Notes:
*
* A. Features have been modelled
*	1. Output timing characteristics
*	2. Output tr & tf vs VDD & CL Capacitor  
*	3. tD-ON, tPD-ON, tD-OFF & tPD-OFF vs VDD
*	4. Output peak current limit for VDD = 10V 
*	5. Input threshold for VDD = 10V
*	6. VDD UVLO Logic at TA = 25°C
*
* B. Features haven't been modelled
*	1. Supply Current vs Supply Voltage
*	2. Supply Current vs Load capacitance
*	3. Frequency dependent plots 
*	4. Temperature dependent characteristics
*
*****************************************************************************
.SUBCKT LM5134A VDD PILOT OUT IN INB VSS
E_E2         INB_INT 0 INB VSS 1
C_U3_C1         U3_N16789866 U3_N16789873  5p  
X_U3_U5         VDD U3_N16789873 U3_N16790218 0 RVAR  RREF=1
C_U3_C3         OUT U3_N16789866  1p  

* E_U3_E1         U3_N16790231 OUT vol=' V(MGATE, 0) > 0.5? 5: -5'
E_U3_E1         U3_N16790231 OUT pwl(1) MGATE 0 0.49 -5 0.51 5

X_U3_U6         U3_N24836 VSS U3_N31827 0 RVAR   RREF=1
R_U3_R1         U3_N16790231 U3_N16789866  20  
M_U3_M2         U3_N24836 U3_N16789871 OUT OUT PMOS01           
E_U3_E4         U3_N16790218 0 pwl(1) VDD_INT 0 
+ 0 0, 4.5 0.9, 10  0.09, 12.6 0.08   
X_U3_U10         VSS OUT d_d1 
R_U3_R2         U3_N16789868 U3_N16789871  20  
C_U3_C2         U3_N24836 U3_N16789871  5p  
X_U3_U9         OUT VDD d_d1 
C_U3_C5         OUT VDD  10p  
C_U3_C6         OUT U3_N16789871  10p  
C_U3_C4         VSS OUT  10p  
M_U3_M1         U3_N16789873 U3_N16789866 OUT OUT NMOS01           
E_U3_E3         U3_N31827 0 pwl(1) VDD_INT 0 
+    0 0, 4.5  0.26, 10  0.01, 12.6 0.01   
E_U3_E2         OUT U3_N16789868 vol= 'V(MGATE, 0) > 0.5? -5: 5'
E_U4_ABM4         U4_N14683241 0 vol= 'V(U4_ON_INT) >=0.5?V(VDD_INT):0'
C_U4_C3         U4_N14683221 0  1n  
V_U4_V6         U4_N155225261 0 80m
R_U4_R2         U4_N14683241 U4_N14683221  1 
X_U4_U47         U4_N14683247 U4_N14683251 d_d1 
E_U4_ABM5         U4_N14683301 0 vol= 'V(U4_ON_INT)<0.5? V(VDD_INT):0'
C_U4_C1         U4_N14683247 0  1n  
X_U4_S1    U4_N14683159 0 U4_N14683247 0 PTON_TOFF_U4_S1 
X_U4_U44         U4_ON_INT PGATE U4_N14683147 AND2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
X_U4_U43         U4_N14683247 PGATE BUF_BASIC_GEN VDD=1 VSS=0
+  VTHRESH=0.5
R_U4_R3         U4_N14683301 U4_N14683281  1 
X_U4_U7         N18232068 U4_N15541612 INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
G_U4_G1         U4_N14683251 U4_N14683247 pwl(1) U4_N14683221 0 
+    0 0, 4.5 140m, 10 500m, 12.6 600m   
G_U4_G2         U4_N14683247 0 pwl(1) U4_N14683281 0 
+    0 0, 4.5 70m, 10 800m, 12.6 900m 
X_U4_U8         MGATE U4_N15541600 INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
C_U4_C4         U4_N14683281 0  1n  
X_U4_S2    U4_N14683147 0 U4_N14683251 U4_N14683247 PTON_TOFF_U4_S2 
X_U4_U48         U4_N155225261 U4_N14683247 d_d1 
X_U4_U45         U4_N15541600 U4_N15541612 U4_ON_INT AND2_BASIC_GEN 
+  VDD=1 VSS=0 VTHRESH=500E-3
V_U4_V5         U4_N14683251 0 1V
X_U4_U46         PGATE U4_ON_INT U4_N14683159 NOR2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
E_E1         IN_INT 0 IN VSS 1
E_E3         VDD_INT 0 VDD VSS 1
C_U2_C1         U2_N14683247 0  1n  
X_U2_U46         MGATE U2_ON_INT U2_N14683159 NOR2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
X_U2_U7         PGATE U2_N15532894 INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
G_U2_G1         U2_N14683251 U2_N14683247 pwl(1)  U2_N14683221 0 
+    0 0,  4.5 58m,  10 140m,  12.6 230m   

E_U2_ABM5         U2_N14683301 0 vol= 'V(U2_ON_INT) <0.5? V(VDD_INT):0'

X_U2_U43         U2_N14683247 MGATE BUF_BASIC_GEN VDD=1 VSS=0
+  VTHRESH=0.5
X_U2_U48         U2_N147032561 U2_N14683247 d_d1 
V_U2_V6         U2_N147032561 0 80m
X_U2_S2    U2_N14683147 0 U2_N14683251 U2_N14683247 MTON_TOFF_U2_S2 

E_U2_ABM4         U2_N14683241 0 vol='V(U2_ON_INT) >=0.5? V(VDD_INT):0  '

R_U2_R2         U2_N14683241 U2_N14683221  1 
G_U2_G2         U2_N14683247 0 PWL(1) U2_N14683281 0 
+    0 0, 4.5 32m, 10 90m, 12.6 160m   
V_U2_V5         U2_N14683251 0 1V
X_U2_U44         U2_ON_INT MGATE U2_N14683147 AND2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
C_U2_C3         U2_N14683221 0  1n  
X_U2_U45         N18232068 U2_N15532894 U2_ON_INT AND2_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=500E-3
X_U2_S1    U2_N14683159 0 U2_N14683247 0 MTON_TOFF_U2_S1 
R_U2_R3         U2_N14683301 U2_N14683281  1 
C_U2_C4         U2_N14683281 0  1n  
X_U2_U47         U2_N14683247 U2_N14683251 d_d1 
X_U1_U6         INB_INT U1_VIH U1_VHYS U1_N15517298 COMPHYS_BASIC_GEN 
+  VDD=1 VSS=0 VTHRESH=0.5
R_U1_R1         U1_N15521766 U1_VDD_UVLO  721.5 
X_U1_U7         U1_N15517298 U1_INB_OUT INV_BASIC_GEN  VDD=1 VSS=0
+  VTHRESH=500E-3
X_U1_U9         U1_IN_OUT U1_INB_OUT U1_VDD_UVLO N18232068 AND3_BASIC_GEN
+   VDD=1 VSS=0 VTHRESH=500E-3
C_U1_C1         U1_VDD_UVLO 0  1n  
X_U1_U5         IN_INT U1_VIH U1_VHYS U1_IN_OUT COMPHYS_BASIC_GEN  VDD=1
+  VSS=0 VTHRESH=0.5
X_U1_U8         VDD_INT U1_N15521824 U1_N15521760 U1_N15521766
+  COMPHYS_BASIC_GEN  VDD=1 VSS=0 VTHRESH=0.5
V_U1_V1         U1_N15521824 0 3.6
E_U1_ABM3         U1_VHYS 0 vol= '0.34*V(VDD_INT)'
E_U1_ABM1         U1_VIH 0 vol='0.67*V(VDD_INT)'
V_U1_V2         U1_N15521760 0 0.36
X_U1_U35         U1_VDD_UVLO U1_N15521766 d_d1 
M_U5_M2         U5_N16789896 U5_N23038 PILOT PILOT PMOS02           
R_U5_R1         U5_N16790231 U5_N16789866  20  
X_U5_U6         U5_N16789896 VSS U5_N16802670 0 RVAR  RREF=1
M_U5_M1         U5_N08221 U5_N16789866 PILOT PILOT NMOS02           
C_U5_C4         VSS PILOT  10p  
C_U5_C2         U5_N16789896 U5_N23038  5p  
R_U5_R2         U5_N16789868 U5_N23038  20  
E_U5_E3         U5_N16802670 0 pwl(1) VDD_INT 0 
+    0 0, 4.5 2.8, 10 1.9, 12.6 1.6   
X_U5_U5         VDD U5_N08221 U5_N26349 0 RVAR RREF=1
C_U5_C1         U5_N08221 U5_N16789866  5p  
C_U5_C5         PILOT VDD  10p  
E_U5_E4         U5_N26349 0 pwl(1) VDD_INT 0 
+    0 0, 4.5 9.5, 10 2.88, 12.6 2.8   
X_U5_U9         PILOT VDD d_d1 
C_U5_C3         PILOT U5_N16789866  10p  
X_U5_U10         VSS PILOT d_d1 
E_U5_E2         PILOT U5_N16789868 vol= ' V(PGATE, 0) > 0.5? -5: 5'
E_U5_E1         U5_N16790231 PILOT vol= ' V(PGATE, 0) > 0.5? 5: -5'
C_U5_C6         PILOT U5_N23038  10p  
.ENDS LM5134A
*$
.SUBCKT PTON_TOFF_U4_S1 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U4_S1         1 2 1G
.ENDS PTON_TOFF_U4_S1
*$
.SUBCKT PTON_TOFF_U4_S2 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U4_S2         1 2 1G
.ENDS PTON_TOFF_U4_S2
*$
.SUBCKT MTON_TOFF_U2_S2 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U2_S2         1 2 1G
.ENDS MTON_TOFF_U2_S2
*$
.SUBCKT MTON_TOFF_U2_S1 1 2 3 4  
G_Switch 3 4 VCR PWL(1) 1 2 0.2 100e6 0.8 1m
RS_U2_S1         1 2 1G
.ENDS MTON_TOFF_U2_S1
*$
****************************** Basic Components **************************
.SUBCKT D_D1 1 2
D1 1 2 DD1
.MODEL DD1 D (IS=1e-15 Rs=0.001 N=0.1  TT=10p)
.ENDS D_D1
*$
.SUBCKT BUF_BASIC_GEN A  Y VDD=1 VSS=0 VTHRESH=0.5 
* E_ABMGATE    YINT 0 vol='V(A) > VTHRESH? VDD:VSS'
E_ABMGATE    YINT 0 pwl(1) A 0 'VTHRESH-0.01' VSS 'VTHRESH+0.01' VDD 
RINT YINT Y 1
CINT Y 0 1n
.ENDS BUF_BASIC_GEN
*$
.MODEL NMOS01 NMOS (VTO = 2 KP = 1.005 LAMBDA  = 0.001)
*$
.MODEL PMOS01 PMOS (VTO = -2 KP = 1.77 LAMBDA  = 0.001)
*$
.MODEL NMOS02 NMOS (VTO = 2 KP = 0.1814 LAMBDA = 0.001)
*$
.MODEL PMOS02 PMOS (VTO = -2 KP = 0.2497 LAMBDA = 0.001)
*$
.SUBCKT COMPHYS_BASIC_GEN INP INM HYS OUT VDD=1 VSS=0 VTHRESH=0.5	
EIN INP1 INM1 INP INM 1 
* EHYS INP1 INP2 vol='V(1) > VTHRESH? -V(HYS):0'
EHYS INP1 INP2 pwl(1) 1 0 'VTHRESH-0.01' 0 'VTHRESH+0.01' '-V(HYS)'

EOUT OUT 0 vol='V(INP2)>V(INM1)? VDD : VSS'
R1 OUT 1 1
C1 1 0 5n
RINP1 INP1 0 1K
.ENDS COMPHYS_BASIC_GEN
*$
.SUBCKT AND2_BASIC_GEN A B Y  VDD=1 VSS=0 VTHRESH=0.5 
E_ABMGATE    YINT 0 vol= 'V(A) > VTHRESH  && V(B) > VTHRESH ? VDD:VSS'
RINT YINT Y 1
CINT Y 0 1n
.ENDS AND2_BASIC_GEN
*$
.SUBCKT NOR2_BASIC_GEN A B Y  VDD=1 VSS=0 VTHRESH=0.5 
E_ABMGATE    YINT 0 vol= 'V(A) > VTHRESH  || V(B) > VTHRESH ? VSS:VDD'
RINT YINT Y 1
CINT Y 0 1n
.ENDS NOR2_BASIC_GEN
*$
.SUBCKT AND3_BASIC_GEN A B C Y VDD=1 VSS=0 VTHRESH=0.5 
E_ABMGATE    YINT 0 vol= 'V(A) > VTHRESH  &&  V(B) > VTHRESH && V(C) > VTHRESH ? VDD:VSS'
RINT YINT Y 1
CINT Y 0 1n
.ENDS AND3_BASIC_GEN
*$
.SUBCKT INV_BASIC_GEN A  Y VDD=1 VSS=0 VTHRESH=0.5 
* E_ABMGATE    YINT 0 vol='V(A) > VTHRESH ? VSS : VDD'
E_ABMGATE    YINT 0 pwl(1) A 0 'VTHRESH-0.01' VDD 'VTHRESH+0.01' VSS
RINT YINT Y 1
CINT Y 0 1n
.ENDS INV_BASIC_GEN
*$
.SUBCKT RVAR 101 102 201 202 RREF=1
* nodes : 101 102 : nodes between which variable resistance is placed
* 201 202 : nodes to whose voltage the resistance is proportional
* parameters : rref : reference value of the resistance
rin 201 202 1G   $$ input resistance
r 301 0 rref
fcopy 0 301 vsense 1 $$  copy output current thru Z
eout 101 106 poly(2) 201 202 301 0 0 0 0 0 1   $$ multiply VoverZ with Vctrl
vsense 106 102 0   $$ sense iout
.ENDS RVAR
*$


*LM317 TI voltage regulator - pin order: In, Adj, Out
*TI adjustable voltage regulator pkg:TO-3
.SUBCKT LM317 1 2 3 **Changes my be required on this line**
J1 1 3 4 JN
Q2 5 5 6 QPL .1
Q3 5 8 9 QNL .2
Q4 8 5 7 QPL .1
Q5 81 8 3 QNL .2
Q6 3 81 10 QPL .2
Q7 12 81 13 QNL .2
Q8 10 5 11 QPL .2
Q9 14 12 10 QPL .2
Q10 16 5 17 QPL .2
Q11 16 14 15 QNL .2
Q12 3 20 16 QPL .2
Q13 1 19 20 QNL .2
Q14 19 5 18 QPL .2
Q15 3 21 19 QPL .2
Q16 21 22 16 QPL .2
Q17 21 3 24 QNL .2
Q18 22 22 16 QPL .2
Q19 22 3 241 QNL 2
Q20 3 25 16 QPL .2
Q21 25 26 3 QNL .2
Q22A 35 35 1 QPL 2
Q22B 16 35 1 QPL 2
Q23 35 16 30 QNL 2
Q24A 27 40 29 QNL .2
Q24B 27 40 28 QNL .2
Q25 1 31 41 QNL 5
Q26 1 41 32 QNL 50
D1 3 4 DZ
D2 33 1 DZ
D3 29 34 DZ
R1 1 6 310
R2 1 7 310
R3 1 11 190
R4 1 17 82
R5 1 18 5.6K
R6 4 8 100K
R7 8 81 130
R8 10 12 12.4K
R9 9 3 180
R10 13 3 4.1K
R11 14 3 5.8K
R12 15 3 72
R13 20 3 5.1K
R14 2 24 12K
R15 24 241 2.4K
R16 16 25 6.7K
R17 16 40 12K
R18 30 41 130
R19 16 31 370
R20 26 27 13K
R21 27 40 400
R22 3 41 160
R23 33 34 18K
R24 28 29 160
R25 28 32 3
R26 32 3 .1
C1 21 3 30PF
C2 21 2 30PF
C3 25 26 5PF
CBS1 5 3 2PF
CBS2 35 3 1PF
CBS3 22 3 1PF
.MODEL JN NJF(BETA=1E-4 VTO=-7)
.MODEL DZ D(BV=6.3)
.MODEL QNL NPN(EG=1.22 BF=80 RB=100 CCS=1.5PF TF=.3NS TR=6NS CJE=2PF
+ CJC=1PF VAF=100)
.MODEL QPL PNP(BF=40 RB=20 TF=.6NS TR=10NS CJE=1.5PF CJC=1PF VAF=50)
.ENDS

.SUBCKT xxxxLM317 1 3 2
* IN ADJ OUT
IADJ 1 4 50U
VREF 4 3 1.25
RC 1 14 0.742
DBK 14 13 D1
CBC 13 15 2.479N
RBC 15 5 247
QP 13 5 2 Q1
RB2 6 5 124
DSC 6 11 D1
ESC 11 2 POLY(2) (13,5) (6,5) 2.85
+ 0 0 0 -70.1M
DFB 6 12 D1
EFB 12 2 POLY(2) (13,5) (6,5) 3.92
+ -135M 0 1.21M -70.1M
RB1 7 6 1
EB 7 2 8 2 2.56
CPZ 10 2 0.796U
DPU 10 2 D1
RZ 8 10 0.104
RP 9 8 100
EP 9 2 4 2 103.6
RI 2 4 100MEG
.MODEL Q1 NPN (IS=30F BF=100
+ VAF=14.27 NF=1.604)
.MODEL D1 D (IS=30F N=1.604)
.ENDS



.SUBCKT LM337 8 1 19
*Connections Input Adj. Output
*LM337 negative voltage regulator
.MODEL QN NPN (BF=50 TF=1N CJC=1P)
.MODEL QPOUT PNP (BF=50 TF=1N RE=.2 CJC=1P)
.MODEL QP PNP CJC=1P TF=2N
.MODEL DN D
.MODEL D2 D BV=12 IBV=100U
R10 25 6 1K
Q3 8 17 16 QPOUT
Q4 8 25 17 QP
R18 19 17 250
R19 19 16 .3
G1 8 6 1 18 .1
C7 6 2 .04U
R24 2 8 100
I_ADJ 0 1 65U
R26 8 25 200K
Q5 25 4 19 QP
R27 16 4 200
R28 7 4 7K
D1 8 7 D2
D2 8 6 DN
V1 18 19 1.25
.ENDS" tclcommand="xschem edit_vi_prop"}
C {devices/launcher} 1120 -800 0 0 {name=h2 
descr="BJT MODELS" 
url="http://www.zen22142.zen.co.uk/ltspice/standard.bjt"}
C {devices/launcher} 1120 -750 0 0 {name=h1 
descr="NPN MODELS" 
url="http://web.mit.edu/Magic/Public/ckt/npn.mod"}
C {devices/launcher} 1120 -850 0 0 {name=h3 
descr="COIL CALCULATION" 
url="http://hamwaves.com/antennas/inductance.html"}
C {devices/launcher} 1120 -900 0 0 {name=h4 
descr="ltwiki BJT MODELS" 
url="http://ltwiki.org/?title=Standard.bjt"}
C {devices/launcher} 1120 -400 0 0 {name=h5 
descr="hspice manual" 
program=evince
url="/home/schippes/hspice_2013/docs_help/hspice_sa.pdf"
}
C {devices/launcher} 1120 -350 0 0 {name=h6 
descr="hspice cmdref" 
program=evince
url="/home/schippes/hspice_2013/docs_help/hspice_cmdref.pdf"
}
C {devices/launcher} 1120 -300 0 0 {name=h7 
descr="hspice aasa" 
program=evince
url="/home/schippes/hspice_2013/docs_help/hspice_aasa.pdf"
}
C {devices/vsource} 160 -1200 0 0 {name=v1 value=50}
C {devices/vsource} 160 -1140 0 0 {name=v0 value=50}
C {devices/lab_pin} 340 -1250 0 1 {name=p5 lab=VPP}
C {devices/lab_pin} 340 -1090 0 1 {name=p6 lab=VNN}
C {devices/lab_pin} 340 -1170 0 1 {name=p3 lab=VSS}
C {devices/launcher} 1120 -950 0 0 {name=h8 
descr="DIODE LIB" 
url="/home/schippes/simulations/diode.lib"
}
C {devices/lab_pin} 860 -240 0 1 {name=p14 lab=OUTP}
C {devices/res} 860 -490 0 1 {name=r1 m=1 value=8}
C {devices/lab_pin} 510 -1200 0 0 {name=p26 lab=VSS}
C {devices/lab_pin} 550 -1240 0 0 {name=p31 lab=IN}
C {devices/vcvs} 620 -1220 0 0 {name=E3 value='gain*0.99'}
C {devices/lab_pin} 710 -1250 0 1 {name=p32 lab=REFP}
C {devices/capa} 280 -1210 0 0 {name=c2 m=1 value=100u}
C {devices/capa} 270 -1130 0 0 {name=c3 m=1 value=100u}
C {devices/res} 220 -1250 1 1 {name=r11 m=1 value=0.3}
C {devices/res} 220 -1090 1 1 {name=r9 m=1 value=0.3}
C {devices/res} 550 -920 0 1 {name=r19 m=1 value='100k'}
C {devices/res} 550 -860 0 1 {name=r0 m=1 value="'100k/gain'"}
C {devices/lab_pin} 550 -750 0 0 {name=p108 lab=IN}
C {examples/mos_power_ampli} 500 -660 0 0 {name=x1}
C {devices/lab_pin} 350 -640 0 0 {name=p2 lab=VPP}
C {devices/lab_pin} 350 -620 0 0 {name=p4 lab=VNN}
C {devices/lab_pin} 860 -700 0 1 {name=p9 lab=OUTM}
C {examples/mos_power_ampli} 500 -200 0 0 {name=x0}
C {devices/lab_pin} 350 -180 0 0 {name=p12 lab=VPP}
C {devices/lab_pin} 350 -160 0 0 {name=p13 lab=VNN}
C {devices/res} 240 -320 0 1 {name=r6 m=1 value=100k}
C {devices/lab_pin} 240 -370 0 0 {name=p7 lab=VPP}
C {devices/res} 260 -160 0 1 {name=r7 m=1 value=100k}
C {devices/lab_pin} 260 -110 0 0 {name=p15 lab=VSS}
C {devices/lab_pin} 510 -1120 0 0 {name=p20 lab=VSS}
C {devices/lab_pin} 550 -1160 0 0 {name=p21 lab=IN}
C {devices/vcvs} 620 -1140 0 0 {name=E0 value='-gain*0.99'}
C {devices/lab_pin} 710 -1170 0 1 {name=p23 lab=REFM}
C {devices/lab_pin} 240 -250 0 0 {name=p8 lab=INX}
C {devices/lab_pin} 880 -1100 0 0 {name=p126 lab=VSS}
C {devices/lab_pin} 960 -1250 0 1 {name=p127 lab=IN}
C {devices/capa} 550 -800 0 0 {name=c5 m=1 value=50u}
C {devices/lab_pin} 550 -290 0 0 {name=p11 lab=VSS}
C {devices/capa} 550 -340 0 0 {name=c6 m=1 value=50u}
C {devices/lab_pin} 350 -200 0 0 {name=p28 lab=VSS}
C {devices/lab_pin} 350 -660 0 0 {name=p1 lab=VSS}
C {devices/res} 550 -460 0 1 {name=r2 m=1 value='100k'}
C {devices/res} 550 -400 0 1 {name=r3 m=1 value="'100k/(gain-2)'"}
C {devices/vsource} 880 -1190 0 0 {name=v3 
xvalue="pulse -.1 .1 1m .1u .1u 10.1u 20u" 
value="sin 0 1 freq 1m"
}
C {devices/res} 240 -780 0 1 {name=r4 m=1 value=100k}
C {devices/lab_pin} 240 -830 0 0 {name=p18 lab=VPP}
C {devices/res} 260 -620 0 1 {name=r5 m=1 value=100k}
C {devices/lab_pin} 260 -570 0 0 {name=p10 lab=VSS}
C {devices/res} 400 -950 0 1 {name=r8 m=1 value=100k}
C {devices/capa} 170 -220 1 0 {name=c4 m=1 value=50u}
C {devices/lab_pin} 140 -220 0 0 {name=p0 lab=IN}
C {devices/capa} 150 -680 1 0 {name=c1 m=1 value=50u}
C {devices/lab_pin} 120 -680 0 0 {name=p17 lab=VSS}
C {devices/lab_pin} 240 -710 0 0 {name=p22 lab=VSSX}
C {devices/res} 930 -1250 1 1 {name=r10 m=1 value=200}
C {devices/lab_pin} 400 -1000 0 0 {name=p24 lab=VPP}
C {devices/res} 400 -490 0 1 {name=r13 m=1 value=100k}
C {devices/lab_pin} 400 -540 0 0 {name=p16 lab=VPP}
C {devices/launcher} 1120 -1000 0 0 {name=h9 
descr="REGULATORS LIB" 
url="http://ltwiki.org/files/LTspiceIV/lib/sub/regulators.lib"
program=firefox}
C {devices/lab_pin} 260 -110 0 0 {name=p30 lab=VSS}
C {devices/lab_pin} 260 -110 0 0 {name=p33 lab=VSS}
C {devices/vsource} 880 -1130 0 0 {name=v4 value=0 xvalue="pwl 0 .1 1m .1 1.01m 0"
}
C {devices/lab_pin} 350 -270 0 0 {name=p19 lab=FB}
C {devices/lab_pin} 350 -730 0 0 {name=p25 lab=FBN}
C {devices/title} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
