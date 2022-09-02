/* File: xschem.h
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2021 Stefan Frederik Schippers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CADGLOBALS
#define CADGLOBALS

#define XSCHEM_VERSION "3.0.0"
#define XSCHEM_FILE_VERSION "1.2"

#if HAS_PIPE == 1
/* fdopen() */
#define _POSIX_C_SOURCE 200112L
#endif

#if  HAS_POPEN==1
/* popen() , pclose(), */
#define _POSIX_C_SOURCE 200112L
#endif

#define TCL_WIDE_INT_TYPE long


#if (defined(__APPLE__) && defined(__MACH__))
#define __unix__
#endif

/*  approximate PI definition */
#define XSCH_PI 3.14159265358979323846264338327950288419716939937

#ifdef __unix__
#include "../config.h"
#else
#include "../XSchemWin/config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <limits.h> /* PATH_MAX */

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef __unix__
#include <unistd.h>
#include <regex.h>
#else
#include <windows.h>
#include "tkWin.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>


#include <fcntl.h>
#include <time.h>

/* #include <sys/time.h>  for gettimeofday(). use time() instead */
#include <signal.h>
#ifdef __unix__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/xpm.h>
#define xunlink unlink
#define xfseek fseek
#define xftell ftell
#else
#include <tkWinInt.h>
#define xunlink _unlink
#define MOUSE_WHEEL_UP 38
extern int XSetClipRectangles(register Display* dpy, GC gc, int clip_x_origin, 
            int clip_y_origin, XRectangle* rectangles, int n, int ordering);
extern int XSetTile(Display* display, GC gctiled, Pixmap s_pixmap);
extern void change_to_unix_fn(char* fn);
extern char win_temp_dir[PATH_MAX];
#define xfseek _fseeki64
#define xftell _ftelli64
#endif

#if HAS_CAIRO==1
#include <cairo.h>
#if __unix__
#include <cairo-xlib.h>
#include "cairo-xlib-xrender.h"
#else
#include <cairo-win32.h>
#endif
#endif

#include <tcl.h>
#include <tk.h>

#define CADHEIGHT 700                   /*  initial window size */
#define CADWIDTH 1000

/* max number of windows (including main) a single xschem process can handle */
#define MAX_NEW_WINDOWS 20
#define WINDOW_PATH_SIZE 30

#define BACKLAYER 0
#define WIRELAYER 1
#define GRIDLAYER 2
#define SELLAYER 2
#define PROPERTYLAYER 1
#define TEXTLAYER 3
#define TEXTWIRELAYER 1 /*  color for wire name labels / pins */
#define SYMLAYER 4
#define PINLAYER 5
#define GENERICLAYER 3

#define CADSNAP 10.0
#define CADGRID 20.0
#define CADGRIDTHRESHOLD 10.0
#define CADGRIDMULTIPLY 2.0
#define CADINITIALZOOM 1
#define CADINITIALX 10
#define CADINITIALY -870
#define CADZOOMSTEP 1.2
#define CADMOVESTEP 200
#define CADMAXZOOM 1000000.0
#define CADMINZOOM 0.000001
#define CADHALFDOTSIZE 4
#define CADNULLNODE -1      /*  no valid node number */
#define CADWIREMINDIST 8.0
#define CADMAXWIRES 200
#define CADMAXTEXT 100
#define CADMAXOBJECTS 100  /*  (initial) max # of lines, rects (for each layer!!) */
#define MAXGROUP 100       /*  (initial) max # of objects that can be drawn while moving */
#define ELEMINST 100       /*  (initial) max # of placed elements,   was 600 20102004 */
#define ELEMDEF 50         /*  (initial) max # of defined elements */
#define EMBEDDED 1   /* used for embedded symbols marking in Symbol.flags */
#define HILIGHT_CONN 4 /* used to hilight instances if connected wire is hilighted */
#define CADMAXGRIDPOINTS 512
#define CADMAXHIER 80
#define CADCHUNKALLOC 512 /*  was 256  20102004 */
#define CADDRAWBUFFERSIZE 512

/*  when x-width of drawing area s below this threshold use spatial */
/*  hash table for drawing wires and instances (for faster lookup) instead of */
/*  looping through the whole wire[] and inst[] arrays */
/*  when drawing area is very big using spatial hash table may take longer than */
/*  a simple for() loop through the big arrays + clip check. */
#define ITERATOR_THRESHOLD  42000.0

#define SCHEMATIC 1
#define SYMBOL 2
#define CAD_SPICE_NETLIST 1
#define CAD_VHDL_NETLIST 2
#define CAD_VERILOG_NETLIST 3
#define CAD_TEDAX_NETLIST 4
#define CAD_SYMBOL_ATTRS 5

#define STARTWIRE 1         /*  possible states, encoded in global 'rubber' */

#define STARTRECT 4
#define STARTLINE 8
#define SELECTION  16       /*  signals that some objects are selected. */
#define STARTSELECT 32      /*  used for drawing a selection rectangle */
#define STARTMOVE 64        /*  used for move/copy  operations */
#define STARTCOPY 128       /*  used for move/copy  operations */
#define STARTZOOM 256       /*  used for move/copy  operations */
#define STARTMERGE 512      /*  used fpr merge schematic/symbol */
#define MENUSTARTWIRE 1024  /*  start wire invoked from menu */
#define MENUSTARTLINE 2048  /*  start line invoked from menu */
#define MENUSTARTRECT 4096  /*  start rect invoked from menu */
#define MENUSTARTZOOM 8192  /*  start zoom box invoked from menu */
#define STARTPAN     16384 /*  new pan method with mouse button3 */
#define PLACE_TEXT 32768
#define MENUSTARTSNAPWIRE 65536   /*  start wire invoked from menu, snap to pin variant 20171022 */
#define STARTPOLYGON 131072
#define MENUSTARTPOLYGON 262144
#define STARTARC 524288
#define MENUSTARTARC 1048576
#define MENUSTARTCIRCLE 2097152
#define PLACE_SYMBOL 4194304 /* used in move_objects after place_symbol to avoid storing intermediate undo state */
#define START_SYMPIN 8388608
#define GRAPHPAN 16777216
#define SELECTED 1          /*  used in the .sel field for selected objs. */
#define SELECTED1 2         /*  first point selected... */
#define SELECTED2 4         /*  second point selected... */
#define SELECTED3 8
#define SELECTED4 16

#define WIRE 1              /*  types of defined objects */
#define xRECT  2
#define LINE 4
#define ELEMENT 8
#define xTEXT 16
#define POLYGON 32
#define ARC 64

/*  for netlist.c */
#define BOXSIZE 400
#define NBOXES 100

#define MAX_UNDO 80

/* viewers xschem can generate plot commands for */
#define NGSPICE 1
#define GAW 2
#define BESPICE 3
#define XSCHEM_GRAPH 4

/*    some useful primes */
/*    109, 163, 251, 367, 557, 823, 1237, 1861, 2777, 4177, 6247, 9371, 14057 */
/*    21089, 31627, 47431, 71143, 106721, 160073, 240101, 360163, 540217, 810343 */
/*    1215497, 1823231, 2734867, 4102283, 6153409, 9230113, 13845163 */

#define HASHSIZE 31627

                   /*  parameters passed to action functions, see actions.c */
#define END      1 /*  endop */
#define START    2 /*  begin placing something */
#define PLACE    4 /*  place something */
#define ADD      8 /*  add something */
#define RUBBER  16 /*  used for drawing rubber objects while placing them */
#define NOW     32 /*  used for immediate (unbuffered) graphic operations */
#define ROTATE  64
#define FLIP   128
#define SET    256 /*  currently used in bbox() function (sets clip rect) */
#define ABORT  512 /*  used in move/copy_objects for aborting without unselecting */
#define THICK 1024 /*  used to draw thick lines (buses) */
#define ROTATELOCAL 2048 /*  rotate each selected object around its own anchor point 20171208 */
#define CLEAR 4096 /* used in new_wire to clear previous rubber when switching xctx->manhattan_lines */
#define SET_INSIDE 8192 /* used in bbox() to set clipping rectangle inside, not adding line width */
/* #define DRAW 8192 */  /* was used in bbox() to draw things by using XCopyArea after setting clip rectangle */
#define HILIGHT 8192  /* used when calling draw_*symbol_outline() for hilighting instead of normal draw */
#define FONTWIDTH 20
#define FONTOFFSET 40
#define FONTHEIGHT 40
#define FONTDESCENT 15
#define FONTWHITESPACE 10

/* hash operations */
#define XINSERT 0
#define XLOOKUP 1
#define XDELETE 2
#define XINSERT_NOREPLACE 3 /* do not replace token value in hash if already present */

/* Cairo text flags (.flags field) */
#define TEXT_BOLD 1
#define TEXT_OBLIQUE 2
#define TEXT_ITALIC 4
/* flag (.flags field) to hide text in symbols when displaying instances */
#define HIDE_TEXT 8

#define S(a) (sizeof(a)/sizeof(a[0]))
#define BUS_WIDTH 4
#define POINTINSIDE(xa,ya,x1,y1,x2,y2)  \
 (xa>=x1 && xa<=x2 && ya>=y1 && ya<=y2 )

#define RECT_INSIDE(xa,ya,xb,yb,x1,y1,x2,y2)  \
 (xa>=x1 && xa<=x2 && xb>=x1 && xb<=x2 && ya>=y1 && ya<=y2 && yb>=y1 && yb<=y2 )

#define RECT_OUTSIDE(xa,ya,xb,yb,x1,y1,x2,y2)  \
( (xa) > (x2) || (xb) < (x1) || (ya) > (y2) || (yb) < (y1) )




#define ROTATION(rot, flip, x0, y0, x, y, rx, ry) \
{ \
  double xxtmp = (flip ? 2 * x0 -x : x); \
  if(rot==0)      {rx = xxtmp;  ry = y;} \
  else if(rot==1) {rx = x0 - y + y0; ry = y0 + xxtmp - x0;} \
  else if(rot==2) {rx = 2 * x0 - xxtmp; ry = 2 * y0 - y;} \
  else            {rx = x0 + y - y0; ry = y0 - xxtmp + x0;} \
}

#define ORDER(x1,y1,x2,y2) {\
  double xxtmp; \
  if(x2 < x1) {xxtmp=x1;x1=x2;x2=xxtmp;xxtmp=y1;y1=y2;y2=xxtmp;} \
  else if(x2 == x1 && y2 < y1) {xxtmp=y1;y1=y2;y2=xxtmp;} \
}

#define RECTORDER(x1,y1,x2,y2) { \
  double xxtmp; \
  if(x2 < x1) {xxtmp = x1; x1 = x2; x2 = xxtmp;} \
  if(y2 < y1) {xxtmp = y1; y1 = y2; y2 = xxtmp;} \
}

#define OUTSIDE(xa,ya,xb,yb,x1,y1,x2,y2) \
 (xa>x2 || xb<x1 ||  ya>y2 || yb<y1 )

#define LINE_OUTSIDE(xa,ya,xb,yb,x1,y1,x2,y2) \
 (xa>=x2 || xb<=x1 ||  ( (ya<yb)? (ya>=y2 || yb<=y1) : (yb>=y2 || ya<=y1) ) )

#define CLIP(x,a,b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

#define MINOR(a,b) ( (a) <= (b) ? (a) : (b) )

#define IS_LABEL_SH_OR_PIN(type) (!(strcmp(type,"label") && strcmp(type,"ipin") && strcmp(type,"opin") && \
                                 strcmp(type,"show_label") && strcmp(type,"iopin")))
#define IS_LABEL_OR_PIN(type) (!(strcmp(type,"label") && strcmp(type,"ipin") && strcmp(type,"opin") && strcmp(type,"iopin")))
#define IS_PIN(type) (!(strcmp(type,"ipin") && strcmp(type,"opin") && strcmp(type,"iopin")))
#define XSIGN(x) ( (x) < 0 ? -1 : 1)
#define XSIGN0(x) ( (x) < 0 ? -1 : (x) > 0 ? 1 : 0)

/* floor not needed? screen coordinates always positive <<<< */
/* #define X_TO_SCREEN(x) ( floor(((x)+xctx->xorigin)* xctx->mooz) ) */
/* #define Y_TO_SCREEN(y) ( floor(((y)+xctx->yorigin)* xctx->mooz) ) */
#define X_TO_SCREEN(x) ( ((x) + xctx->xorigin) * xctx->mooz )
#define Y_TO_SCREEN(y) ( ((y) + xctx->yorigin) * xctx->mooz )
#define X_TO_XSCHEM(x) ( (x) * xctx->zoom - xctx->xorigin )
#define Y_TO_XSCHEM(y) ( (y) * xctx->zoom - xctx->yorigin )

/* coordinate transformations graph to xschem */
#define W_X(x) (gr->cx * (x) + gr->dx)
#define W_Y(y) (gr->cy * (y) + gr->dy)
/* for digital waves */
#define DW_Y(y) (gr->dcy * (y) + gr->ddy)

/* coordinate transformations graph to screen */
#define S_X(x) (gr->scx * (x) + gr->sdx)
#define S_Y(y) (gr->scy * (y) + gr->sdy)
/* for digital waves */
#define DS_Y(y) (gr->dscy * (y) + gr->dsdy)

/* inverse graph transforms */
#define G_X(x) (((x) - gr->dx) / gr->cx)
#define G_Y(y) (((y) - gr->dy) / gr->cy)
/* for digital graphs (gr->ypos1, gr->ypos2 instead of gr->gy1, gr->gy2) */
#define DG_Y(y) (((y) - gr->ddy) / gr->dcy)


/* given a dest_string of size 'size', allocate space to make sure it can
 * hold 'add' characters */
#define  STR_ALLOC(dest_string, add, size) \
do { \
  register int __str_alloc_tmp__ = add; \
  if( __str_alloc_tmp__ >= *size) { \
    *size = __str_alloc_tmp__ + CADCHUNKALLOC; \
    my_realloc(1212, dest_string, *size); \
  } \
} while(0)

#define INT_WIDTH(x) ( (int)(x) == 0 ? 1 : (int)(x) ) 
#define INT_BUS_WIDTH(x) ( (int)( (BUS_WIDTH) * (x) ) == 0 ? 1 : (int)( (BUS_WIDTH) * (x) ) ) 

/* set do double if you need more precision at the expense of memory */
#define SPICE_DATA float
#define DIG_NWAVES 0.1  /* inverse number: by default 10 digital traces per graph */
#define DIG_SPACE 0.07 /* trace extends from 0 to DIG_SPACE, so we have DIG_WAVES-DIG_SPACE
                        * spacing between traces */

typedef struct
{
  unsigned short type;
  int n;
  unsigned int col;
} Selected;

typedef struct
{
  double x1;
  double x2;
  double y1;
  double y2;
  short  end1;
  short  end2;
  short sel;
  char  *node;
  char *prop_ptr;
  short bus; /*  20171201 cache here wire "bus" property, to avoid too many get_tok_value() calls */
} xWire;

typedef struct
{
  double x1;
  double x2;
  double y1;
  double y2;
  unsigned short sel;
  char *prop_ptr;
  short dash;
  short bus;
} xLine;

#if HAS_CAIRO==1
typedef struct
{
  cairo_surface_t *image;
} xEmb_image;
#endif

typedef struct
{
  double x1;
  double x2;
  double y1;
  double y2;
  unsigned short sel;
  char *prop_ptr;
  void *extraptr; /* generic data pointer (images) */
  short dash;
  /* bit0=1 for graph function, bit1=1 for unlocked x axis
   * bit10: image embedding (png)
   */
  unsigned short flags;
} xRect;

typedef struct
{
  /*  last point coincident to first, added by program if needed. */
  /*  XDrawLines needs first and last point to close the polygon */
  int points;
  double *x;
  double *y;
  unsigned short *selected_point;
  unsigned short sel;
  char *prop_ptr;
  short fill;
  short dash;
} xPoly;

typedef struct
{
  double x;
  double y;
  double r;
  double a; /* start angle */
  double b; /* arc angle */
  unsigned short sel;
  char *prop_ptr;
  short fill;
  short dash;
} xArc;

typedef struct
{
  char *txt_ptr;
  double x0,y0;
  short rot;
  short flip;
  short sel;
  double xscale;
  double yscale;
  char *prop_ptr;
  int layer; /*  20171201 for cairo  */
  short hcenter, vcenter;
  char *font; /*  20171201 for cairo */
  int flags; /* TEXT_BOLD:1 TEXT_OBLIQUE:2 TEXT_ITALIC:4 HIDE_TEXT:8 */
} xText;

typedef struct
{
  char *name;
  double minx;
  double maxx;
  double miny;
  double maxy;
  xLine **line;  /*  array of [cadlayers] pointers to Line */
  xRect  **rect;
  xPoly **poly;
  xArc **arc;
  xText  *text;
  int *lines;     /*  array of [cadlayers] integers */
  int *rects;
  int *polygons;
  int *arcs;
  int texts;
  char *prop_ptr;
  char *type;
  char *templ;
  int flags;   /* bit 0: embedded flag 
                * bit 1: **free**
                * bit 2: highight if connected wire highlighted */
} xSymbol;

typedef struct
{
  char *name;/*  symbol name (ex: devices/lab_pin)  */
  int ptr;   /*  was a pointer formerly... */
  double x0;  /* symbol origin / anchor point */
  double y0;
  double x1;  /* symbol bounding box */
  double y1;
  double x2;
  double y2;
  double xx1; /* bounding box without texts */
  double yy1;
  double xx2;
  double yy2;
  short rot;
  short flip;
  short sel;
  int color; /* hilight color */
  short flags; /* bit 0: skip field, 
                * bit 1: flag for different textlayer for pin/labels,
                * 1: ordinary symbol, 0: label/pin/show 
                * bit 2: highlight if connected net/label is highlighted */
  char *prop_ptr;
  char **node;
  char *lab;      /*  lab attribute if any (pin/label) */
  char *instname; /*  20150409 instance name (example: I23)  */
} xInstance;


typedef struct
{
  double x;
  double y;
  double zoom;
} Zoom;


typedef struct
{
  char *function;
  char *go_to;
  int value;
  short clock;
} Simdata_pin;
 
typedef struct
{
  Simdata_pin *pin;
  int npin;
} Simdata;

typedef struct
{
  char     *gptr;
  char     *vptr;
  char     *sptr;
  char     *kptr;
  char     *eptr;
  int *lines;
  int *rects;
  int *polygons;
  int *arcs;
  int wires;
  int texts;
  int instances;
  int symbols;
  xLine     **lptr;
  xRect      **bptr;
  xPoly  **pptr;
  xArc      **aptr;
  xWire     *wptr;
  xText     *tptr;
  xInstance *iptr;
  xSymbol *symptr;
} Undo_slot;

typedef struct
{ /* used for symbols containing schematics as instances (LCC, Local Custom Cell) */
  double x0;
  double y0;
  short rot;
  short flip;
  FILE *fd;
  char *prop_ptr;
  char *symname;
} Lcc;

typedef struct {
  int in;
  int out;
  int inout;
  int port;
} Drivers;

/* instance name (refdes) hash table, for unique name checking */
typedef struct inst_hashentry Inst_hashentry;
struct inst_hashentry 
{
  struct inst_hashentry *next;
  unsigned int hash;
  char *token;
  int value;
};

/* generic string hash table */
typedef struct str_hashentry Str_hashentry;
struct str_hashentry
{
  struct str_hashentry *next;
  unsigned int hash;
  char *token;
  char *value;
};

/* generic int hash table */
typedef struct int_hashentry Int_hashentry;
struct int_hashentry
{
  struct int_hashentry *next;
  unsigned int hash;
  char *token;
  int value;
};

typedef struct node_hashentry Node_hashentry;
struct node_hashentry
{
  struct node_hashentry *next;
  unsigned int hash;
  char *token;
  char *sig_type;
  char *verilog_type;
  char *value;
  char *class;
  char *orig_tok;
  Drivers d;
};


typedef struct hilight_hashentry Hilight_hashentry;
struct hilight_hashentry
{
  struct hilight_hashentry *next;
  unsigned int hash;
  char *token;
  char *path; /* hierarchy path */
  int oldvalue;  /* used for FF simulation */
  int value;  /* hilight color */
  int time; /*delta-time for sims */
};

/*  for netlist.c */
typedef struct instpinentry Instpinentry;
struct instpinentry
{
  struct instpinentry *next;
  double x0,y0;
  int n;
  int pin;
};

typedef struct wireentry Wireentry;
struct wireentry
{
  struct wireentry *next;
  int n;
};

typedef struct instentry Instentry;
struct instentry
{
  struct instentry *next;
  int n;
};

typedef struct 
{
  int x1a, x2a;
  int y1a, y2a;
  int i, j, counti, countj;
  int tmpi, tmpj;
  Instentry *instanceptr;
  Wireentry *wireptr;
  unsigned short *instflag;
  unsigned short *wireflag;
} Iterator_ctx;


/* context struct for waveform graphs */
typedef struct {
  int i;  /* index number of container rectangle */
  double digital;
  double rx1, ry1, rx2, ry2, rw, rh; /* container rectangle */
  double sx1, sy1, sx2, sy2; /* screen coordinates of above */
  /* graph box (smaller than rect container due to margins) */
  double x1, y1, x2, y2, w, h;
  double gx1, gy1, gx2, gy2, gw, gh;
  double master_gx1, master_gx2, master_gw;
  /* y area range for digital graphs */
  double ypos1, ypos2, posh;
  double marginx; /* will be recalculated later */
  double marginy; /* will be recalculated later */
  /* coefficients for graph to container coordinate transformations W_X() and W_Y()*/
  double cx, dx, cy, dy;
  /* y-axis transform for digital graphs */
  double dcy, ddy;
  /* direct graph->screen transform */
  double scx, sdx, scy, sdy;
  double dscy, dsdy;
  int divx;
  int divy;
  int subdivx;
  int subdivy;
  double unitx;
  double unity;
  int unitx_suffix; /* 'n' or 'u' or 'M' or 'k' ... */
  int unity_suffix;
  double txtsizelab, digtxtsizelab, txtsizey, txtsizex;
  int dataset;
  int hilight_wave[2]; /* [0] : graph index, [1] : wave index */
} Graph_ctx;

typedef struct {
  xWire *wire;
  xText *text;
  xRect **rect;
  xLine **line;
  xPoly **poly;
  xArc **arc;
  xInstance *inst;
  xSymbol *sym;
  int sym_txt;
  int wires;
  int instances;
  int symbols;
  int texts;
  int *rects;
  int *polygons;
  int *arcs;
  int *lines;
  int *maxr;
  int *maxp;
  int *maxa;
  int *maxl;
  int maxt;
  int maxw;
  int maxi;
  int maxs;
  char *schprop;
  char *schtedaxprop;
  char *schvhdlprop;
  char *schsymbolprop;
  char *schverilogprop;
  char sch[CADMAXHIER][PATH_MAX];
  int currsch;
  char *version_string;
  char current_name[PATH_MAX];
  char file_version[100];
  char *sch_path[CADMAXHIER];
  int sch_path_hash[CADMAXHIER];
  int sch_inst_number[CADMAXHIER];
  int previous_instance[CADMAXHIER]; /* to remember the instance we came from when going up the hier. */
  Zoom zoom_array[CADMAXHIER];
  double xorigin,yorigin;
  double zoom;
  double mooz;
  double lw;
  unsigned long ui_state ; /* this signals that we are doing a net place,panning etc.
                            * used to prevent nesting of some commands */
  double mousex,mousey; /* mouse coord. */
  double mousex_snap,mousey_snap; /* mouse coord. snapped to grid */
  double mx_double_save, my_double_save;
  int areax1,areay1,areax2,areay2,areaw,areah; /* window corners / size, line width beyond screen edges */
  int need_reb_sel_arr;
  int lastsel;
  int maxsel;
  Selected *sel_array;
  int prep_net_structs;
  int prep_hi_structs;
  int prep_hash_inst;
  int prep_hash_wires;
  Simdata *simdata;
  int simdata_ninst;
  int modified;
  int semaphore;
  int tok_size;
  char netlist_name[PATH_MAX];
  int flat_netlist;
  char current_dirname[PATH_MAX];
  int netlist_unconn_cnt; /* unique count of unconnected pins while netlisting */
  Instpinentry *instpin_spatial_table[NBOXES][NBOXES];
  Wireentry *wire_spatial_table[NBOXES][NBOXES];
  Instentry *inst_spatial_table[NBOXES][NBOXES];
  Window window;
  Pixmap save_pixmap;
  XRectangle xrect[1];
  #if HAS_CAIRO==1
  cairo_surface_t *cairo_sfc, *cairo_save_sfc;
  cairo_t *cairo_ctx, *cairo_save_ctx;
  #endif
  GC gctiled;
  GC *gc;
  GC *gcstipple;
  char **color_array;
  unsigned int color_index[256];
  XColor xcolor_array[256];
  int *enable_layer;
  int n_active_layers;
  int *active_layer;
  char *undo_dirname;
  int cur_undo_ptr;
  int tail_undo_ptr;
  int head_undo_ptr;
  Inst_hashentry **inst_table;
  Node_hashentry **node_table;
  Hilight_hashentry **hilight_table;

  int hilight_nets;
  int hilight_color;
  int hilight_time; /* timestamp for sims */
  unsigned int rectcolor; /* current layer */
  /* get_unnamed_node() */
  int new_node;
  int *node_mult;
  int node_mult_size;
  /* callback.c */
  int mx_save, my_save, last_command;
  char sel_or_clip[PATH_MAX];
  int onetime;
  /* list of nodes, instances attached to these need redraw */
  Int_hashentry **node_redraw_table;
  /* list of instances, collected using previous table, that need redraw */
  unsigned char *inst_redraw_table;
  int inst_redraw_table_size;
  /* move.c */
  double rx1, rx2, ry1, ry2;
  short move_rot;
  /* a wire was created while separating a component frm a net or another component */
  short kissing;
  short move_flip;
  int manhattan_lines;
  double x1, y_1, x2, y_2, deltax, deltay;
  int movelastsel;
  short rotatelocal;
  /* new_wire, new_line, new_rect*/
  double nl_x1,nl_y1,nl_x2,nl_y2;
  double nl_xx1,nl_yy1,nl_xx2,nl_yy2;
  /* new_arc */
  double nl_x, nl_y, nl_r, nl_a, nl_b;
  double nl_x3, nl_y3;
  int nl_state;
  double nl_sweep_angle;
  /* new_polygon */
  double *nl_polyx, *nl_polyy;
  int nl_points, nl_maxpoints;
  /* select_rect */
  double nl_xr, nl_yr, nl_xr2, nl_yr2;
  /* pan */
  double xpan,ypan,xpan2,ypan2;
  double p_xx1,p_xx2,p_yy1,p_yy2;
  /* set_modify */
  int prev_set_modify;
  /* pan */
  int mx_s, my_s;
  int mmx_s, mmy_s;
  double xorig_save, yorig_save;
  /* load_schematic */
  int save_netlist_type;
  int loaded_symbol;
  /* bbox */
  int bbx1, bbx2, bby1, bby2;
  int savew, saveh, savex1, savex2, savey1, savey2;
  int sem; /* set to 1 if a clipping bbox is set (void bbox() ) */
  XRectangle savexrect;
  /* new_prop_string */
  char prefix;
  /* edit_symbol_property, update_symbol */
  char *old_prop;
  int edit_sym_i;
  int netlist_commands;
  /* in_memory_undo */
  Undo_slot uslot[MAX_UNDO];
  int undo_initialized;
  /* graph context struct */
  Graph_ctx graph_struct;
  /* read raw files (draw.c) */
  char **graph_names;
  SPICE_DATA **graph_values;
  int graph_nvars;
  int *graph_npoints;
  int graph_allpoints; /* all points of all datasets combined */
  int graph_datasets;
  double graph_cursor1_x;
  double graph_cursor2_x;
  int graph_unlock_x; 
  /* graph_flags:
   *  1: dnu, reserved, used in draw_graphs()
   *  2: draw x-cursor1
   *  4: draw x-cursor2
   *  8: dnu, reserved, used in draw_graphs()
   * 16: move cursor1
   * 32: move cursor2
   * 64: show measurement tooltip
   */
  int graph_flags;
  int graph_master; /* graph where mouse operations are started, used to lock x-axis */
  int graph_top; /* regions of graph where mouse events occur */
  int graph_bottom; 
  int graph_left;
  int graph_lastsel; /* last graph that was clicked (selected) */
  int graph_sim_type; /* type of sim, 1: Tran, 2: Dc, 3: Ac */
  Int_hashentry **raw_table;
  char *raw_schname;
  /*     */
  int nl_sel, nl_sem;
  XSegment *biggridpoint;
  XPoint *gridpoint;
  char plotfile[PATH_MAX];
  int enable_drill;
  int pending_fullzoom;
  char hiersep[20];
  int no_undo;
  int draw_single_layer;
  int draw_dots;
  int only_probes;
  int menu_removed; /* fullscreen pervious setting */
  double save_lw; /* used to save linewidth when selecting 'only_probes' view */
  int no_draw;
  int draw_pixmap; /* pixmap used as 2nd buffer */
  int netlist_count; /* netlist counter incremented at any cell being netlisted */
  int hide_symbols;
  int netlist_type;
  char *top_path;
  /* top_path is the path prefix of drawing canvas (current_win_path):
   *
   * current_win_path
   *    canvas           top_path
   *  ----------------------------
   *    ".drw"            ""
   *    ".xx.drw"         ".xx"
   */
  char *current_win_path;
  int *fill_type; /* for every layer: 0: no fill, 1, solid fill, 2: stipple fill */
  int fill_pattern;
  int draw_window; 
  time_t time_last_modify;
  int undo_type; /* 0: on disk, 1: in memory */
  void (*push_undo)(void);
  void (*pop_undo)(int, int);
  void (*delete_undo)(void);
  void (*clear_undo)(void);
  int case_insensitive; /* for case insensitive compare where needed */
  int (*x_strcmp)(const char *, const char *);
  Lcc hier_attr[CADMAXHIER]; /* hierarchical recursive attribute substitution when descending */
} Xschem_ctx;

/* GLOBAL VARIABLES */

/*********** X11 specific globals ***********/
extern Colormap colormap;
extern unsigned char **pixdata;
extern unsigned char pixdata_init[22][32];
extern Display *display;
extern int screen_number;
extern int screendepth;
extern Pixmap cad_icon_pixmap, cad_icon_mask, *pixmap;
extern Visual *visual;

/*********** These variables are mirrored in tcl code ***********/
extern int cadlayers; 
extern int has_x; 
extern int rainbow_colors; 
extern int color_ps; 
extern double nocairo_vert_correct;
extern double cairo_vert_correct;
extern int constrained_move;
extern double cairo_font_scale; /*  default: 1.0, allows to adjust font size */
extern double cairo_font_line_spacing;
extern int debug_var;

/*********** These variables are NOT mirrored in tcl code ***********/
extern int help;
extern char *cad_icon[];
extern FILE *errfp;
extern char home_dir[PATH_MAX]; /* home dir obtained via getpwuid */
extern char user_conf_dir[PATH_MAX]; /* usually ~/.xschem */
extern char pwd_dir[PATH_MAX]; /* obtained via getcwd() */
extern int tcp_port;
extern int text_svg;
extern int text_ps;
extern double cadhalfdotsize;
extern char bus_char[];
extern int yyparse_error;
extern char *xschem_executable;
extern Tcl_Interp *interp;
extern double *character[256];
extern int quit;
extern int detach; /* no TCL console */
extern const char fopen_read_mode[]; /* "r" on unix, "rb" on windows */

/*********** Cmdline options  (used at xinit, and then not used anymore) ***********/
extern int cli_opt_argc;
extern char **cli_opt_argv;
extern int cli_opt_netlist_type;
extern int cli_opt_flat_netlist;
extern char cli_opt_plotfile[PATH_MAX];
extern char cli_opt_netlist_dir[PATH_MAX];
extern char cli_opt_filename[PATH_MAX];
extern int cli_opt_no_readline;
extern char *cli_opt_tcl_command;
extern int cli_opt_do_print;
extern int cli_opt_do_netlist;
extern int cli_opt_do_simulation;
extern int cli_opt_do_waves;
extern char cli_opt_tcl_script[PATH_MAX];
extern char cli_opt_initial_netlist_name[PATH_MAX];
extern char cli_opt_rcfile[PATH_MAX];
extern int cli_opt_load_initfile;

/*********** Following data is relative to the current schematic ***********/
extern Xschem_ctx *xctx;

/*  FUNCTIONS */
extern void draw_image(int draw, xRect *r, double *x1, double *y1, double *x2, double *y2, int rot, int flip);
extern int filter_data(const char *din, const size_t ilen,
           char **dout, size_t *olen, const char *cmd);
extern int embed_rawfile(const char *rawfile);
extern int read_rawfile_from_attr(const char *b64s, size_t length);
extern int read_embedded_rawfile(void);
extern char *base64_from_file(const char *f, size_t *length);
extern int set_rect_flags(xRect *r);
extern int set_rect_extraptr(int what, xRect *drptr);
extern unsigned char *base64_decode(const char *data, const size_t input_length, size_t *output_length);
extern char *base64_encode(const unsigned char *data, const size_t input_length, size_t *output_length, int brk);
extern int get_raw_index(const char *node);
extern void free_rawfile(int dr);
extern int read_rawfile(const char *f);
extern double get_raw_value(int dataset, int idx, int point);
extern int plot_raw_custom_data(int sweep_idx, int first, int last, const char *ntok);
extern int calc_custom_data_yrange(int sweep_idx, const char *express, Graph_ctx *gr);
extern int schematic_waves_loaded(void);
extern int edit_wave_attributes(int what, int i, Graph_ctx *gr);
extern void draw_graph(int i, int flags, Graph_ctx *gr);
extern void setup_graph_data(int i, const int flags, int skip, Graph_ctx *gr);
extern double timer(int start);
extern void enable_layers(void);
extern void set_snap(double);
extern void set_grid(double);
extern void create_plot_cmd(void);
extern void set_modify(int mod);
extern void dbg(int level, char *fmt, ...);
extern unsigned int hash_file(const char *f, int skip_path_lines);
extern void here(double i);
extern void print_version(void);
extern int set_netlist_dir(int force, char *dir);
extern void netlist_options(int i);
extern int  check_lib(int what, const char *s);
extern void select_all(void);
extern void change_linewidth(double w);
extern void schematic_in_new_window(void);
extern void symbol_in_new_window(void);
extern void new_xschem_process(const char *cell, int symbol);
extern void ask_new_file(void);
extern void saveas(const char *f, int type);
extern const char *get_file_path(char *f);
extern int save(int confirm);
extern void save_ascii_string(const char *ptr, FILE *fd, int newline);
extern Hilight_hashentry *bus_hilight_hash_lookup(const char *token, int value, int what) ;
extern Hilight_hashentry *hilight_lookup(const char *token, int value, int what);
extern int  name_strcmp(char *s, char *d) ;
extern int search(const char *tok, const char *val, int sub, int sel);
extern int process_options(int argc, char **argv);
extern void calc_drawing_bbox(xRect *boundbox, int selected);
extern int ps_draw(int what);
extern void svg_draw(void);
extern void set_viewport_size(int w, int h, double lw);
extern void print_image();
extern const char *get_trailing_path(const char *str, int no_of_dir, int skip_ext);
extern const char *skip_dir(const char *str);
extern const char *get_cell(const char *str, int no_of_dir);
extern const char *get_cell_w_ext(const char *str, int no_of_dir);
extern const char *rel_sym_path(const char *s);
extern const char *abs_sym_path(const char *s, const char *ext);
extern const char *add_ext(const char *f, const char *ext);
extern void make_symbol(void);
extern void make_schematic_symbol_from_sel(void);
extern const char *get_sym_template(char *s, char *extra);
/* bit0: invoke change_linewidth(), bit1: centered zoom */
extern void zoom_full(int draw, int sel, int flags, double shrink);
extern void updatebbox(int count,xRect *boundbox,xRect *tmp);
extern void draw_selection(GC g, int interruptable);
extern void delete(int to_push_undo);
extern void delete_only_rect_line_arc_poly(void);
extern void polygon_bbox(double *x, double *y, int points, double *bx1, double *by1, double *bx2, double *by2);
extern void arc_bbox(double x, double y, double r, double a, double b,
                     double *bx1, double *by1, double *bx2, double *by2);
extern void bbox(int what,double x1,double y1, double x2, double y2);
extern int set_text_custom_font(xText *txt);
extern int text_bbox(const char * str,double xscale, double yscale,
            short rot, short flip, int hcenter, int vcenter, 
            double x1,double y1, double *rx1, double *ry1,
            double *rx2, double *ry2, int *cairo_lines, int *longest_line);

extern int get_color(int value);
extern void incr_hilight_color(void);
extern void hash_inst(int what, int n);
extern void del_inst_table(void);
extern void hash_wires(void);
extern void hash_wire(int what, int n, int incremental);
extern void hash_instances(void); /*  20171203 insert instance bbox in spatial hash table */

#if HAS_CAIRO==1
extern int text_bbox_nocairo(const char * str,double xscale, double yscale,
            short rot, short flip, int hcenter, int vcenter,
            double x1,double y1, double *rx1, double *ry1,
            double *rx2, double *ry2, int *cairo_lines, int *longest_line);
#endif

extern unsigned short select_object(double mx,double my, unsigned short sel_mode,
                                    int override_lock); /*  return type 20160503 */
extern void unselect_all(void);
extern void select_inside(double x1,double y1, double x2, double y2, int sel);
extern int Tcl_AppInit(Tcl_Interp *interp);
extern int callback(const char *winpath, int event, int mx, int my, KeySym key,
                        int button, int aux, int state);
extern void resetwin(int create_pixmap, int clear_pixmap, int force, int w, int h);
extern Selected find_closest_obj(double mx,double my);
extern void find_closest_net_or_symbol_pin(double mx,double my, double *x, double *y);

extern void drawline(int c, int what, double x1,double y1,double x2,double y2, int dash);
extern void draw_string(int layer,int what, const char *str, short rot, short flip, int hcenter, int vcenter,
       double x1, double y1, double xscale, double yscale);
extern void draw_symbol(int what,int c, int n,int layer,
            short tmp_flip, short tmp_rot, double xoffset, double yoffset);
extern void drawrect(int c, int what, double rectx1,double recty1,
            double rectx2,double recty2, int dash);
extern void filledrect(int c, int what, double rectx1,double recty1,
            double rectx2,double recty2);


extern void drawtempline(GC gc, int what, double x1,double y1,double x2,double y2);
extern void drawtemprect(GC gc, int what, double rectx1,double recty1,
            double rectx2,double recty2);
extern void drawtemparc(GC gc, int what, double x, double y, double r, double a, double b);
extern void drawarc(int c, int what, double x, double y, double r, double a, double b, int arc_fill, int dash);
extern void filledarc(int c, int what, double x, double y, double r, double a, double b);
extern void drawtemppolygon(GC gc, int what, double *x, double *y, int points);
extern void drawpolygon(int c, int what, double *x, double *y, int points, int poly_fill, int dash);
extern void draw_temp_symbol(int what, GC gc, int n,int layer,
            short tmp_flip, short tmp_rot, double xoffset, double yoffset);
extern void draw_temp_string(GC gc,int what, const char *str, short rot, short flip, int hcenter, int vcenter,
       double x1, double y1, double xscale, double yscale);

extern void draw(void);
extern void clip_xy_to_short(double x, double y, short *sx, short *sy);
extern int clip( double*,double*,double*,double*);
extern int textclip(int x1,int y1,int x2,int y2,
           double xa,double ya,double xb,double yb);
extern double dist_from_rect(double mx,
              double my, double x1, double y1, double x2, double y2);
extern double dist(double x1,double y1,double x2,double y2,double xa,double ya);
extern double rectdist(double x1,double y1,double x2,double y2,double xa,double ya);
extern int touch(double,double,double,double,double,double);
extern int rectclip(int,int,int,int,
           double*,double*,double*,double*);
extern void trim_wires(void);
extern void update_conn_cues(int draw_cues, int dr_win);
extern void break_wires_at_pins(void);

extern void check_touch(int i, int j,
         unsigned short *parallel,unsigned short *breaks,
         unsigned short *broken,unsigned short *touches,
         unsigned short *included, unsigned short *includes,
         double *xt, double *yt);

extern void storeobject(int pos, double x1,double y1,double x2,double y2,
                        unsigned short type,unsigned int rectcolor,
                        unsigned short sel, const char *prop_ptr);
extern void store_poly(int pos, double *x, double *y, int points,
           unsigned int rectcolor, unsigned short sel, char *prop_ptr);
extern void store_arc(int pos, double x, double y, double r, double a, double b,
               unsigned int rectcolor, unsigned short sel, char *prop_ptr);

extern void hier_psprint(void);
extern void global_spice_netlist(int global);
extern void global_tedax_netlist(int global);
extern void global_vhdl_netlist(int global);
extern void global_verilog_netlist(int global);
extern void vhdl_block_netlist(FILE *fd, int i);
extern void verilog_block_netlist(FILE *fd, int i);
extern void spice_block_netlist(FILE *fd, int i);
extern void remove_symbols(void);
extern void remove_symbol(int i);
extern void clear_drawing(void);
extern int load_sym_def(const char name[], FILE *embed_fd);
extern void descend_symbol(void);
extern int place_symbol(int pos, const char *symbol_name, double x, double y, short rot, short flip,
                         const char *inst_props, int draw_sym, int first_call, int to_push_undo);
extern void place_net_label(int type);
extern void attach_labels_to_inst(int interactive);
extern int connect_by_kissing(void);
extern void delete_files(void);
extern int sym_vs_sch_pins(void);
extern int match_symbol(const char name[]);
extern int save_schematic(const char *); /*  20171020 added return value */
extern void push_undo(void);
extern void pop_undo(int redo, int set_modify_status);
extern void delete_undo(void);
extern void clear_undo(void);
extern void mem_push_undo(void);
extern void mem_pop_undo(int redo, int set_modify_status);
extern void mem_delete_undo(void);
extern void mem_clear_undo(void);
extern void load_schematic(int load_symbol, const char *abs_name, int reset_undo);
/* check if filename already in an open window/tab */
extern int check_loaded(const char *f, char *win_path);
extern void link_symbols_to_instances(int from);
extern void load_ascii_string(char **ptr, FILE *fd);
extern char *read_line(FILE *fp, int dbg_level);
extern void read_record(int firstchar, FILE *fp, int dbg_level);
extern void create_sch_from_sym(void);
extern void descend_schematic(int instnumber);
extern void go_back(int confirm);
extern void view_unzoom(double z);
extern void view_zoom(double z);
extern void draw_stuff(void);
extern void new_wire(int what, double mx_snap, double my_snap);
extern void new_line(int what);
extern void new_arc(int what, double sweep);
extern void arc_3_points(double x1, double y1, double x2, double y2, double x3, double y3,
         double *x, double *y, double *r, double *a, double *b);
extern void move_objects(int what,int merge, double dx, double dy);
extern void check_collapsing_objects();
extern void redraw_w_a_l_r_p_rubbers(void); /* redraw wire, arcs, line, polygon rubbers */
extern void copy_objects(int what);
extern void find_inst_to_be_redrawn(int what);
extern void pan(int what, int mx, int my);
extern void zoom_rectangle(int what);
extern void zoom_box(double x1, double y1, double x2, double y2, double factor);
extern void save_restore_zoom(int save);
extern void select_rect(int what, int select);
extern void new_rect(int what);
extern void new_polygon(int what);
extern void compile_font(void);
extern void rebuild_selected_array(void);

extern void edit_property(int x);
extern int xschem(ClientData clientdata, Tcl_Interp *interp,
           int argc, const char * argv[]);
extern const char *tcleval(const char str[]);
extern const char *tclresult(void);
extern const char *tclgetvar(const char *s);
extern int tclgetboolvar(const char *s);
extern int tclgetintvar(const char *s);
extern double tclgetdoublevar(const char *s);
extern void tclsetvar(const char *s, const char *value);
extern void tclsetdoublevar(const char *s, const double value);
extern void tclsetboolvar(const char *s, const int value);
extern void tclsetintvar(const char *s, const int value);
extern int tclvareval(const char *script, ...);
extern const char *tcl_hook2(char **res);
extern void statusmsg(char str[],int n);
extern int place_text(int draw_text, double mx, double my);
extern void init_inst_iterator(Iterator_ctx *ctx, double x1, double y1, double x2, double y2);
extern Instentry *inst_iterator_next(Iterator_ctx *ctx);
extern void init_wire_iterator(Iterator_ctx *ctx, double x1, double y1, double x2, double y2);
extern Wireentry *wire_iterator_next(Iterator_ctx *ctx);
extern void check_unique_names(int rename);

extern void clear_instance_hash();

extern unsigned int str_hash(const char *tok);
extern void str_hash_free(Str_hashentry **table);
extern Str_hashentry *str_hash_lookup(Str_hashentry **table,
       const char *token, const char *value, int what);
extern void int_hash_free(Int_hashentry **table);
extern Int_hashentry *int_hash_lookup(Int_hashentry **table,
       const char *token, const int value, int what);

extern char *find_nth(const char *str, const char *sep, int n);
extern int isonlydigit(const char *s);
extern const char *translate(int inst, const char* s);
extern const char* translate2(Lcc *lcc, int level, char* s);
extern void print_tedax_element(FILE *fd, int inst);
extern int print_spice_element(FILE *fd, int inst);
extern void print_spice_subckt(FILE *fd, int symbol);
extern void print_tedax_subckt(FILE *fd, int symbol);
extern void print_vhdl_element(FILE *fd, int inst);
extern void print_verilog_element(FILE *fd, int inst);
extern const char *get_tok_value(const char *s,const char *tok,int with_quotes);
extern const char *list_tokens(const char *s, int with_quotes);
extern int  my_snprintf(char *str, int size, const char *fmt, ...);
extern size_t my_strdup(int id, char **dest, const char *src);
extern void my_strndup(int id, char **dest, const char *src, int n);
extern size_t my_strdup2(int id, char **dest, const char *src);
extern char *my_strtok_r(char *str, const char *delim, const char *quote, char **saveptr);
extern int my_strncpy(char *d, const char *s, int n);
extern int my_strcasecmp(const char *s1, const char *s2);
extern int my_strncasecmp(const char *s1, const char *s2, size_t n);
extern char* strtolower(char* s);
extern char* strtoupper(char* s);
extern void *my_malloc(int id, size_t size);
extern void my_realloc(int id, void *ptr,size_t size);
extern void *my_calloc(int id, size_t nmemb, size_t size);
extern void my_free(int id, void *ptr);
extern size_t my_strcat(int id, char **, const char *);
extern size_t my_mstrcat(int id, char **str, const char *append_str, ...);
extern char *itoa(int i);
extern char *dtoa(double i);
extern char *dtoa_prec(double i);
extern double my_round(double a);
extern double round_to_n_digits(double x, int n);
extern double floor_to_n_digits(double x, int n);
extern double ceil_to_n_digits(double x, int n);
extern const char *subst_token(const char *s, const char *tok, const char *new_val);
extern void new_prop_string(int i, const char *old_prop,int fast, int dis_uniq_names);
extern void hash_name(char *token, int remove);
extern void hash_all_names(int n);
extern void symbol_bbox(int i, double *x1,double *y1, double *x2, double *y2);
/* extern char *escape_chars(char *dest, const char *source, int size); */

extern void set_inst_prop(int i);
extern void unselect_wire(int i);
extern void select_hilight_net(void);
extern void check_wire_storage(void);
extern void check_text_storage(void);
extern void check_inst_storage(void);
extern void check_symbol_storage(void);
extern void check_selected_storage(void);
extern void check_box_storage(int c);
extern void check_arc_storage(int c);
extern void check_line_storage(int c);
extern void check_polygon_storage(int c);
extern const char *expandlabel(const char *s, int *m);
extern void parse(const char *s);
extern void clear_expandlabel_data(void);
extern void merge_file(int selection_load, const char ext[]);
extern void select_wire(int i, unsigned short select_mode, int fast);
extern void select_element(int i, unsigned short select_mode, int fast, int override_lock);
extern void select_text(int i, unsigned short select_mode, int fast);
extern void select_box(int c, int i, unsigned short select_mode, int fast, int override_lock);
extern void select_arc(int c, int i, unsigned short select_mode, int fast);
extern void select_line(int c, int i, unsigned short select_mode, int fast);
extern void select_polygon(int c, int i, unsigned short select_mode, int fast );
extern const char *net_name(int i, int j, int *mult, int hash_prefix_unnamed_net, int erc);
extern int record_global_node(int what, FILE *fp, char *node);
extern int count_items(const char *s, const char *sep, const char *quote);
extern int get_unnamed_node(int what, int mult, int node);
extern void node_hash_free(void);
extern void traverse_node_hash();
extern Node_hashentry
                *bus_node_hash_lookup(const char *token, const char *dir,int what, int port, char *sig_type,
                char *verilog_type, char *value, char *class);
/* extern void insert_missing_pin(); */
extern void round_schematic_to_grid(double cadsnap);
extern void save_selection(int what);
extern void print_vhdl_signals(FILE *fd);
extern void print_verilog_signals(FILE *fd);
extern void print_generic(FILE *fd, char *ent_or_comp, int symbol);
extern void print_verilog_param(FILE *fd, int symbol);
extern void hilight_net(int to_waveform);
extern void logic_set(int v, int num);
extern int hilight_netname(const char *name);
extern void unhilight_net();
extern void propagate_hilights(int set, int clear, int mode);
extern void  select_connected_wires(int stop_at_junction);
extern void draw_hilight_net(int on_window);
extern void display_hilights(char **str);
extern void redraw_hilights(int clear);
extern void set_tcl_netlist_type(void);
extern void prepare_netlist_structs(int for_netlist);
extern void free_simdata(void);
extern void delete_netlist_structs(void);
extern void delete_inst_node(int i);
extern void clear_all_hilights(void);
extern void hilight_child_pins(void);
extern void hilight_parent_pins(void);
extern void hilight_net_pin_mismatches(void);
extern Node_hashentry **get_node_table_ptr(void);
extern void change_elem_order(void);
extern int set_different_token(char **s,const char *new, const char *old, int object, int n);
extern void print_hilight_net(int show);
extern void list_hilights(void);
extern void change_layer();
extern void launcher();
extern void windowid(const char *winpath);
extern void preview_window(const char *what, const char *tk_win_path, const char *fname);
extern int new_schematic(const char *what, const char *win_path, const char *fname);
extern void toggle_fullscreen(const char *topwin);
extern void toggle_only_probes();
extern int build_colors(double dim, double dim_bg); /*  reparse the TCL 'colors' list and reassign colors 20171113 */
extern void set_clip_mask(int what);
#ifdef __unix__
extern int pending_events(void);
#endif
extern void get_square(double x, double y, int *xx, int *yy);
extern void del_wire_table(void);
extern void del_object_table(void);
extern const char *create_tmpdir(char *prefix);
extern FILE *open_tmpfile(char *prefix, char **filename);
extern void create_ps(char** psfile, int what);
extern void MyXCopyArea(Display* display, Drawable src, Drawable dest, GC gc, int src_x, int src_y, unsigned int width, unsigned int height, int dest_x, int dest_y);
#endif /*CADGLOBALS */
