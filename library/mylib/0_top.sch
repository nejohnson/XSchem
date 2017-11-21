G {} 
V {} 
S {} 
L 3 920 -460 970 -440 {} 
L 3 890 -580 1210 -580 {} 
L 8 910 -440 960 -460 {} 
L 11 910 -450 960 -430 {} 
L 16 920 -430 970 -450 {} 
B 4 950 -410 970 -350 {} 
B 7 910 -390 980 -370 {} 
B 8 920 -410 940 -350 {} 
P 1 10 940 -270 930 -330 1010 -310 970 -290 1000 -260 980 -260 970 -240 950 -280 940 -260 940 -270 {} 
P 15 11 930 -300 940 -340 960 -310 970 -330 980 -300 1000 -310 980 -270 950 -280 920 -260 900 -310 930 -300 {} 
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
T {Lines} 1020 -460 0 0 0.6 0.6 {layer=4} 
T {Rectangles} 1020 -400 0 0 0.6 0.6 {layer=4} 
T {Polygons} 1020 -310 0 0 0.6 0.6 {layer=4} 
T {Electrical
wires} 1020 -230 0 0 0.6 0.6 {layer=4} 
T {XSCHEM OBJECTS} 920 -620 0 0 0.6 0.6 {layer=7} 
T {Components} 1020 -110 0 0 0.6 0.6 {layer=4} 
T {TEXT} 900 -520 0 0 0.5 0.5 {layer=8} 
T {TEXT} 900 -570 1 0 0.7 0.7 {layer=6 font=FreeMono} 
T {Text} 910 -560 0 0 0.5 0.5 {layer=7 font="Times"} 
T {TEXT} 1000 -560 1 0 0.5 0.5 {} 
T {Text} 1020 -550 0 0 0.6 0.6 {layer=4} 
N 880 -200 1000 -200 {} 
N 920 -240 920 -150 {} 
N 920 -170 980 -170 {} 
N 940 -210 970 -200 {} 
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
} 
C {devices/launcher} 450 -190 0 0 {name=h1 
descr="XSCHEM ON SOURCEFORGE" 
url="https://sourceforge.net/projects/xschem/"
} 
C {examples/cmos_example} 160 -350 0 0 {name=x6} 
C {devices/launcher} 450 -240 0 0 {name=h2 
descr="LOCAL DOCUMENTATION" 
url="$env(HOME)/share/doc/xschem/xschem.html"
} 
C {examples/greycnt} 160 -390 0 0 {name=x8} 
C {examples/loading} 160 -430 0 0 {name=x9} 
C {devices/launcher} 450 -140 0 0 {name=h3 
descr="Toggle light/dark 
colorscheme" 
tclcommand="xschem toggle_colorscheme"
} 
C {examples/inv} 940 -100 0 0 {name=E1 TABLE="1.4 3.0 1.6 0.0"} 
