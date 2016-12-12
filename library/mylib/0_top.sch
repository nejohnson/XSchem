G {} 
V {} 
S {} 
T {Welcome to XSCHEM!} 530 -860 0 0 1 1 {} 
T {This is the start schematic window specified in your .xschem file.
You may change this to any different schematic file or even start 
with an empty window. Just set the XSCHEM_START_WINDOW variable: 
- set XSCHEM_START_WINDOW \{mylib/0_top\}
if you want to start with an empty window: 
- set XSCHEM_START_WINDOW \{\}
On the left you see 6 sample circuits. You may descend into any 
of these by selecting one with a left mouse button click and 
pressing the 'e' key, or by menu 'Edit -> Push Schematic'.
You can return here after descending into a schematic hitting
'<Ctrl>-e' or by menu 'Edit -> Pop'
} 370 -780 0 0 0.4 0.4 {} 
T {Select the component here under and press
'<Shift>-H' to visit xschem websites
or local docs} 420 -430 0 0 0.7 0.7 {} 
C {examples/poweramp} 150 -340 0 0 {name=x1} 
C {examples/tesla} 150 -260 0 0 {name=x2} 
C {examples/test_ne555} 150 -220 0 0 {name=x3} 
C {examples/test_lm324} 150 -180 0 0 {name=x4} 
C {examples/osc} 150 -300 0 0 {name=x5} 
C {examples/tesla2} 150 -140 0 0 {name=x7} 
C {devices/title} 160 -30 0 0 {name=l1 author="Stefan Schippers"} 
C {devices/launcher} 750 -260 0 0 {name=h8 
descr="XSCHEM WEBSITE" 
url="stefanschippers.host-ed.me/stefan/xschem.html"
} 
C {devices/launcher} 750 -150 0 0 {name=h1 
descr="XSCHEM ON SOURCEFORGE" 
url="https://sourceforge.net/projects/xschem/"
} 
C {examples/cmos_example} 150 -380 0 0 {name=x6} 
C {devices/launcher} 750 -210 0 0 {name=h2 
descr="LOCAL DOCUMENTATION" 
url="$env(HOME)/share/doc/xschem/xschem.html"
} 
