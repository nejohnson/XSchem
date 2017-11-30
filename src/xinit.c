/* File: xinit.c
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

#include "xschem.h"

static int init_done=0; // 20150409 to avoid double call by Xwindows close and TclExitHandler

static XSetWindowAttributes winattr;
static int screen_number;
static Tk_Window  tkwindow, mainwindow;
static XWMHints *hints_ptr;
static Window topwindow;
static XColor xcolor_exact,xcolor;
typedef int myproc(
             ClientData clientData,
             Tcl_Interp *interp,
             int argc,
             const char *argv[]);

// -----------------------------------------------------------------------
// EWMH message handling routines 20071027... borrowed from wmctrl code
// -----------------------------------------------------------------------
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
        fprintf(stderr, "Cannot send %s event.\n", msg);
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


    my_strdup(&arg_copy, arg);

    if(debug_var>=1) fprintf(errfp,"window_state() , win=0x%x arg_copy=%s\n", 
          (int)win,arg_copy);
    if (!arg_copy || strlen(arg_copy) == 0) {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }

    if ((p1 = strchr(arg_copy, ','))) {
        static char tmp_prop1[1024], tmp1[1024]; // overflow safe 20161122
        
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
            fputs("Invalid action. Use either remove, add or toggle.\n", stderr);
            return EXIT_FAILURE;
        }
        p1++;

        /* the second property */
        if ((p2 = strchr(p1, ','))) {
            static char tmp_prop2[1024], tmp2[1024]; // overflow safe
            *p2 = '\0';
            p2++;
            if (strlen(p2) == 0) {
                fputs("Invalid zero length property.\n", stderr);
                return EXIT_FAILURE;
            }
            for( i = 0; p2[i]; i++) tmp2[i] = toupper( p2[i] );
            my_snprintf(tmp_prop2, S(tmp_prop2), "_NET_WM_STATE_%s", tmp2);
            prop2 = XInternAtom(disp, tmp_prop2, False);
        }

        /* the first property */
        if (strlen(p1) == 0) {
            fputs("Invalid zero length property.\n", stderr);
            return EXIT_FAILURE;
        }
        for( i = 0; p1[i]; i++) tmp1[i] = toupper( p1[i] );
        my_snprintf(tmp_prop1, S(tmp_prop1), "_NET_WM_STATE_%s", tmp1);
        prop1 = XInternAtom(disp, tmp_prop1, False);

        
        return client_msg(disp, win, "_NET_WM_STATE", 
            action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
    }
    else {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }
}/*}}}*/

// -----------------------------------------------------------------------

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
    Tcl_Eval(interp, "winfo id .");
    sscanf(Tcl_GetStringResult(interp), "0x%x", (unsigned int *) &ww);
    framewin = ww;
    XQueryTree(display, framewin, &rootwindow, &parent_of_topwindow, &framewin_child_ptr, &framweindow_nchildren);
    if(debug_var>=1) fprintf(errfp,"framewinID=%x\n", (unsigned int) framewin);
    if(debug_var>=1) fprintf(errfp,"framewin nchilds=%d\n", (unsigned int) framweindow_nchildren);
    if(debug_var>=1) fprintf(errfp,"framewin parentID=%x\n", (unsigned int) parent_of_topwindow);
    if(debug_var>=1) fprintf(errfp,"framewin child 0=%x\n", (unsigned int) framewin_child_ptr[0]);

    // here I create the icon pixmap,to be used when iconified, 
    // I will use it when I know how to use it as icon :-(
    // removed icon, ts created by tcl  31102004
    if(!cad_icon_pixmap) {
      i=XpmCreatePixmapFromData(display,framewin, cad_icon,&cad_icon_pixmap, NULL, NULL);
      if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): creating icon pixmap returned: %d\n",i);
      //this does not work (sending icon pixmap hint)
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
   return;  // 20150409
 }
 if(has_x) {
    #ifdef HAS_CAIRO // 20171105
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
 my_free(wire);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): wire\n"); 
 my_free(gridpoint);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): gridpoint\n"); 
 my_free(textelement);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): textelement\n"); 
 for(i=0;i<cadlayers;i++) {
      my_free(color_array[i]);
      my_free(pixdata[i]);
      my_free(rect[i]);
      my_free(line[i]);
 }
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): freeing instances\n");
 my_free(inst_ptr);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): freeing selected group array\n");
  my_free(selectedgroup);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): removing symbols\n");
 remove_symbols();
 for(i=0;i<max_symbols;i++) {
    my_free(instdef[i].lineptr);
    my_free(instdef[i].boxptr);
    my_free(instdef[i].lines);
    my_free(instdef[i].polygons); // 20171115
    my_free(instdef[i].rects);
 }
 my_free(instdef);
 my_free(rect);
 my_free(line);
 my_free(fill_type);
 my_free(pixdata);
 my_free(lastrect);
 my_free(polygon); // 20171115
 my_free(lastpolygon); // 20171115
 my_free(lastline);
 my_free(max_rects);
 my_free(max_polygons); // 20171115
 my_free(max_lines);
 my_free(pixmap);
 my_free(gc);
 my_free(gcstipple);
 my_free(color_array);
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): removing font\n");
 for(i=0;i<127;i++) my_free(character[i]);

 if(debug_var>=1) fprintf(errfp, "xwin_exit(): closed display\n");
 my_free(filename);
 delete_undo(); // 20150327
 if(debug_var>=1) fprintf(errfp, "xwin_exit(): deleted undo buffer\n");
 if(errfp!=stderr) fclose(errfp);
 errfp=stderr;
 printf("\n");
 init_done=0; // 20150409 to avoid multiple calls
}


int err(Display *display, XErrorEvent *xev)
{
 char s[1024];  // overflow safe 20161122
 int l=250;
 XGetErrorText(display, xev->error_code, s,l);
 if(debug_var>=1) fprintf(errfp, "err(): Err %d :%s maj=%d min=%d\n", xev->error_code, s, xev->request_code,
          xev->minor_code);
 return 0;
}

unsigned int  find_best_color(char colorname[])
{
 int i;
 static int color_flag=0;
 double distance=10000000000.0, dist, r, g, b, red, green, blue;
 double deltar,deltag,deltab;
 unsigned int index;

 if( XAllocNamedColor(display, colormap, colorname, &xcolor_exact, &xcolor) ==0 )
 {

  if(!color_flag)
  {
   for(i=0;i<=255;i++)
   {
    xcolor_array[i].pixel=i;
    XQueryColor(display, colormap, xcolor_array+i);
    color_flag=1;
   }
  }
  // debug ...
  if(debug_var>=1) fprintf(errfp, 
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
  //XLookupColor(display, colormap, colorname, &xcolor_exact, &xcolor);
  index = xcolor.pixel;
 }

 return index;
}


void init_color_array(int skip_background, double dim)
{
 char s[256]; // overflow safe 20161122
 int i;
 int r, g, b; // 20171123
 double rr, gg, bb; // 20171123
 for(i=0;i<cadlayers;i++) {
   my_snprintf(s, S(s), "lindex $colors %d",i);
   Tcl_EvalEx(interp, s, -1, TCL_EVAL_GLOBAL);
   if(debug_var>=1) fprintf(errfp, "init_color_array(): color:%s\n",Tcl_GetStringResult(interp));

   sscanf(Tcl_GetStringResult(interp), "#%02x%02x%02x", &r, &g, &b);// 20171123
   rr=r; gg=g; bb=b;
  
   if(1|| !(i==0 && skip_background) ) {
     if(dim>=0.) {
       rr +=(51.-rr/5.)*dim;
       gg +=(51.-gg/5.)*dim;
       bb +=(51.-bb/5.)*dim;
     } else {
       rr +=(rr/5.)*dim;
       gg +=(gg/5.)*dim;
       bb +=(bb/5.)*dim;
     }
     // fprintf(errfp, "init_color_array: colors: %.16g %.16g %.16g dim=%.16g c=%d\n", rr, gg, bb, dim, i);
     r=rr;g=gg;b=bb;
     if(r>0xff) r=0xff;
     if(g>0xff) g=0xff;
     if(b>0xff) b=0xff;
   }
   my_snprintf(s, S(s), "#%02x%02x%02x", r, g, b);
   my_strdup(&color_array[i], s);
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
   //fprintf(errfp, "fill_type[%d]= %d\n", i, fill_type[i]);
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
 textelement=my_calloc(max_texts,sizeof(Text));
 if(textelement==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 wire=my_calloc(max_wires,sizeof(Wire));
 if(wire==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 gridpoint=(XPoint*)my_calloc(CADMAXGRIDPOINTS,sizeof(XPoint));
 if(gridpoint==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 inst_ptr=my_calloc(max_instances , sizeof(Instance) );
 if(inst_ptr==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 instdef=my_calloc(max_symbols , sizeof(Instdef) );
 if(instdef==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }
 for(i=0;i<max_symbols;i++) {
   instdef[i].lineptr=my_calloc(cadlayers, sizeof(Line *));
   if(instdef[i].lineptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }

   instdef[i].polygonptr=my_calloc(cadlayers, sizeof(Polygon *));
   if(instdef[i].polygonptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }

   instdef[i].boxptr=my_calloc(cadlayers, sizeof(Line *));
   if(instdef[i].boxptr==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }

   instdef[i].lines=my_calloc(cadlayers, sizeof(int));
   if(instdef[i].lines==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }

   instdef[i].rects=my_calloc(cadlayers, sizeof(int));
   if(instdef[i].rects==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }
   instdef[i].polygons=my_calloc(cadlayers, sizeof(int)); // 20171115
   if(instdef[i].polygons==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }
 }

 selectedgroup=my_calloc(max_selected, sizeof(Selected));
 if(selectedgroup==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 max_rects=my_calloc(cadlayers, sizeof(int));
 if(max_rects==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 max_polygons=my_calloc(cadlayers, sizeof(int)); // 20171115
 if(max_polygons==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 max_lines=my_calloc(cadlayers, sizeof(int));
 if(max_lines==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 for(i=0;i<cadlayers;i++)
 {
  max_rects[i]=CADMAXOBJECTS;
  max_polygons[i]=CADMAXOBJECTS; // 20171115
  max_lines[i]=CADMAXOBJECTS;
 }

 rect=my_calloc(cadlayers, sizeof(Box *));
 if(rect==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 line=my_calloc(cadlayers, sizeof(Line *));
 if(rect==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 polygon=my_calloc(cadlayers, sizeof(Polygon *));
 if(polygon==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }


 for(i=0;i<cadlayers;i++)
 {
  rect[i]=my_calloc(max_rects[i],sizeof(Box));
  if(rect[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
  }

  polygon[i]=my_calloc(max_polygons[i],sizeof(Polygon));
  if(polygon[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
  }

  line[i]=my_calloc(max_lines[i],sizeof(Line));
  if(line[i]==NULL){
    fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
  }
 }

 lastrect=my_calloc(cadlayers, sizeof(int));
 if(lastrect==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 lastpolygon=my_calloc(cadlayers, sizeof(int)); // 20171115
 if(lastpolygon==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 lastline=my_calloc(cadlayers, sizeof(int));
 if(lastline==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 pixmap=my_calloc(cadlayers, sizeof(Pixmap));
 if(pixmap==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 color_array=my_calloc(cadlayers, sizeof(char*));
 if(color_array==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 gc=my_calloc(cadlayers, sizeof(GC));
 if(gc==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 gcstipple=my_calloc(cadlayers, sizeof(GC));
 if(gcstipple==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 fill_type=my_calloc(cadlayers, sizeof(int));
 if(fill_type==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 pixdata=my_calloc(cadlayers, sizeof(char*));
 if(pixdata==NULL){
   fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
 }

 for(i=0;i<cadlayers;i++)
 {
   pixdata[i]=my_calloc(32, sizeof(char));
   if(pixdata[i]==NULL){
     fprintf(errfp, "Tcl_AppInit(): calloc error\n");Tcl_Eval(interp, "exit");
   }
 }
}


int build_colors(int skip_background, double dim) // 20171113
{
    int i;
    if(dark_colorscheme) {
      Tcl_EvalEx(interp, "llength $dark_colors", -1, TCL_EVAL_GLOBAL);
      if(atoi(Tcl_GetStringResult(interp))>=cadlayers){
        Tcl_EvalEx(interp, "set colors $dark_colors", -1, TCL_EVAL_GLOBAL);
      }
    } else {
      Tcl_EvalEx(interp, "llength $light_colors", -1, TCL_EVAL_GLOBAL);
      if(atoi(Tcl_GetStringResult(interp)) >=cadlayers){
        Tcl_EvalEx(interp, "set colors $light_colors", -1, TCL_EVAL_GLOBAL);
      }
    }
    Tcl_EvalEx(interp, "llength $colors", -1, TCL_EVAL_GLOBAL);
    if(atoi(Tcl_GetStringResult(interp))<cadlayers){
      fprintf(errfp,"Tcl var colors not set correctly\n");
      return -1; // fail
    }
    init_color_array(skip_background, dim);
    for(i=0;i<cadlayers;i++)
    {
     color_index[i] = find_best_color(color_array[i]);
    }
    for(i=0;i<cadlayers;i++)
    {
      if(!(i==0 && skip_background) ) {
        XSetForeground(display, gc[i], color_index[i]);
        XSetForeground(display, gcstipple[i], color_index[i]);
      }
    }
    for(i=0;i<cadlayers;i++) {
      XLookupColor(display, colormap, color_array[i], &xcolor_exact, &xcolor);
      xcolor_array[i] = xcolor;
    }
    Tcl_EvalEx(interp, "reconfigure_layers_menu", -1, TCL_EVAL_GLOBAL);
    if(!skip_background) {
      XSetWindowBackground(display, window, color_index[0]); // 20171124
    }
    return 0; // success
}


void tclexit(ClientData s)
{
  if(debug_var>=1) fprintf(errfp, "tclexit() INVOKED\n");
  xwin_exit();
}

#if HAS_XCB==1
// from xcb.freedesktop.org -- don't ask me what it does... 20171125
static xcb_visualtype_t *find_visual(xcb_connection_t *xcbconn, xcb_visualid_t visual)
{
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(xcbconn));

    printf("\n");
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
#endif //HAS_XCB

int Tcl_AppInit(Tcl_Interp *inter)
{
 static char *name=NULL; // overflow safe 20161122
 char tmp[1024]; // 20161122 overflow safe
 int i;
 int initfile_found; // 20170330
 struct stat buf;

 #if HAS_XCB==1
 xcb_render_query_pict_formats_reply_t *formats_reply;
 xcb_render_pictforminfo_t *formats;
 xcb_render_query_pict_formats_cookie_t formats_cookie;
 #endif

 if(!getenv("DISPLAY")) has_x=0;
 if(debug_var>=1 && !has_x) fprintf(errfp, "Tcl_AppInit(): no DISPLAY environment var, assuming no X available\n");
 for(i=0;i<CADMAXHIER;i++) sch_prefix[i]=NULL;
 my_strdup(&sch_prefix[0],".");

 XSetErrorHandler(err);

 initfile_found=0; // 20170330
 interp=inter;
 Tcl_Init(interp);
 if(has_x) Tk_Init(interp);
 if(!has_x)  Tcl_SetVar(interp, "no_x", "1", TCL_GLOBAL_ONLY);

 Tcl_CreateExitHandler(tclexit, 0);

 if( getenv("PWD") ) { // 20161204
   my_strdup(&name, getenv("PWD") );
   my_strcat(&name, "/.xschem");
   if(!stat(name, &buf)) initfile_found=1; // 20170330
 }
 if(!getenv("HOME")) {
   fprintf(errfp, "Tcl_AppInit() err 2: HOME env var not set\n");
   if(has_x) {
     Tcl_Eval(interp,
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 2: HOME env variable not set, please fix it\"");
   }
   return TCL_ERROR;
 } else if( !initfile_found ) {
   my_strdup(&name, getenv("HOME") );
   my_strcat(&name, "/.xschem");
   if(!stat(name, &buf)) initfile_found=1; // 20170330
 }

 if(initfile_found) {  // file exists 20121110 // used initfile_found, 20170330
   if(Tcl_EvalFile(interp, name)==TCL_ERROR) { // source ~/.xschem 20121110
     fprintf(errfp, "Tcl_AppInit() err 1: can not execute .xschem, please fix:\n");
     fprintf(errfp, Tcl_GetStringResult(interp));
     fprintf(errfp, "\n");
     my_snprintf(tmp, S(tmp), "tk_messageBox -icon error -type ok -message \
        {Tcl_AppInit() err 1: can not execute .xschem, please fix:\n %s}",
        Tcl_GetStringResult(interp));
     if(has_x) {
       Tcl_Eval(interp, "wm withdraw ."); // 20161217
       Tcl_Eval(interp, tmp); // 20161217
       Tcl_Eval(interp, "exit");
     }
     return TCL_ERROR;
   }
 }

 if(rainbow_colors) Tcl_SetVar(interp, "rainbow_colors", "1", TCL_GLOBAL_ONLY); // 20171013
 

 if(Tcl_GetVar(interp, "XSCHEM_HOME_DIR",TCL_GLOBAL_ONLY)) {
 } else if(getenv("XSCHEM_HOME_DIR")) { // 20121111
   Tcl_SetVar(interp, "XSCHEM_HOME_DIR", getenv("XSCHEM_HOME_DIR"), TCL_GLOBAL_ONLY);
 } else if( !stat("/opt/xschem", &buf) ) { 
   Tcl_SetVar(interp, "XSCHEM_HOME_DIR", "/opt/xschem", TCL_GLOBAL_ONLY);
 } else if( !stat("/usr/share/xschem", &buf) ) { 
   Tcl_SetVar(interp, "XSCHEM_HOME_DIR", "/usr/share/xschem", TCL_GLOBAL_ONLY);
 } else {			// if XSCHEM_HOME_DIR is not defined we must guve up... 20121110
   fprintf(errfp, "Tcl_AppInit() err 3: cannot execute xschem.tcl\n");
   if(has_x) {
     Tcl_Eval(interp,
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 3: xschem.tcl not found, \
         you are probably missing XSCHEM_HOME_DIR\"");
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, "Tcl_AppInit() err 3: xschem.tcl not found, you are probably missing XSCHEM_HOME_DIR",NULL);
   return TCL_ERROR; // 20121110
 }
 if(getenv("XSCHEM_DESIGN_DIR") != NULL) {   // great cleanup done here 20121110
   Tcl_SetVar(interp, "XSCHEM_DESIGN_DIR", getenv("XSCHEM_DESIGN_DIR"), TCL_GLOBAL_ONLY);
 }

 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): XSCHEM_HOME_DIR=%s  XSCHEM_DESIGN_DIR=%s\n",
       Tcl_GetVar(interp, "XSCHEM_HOME_DIR", TCL_GLOBAL_ONLY), 
       Tcl_GetVar(interp, "XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY) 
 );
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step a of xinit()\n");
 Tcl_CreateCommand(interp, "xschem",   (myproc *) xschem, NULL, NULL);
 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step a1 of xinit()\n");

 my_strdup(&name,Tcl_GetVar(interp, "XSCHEM_HOME_DIR", TCL_GLOBAL_ONLY));
 my_strcat(&name,"/xschem.tcl");

 if(stat(name, &buf) ) {
   fprintf(errfp, "Tcl_AppInit() err 4: cannot find %s\n", name);
   if(has_x) {
     Tcl_Eval(interp,
       "tk_messageBox -icon error -type ok -message \"Tcl_AppInit() err 4: xschem.tcl not found, installation problem or undefined  XSCHEM_HOME_DIR\"");
   }
   Tcl_ResetResult(interp);
   Tcl_AppendResult(interp, "Tcl_AppInit() err 4: xschem.tcl not found, you are probably missing XSCHEM_HOME_DIR\n",NULL);
   return TCL_ERROR; // 20121110
 }
 

 if(Tcl_EvalFile(interp, name)==TCL_ERROR) {
     fprintf(errfp, "Tcl_AppInit() err 5: cannot execute %s, probably due to a syntax error\n", name);
     fprintf(errfp, "\n%s\n", Tcl_GetStringResult(interp));
     if(has_x) {
       my_snprintf(tmp, S(tmp), "tk_messageBox -icon error -type ok -message \
          {Tcl_AppInit() err 5: can not execute xschem.tcl, please fix:\n %s}",
          Tcl_GetStringResult(interp));
       Tcl_Eval(interp, "wm withdraw ."); // 20161217
       Tcl_Eval(interp, tmp); // 20161217
       Tcl_Eval(interp, "exit");
     }
     Tcl_AppendResult(interp, "\nTcl_AppInit() err 5: can not execute xschem.tcl, probaby due to a syntax error\n",NULL);
     return TCL_ERROR; // 20121110
 }

// set global variables fetching data from tcl code 25122002
 if(netlist_type==-1) {
  if(!strcmp(Tcl_GetVar(interp, "netlist_type", TCL_GLOBAL_ONLY),"vhdl") ) netlist_type=CAD_VHDL_NETLIST;
  else if(!strcmp(Tcl_GetVar(interp, "netlist_type", TCL_GLOBAL_ONLY),"verilog") ) netlist_type=CAD_VERILOG_NETLIST;
  else netlist_type=CAD_SPICE_NETLIST;
 } else {
  if(netlist_type==CAD_VHDL_NETLIST)  Tcl_SetVar(interp,"netlist_type","vhdl",TCL_GLOBAL_ONLY);
  else if(netlist_type==CAD_VERILOG_NETLIST)  Tcl_SetVar(interp,"netlist_type","verilog",TCL_GLOBAL_ONLY);
  else Tcl_SetVar(interp,"netlist_type","spice",TCL_GLOBAL_ONLY);
 }

 split_files=atoi(Tcl_GetVar(interp, "split_files", TCL_GLOBAL_ONLY));
 hspice_netlist=atoi(Tcl_GetVar(interp, "hspice_netlist", TCL_GLOBAL_ONLY));
 netlist_show=atoi(Tcl_GetVar(interp, "netlist_show", TCL_GLOBAL_ONLY));
 fullscreen=atoi(Tcl_GetVar(interp, "fullscreen", TCL_GLOBAL_ONLY));
 if(color_ps==-1) 
   color_ps=atoi(Tcl_GetVar(interp, "color_ps", TCL_GLOBAL_ONLY));
 else  {
   my_snprintf(tmp, S(tmp), "%d",color_ps);
   Tcl_SetVar(interp,"color_ps",tmp,TCL_GLOBAL_ONLY);
 }
 change_lw=atoi(Tcl_GetVar(interp, "change_lw", TCL_GLOBAL_ONLY));
 incr_hilight=atoi(Tcl_GetVar(interp, "incr_hilight", TCL_GLOBAL_ONLY));
 if(a3page==-1) 
   a3page=atoi(Tcl_GetVar(interp, "a3page", TCL_GLOBAL_ONLY));
 else  {
   my_snprintf(tmp, S(tmp), "%d",a3page);
   Tcl_SetVar(interp,"a3page",tmp,TCL_GLOBAL_ONLY);
 }
 enable_stretch=atoi(Tcl_GetVar(interp, "enable_stretch", TCL_GLOBAL_ONLY));
 draw_grid=atoi(Tcl_GetVar(interp, "draw_grid", TCL_GLOBAL_ONLY));
 cadlayers=atoi(Tcl_GetVar(interp, "cadlayers", TCL_GLOBAL_ONLY));
 if(debug_var==-10) debug_var=0;

// [m]allocate dynamic memory
 alloc_data();

 // 20150327 create undo directory
 my_strdup(&undo_dirname, getenv("HOME"));
 my_strcat(&undo_dirname, "/xschem_undo_XXXXXX");

 if( !mkdtemp(undo_dirname) ) {
   if(debug_var>=1) fprintf(errfp, "xinit(): problems creating tmp undo dir\n");
   Tcl_Eval(interp, "exit");
 }

 init_pixdata();
 init_color_array(0, 0.0);
 my_snprintf(tmp, S(tmp), "%d",debug_var);
 Tcl_SetVar(interp,"tcl_debug",tmp ,TCL_GLOBAL_ONLY);
 if(flat_netlist) Tcl_SetVar(interp,"flat_netlist","1",TCL_GLOBAL_ONLY);

 lw=1;
 areaw = CADWIDTH+4*lw;  // clip area extends 1 pixel beyond physical window area
 areah = CADHEIGHT+4*lw; // to avoid drawing clipped rectangle borders at window edges
 areax1 = -2*lw;
 areay1 = -2*lw;
 areax2 = areaw-2*lw;
 areay2 = areah-2*lw;
 xrect[0].x = 0;
 xrect[0].y = 0;
 xrect[0].width = CADWIDTH;
 xrect[0].height = CADHEIGHT;

 compile_font();

//  ************ X INITIALIZATION *******************
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
    // grab an existing xlib connection  20171125
    xcbconn = XGetXCBConnection(display);
    if(xcb_connection_has_error(xcbconn)) {
      fprintf(stderr, "Could not connect to X11 server");
      return 1;
    }
    screen_xcb = xcb_setup_roots_iterator(xcb_get_setup(xcbconn)).data;
    visual_xcb = find_visual(xcbconn, screen_xcb->root_visual);
    if(!visual_xcb) {
      fprintf(stderr, "got NULL (xcb_visualtype_t)visual");
      return 1;
    }
    ///--------------------------Xrender xcb  stuff-------
    formats_cookie = xcb_render_query_pict_formats(xcbconn);
    formats_reply = xcb_render_query_pict_formats_reply(xcbconn, formats_cookie, 0);

    formats = xcb_render_query_pict_formats_formats(formats_reply);
    for (i = 0; i < formats_reply->num_formats; i++) {
            // fprintf(errfp, "i=%d depth=%d  type=%d red_shift=%d\n", i, 
            //      formats[i].depth, formats[i].type, formats[i].direct.red_shift);
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
    free(formats_reply);
    ///----------------------------------------------------
    // /20171125
    #endif //HAS_XCB
   
    screen_number = DefaultScreen(display);
    colormap = DefaultColormap(display, screen_number);
    depth = DisplayPlanes(display, screen_number);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): screen depth: %d\n",depth);
  
    visual = DefaultVisual(display, screen_number);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step b of xinit()\n");
    rectcolor= 4;  // this is the current layer when xschem started.
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
    if(build_colors(0, 0.0)) exit(-1);
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done step e of xinit()\n");
    //// save_pixmap must be created as resetwin() frees it before recreating with new size.
    save_pixmap = XCreatePixmap(display,window,CADWIDTH,CADHEIGHT,depth);
    XSetTile(display,gctiled,save_pixmap);
    XSetFillStyle(display,gctiled,FillTiled);
    #ifdef HAS_CAIRO // 20171105
    {
      XWindowAttributes wattr;
      XGetWindowAttributes(display, window, &wattr);

      sfc = cairo_xlib_surface_create(display, window, visual, wattr.width, wattr.height);
      save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, wattr.width, wattr.height);

      #if HAS_XCB==1 && HAS_XRENDER==1
      cairo_surface_destroy(sfc);
      cairo_surface_destroy(save_sfc);
      sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, window, &format_rgb, 1 , 1);
      save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, save_pixmap, &format_rgb, 1 , 1);
      #elif HAS_XRENDER==1
      // format = cairo_xlib_surface_get_xrender_format (sfc);
      format = XRenderFindStandardFormat(display, PictStandardRGB24);
      cairo_surface_destroy(sfc);
      sfc = cairo_xlib_surface_create_with_xrender_format (display, window, DefaultScreenOfDisplay(display), format, 1, 1); 
      // format = cairo_xlib_surface_get_xrender_format (save_sfc);
      cairo_surface_destroy(save_sfc);
      save_sfc = cairo_xlib_surface_create_with_xrender_format (display, save_pixmap, DefaultScreenOfDisplay(display), format, 1, 1); 
      #endif //HAS_XRENDER
  

      if(cairo_surface_status(sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "ERROR: invalid cairo surface\n");
        return 1;
      }
      if(cairo_surface_status(save_sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "ERROR: invalid cairo surface\n");
        return 1;
      }
      ctx = cairo_create(sfc);
      save_ctx = cairo_create(save_sfc);

      // load font from tcl 20171112
      Tcl_EvalEx(interp,"xschem set cairo_font_name $cairo_font_name", -1, TCL_EVAL_GLOBAL);
      Tcl_SetVar(interp, "has_cairo", "1", TCL_GLOBAL_ONLY);
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
    #endif

    set_linewidth();
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): done xinit()\n");
    // winattr.backing_store = WhenMapped; //<<<
    //winattr.backing_store = NotUseful;
    Tk_ChangeWindowAttributes(tkwindow, CWBackingStore, &winattr);
   
    if(debug_var>=1) 
       fprintf(errfp, "Tcl_AppInit(): sizeof Instance=%lu , sizeof Instdef=%lu\n",
             (unsigned long) sizeof(Instance),(unsigned long) sizeof(Instdef)); 
    
    // 20121111
    Tcl_Eval(interp,"xschem line_width $line_width");
    if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): xserver max request size: %d\n", 
                             (int)XMaxRequestSize(display));


 } // if(has_x)
 x_initialized=1;
//  ************ END X INITIALIZATION *******************


// we look here at user options, and act accordingly before going to interactive mode
 init_done=1;  // 20171008 moved before option processing, otherwise xwin_exit will not be invoked
               // leaving undo buffer and other garbage around.


 if(filename) {
    char str[1024];  // 20161122 overflow safe
    my_snprintf(str, S(str), "get_cell {%s}", filename);
    tkeval(str);
    strcpy(schematic[currentsch], Tcl_GetStringResult(interp));
    remove_symbols();
    if(strstr(filename,".sym")) load_symbol( NULL);
    else load_schematic(1, NULL,1);
 }
 else { 
   char * tmp; // 20121110
   tmp = (char *) Tcl_GetVar(interp, "XSCHEM_START_WINDOW", TCL_GLOBAL_ONLY); // 20121110
   if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): tmp=%s\n", tmp);

   if(tmp && tmp[0]) strcpy(schematic[currentsch],tmp); // 20070323
   load_schematic(1, NULL,1);					// 20121110
 }
 zoom_full(/*no draw */ 0);				// Necessary to tell xschem the 
							// initial area to display
 pending_fullzoom=1; // 20121111
 if(do_netlist) {
  if(debug_var>=1) {
   if(flat_netlist) 
     fprintf(errfp, "xschem: flat netlist requested\n");
  }
  if(!filename) {
    fprintf(errfp, "xschem: cant do a netlist without a filename\n");
    Tcl_Eval(interp, "exit");
  }
  if(netlist_type == CAD_SPICE_NETLIST)
    global_spice_netlist(1);                  // 1 means global netlist
  else if(netlist_type == CAD_VHDL_NETLIST)
    global_vhdl_netlist(1);                   // 1 means global netlist
  else if(netlist_type == CAD_VERILOG_NETLIST)
    global_verilog_netlist(1);                   // 1 means global netlist
 }
 if(do_print) {
   if(!filename) {
     fprintf(errfp, "xschem: can't do a print without a filename\n");
     Tcl_Eval(interp, "exit");
   }
   ps_draw();
 }

 if(do_simulation) {
   if(!filename) {
     fprintf(errfp, "xschem: can't do a simulation without a filename\n");
     Tcl_Eval(interp, "exit");
   }
   Tcl_Eval(interp, "xschem simulate");
 }

 if(do_waves) {
   if(!filename) {
     fprintf(errfp, "xschem: can't show simulation waves without a filename\n");
     Tcl_Eval(interp, "exit");
   }
   Tcl_Eval(interp, "waves [file tail [xschem get schname]]");
 }

 if(quit) {
   Tcl_Eval(interp, "exit");
 }

// end processing user options

 if(!no_readline) {
   Tcl_Eval(interp, " package require tclreadline ; ::tclreadline::Loop " ) ;
 }

 if(debug_var>=1) fprintf(errfp, "Tcl_AppInit(): returning TCL_OK\n");

 return TCL_OK;
}



