G {}
V {}
S {}
E {}
L 3 910 -560 960 -540 {}
L 3 880 -680 1200 -680 {}
L 8 900 -540 950 -560 {}
L 11 900 -550 950 -530 {}
L 16 910 -530 960 -550 {}
B 4 940 -510 960 -450 {}
B 7 900 -490 970 -470 {}
B 8 910 -510 930 -450 {}
P 1 10 930 -370 920 -430 1000 -410 960 -390 990 -360 970 -360 960 -340 940 -380 930 -360 930 -370 {}
P 4 6 880 -350 880 -440 870 -390 870 -430 860 -400 860 -370 {}
P 15 11 920 -400 930 -440 950 -410 960 -430 970 -400 990 -410 970 -370 940 -380 910 -360 890 -410 920 -400 {fill=true}
T {Welcome to XSCHEM!} 60 -950 0 0 1 1 {layer=5}
T {This is the start schematic window specified in your .xschem file. 
You may change this to any different schematic file or even start
with an empty window. Just set the XSCHEM_START_WINDOW variable: 

- set XSCHEM_START_WINDOW \{mylib/0_top\}

or, if you want to start with an empty window: 

- set XSCHEM_START_WINDOW \{\}

On the left you see some sample circuits. You may descend into any
of these by selecting one with a left mouse button click and
pressing the 'e' key, or by menu 'Edit -> Push Schematic'.
You can return here after descending into a schematic by hitting 
'<Ctrl>-e' or by menu 'Edit -> Pop'.
} 10 -880 0 0 0.4 0.4 {}
T {Select the components here under
and press'<Ctrl>-h' or <Alt>-click
them to visit xschem websites
or local docs} 400 -420 0 0 0.3 0.3 {}
T {Lines} 1010 -560 0 0 0.6 0.6 {layer=4}
T {Rectangles} 1010 -500 0 0 0.6 0.6 {layer=4}
T {Polygons} 1010 -410 0 0 0.6 0.6 {layer=4}
T {Electrical
wires} 1010 -330 0 0 0.6 0.6 {layer=4}
T {XSCHEM OBJECTS} 910 -720 0 0 0.6 0.6 {layer=7}
T {Components} 1010 -210 0 0 0.6 0.6 {layer=4}
T {TEXT} 890 -620 0 0 0.5 0.5 {layer=8}
T {TEXT} 890 -670 1 0 0.7 0.7 {layer=6 font=FreeMono}
T {Text} 900 -660 0 0 0.5 0.5 {layer=7 font="Times"}
T {TEXT} 990 -660 1 0 0.5 0.5 {}
T {Text} 1010 -650 0 0 0.6 0.6 {layer=4}
T {Arcs/Circles} 1010 -120 0 0 0.6 0.6 {layer=4}
T {TBD} 890 -120 0 0 0.6 0.6 {}
N 870 -300 990 -300 {lab=#net1}
N 910 -340 910 -250 {lab=#net2}
N 910 -270 970 -270 {lab=#net2}
N 930 -310 960 -300 {lab=#net1}
C {examples/poweramp} 160 -310 0 0 {name=x1}
C {examples/tesla} 160 -230 0 0 {name=x2}
C {examples/test_ne555} 160 -190 0 0 {name=x3}
C {examples/test_lm324} 160 -150 0 0 {name=x4}
C {examples/osc} 160 -270 0 0 {name=x5}
C {examples/tesla2} 160 -110 0 0 {name=x7}
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {devices/launcher} 450 -290 0 0 {name=h8 
descr="XSCHEM WEBSITE" 
url="https://xschem.sourceforge.io/stefan/xschem.html"
program=x-www-browser}
C {devices/launcher} 450 -190 0 0 {name=h1 
descr="XSCHEM ON SOURCEFORGE" 
url="https://sourceforge.net/projects/xschem/"
program=x-www-browser}
C {examples/cmos_example} 160 -350 0 0 {name=x6}
C {devices/launcher} 450 -240 0 0 {name=h2 
descr="LOCAL DOCUMENTATION" 
url="$env(HOME)/share/doc/xschem/xschem.html"
program=x-www-browser

}
C {examples/greycnt} 160 -390 0 0 {name=x8}
C {examples/loading} 160 -430 0 0 {name=x9}
C {devices/launcher} 450 -140 0 0 {name=h3 
descr="Toggle light/dark 
colorscheme" 
tclcommand="xschem toggle_colorscheme"
}
C {examples/inv} 930 -200 0 0 {name=E1 TABLE="1.4 3.0 1.6 0.0"}
C {examples/pcb_test1} 160 -470 0 0 {name=x0}