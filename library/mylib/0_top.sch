G {} 
V {} 
S {} 
L 3 860 -460 910 -440 {} 
L 3 830 -470 1150 -470 {} 
L 8 850 -440 900 -460 {} 
L 11 850 -450 900 -430 {} 
L 16 860 -430 910 -450 {} 
B 4 890 -410 910 -350 {} 
B 7 850 -390 920 -370 {} 
B 8 860 -410 880 -350 {} 
P 1 10 880 -270 870 -330 950 -310 910 -290 940 -260 920 -260 910 -240 890 -280 880 -260 880 -270 {} 
P 4 11 870 -300 880 -340 900 -310 910 -330 920 -300 940 -310 920 -270 890 -280 860 -260 840 -310 870 -300 {} 
T {Welcome to XSCHEM!} 390 -870 0 0 1 1 {layer=5} 
T {This is the start schematic window specified in your .xschem file.
You may change this to any different schematic file or even start 
with an empty window. Just set the XSCHEM_START_WINDOW variable: 
- set XSCHEM_START_WINDOW \{mylib/0_top\}
if you want to start with an empty window: 
- set XSCHEM_START_WINDOW \{\}
On the left you see some sample circuits. You may descend into any 
of these by selecting one with a left mouse button click and 
pressing the 'e' key, or by menu 'Edit -> Push Schematic'.
You can return here after descending into a schematic hitting
'<Ctrl>-e' or by menu 'Edit -> Pop'
} 340 -800 0 0 0.4 0.4 {} 
T {Select the components here under
and press'<Ctrl>-h' or <Alt>-click
them to visit xschem websites
or local docs} 370 -410 0 0 0.3 0.3 {} 
T {Lines} 960 -460 0 0 0.6 0.6 {layer=4} 
T {Rectangles} 960 -400 0 0 0.6 0.6 {layer=4} 
T {Polygons} 960 -310 0 0 0.6 0.6 {layer=4} 
T {Electrical
wires} 960 -230 0 0 0.6 0.6 {layer=4} 
T {XSCHEM OBJECTS} 860 -510 0 0 0.6 0.6 {layer=7} 
T {Components} 960 -110 0 0 0.6 0.6 {layer=4} 
N 820 -200 940 -200 {} 
N 860 -240 860 -150 {} 
N 860 -170 920 -170 {} 
N 880 -210 910 -200 {} 
C {examples/poweramp} 150 -340 0 0 {name=x1} 
C {examples/tesla} 150 -260 0 0 {name=x2} 
C {examples/test_ne555} 150 -220 0 0 {name=x3} 
C {examples/test_lm324} 150 -180 0 0 {name=x4} 
C {examples/osc} 150 -300 0 0 {name=x5} 
C {examples/tesla2} 150 -140 0 0 {name=x7} 
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"} 
C {devices/launcher} 460 -310 0 0 {name=h8 
descr="XSCHEM WEBSITE" 
url="https://xschem.sourceforge.io/stefan/xschem.html"
} 
C {devices/launcher} 460 -210 0 0 {name=h1 
descr="XSCHEM ON SOURCEFORGE" 
url="https://sourceforge.net/projects/xschem/"
} 
C {examples/cmos_example} 150 -380 0 0 {name=x6} 
C {devices/launcher} 460 -260 0 0 {name=h2 
descr="LOCAL DOCUMENTATION" 
url="$env(HOME)/share/doc/xschem/xschem.html"
} 
C {examples/greycnt} 150 -420 0 0 {name=x8} 
C {examples/loading} 150 -460 0 0 {name=x9} 
C {devices/launcher} 70 -590 0 0 {name=h3 
descr="Toggle light/dark 
colorscheme" 
tclcommand="xschem toggle_colorscheme"
} 
C {examples/inv} 880 -100 0 0 {name=E1 TABLE="1.4 3.0 1.6 0.0"} 
