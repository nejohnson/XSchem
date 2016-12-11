G {} 
V {} 
S {} 
T {CMOS DIFFERENTIAL AMPLIFIER
EXAMPLE} 810 -570 0 0 0.4 0.4 {} 
N 500 -180 500 -120 {lab=0} 
N 60 -240 60 -210 {lab=VCC} 
N 60 -240 90 -240 {lab=VCC} 
N 240 -260 240 -210 {lab=GN} 
N 240 -210 280 -210 {lab=GN} 
N 280 -210 280 -180 {lab=GN} 
N 280 -180 460 -180 {lab=GN} 
N 240 -180 240 -120 {lab=0} 
N 500 -250 500 -210 {lab=SN} 
N 430 -280 450 -280 {lab=0} 
N 550 -280 570 -280 {lab=0} 
N 430 -250 570 -250 {lab=SN} 
N 370 -280 390 -280 {lab=PLUS} 
N 610 -280 630 -280 {lab=MINUS} 
N 570 -430 590 -430 {lab=VCC} 
N 410 -430 430 -430 {lab=VCC} 
N 470 -430 530 -430 {lab=GP} 
N 470 -430 470 -400 {lab=GP} 
N 430 -400 470 -400 {lab=GP} 
N 430 -400 430 -310 {lab=GP} 
N 570 -400 570 -310 {lab=DIFFOUT} 
N 570 -500 570 -460 {lab=VCC} 
N 430 -500 570 -500 {lab=VCC} 
N 430 -500 430 -460 {lab=VCC} 
N 500 -520 500 -500 {lab=VCC} 
N 570 -360 690 -360 {lab=DIFFOUT} 
N 60 -370 60 -340 {lab=PLUS} 
N 60 -370 90 -370 {lab=PLUS} 
N 60 -520 60 -490 {lab=MINUS} 
N 60 -520 90 -520 {lab=MINUS} 
C {devices/netlist_not_shown} 870 -190 0 0 {name=STIMULI
only_toplevel=true
value=".option PARHIER=LOCAL RUNLVL=6 post MODMONTE=1 warn maxwarns=400
.option sampling_method = SRS 
.option method=gear
.temp 30


.dc VPLUS 1.3 1.7 0.001
.probe i(*) 
"} 
C {devices/lab_pin} 60 -150 0 0 {name=p17 lab=0} 
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"} 
C {devices/netlist_not_shown} 1010 -190 0 0 {name=MOSIS_MODELS value="

* hdif    = 0.5e-6
.MODEL CMOSN NMOS (
+LEVEL   = 49             acm     = 3
+VERSION = 3.1            TNOM    = 27             TOX     = 7.7E-9
+XJ      = 1E-7           NCH     = 2.3579E17      VTH0    = 0.5048265
+K1      = 0.5542796      K2      = 0.0155863      K3      = 2.3475646
+K3B     = -3.3142916     W0      = 4.145888E-5    NLX     = 1.430868E-7
+DVT0W   = 0              DVT1W   = 0              DVT2W   = 0
+DVT0    = -0.0150839     DVT1    = 1.51022E-3     DVT2    = 0.170688
+U0      = 415.8570638    UA      = 5.057324E-11   UB      = 1.496793E-18
+UC      = 2.986268E-11   VSAT    = 1.237033E5     A0      = 0.9098788
+AGS     = 0.2120181      B0      = 1.683612E-6    B1      = 5E-6
+KETA    = -4.011887E-4   A1      = 0              A2      = 1
+RDSW    = 1.156967E3     PRWG    = -8.468558E-3   PRWB    = -7.678669E-3
+WR      = 1              WINT    = 5.621821E-8    LINT    = 1.606205E-8
+XL      = -2E-8          XW      = 0              DWG     = -6.450939E-9
+DWB     = 6.530228E-9    VOFF    = -0.1259348     NFACTOR = 0.3344887
+CIT     = 0              CDSC    = 1.527511E-3    CDSCD   = 0
+CDSCB   = 0              ETA0    = 1.21138E-3     ETAB    = -1.520242E-4
+DSUB    = 0.1259886      PCLM    = 0.8254768      PDIBLC1 = 0.4211084
+PDIBLC2 = 6.081164E-3    PDIBLCB = -5.865856E-6   DROUT   = 0.7022263
+PSCBE1  = 7.238634E9     PSCBE2  = 5E-10          PVAG    = 0.6261655
+DELTA   = 0.01           MOBMOD  = 1              PRT     = 0
+UTE     = -1.5           KT1     = -0.11          KT1L    = 0
+KT2     = 0.022          UA1     = 4.31E-9        UB1     = -7.61E-18
+UC1     = -5.6E-11       AT      = 3.3E4          WL      = 0
+WLN     = 1              WW      = -1.22182E-15   WWN     = 1.137
+WWL     = 0              LL      = 0              LLN     = 1
+LW      = 0              LWN     = 1              LWL     = 0
+CAPMOD  = 2              XPART   = 0.4            CGDO    = 1.96E-10
+CGSO    = 1.96E-10       CGBO    = 0              CJ      = 8.829973E-4
+PB      = 0.7946332      MJ      = 0.3539285      CJSW    = 2.992362E-10
+PBSW    = 0.9890846      MJSW    = 0.1871372      PVTH0   = -0.0148617
+PRDSW   = -114.7860236   PK2     = -5.151187E-3   WKETA   = 5.687313E-3
+LKETA   = -0.018518       )
*
* hdif    = 0.5e-6
.MODEL CMOSP PMOS (
+LEVEL   = 49             acm     = 3
+VERSION = 3.1            TNOM    = 27             TOX     = 7.7E-9
+XJ      = 1E-7           NCH     = 8.52E16        VTH0    = -0.6897992
+K1      = 0.4134289      K2      = -5.342989E-3   K3      = 24.8361788
+K3B     = -1.4390847     W0      = 2.467689E-6    NLX     = 3.096223E-7
+DVT0W   = 0              DVT1W   = 0              DVT2W   = 0
+DVT0    = 1.3209807      DVT1    = 0.4695965      DVT2    = -8.790762E-4
+U0      = 150.6275733    UA      = 2.016943E-10   UB      = 1.714919E-18
+UC      = -1.36948E-11   VSAT    = 9.559222E4     A0      = 0.9871247
+AGS     = 0.3541967      B0      = 3.188091E-6    B1      = 5E-6
+KETA    = -0.0169877     A1      = 0              A2      = 1
+RDSW    = 2.443009E3     PRWG    = 0.0260616      PRWB    = 0.141561
+WR      = 1              WINT    = 5.038936E-8    LINT    = 1.650588E-9
+XL      = -2E-8          XW      = 0              DWG     = -1.535456E-8
+DWB     = 1.256904E-8    VOFF    = -0.15          NFACTOR = 1.5460516
+CIT     = 0              CDSC    = 1.413317E-4    CDSCD   = 0
+CDSCB   = 0              ETA0    = 0.3751392      ETAB    = 2.343374E-3
+DSUB    = 0.8877574      PCLM    = 5.8638076      PDIBLC1 = 1.05224E-3
+PDIBLC2 = 3.481753E-5    PDIBLCB = 2.37525E-3     DROUT   = 0.0277454
+PSCBE1  = 3.013379E10    PSCBE2  = 3.608179E-8    PVAG    = 3.9564294
+DELTA   = 0.01           MOBMOD  = 1              PRT     = 0
+UTE     = -1.5           KT1     = -0.11          KT1L    = 0
+KT2     = 0.022          UA1     = 4.31E-9        UB1     = -7.61E-18
+UC1     = -5.6E-11       AT      = 3.3E4          WL      = 0
+WLN     = 1              WW      = -5.22182E-16   WWN     = 1.125
+WWL     = 0              LL      = 0              LLN     = 1
+LW      = 0              LWN     = 1              LWL     = 0
+CAPMOD  = 2              XPART   = 0.4            CGDO    = 2.307E-10
+CGSO    = 2.307E-10      CGBO    = 0              CJ      = 1.397645E-3
+PB      = 0.99           MJ      = 0.5574537      CJSW    = 3.665392E-10
+PBSW    = 0.99           MJSW    = 0.3399328      PVTH0   = 0.0114364
+PRDSW   = 52.7951169     PK2     = 9.714153E-4    WKETA   = 0.0109418
+LKETA   = 7.702974E-3     )
"
} 
C {devices/nmos4} 480 -180 0 0 {name=m1 model=cmosn w=5u l=2u m=1} 
C {devices/pmos4} 550 -430 0 0 {name=m2 model=cmosp w=5u l=2u m=1} 
C {devices/vsource} 60 -180 0 0 {name=VVCC value=3} 
C {devices/lab_pin} 500 -120 0 0 {name=p1 lab=0} 
C {devices/lab_pin} 90 -240 0 1 {name=p2 lab=VCC} 
C {devices/nmos4} 260 -180 0 1 {name=m3 model=cmosn w=5u l=2u m=1} 
C {devices/lab_pin} 240 -120 0 0 {name=p3 lab=0} 
C {devices/isource} 240 -290 0 0 {name=IBIAS value=10u} 
C {devices/lab_pin} 240 -320 0 0 {name=p4 lab=0} 
C {devices/nmos4} 410 -280 0 0 {name=m4 model=cmosn w=10u l=1u m=1} 
C {devices/lab_pin} 450 -280 0 1 {name=p5 lab=0} 
C {devices/nmos4} 590 -280 0 1 {name=m5 model=cmosn w=10u l=1u m=1} 
C {devices/lab_pin} 550 -280 0 0 {name=p0 lab=0} 
C {devices/lab_pin} 590 -430 0 1 {name=p6 lab=VCC} 
C {devices/pmos4} 450 -430 0 1 {name=m6 model=cmosp w=5u l=2u m=1} 
C {devices/lab_pin} 410 -430 0 0 {name=p7 lab=VCC} 
C {devices/lab_pin} 500 -520 0 0 {name=p8 lab=VCC} 
C {devices/lab_pin} 370 -280 0 0 {name=p9 lab=PLUS} 
C {devices/lab_pin} 630 -280 0 1 {name=p10 lab=MINUS} 
C {devices/lab_pin} 690 -360 0 1 {name=p11 lab=DIFFOUT} 
C {devices/lab_pin} 430 -380 0 0 {name=p12 lab=GP} 
C {devices/lab_pin} 240 -230 0 0 {name=p13 lab=GN} 
C {devices/lab_pin} 60 -280 0 0 {name=p14 lab=0} 
C {devices/vsource} 60 -310 0 0 {name=VPLUS value=1.5} 
C {devices/lab_pin} 90 -370 0 1 {name=p15 lab=PLUS} 
C {devices/lab_pin} 60 -430 0 0 {name=p16 lab=0} 
C {devices/vsource} 60 -460 0 0 {name=V1 value=1.5} 
C {devices/lab_pin} 90 -520 0 1 {name=p18 lab=MINUS} 
C {devices/lab_pin} 500 -230 0 0 {name=p19 lab=SN} 
