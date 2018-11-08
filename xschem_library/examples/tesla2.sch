G {}
V {}
S {}
E {}
L 4 40 -1110 860 -1110 {}
L 4 860 -1110 860 -1060 {}
L 4 40 -1110 40 -1060 {}
L 4 930 -1110 1800 -1110 {}
L 4 1800 -1110 1800 -1060 {}
L 4 930 -1110 930 -1060 {}
T {BEHAVIORAL PART} 200 -1190 0 0 1 1 {}
T {REAL DISCRETE COMPONENTS} 1010 -1190 0 0 1 1 {}
N 1320 -570 1440 -570 {lab=#net1}
N 1010 -570 1010 -550 {lab=D}
N 1010 -700 1010 -670 {lab=VPP}
N 1010 -460 1010 -400 {lab=VNN}
N 310 -990 310 -970 {lab=#net2}
N 310 -850 310 -830 {lab=#net3}
N 1230 -640 1230 -570 {lab=D}
N 1010 -700 1230 -700 {lab=VPP}
N 1210 -570 1210 -530 {lab=D}
N 1010 -470 1210 -470 {lab=VNN}
N 370 -220 370 -190 {lab=CURSENSE2}
N 320 -220 370 -220 {lab=CURSENSE2}
N 1440 -510 1440 -480 {lab=#net4}
N 1410 -420 1440 -420 {lab=VSS}
N 1380 -510 1380 -420 {lab=VSS}
N 1410 -230 1680 -230 {lab=VSS}
N 1230 -570 1260 -570 {lab=D}
N 1010 -750 1010 -700 {lab=VPP}
N 1010 -490 1010 -470 {lab=VNN}
N 1210 -570 1230 -570 {lab=D}
N 1010 -570 1210 -570 {lab=D}
N 1010 -580 1010 -570 {lab=D}
N 1650 -510 1650 -480 {lab=#net5}
N 1680 -570 1710 -570 {lab=HV}
N 1680 -420 1710 -420 {lab=#net6}
N 1710 -510 1710 -420 {lab=#net6}
N 1680 -610 1680 -570 {lab=HV}
N 1700 -610 1780 -610 {lab=HV}
N 1650 -570 1680 -570 {lab=HV}
N 1650 -420 1680 -420 {lab=#net6}
N 600 -220 600 -190 {lab=CURSENSE1}
N 550 -220 600 -220 {lab=CURSENSE1}
N 600 -350 600 -320 {lab=CURSENSE}
N 550 -350 600 -350 {lab=CURSENSE}
N 190 -640 190 -590 {lab=#net7}
N 190 -640 250 -640 {lab=#net7}
N 190 -440 250 -440 {lab=#net8}
N 190 -490 190 -440 {lab=#net8}
N 430 -660 430 -640 {lab=#net9}
N 430 -480 430 -440 {lab=#net10}
N 1380 -420 1410 -420 {lab=VSS}
N 1230 -230 1410 -230 {lab=VSS}
N 1780 -610 1780 -510 {lab=HV}
N 1780 -450 1780 -230 {lab=VSS}
N 1680 -230 1780 -230 {lab=VSS}
N 410 -910 500 -910 {lab=VSS}
N 400 -840 400 -830 {lab=VNN}
N 400 -990 400 -980 {lab=VPP}
N 390 -910 400 -900 {lab=VSS}
N 400 -920 410 -910 {lab=VSS}
N 310 -990 330 -990 {lab=#net2}
N 400 -990 500 -990 {lab=VPP}
N 310 -830 330 -830 {lab=#net3}
N 400 -830 500 -830 {lab=VNN}
N 530 -680 590 -680 {lab=VSS}
N 590 -700 640 -700 {lab=#net11}
N 470 -700 530 -700 {lab=#net9}
N 470 -700 470 -640 {lab=#net9}
N 620 -660 640 -660 {lab=#net9}
N 620 -660 620 -640 {lab=#net9}
N 310 -910 390 -910 {lab=VSS}
N 390 -910 410 -910 {lab=VSS}
N 390 -830 400 -830 {lab=VNN}
N 390 -990 400 -990 {lab=VPP}
N 430 -640 470 -640 {lab=#net9}
N 470 -640 620 -640 {lab=#net9}
N 530 -520 590 -520 {lab=VSS}
N 590 -540 640 -540 {lab=#net12}
N 470 -540 530 -540 {lab=#net10}
N 470 -540 470 -480 {lab=#net10}
N 620 -500 640 -500 {lab=#net10}
N 620 -500 620 -480 {lab=#net10}
N 430 -480 470 -480 {lab=#net10}
N 470 -480 620 -480 {lab=#net10}
N 810 -460 1010 -460 {lab=VNN}
N 810 -580 1010 -580 {lab=D}
N 740 -680 740 -630 {lab=GH}
N 740 -630 770 -630 {lab=GH}
N 740 -520 740 -510 {lab=GL}
N 740 -510 770 -510 {lab=GL}
N 810 -640 910 -640 {lab=#net13}
N 810 -520 910 -520 {lab=#net14}
N 1010 -470 1010 -460 {lab=VNN}
N 1010 -610 1010 -580 {lab=D}
N 1680 -610 1700 -610 {lab=HV}
N 1410 -260 1410 -230 {lab=VSS}
N 1380 -630 1380 -570 {lab=#net1}
N 1410 -420 1410 -380 {lab=VSS}
N 1670 -420 1670 -350 {lab=#net6}
N 1410 -390 1410 -260 {lab=VSS}
C {devices/code} 1850 -510 0 0 {
name=STIMULI 
value=".option PARHIER=LOCAL RUNLVL=6 post MODMONTE=1
.option sampling_method = SRS
.option method=gear
vvss vss 0 dc 0 
.temp 30
.tran 20n 1m uic $$ sweep CTUNE 1.5n 8.5n 0.1n

* .meas tran pavg_gatedrv AVG p(xgatedrv) from=950u to=990u
* .meas tran pavg_mos1 AVG p(xmos1) from=950u to=990u
* .meas tran pavg_mos2 AVG p(xmos2) from=950u to=990u
* .probe tran p(xgatedrv) p(xmos1) p(xmos2) i(d1) I(L1) I(L2)
.include stimuli.cir
.probe tran i(l1) i(l2) p(xm1) p(xm2) p(d1) p(d2) 

.meas tran pavg_mos1 AVG p(xm1) from=950u to=990u
.meas tran pavg_mos2 AVG p(xm2) from=950u to=990u
.meas tran pavg_d1 AVG p(d1) from=950u to=990u
.meas tran pavg_d2 AVG p(d2) from=950u to=990u"}
C {devices/launcher} 1920 -770 0 0 {name=h2 
descr="BJT MODELS" 
url="http://www.zen22142.zen.co.uk/ltspice/standard.bjt"}
C {devices/launcher} 1920 -720 0 0 {name=h1 
descr="NPN MODELS" 
url="http://web.mit.edu/Magic/Public/ckt/npn.mod"}
C {devices/launcher} 1920 -820 0 0 {name=h3 
descr="COIL CALCULATION" 
url="http://hamwaves.com/antennas/inductance.html"}
C {devices/launcher} 1920 -870 0 0 {name=h4 
descr="ltwiki BJT MODELS" 
url="http://ltwiki.org/?title=Standard.bjt"}
C {devices/lab_pin} 1230 -230 0 0 {name=p1 lab=VSS}
C {devices/ammeter} 1290 -570 3 0 {name=vmeasure}
C {devices/vsource} 310 -940 0 0 {name=v1 value=12}
C {devices/lab_pin} 310 -910 0 0 {name=p4 lab=VSS}
C {devices/vsource} 310 -880 0 0 {name=v2 value=12}
C {devices/lab_pin} 500 -990 0 1 {name=p5 lab=VPP}
C {devices/lab_pin} 500 -830 0 1 {name=p6 lab=VNN}
C {devices/lab_pin} 1010 -750 0 0 {name=p7 lab=VPP}
C {devices/lab_pin} 1010 -400 0 0 {name=p9 lab=VNN}
C {devices/lab_pin} 1010 -560 0 0 {name=p11 lab=D}
C {devices/vsource_arith} 370 -160 0 0 {name=E1 VOL=(i(rfb)-v(offset))*200 MAX=5 MIN=0}
C {devices/lab_pin} 370 -130 0 0 {name=p15 lab=VSS}
C {devices/lab_pin} 320 -220 0 0 {name=p16 lab=CURSENSE2}
C {devices/ind} 1440 -540 2 1 {name=L1 value=9.8u}
C {devices/res} 1440 -450 0 1 {name=r1 m=1 value=0.22}
C {devices/capa} 1380 -540 0 0 {name=c5 m=1 value=13p}
C {devices/ind} 1650 -540 0 1 {name=L2 value=9.1m}
C {devices/k} 1540 -540 0 0 {name=K1 K=0.15 L1=L1 L2=L2}
C {devices/res} 1650 -450 0 0 {name=r4 m=1 value=.32}
C {devices/capa} 1710 -540 0 1 {name=c7 m=1 value=4.1p}
C {devices/lab_pin} 1780 -610 0 1 {name=p71 lab=HV}
C {devices/vsource_arith} 600 -160 0 0 {name=E11 VOL=(i(rfb)+v(offset))*20000 MAX=5 MIN=0}
C {devices/lab_pin} 600 -130 0 0 {name=p0 lab=VSS}
C {devices/lab_pin} 550 -220 0 0 {name=p8 lab=CURSENSE1}
C {devices/vsource} 350 -290 0 0 {name=v3 value="pwl 0 0 280u 0 600u 1.2"
}
C {devices/lab_pin} 350 -260 0 0 {name=p25 lab=VSS}
C {devices/lab_pin} 350 -320 0 0 {name=p27 lab=OFFSET}
C {devices/vsource_arith} 600 -290 0 0 {name=E2 VOL=i(rfb)*20000 MAX=5 MIN=0}
C {devices/lab_pin} 600 -260 0 0 {name=p13 lab=VSS}
C {devices/lab_pin} 550 -350 0 0 {name=p17 lab=CURSENSE}
C {examples/sr_flop} 340 -430 0 0 {name=x8 VTH=2.5 VHI=5}
C {devices/lab_pin} 740 -520 0 1 {name=p28 lab=GL}
C {devices/lab_pin} 250 -420 0 0 {name=p29 lab=CURSENSE1}
C {devices/lab_pin} 250 -400 0 0 {name=p30 lab=VSS}
C {examples/inv} 150 -490 0 0 {name=E7 TABLE="2.3 5 2.7 0.0"}
C {devices/lab_pin} 150 -460 0 0 {name=p35 lab=VSS}
C {devices/lab_pin} 110 -490 0 0 {name=p36 lab=CURSENSE}
C {examples/sr_flop} 340 -650 0 0 {name=x5 VTH=2.5 VHI=5}
C {devices/lab_pin} 740 -680 0 1 {name=p37 lab=GH}
C {devices/lab_pin} 250 -660 0 0 {name=p38 lab=CURSENSE}
C {examples/inv} 150 -590 0 0 {name=E12 TABLE="2.3 5 2.7 0.0"}
C {devices/lab_pin} 150 -560 0 0 {name=p39 lab=VSS}
C {devices/lab_pin} 250 -620 0 0 {name=p40 lab=VSS}
C {devices/lab_pin} 110 -590 0 0 {name=p41 lab=CURSENSE2}
C {devices/diode} 1210 -500 2 1 {name=D1 model=d1n5400 area=1
url="http://pdf.datasheetcatalog.com/datasheet/bytes/1N5406.pdf"}
C {devices/diode} 1230 -670 2 0 {name=D2 model=d1n5400 area=1
url="http://pdf.datasheetcatalog.com/datasheet/bytes/1N5406.pdf"}
C {devices/capa} 1780 -480 0 1 {name=c1 m=1 value=1p}
C {devices/capa} 400 -950 0 0 {name=c2 m=1 value=10n}
C {devices/capa} 400 -870 0 0 {name=c3 m=1 value=10n}
C {devices/lab_pin} 500 -910 0 1 {name=p3 lab=VSS}
C {devices/res} 360 -990 1 1 {name=r5 m=1 value=0.3}
C {devices/res} 360 -830 1 1 {name=r6 m=1 value=0.3}
C {examples/an2} 680 -680 0 0 {name=E6 TABLE="2 0 6 8"}
C {devices/lab_pin} 680 -640 0 0 {name=p19 lab=VSS}
C {devices/delay_hsp} 560 -700 0 0 {name=E4 DEL=5n SCALE=1}
C {devices/lab_pin} 530 -680 0 0 {name=p20 lab=VSS}
C {examples/an2} 680 -520 0 0 {name=E5 TABLE="2 0 6 8"}
C {devices/lab_pin} 680 -480 0 0 {name=p21 lab=VSS}
C {devices/delay_hsp} 560 -540 0 0 {name=E9 DEL=5n SCALE=1}
C {devices/lab_pin} 530 -520 0 0 {name=p22 lab=VSS}
C {devices/nmos3} 990 -640 0 0 {name=xm2 model=irf540 m=1}
C {devices/nmos3} 990 -520 0 0 {name=xm1 model=irf540 m=1}
C {devices/lab_pin} 770 -590 0 0 {name=p2 lab=VSS}
C {devices/lab_pin} 770 -470 0 0 {name=p10 lab=VSS}
C {devices/res} 940 -640 1 1 {name=r2 m=1 value=1.5}
C {devices/res} 940 -520 1 1 {name=r3 m=1 value=1.5}
C {devices/vsource_pwl} 810 -490 0 0 {name=E10 TABLE="1 0 7 8"}
C {devices/vsource_pwl} 810 -610 0 0 {name=E3 TABLE="1 0 7 8"}
C {devices/res} 1670 -260 0 1 {name=rfb m=1 value=10}
C {devices/isource} 1380 -660 0 0 {name=i1 value="pwl 0 0 1u 3 2u 3 3u 0"}
C {devices/lab_pin} 1380 -690 0 0 {name=p12 lab=VSS}
C {devices/ammeter} 1670 -320 0 0 {name=vr}
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {devices/code} 1850 -660 0 0 {name=MODELS 
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
.ENDS
"}
