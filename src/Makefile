
## set prefix for installation
#prefix=/usr/local
prefix=$(HOME)
library_prefix=$(HOME)/xschem_library

OBJ = icon.o callback.o actions.o move.o check.o clip.o draw.o globals.o main.o netlist.o \
      findnet.o scheduler.o store.o xinit.o select.o font.o editprop.o save.o paste.o \
      token.o psprint.o node_hash.o hilight.o options.o vhdl_netlist.o \
      svgdraw.o spice_netlist.o verilog_netlist.o  parselabel.o expandlabel.o
CC=gcc

## set the TCL-TK version on the system
TCLVER=8.6

## enable one of the PROFILEFLAGS below if debugging stuff is needed
PROFILEFLAGS=-g  -pg 

CFLAGS = -Wall  $(PROFILEFLAGS)  -I/usr/include/tcl${TCLVER} 
LDFLAGS = -L/usr/X11R6/lib -lX11 -lm -lXpm -ltcl${TCLVER} -ltk${TCLVER}  -lfl  \
$(PROFILEFLAGS)

xschem: 	$(OBJ)
	$(CC) -o $@   $^ $(LDFLAGS)

parselabel.c: parselabel.l expandlabel.h
	flex -o $@  $<

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
	mkdir -p $(library_prefix)
	mkdir -p $(library_prefix)/simulations
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
	cp verilog.awk $(prefix)/share/xschem
	cp vhdl.awk $(prefix)/share/xschem
	cp -r ../doc/* $(prefix)/share/doc/xschem/
	cp -r ../utile $(prefix)/share/xschem
	cp -ru ../library/mylib $(library_prefix)
	cp -ru ../library/devices $(library_prefix)
	cp -ru ../library/examples $(library_prefix)

.PHONY: uninstall
uninstall:
	rm $(prefix)/bin/xschem
	rm -rf $(prefix)/share/xschem
	rm -rf $(prefix)/share/doc/xschem

