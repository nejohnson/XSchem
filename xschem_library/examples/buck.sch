G {}
V {}
S {}
E {}
T {PARAX} 680 -190 0 0 0.5 0.5 {}
T {PARAX} 800 -310 0 0 0.5 0.5 {}
N 930 -140 1010 -140 {lab=VSS}
N 580 -270 730 -270 {lab=A}
N 660 -140 930 -140 {lab=VSS}
N 1010 -270 1110 -270 {lab=VOUT}
N 290 -270 390 -270 {lab=VCC}
N 450 -270 520 -270 {lab=#net1}
N 120 -850 220 -850 {lab=CTRL}
N 220 -850 360 -850 {lab=CTRL}
N 290 -870 290 -850 {lab=CTRL}
N 250 -480 390 -480 {lab=VREF}
N 870 -610 870 -530 {lab=FB}
N 840 -610 910 -610 {lab=FB}
N 970 -610 1080 -610 {lab=SW}
N 1080 -610 1080 -490 {lab=SW}
N 660 -150 660 -140 {lab=VSS}
N 930 -210 930 -140 {lab=VSS}
N 1010 -210 1010 -140 {lab=VSS}
N 850 -270 1010 -270 {lab=VOUT}
N 690 -280 690 -270 {lab=A}
N 890 -630 890 -610 {lab=FB}
N 1200 -750 1200 -730 {lab=VSS}
N 1200 -830 1200 -810 {lab=HYST}
N 1100 -800 1160 -800 {lab=HYSTD}
N 1060 -890 1120 -890 {lab=HYSTD}
N 1180 -890 1200 -890 {lab=HYST}
N 1200 -890 1200 -830 {lab=HYST}
N 900 -1150 1000 -1150 {lab=CTRL2}
N 1610 -490 1610 -430 {lab=#net2}
N 1610 -630 1610 -550 {lab=#net3}
N 1610 -640 1610 -630 {lab=#net3}
N 1610 -460 1740 -460 {lab=#net2}
N 1340 -600 1610 -600 {lab=#net3}
N 1340 -600 1340 -430 {lab=#net3}
N 1610 -670 1740 -670 {lab=#net3}
N 1610 -670 1610 -640 {lab=#net3}
N 1800 -670 1840 -670 {lab=CAP2}
N 1840 -670 1840 -460 {lab=CAP2}
N 1800 -460 1840 -460 {lab=CAP2}
N 1940 -460 1940 -420 {lab=CAP2}
N 1840 -460 1940 -460 {lab=CAP2}
C {library_t9/micron} 170 -10 0 0 {name=l3 author="Stefan Schippers"}
C {devices/code} 30 -190 0 0 {name=STIMULI value="
**   eldo
*.OPTION EPS=1e-6  SMOOTH AMMETER


**  model hspice 1.5
* .lib  '/home/schippes/hspice/pcm100s_1.5/PCM100s_1.5.lib' ssss

** model hspice 1.5b
* .lib '/home/schippes/hspice/pcm100s_1.5b/PCM100s_1.5.lib' ssss
* .lib '/home/schippes/hspice/pcm100s_1.5b/pcm100s_1.5_ic.lib' typ

** model hspice 1.6
* .lib '/home/schippes/hspice/pcm100s_1.6/PCM100s_1.6.lib' ssss
* .lib \\"./model_100s_1.6_montecarlo\\" ttttt

** model hspice 1.9.2
* .lib '/home/schippes/hspice/pcm100s_1.9.2/pcm100s_1.9.2.lib' ssss
* .lib \\"./model_100s_1.9.2_montecarlo\\" ttttt
* .lib \\"./model_100s_1.9.2_naox_montecarlo\\" ttttt
* .lib \\"./model_100s_1.9.2_paox_montecarlo\\" ttttt

** model hspice 2.0.0_stef
* .lib /home/schippes/hspice/pcm100s_2.0.0/mlib  ss

** model hspice 2.0.0 .... ss, tt, ff, ss_spec ff_spec
* .lib /home/schippes/hspice/pcm100s_2.0.0/mlib  ss

** model hspice 2.3.1 .... ss, tt, ff, ss_spec ff_spec
*.lib /home/schippes/hspice/pcm100s_2.3.1/mlib  ss
.param VCC=24
.temp 25

*   hspice
.option post captab RUNLVL=5 $ probe
.OPTION METHOD=GEAR
vvss vss 0 0
vvcc vcc 0 VCC pwl 0 0 100u VCC

* .check setup ( C rise 800p fall ) B   ( VCC 0 'VCC/2' 'VCC/2' )
* .include stimuli.cir

.probe tran i1(r1)
.probe tran i1(l1)

*.op ALL  4n
*.dc vvcc 0 2 0.1
.tran 0.05m 80m uic $$ sweep monte=list 4"
}
C {pcm100s/ideal_switch_hsp} 550 -270 0 0 {name=x1 V_TO_R=1G MIN=1 MAX=1G VCTRL=1.8 m=1}
C {pcm100s/ideal_diode_hsp} 660 -240 2 0 {name=x2 VON=0.5}
C {devices/ind} 760 -270 3 0 {name=l1 value=10u}
C {devices/capa} 930 -240 0 0 {name=c1 m=1 value=100u}
C {devices/res} 1010 -240 0 0 {name=r1 m=1 value=40}
C {devices/lab_pin} 660 -140 0 0 {name=p1 lab=VSS}
C {devices/lab_pin} 1110 -270 0 1 {name=p2 lab=VOUT}
C {devices/lab_pin} 290 -270 0 0 {name=p3 lab=VCC}
C {devices/lab_pin} 550 -310 0 0 {name=p4 lab=SW}
C {devices/vsource} 420 -270 3 0 {name=vsupply value=0}
C {devices/lab_pin} 560 -850 0 1 {name=p1166 lab=F1}
C {devices/lab_pin} 560 -830 0 1 {name=p1167 lab=F2}
C {devices/lab_pin} 560 -810 0 1 {name=p1183 lab=F1N}
C {devices/lab_pin} 560 -790 0 1 {name=p1184 lab=F2N}
C {devices/vsource} 120 -820 0 0 {name=v1 value="pwl 0 0 10u 1.5"}
C {devices/lab_pin} 120 -790 0 0 {name=p6 lab=VSS}
C {devices/lab_pin} 290 -870 0 0 {name=p8 lab=CTRL}
C {pcm100s/ideal_vco_phasegen_hsp} 460 -820 0 0 {name=x3 V_TO_FREQ=100K MIN=0 MAX=VCC}
C {devices/lab_pin} 560 -770 0 1 {name=p5 lab=CAP}
C {library_t9/ampli_ideale_hsp} 470 -520 0 0 {name=x453 offset=0.0 gain=500 vmin="0" vmax="VCC" rout=0.01 rc=1e-6}
C {devices/vsource} 250 -450 0 0 {name=v2 value="pwl 0 0 10u 10"}
C {devices/lab_pin} 250 -420 0 0 {name=p0 lab=VSS}
C {devices/lab_pin} 250 -480 0 0 {name=p7 lab=VREF}
C {devices/lab_pin} 390 -560 0 0 {name=p9 lab=VOUT}
C {library_t9/ampli_ideale_hsp} 950 -490 0 0 {name=x4 offset=0.0 gain=5000 vmin="0" vmax="VCC" rout=0.01 rc=1e-11}
C {devices/lab_pin} 780 -610 0 0 {name=p10 lab=CAP}
C {devices/lab_pin} 600 -520 0 1 {name=p11 lab=PWMCTRL}
C {devices/lab_pin} 870 -450 0 0 {name=p12 lab=PWMCTRL}
C {devices/lab_pin} 1080 -490 0 1 {name=p13 lab=SW}
C {devices/res} 810 -610 1 1 {name=r2 m=1 value=100K}
C {devices/res} 940 -610 1 1 {name=r3 m=1 value=600K}
C {devices/res} 820 -270 1 0 {name=r4 m=1 value=2}
C {devices/res} 660 -180 2 0 {name=r5 m=1 value=5}
C {devices/lab_pin} 690 -280 0 0 {name=p14 lab=A}
C {devices/lab_pin} 890 -630 0 0 {name=p15 lab=FB}
C {devices/vcvs} 1200 -780 0 0 {name=e1 value="100 MAX=VCC MIN=0"}
C {devices/lab_pin} 1200 -730 0 0 {name=p16 lab=VSS}
C {devices/lab_pin} 950 -980 0 0 {name=p17 lab=VSS}
C {devices/lab_pin} 1160 -760 0 0 {name=p21 lab=CAP2}
C {devices/lab_pin} 1200 -830 0 1 {name=p22 lab=HYST}
C {devices/vsource} 950 -1010 0 0 {name=v3 value="pwl 0 0 10u 12"}
C {devices/lab_pin} 950 -1040 0 0 {name=p24 lab=REF2}
C {devices/lab_pin} 1060 -890 0 0 {name=p27 lab=HYSTD}
C {devices/capa} 1060 -860 0 0 {name=c3 m=1 value=100f}
C {devices/res} 1150 -890 1 1 {name=r6 m=1 value=3000}
C {devices/lab_pin} 1060 -830 0 0 {name=p23 lab=VSS}
C {devices/lab_pin} 1100 -800 0 0 {name=p28 lab=HYSTD}
C {devices/vsource} 900 -1120 0 0 {name=v4 value="pwl 0 0 1000u 12"}
C {devices/lab_pin} 900 -1090 0 0 {name=p29 lab=VSS}
C {devices/lab_pin} 1000 -1150 0 1 {name=p31 lab=CTRL2}
C {devices/switch_hsp} 1610 -400 0 0 {name=g1 v_to_r=2G max=10G min=0.1 m=1}
C {devices/isource_arith} 1610 -520 0 0 {name=Gp value="1e-3*v(ctrl2)"
}
C {devices/switch_hsp} 1770 -460 1 0 {name=g2 v_to_r=2G max=10G min=0.1 m=1}
C {devices/switch_hsp} 1770 -670 1 0 {name=g3 v_to_r=2G max=10G min=0.1 m=1}
C {devices/switch_hsp} 1340 -400 0 0 {name=g4 v_to_r=2G max=10G min=0.1 m=1}
C {devices/lab_pin} 1340 -370 0 0 {name=p30 lab=VSS}
C {devices/lab_pin} 1610 -370 0 0 {name=p32 lab=VSS}
C {devices/lab_pin} 1380 -400 0 1 {name=p33 lab=HYST}
C {devices/lab_pin} 1770 -420 0 1 {name=p34 lab=HYST}
C {devices/lab_pin} 1570 -400 0 0 {name=p35 lab=HYST}
C {devices/lab_pin} 1770 -710 0 0 {name=p36 lab=HYST}
C {devices/lab_pin} 1300 -400 0 0 {name=p37 lab=REF2}
C {devices/lab_pin} 1770 -630 0 1 {name=p38 lab=REF2}
C {devices/lab_pin} 1770 -500 0 1 {name=p39 lab=REF2}
C {devices/lab_pin} 1650 -400 0 1 {name=p40 lab=REF2}
C {devices/capa} 1940 -390 0 0 {name=c4 m=1 value=1n}
C {devices/lab_pin} 1940 -360 0 0 {name=p41 lab=VSS}
C {devices/lab_pin} 1940 -440 0 1 {name=p42 lab=CAP2}
