G {} 
V {} 
S {} 
T {NE555 TIMER IC EXAMPLE} 360 -490 0 0 0.4 0.4 {} 
N 130 -150 200 -150 {lab=TRIG} 
C {devices/netlist_not_shown} 710 -190 0 0 {
name=STIMULI 
value=".option SCALE=1e-6 PARHIER=LOCAL RUNLVL=6 post MODMONTE=1
.option sampling_method = SRS
* .option method=gear
vvss vss 0 dc 0

.temp 30
.tran 1n 1m uic"} 
C {devices/netlist_not_shown} 710 -340 0 0 {
name=MODELS 
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
+IS=2.3985E-13 mfg=Fairchild Vceo=80 Icrating=1500m
+BF=244.9 NF=1.0 BR=78.11 NR=1.007 ISE=1.0471E-14
+NE=1.2 ISC=1.9314E-11 NC=1.45 VAF=98.5 VAR=7.46
+IKF=1.1863 IKR=0.1445 RB=2.14 RBM=0.001 IRB=0.031
+RE=0.0832 RC=0.01 CJE=2.92702E-10 VJE=0.67412
+MJE=0.3300 FC=0.5 CJC=4.8831E-11 VJC=0.5258
+MJC=0.3928 XCJC=0.5287 XTB=1.1398 EG=1.2105 XTI=3.0) 

* sot-32
.MODEL bd237 npn
+IS=3.39253e-12 BF=108.002 NF=0.85 VAF=54.0829
+IKF=1.0435 ISE=6.43245e-12 NE=3.3968 BR=4.46493
+NR=0.843007 VAR=4.13248 IKR=3.42156 ISC=3.89999e-13
+NC=3.99 RB=8.81471 IRB=0.1 RBM=0.1
+RE=0.00101545 RC=0.282939 XTB=0.1 XTI=1
+EG=1.206 CJE=6.77107e-08 VJE=0.559947 MJE=0.503582
+TF=1e-08 XTF=1.35725 VTF=0.996034 ITF=0.999921
+CJC=4.44295e-10 VJC=0.400232 MJC=0.41007 XCJC=0.803125
+FC=0.598714 CJS=0 VJS=0.75 MJS=0.5
+TR=1e-07 PTF=0 KF=0 AF=1

.MODEL Q2N2222A NPN IS =3.0611E-14 NF =1.00124 BF =220 IKF=0.52 
+              VAF=104 ISE=7.5E-15 NE =1.41 NR =1.005 BR =4 IKR=0.24 
+              VAR=28 ISC=1.06525E-11 NC =1.3728 RB =0.13 RE =0.22 
+              RC =0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 
+              CJE=27.01E-12 TF =0.325E-9 TR =100E-9

.MODEL Q2N2222 NPN      (
+         IS = 3.97589E-14
+         BF = 195.3412
+         NF = 1.0040078
+        VAF = 53.081
+        IKF = 0.976
+        ISE = 1.60241E-14
+         NE = 1.4791931
+         BR = 1.1107942
+         NR = 0.9928261
+        VAR = 11.3571702
+        IKR = 2.4993953
+        ISC = 1.88505E-12
+         NC = 1.1838278
+         RB = 56.5826472
+        IRB = 1.50459E-4
+        RBM = 5.2592283
+         RE = 0.0402974
+         RC = 0.4208
+        CJE = 2.56E-11
+        VJE = 0.682256
+        MJE = 0.3358856
+         TF = 3.3E-10
+        XTF = 6
+        VTF = 0.574
+        ITF = 0.32
+        PTF = 25.832
+        CJC = 1.40625E-11
+        VJC = 0.5417393
+        MJC = 0.4547893
+       XCJC = 1
+         TR = 3.2E-7
+        CJS = 0
+        VJS = .75
+        MJS = 0
+        XTB = 1.6486
+         EG = 1.11
+        XTI = 5.8315
+         KF = 0
+         AF = 1
+         FC = 0.83
+ )

.model D1N4007 D(IS=7.02767n RS=0.0341512 N=1.80803 EG=1.05743 XTI=5 BV=1000 IBV=5e-08 CJO=1e-11 VJ=0.7 M=0.5 FC=0.5 TT=1e-07 mfg=OnSemi type=silicon)


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

** param order
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
.ENDS"} 
C {examples/ne555} 490 -220 0 0 {name=x1} 
C {devices/res} 130 -180 0 0 {name=r3 m=1 value=2k} 
C {devices/capa} 130 -120 0 0 {name=c6 m=1 value=2n} 
C {devices/vsource} 130 -360 0 1 {name=v1 value=5} 
C {devices/lab_pin} 130 -330 0 0 {name=p16 lab=VSS} 
C {devices/lab_pin} 130 -390 0 1 {name=p6 lab=VSUPPLY} 
C {devices/lab_pin} 570 -340 0 1 {name=p8 lab=VSUPPLY} 
C {devices/lab_pin} 410 -340 0 0 {name=p9 lab=VSS} 
C {devices/lab_pin} 410 -260 0 0 {name=p11 lab=TRIG} 
C {devices/lab_pin} 410 -180 0 0 {name=p12 lab=OUT} 
C {devices/lab_pin} 570 -180 0 1 {name=p15 lab=TRIG} 
C {devices/lab_pin} 200 -150 0 1 {name=p19 lab=TRIG} 
C {devices/lab_pin} 130 -270 0 0 {name=p20 lab=VSUPPLY} 
C {devices/lab_pin} 130 -90 0 0 {name=p21 lab=VSS} 
C {devices/lab_pin} 570 -100 0 1 {name=p14 lab=CTRL} 
C {devices/lab_pin} 410 -100 0 0 {name=p13 lab=VSUPPLY} 
C {devices/lab_pin} 570 -260 0 1 {name=p17 lab=DIS} 
C {devices/res} 130 -240 0 0 {name=r5 m=1 value=2k} 
C {devices/lab_pin} 130 -210 0 0 {name=p18 lab=DIS} 
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"} 
