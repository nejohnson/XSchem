/* File: xschem.h
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2016 Stefan Frederik Schippers
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

#define  _XOPEN_SOURCE 600     /* realpath(), round(), strtok_r(), getopt() */

/*  approximate PI definition */
#define XSCH_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170676

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <limits.h> /* PATH_MAX */

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>

#include <fcntl.h>
#include <time.h>

  
/* #include <sys/time.h>  for gettimeofday(). use time() instead */
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/xpm.h>

#include <tcl.h>
#include <tk.h>
/*  #include "memwatch.h" */

#define CADHEIGHT 400			/*  initial window size */
#define CADWIDTH 600

#define BACKLAYER 0
#define WIRELAYER 1
#define GRIDLAYER 2
#define SELLAYER 2
#define PROPERTYLAYER 1
#define TEXTLAYER 3
#define TEXTWIRELAYER 1 /*  color for wire name labels / pins */
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
#define CADMAXZOOM 10000.0
#define CADMINZOOM 0.0001
#define CADHALFDOTSIZE 2.5
#define CADNULLNODE -1	    /*  no valid node number */
#define CADWIREMINDIST 8.0
#define CADMAXWIRES 4096
#define CADMAXTEXT 2048
#define CADMAXOBJECTS 512   /*  (initial) max # of lines, rects (for each layer!!) */
#define MAXGROUP 300	    /*  (initial) max # of objects that can be drawn while moving */
#define ELEMINST 4096        /*  (initial) max # of placed elements,   was 600 20102004 */
#define ELEMDEF 256         /*  (initial) max # of defined elements */
#define CADMAXGRIDPOINTS 512
#define CADMAXHIER 80
#define CADCHUNKALLOC 512 /*  was 256  20102004 */
#define CADDRAWBUFFERSIZE 128

/*  20180104 when x-width of drawing area s below this threshold use spatial */
/*  hash table for drawing wires and instances (for faster lookup) instead of */
/*  looping through the whole wire[] and inst_ptr[] arrays */
/*  when drawing area is very big using spatial hash table may take longer than */
/*  a simple for() loop through the big arrays + clip check. */
#define ITERATOR_THRESHOLD  12000.0

#define SCHEMATIC 1
#define SYMBOL 2
#define CAD_SPICE_NETLIST 1
#define CAD_VHDL_NETLIST 2
#define CAD_VERILOG_NETLIST 3
#define CAD_TEDAX_NETLIST 4

#define STARTWIRE 1	    /*  possible states, encoded in global 'rubber' */
#define STARTPAN  2
#define STARTRECT 4
#define STARTLINE 8
#define SELECTION  16	    /*  signals that some objects are selected. */
#define STARTSELECT 32      /*  used for drawing a selection rectangle */
#define STARTMOVE 64        /*  used for move/copy  operations */
#define STARTCOPY 128       /*  used for move/copy  operations */
#define STARTZOOM 256       /*  used for move/copy  operations */
#define STARTMERGE 512       /*  used fpr merge schematic/symbol */
#define MENUSTARTWIRE 1024   /*  start wire invoked from menu */
#define MENUSTARTLINE 2048   /*  start line invoked from menu */
#define MENUSTARTRECT 4096   /*  start rect invoked from menu */
#define MENUSTARTZOOM 8192   /*  start zoom box invoked from menu */
#define STARTPAN2     16384  /*  new pan method with mouse button3 20121123 */
#define MENUSTARTTEXT 32768  /*  20161201 click to place text if action starts from menu */
#define MENUSTARTSNAPWIRE 65536   /*  start wire invoked from menu, snap to pin variant 20171022 */
#define STARTPOLYGON 131072   /*  20171115 */
#define MENUSTARTPOLYGON 262144   /*  20171117 */
#define STARTARC 524288
#define MENUSTARTARC 1048576
#define MENUSTARTCIRCLE 2097152

#define SELECTED 1          /*  used in the .sel field for selected objs. */
#define SELECTED1 2	    /*  first point selected... */
#define SELECTED2 4         /*  second point selected... */
#define SELECTED3 8
#define SELECTED4 16


#define WIRE 1		    /*  types of defined objects */
#define RECT  2
#define LINE 4
#define ELEMENT 8
#define TEXT 16
#define POLYGON 32 /*  20171115 */
#define ARC 64

/*  for netlist.c */
#define BOXSIZE 1000
#define NBOXES 50


/*    some useful primes */
/*    109, 163, 251, 367, 557, 823, 1237, 1861, 2777, 4177, 6247, 9371, 14057 */
/*    21089, 31627, 47431, 71143, 106721, 160073, 240101, 360163, 540217, 810343 */
/*    1215497, 1823231, 2734867, 4102283, 6153409, 9230113, 13845163 */

#define HASHSIZE 31627

/*  20180104 */
/*  big enouth to contain instances and wires in viewable area when draw() switching */
/*  to spatial hash iterator. Dont make it too big as clearing big hash tables */
/*  (and this is done on every redraw) takes time. */
#define INTHASHSIZE 4177

		   /*  parameters passed to action functions, see actions.c */
#define END      1 /*  endop */
#define BEGIN    2 /*  begin placing something */
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
#define CLEAR 4096 /* used in new_wire to clear previous rubber when switching manhattan_lines */
#define DRAW 8192  /* used in bbox() to draw things by using XCopyArea after setting clip rectangle */
#define FONTWIDTH 20
#define FONTOFFSET 40
#define FONTHEIGHT 40
#define FONTDESCENT 15
#define FONTWHITESPACE 10

#define S(a) (sizeof(a)/sizeof(char))
#define BUS_WIDTH ( lw_double>5 ? (int)lw_double:5 )
#define POINTINSIDE(xa,ya,x1,y1,x2,y2)  \
 (xa>=x1 && xa<=x2 && ya>=y1 && ya<=y2 )

#define RECTINSIDE(xa,ya,xb,yb,x1,y1,x2,y2)  \
 (xa>=x1 && xa<=x2 && xb>=x1 && xb<=x2 && ya>=y1 && ya<=y2 && yb>=y1 && yb<=y2 )

#define RECTOUTSIDE(xa,ya,xb,yb,x1,y1,x2,y2)  \
 (xb<x1 || xa>x2 || yb<y1 || ya>y2)

#define ROTATION(x0, y0, x, y, rx, ry) \
xxtmp = (flip ? 2 * x0 -x : x); \
if(rot==0)      {rx = xxtmp;  ry = y;} \
else if(rot==1) {rx = x0 - y + y0; ry = y0 + xxtmp - x0;} \
else if(rot==2) {rx = 2 * x0 - xxtmp; ry = 2 * y0 - y;} \
else            {rx = x0 + y - y0; ry = y0 - xxtmp + x0;}

#define ORDER(x1,y1,x2,y2) \
if(x2 < x1) {xxtmp=x1;x1=x2;x2=xxtmp;xxtmp=y1;y1=y2;y2=xxtmp;} \
else if(x2 == x1 && y2 < y1) {xxtmp=y1;y1=y2;y2=xxtmp;}
#define RECTORDER(x1,y1,x2,y2) \
if(x2 < x1) { xxtmp=x1;x1=x2;x2=xxtmp;} \
if(y2 < y1) { xxtmp=y1;y1=y2;y2=xxtmp;}

#define OUTSIDE(xa,ya,xb,yb,x1,y1,x2,y2) \
 (xa>=x2 || xb<=x1 ||  ya>=y2 || yb<=y1 )

#define LINE_OUTSIDE(xa,ya,xb,yb,x1,y1,x2,y2) \
 (xa>=x2 || xb<=x1 ||  ( (ya<yb)? (ya>=y2 || yb<=y1) : (yb>=y2 || ya<=y1) ) )

#define CLIP(x,a,b) (x<a?a:x>b?b:x)

#define MINOR(a,b) ( (a) <= (b) ? (a) : (b) )

#define X_TO_SCREEN(x) ( floor((x+xorigin)* mooz) )
#define Y_TO_SCREEN(y) ( floor((y+yorigin)* mooz) )
#define X_TO_XSCHEM(x) ((x)*zoom -xorigin)
#define Y_TO_XSCHEM(y) ((y)*zoom -yorigin)

typedef struct
{
   unsigned short type;
   int n;
   unsigned short col;
} Selected;

typedef struct 
{
   double x1;
   double x2;
   double y1;
   double y2;
   unsigned short  end1;
   unsigned short  end2;
   unsigned short sel;
   char  *node;
   char *prop_ptr;
   int bus; /*  20171201 cache here wire "bus" property, to avoid too many get_tok_value() calls */
} Wire;

typedef struct
{
   double x1;
   double x2;
   double y1;
   double y2;
   unsigned short sel;
   char *prop_ptr;
} Line;

typedef struct
{
   double x1;
   double x2;
   double y1;
   double y2;
   unsigned short sel;
   char *prop_ptr;
} Box;


typedef struct /*  20171115 */
{
  /*  last point coincident to first, added by program if needed. */
  /*  XDrawLines needs first and last point to close the polygon */
  int points;
  double *x;
  double *y;
  unsigned short *selected_point;
  unsigned short sel;
  char *prop_ptr;
  int fill; /*  20180914 */
} Polygon; 

typedef struct // 20181012
{
  double x;
  double y;
  double r;
  double a; /* start angle */
  double b; /* arc angle */
  unsigned short sel;
  char *prop_ptr;
} Arc;

typedef struct
{
  char *txt_ptr;
  double x0,y0;
  int rot;
  int flip;
  int sel;
  double xscale;
  double yscale;
  char *prop_ptr;
  int layer; /*  20171201 for cairo  */
  char *font; /*  20171201 for cairo */
} Text;

typedef struct
{
   char *name;
   double minx;
   double maxx;
   double miny;
   double maxy;
   Line **lineptr;  /*  array of [cadlayers] pointers to Line */
   Box  **boxptr;
   Polygon **polygonptr; /* 20171115 */
   Arc **arcptr; // 20181012
   Text  *txtptr;
   int *lines;     /*  array of [cadlayers] integers */
   int *rects;
   int *polygons; /* 20171115 */
   int *arcs; /* 20181012 */
   int texts;
   char *prop_ptr;
   char *type; /*  20150409 */
   char *templ; /*  20150409 */
} Instdef;

typedef struct
{
   char *name;/*  symbol name (ex: devices/lab_pin)  */
   int ptr;  /*  was a pointer formerly... */
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
   int rot;
   int flip;
   int sel;
   int flags; /*  bit 0: skip field, bit 1: flag for different textlayer for pin/labels
	       *  bit 2 : hilight flag. 
               */
   char *prop_ptr;
   char **node;
   char *instname; /*  20150409 instance name (example: I23)  */
} Instance;

typedef struct 
{
  double x;
  double y;
  double zoom;
} Zoom;

struct drivers {
                int in;
                int out;
                int inout;
                int port;
               };
       

struct int_hashentry { /*  20180104 */
                  struct int_hashentry *next;
                  unsigned int token;
                 };


struct node_hashentry {
                  struct node_hashentry *next;
                  unsigned int hash;
                  char *token;
                  char *sig_type;
                  char *verilog_type;
                  char *value;
                  char *class;
                  char *orig_tok;
                  struct drivers d;
                 };


struct hilight_hashentry {
                  struct hilight_hashentry *next;
                  unsigned int hash;
                  char *token;
                  char *path;
                  int value;
                 };

/*  for netlist.c */
struct instpinentry {
 struct instpinentry *next;
 double x0,y0;
 int n;
 int pin;
};

struct wireentry {
  struct wireentry *next;
  int n;
};

struct objectentry {
 struct objectentry *next;
 int n;
};


extern int help; /* 20140406 */
extern char *cad_icon[];
extern int a3page;
extern int manhattan_lines;
extern int cadlayers;
extern  int hilight_color;
extern int do_print;
extern int prepared_netlist_structs;
extern int prepared_hilight_structs;
extern int prepared_hash_objects;
extern int prepared_hash_wires;
extern void hash_inst_pin(int i, int j);
extern int has_x; 
extern int sym_txt;
extern int rainbow_colors; 
extern FILE *errfp;
extern int no_readline;
extern char *filename;
extern char home_dir[PATH_MAX]; /* home dir obtained via getpwuid */
extern char pwd_dir[PATH_MAX]; /* obtained via getcwd() */
extern int load_initfile;
extern int persistent_command;

extern int debug_var; 
extern char **color_array;
extern Colormap colormap;
extern int current_type;
extern unsigned int color_index[];
extern int lw; /*  line width */
extern double lw_double; /*  line width */
extern int change_lw; /*  allow change line width */
extern int thin_text;
extern int incr_hilight;
extern int auto_hilight;
extern int fill; /*  fill rectangles */
extern int draw_grid;
extern int draw_pixmap; /*  pixmap used as 2nd buffer */
extern int draw_window; /* 20181009 */
extern int need_rebuild_selected_array;
extern unsigned int rectcolor;
extern XEvent xev;
extern KeySym key;
extern double xxtmp;
extern unsigned short enable_stretch;
extern unsigned int button;
extern unsigned int state; /*  status of shift,ctrl etc.. */
extern Wire *wire;
extern Box  **rect;
extern Polygon **polygon; /*  20171115 */
extern Arc **arc; /*  20181012 */
extern Line **line;
extern XPoint *gridpoint;
extern XRectangle *rectangle;
extern Selected *selectedgroup; /*  array of selected objs to draw while moving */
extern int lastwire;
extern int lastselected;
extern int *lastrect;
extern int *lastpolygon; /*  20171115 */
extern int *lastarc; /*  20181012 */
extern int *lastline;
extern int lastinst ;
extern int lastinstdef ;
extern int lasttext;              
extern Instance *inst_ptr;      /*  Pointer to element INSTANCE */
extern Instdef *instdef;	/*  Pointer to element definition */
extern Text *textelement; 
extern char schematic[CADMAXHIER][PATH_MAX];
extern int currentsch;
extern char *schtedaxprop; 
extern char *schprop; 
extern char *schvhdlprop; 
extern char *schverilogprop; 
extern int max_texts;
extern int max_wires;
extern int max_instances;
extern int max_symbols;
extern int max_selected;
extern int *max_rects;
extern int *max_polygons; /*  20171115 */
extern int *max_arcs; /*  20181012 */
extern int *max_lines;
extern int previous_instance[];
extern int split_files;
extern int hspice_netlist;
extern char *netlist_dir;

extern unsigned long ui_state ; /*  this signals that we are doing a net place, */
			      /*  panning etc... */

extern char *undo_dirname; /*  20150327 */
extern int cur_undo_ptr;
extern int tail_undo_ptr;
extern int head_undo_ptr;
extern int max_undo;
extern int draw_dots;
extern int draw_single_layer; /*  20151117 */

extern Window window;
extern Window parent_of_topwindow;
extern Pixmap cad_icon_pixmap, *pixmap,save_pixmap;
extern int depth;
extern int *fill_type; /* 20171117 for every layer: 0: no fill, 1, solid fill, 2: stipple fill */
extern unsigned char **pixdata;
extern unsigned char pixdata_init[22][32];
extern int  areax1,areay1,areax2,areay2,areaw,areah;
extern GC *gc, *gcstipple, gctiled;
extern Display *display;
extern Tcl_Interp *interp;
extern XRectangle xrect[];
extern int xschem_h, xschem_w; /*  20171130 window size */
extern double xorigin,yorigin;
extern double zoom;
extern double mooz;
extern double mousex,mousey; /*  mouse coord. */
extern double mousex_snap,mousey_snap; /*  mouse coord. snapped to grid */
extern double cadsnap;
extern int horizontal_move; /*  20171023 */
extern int vertical_move; /*  20171023 */

extern void set_snap(double); /*  20161212 */
extern double *character[256];
extern int netlist_show;
extern int flat_netlist;
extern int netlist_type;
extern int do_netlist;
extern int do_simulation;
extern int do_waves;
extern int netlist_count;
extern int quit;
extern int show_erc;
extern int hilight_nets;
extern char *sch_prefix[];
extern int modified;
extern int color_ps;
extern int only_probes; /*  20110112 */
extern Zoom zoom_array[];
extern int pending_fullzoom;
extern int fullscreen;
extern int unzoom_nodrift;
extern XColor xcolor_array[];/*  20171109 */
extern Visual *visual;
extern int dark_colorscheme; /*  20171113 */
extern double color_dim;
extern int skip_dim_background;
extern int no_undo; /*  20171204 */
extern int enable_drill;

extern struct wireentry *wiretable[NBOXES][NBOXES];
extern struct instpinentry *instpintable[NBOXES][NBOXES];
extern double mx_double_save, my_double_save; /*  20070322 */
extern struct objectentry *objecttable[NBOXES][NBOXES];
extern size_t get_tok_value_size;

/*  functions */
extern int set_netlist_dir(int force);
extern int  check_lib(char * s);
extern void select_all(void);
extern void change_linewidth(double w);
extern void set_fill(int n);
extern void schematic_in_new_window(void);
extern void symbol_in_new_window(void);
extern void new_window(char *cell, int symbol);
extern void ask_new_file(void);
extern void saveas(void);
extern int save(int confirm);
extern struct hilight_hashentry *bus_hilight_lookup(const char *token, int value, int remove) ;
extern int  name_strcmp(char *s, char *d) ;
extern void search_inst(char *tok, char *val, int sub, int sel, int what);
extern void process_options(int argc, char **argv);
extern void calc_drawing_bbox(Box *boundbox);
extern void ps_draw(void);
extern void svg_draw(void);
extern void print_image();
extern const char *skip_dir(const char *str);
extern const char *get_cell(const char *str, int no_of_dir);
extern const char *get_cell_w_ext(const char *str, int no_of_dir);
extern const char *rel_sym_path(const char *s);
extern const char *abs_sym_path(const char *s, const char *required_ext);
extern void make_symbol(void);
extern char *get_sym_template(char *s, char *extra);
extern void zoom_full(int draw);
extern void updatebbox(int count,Box *boundbox,Box *tmp);
extern void draw_selection(GC g, int interruptable);
extern void delete(void);
extern void delete_only_rect_line_arc_poly(void);
extern void polygon_bbox(double *x, double *y, int points, double *bx1, double *by1, double *bx2, double *by2);
extern void arc_bbox(double x, double y, double r, double a, double b, double *bx1, double *by1, double *bx2, double *by2);
extern void bbox(int what,double x1,double y1, double x2, double y2);
extern int set_text_custom_font(Text *txt);
extern int text_bbox(char * str,double xscale, double yscale,
            int rot, int flip, double x1,double y1, double *rx1, double *ry1,
            double *rx2, double *ry2);

/* //test 20171203 */
extern void del_object_table(void);
extern void hash_wires(void);
extern void hash_wire(int n);
extern void hash_objects(void); /*  20171203 insert instance bbox in spatial hash table */

extern struct int_hashentry *int_hash_lookup(struct int_hashentry **table, int token, int remove); /*  20180104 */
extern void free_int_hash(struct int_hashentry **table); /*  20180104 */

#ifdef HAS_CAIRO
extern int text_bbox_nocairo(char * str,double xscale, double yscale,
            int rot, int flip, double x1,double y1, double *rx1, double *ry1,
            double *rx2, double *ry2);
#endif

extern unsigned short select_object(double mx,double my, unsigned short sel_mode); /*  return type 20160503 */
extern void unselect_all(void);
extern void select_inside(double x1,double y1, double x2, double y2, int sel);
extern void xwin_exit(void);
extern int Tcl_AppInit(Tcl_Interp *interp); 
extern int source_tcl_file(char *s);
extern int callback(int event, int mx, int my, KeySym key,
                        int button, int aux, int state);
extern void resetwin(void);
extern void find_closest_net(double mx,double my);
extern void find_closest_box(double mx,double my);
extern void find_closest_arc(double mx,double my);
extern void find_closest_element(double mx,double my);
extern void find_closest_line(double mx,double my);
extern void find_closest_polygon(double mx,double my);/* 20171115 */
extern void find_closest_text(double mx,double my);
extern Selected find_closest_obj(double mx,double my);
extern void find_closest_net_or_symbol_pin(double mx,double my, double *x, double *y);

extern void drawline(int c, int what, double x1,double y1,double x2,double y2);
extern void draw_string(int layer,int what, char *str, int rot, int flip, 
       double x1, double y1, double xscale, double yscale);
extern void draw_symbol_outline(int what,int c, int n,int layer,
            int tmp_flip, int tmp_rot, double xoffset, double yoffset);
extern void drawrect(int c, int what, double rectx1,double recty1,
            double rectx2,double recty2);
extern void filledrect(int c, int what, double rectx1,double recty1,
            double rectx2,double recty2);


extern void drawtempline(GC gc, int what, double x1,double y1,double x2,double y2);
extern void drawgrid(void);
extern void drawtemprect(GC gc, int what, double rectx1,double recty1,
            double rectx2,double recty2);
extern void drawtemparc(GC gc, int what, double x, double y, double r, double a, double b);
extern void drawarc(int c, int what, double x, double y, double r, double a, double b);
extern void drawtemppolygon(GC gc, int what, double *x, double *y, int points);
extern void drawpolygon(int c, int what, double *x, double *y, int points, int poly_fill);
extern void draw_temp_symbol_outline(int what, GC gc, int n,int layer,
            int tmp_flip, int tmp_rot, double xoffset, double yoffset);
extern void draw_temp_string(GC gc,int what, char *str, int rot, int flip, 
       double x1, double y1, double xscale, double yscale);


extern void draw(void);
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
extern void break_wires_at_pins(void);

extern void check_touch(int i, int j,
         unsigned short *parallel,unsigned short *breaks,
         unsigned short *broken,unsigned short *touches,
         unsigned short *included, unsigned short *includes,
         double *xt, double *yt);

extern void storeobject(int pos, double x1,double y1,double x2,double y2,
                        unsigned short type,unsigned int rectcolor,
		        unsigned short sel, char *prop_ptr);
extern void store_polygon(int pos, double *x, double *y, int points,  /*  20171115 */
           unsigned int rectcolor, unsigned short sel, char *prop_ptr);
extern void store_arc(int pos, double x, double y, double r, double a, double b,
               unsigned int rectcolor, unsigned short sel, char *prop_ptr);

extern void freenet_nocheck(int i);
extern void spice_netlist(FILE *fd, int spice_stop);
extern void tedax_netlist(FILE *fd, int spice_stop);
extern void global_spice_netlist(int global);
extern void global_tedax_netlist(int global);
extern void vhdl_netlist(FILE *fd, int vhdl_stop);
extern void global_vhdl_netlist(int global);
extern void verilog_netlist(FILE *fd, int verilog_stop);
extern void global_verilog_netlist(int global);
extern void vhdl_block_netlist(FILE *fd, int i);
extern void verilog_block_netlist(FILE *fd, int i);
extern void spice_block_netlist(FILE *fd, int i);
extern void tedax_block_netlist(FILE *fd, int i);
extern int save_symbol(char *);
extern void remove_symbols(void);
extern void remove_symbol(void);
extern void clear_drawing(void);
extern int load_symbol_definition(char name[]);
extern void load_symbol(const char *abs_name);
extern void descend_symbol(void);
extern void place_symbol(int pos, char *symbol_name, double x, double y, int rot, int flip, 
                         char *inst_props, int draw_sym, int first_call);
extern void attach_labels_to_inst(void);
extern int match_symbol(char name[]);
extern int save_schematic(char *); /*  20171020 added return value */
extern void push_undo(void);
extern void pop_undo(int redo);
extern void delete_undo(void);
extern void clear_undo(void);
extern void load_schematic(int load_symbol, const char *abs_name, int reset_undo);
extern void link_symbols_to_instances(void);
extern void load_ascii_string(char **ptr, FILE *fd);
extern void load_text(FILE *fd);
extern void load_wire(FILE *fd);
extern void load_inst(FILE *fd);
extern void load_box(FILE *fd);
extern void load_arc(FILE *fd);
extern void read_xschem_file(FILE *fd); /*  20180912 */
extern char *read_line(FILE *fp);
extern void load_polygon(FILE *fd); /*  20171115 */
extern void load_line(FILE *fd);
extern void create_sch_from_sym(void);
extern void descend_schematic(void);
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
extern void copy_objects(int what);
extern void pan(int what);
extern void pan2(int what, int mx, int my);
extern void zoom_box(int what);
extern void select_rect(int what, int select);
extern void new_rect(int what);
extern void new_polygon(int what); /*  20171115 */
extern void compile_font(void);
extern void rebuild_selected_array(void);

extern void edit_property(int x);
extern int xschem(ClientData clientdata, Tcl_Interp *interp, 
           int argc, char * argv[]);
extern void tcleval(const char str[]);
extern const char *tclgetvar(const char *s);
extern void tclsetvar(const char *s, const char *value);
extern void statusmsg(char str[],int n);
extern void place_text(int draw_text, double mx, double my);
extern void hash_proplist(char *s,int remove);
extern struct hashentry *hash_lookup(char *token,char *value,int remove, size_t token_size);
extern void init_inst_iterator(double x1, double y1, double x2, double y2);
extern struct objectentry *inst_iterator_next();
extern void init_wire_iterator(double x1, double y1, double x2, double y2);
extern struct wireentry *wire_iterator_next();
extern void free_hash(void);
extern char *find_nth(char *str, char sep, int n);
extern char *translate(int inst, char* s);
extern void print_tedax_element(FILE *fd, int inst);
extern void print_spice_element(FILE *fd, int inst);
extern void print_vhdl_element(FILE *fd, int inst);
extern void print_verilog_element(FILE *fd, int inst);
extern void print_verilog_primitive(FILE *fd, int inst);
extern void print_vhdl_primitive(FILE *fd, int inst);
extern char *get_tok_value(char *s,char *tok,int with_quotes);
extern int  my_snprintf(char *str, int size, const char *fmt, ...);
extern size_t my_strdup(char **dest, const char *src);
extern void my_strndup(char **dest, const char *src, int n);
extern size_t my_strdup2(char **dest, const char *src);
extern void my_strncpy(char *d, const char *s, int n);
extern char *subst_token(char *s,char *tok, char *new_val);
extern void new_prop_string(char **new_prop,char *old_prop,int fast);
extern void symbol_bbox(int i, double *x1,double *y1, double *x2, double *y2);
extern void set_inst_prop(int i);
extern void *my_malloc(size_t size);
extern void my_realloc(void *ptr,size_t size);
extern void *my_calloc(size_t nmemb, size_t size);
extern void my_free(void *ptr);
extern void unselect_wire(int i);
extern void check_wire_storage(void);
extern void check_text_storage(void);
extern void check_inst_storage(void);
extern void check_symbol_storage(void);
extern void check_selected_storage(void);
extern void check_box_storage(int c);
extern void check_arc_storage(int c);
extern void check_line_storage(int c);
extern void check_polygon_storage(int c); /*  20171115 */
extern const char *expandlabel(const char *s, int *m);
extern void merge_inst(int k, FILE *fd);
extern void merge_file(int selection_load, char ext[]);
extern void select_wire(int i, unsigned short select_mode, int fast);
extern void select_element(int i, unsigned short select_mode, int fast);
extern void select_text(int i, unsigned short select_mode, int fast);
extern void select_box(int c, int i, unsigned short select_mode, int fast);
extern void select_arc(int c, int i, unsigned short select_mode, int fast);
extern void select_line(int c, int i, unsigned short select_mode, int fast);
extern void select_polygon(int c, int i, unsigned short select_mode, int fast );
extern const char *pin_node(int i, int j, int *mult, int hash_prefix_unnamed_net);
extern void record_global_node(int what, FILE *fp, char *node);
extern int count_labels(char *s);
extern size_t my_strcat(char **, const char *);
extern int get_unnamed_node(int what, int mult, int node);
extern void free_node_hash(void);
extern struct node_hashentry 
		*node_hash_lookup(char *token, char *dir,int remove, int port, char *sig_type, 
                char *verilog_type, char *value, char *class, char *orig_tok);
extern void traverse_node_hash();
extern struct node_hashentry 
		*bus_hash_lookup(char *token, char *dir,int remove, int port, char *sig_type, 
                char *verilog_type, char *value, char *class);
/* extern void insert_missing_pin(); */
extern void round_schematic_to_grid(double cadsnap);
extern void save_selection(int what);
extern void print_vhdl_signals(FILE *fd);
extern void print_verilog_signals(FILE *fd);
extern void print_generic(FILE *fd, char *ent_or_comp, int symbol);
extern void print_verilog_param(FILE *fd, int symbol);
extern void hilight_net();
extern void unhilight_net();
extern void draw_hilight_net(int on_window);
extern void undraw_hilight_net(int on_window); /*  20160413 */
extern void prepare_netlist_structs(int for_hilight_only);
extern void delete_netlist_structs(void);
extern void delete_inst_node(int i);
extern void delete_hilight_net(void);
extern void hilight_child_pins(int n);
extern void hilight_parent_pins(void);
extern struct node_hashentry **get_node_table_ptr(void);
extern void change_elem_order(void);
extern int set_different_token(char **s,char *new, char *old);
extern void print_hilight_net(int show);
extern void change_layer();
extern void launcher(); /*  20161102 */
extern void windowid();
extern int window_state (Display *disp, Window win, char *arg);
extern void toggle_fullscreen();
extern void toggle_only_probes(); /*  20110112 */
extern void fill_symbol_editprop_form(int x);
extern void update_symbol(char *result, int x);
extern void tclexit(ClientData s);
extern int build_colors(int skip_background, double dim); /*  reparse the TCL 'colors' list and reassign colors 20171113 */
extern void drill_hilight(void);
extern void get_square(double x, double y, int *xx, int *yy);
extern void del_wire_table(void); /*  20180917 */
extern void del_object_table(void); /*  20180917 */
extern const char *random_string(const char *prefix);
extern const char *create_tmpdir(char *prefix);
extern FILE *open_tmpfile(char *prefix, char **filename);
extern void child_handler(int signum);
#ifdef HAS_CAIRO /*  20171105 */
#include <cairo.h>
#include <cairo-xlib.h>
#include "cairo-xlib-xrender.h"

  #if HAS_XCB==1
  #include <X11/Xlib-xcb.h>  /*  20171125 */
  #include <cairo-xcb.h>
  extern xcb_connection_t *xcbconn; /*  20171125 */
  extern xcb_screen_t *screen_xcb;
  extern xcb_render_pictforminfo_t format_rgb, format_rgba;
  extern xcb_visualtype_t *visual_xcb;
  #endif  /*  HAS_XCB */

extern cairo_surface_t *sfc, *save_sfc;
extern cairo_t *ctx, *save_ctx;
extern XRenderPictFormat *format;


#endif /*  HAS_CAIRO */
extern char cairo_font_name[1024]; /*  should be monospaced */
extern int cairo_longest_line;
extern int cairo_lines;
extern double cairo_font_scale; /*  default: 1.0, allows to adjust font size */
extern double cairo_font_line_spacing; /*  allows to change line spacing: default: 1.0 */
extern double cairo_vert_correct;

#endif


