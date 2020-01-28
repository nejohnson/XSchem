/* File: xinit.c
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2020 Stefan Frederik Schippers
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

#include "xschem.h"
#include <pwd.h> /* getpwuid */

static int init_done=0; /* 20150409 to avoid double call by Xwindows close and TclExitHandler */
static Window save_window;
static XSetWindowAttributes winattr;
static int screen_number;
static Tk_Window  tkwindow, mainwindow, tkpre_window;
static XWMHints *hints_ptr;
static Window topwindow;
static XColor xcolor_exact,xcolor;
typedef int myproc(
             ClientData clientData,
             Tcl_Interp *interp,
             int argc,
             const char *argv[]);

/* ----------------------------------------------------------------------- */
/* EWMH message handling routines 20071027... borrowed from wmctrl code */
/* ----------------------------------------------------------------------- */
#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

static int client_msg(Display *disp, Window win, char *msg, /* {{{ */
        unsigned long data0, unsigned long data1,
        unsigned long data2, unsigned long data3,
        unsigned long data4) {
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(disp, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = data0;
    event.xclient.data.l[1] = data1;
    event.xclient.data.l[2] = data2;
    event.xclient.data.l[3] = data3;
    event.xclient.data.l[4] = data4;

    if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event)) {
        return EXIT_SUCCESS;
    }
    else {
        fprintf(errfp, "Cannot send %s event.\n", msg);
        return EXIT_FAILURE;
    }
}/*}}}*/


int window_state (Display *disp, Window win, char *arg) {/*{{{*/
    static char *arg_copy=NULL;
    unsigned long action;
    int i;
    Atom prop1 = 0;
    Atom prop2 = 0;
    char *p1, *p2;
    const char *argerr = "expects a list of comma separated parameters: \"(remove|add|toggle),<PROP1>[,<PROP2>]\"\n";


    my_strdup(604, &arg_copy, arg);

    if(debug_var>=1) fprintf(errfp,"window_state() , win=0x%x arg_copy=%s\n", 
          (int)win,arg_copy);
    if (!arg_copy || strlen(arg_copy) == 0) {
        fputs(argerr, errfp);
        return EXIT_FAILURE;
    }

    if ((p1 = strchr(arg_copy, ','))) {
        static char tmp_prop1[1024], tmp1[1024]; /* overflow safe 20161122 */
        
        *p1 = '\0';

        /* action */
        if (strcmp(arg_copy, "remove") == 0) {
            action = _NET_WM_STATE_REMOVE;
        }
        else if (strcmp(arg_copy, "add") == 0) {
            action = _NET_WM_STATE_ADD;
        }
        else if (strcmp(arg_copy, "toggle") == 0) {
            action = _NET_WM_STATE_TOGGLE;
        }
        else {
            fputs("Invalid action. Use either remove, add or toggle.\n", errfp);
            return EXIT_FAILURE;
        }
        p1++;

        /* the second property */
        if ((p2 = strchr(p1, ','))) {
            static char tmp_prop2[1024], tmp2[1024]; /* overflow safe */
            *p2 = '\0';
            p2++;
            if (strlen(p2) == 0) {
                fputs("Invalid zero length property.\n", errfp);
                return EXIT_FAILURE;
            }
            for( i = 0; p2[i]; i++) tmp2[i] = toupper( p2[i] );
            my_snprintf(tmp_prop2, S(tmp_prop2), "_NET_WM_STATE_%s", tmp2);
            prop2 = XInternAtom(disp, tmp_prop2, False);
        }

        /* the first property */
        if (strlen(p1) == 0) {
            fputs("Invalid zero length property.\n", errfp);
            return EXIT_FAILURE;
        }
        for( i = 0; p1[i]; i++) tmp1[i] = toupper( p1[i] );
        my_snprintf(tmp_prop1, S(tmp_prop1), "_NET_WM_STATE_%s", tmp1);
        prop1 = XInternAtom(disp, tmp_prop1, False);

        
        return client_msg(disp, win, "_NET_WM_STATE", 
            action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
    }
    else {
        fputs(argerr, errfp);
        return EXIT_FAILURE;
    }
}/*}}}*/

/* ----------------------------------------------------------------------- */

void windowid()
{
  int i;
  Display *display;
  Tk_Window mainwindow;

  unsigned int ww;
  Window framewin, rootwindow;
  Window *framewin_child_ptr;
  unsigned int framweindow_nchildren;
 
    mainwindow=Tk_MainWindow(interp);
    display = Tk_Display(mainwindow);
    tcleval( "winfo id .");
    sscanf(Tcl_GetStringResult(interp), "0x%x", (unsigned int *) &ww);
    framewin = ww;
    XQueryTree(display, framewin, &rootwindow, &parent_of_topwindow, &framewin_child_ptr, &framweindow_nchildren);
    if(debug_var>=1) fprintf(errfp,"framewinID=%x\n", (unsigned int) framewin);
    if(debug_var>=1) fprintf(errfp,"framewin nchilds=%d\n", (unsigned int) framweindow_nchildren);
    if(debug_var>=1) fprintf(errfp,"framewin parentID=%x\n", (unsigned int) parent_of_topwindow);
    if(debug_var>=1) fprintf(errfp,"framewin child 0=%x\n", (unsigned int) framewin_child_ptr[0]);

    /* here I create the icon pixmap,to be used when iconified,  */
    if(!cad_icon_pixmap) {
      i=XpmCreatePixmapFromData(display,framewin, cad_icon,&cad_icon_pixmap, NULL, NULL);
      if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): creating icon pixmap returned: %d\n",i);
      hints_ptr = XAllocWMHints();
      hints_ptr->icon_pixmap = cad_icon_pixmap ;
      hints_ptr->flags = IconPixmapHint ;
      XSetWMHints(display, parent_of_topwindow, hints_ptr);
      XFree(hints_ptr);
    }
    Tcl_SetResult(interp,"",TCL_STATIC);
}


void xwin_exit(void)
{
 int i;
  
 if(!init_done) {
   if(debug_var>=1) fprintf(errfp, "xwin_exit() double call, doing nothing...\n");
   return;  /* 20150409 */
 }
 delete_netlist_structs();
 delete_hilight_net();
 get_unnamed_node(0, 0, 0);

 if(has_x) {
    #ifdef HAS_CAIRO /* 20171105 */
    cairo_destroy(ctx);
    cairo_destroy(save_ctx);
    cairo_surface_destroy(sfc);
    cairo_surface_destroy(save_sfc);
    #endif
    XFreePixmap(display,save_pixmap);
    for(i=0;i<cadlayers;i++)XFreePixmap(display,pixmap[i]);
    if(debug_var>=1) fprintf(errfp, "xwin_exit(): Releasing pixmaps\n");
    for(i=0;i<cadlayers;i++) 
    {
     XFreeGC(display,gc[i]);
     XFreeGC(display,gcstipple[i]);
    }
    XFreeGC(display,gctiled);
    if(debug_var>=1) fprintf(errfp, "xwin_exit(): destroying tk windows and releasing X11 stuff\n");
    Tk_DestroyWindow(mainwindow);
    if(cad_icon_pixmap) XFreePixmap(display, cad_icon_pixmap);
 }
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): clearing drawing data structures\n"); 
 clear_drawing();
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): freeing graphic primitive arrays\n"); 
 my_free(&wire);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): wire\n"); 
 my_free(&gridpoint);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): gridpoint\n"); 
 my_free(&textelement);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): textelement\n"); 
 for(i=0;i<cadlayers;i++) {
      my_free(&color_array[i]);
      my_free(&pixdata[i]);
      my_free(&rect[i]);
      my_free(&line[i]);
      my_free(&polygon[i]);
      my_free(&arc[i]);
 }
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): freeing instances\n");
 my_free(&inst_ptr);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): freeing selected group array\n");
  my_free(&selectedgroup);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): removing symbols\n");
 remove_symbols();
 for(i=0;i<max_symbols;i++) {
    my_free(&instdef[i].lineptr);
    my_free(&instdef[i].boxptr);
    my_free(&instdef[i].arcptr);
    my_free(&instdef[i].polygonptr);
    my_free(&instdef[i].lines);
    my_free(&instdef[i].polygons); /* 20171115 */
    my_free(&instdef[i].arcs); /* 20181012 */
    my_free(&instdef[i].rects);
 }
 my_free(&instdef);
 my_free(&rect);
 my_free(&line);
 my_free(&fill_type);
 my_free(&active_layer);
 my_free(&pixdata);
 my_free(&enable_layer);
 my_free(&lastrect);
 my_free(&polygon); /* 20171115 */
 my_free(&arc); /* 20171115 */
 my_free(&lastpolygon); /* 20171115 */
 my_free(&lastarc); /* 20171115 */
 my_free(&lastline);
 my_free(&max_rects);
 my_free(&max_polygons); /* 20171115 */
 my_free(&max_arcs); /* 20171115 */
 my_free(&max_lines);
 my_free(&pixmap);
 my_free(&gc);
 my_free(&gcstipple);
 my_free(&color_array);
 my_free(&tcl_command);

 for(i=0;i<CADMAXHIER;i++) my_free(&sch_prefix[i]);

 if(debug_var>=1) fprintf(errfp, "xwin_exit(): removing font\n");
 for(i=0;i<127;i++) my_free(&character[i]);

 if(debug_var>=1) fprintf(errfp, "xwin_exit(): closed display\n");
 my_free(&filename);
 
 delete_undo(); /* 20150327 */
 my_free(&netlist_dir);
 my_free(&xschem_executable);
 record_global_node(2, NULL, NULL); /* delete global node array */
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): deleted undo buffer\n");
 if(errfp!=stderr) fclose(errfp);
 errfp=stderr;
 printf("\n");
 init_done=0; /* 20150409 to avoid multiple calls */
}


int err(Display *display, XErrorEvent *xev)
{
 char s[1024];  /* overflow safe 20161122 */
 int l=250;
 XGetErrorText(display, xev->error_code, s,l);
 if(debug_var>=1) fprintf(errfp, "err(): Err %d :%s maj=%d min=%d\n", xev->error_code, s, xev->request_code,
          xev->minor_code);
 return 0;
}

unsigned int  find_best_color(char colorname[])
{
 int i;
 double distance=10000000000.0, dist, r, g, b, red, green, blue;
 double deltar,deltag,deltab;
 unsigned int index;

 if( XAllocNamedColor(display, colormap, colorname, &xcolor_exact, &xcolor) ==0 )
 {
  for(i=0;i<=255;i++) {
    xcolor_array[i].pixel=i;
    XQueryColor(display, colormap, xcolor_array+i);
  }
  /* debug ... */
  if(debug_var>=2) fprintf(errfp, 
        "find_best_color(): Server failed to allocate requested color, finding substitute\n");
  XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor);
  red = xcolor.red; green = xcolor.green; blue = xcolor.blue;
  index=0;
  for(i = 0;i<=255; i++)
  {
   r = xcolor_array[i].red ; g = xcolor_array[i].green ; b = xcolor_array[i].blue;
   deltar = (r - red);deltag = (g - green);deltab = (b - blue);
   dist = deltar*deltar + deltag*deltag + deltab*deltab;
   if( dist <= distance )
   {
    index = i;
    distance = dist;
   }
  }
 }
 else
 {
  /*XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor); */
  index = xcolor.pixel;
 }

 return index;
}


void init_color_array(double dim)
{
 char s[256]; /* overflow safe 20161122 */
 int i;
 unsigned int r, g, b; /* 20171123 */
 double rr, gg, bb; /* 20171123 */
 for(i=0;i<cadlayers;i++) {
   my_snprintf(s, S(s), "lindex $colors %d",i);
   tcleval(s);
   if(debug_var>=2) fprintf(errfp, "init_color_array(): color:%s\n",Tcl_GetStringResult(interp));

   sscanf(Tcl_GetStringResult(interp), "#%02x%02x%02x", &r, &g, &b);/* 20171123 */
   rr=r; gg=g; bb=b;
  
   if( (i!=BACKLAYER) ) {
     if(dim>=0.) {
       rr +=(51.-rr/5.)*dim;
       gg +=(51.-gg/5.)*dim;
       bb +=(51.-bb/5.)*dim;
     } else {
       rr +=(rr/5.)*dim;
       gg +=(gg/5.)*dim;
       bb +=(bb/5.)*dim;
     }
     /* fprintf(errfp, "init_color_array: colors: %.16g %.16g %.16g dim=%.16g c=%d\n", rr, gg, bb, dim, i); */
     r=rr;g=gg;b=bb;
     if(r>0xff) r=0xff;
     if(g>0xff) g=0xff;
     if(b>0xff) b=0xff;
   }
   my_snprintf(s, S(s), "#%02x%02x%02x", r, g, b);
   my_strdup(605, &color_array[i], s);
 }

}

void set_fill(int n) 
{
     XFreePixmap(display,pixmap[rectcolor]);
     pixmap[rectcolor] = XCreateBitmapFromData(display, window, (char*)(pixdata[n]),16,16);
     XSetStipple(display,gcstipple[rectcolor],pixmap[rectcolor]);
}

void init_pixdata() 
{
 int i,j, full, empty;
 for(i=0;i<cadlayers;i++) {
   full=1; empty=1;
   for(j=0;j<32;j++) {
     if(i<sizeof(pixdata_init)/sizeof(pixdata_init[0])) 
       pixdata[i][j] = pixdata_init[i][j];
     else 
       pixdata[i][j] = 0x00;
 
     if(pixdata[i][j]!=0xff) full=0;
     if(pixdata[i][j]!=0x00) empty=0;
   }
   if(full) fill_type[i] = 1;
   else if(empty) fill_type[i] = 0;
   else fill_type[i]=2;
   if(rainbow_colors && i>5) fill_type[i]=1; /* 20171212 solid fill style */
   /*fprintf(errfp, "fill_type[%d]= %d\n", i, fill_type[i]); */
 }
}

void alloc_data()
{
 int i;
 max_texts=CADMAXTEXT;
 max_wires=CADMAXWIRES;
 max_instances=ELEMINST;
 max_symbols=ELEMDEF;
 max_selected=MAXGROUP;
 textelement=my_calloc(606, max_texts,sizeof(Text));
 if(textelement==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 wire=my_calloc(607, max_wires,sizeof(Wire));
 if(wire==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 gridpoint=(XPoint*)my_calloc(608, CADMAXGRIDPOINTS,sizeof(XPoint));
 if(gridpoint==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 inst_ptr=my_calloc(609, max_instances , sizeof(Instance) );
 if(inst_ptr==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 instdef=my_calloc(610, max_symbols , sizeof(Instdef) );
 if(instdef==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }
 for(i=0;i<max_symbols;i++) {
   instdef[i].lineptr=my_calloc(611, cadlayers, sizeof(Line *));
   if(instdef[i].lineptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   instdef[i].polygonptr=my_calloc(612, cadlayers, sizeof(Polygon *));
   if(instdef[i].polygonptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   instdef[i].arcptr=my_calloc(613, cadlayers, sizeof(Arc *));
   if(instdef[i].arcptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   instdef[i].boxptr=my_calloc(614, cadlayers, sizeof(Box *));
   if(instdef[i].boxptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   instdef[i].lines=my_calloc(615, cadlayers, sizeof(int));
   if(instdef[i].lines==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }

   instdef[i].rects=my_calloc(616, cadlayers, sizeof(int));
   if(instdef[i].rects==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }
   instdef[i].arcs=my_calloc(617, cadlayers, sizeof(int));
   if(instdef[i].arcs==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }
   instdef[i].polygons=my_calloc(618, cadlayers, sizeof(int)); /* 20171115 */
   if(instdef[i].polygons==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }
 }

 selectedgroup=my_calloc(619, max_selected, sizeof(Selected));
 if(selectedgroup==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 max_rects=my_calloc(620, cadlayers, sizeof(int));
 if(max_rects==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 max_arcs=my_calloc(621, cadlayers, sizeof(int));
 if(max_arcs==NULL){
   fprintf(errfp, "Tcl_AppInit(): max_arcscalloc error\n");tcleval( "exit");
 }

 max_polygons=my_calloc(622, cadlayers, sizeof(int)); /* 20171115 */
 if(max_polygons==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 max_lines=my_calloc(623, cadlayers, sizeof(int));
 if(max_lines==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 for(i=0;i<cadlayers;i++)
 {
  max_rects[i]=CADMAXOBJECTS;
  max_polygons[i]=CADMAXOBJECTS; /* 20171115 */
  max_lines[i]=CADMAXOBJECTS;
  max_arcs[i]=CADMAXOBJECTS;
 }

 rect=my_calloc(624, cadlayers, sizeof(Box *));
 if(rect==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 line=my_calloc(625, cadlayers, sizeof(Line *));
 if(rect==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 polygon=my_calloc(626, cadlayers, sizeof(Polygon *));
 if(polygon==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 arc=my_calloc(627, cadlayers, sizeof(Arc *));
 if(arc==NULL){
   fprintf(errfp, "Tcl_AppInit(): arc calloc error\n");tcleval( "exit");
 }

 for(i=0;i<cadlayers;i++)
 {
  rect[i]=my_calloc(628, max_rects[i],sizeof(Box));
  if(rect[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  arc[i]=my_calloc(629, max_arcs[i],sizeof(Arc));
  if(arc[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): arc[] calloc error\n");tcleval( "exit");
  }

  polygon[i]=my_calloc(630, max_polygons[i],sizeof(Polygon));
  if(polygon[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }

  line[i]=my_calloc(631, max_lines[i],sizeof(Line));
  if(line[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
  }
 }

 lastrect=my_calloc(632, cadlayers, sizeof(int));
 if(lastrect==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 lastpolygon=my_calloc(633, cadlayers, sizeof(int)); /* 20171115 */
 if(lastpolygon==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 lastarc=my_calloc(634, cadlayers, sizeof(int)); /* 20171115 */
 if(lastarc==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 lastline=my_calloc(635, cadlayers, sizeof(int));
 if(lastline==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 pixmap=my_calloc(636, cadlayers, sizeof(Pixmap));
 if(pixmap==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 color_array=my_calloc(637, cadlayers, sizeof(char*));
 if(color_array==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 gc=my_calloc(638, cadlayers, sizeof(GC));
 if(gc==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 gcstipple=my_calloc(639, cadlayers, sizeof(GC));
 if(gcstipple==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 fill_type=my_calloc(640, cadlayers, sizeof(int));
 if(fill_type==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 active_layer=my_calloc(563, cadlayers, sizeof(int));
 if(active_layer==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 pixdata=my_calloc(641, cadlayers, sizeof(char*));
 if(pixdata==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
 }

 for(i=0;i<cadlayers;i++)
 {
   pixdata[i]=my_calloc(642, 32, sizeof(char));
   if(pixdata[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");tcleval( "exit");
   }
 }
 enable_layer=my_calloc(87, cadlayers, sizeof(int));
}


int build_colors(double dim) /* 20171113 */
{
    int i;
    if(dark_colorscheme) {
      tcleval("llength $dark_colors");
      if(atoi(Tcl_GetStringResult(interp))>=cadlayers){
        tcleval("set colors $dark_colors");
      }
    } else {
      tcleval("llength $light_colors");
      if(atoi(Tcl_GetStringResult(interp)) >=cadlayers){
        tcleval("set colors $light_colors");
      }
    }
    tcleval("llength $colors");
    if(atoi(Tcl_GetStringResult(interp))<cadlayers){
      fprintf(errfp,"Tcl var colors not set correctly\n");
      return -1; /* fail */
    }
    init_color_array(dim);
    for(i=0;i<cadlayers;i++)
    {
     color_index[i] = find_best_color(color_array[i]);
    }
    for(i=0;i<cadlayers;i++)
    {
      XSetForeground(display, gc[i], color_index[i]);
      XSetForeground(display, gcstipple[i], color_index[i]);
    }
    for(i=0;i<cadlayers;i++) {
      XLookupColor(display, colormap, color_array[i], &xcolor_exact, &xcolor);
      xcolor_array[i] = xcolor;
    }
    tcleval("reconfigure_layers_menu");
    return 0; /* success */
}


void tclexit(ClientData s)
{
  if(debug_var>=1) fprintf(errfp, "tclexit() INVOKED\n");
  if(init_done) xwin_exit();
}

#if HAS_XCB==1
/* from xcb.freedesktop.org -- don't ask me what it does... 20171125 */
static xcb_visualtype_t *find_visual(xcb_connection_t *xcbconn, xcb_visualid_t visual)
{
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(xcbconn));

    for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
        xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);
        for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
            xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
            for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
                if (visual == visual_iter.data->visual_id) {
                    return visual_iter.data;
                }
            }
        }
    }

    return NULL;
}
#endif /*HAS_XCB */

int source_tcl_file(char *s)
{
  char tmp[1024];
  if(Tcl_EvalFile(interp, s)==TCL_ERROR) { 
    fprintf(errfp, "Tcl_AppInit() error: can not execute %s, please fix:\n", s);
    fprintf(errfp, "%s", Tcl_GetStringResult(interp));
    fprintf(errfp, "\n");
    my_snprintf(tmp, S(tmp), "tk_messageBox -icon error -type ok -message \
       {Tcl_AppInit() err 1: can not execute %s, please fix:\n %s}",
       s, Tcl_GetStringResult(interp));
    if(has_x) {
      tcleval( "wm withdraw ."); /* 20161217 */
      tcleval( tmp); /* 20161217 */
      Tcl_Exit(EXIT_FAILURE);
    }
    return TCL_ERROR;
  }
  return TCL_OK;
}

void preview_window(const char *what, const char *tk_win_path, const char *filename)
{
  char *saveptr = NULL;
  char save_name[PATH_MAX];
  if(!strcmp(what, "create")) {
    tkpre_window = Tk_NameToWindow(interp, tk_win_path, mainwindow);
    Tk_MakeWindowExist(tkpre_window);
    pre_window = Tk_WindowId(tkpre_window);
  }
  else if(!strcmp(what, "draw")) {
    double xor, yor, z;
    int save_mod, save_ev;
 
    /* save context */
    xor = xorigin;
    yor = yorigin;
    z = zoom;
    save_window = window;
    save_mod = modified;
    save_ev = event_reporting;
    event_reporting = 0;
    my_strncpy(save_name, current_name, S(save_name));
    my_strdup(117, &saveptr, tclgetvar("current_dirname"));
    push_undo();
    my_strdup(114, &sch_prefix[currentsch+1], sch_prefix[currentsch]);
    my_strcat(115, &sch_prefix[currentsch+1], "___preview___");
    my_strcat(116, &sch_prefix[currentsch+1], ".");

    currentsch++;
    
    unselect_all();
    remove_symbols();

    /* preview */
    check_version = 0; /* if set refuse to load and preview anything if not an xschem file */
                       /* heuristics is done in xschem.tcl to ensure it is an xschem file */
    load_schematic(1,filename, 0);
    window = pre_window;
    resetwin();
    zoom_full(1, 0); /* draw */
    check_version = 0;
   
    /* restore context */
    tclsetvar("current_dirname", saveptr);
    my_free(&saveptr);
    unselect_all();
    remove_symbols();
    my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
    currentsch--;
    pop_undo(0);
    modified = save_mod;
    set_modify(modified);
    window = save_window;
    xorigin = xor;
    yorigin = yor;
    zoom = z;
    mooz = 1/z;
    resetwin();
    change_linewidth(-1.);
    my_strncpy(current_name, save_name, S(save_name));
    draw();
    event_reporting = save_ev;
  }
  else if(!strcmp(what, "destroy")) {
    Tk_DestroyWindow(tkpre_window);
  }
}

int Tcl_AppInit(Tcl_Interp *inter)
{
 char name[PATH_MAX]; /* overflow safe 20161122 */
 char tmp[2*PATH_MAX+100]; /* 20161122 overflow safe */
 const char *tmp_str_ptr;
 int i;
 struct stat buf;
 const char *home_buff;
 /* XVisualInfo vinfo; */

 #if HAS_XCB==1
 xcb_render_query_pict_formats_reply_t *formats_reply;
 xcb_render_pictforminfo_t *formats;
 xcb_render_query_pict_formats_cookie_t formats_cookie;
 #endif
 /* get PWD and HOME */
 if(!getcwd(pwd_dir, PATH_MAX)) {
   fprintf(errfp, "Tcl_AppInit(): getcwd() failed\n");
 }
 if ((home_buff = getenv("HOME")) == NULL) {
   home_buff = getpwuid(getuid())->pw_dir;
 }
 my_strncpy(home_dir, home_buff, S(home_dir));

 for(i=0;i<CADMAXHIER;i++) sch_prefix[i]=NULL;
 my_strdup(643, &sch_prefix[0],".");

 XSetErrorHandler(err);

 interp=inter;
 Tcl_Init(interp);
 if(has_x) Tk_Init(interp);
 if(!has_x)  tclsetvar("no_x","1");

 Tcl_CreateExitHandler(tclexit, 0);

 my_snprintf(tmp, S(tmp),"regsub -all {~/} {.:%s} {%s/}", XSCHEM_LIBRARY_PATH, home_dir);
 tcleval(tmp);
 tclsetvar("XSCHEM_LIBRARY_PATH", Tcl_GetStringResult(interp));
 
 if( !stat("./xschem.tcl", &buf) && !stat("./systemlib", &buf) && !stat("./xschem", &buf)) {
   tclsetvar("XSCHEM_SHAREDIR",pwd_dir); /* for testing xschem builds in src dir*/
   my_snprintf(tmp, S(tmp), "subst .:[file normalize \"%s/../xschem_library/devices\"]", pwd_dir);
   tcleval(tmp);
   tclsetvar("XSCHEM_LIBRARY_PATH", Tcl_GetStringResult(interp));
 } else if( !stat(XSCHEM_SHAREDIR, &buf) ) {  /* 20180918 */
   tclsetvar("XSCHEM_SHAREDIR",XSCHEM_SHAREDIR);
   /* ... else give up searching, may set later after loading xschemrc */
 }
 /* create user conf dir , remove ~ if present */
 my_snprintf(tmp, S(tmp),"regsub {^~/} {%s} {%s/}", USER_CONF_DIR, home_dir);
 tcleval(tmp);
 my_snprintf(user_conf_dir, S(user_conf_dir), "%s", Tcl_GetStringResult(interp));
 tclsetvar("USER_CONF_DIR", user_conf_dir);

 /* create USER_CONF_DIR if it was not installed */
 if(stat(user_conf_dir, &buf)) {
   if(!mkdir(user_conf_dir, 0700)) {
     if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): created %s dir\n", user_conf_dir);
   } else {
    fprintf(errfp, "Tcl_AppInit(): failure creating %s\n", user_conf_dir);
    Tcl_Exit(EXIT_FAILURE);
   }
 }
  
 /*                         */
 /*    SOURCE xschemrc file */
 /*                         */
 if(load_initfile) {
   if(rcfile[0]) {
     my_snprintf(name, S(name), rcfile);
     if(stat(name, &buf) ) {
       /* rcfile given on cmdline is not existing */
       fprintf(errfp, "Tcl_AppInit() err 2: cannot find %s\n", name);
       Tcl_ResetResult(interp);
       Tcl_Exit(EXIT_FAILURE);
       return TCL_ERROR; /* 20121110 */
     }
   } else { /* look in current dir */
     my_snprintf(name, S(name), "%s/xschemrc",pwd_dir);
   }
   /* source command-line given rcfile or xschemrc in present directory if existing ... */
   if(!stat(name, &buf)) {
     if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): sourcing %s\n", name);
     source_tcl_file(name);
   } else {
     /* ... else look for (user_conf_dir)/xschemrc ... */
     my_snprintf(name, S(name), "%s/xschemrc", user_conf_dir);
     if(!stat(name, &buf)) {
       if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): sourcing %s\n", name);
       source_tcl_file(name);
     } else if(tclgetvar("XSCHEM_SHAREDIR")) {
       /* ... else get systemwide xschemrc */
       my_snprintf(name, S(name), "%s/xschemrc",tclgetvar("XSCHEM_SHAREDIR"));
       if(!stat(name, &buf)) {
         if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): sourcing %s\n", name);
         source_tcl_file(name);
       }
     }
   }
 }
 /* END LOOKING FOR xschemrc */

 if(rainbow_colors) tclsetvar("rainbow_colors","1"); /* 20171013 */
 
 /*                               */
 /*  START LOOKING FOR xschem.tcl */
 /*                               */
 if(!tclgetvar("XSCHEM_SHAREDIR")) {
   fprintf(errfp, "Tcl_AppInit() err 3: cannot find xschem.tcl\n");
   if(has_x) {
     tcleval( "wm withdraw ."); /* 20161217 */
     tcleval(
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 3: xschem.tcl not found, "
       "you are probably missing XSCHEM_SHAREDIR\"");
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, "Tcl_AppInit() err 3: xschem.tcl not found, "
                            "you are probably missing XSCHEM_SHAREDIR",NULL);
   Tcl_Exit(EXIT_FAILURE);
   return TCL_ERROR; /* 20121110 */
 }
 /*  END LOOKING FOR xschem.tcl   */

 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): XSCHEM_SHAREDIR=%s  XSCHEM_LIBRARY_PATH=%s\n",
       tclgetvar("XSCHEM_SHAREDIR"), 
       tclgetvar("XSCHEM_LIBRARY_PATH") ? tclgetvar("XSCHEM_LIBRARY_PATH") : "NULL"
 );
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step a of xinit()\n");

 /*                                */
 /* CREATE XSCHEM 'xschem' COMMAND */
 /*                                */
 Tcl_CreateCommand(interp, "xschem",   (myproc *) xschem, NULL, NULL);

 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step a1 of xinit()\n");
 
 if(tcl_command) {
   tcleval(tcl_command);
 }
 
 /*                                */
 /*  EXECUTE xschem.tcl            */
 /*                                */
 my_snprintf(name, S(name), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "xschem.tcl");
 if(stat(name, &buf) ) {
   fprintf(errfp, "Tcl_AppInit() err 4: cannot find %s\n", name);
   if(has_x) {
     tcleval( "wm withdraw ."); /* 20161217 */
     tcleval(
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 4: xschem.tcl not found, "
         "installation problem or undefined  XSCHEM_SHAREDIR\"");
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, "Tcl_AppInit() err 4: xschem.tcl not found, "
                            "you are probably missing XSCHEM_SHAREDIR\n",NULL);
   Tcl_Exit(EXIT_FAILURE);
   return TCL_ERROR; /* 20121110 */
 }
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): sourcing %s\n", name);
 source_tcl_file(name);
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done executing xschem.tcl\n");
 /*  END EXECUTE xschem.tcl */

 /* resolve absolute pathname of xschem (argv[0]) for future usage */
 my_strdup(44, &xschem_executable, get_file_path(xschem_executable));
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): resolved xschem_executable=%s\n", xschem_executable);

 /* READ COLORS */
 my_snprintf(name, S(name), "%s/colors", user_conf_dir);
 if(!stat(name, &buf)) {
   source_tcl_file(name);
 }
   
 /* set global variables fetching data from tcl code 25122002 */
 if(tclgetvar("dark_colorscheme")[0] == '1') dark_colorscheme = 1; 
 else dark_colorscheme = 0;
 if(netlist_type==-1) {
  if(!strcmp(tclgetvar("netlist_type"),"vhdl") ) netlist_type=CAD_VHDL_NETLIST;
  else if(!strcmp(tclgetvar("netlist_type"),"verilog") ) netlist_type=CAD_VERILOG_NETLIST;
  else if(!strcmp(tclgetvar("netlist_type"),"tedax") ) netlist_type=CAD_TEDAX_NETLIST;
  else netlist_type=CAD_SPICE_NETLIST;
 } else {
  if(netlist_type==CAD_VHDL_NETLIST)  tclsetvar("netlist_type","vhdl");
  else if(netlist_type==CAD_VERILOG_NETLIST)  tclsetvar("netlist_type","verilog");
  else if(netlist_type==CAD_TEDAX_NETLIST)  tclsetvar("netlist_type","tedax");
  else tclsetvar("netlist_type","spice");
 }

 split_files=atoi(tclgetvar("split_files"));
 netlist_show=atoi(tclgetvar("netlist_show"));
 fullscreen=atoi(tclgetvar("fullscreen"));
 unzoom_nodrift=atoi(tclgetvar("unzoom_nodrift"));
 
 tmp_str_ptr = tclgetvar("bus_replacement_char"); 
 if(tmp_str_ptr && tmp_str_ptr[0] && tmp_str_ptr[1]) {
   bus_replacement_char[0] = tmp_str_ptr[0];
   bus_replacement_char[1] = tmp_str_ptr[1];
 }
 if(color_ps==-1) 
   color_ps=atoi(tclgetvar("color_ps"));
 else  {
   my_snprintf(tmp, S(tmp), "%d",color_ps);
   tclsetvar("color_ps",tmp);
 }
 change_lw=atoi(tclgetvar("change_lw"));
 draw_window=atoi(tclgetvar("draw_window"));
 incr_hilight=atoi(tclgetvar("incr_hilight"));
 if(a3page==-1) 
   a3page=atoi(tclgetvar("a3page"));
 else  {
   my_snprintf(tmp, S(tmp), "%d",a3page);
   tclsetvar("a3page",tmp);
 }
 enable_stretch=atoi(tclgetvar("enable_stretch"));
 draw_grid=atoi(tclgetvar("draw_grid"));
 cadlayers=atoi(tclgetvar("cadlayers"));
 if(debug_var==-10) debug_var=0;

 /*                             */
 /*  [m]allocate dynamic memory */
 /*                             */
 alloc_data();

 #ifndef IN_MEMORY_UNDO
 /* 20150327 create undo directory */
 /* 20180923 no more mkdtemp (portability issues) */
 if( !my_strdup(644, &undo_dirname, create_tmpdir("xschem_undo_") )) {
   fprintf(errfp, "xinit(): problems creating tmp undo dir\n");
   tcleval( "exit");
 }
 if(debug_var>=1) fprintf(errfp, "undo_dirname=%s\n", undo_dirname);
 #endif

 init_pixdata();
 init_color_array(0.0);
 my_snprintf(tmp, S(tmp), "%d",debug_var);
 tclsetvar("tcl_debug",tmp );
 if(flat_netlist) tclsetvar("flat_netlist","1");

 lw=1;
 xschem_w = CADWIDTH;
 xschem_h = CADHEIGHT;
 areaw = CADWIDTH+4*lw;  /* clip area extends 1 pixel beyond physical window area */
 areah = CADHEIGHT+4*lw; /* to avoid drawing clipped rectangle borders at window edges */
 areax1 = -2*lw;
 areay1 = -2*lw;
 areax2 = areaw-2*lw;
 areay2 = areah-2*lw;
 xrect[0].x = 0;
 xrect[0].y = 0;
 xrect[0].width = CADWIDTH;
 xrect[0].height = CADHEIGHT;


 my_strncpy(file_version, "1.1", S(file_version));
 compile_font();
 /* restore current dir after loading font */
 my_snprintf(tmp, S(tmp), "set current_dirname \"%s\"", pwd_dir);
 tcleval(tmp);

 /*                      */
 /*  X INITIALIZATION    */
 /*                      */
 if( has_x ) {
    mainwindow=Tk_MainWindow(interp);
    if(!mainwindow) {
       fprintf(errfp, "Tcl_AppInit() err 6: Tk_MainWindow returned NULL...\n");
       return TCL_ERROR;
    }
    display = Tk_Display(mainwindow);
    tkwindow = Tk_NameToWindow(interp, ".drw", mainwindow);
    Tk_MakeWindowExist(tkwindow);
    window = Tk_WindowId(tkwindow);
    topwindow = Tk_WindowId(mainwindow);

    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): drawing window ID=0x%lx\n",window);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): top window ID=0x%lx\n",topwindow);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done tkinit()\n");                  

    #if HAS_XCB==1
    /* grab an existing xlib connection  20171125 */
    xcbconn = XGetXCBConnection(display);
    if(xcb_connection_has_error(xcbconn)) {
      fprintf(errfp, "Could not connect to X11 server");
      return 1;
    }
    screen_xcb = xcb_setup_roots_iterator(xcb_get_setup(xcbconn)).data;
    visual_xcb = find_visual(xcbconn, screen_xcb->root_visual);
    if(!visual_xcb) {
      fprintf(errfp, "got NULL (xcb_visualtype_t)visual");
      return 1;
    }
    /*/--------------------------Xrender xcb  stuff------- */
    formats_cookie = xcb_render_query_pict_formats(xcbconn);
    formats_reply = xcb_render_query_pict_formats_reply(xcbconn, formats_cookie, 0);

    formats = xcb_render_query_pict_formats_formats(formats_reply);
    for (i = 0; i < formats_reply->num_formats; i++) {
             /* fprintf(errfp, "i=%d depth=%d  type=%d red_shift=%d\n", i, 
                  formats[i].depth, formats[i].type, formats[i].direct.red_shift); */
            if (formats[i].direct.red_mask != 0xff &&
                formats[i].direct.red_shift != 16)
                    continue;
            if (formats[i].type == XCB_RENDER_PICT_TYPE_DIRECT &&
                formats[i].depth == 24 && formats[i].direct.red_shift == 16)
                    format_rgb = formats[i];
            if (formats[i].type == XCB_RENDER_PICT_TYPE_DIRECT &&
                formats[i].depth == 32 &&
                formats[i].direct.alpha_mask == 0xff &&
                formats[i].direct.alpha_shift == 24)
                    format_rgba = formats[i];
    }
    my_free(&formats_reply);
    /*/---------------------------------------------------- */
    /* /20171125 */
    #endif /*HAS_XCB */
   
    screen_number = DefaultScreen(display);
    colormap = DefaultColormap(display, screen_number);
    depth = DisplayPlanes(display, screen_number);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): screen depth: %d\n",depth);

    visual = DefaultVisual(display, screen_number);

    /* 
    if (!XMatchVisualInfo(
        display, XDefaultScreen(display), 24, TrueColor, &vinfo)
    ) return fprintf(errfp, "no 32 bit visual\n");
    visual = vinfo.visual;
    */
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step b of xinit()\n");
    rectcolor= 4;  /* this is the current layer when xschem started. */
    for(i=0;i<cadlayers;i++)
    {
     pixmap[i] = XCreateBitmapFromData(display, window, (char*)(pixdata[i]),16,16);
     gc[i] = XCreateGC(display,window,0L,NULL);
     gcstipple[i] = XCreateGC(display,window,0L,NULL);
     XSetStipple(display,gcstipple[i],pixmap[i]);
     if(fill_type[i]==1)  XSetFillStyle(display,gcstipple[i],FillSolid);
     else XSetFillStyle(display,gcstipple[i],FillStippled);
    }
    gctiled = XCreateGC(display,window,0L, NULL);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step c of xinit()\n");
    if(build_colors(0.0)) exit(-1);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step e of xinit()\n");
    /* save_pixmap must be created as resetwin() frees it before recreating with new size. */
    save_pixmap = XCreatePixmap(display,window,CADWIDTH,CADHEIGHT,depth);
    XSetTile(display,gctiled,save_pixmap);
    XSetFillStyle(display,gctiled,FillTiled);
    #ifdef HAS_CAIRO /* 20171105 */
    {
      XWindowAttributes wattr;
      XGetWindowAttributes(display, window, &wattr);
      #if HAS_XRENDER==1
      #if HAS_XCB==1
      sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, window, &format_rgb, 1 , 1);
      save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, save_pixmap, &format_rgb, 1 , 1);
      #else
      format = XRenderFindStandardFormat(display, PictStandardRGB24);
      sfc = cairo_xlib_surface_create_with_xrender_format (display, window, DefaultScreenOfDisplay(display), format, 1, 1); 
      save_sfc = cairo_xlib_surface_create_with_xrender_format (display, save_pixmap, DefaultScreenOfDisplay(display), format, 1, 1); 
      #endif 
      #else
      sfc = cairo_xlib_surface_create(display, window, visual, wattr.width, wattr.height);
      save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, wattr.width, wattr.height);
      #endif
      if(cairo_surface_status(sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(errfp, "ERROR: invalid cairo surface\n");
        return 1;
      }
      if(cairo_surface_status(save_sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(errfp, "ERROR: invalid cairo surface\n");
        return 1;
      }
      ctx = cairo_create(sfc);
      save_ctx = cairo_create(save_sfc);

      #if 0
      {
        cairo_font_options_t *cfo;
        cfo = cairo_font_options_create ();
        cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_DEFAULT); /* CAIRO_ANTIALIAS_NONE */
        cairo_set_font_options (ctx, cfo);
        cairo_set_font_options (save_ctx, cfo);
      }
      #endif

      /* load font from tcl 20171112 */
      tcleval("xschem set cairo_font_name $cairo_font_name");
      tclsetvar("has_cairo","1");
      cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (ctx, 20);
      cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (save_ctx, 20);

      save_ctx = cairo_create(save_sfc);
      cairo_set_line_width(ctx, 1);
      cairo_set_line_width(save_ctx, 1);
      cairo_set_line_join(ctx, CAIRO_LINE_JOIN_ROUND);
      cairo_set_line_cap(ctx, CAIRO_LINE_CAP_ROUND);
      cairo_set_line_join(save_ctx, CAIRO_LINE_JOIN_ROUND);
      cairo_set_line_cap(save_ctx, CAIRO_LINE_CAP_ROUND);

    }
    #endif /* HAS_CAIRO */

    change_linewidth(0.);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done xinit()\n");
    winattr.backing_store = WhenMapped;
    /* winattr.backing_store = NotUseful;*/
    Tk_ChangeWindowAttributes(tkwindow, CWBackingStore, &winattr);
   
    if(debug_var>=1) 
       fprintf(errfp, "Tcl_AppInit(): sizeof Instance=%lu , sizeof Instdef=%lu\n",
             (unsigned long) sizeof(Instance),(unsigned long) sizeof(Instdef)); 
    
    /* 20121111 */
    tcleval("xschem line_width $line_width");
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): xserver max request size: %d\n", 
                             (int)XMaxRequestSize(display));

    set_snap(0); /* set default value specified in xschemrc as 'snap' else CADSNAP */
    set_grid(0); /* set default value specified in xschemrc as 'grid' else CADGRID */
 } /* if(has_x) */
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done X init\n");
 /*  END X INITIALIZATION */

 init_done=1;  /* 20171008 moved before option processing, otherwise xwin_exit will not be invoked */
               /* leaving undo buffer and other garbage around. */

 /*                                */
 /*  START PROCESSING USER OPTIONS */
 /*                                */
 if(event_reporting) {
   tcleval("set tcl_prompt1 {}");
   tcleval("set tcl_prompt2 {}");
 }


 if(!set_netlist_dir(0, NULL)) {
   fprintf(errfp, "problems creating netlist directory %s\n", netlist_dir ? netlist_dir : "<NULL>");
 }

 enable_layers();

 if(filename) {
    char s[PATH_MAX+100];
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): filename %s given, removing symbols\n", filename);
    remove_symbols();
    my_snprintf(s, S(s), "file normalize %s", filename);
    tcleval(s);
    load_schematic(1, abs_sym_path(Tcl_GetStringResult(interp), ""), 1); /* 20180925.1 */
 }
 else { 
   char * tmp; /* 20121110 */
   char filename[PATH_MAX];
   tmp = (char *) tclgetvar("XSCHEM_START_WINDOW"); /* 20121110 */
   if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): tmp=%s\n", tmp? tmp: "NULL");
   my_strncpy(filename, abs_sym_path(tmp, ""), S(filename));
   load_schematic(1, filename, 1);
 }
 zoom_full(0, 0);   /* Necessary to tell xschem the 
                  * initial area to display
                  */
 pending_fullzoom=1; /* 20121111 */
 if(do_netlist) {
   if(debug_var>=1) {
     if(flat_netlist) 
       fprintf(errfp, "xschem: flat netlist requested\n");
   }
   if(!filename) {
     fprintf(errfp, "xschem: cant do a netlist without a filename\n");
     tcleval( "exit");
   }
   if(netlist_dir && netlist_dir[0]) {
     if(netlist_type == CAD_SPICE_NETLIST)
       global_spice_netlist(1);                  /* 1 means global netlist */
     else if(netlist_type == CAD_VHDL_NETLIST)
       global_vhdl_netlist(1);                   /* 1 means global netlist */
     else if(netlist_type == CAD_VERILOG_NETLIST)
       global_verilog_netlist(1);                /* 1 means global netlist */
     else if(netlist_type == CAD_TEDAX_NETLIST)
       global_tedax_netlist(1);                  /* 1 means global netlist */
   } else {
    fprintf(errfp, "xschem: please set netlist_dir in xschemrc\n");
   }
 }
 if(do_print) {
   if(!filename) {
     fprintf(errfp, "xschem: can't do a print without a filename\n");
     tcleval( "exit");
   }
   if(do_print==1) ps_draw();
   else if(do_print == 2) print_image();
   else svg_draw();
 }

 if(do_simulation) {
   if(!filename) {
     fprintf(errfp, "xschem: can't do a simulation without a filename\n");
     tcleval( "exit");
   }
   tcleval( "simulate");
 }

 if(do_waves) {
   if(!filename) {
     fprintf(errfp, "xschem: can't show simulation waves without a filename\n");
     tcleval( "exit");
   }
   tcleval( "waves [file tail \"[xschem get schname]\"]");
 }

 if(quit) {
   tcleval( "exit");
 }

 /* */
 /*  END PROCESSING USER OPTIONS */
 /* */

 if(!no_readline) {
   tcleval( "if {![catch {package require tclreadline}]} "
      "{::tclreadline::readline customcompleter  completer; ::tclreadline::Loop }" ) ;
 }

 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): returning TCL_OK\n");
 return TCL_OK;
}



