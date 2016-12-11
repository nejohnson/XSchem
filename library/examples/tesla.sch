G {} 
V {} 
S {} 
T {PARASITIC
CAP} 1020 -510 0 0 0.25 0.2 {} 
T {TESLA COIL HIGH VOLTAGE GENERATOR EXAMPLE} 420 -740 0 0 0.4 0.4 {} 
N 60 -550 60 -500 {lab=VCCS} 
N 60 -640 60 -610 {lab=VCC} 
N 150 -640 150 -500 {lab=VCC} 
N 870 -530 1010 -530 {lab=HV} 
N 1010 -420 1010 -250 {lab=VSS} 
N 1010 -530 1010 -480 {lab=HV} 
N 870 -340 870 -140 {lab=FB} 
N 870 -580 870 -530 {lab=HV} 
N 860 -580 870 -580 {lab=HV} 
N 845 -590 860 -580 {lab=HV} 
N 840 -600 845 -590 {lab=HV} 
N 840 -610 840 -600 {lab=HV} 
N 840 -620 845 -630 {lab=HV} 
N 845 -630 860 -640 {lab=HV} 
N 870 -640 880 -640 {lab=HV} 
N 870 -580 880 -580 {lab=HV} 
N 880 -580 895 -590 {lab=HV} 
N 895 -590 900 -600 {lab=HV} 
N 900 -610 900 -600 {lab=HV} 
N 895 -630 900 -620 {lab=HV} 
N 880 -640 895 -630 {lab=HV} 
N 840 -610 860 -600 {lab=HV} 
N 860 -600 880 -600 {lab=HV} 
N 880 -600 900 -610 {lab=HV} 
N 870 -640 880 -620 {lab=HV} 
N 880 -600 880 -590 {lab=HV} 
N 870 -580 880 -590 {lab=HV} 
N 860 -620 870 -640 {lab=HV} 
N 860 -600 860 -590 {lab=HV} 
N 860 -590 870 -580 {lab=HV} 
N 840 -620 860 -610 {lab=HV} 
N 860 -610 880 -610 {lab=HV} 
N 880 -610 900 -620 {lab=HV} 
N 845 -630 860 -620 {lab=HV} 
N 860 -620 880 -620 {lab=HV} 
N 880 -620 895 -630 {lab=HV} 
N 840 -600 860 -590 {lab=HV} 
N 860 -590 880 -590 {lab=HV} 
N 880 -590 900 -600 {lab=HV} 
N 895 -630 907.5 -642.5 {lab=HV} 
N 907.5 -642.5 907.5 -637.5 {lab=HV} 
N 907.5 -637.5 930 -660 {lab=HV} 
N 922.5 -655 930 -660 {lab=HV} 
N 925 -652.5 930 -660 {lab=HV} 
N 832.5 -642.5 845 -630 {lab=HV} 
N 832.5 -642.5 832.5 -637.5 {lab=HV} 
N 810 -660 832.5 -637.5 {lab=HV} 
N 810 -660 817.5 -655 {lab=HV} 
N 810 -660 815 -652.5 {lab=HV} 
N 832.5 -577.5 845 -590 {lab=HV} 
N 832.5 -582.5 832.5 -577.5 {lab=HV} 
N 810 -560 832.5 -582.5 {lab=HV} 
N 810 -560 817.5 -565 {lab=HV} 
N 810 -560 815 -567.5 {lab=HV} 
N 895 -590 907.5 -577.5 {lab=HV} 
N 907.5 -582.5 907.5 -577.5 {lab=HV} 
N 907.5 -582.5 930 -560 {lab=HV} 
N 922.5 -565 930 -560 {lab=HV} 
N 925 -567.5 930 -560 {lab=HV} 
N 840 -620 840 -610 {lab=HV} 
N 860 -640 870 -640 {lab=HV} 
N 900 -620 900 -610 {lab=HV} 
N 860 -610 860 -600 {lab=HV} 
N 880 -610 880 -600 {lab=HV} 
N 860 -620 860 -610 {lab=HV} 
N 880 -620 880 -610 {lab=HV} 
N 810 -430 810 -400 {lab=#net1} 
N 810 -490 870 -490 {lab=HV} 
N 580 -430 580 -400 {lab=#net2} 
N 520 -430 520 -340 {lab=#net3} 
N 520 -340 720 -340 {lab=#net3} 
N 810 -340 870 -340 {lab=FB} 
N 520 -490 580 -490 {lab=VCC} 
N 580 -640 580 -490 {lab=VCC} 
N 870 -530 870 -490 {lab=HV} 
N 870 -430 870 -340 {lab=FB} 
N 580 -170 580 -150 {lab=VSS} 
N 60 -640 150 -640 {lab=VCC} 
N 60 -420 150 -420 {lab=VSS} 
N 800 -220 800 -200 {lab=VCC} 
N 800 -140 870 -140 {lab=FB} 
N 150 -440 150 -420 {lab=VSS} 
N 60 -440 60 -420 {lab=VSS} 
N 460 -200 540 -200 {lab=FB} 
N 580 -280 580 -230 {lab=D} 
N 150 -640 580 -640 {lab=VCC} 
N 470 -280 470 -260 {lab=VCC} 
N 460 -420 460 -400 {lab=VCC} 
N 460 -340 520 -340 {lab=#net3} 
N 720 -280 720 -260 {lab=VSS} 
C {devices/netlist_not_shown} 1140 -170 0 0 {
name=STIMULI 
value=".option SCALE=1e-6 PARHIER=LOCAL RUNLVL=6 post MODMONTE=1
.option sampling_method = SRS
.option method=gear
vvss vss 0 dc 0

.temp 30
.tran 1n 1m  uic
.meas tran iavg AVG i(vc) from=950u to=990u
.probe tran p(q1) i(d2) i(d1) i(d3) i(l1) i(l2) i(L3)
.meas tran pavg AVG p(q1) from=950u to=990u"} 
C {devices/lab_pin} 60 -640 0 0 {name=p1 lab=VCC} 
C {devices/res} 60 -580 0 1 {name=rs m=1 value=1} 
C {devices/vsource} 60 -470 0 1 {name=vvcc value="pwl 0 0 1u 12"} 
C {devices/lab_pin} 60 -420 0 0 {name=p7 lab=VSS} 
C {devices/capa} 150 -470 0 0 {name=cvcc m=1 value=10u} 
C {devices/launcher} 1180 -530 0 0 {name=h2 
descr="BJT MODELS" 
url="http://www.zen22142.zen.co.uk/ltspice/standard.bjt"} 
C {devices/launcher} 1180 -480 0 0 {name=h1 
descr="NPN MODELS" 
url="http://web.mit.edu/Magic/Public/ckt/npn.mod"} 
C {devices/launcher} 1180 -580 0 0 {name=h3 
descr="COIL CALCULATION" 
url="http://hamwaves.com/antennas/inductance.html"} 
C {devices/launcher} 1180 -630 0 0 {name=h4 
descr="ltwiki BJT MODELS" 
url="http://ltwiki.org/?title=Standard.bjt"} 
C {devices/lab_pin} 60 -530 0 0 {name=p6 lab=VCCS} 
C {devices/lab_pin} 870 -560 0 1 {name=p4 lab=HV} 
C {devices/capa} 1010 -450 0 0 {name=c1 m=1 value=1p} 
C {devices/lab_pin} 1010 -250 0 0 {name=p3 lab=VSS} 
C {devices/ind} 580 -460 2 1 {name=L1 value=9.8u} 
C {devices/ind} 810 -460 0 1 {name=L2 value=9.1m} 
C {devices/k} 700 -460 0 0 {name=K0 K=0.15 L1=L1 L2=L2} 
C {devices/res} 810 -370 0 0 {name=r0 m=1 value=.32} 
C {devices/capa} 870 -460 0 1 {name=c0 m=1 value=4.1p} 
C {devices/res} 580 -370 0 1 {name=r3 m=1 value=0.22} 
C {devices/capa} 520 -460 0 0 {name=c5 m=1 value=13p} 
C {devices/lab_pin} 580 -260 0 1 {name=p5 lab=D} 
C {devices/lab_pin} 870 -170 0 1 {name=p0 lab=FB} 
C {devices/diode} 800 -110 2 0 {name=D2 model=d1n4148 area=1
} 
C {devices/lab_pin} 580 -150 0 1 {name=p10 lab=VSS} 
C {devices/diode} 800 -170 2 0 {name=D1 model=d1n4148 area=1
} 
C {devices/lab_pin} 800 -220 0 1 {name=p16 lab=VCC} 
C {devices/lab_pin} 460 -200 0 0 {name=p11 lab=FB} 
C {devices/lab_pin} 800 -80 0 0 {name=p12 lab=VSS} 
C {devices/npn} 560 -200 0 0 {name=Q5 model=bd139 area=1} 
C {devices/res} 470 -230 0 0 {name=r1 m=1 value=100k} 
C {devices/lab_pin} 470 -280 0 0 {name=p8 lab=VCC} 
C {devices/diode} 460 -370 2 0 {name=D3 model=d1n5400 area=1
} 
C {devices/lab_pin} 460 -420 0 0 {name=p13 lab=VCC} 
C {devices/ammeter} 580 -310 0 0 {name=vmeasure} 
C {devices/diode} 720 -310 2 0 {name=D4 model=d1n5400 area=1
} 
C {devices/lab_pin} 720 -260 0 0 {name=p14 lab=VSS} 
C {devices/netlist_not_shown} 1140 -330 0 0 {name=MODELS 
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
+ XTI = 5.8315 KF = 0 AF = 1 FC = 0.83)


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
+ VCEO=40 ICRATING=500M )

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
+ RE=0.6 RC=0.25 CJE=1.3E-11 TF=6.4E-10 CJC=4E-12 VJC=0.54 TR=5.072E-8 )

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
+     FC=0.7947 )
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
*$"} 
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"} 
