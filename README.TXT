XSCHEM : schematic capture and netlisting EDA tool

Xschem is a schematic capture program, it allows creation of hierarchical representation
of circuits with a top down approach . By focusing on interfaces, hierarchy and instance
properties a complex system can be described in terms of simpler building blocks. A VHDL
or Verilog or Spice netlist can be generated from the drawn schematic, allowing the 
simulation of the circuit. Key feature of the program is its drawing engine written in C
and using directly the Xlib drawing primitives; this gives very good speed performance,
even on very big circuits. The user interface is built with the Tcl-Tk toolkit, tcl is
also the extension language used.

INSTALLATION AND MANUAL

https://xschem.sourceforge.io/stefan/xschem.html

BUILDING

go to the 'src' directory, edit the Makefile, in particular the 'prefix' and the
'library_prefix' variables, also check that the TCLVER variable matches the tcl version 
installed on the system.

In order to build xschem you need
- gcc
- tcl-tk and development header files (tcl8.x, tcl8.x-dev, tk8.x, tk8.x-dev)
- bison grammar parser
- flex lexycal analyzer
- an awk implementation (gawk or mawk are OK)
- X11 and development header files
- libxpm and development files (libxpm, libxpm-dev)

If all dependencies are met 'make' will build the program

INSTALL

If destination paths are correctly set in the Makefile: 'make install'

CHANGELOG
2.1.12:
- fixed a bug causing incorrect VHDL netlist when bussed nets or bus slices are
  connected to primitive VHDL symbols with behaviour described with vhdl_format.
  Thanks to Svenn Are Bjerkem for reporting the problem.

2.1.11:
- When saving a newly created schematic (a schematic never saved before) using 
  the 'File->Save' function, revert to a 'Save As' operation asking for a correct filename
  Thanks to Svenn Are Bjerkem for pointing out the issue.

2.1.10: 
callback.c: 
- filter out NumLock status in event processing (20170511)
  Thanks to Saul Rodriguez for pointing out the issue.
- added 'Alt + Right-Mouse-Button' to activate the 'launcher' feature on
  selected component instance (also bound to 'H' key)

2.1.9:
- added 'propstring' search key to search patterns into the whole instance 
  property string instead of looking into single property tokens
- added the following instance properties:
  - 'url' : specify an url to open when pressing the launch ('H') key on an instance
  - 'program' : specify a program to use for opening the file (default if missing: 
    xdg-open) 
  - 'tclcommand' : execute a tcl script
  these properties allow the creation of 'launchers', special instances that when
  triggered with the 'H' key perform some actions (open documentation, datasheets, 
  view data, run special viewers, bindings to new simulators, ...)
  the special 'launcher' component in the 'devices' library can be used for this purpose.
  All these properties can be attached to any schematic component.
- fixes in the spice.awk netlister (20170407) fixed wrong netlisting for ammeters (current 
  probes) placed on bussed wires.
- token.c: get_tok_value: recognize escaped backslashes (\\), return single backslash (\)
  in the value string.
- .xschem config file: removed 'hspicerf_simulator', 'modelsim_simulator', added
  'spice_simulator', 'verilog_simulator', 'finesim_path', 'finesim_opts' variables.
  previously the default spice simulator was hspice and optionally hspicerf was used when
  hspicerf_simulator was set to 1. This is unflexible, the new approach is to specify 
  the name of the simulator to use: (set spice_simulator hspice).
  Same approach for the verilog simulator (set verilog_simulator iverilog)
  Added finesim spice simulator (set the 'finesim_path' variable and set 
  spice_simulator to finesim).
- some cosmetic changes in the menus (radiobuttons for viewer/simulator/netlist format)

2.1.8: 
- Fixed a segfault regression, some minor fixes in the code.
- Fixed wrong search order of the .xschem file. The correct
  behavior is to look in ${PWD} for an initialization .xschem 
  file, and then (if not found) look in ${HOME}.
  Give up execution if ${HOME} is not defined.

2.1.7:
- Fixes some wrong references to symbols in the provided example schematics
- Small documentation updates.

AUTHOR

Stefan Frederik Schippers
stefan.schippers@gmail.com



