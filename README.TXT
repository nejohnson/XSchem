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
2.3.4:
- fix zoom full function, expecially when zooming back
  from a very very deep zoom in.
- avoid calling cairo_text_extents() and cairo_set_font_size() for
  extremely large characters (this happens when doing an extreme zoom in)
  as this causes Floating point exceptions on older cairo versions
- enable constrained move (vertical/horizontal) when drawing polygons
- better text alignent on rotated objects, when non-zero cairo_vert_correct
  parameter is defined in ~/.xschem
- lock hsize and vsize when entering text with cairo enabled, since there is
  no option in cairo text for different x/y scaling.
- code cleanups

2.3.3:
- do not try to draw polygons that are offscreen (discard by
  checking bounding box) - faster and no arithetic int16 overflow
- copy /paste was not implemented yet for polygons - fixed.
- bug fix for copy/save to clipboard of polygons
- select_all() was not updated for polygons - fixed.
- fix an error in the tcl code that prevented saving to libraries
  pointed to by symlinks (20171119 tag in xschem.tcl)

2.3.2:
- added polygon graphic primitive in addition to lines, rectangles and text.
  ctrl-w command or menu 'Tools-> Insert Polygon'. Operation is concluded by
  closing the polygon, i.e. placing the last point over the first, or
  by hitting the 'Return' key.
- added font="font name" property for text objects, allowing to change
  font on specific text objects. Use only monospaced types, expecially
  if you plan to rotate or flip the text, as bounding box calculation is not
  accurate for proportional typefaces.
- performance improvement: do not call Fill drawing routines for shapes that
  are on layers with no visible fill pattern defined.
- the 'set cairo_vert_correct x' (with x=positive or negative number)
  can now be set in the .xschem file to any value, text bounding box is
  shifted accordingly. This allows to change the vertical alignment of fonts
  for best appearance.
- white and dark colorscheme toggle View->Toggle colorscheme, Shift-C key)
- added 'xschem toggle_colorscheme' command
- removed obsolete and deprecated Tcl_GlobalEval, replaced with Tcl_EvalEx
- Tcl_Eval()s referring to TCL global vars changed to Tcl_EvalEx, so we ensure the
  script is run in the global scope.
- print error message if execution of xschem.tcl fails due to errors in the tcl code
  before dying. this way we have a clue to fix the error.
- custom colorschemes can be set in the ~/.xschem file: 
    set cadlayers 22
    set light_colors {
     "#ffffff" "#0044ee" "#aaaaaa" "#222222" "#229900"
     "#bb2200" "#00ccee" "#ff0000" "#888800" "#00aaaa"
     "#880088" "#00ff00" "#0000cc" "#666600" "#557755"
     "#aa2222" "#7ccc40" "#00ffcc" "#ce0097" "#d2d46b"
     "#ef6158" "#fdb200" }
  
    set dark_colors {
     "#000000" "#00ccee" "#3f3f3f" "#cccccc" "#88dd00"
     "#bb2200" "#00ccee" "#ff0000" "#ffff00" "#ffffff"
     "#ff00ff" "#00ff00" "#0000cc" "#aaaa00" "#aaccaa"
     "#ff7777" "#bfff81" "#00ffcc" "#ce0097" "#d2d46b"
     "#ef6158" "#fdb200" }

2.3.0:
- Antialiased CAIRO text for all XSCHEM elements!!
  if variable HAS_CAIRO is set in the Makefile all text processing in XSCHEM
  will be done using the cairo library.
- all text elements may specify a 'layer=<number>' in the property string box
  to specify a user layer color (instead of the default TEXTLAYER) to draw
  the text. Available only if cairo fonts enabled.
- a new command is added:
     xschem set cairo_font_name {<font name>}
  will set the font to be used for text rendering. Use only monospaced fonts
  to allow proper bounding box calculations.
  Example:
     xschem set cairo_font_name {DejaVu Sans Mono}
- various improvements in drawing engine (redraw selected elements on Expose events,
  this is needed if the server does not provide backing store).
- configuration variables controlling cairo fonts, may be placed in ~/.xschem:
  #### scale all fonts by this number
  set cairo_font_scale 1.0
  #### scale line spacing by this number
  set cairo_font_line_spacing 1.0
  #### specify a font (Use only monospaced fonts!)
  set cairo_font_name {Liberation Mono}
  #### lift up text by some zoom-corrected pixels for
  #### better compatibility wrt no cairo version.
  #### USE WITH CAUTION!!! big values cause text to
  #### go beyond the calculated bouning box, leaving garbage
  #### on the screen when moving objects/redrawing.
  #### Useful values in the range [-1, 3]
  set cairo_vert_correct 1
- following commands added to xschem tcl command, to runtime change these behaviours.
  xschem set cairo_font_name
  xschem set cairo_font_scale
  xschem set cairo_font_line_spacing
  xschem set cairo_vert_correct
- various bug fixes due to *BIG* cairo commit.

2.2.5:
- **LONG** standing crashing bug fixed in token.c --> subst_token(), causing
  xschem to crash on unusually large token strings, this can happen if a generic
  text file is loaded as a component property, instead of the usual 
  "name=xyyy tok1=val1 tok=val2  ..." property strings. tag 20171104
- some other bug fixes (removed sprintf of a string into itself :-) )
- when pressing the 'LOAD' button in tcl-tk text entries to load a file, 
  place the file starting at the cursor position instead of at the beginning.
- when an instance has empty property string do *not* preload the edit property text
  widget with a copy of the property string of another instance found in the schematic.
  This creates the illusion that the selected instance has a property string while
  it does not.
- added @tcleval(<<tcl script>>) tag that can be used in 'format',
  'vhdl_format', 'verilog_format' strings to allow dynamic netlisting.
  <<tcl script>> can be any valid tcl code, two variables are defined
  in the script context: 'symname' and 'instname', these variables can
  be used to query instance or symbol properties with the 'xschem' 
  builtin command.
  in <<tcl script>> spaces and quotes must be escaped.
- Added 'xschem getprop' command:
  * xschem getprop instance <<instname>> <<property>>
    returns instance property
    example: xschem getprop instance m3 w --> returns mos width
  * xschem getprop instance_n <<instnum>> <<property>>
    same as previous, but use instance index number instead of name
  * xschem getprop symbol <<symname>> <<property>>
    returns symbol property
    example: xchem getprop symbol devices/nmos format 
             --> return format property string of symbol
  * xschem getprop instance_net <<instname>> <<pin>>
    returns net name attached to <<pin>>
- code cleanup (removed obsolete comments)

2.2.4:
- Better drawing when selecting by area / unselecting by area. Selection drag
  rectangle no more clutters the underlying already selected stuff.
  When unselecting (Shift-right button drag) do a live update of objects as soon
  as they get unselected.
- hilight_inside() function renamed to select_inside. the previous ambigous name
  really pissed me off.
Stefan 20171027
2.2.3:
- Added menu commands 'New Schematic'; and 'New Symbol' so the
  'current_type' global is correctly set for schematic or symbol 
  editing and saving.
- put global properties in G {} field when saving as symbol, take 
  global sym props from schprop, schvhdlprop, schverilogprop, the first
  one that contains 'format=' or 'type=' fields
Stefan 20171025
2.2.2:
- added easy keybind customization in the ~/.xschem init file. 
  See following examples:
  ## replace Ctrl-d with Escape (so you wont kill the program)
  set replace_key(Control-d) Escape
  ## swap w and W keybinds; Always specify Shift for capital letters
  set replace_key(Shift-W) w
  set replace_key(w) Shift-W
- Documentation added for keybind customization.
2.2.1: 
- some regressions in bindkeys fixed
2.2.0:
- Added description option -no-pie in Makefile.
  if a profiled executable is needed '-no-pie -pg' must be given.
  Position Independent Executable is now the default on Gcc, 
  but gprof will not work with this default.
- xschem will not descend or go up from a schematic hierarchy level
  if it has been modified, the user wants to save changes but the
  file can not be saved, due for example to permission problems.
  In this case the user either needs to save to a different place
  or say 'no' when prompted for save.
- bindkey 'W' 'show discrepancies of nets vs pin names on selected
  instance' changed to 'X'
- added the 'snap wire' function (uppercase) 'W' bindkey, the wire
  starts/ends from/to closest instance pin or net endpoint.
- fixes in the undo buffer, some actions were not undoable.
- bindkey 'H' for 'execute tclcommand or open file or
  http link (tclcomand or url properties) changed to 'ctrl-h'
- bindkey 'h' for 'attach nets to instance pin' changed to 'H'
- added constrained horizontal ('h' key) or vertical ('v' key)
  movement for wire, line, object copy/move.
- added menu entries for png/pdf/svg print, h/v constrained move,
  collapse wires, snap wire creation.
- '-z' xschem option now works again, this will start xschem with
  a rich rainbow colored layer set.
- Documentation updates.

2.1.14:
- Documentation updated (component pin auto labeling)
- added option '-S' to launch a simulation in batch command mode:
    'xschem -q -n -s -S examples/poweramp'
  will do a spice (-s) netlist (-n) and launch simulation (-S), then quits (-q) 
- added option '-W' to launch waveform viewer
    'xschem -q -s -W examples poweramp'
  will bring up the default spice wave viewer and quits.
- do not open a confitmation dialog asking to save if TCL 'xschem save' is given
  This will allow to build tcl scripts without needing user confirmation.
- tcl command 'xschem select' now allows to select an instance by specifying its instance name
  (instead of its numeric position into the inst_ptr array, still allowed)
    'xschem select xampli'
  will select instance 'xampli' if found in current schematic
- Added #define __USE_GNU to allow usage of 'strndup' function (POSIX 2008) in older systems
- improvements on the 'create pins to selected instance' command ('h' key) 
  a dialog box asks if pins need to be prefixed with a string, also allow
  to choose lab_pin or lab_wire net labels.
- Control-return and Escape bindkeys added to most toplevel dialog boxes
  to allow quickly closing unwanted dialog boxes.
- devices/lab_wire library symbol flipped horizontally to align orientation with lab_pin
- documentation about automatic symbol generation
- added a 'Make schematic from symbol' (Ctrl-l) menu entry.
  if one symbol is selected and this function is invoked (ctrl-l)
  a schematic view (.sch) will be automatically created from the
  symbol view. The schematic will only contain the interface
  pins / generics (if any). This will overwrite the symbol schematic
  view if it exist. A warning is issued requesting user confirmation.
- some cosmetic additions (dialog titles) in xschem.tcl
- fixed a crashing bug  when the 'create pins from highlight nets'
  (ctrl-j) is invoked after hilighting some nets and deleting a
  subset of those.

2.1.13:
- Added binding for the 'ghdl' free VHDL simulator, update your ~/.xschem file to
  set pathnames and defaults.
- added a VHDL testbench ('loading') in the examples library to test ghdl
- small fixes in the 'vhdl.awk' netlist post processor

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



