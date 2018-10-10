
## set prefix for installation
## XSCHEM will install in ${prefix}/bin, ${prefix}/share/xschem 
## and ${prefix}/share/doc/xschem
prefix=$(HOME)

## specify user schematic/symbol libraries; 
## you may redefine it later in .xschem or xschemrc startup files
user_library_dir=$(HOME)/xschem_library

## specify system-wide symbol libraries;
## you may redefine it later in .xschem or xschemrc startup files
system_library_dir=${prefix}/share/xschem/xschem_library

xschem_library_path=$(user_library_dir):$(system_library_dir)

OBJ = icon.o callback.o actions.o move.o check.o clip.o draw.o \
      globals.o main.o netlist.o \
      findnet.o scheduler.o store.o xinit.o select.o font.o editprop.o save.o paste.o \
      token.o psprint.o node_hash.o hilight.o options.o vhdl_netlist.o \
      svgdraw.o spice_netlist.o tedax_netlist.o verilog_netlist.o  parselabel.o expandlabel.o
CC=gcc

## set the TCL-TK version on the system
TCLVER=8.6


## set to 0 if you don't have CAIRO or don't want to use it
## set to 1 for CAIRO antialiased fonts
HAS_CAIRO=1

## set to 0 these 2 variables on old machines.
HAS_XCB=1
HAS_XRENDER=1

XCB_INC=
ifeq ($(HAS_XCB),1)
  XCB_INC = xcb x11-xcb xcb-render xrender
else ifeq  ($(HAS_XRENDER),1)
  XCB_INC = xrender
endif

ifeq ($(HAS_CAIRO),1)
	CAIRO_INCLUDES=`pkg-config cairo $(XCB_INC) --cflags` \
              -DHAS_CAIRO -DHAS_XRENDER=$(HAS_XRENDER) -DHAS_XCB=$(HAS_XCB)
	CAIRO_LIBS=`pkg-config cairo $(XCB_INC)  --libs`
else
	CAIRO_INCLUDES=
	CAIRO_LIBS=
endif

## use the MOREFLAGS below if debugging stuff is needed
## -g -pg -no-pie 
## recent gcc generates Position Independent Executable (pie), this breaks
## gprof, so use -no-pie together with -pg when profiling code.
## for cairo: $(pkg-config cairo  --cflags --libs)

## to fix rendering problems due to bugs in the exceed turboX server
## add -DTURBOX_FIX
MOREFLAGS= -g -std=c99 -pedantic

CFLAGS  = -Wall  $(MOREFLAGS)  -I/usr/include/tcl${TCLVER} $(CAIRO_INCLUDES) -I/home/sschippe/include \
           -DPREFIX='"${prefix}"' \
           -DXSCHEM_LIBRARY_PATH='"${xschem_library_path}"'

LDFLAGS = -L/usr/X11R6/lib -L/home/sschippe/lib $(MOREFLAGS) -lX11 -lm -lXpm -ltcl${TCLVER} -ltk${TCLVER} \
           -lfl $(CAIRO_LIBS)

xschem: 	$(OBJ)
	$(CC) -o $@   $^ $(LDFLAGS)

parselabel.c: parselabel.l expandlabel.h
	flex -l -o$@  $<

expandlabel.c expandlabel.h: expandlabel.y
	bison -d -o expandlabel.c  $<

parselabel.o: expandlabel.h

$(OBJ): xschem.h

.PHONY: clean
clean:
	rm -rf xschem *.o expandlabel.[ch] parselabel.c netlist netlist.spice netlist.vhdl netlist.v *.da gmon.out

.PHONY: install
install: xschem
	mkdir -p $(prefix)/bin
	mkdir -p $(prefix)/share/xschem
	mkdir -p $(prefix)/share/doc/xschem
	mkdir -p $(system_library_dir)
	mkdir -p $(user_library_dir)
	mkdir -p $(user_library_dir)/simulations
	cp xschem $(prefix)/bin
	cp xschem $(prefix)/share/xschem
	cp keys.help  $(prefix)/share/xschem
	cp xschem.help  $(prefix)/share/xschem
	cp xschem.tcl  $(prefix)/share/xschem
	cp -r systemlib $(prefix)/share/xschem
	cp break.awk $(prefix)/share/xschem
	cp convert_to_verilog2001.awk $(prefix)/share/xschem
	cp flatten.awk $(prefix)/share/xschem
	cp make_sym.awk $(prefix)/share/xschem
	cp order_labels.awk $(prefix)/share/xschem
	cp sort_labels.awk $(prefix)/share/xschem
	cp spice.awk $(prefix)/share/xschem
	cp tedax.awk $(prefix)/share/xschem
	cp verilog.awk $(prefix)/share/xschem
	cp ../xschemrc $(prefix)/share/xschem
	cp ../.xschem $(HOME)/.xschem
	cp vhdl.awk $(prefix)/share/xschem
	cp -r ../doc/* $(prefix)/share/doc/xschem/
	cp -r ../utile $(prefix)/share/xschem
	cp -ru ../xschem_library/mylib $(user_library_dir)
	cp -ru ../xschem_library/devices $(system_library_dir)
	cp -ru ../xschem_library/examples $(user_library_dir)
	cp -ru ../xschem_library/pcb $(system_library_dir)

.PHONY: uninstall
uninstall:
	rm $(prefix)/bin/xschem
	rm -rf $(prefix)/share/xschem
	rm -rf $(prefix)/share/doc/xschem

