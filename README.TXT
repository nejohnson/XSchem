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
- flex lexycal analyzer and libfl-dev header files
- an awk implementation (gawk or mawk are OK)
- X11 and development header files
- libxpm and development files (libxpm, libxpm-dev)
- libcairo2 and cairo2-dev development files (if antialiased fonts 
  are compiled in).

If all dependencies are met 'make' will build the program

INSTALL

If destination paths are correctly set in the Makefile: 'make install'

CHANGELOG

2.8.2
faq about automatic rename / allow duplicate refdes.

remove references to 'Shift-J' in docs (removed messy code), better appearance for 'list_dirs' dialog.

added License/Copyright info for scconfig (GNU LGPL2).

no save schematic before editing disk file - this is 
an expert command so user should decide himself what to do.

2.8.2_RC4
removed gensch procedure (create sym from pinlist, un-maintainable mess)

do not print 'conn' lines for unconnecteds in tedax netlist

Edit -> Push Symbol menu was not functional.

fix segfault when placing symbols with off-bound @#<n>:<attr> attributes (n referencing a non existent pin)

allow/forbid duplicated refdes, highlight/rename duplicates, directory chooser when inserting symbols

xschemrc variable 'disable_unique_names' may be set to 0 in xschemrc to forbid duplicated refdes
- default is to allow duplicates (1)
- menu 'Highlight -> (Highlight|Rename) duplicate instance names' can be used to flag / correct duplicate names.
- when inserting components (Insert key) present a list of directories to start fileselector from.

disabled all testmodes

align to grid function: for pin objects align the center point of the rectangle instead of the (x1,y1)-(x2,y2) points.

Create symbol tutorial in xschem docs

alert if trying to open non existent symbol

allow @#drain:pinnumber to be used in sybols to print 'pinnumber' attribute of pin with 'name=drain' in addition to 
@#3:pinnumber (print 'pinnumber' attribute of 3rd pin of symbol)

removed 'string cat' as it is a tcl8.6 addition (not needed anyway)

Add $DESTDIR prefix to system_library_dir in Makefile.conf.in as it is used in trunk/xschem_library/Makefile to install files.

--plotfile on cmdline to specify image export output file. Valid for all formats (png, svg, pdf)

calculate correct line widths before rendering to screen / plot images

fix: start numbering new instances from 1 instead of 0 
xschem.1.in manpage and expand $PREFIX

fix rotate /flip without moving when multiple objects selected.
(in this case do not rotate individual objects around their anchor points)
doc fixes

doc and symbol updates

Allow pinnumber(name) in instance attributes to change symbol pinnumber, 
in addition to pinnumber(n) where n is xschem pin index number. Syntax changed from pinnumber_n.

Allow mouse panning schematic while editing properties.
Do not allow some command nesting combinations  that could be dangerous.


Tutorial on creating a simple schematic and running a simulation with ngspice
Some fixes on UI (bindkeys) 

generic, 'light' models created for testing tedax moved from pcb/ to devices/

added @spiceprefix to symbols that might need a different spice prefix
if spiceprefix unspecified at instance level no change wrt previous behavior.

netlist_dir default set to '.' (current directory, where xschem is launched) if not defined in xschemrc

option -o <dir> (--netlist_path=<dir>) can be used to set output netlist *directory* with cmdline

Token subsitution now correctly replaces @token1@token2 with their values 
(no separators needed, second @ is an implied token separator and next token start)

allow set / change selected attributes on multiple wselected ires/rects/lines

leave post mortem shell open in case of ngspice fatal errors, in interactive ngspice sims.

trunk/utile dir moved under trunk/src/utile so it will be in the same
relative position wrt XSCHEM_SHAREDIR whether xschem is run from
build trunk/src directory or after installation.

make uninstall fixes.

2.8.2_RC3
- commercial simulators/viewers not available in xschem menus if their 
  *_path variables not defined in xschemrc. Better quoting for allowing filenames with spaces
- removed potentially proprietary models from example schematics.
- copyright info updated to 2019
- objects can now be flipped/rotated (R,F keys) without initiating a move operation.
- Right mouse button brings up edit attributes of object under mouse,
  ctrl-button1 unselects object, ctrl-shift-button1 drag unselects by area
  netlisting mode shown in status line, no more alert box when switching
- fix some inaccuracies in menu keybindings
- doc cleanup, added first annoying FAQ: http://repo.hu/projects/xschem/xschem_man/faq.html
- step-by-step tutorial for installing xschem: http://repo.hu/projects/xschem/xschem_man/tutorial_install_xschem.html
- xschemrc comments and sample lines aligned to current xschem behavior

2.8.2_RC1:
- improvements of xschem TCL API for backannotation
- refdes (instance name) automatic numbering starts from 1 instead of
  0 (D1, D2, ...) 
- ngspice integration
- documentation updates
- save only one copy of embedded symbols even if multiple instances of it have the
  'embed=true' attribute set
- allow arbitrary spaces around '=' in attribute assignments 
  (name = x1 footprint=  TO220 model= IRF540)

- '.' in path search list is now the location of the containing schematic, not the
  directory from where xschem was launched (thanks to Igor2 for consulting).
- Default search list if running xschem from build directory is 
  '.:<path>/xschem/trunk/xschem_library/devices', so all examples will run out
  of the box with no config file tweaking.
- tedax netlist: instance property 'pinnumber_n' where n is the index of pin (0, 1, 2, ...) may override pin 'pinnumber' 
  of symbol pin with index 'n'
- tedax netlist: instance property extra_pinnumber="idx1 idx2" may override symbol inherited
  connections (usually supply and ground pins for slotted devices) 
- improvements and cleanup in xschem query language (xschem pinlist)
- print index number (order in xschem array) of selected objects
  in status line
- version info added as first line when saving .sch/.sym files.
  v {xschem version=2.8.2_RC1 file_version=1.0}
  for now no checks are made when loading files, version string from file 
  is just stored for later usage.
- documentation updates, example files updates, variable expansion in launcher() 
  so XSCHEM_SHAREDIR and HOME can be used in url attributes
- fullscreen moved from options to View menu, more appropriate.
- make default compile time XSCHEM_LIBRARY_PATH definition available 
  to xschemrc so a directory can be prepended or appended to it
- replaced execlp with execl everywhere
- hooks.c: default to ~/.xschem/xschem_library if no --user-lib-path given in configure.
- examples/diode.* renamed to examples/diode_1.* to avoud clashing with devices/diode.sym.
- If no search path (XSCHEM_LIBRARY_PATH) defined in xschemrc default to compile time 
  with '.' prepended
- Template xschemrc with better comments by Richard Lucassen (thanks!)
- xschem_library/Makefile: use $system_library_dir for system libraries (devices)

2.8.1:
- connecting dots are highlighted together with the hilight net.
- avoid shebangs when launching scripts, rely on host PATH variable 
  to lookup the correct (awk) interpreter
- extensive debug to catch and fix memory leaks / pointer deallocation
- search function improved, if empty value is given then look 
  for objects that *have* search key in property string.
- 'DELETE' attribute can be used to erase an attribute from a 
  multiple selection of etherogeneus objects, if 'preserve unchanged props'
  is enabled.
- Added a 'dim color' slider (View menu) to adjust brightness.
- xschem - tcl API improvements for spice operating point current / voltages backannotation
- ability to do block deletion of a specific set of attributes on selected set
  (set attribute values to 'DELETE') 
- when drawing selected elements do not display bbox of empty symbol strings
- xschem tcl API improvements:
  * xschem set no_draw 1|0   --> used to disable screen redraw, to speed up complex scripts
  * xschem set no_undo 1|0   --> used to disable undo, to speed up complex scripts
    it is wise to do a 'xschem push_undo' before doing this
  * xschem get expandlabel <net label> --> expands bus notation:
    example: xschem get expandlabel S[4,2:0] will return S[4],S[2],S[1],S[0] 4
    that is the expanded bits and the number of bits.
  * xschem get sch_path --> return the current hierarchy path:
    example: xinput.xreg.xlat
  * subst_token() now allows to set a token to an empty value (new_val="") or delete the token
    (new_val = NULL)
- tcl command 'xschem set renumber [1|0]' allows to do automatic renumbering
  of instance names (refdes) when loading schematics with duplicated inst names
- View->Change current layer color' menu added allows to change layer colors
  changes are saved in ${USER_CONF_DIR}/colors
- added '--rcfile <filename>' for custom rc file loading
- 'about xschem' dialog added
- add '*' in window title bar / icon when schematic is in modified state and need save
- default file name to 'untitled.sch' when creating a new schematic
- long option syntax (--version etc)
- make xschem compile with std=c89 -pedantic
  no snprintf(), no // comments, no VLAs, no getopt(), no strtok*()
- cirles / arcs in pdf / svg export
- option for in-memory undo in addition to file backed undo (#define IN_MEMORY_UNDO)
- Menu 'Tools->Remember last command' and 'set persistent_command [1|0] in xschemrc
  to repeat last wire/line command just with a mouse click
- removed copyrighted material (sample spice models) 
- keybinding changes: Corl-Q for exit, Ctrl-N for 'New schematic'
- better checking on malformed input (discard vs crash) 
- Menu command Tools->Join/Trim wires (remove overlapping wires, join parallel wires etc)
- Menu command Tools->Break wires (break wires at component pins, or 'T/Y intersections)
                      this will work on *selected* wires.

2.8.0
- draw connecting dots automatically on T or Y connected wires,
- new commands in Tools menu to trim and break wires
  Break wires will break *selected wires* at instance pin / other net connettions.
  Trim wires will resolve overlapped nets, join parallel nets, etc.
- .xschem/ directory for private data and config files.
- initfile renamed, from .xschem to .xschem/xschemrc
- XSCHEM_HOME_DIR renamed to XSCHEM_SHAREDIR (sharedir in Makefile)
- added USER_CONF_DIR (Makedile userconfdir) (~/.xschem) 
- XSCHEM_DESIGN_PATH renamed to XSCHEM_LIBRARY_PATH (makefile xschem_library_path) 
- make xschem work correctly with all combinations of HAS_XCB and HAS_XRENDER
- optmizations in update_conn_cues()
- no recursive functions for deleting hash tables.
- simpler loops for iterating rectangles that contain a wire in spatial hash table.
- different location of startup user rc files: .xschem/xschemrc instead
  of .xschem
- deleted utile junk old files
- xschemrc moved under src/

2.7.0:
- some fixes in library path management (abs_sym_path and rel_sym_path in xschem.tcl)
- documentation updates on required -dev files, libfl-dev and cairo2-dev
- documentation updates on open/closed polygons
- embedded symbols, (embed=true attr on instances), documentation updated.
- Status bar grid entry configured as disabled as it is output only
- make xschem accept some malformed input files, like T{} ... or leading spaces before element tags.
- code in place for command to cut wires at instance pins and intersections
- created iterator function for spatial wire & inst hash tables to avoid
  code duplication
- removed exec flag on all doc/ files, moved xschem.html to index.html
- keep calculating bbox for unusually big texts, even if these text are not
  displayed being fonts too big (crash on old cairo libs)
- corrected menu accelerators shown for zoom / unzoom.
- unzoom_nodrift tcl variable can be set to 1 or 0 to produce slightly
  different unzoom behaviors.
- flush arc drawing routines (draw[temp]arc(END,...) in addition to lines
  and rectangles when doing selections /hilight. (forgotten since arcs
  were introduced).
- nets are drawn after the other elements so they are always on top.
- new xschem TCL commands
 - xschem setprop instance x25 w 55
   replaces property w of instance x25 with new 55 value
 - xschem setprop instance_n 123 w 55
   same as above but indicate instance number (position in inst_ptr array)
 - xschem replace_symbol instance x25 .../../new_symbol.sym
   replaces symbol of x25
 - xschem replace_symbol instance_n 123 .../../new_symbol.sym
   same as above but indicate instance number (position in inst_ptr array)
- new commandline option '-i' to tell xschem not to load init files (xschemrc and .xschem)
- order coordinates before storing nets in load_wire(),
  for some reason this trivial thing was missing
- drawing glitches fixed

2.6.8:
- Pressing 'space' while drawing a net or a line switches between horizontal-vertical
  manhattan path, vertical-horizontal path or oblique path.

2.6.7:
- in 'persistent command' mode wire or line placement continues after a double click
  so it is possible to place multiple unconnected nets / lines
- no more dialog on grid change. grid is shown in status bar

2.6.6:
- double click to terminate a polygon placement
- unzoom and zoom now correctly unzoom/zoom around the mouse pointer with no drifts
- ordered menu items in window so using keys switches menus in a predictable way
- Double click no more brings up edit property dialog, control-double-click is needed.
- 'persistent command' checkbox in Tools menu, if checked LINE and WIRE placement continues
  until an Escape or a double click is given. Default value can be set in .xschem rc file.
- line clipping now clips lines to window size and not to currently set bbox. This avoids
  off by one pixel calculations leaving garbage on screen in some cases.

2.6.5:
- Added arcs / circles.
  Arcs are started with the 'Shift-C' key, while circles are entered with 'Ctrl-Shift-C'

2.6.2:
- Open polygons are now supported
- Button3 for terminating an open Polygon placement
- Return key for terminating a close polygon placement 
  (this can be done also by clicking the last point on the first polygon point).
- some drawing fixes when highlight nets are shown.
- Waves, Simulate and Netlist menu buttons with borders so they look different
  from menu entries.
- Merge schematic (copying across different xschem instances) was not cacihng correctly
  the 'fill attribute for polyons.
- save/load widgets: no change initial dir if user cancells action
- 'button' look for 'Netlist', 'Waves' and 'Simulate' menu buttons.

2.6.1:
- remove copying .xschem into home directory from the Makefile, this was 
  only for testing and if not properly configured will cause xschem 
  to look in the wrong places for schematics

2.6.0: 
- small change in Makefile: when doing 'make install' copy also sample .xschem in $HOME/
  so installation will be ready to use without user having to manually setup the .xschem
  file.
- Documentation updates
- Makefile passes PREFIX and XSCHEM_DESIGN_PATH cpp variables to xschem
  for default path setting
- small bug fixes on the new drawing mode: 
  pixmap -> 'XCopyArea' -> screen
- xinit.c sets empty XSCHEM_DESIGN_PATH before reading startup files (xschemrc and .xschem) 
  so variable can be appended without complains from the tcl interpreter
  (variable non existing)
- allow direct execution of xschem from within the src/ directory for testing purposes.
  removed special startup handling if xschem run from the parent of src/ dir.

---

- xschem now understands a list of directories for symbol files, this list is set
  as a colon separated list of paths in XSCHEM_DESIGN_PATH.
  this tcl variable may be set in the .xschem or system xschemrc files, or via env variable.
  if no setting is done then xschem uses the compile time default, unless it is started
  from a  source build directort (see below).
  the old XSCHEM_DESIGN_DIR (now deprecated) is still understood by xschem, this variable
  may specify only one directory.

- reverted previous change 'tcl current working dir changed to directory of
  loaded schematic', not necessary and dangerous as it makes xschem
  behaviour unpredictable.

- Makefile variables passed to XSCHEM:
    PREFIX: root directory of xschem installation
      (example: /usr ,  /home/user)
    SYSTEM_LIBRARY_PATH: system-wide directory of xschem symbols / schematics
      (example: $PREFIX/share/xschem/xschem_library )
    USER_LIBRARY_PATH: directory of user schematic / symbol files
      (example: $HOME/xschem_library)

- xschem reads first xschemrc file in system xschem directory (.../share/xschem/xschemrc)
  and then .xschem in current directory or in home directory.

- *NEW* drawing model (enabled by default) that avoid drawing to window, draws only to save
  pixmap, using XCopyArea to paint the screen. This avoids flickering when moving/zooming
  or panning the schematic. Previous drawing model (still available via menu) used
  to draw both in window and save pixmap. Depending on X server performance choose the
  method that runs best.
- bug fixes in 'create symbol from schematic'

- startup sequence of xschem:
 * look for XSCHEM_HOME_DIR from environment variables.

 * else if there is ../src/xchem.tcl with respect to current dir and
   ../xschem_library then we are starting
   from a build directory, set XSCHEM_HOME_DIR to `pwd` and
   also set XSCHEM_DESIGN_PATH to `pwd`/../xschem_library.

 * else if there is a PREFIX/share/xschem directory then set XSCHEM_HOME_DIR
   to PREFIX/share/xschem.

 * source XSCHEM_HOME_DIR/xschemrc if existing.

 * source `pwd`/.xschem or $HOME/.xschem if existing.

 * if XSCHEM_HOME_DIR not defined --&gt; error and quit.

 * look for XSCHEM_DESIGN_PATH from environment variables. This will override any
   value given in  xschemrc and .xschem startup files.

 * if XSCHEM_DESIGN_PATH undefined set it to compile time default provided by Makefile:
   xschem_library_path.

 * source $XSCHEM_HOME_DIR/xschem.tcl.

 * start loading user provided schematic file or start with empty window  (or filename
   specified in XSCHEM_START_WINDOW tcl variable).

--- 

- all string args passed from C to tcl encloded in {} braces to allow spaces
  in file and library names
- tcl current working dir changed to directory of loaded schematic to make relative symbol
  resolution work the same regardless of the directory xschem was started from.

--- 

- all path manipulation goes through tcl functions resolve_filename and abs_path_of_symbol,
  no direct manipulation of XSCHEM_DESIGN_PATH in .c or .tcl files.
  This is in preparation of multiple path list in XSCHEM_DESIGN_PATH, requiring
  changes only in those 2 functions.
- temp files for editing attributes with editor now in XSCHEM_TMP_DIR instead of XSCHEM_DESIGN_PATH

--- 

- new_window(): if opening a schematic in a new window change .sym extension
  to .sch extension, 20181006
- get_resolved_symbol(): recognize parent dir and replace absolute path to parent dir with ../
- load_inst() use get_resolved_symbol() to remove absolute paths when possible, 20181006

--- 

- XSCHEM_DESIGN_DIR renamed to XSCHEM_DESIGN_PATH as it should be a list of multiple 
  library paths to search for.
- created a small function, source_tcl_file() for sourcing tcl files and do error checking.
  used to load  xschemrc / .xschem / xschem.tcl files

- look for xschemrc in {PREFIX}/share/xschem for system wide options, before looking in $pwd/.xschem
  or $HOME/.xschem for user options.

- look for xschem.tcl in $PWD or  XSCHEM_HOME_DIR if this tcl var is defined (or env var defined)
  otherwise source from {PREFIX}/share/xschem.

- library directory renamed to xschem_library

- if no .xschem is found and no XSCHEM_DESIGN_PATH defined set xschem symbol library to 'xschem_library'
  in current working directory. (good for testing xschem builds before install) or in ../xschem_library

--- 

- fixed an off-by-one error in selecting a random char from charset[] in random_string(), leading
  to terminating \0 sometimes chosen as part of the random string.

--- 

- tclsetvar() wrapper for all Tcl_SetVar() calls.
- ask_new_file() calls get_resolved_symbol(fullname) to reduce 
  filename to library/symbol if located in standard xschem library
  or just symbol.sym if in current directory, in order to avoid
  as long as possible absolute paths.
- xschem .selection and .clipboard files renamed to ~/.xschem_selection 
  and ~/.xschem_clipboard
- TCL XSCHEM_TMP_DIR created to indicate tmp directory (default /tmp) 
  this can be set in user's .xschem.
- store_polygon() was not propagating the .fill member from 'fill' property
  this resulted in no fill pattern on polygons with fill=true property set
  when changing polygon layer.
  (store.c, 20181002)
- removed all strcpy() calls, use my_strncpy()

--- 

- all Tcl_Eval(), Tcl_EvalEx() called with tcleval() that does error checking.
- all Tcl_GetVar replaced with tclgetvar.
- my_free() now frees the memory and sets ptr to NULL, called with &ptr.
- removed all sprintf, replaced with my_snprintf.
- remaining malloc()s replaced with my_malloc().
- undo/redo handling now done with popen()/pclose() instead of pipe(), fork(),
  dup2(), fdopen(), freopen(), waitpid(), for better portability.
- @#n:property syntax is accepted in print_tedax_element() for format string, 
  this allow to print in netlist attribute values specified on pins
  @#3:pinnumber --> print 'pinnumber' attribute set for pin #3 of symbol.
- removed some variable length arrays in token.c
- tcl make_symbol fixed t work on schematics located anywhere in the filesystem

--- 

- many static (char *name) pointers in functions replaced with 
  scoped arrays (char name[]) for efficiency reasons
  path arrays are of type 'char filename[PATH_MAX]'
- added ability to load schematics and components located anywhere in the filesystem
  for components the (libname/symbolname) is still supported but new syntax is 
  to specify relative or absolute names ([/][..../..../]symbol.sym) 
  if specified symbol is in the standard xschem libraries (XSCHEM_DESIGN_DIR)
  it is converted to library/symbol:
  Example: 
    /home/schippes/xschem_library/devices/npn.sym --> devices/npn
  If a symbol located in the current directory  is placed into the xschem window
  all its path components will be stripped off: 
  Example:
    /home/schippes/xschem-code/test.sym --> test.sym (if CWD is /home/schippes/xschem-code).
  next feature (still tbd) will be to search a list of paths for sym libraries.
- more reliable detection of PWD and HOME (getcwd, getenv, getpwuid)
- cleanup in tcl save & load dialogs, now we only have save_file_dialog and load_file_dialog.

--- 

- draw(): don't use spatial hash table for small schematics (<2000 instances, <2000nets).
- xschem now loads filenames with absolute paths, so any schematic (even outside
  XSCHEM_DESIGN_DIR) can be given.
- less usage of strlen() reusing size information already available
- get_tok_value() sets get_tok_value_size to the size of the returned token value.

--- 

- xschem now compiles with -std=c99 and 
- double fork method to spawn a new xschem instance to avoid zombies
- all snprintf calls wrapped in my_snprintf
- rint() replaced with round()
- some // comments changed in /* ... */
- my_strdup() returns size of dup'd string instead of void
- all strdup() calls wrapped into my_strdup() which in turn uses malloc() + memcpy()
- all strndup() replaced with my_strndup() which in turn uses malloc() + memcpy()
- all strcat() replaced with my_strcat() which uses realloc() and memcpy()
- all strcpy() replaced with memcpy() since size of src string is known.
- Corrected a long standing typo: FONTDESCENT+1i, where 1i is considered as imaginary number
  Thanks to Ian Van der Neut for pointing out the problem.
- removed mkstemp, mkdtemp, replaced with self made create_tmpfile and create_tmpdir
- added '%option never-interactive' in parselabel.l so it avoids usig fileno()
  which is not needed and creates portability issues
- cleanups in save/load/merge procedures

--- 

- flatten.awk changed to awk hasbang script instead of sh --> awk 

--- 

- pass PREFIX and LIBRARY_PREFIX from Makefile prefix and library_prefix variables
- check if symbol pointer is set in spatial hash table drawing (draw.c, tag 20180921)
- added some const type declarators: 
   *bus_hilight_lookup(const char *token...)
    const char *expandlabel(const char *s, int *m)
  and other const declarations here and there 
- save_ascii_string(): no trailing space added.
- load_symbol(): if symbol definition contains components link them with corresponding symbols
  (although a symbol should not contain nets / components), tag 20180921
- some comments / cleanups in xinit.c
- fixed menu accelerators shown in 'File' menu to reflect recent keybinding changes

--- 

2.4.9:
- ability to swap mouse buttons using tcl key_binding proc
  NOTE: it is not possible to replace Button1 since it is sent in Motion events as well.
- clear_drawing() did not delete spatial hash tables leading to segfaults when doing File-> New schematic
- swapped Button4/5 (wheel zoom)
- changed some key bindings to more standard values:
  - ctrl-shift-s --> Save As
  - ctrl-s --> Save
  - ctrl-o --> Open
  - ctrl-alt-s --> save as symbol
  - shift-s --> change element ordering
- fill=true property for polygons to indicate fill is wanted
  default polygons are not filled.
- tcl key_binding procedure allows to change/swap mouse buttons
- save.c: removed code duplication by creating a 'read_xschem_file() function that read files.
- removed some dead /commented old code
- get_tok_value(inst_ptr....prop_ptr, "name"...) replaced with inst_ptr[..].instname (20180911)
- fix crashing bug in hilight.c when instances with empty property string are traversed during a search
  20180906
- new '@#n format allowed for referencing a net attached to a pin in symbol format strings.
  This pattern can be used in 'format, 'vhdl_format', 'verilog_format', 'tedax_format'
  global attributes of symbols.
  This pattern is faster than the traditional '@@pin_name' syntax since xschem does not need to
  search all pins in symbol for a name matching 'pin_name'
  Example: verilog_format="assign @#2 = @#0 ;"
    in this case @#2 expands to the net name attached to pin stored at position 2 (starting from 0) in xschem,
    (that is the third symbol pin), @#0 expands to net name attached to first pin of the symbol
- do not print escaping backslashes in print_verilog_primitive, print_verilog_primitive, (20180911)
- new '#@n:pin_attr' format allowed in text objects placed in symbols, this will expand
  to the value of attribute 'pin_attr' stored in the property string of the 'n-th' pin of symbol.
  Example: @#0:pinnumber
  if pinnumber of pin 0 is defined as 'pinnumber=1:4:9:12' and symbol instance name is 'U1:4'
  xschem will resolve slot information (4) and replace the '@#0:pinnumber' with '12'.
  this is used to correctly draw correct pin numbers for slotted devices.

2.4.8:
- added (experimental for now) tEDAx netlist export, this is a netlist format that
  the pcb-rnd printed board editor understands.
  sample test schematic included in the source distribution (pcb_test1.sch)
- better lookup of spice_ignore, verilog_ignore and vhdl_ignore for netlisting,
  after looking in instance properties look also in symbol definition.
- fixed a bug in TCL savefile procedure, where it refused to save a schematic
  in a library pointed to by a symlink
- Escape key no more closes a property edit dialog (proc text_line) if something has been written
  into it.

2.4.5:
- 20180208 fix crashing bug: delete node info if changing symbol
  if number of pins is different we must delete these data *before*
  changing ysmbol, otherwise i might end up deleting non allocated data.
Stefan, 20180417

2.4.4:
- improvements in spice.awk, do not clobber with user defined code 
- re-enabled backing store, (xserver takes care of saving obscured window portions)
  this was disabled to ensure correct response of XSCHEM to expose events after
  some changes in the drawing engine.
Stefan, 20180203

2.4.3:
- devices/code.sym component added: this component has a 'value' property that may be
  used to place arbitrary code (spice/verilog/VHDL code) into a schematic. Multiple
  'code.sym' components may be used, their 'value' properties will be concatenated
  into the resulting netlist. code may be entered by editing the value property
  with <shift-q>

2.4.2:
- delete objects (mainly for text) before overstriking with hilight color, this avoids
  ugly character borders of hilighted texts (antialiasing presumes you draw text 
  on empty background)
- small bug fixes of regressions introduced by previous commit

2.4.1: 
- spatial hash iterator added in drawing engine when the viewing area is small compared
  to the total area of a freaking big schematic; using spatial hash the lookup of elements
  (instances and wires for now) that are in the viewing area is *much* faster.
  so we avoid lot of clip checks on the whole wire[] and inst_ptr[] arrays.
  When a full zoom is done (viewing area is big) we return to normal loop
  on the wire and inst_ptr arrays, since spatial hash gives no gain in this case.
  (20180104)
- some small glitches about not drawing small objects (for performance reasons) fixed.
  (20171206)
- some fixes in symbol editing: rename pins next to pin labels (20171221)
- fixed regresion in print_hilight_net() to recognize pin direction when creating pins
  from hilight nets (20171221)
- allow multi-instance primitive format specifications (verilog_format prop) using the
  #xxx#@name construct to build unique names for nodes and inst names,
  as done for spice primitives

2.4.0:
- some string overflow checks (schematic[])
- major speed improvement in instance wiring tool ('H' bindkey) for multiple selected
  instances.
- 'first_call' parameter in place_symbol() to speed up finding unique
  instance name in case of multiple placements of the same type.
- 'drill through' option in net highlight, (ctrl-alt-k key) this command
  will highlight a net passing through components with pins that have a
  'propagate_to' property set to the number of the pin to pass the
  highlight to.
  Example:
  if pin #0 on a resistor has 'propagate_to=1' property set
  then hilight net on pin 0 will propagate to pin #1.
  the reverse propagate_to may be set as well.

  propagate_to=1     propagate_to=0
  pin #0             pin #1
      o---/\/\/\/\---o

- correctly redraw selected objects when passing with selection rectangle
  over already selected items
- select_text and select_wire now have 'fast' param to avoid drawing information
  in window status line, this is used when doing block selection of multiple objects
  to make the process *much* faster
- skip drawing symbols whose bounding box in current zoom factor <0.3 pixels.
- fixed some wrong bbox() usage in edit_text_property
- performance improvements in prepare_netlist_structs when called for highlighting nets
- fixed a regression in copy_objects of polygons
- 'xschem' tcl command: print error message if unknow subcommand given
- fixed some regression in partial rectangle selections (select_box() )
- 'hash_prefix_unnamed_net' parameter in pin_node(), needed for correctly
  hashing unnamed nets for 'drill' hilight.
- added 'unselect' radio button in instance search tool (ctrl-f key)
- added 'layer' and 'font' members to text struct, caching the respective
  'layer=n and font=string' properties, to avoid calling get_tok_value
- more flexibility in 'attach_labels_to_inst ('H' key), allow to wire all
  selected instances without querying the user, option for user set rotated text
- performance optimization, do not call symbol_bbox on every full zoom
- create symbol pins from selected schematic pins (Alt-h key)
- Alt-s key (reload from disk) now correctly reloads symbol (.sym) file
  if editing a symbol.
- fullscreen key moved to Alt-Shift-F
- added Alt-h and Alt-r keys to flip and rotate objects around their anchor points 
  while copying/moving
- performance optimization: skip drawing of objects that are too small in
  current zoom factor.
- added command' xschem set no_undo [01]' to disable/reenable undo.
- performance improvements in wirecheck(), skip wires that already have a .node field
- renamed functions in netlist.c for better understanding
- performance improvement, simplified and almost eliminated use of hash_lookup function (token.c)
  it is now used only to store instance names to allow collision checking in new_prop_string().
  hash_proplist() now only inserts instance name in hash table, rest of property string is skipped.
- new_prop_string, start checking from 0 instead of from 1 when looking for free instance
  name slots (x0 instead of x1 as first free name for subcircuit instances, for example)
- xschem.tcl: refer to $HOME/.clipboard.sch instead of $PWD/.clipboard.sch

2.3.7:
- do not recalculate symbol bounding box on any zoom / pan / draw operation, 
  do it when moving / changing props / deleting / loading / drawing selection.
- added a 'bus' int field in the wire struct, where the 'bus' property is cached
  this avoid many get_tok_value() calls on every draw() call.
- fixed a regression introduced by [20171104] in subst_token() [20171201]

2.3.6:
- some cleanups in the code, 
- performance optimization, do not call symbol_bbox() in view_zoom and view_unzoom, 
- dim and bright colors (+ and - keys) 
- include/exclude background color in dim/bright operations (alt-c)
- cairo clipping implemented in bbox() so text will not be overstriked making it look bold
  when placing and deleting objects that partially overlap text.
- Dirty hack in callback() (TURBOX_FIX) to work-around bugs in the Exceed
  TurboX server. Include -DTURBOX_FIX in the Makefile if needed.
  without this hack repainting the window with the save_pixmap (FillTiled fill style)
  will sometimes not work, leaving garbage or empty areas.
- full precision in save operations (%.16g instead of %g), allowing *much* bigger
  drawing areas to be saved and restored without loss of precision.

2.3.5:
- proportional fonts now fully supported, text correctly aligned even on rotated objects
- postscript and svg print routines updated to render polygons.

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



