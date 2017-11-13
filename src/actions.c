/* File: actions.c
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

void set_snap(double newsnap) // 20161212 set new snap factor and just notify new value
{
    char str[256];
    cadsnap = newsnap;
    my_snprintf(str, S(str), "alert_ {snap: %g, default: %g} {}", cadsnap, CADSNAP);
    tkeval(str);
}

int set_netlist_dir(int force) // 20081210
{
    if( !netlist_dir || force ) {
      Tcl_Eval(interp, "select_dir"); // 20081210
      if(!strcmp("", Tcl_GetStringResult(interp)) ) {
        return 0;
      }
      my_strdup(&netlist_dir, Tcl_GetStringResult(interp)); // 20081210
    }
    return 1;
}

void resetwin(void)
{
  int i;
  unsigned int w,h;
  XWindowAttributes wattr;
  if(has_x) {
    i = XGetWindowAttributes(display, window, &wattr); // should call only when resized
     					      // to avoid server roundtrip replies
    if(!i) { // 20171105
      return;
    }
    //if(wattr.map_state==IsUnmapped) return;
    w=wattr.width;
    h=wattr.height;
    if(debug_var>=1) fprintf(errfp, "resetwin(): x=%d y=%d   w=%d h=%d\n",wattr.x, wattr.y, w,h);
    if(debug_var>=1) fprintf(errfp, "resetwin(): changing size\n\n");
    xrect[0].x = 0;
    xrect[0].y = 0; 
    xrect[0].width = w;
    xrect[0].height = h;
    areax2 = w+2*lw;
    areay2 = h+2*lw;
    areax1 = -2*lw;
    areay1 = -2*lw;
    areaw = areax2-areax1;
    areah = areay2-areay1;

    XFreePixmap(display,save_pixmap);
    save_pixmap = XCreatePixmap(display, window, wattr.width, wattr.height, depth); // 20171111
    // save_pixmap = XCreatePixmap(display,window,areaw,areah ,depth);
    XSetTile(display,gctiled, save_pixmap);

    #ifdef HAS_CAIRO
    cairo_destroy(save_ctx);
    cairo_surface_destroy(save_sfc);
    save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, wattr.width, wattr.height);
    save_ctx = cairo_create(save_sfc);
    cairo_set_line_width(save_ctx, 1);
    cairo_set_line_join(save_ctx, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap(save_ctx, CAIRO_LINE_CAP_ROUND);
    cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (save_ctx, 20);
    #endif

/*
    for(i=0;i<cadlayers;i++)
    {
      XSetClipRectangles(display, gc[i], 0,0, xrect, 1, Unsorted);
      XSetClipRectangles(display, gcstipple[i], 0,0, xrect, 1, Unsorted);
    }
    XSetClipRectangles(display, gctiled, 0,0, xrect, 1, Unsorted);
*/
    if(pending_fullzoom) {
      zoom_full(0);
      pending_fullzoom=0;
    } 
    //debug ...
    if(debug_var>=1) fprintf(errfp, "resetwin(): Window reset\n");


    #ifdef HAS_CAIRO // 20171105
    cairo_xlib_surface_set_size(sfc, wattr.width, wattr.height);
    #endif

  }
}

void toggle_only_probes()
{

   static double save_lw;

   if(!only_probes) {
     save_lw = lw_double;
     lw_double=3.0;
   } else {
     lw_double= save_lw;
   }
   only_probes =!only_probes;
   if(only_probes) {
       Tcl_SetVar(interp,"only_probes","1",TCL_GLOBAL_ONLY);
   }
   else {
       Tcl_SetVar(interp,"only_probes","0",TCL_GLOBAL_ONLY);
   }
   change_linewidth(lw_double, 1);
}


void toggle_fullscreen()
{
    char fullscr[]="add,fullscreen";
    char normal[]="remove,fullscreen";
    static int menu_removed = 0;
    fullscreen = (fullscreen+1)%3;
    if(fullscreen==1) Tcl_SetVar(interp,"fullscreen","1",TCL_GLOBAL_ONLY);
    else if(fullscreen==2) Tcl_SetVar(interp,"fullscreen","2",TCL_GLOBAL_ONLY);
    else Tcl_SetVar(interp,"fullscreen","0",TCL_GLOBAL_ONLY);

    if(debug_var>=1) fprintf(errfp, "toggle_fullscreen(): fullscreen=%d\n", fullscreen);
    if(fullscreen==2) {
      Tcl_Eval(interp,"pack forget .menubar .statusbar; update");
      menu_removed = 1;
    }
    if(fullscreen !=2 && menu_removed) {
      Tcl_Eval(interp,"pack .menubar -anchor n -side top -fill x  -before .drw\n\
                       pack .statusbar -after .drw -anchor sw  -fill x; update");
      menu_removed=0;
    }
     

    if(fullscreen !=0) {
      window_state(display , parent_of_topwindow,fullscr);
    } else {
      window_state(display , parent_of_topwindow,normal);
    }
    pending_fullzoom=1;
}

void change_linewidth(double w, int dr)
{
    int i,j;
    j=lw=lw_double=w;
    if(lw==0) lw=1;
    if(has_x) {
      for(i=0;i<cadlayers;i++) {
          XSetLineAttributes (display, gc[i], j, LineSolid, CapRound , JoinRound);
      }                                  // lw  20070307
      // 20101211
      XSetLineAttributes (display, gctiled, j, LineSolid, CapRound , JoinRound);
    }
    if(dr) {
      resetwin();
      draw();
    }
}


void new_window(char *cell, int symbol)
{
     static char *cadname=NULL; // overflow safe 20161122
     static char *sym=NULL; // overflow safe 20161122
     struct stat buf;
     my_strdup(&cadname, (char *)Tcl_GetVar(interp,"XSCHEM_HOME_DIR", TCL_GLOBAL_ONLY));
     my_strcat(&cadname, "/xschem");
     if(stat(cadname,&buf)) { // 20121110
       my_strdup(&cadname, "/usr/bin/xschem");
     }
     if(stat(cadname,&buf)) { // 20121110
       my_strdup(&cadname, "/usr/local/bin/xschem");
     }
     if(stat(cadname,&buf)) { // 20121110
       if(debug_var>=1) fprintf(errfp, "new_window(): executable not found\n");
       return;
     }
     if(fork() == 0)
     {
      freopen("/dev/null","w",stdout);
      freopen("/dev/null","r",stdin);
      freopen("/dev/null","w",stderr);
      my_strdup(&sym, cell);
      if(!symbol) {
        my_strcat(&sym, ".sch");
	execl(cadname,cadname,"-r",sym, NULL);
      }
      else {
        my_strcat(&sym, ".sym");
        execl(cadname,cadname,"-r",sym, NULL);
      }
     }
}
int save(int confirm) //20171006 add confirm
{
     int cancel;
     int save_ok; // 20171020

     save_ok=0;
     cancel=0;
     if(current_type==SCHEMATIC)
     {
       if(modified)
       {
         if(confirm) {
           tkeval("ask_save");
           if(!strcmp(Tcl_GetStringResult(interp), "") ) cancel=1;
           if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_file(NULL);
         } else {
           save_ok = save_file(NULL);
         }
       }
     }
     else
     {
       if(modified)
       {
         if(confirm) {
           tkeval("ask_save");
           if(!strcmp(Tcl_GetStringResult(interp), "") ) cancel=1;
           if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_symbol(NULL);
         } else {
           save_ok = save_symbol(NULL);
         }
       }
     }
     if(save_ok==-1) return 1;
     return cancel;
}
void saveas(void) // changed name from ask_save_file to saveas 20121201
{

    static char *name=NULL; // overflow safe 20161122
    static char *res=NULL;
    if(current_type==SYMBOL)
    {
      my_strdup(&name, "savefile ");
      my_strcat(&name, schematic[currentsch]);
      my_strcat(&name, ".sym .sym");
      tkeval(name);
      my_strdup(&res, Tcl_GetStringResult(interp));
      if(!res) return; //20071104
      // 20070323
      if( !strcmp( Tcl_GetVar(interp,"entry1",TCL_GLOBAL_ONLY), ".sch"  ) )
        save_file(res);
      else 
        save_symbol(res);
      return;
    }
    else
    {
      my_strdup(&name, "savefile ");
      my_strcat(&name, schematic[currentsch]);
      my_strcat(&name, ".sch .sch");
      if(debug_var>=1) fprintf(errfp, "saveas(): saving: %s\n",name);
      tkeval(name);
      my_strdup(&res, Tcl_GetStringResult(interp));
      if(!res) return; //20071104
      // 20070323
      if( !strcmp( Tcl_GetVar(interp,"entry1",TCL_GLOBAL_ONLY), ".sym"  ) )
        save_symbol(res);
      else 
        save_file(res);
      return;
    }
}
void ask_new_file(void)
{
    char name[4096]; // overflow safe 20161125
    static char *s=NULL; // overflow safe 20161122
    
    if(!has_x) return;

    if(modified) { // 20161209
      if(save(1)) return;
    }

    tkeval("loadfile .sch");
    my_snprintf(name, S(name),"%s", Tcl_GetStringResult(interp));
    my_strdup(&s, "get_cell {");
    my_strcat(&s, (char *)Tcl_GetStringResult(interp));
    my_strcat(&s, "}");
    tkeval(s);
    if( strcmp(Tcl_GetStringResult(interp),"") )
    {
     if(debug_var>=1) fprintf(errfp, "ask_new_file(): load file: %s\n",Tcl_GetStringResult(interp));
     delete_hilight_net();
     currentsch = 0;
     strcpy(schematic[currentsch], Tcl_GetStringResult(interp));
     //clear_drawing();
     remove_symbols();
     if(strstr(name,".sym")) load_symbol( NULL);
     else load_file(1, NULL,1);
     my_strdup(&sch_prefix[currentsch],".");
     zoom_full(1);
    }
}

// 20071007
// remove last symbol and decrement lastinstdef
void remove_symbol(void)
{
  int i,c,j;
  j = lastinstdef-1;
  if(instdef[j].prop_ptr != NULL) {
    my_free(instdef[j].prop_ptr);
    instdef[j].prop_ptr=NULL;
  }
  // 20150409
  if(instdef[j].templ != NULL) {
    my_free(instdef[j].templ);
    instdef[j].templ=NULL;
  }
  if(instdef[j].type != NULL) {
    my_free(instdef[j].type);
    instdef[j].type=NULL;
  }
  // /20150409
  for(c=0;c<cadlayers;c++)
  {
   for(i=0;i<instdef[j].lines[c];i++)
   {
    if(instdef[j].lineptr[c][i].prop_ptr != NULL)
    {
     my_free(instdef[j].lineptr[c][i].prop_ptr);
     instdef[j].lineptr[c][i].prop_ptr=NULL;
    }
   }
   if(instdef[j].lineptr[c]) {my_free(instdef[j].lineptr[c]);instdef[j].lineptr[c]=NULL;}
   for(i=0;i<instdef[j].rects[c];i++)
   {
    if(instdef[j].boxptr[c][i].prop_ptr != NULL)
    {
     my_free(instdef[j].boxptr[c][i].prop_ptr);
     instdef[j].boxptr[c][i].prop_ptr=NULL;
    }
   }
   if(instdef[j].boxptr[c]) {my_free(instdef[j].boxptr[c]);instdef[j].boxptr[c]=NULL;}
  }
  for(i=0;i<instdef[j].texts;i++)
  {
   if(instdef[j].txtptr[i].prop_ptr != NULL)
   {
    my_free(instdef[j].txtptr[i].prop_ptr);
    instdef[j].txtptr[i].prop_ptr=NULL;
   }
   if(instdef[j].txtptr[i].txt_ptr != NULL)
   {
    my_free(instdef[j].txtptr[i].txt_ptr);
    instdef[j].txtptr[i].txt_ptr=NULL;
   }
  }
  if(instdef[j].txtptr) my_free(instdef[j].txtptr);
  instdef[j].txtptr =NULL;
  my_strdup(&instdef[j].name, NULL);

  lastinstdef--;
}

void remove_symbols(void)
{
 int j;
 
 for(j=lastinstdef-1;j>=0;j--) {
   if(debug_var>=2) fprintf(errfp, "remove_symbols(): removing symbol %d\n",j);
   remove_symbol();
 }
  if(debug_var>=2) fprintf(errfp, "remove_symbols(): done\n");
}

void clear_drawing(void)
{
 int i,j;
 if(schprop!=NULL) {my_free(schprop);schprop=NULL;}
 if(schvhdlprop!=NULL) {my_free(schvhdlprop);schvhdlprop=NULL;}
 if(schverilogprop!=NULL) {my_free(schverilogprop);schverilogprop=NULL;} //09112003
 for(i=0;i<lastwire;i++)
 {
  my_strdup(&wire[i].prop_ptr, NULL);
  my_strdup(&wire[i].node, NULL);
 } 
 lastwire = 0;
 for(i=0;i<lastinst;i++)
 {
  my_strdup(&inst_ptr[i].prop_ptr, NULL);
  my_strdup(&inst_ptr[i].name, NULL);
  my_strdup(&inst_ptr[i].instname, NULL); // 20150409
  delete_inst_node(i);
 }
 lastinst = 0;
 for(i=0;i<lasttext;i++)
 {
  if(textelement[i].prop_ptr!=NULL) 
    {my_free(textelement[i].prop_ptr);textelement[i].prop_ptr=NULL;}
  if(textelement[i].txt_ptr!=NULL) 
    {my_free(textelement[i].txt_ptr);textelement[i].txt_ptr=NULL;}
 }
 lasttext = 0;
 for(i=0;i<cadlayers;i++)
 {
  for(j=0;j<lastline[i];j++)
  {
   if(line[i][j].prop_ptr!=NULL) {my_free(line[i][j].prop_ptr);line[i][j].prop_ptr=NULL;}
  }
  for(j=0;j<lastrect[i];j++)
  {
   if(rect[i][j].prop_ptr!=NULL) {my_free(rect[i][j].prop_ptr);rect[i][j].prop_ptr=NULL;}
  }
  lastline[i] = 0;
  lastrect[i] = 0;
 }
 if(debug_var>=2) fprintf(errfp, "clear drawing(): deleted data structures, now deleting hash\n");
 free_hash();
}

void attach_labels_to_inst() // offloaded from callback.c 20171005
{
  Instdef *symbol, *symbol1;
  int npin, i, j;
  int npin1;
  double x0,y0, x10, y10, pinx0, piny0, pin1x0, pin1y0; 
  int flip, rot, rot1 ; // 20101129 
  Box *rect, *rect1;
  static char *labname=NULL;
  static char *prop=NULL; // 20161122 overflow safe
  char symname_pin[] = "devices/lab_pin";
  char symname_wire[] = "devices/lab_wire"; // 20171005
  static char *type=NULL;
  int dir;
  int drawsym;
  int k,ii,iii, skip;

  rebuild_selected_array();
  k = lastselected;
  if(k) push_undo(); // 20150327
  bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
  for(j=0;j<k;j++) if(selectedgroup[j].type==ELEMENT) {


    // 20171005
    my_strdup(&prop, inst_ptr[selectedgroup[j].n].instname);
    my_strcat(&prop, "_");
    Tcl_SetVar(interp,"custom_label_prefix", prop, TCL_GLOBAL_ONLY);
    tkeval("attach_labels_to_inst");
    if(!strcmp(Tcl_GetVar(interp, "rcode", TCL_GLOBAL_ONLY),"") ) continue;

    // 20111030 skip labels / pins
    // my_strdup(&type,get_tok_value((inst_ptr[selectedgroup[j].n].ptr+instdef)->prop_ptr,"type",0)); // 20150409
    my_strdup(&type,(inst_ptr[selectedgroup[j].n].ptr+instdef)->type); // 20150409
    if( type && (strcmp(type,"label") && strcmp(type,"ipin")&&strcmp(type,"opin")&&strcmp(type,"iopin") )==0)
      continue;
    // /20111030

    if(debug_var>=1) fprintf(errfp, " 200711 1--> %s %g %g   %s\n", 
        inst_ptr[selectedgroup[j].n].name, 
        inst_ptr[selectedgroup[j].n].x0, 
        inst_ptr[selectedgroup[j].n].y0, 
        instdef[inst_ptr[selectedgroup[j].n].ptr].name);

    x0 = inst_ptr[selectedgroup[j].n].x0;
    y0 = inst_ptr[selectedgroup[j].n].y0;
    rot = inst_ptr[selectedgroup[j].n].rot;
    flip = inst_ptr[selectedgroup[j].n].flip;
    symbol = instdef + inst_ptr[selectedgroup[j].n].ptr;
    npin = symbol->rects[PINLAYER];
    rect=symbol->boxptr[PINLAYER];
    
    for(i=0;i<npin;i++) {
       drawsym=0;
       my_strdup(&labname,get_tok_value(rect[i].prop_ptr,"name",0));
       if(debug_var>=1) fprintf(errfp,"200711 2 --> labname=%s\n", labname);
       
       if(strcmp(get_tok_value(rect[i].prop_ptr,"dir",0),"in")) dir=1; // out or inout pin
       else dir=0; // input pin

       ROTATION(0.0, 0.0, (rect[i].x1+rect[i].x2)/2, (rect[i].y1+rect[i].y2)/2, pinx0, piny0); // 20101129
       pinx0 += x0;
       piny0 += y0;
       // pinx0=x0+(rect[i].x1+rect[i].x2)/2;
       // piny0=y0+(rect[i].y1+rect[i].y2)/2;

       skip=0;
       for(ii=0;ii<lastinst;ii++) {   // 20090217 no labels if already labeled pin
         if(ii == selectedgroup[j].n) continue;
  
       // 20111030 correctly handle pins and orientations
         x10 = inst_ptr[ii].x0;
         y10 = inst_ptr[ii].y0;
         rot = inst_ptr[ii].rot;
         flip = inst_ptr[ii].flip;
         symbol1 = instdef + inst_ptr[ii].ptr;
         npin1 = symbol1->rects[PINLAYER];
         rect1 = symbol1->boxptr[PINLAYER];

         for(iii=0;iii<npin1;iii++) {
           ROTATION(0.0, 0.0, (rect1[iii].x1+rect1[iii].x2)/2, (rect1[iii].y1+rect1[iii].y2)/2, pin1x0, pin1y0);
           pin1x0 += x10;
           pin1y0 += y10;

       // /20111030


           if( pin1x0 == pinx0 && pin1y0 == piny0 ) {
             skip=1;
             break;
           }

         // 20111030
         }
         rot = inst_ptr[selectedgroup[j].n].rot;  // restore original flip and rot values
         flip = inst_ptr[selectedgroup[j].n].flip;
         // /20111030
       }
       for(ii=0; ii<lastwire; ii++) {  // 20090225 also skip if wires attached

         if( touch(wire[ii].x1, wire[ii].y1, wire[ii].x2, wire[ii].y2, pinx0, piny0)) {

           skip=1;
           break;
         }
       }
       if(!skip) {
         my_strdup(&prop, "name=p1 lab=");

         // 20171005
         if(!strcmp(Tcl_GetVar(interp,"use_label_prefix",TCL_GLOBAL_ONLY),"1")) {
           my_strcat(&prop, (char *)Tcl_GetVar(interp,"custom_label_prefix",TCL_GLOBAL_ONLY));
         }
         // /20171005

         my_strcat(&prop, labname);
         dir ^= flip; // 20101129  20111030
         rot1=rot; // 20111103
         if(rot1==1 || rot1==2) { dir=!dir;rot1 = (rot1+2) %4;}  // 20111103
         if(!strcmp(Tcl_GetVar(interp,"use_lab_wire",TCL_GLOBAL_ONLY),"0")) {
           place_symbol(-1,symname_pin, pinx0, piny0, rot1, dir, prop, drawsym);
         } else {
           place_symbol(-1,symname_wire, pinx0, piny0, rot1, dir, prop, drawsym);
         }


       }
       if(debug_var>=1) fprintf(errfp, "%d   %g %g %s\n", i, pinx0, piny0,labname);
    }
  }
  // draw things if last place_symbol was skipped
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
}

// draw_sym==3 start bbox end bbox, draw placed symbols 
// draw_sym==1 end bbox, draw placed symbols 
// draw_sym==2 start  bbox
// draw_sym==0 dont draw
void place_symbol(int pos,char *symbol_name, double x, double y, int rot, int flip, 
                   char *inst_props, int draw_sym)
// if symbol_name is a valid string load specified cell and
// use the given params, otherwise query user
{
 int i,j,n;
 static char *name=NULL; // overflow safe 20161122
 char *type;
 int cond;
 if(current_type==SYMBOL) return; // 20161210 dont allow components placed inside symbols
 if(symbol_name==NULL) {
   tkeval("loadinst .sym");
   my_strdup(&name, Tcl_GetStringResult(interp));
   if(name && strcmp(name,"")) push_undo(); // 20150327
 }
 else my_strdup(&name, symbol_name);
 if(!name || !strcmp(name,"")) return;
 i=match_symbol(name);

 if(i!=-1)
 {
  check_inst_storage();
  if(pos==-1) n=lastinst;
  else
  {
   for(j=lastinst;j>pos;j--)
   {
    inst_ptr[j]=inst_ptr[j-1];
   }
   n=pos;
  }
  // 03-02-2000
  if(debug_var>=1) fprintf(errfp, "place_symbol(): checked inst_ptr storage, instdef number i=%d\n", i);
  inst_ptr[n].ptr = i;
  inst_ptr[n].name=NULL;
  inst_ptr[n].instname=NULL; // 20150409
  if(debug_var>=1) fprintf(errfp, "place_symbol(): entering my_strdup: name=%s\n",name);  // 03-02-2000
  my_strdup(&inst_ptr[n].name ,name);
  if(debug_var>=1) fprintf(errfp, "place_symbol(): done my_strdup: name=%s\n",name);  // 03-02-2000
  // inst_ptr[n].x0=symbol_name ? x : mousex_snap;
  // inst_ptr[n].y0=symbol_name ? y : mousey_snap;
  inst_ptr[n].x0= x ; // 20070228 x and y given in callback
  inst_ptr[n].y0= y ; // 20070228
  inst_ptr[n].rot=symbol_name ? rot : 0;
  inst_ptr[n].flip=symbol_name ? flip : 0;

  inst_ptr[n].flags=0;
  inst_ptr[n].sel=0;
  inst_ptr[n].node=NULL;
  inst_ptr[n].prop_ptr=NULL;
  if(debug_var>=1) fprintf(errfp, "place_symbol() :all inst_ptr members set\n");  // 03-02-2000
  if(inst_props) {
    new_prop_string(&inst_ptr[n].prop_ptr, inst_props,0);
  }
  else {
    set_inst_prop(n);
  }
  if(debug_var>=1) fprintf(errfp, "place_symbol(): done set_inst_prop()\n");  // 03-02-2000
  hash_proplist(inst_ptr[n].prop_ptr , 0);

  my_strdup2(&inst_ptr[n].instname, get_tok_value(inst_ptr[n].prop_ptr,"name",0) ); // 20150409

  // type=get_tok_value(instdef[inst_ptr[n].ptr].prop_ptr,"type",0); // 20150409 removed
  type = instdef[inst_ptr[n].ptr].type; //20150409
  cond= strcmp(type,"label") && strcmp(type,"ipin") &&
        strcmp(type,"opin") &&  strcmp(type,"iopin");
  if(cond) inst_ptr[n].flags|=2;
  else inst_ptr[n].flags &=~2;

  if(draw_sym&2) bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);

  symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1,
                    &inst_ptr[n].x2, &inst_ptr[n].y2);
  bbox(ADD, inst_ptr[n].x1, inst_ptr[n].y1, 
            inst_ptr[n].x2, inst_ptr[n].y2);
  lastinst++;
  if(draw_sym&1) {
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  modified=1;
  //  hilight new element 24122002
  if(x_initialized) drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  if(x_initialized) drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  select_element(n, SELECTED,0);
  if(x_initialized) drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  if(x_initialized) drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);

 }
}

void symbol_in_new_window(void)
{
 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT)
 {
  new_window(schematic[currentsch],1);
 }
 else
 {
  new_window(inst_ptr[selectedgroup[0].n].name,1);
 }

}


void edit_in_new_window(void)
{
 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT) 
 {
  // new_window("", 0);
  new_window(schematic[currentsch], 0); // 20111007 duplicate current schematic if no inst selected
  return;
 }
 else
 {
  if(                   // do not descend if not subcircuit
     strcmp(
        // get_tok_value( (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "type",0), // 20150409
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, // 20150409
         "subcircuit"
     ) && 
     strcmp(
        // get_tok_value( (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "type",0), // 20150409
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, // 20150409
         "primitive"
     ) 
  ) return;
  new_window(inst_ptr[selectedgroup[0].n].name,0);
 }
}

void launcher(void) // 20161102
{
  char *str;
  static char *program=NULL;
  int n;
  rebuild_selected_array();
  if(lastselected ==1 && selectedgroup[0].type==ELEMENT) 
  {
    n=selectedgroup[0].n;
    my_strdup(&program, get_tok_value(inst_ptr[n].prop_ptr,"program",2)); // 20170414 handle backslashes
    str = get_tok_value(inst_ptr[n].prop_ptr,"url",2); // 20170414 handle backslashes
    if(str[0] || (program && program[0])) {
      Tcl_SetVar(interp,"launcher_var", str, TCL_GLOBAL_ONLY);
      if(program && program[0]) { // 20170413 leave launcher_program empty if unspecified
        Tcl_SetVar(interp,"launcher_program", program, TCL_GLOBAL_ONLY);
      } else {
        Tcl_SetVar(interp,"launcher_program", "", TCL_GLOBAL_ONLY);
      }
      Tcl_Eval(interp, "launcher");
    } else {
      my_strdup(&program, get_tok_value(inst_ptr[n].prop_ptr,"tclcommand",2)); // 20170415
      if(program && program[0]) { // 20170415 execute tcl command
        Tcl_EvalEx(interp, program, -1, TCL_EVAL_GLOBAL);
      }
    }
  } 
}

void descend(void)
{
 char *str;
 static char *name=NULL; // overflow safe 20161122
 int save_ok; // 20171020

 save_ok=0;

 rebuild_selected_array();
 if(lastselected !=1 || selectedgroup[0].type!=ELEMENT) 
 {
  if(debug_var>=1) fprintf(errfp, "descend(): wrong selection\n");
  return;
 }
 else
 {
  if(debug_var>=1) fprintf(errfp, "descend(): selected:%s\n", inst_ptr[selectedgroup[0].n].name);
  if(!strcmp(schematic[currentsch],""))
  {
    my_strdup(&name, "savefile ");
    my_strcat(&name, schematic[currentsch]);
    my_strcat(&name, ".sch .sch");
    if(debug_var>=1) fprintf(errfp, "descend(): saving: %s\n",name);
    tkeval(name);
    strcpy(schematic[currentsch], Tcl_GetStringResult(interp));
    if(!strcmp(schematic[currentsch],"")) return;
    save_ok = save_file(schematic[currentsch]);

//    Tcl_SetVar(interp,"entry1",schematic[currentsch],TCL_GLOBAL_ONLY);
//    tkeval("entry_line {Save file:}");
//    strcpy(schematic[currentsch], Tcl_GetStringResult(interp));
//    if(!strcmp(schematic[currentsch],"")) return;
    if(save_ok==-1) return; // 20171020
  }

  if(			// do not descend if not subcircuit
     strcmp(
        // get_tok_value( (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "type",0), // 20150409
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, // 20150409
         "subcircuit"
     ) && 
     strcmp(
        // get_tok_value( (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "type",0), // 20150409
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type, // 20150409
         "primitive"
     ) 
  ) return;

  if(modified)
  {
    if(save(1)) return; // 20161209
  }
  current_type=SCHEMATIC;

  // build up current hierarchy path
  // str=get_tok_value(inst_ptr[selectedgroup[0].n].prop_ptr,"name",0); // 20150409
  str=inst_ptr[selectedgroup[0].n].instname; // 20150409
  my_strdup(&sch_prefix[currentsch+1], sch_prefix[currentsch]);
  my_strcat(&sch_prefix[currentsch+1], str);
  my_strcat(&sch_prefix[currentsch+1], ".");
  if(debug_var>=1) fprintf(errfp, "descend(): current path: %s\n", sch_prefix[currentsch+1]);

  strcpy(schematic[currentsch+1],inst_ptr[selectedgroup[0].n].name);
  //clear_drawing();
  previous_instance[currentsch]=selectedgroup[0].n;
  zoom_array[currentsch].x=xorigin;
  zoom_array[currentsch].y=yorigin;
  zoom_array[currentsch].zoom=zoom;
  hilight_child_pins(previous_instance[currentsch]);
  currentsch++;
  unselect_all();
  remove_symbols();
  load_file(1,NULL,1);
  if(hilight_nets) 
  {
    prepare_netlist_structs();
    //delete_netlist_structs(); // 20161222 done in load_file() and in prepare_netlist_structs() when needed
 
  }
  zoom_full(1);
 }
}

void go_back(int confirm) // 20171006 add confirm
{
 int prev_curr_type=0;
 int save_ok;  // 20171020

 save_ok=0;
 if(currentsch>0)
 {
  //clear_drawing();
  if( (prev_curr_type=current_type)==SCHEMATIC)
  {
   if(modified)
   {
     if(confirm) {
       tkeval("ask_save");
       if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_file(NULL);
       else if(!strcmp(Tcl_GetStringResult(interp), "") ) return;
     } else {
       save_ok = save_file(NULL);
     }
   }
  }
  else
  {
   if(modified)
   {
     if(confirm) {
       tkeval("ask_save");
       if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_symbol(NULL);
       else if(!strcmp(Tcl_GetStringResult(interp), "") ) return;
     } else {
       save_ok = save_symbol(NULL);
     }
   }
  }
  if(save_ok==-1) return; // 20171020
  strcpy(schematic[currentsch] , "");
  currentsch--;
  remove_symbols();
  load_file(1,NULL,1);
  unselect_all();

  if(prev_curr_type==SCHEMATIC) {
    hilight_parent_pins();
  } // else {
    //   prepare_netlist_structs();  // 20150407 moved above
    //   delete_netlist_structs();
    //}
  xorigin=zoom_array[currentsch].x;
  yorigin=zoom_array[currentsch].y;
  zoom=zoom_array[currentsch].zoom;
  mooz=1/zoom;

  set_linewidth();
  resetwin();
  draw();

  current_type=SCHEMATIC;
  if(debug_var>=1) fprintf(errfp, "go_back(): current path: %s\n", sch_prefix[currentsch]);
 }
}

void set_linewidth()
{
 int i,j;
   if(change_lw) 
    lw_double=1/zoom*1.3;		// on some servers zero width
   lw=j=lw_double;			// draws fast but not good...
   if(lw==0) lw=1;			// lw used to calculate bound boxes
   if(debug_var>=1) fprintf(errfp, "set_linewidth(): lw=%d, lw_double=%g\n", lw, lw_double);
   if(has_x) {
      for(i=0;i<cadlayers;i++) {
          XSetLineAttributes (display, gc[i], j, LineSolid, CapRound , JoinRound);
      }
      XSetLineAttributes (display, gctiled, j, LineSolid, CapRound , JoinRound); // j as linewidth 20171105
   }
}

void calc_drawing_bbox(Box *boundbox)
{

 Box tmp;
 int c, i; 
 int count=0;

 boundbox->x1=-100;
 boundbox->x2=100;
 boundbox->y1=-100;
 boundbox->y2=100;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastline[c];i++)
  {
   tmp.x1=line[c][i].x1;
   tmp.x2=line[c][i].x2;
   tmp.y1=line[c][i].y1;
   tmp.y2=line[c][i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
  }
  for(i=0;i<lastrect[c];i++)
  {
   tmp.x1=rect[c][i].x1;
   tmp.x2=rect[c][i].x2;
   tmp.y1=rect[c][i].y1;
   tmp.y2=rect[c][i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
  }
 }
 for(i=0;i<lastwire;i++)
 { 
   tmp.x1=wire[i].x1;
   tmp.x2=wire[i].x2;
   tmp.y1=wire[i].y1;
   tmp.y2=wire[i].y2;
   count++;
   updatebbox(count,boundbox,&tmp);
 }
 for(i=0;i<lasttext;i++)
 {
   text_bbox(textelement[i].txt_ptr, textelement[i].xscale,
         textelement[i].yscale,textelement[i].rot, textelement[i].flip,
         textelement[i].x0, textelement[i].y0,
         &tmp.x1,&tmp.y1, &tmp.x2,&tmp.y2);
   count++;
   updatebbox(count,boundbox,&tmp);
 }
 for(i=0;i<lastinst;i++)
 {
  tmp.x1=inst_ptr[i].x1;
  tmp.y1=inst_ptr[i].y1;
  tmp.x2=inst_ptr[i].x2;
  tmp.y2=inst_ptr[i].y2;
  count++;
  updatebbox(count,boundbox,&tmp);
 }

}

void zoom_full(int dr)
{
  Box boundbox;
  double yy1;

  calc_drawing_bbox(&boundbox);
  zoom=(boundbox.x2-boundbox.x1)/(areaw-4*lw);
  yy1=(boundbox.y2-boundbox.y1)/(areah-4*lw);
  if(yy1>zoom) zoom=yy1;
  zoom*=1.05;
  mooz=1/zoom;
  xorigin=-boundbox.x1+(areaw-4*lw)/40*zoom;
  yorigin=(areah-4*lw)*zoom-boundbox.y2 - (areah-4*lw)/40*zoom;
  if(debug_var>=1) fprintf(errfp, "zoom_full(): areaw=%d, areah=%d\n", areaw, areah);
   
  if(dr)
  { 
   set_linewidth();
   draw(); // 20121111
  }
}


void view_unzoom(double z)
{
    double factor;
    factor = z!=0.0 ? z : CADZOOMSTEP;
    if(zoom<CADMINZOOM) return;
    zoom/= factor;
    mooz=1/zoom;
    xorigin=-mousex_snap+(mousex_snap+xorigin)/factor;
    yorigin=-mousey_snap+(mousey_snap+yorigin)/factor;
    set_linewidth();
    resetwin();
    draw();
}

void view_zoom(double z)
{
  double factor;
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(zoom>CADMAXZOOM) return;
  zoom*= factor;
  mooz=1/zoom;
  xorigin=xorigin+areaw*zoom*(1-1/factor)/2;
  yorigin=yorigin+areah*zoom*(1-1/factor)/2;
  set_linewidth();
  resetwin();
  draw();
}

void zoom_box(int what)
{
  static double x1,y1,x2,y2;
  static double xx1,yy1,xx2,yy2;

  if( (what & BEGIN) )
  {
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    rubber |= STARTZOOM;
  }
  if( what & END)
  {
    rubber &= ~STARTZOOM;
    RECTORDER(x1,y1,x2,y2);
    drawtemprect(gctiled, NOW, xx1,yy1,xx2,yy2);
    xorigin=-x1;yorigin=-y1;
    zoom=(x2-x1)/(areaw-4*lw);
    yy1=(y2-y1)/(areah-4*lw);
    if(yy1>zoom) zoom=yy1;
    mooz=1/zoom;
    set_linewidth();
    resetwin();
    draw();
    if(debug_var>=1) fprintf(errfp, "zoom_box(): coord: %g %g %g %g zoom=%g\n",x1,y1,mousex_snap, mousey_snap,zoom);
  }
  if(what & RUBBER)
  {
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    x2=mousex_snap;y2=mousey_snap;
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[SELLAYER], NOW, xx1,yy1,xx2,yy2);
  }
}

void draw_stuff(void)
{
   double x1,y1,w,h, x2, y2;
   int i;
   for(i=0;i<=4000;i++)
    {
     w=(float)(areaw*zoom/15) * rand() / (RAND_MAX+1.0);
     h=(float)(areah*zoom/15) * rand() / (RAND_MAX+1.0);
     x1=(float)(areaw*zoom) * rand() / (RAND_MAX+1.0)-xorigin;
     y1=(float)(areah*zoom) * rand() / (RAND_MAX+1.0)-yorigin;
     x2=x1+w;
     y2=y1+h;
     ORDER(x1,y1,x2,y2);
     rectcolor = (int) (10.0*rand()/(RAND_MAX+1.0))+4;
     storeobject(-1, x1,y1,x2,y2,RECT,rectcolor, 0, NULL);
   }
   for(i=0;i<=40;i++)
   {
     w=(float)(4);
     h=(float)(4);
     x1=(float)(areaw*zoom) * rand() / (RAND_MAX+1.0)-xorigin;
     y1=(float)(areah*zoom) * rand() / (RAND_MAX+1.0)-yorigin;
     x2=x1+w;
     y2=y1+h;
     rectcolor = (int) (10.0*rand()/(RAND_MAX+1.0))+4;
     RECTORDER(x1,y1,x2,y2);
     storeobject(-1, x1,y1,x2,y2,RECT,rectcolor, 0, NULL);
   }
}

void new_wire(int what, double mx_snap, double my_snap)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) ) {
     if( (x1!=x2 || y1!=y2) && (rubber & STARTWIRE) ) {
       ORDER(x1,y1,x2,y2);
       push_undo();
       storeobject(-1, x1,y1,x2,y2,WIRE,0,0,NULL);
       drawline(gc[WIRELAYER],NOW, x1,y1,x2,y2);
     }

     if(! (what &END)) {
       x1=mx_snap;
       y1=my_snap;
       x2=mousex_snap;
       y2=mousey_snap;
       xx1=x1;
       yy1=y1;
       xx2=mousex_snap;
       yy2=mousey_snap;
       ORDER(xx1,yy1,xx2,yy2);
       drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx2,yy2);
     }

     rubber |= STARTWIRE;
   }
   if( what & END) {
     rubber &= ~STARTWIRE;
   }
   if( (what & RUBBER)  ) {
     xx1=x1;yy1=y1;xx2=x2;yy2=y2;
     ORDER(xx1,yy1,xx2,yy2);
     drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
     x2 = mx_snap; y2 = my_snap;
     xx1 = x1; yy1 = y1;
     xx2 = x2; yy2=y2;
     ORDER(xx1,yy1,xx2,yy2);
     drawtempline(gc[WIRELAYER], NOW, xx1,yy1,xx2,yy2);
   }
}

void change_layer()
{
  int k, n, type, c;
  double x1,y1,x2,y2;


   if(lastselected) push_undo(); // 20151204
   for(k=0;k<lastselected;k++)
   {
     n=selectedgroup[k].n;
     type=selectedgroup[k].type;
     c=selectedgroup[k].col;
     if(type==LINE && line[c][n].sel==SELECTED) {
       x1 = line[c][n].x1;
       y1 = line[c][n].y1;
       x2 = line[c][n].x2;
       y2 = line[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,LINE,rectcolor, 0, line[c][n].prop_ptr);
     }
     else if(type==RECT && rect[c][n].sel==SELECTED) {
       x1 = rect[c][n].x1;
       y1 = rect[c][n].y1;
       x2 = rect[c][n].x2;
       y2 = rect[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,RECT,rectcolor, 0, rect[c][n].prop_ptr);
     }
   }
   if(lastselected) delete_only_rect_and_line();
   unselect_all();
}

void new_line(int what)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) )
   {
    if( (x1!=x2 || y1!=y2) && (rubber & STARTLINE) )
    {
     ORDER(x1,y1,x2,y2);
     push_undo();
     drawline(gc[rectcolor], NOW, x1,y1,x2,y2);
     storeobject(-1, x1,y1,x2,y2,LINE,rectcolor, 0, NULL);
    }
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    rubber |= STARTLINE;
   }
   if( what & END)
   {
    rubber &= ~STARTLINE;
   }

   if(what & RUBBER)
   {
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    ORDER(xx1,yy1,xx2,yy2);
    drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2);
    x2=mousex_snap;y2=mousey_snap;
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    ORDER(xx1,yy1,xx2,yy2);
    drawtempline(gc[rectcolor], NOW, xx1,yy1,xx2,yy2);
   }
}

void new_rect(int what)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) )
   {
    if( (x1!=x2 || y1!=y2) && (rubber & STARTRECT) )
    {
     RECTORDER(x1,y1,x2,y2); 
     push_undo();
     drawrect(gc[rectcolor], NOW, x1,y1,x2,y2);
     filledrect(gcstipple[rectcolor], NOW, x1,y1,x2,y2);
     storeobject(-1, x1,y1,x2,y2,RECT,rectcolor, 0, NULL);
    }
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    rubber |= STARTRECT;
   }
   if( what & END)
   {
    rubber &= ~STARTRECT;
   }
   if(what & RUBBER)
   {
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    x2=mousex_snap;y2=mousey_snap;
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[rectcolor], NOW, xx1,yy1,xx2,yy2);
   }
}

#ifdef HAS_CAIRO
void text_bbox(char *str, double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
{
  int c=0, length =0;
  double size;
  cairo_text_extents_t ext;
  cairo_font_extents_t fext;
  double ww, hh;

  size = (xscale+yscale)*26.*cairo_font_scale;
  cairo_set_font_size (ctx, size*mooz);
  cairo_text_extents(ctx, "A", &ext);
  cairo_font_extents(ctx, &fext);
  cairo_longest_line=0;
  cairo_lines=1;
  if(str!=NULL) while( str[c] )
  {
   if(str[c++]=='\n') {
     (cairo_lines)++;length=0;
   }
   else {
     length++;
   }
   if(length > cairo_longest_line) {
     cairo_longest_line = length;
   }
  }
  ww=cairo_longest_line;
  hh=cairo_lines;
  ww = ww*ext.x_advance*zoom;
  hh = hh*fext.height*cairo_font_line_spacing*zoom;
  *rx1=x1;*ry1=y1;
  ROTATION(0.0,0.0,ww,hh,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
}
#else //!HAS_CAIRO
void text_bbox(char * str,double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
{
 register int c=0, length =0;
 double w, h;

  w=0;h=1;
  if(str!=NULL) while( str[c] )
  {
   if((str)[c++]=='\n') {h++;length=0;}
   else length++;
   if(length > w)
     w = length;
  }                
  w *= (FONTWIDTH+FONTWHITESPACE)*xscale;
  h *= (FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)*yscale;
  *rx1=x1;*ry1=y1;
  ROTATION(0.0,0.0,w,h,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2)); 
}
#endif

void place_text(int draw_text, double mx, double my)
{
  char *txt;

  // 20171112
  #ifdef HAS_CAIRO
  char *textprop;
  int textlayer;
  #endif
  GC savegc, gctext;

  Tcl_SetVar(interp,"props","",TCL_GLOBAL_ONLY);

  if(Tcl_GetVar(interp,"hsize",TCL_GLOBAL_ONLY)==NULL)
   Tcl_SetVar(interp,"hsize","0.4",TCL_GLOBAL_ONLY);
  if(Tcl_GetVar(interp,"vsize",TCL_GLOBAL_ONLY)==NULL)
   Tcl_SetVar(interp,"vsize","0.4",TCL_GLOBAL_ONLY);
  tkeval("enter_text {text:}");

  if(debug_var>=1) fprintf(errfp, "place_text(): hsize=%s vsize=%s\n",Tcl_GetVar(interp,"hsize",TCL_GLOBAL_ONLY),
           Tcl_GetVar(interp,"vsize",TCL_GLOBAL_ONLY) );
  
  txt =  (char *)Tcl_GetVar(interp,"txt",TCL_GLOBAL_ONLY);
  if(!strcmp(txt,"")) return;	// 01112004 dont allocate text object if empty string given
  push_undo(); // 20150327
  check_text_storage();
  textelement[lasttext].txt_ptr=NULL;
  textelement[lasttext].prop_ptr=NULL;  // 20111006 added missing initialization of pointer
  my_strdup(&textelement[lasttext].txt_ptr, txt);
  textelement[lasttext].x0=mx;
  textelement[lasttext].y0=my;
  textelement[lasttext].rot=0;
  textelement[lasttext].flip=0;
  textelement[lasttext].sel=0;
  textelement[lasttext].xscale=
   atof(Tcl_GetVar(interp,"hsize",TCL_GLOBAL_ONLY));
  textelement[lasttext].yscale=
   atof(Tcl_GetVar(interp,"vsize",TCL_GLOBAL_ONLY));
   my_strdup(& textelement[lasttext].prop_ptr, (char *)Tcl_GetVar(interp,"props",TCL_GLOBAL_ONLY));
  // debug ...
  // textelement[lasttext].prop_ptr=NULL;
  if(debug_var>=1) fprintf(errfp, "place_text(): done text input\n");

  gctext = gc[TEXTLAYER];
  savegc = gctext;
  #ifdef HAS_CAIRO
  textprop = get_tok_value(textelement[lasttext].prop_ptr, "layer", 0);
  if(textprop[0]!=0) {
    textlayer = atoi(textprop);
    if(textlayer >= 0 && textlayer < cadlayers) gctext = gc[textlayer];
  }
  #endif
  if(draw_text) draw_string(gctext, NOW, textelement[lasttext].txt_ptr, 0, 0, 
              textelement[lasttext].x0,textelement[lasttext].y0,
              textelement[lasttext].xscale, textelement[lasttext].yscale);
  gctext = savegc;

  if(x_initialized) drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  if(x_initialized) drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  select_text(lasttext, SELECTED);
  if(x_initialized) drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  if(x_initialized) drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  lasttext++;
  modified=1;
}

void pan2(int what, int mx, int my) // 20121123
{
  int dx, dy, ddx, ddy;
  static int mx_save, my_save;
  static int mmx_save, mmy_save;
  static double xorig_save, yorig_save;
  if(what & BEGIN) {
    mmx_save = mx_save = mx; 
    mmy_save = my_save = my; 
    xorig_save = xorigin;
    yorig_save = yorigin;
  }
  else if(what == RUBBER) {
    dx = mx - mx_save;
    dy = my - my_save;
    ddx = abs(mx -mmx_save);
    ddy = abs(my -mmy_save);
    if(ddx>5 || ddy>5) {
      xorigin = xorig_save + dx*zoom;
      yorigin = yorig_save + dy*zoom;
      draw();
      mmx_save = mx;
      mmy_save = my;
    }
  }
}

void pan(int what)
{
 static double xpan,ypan,xpan2,ypan2;
 static double xx1,xx2,yy1,yy2;
 if(what & RUBBER)
 {
    xx1=xpan;yy1=ypan;xx2=xpan2;yy2=ypan2;
    ORDER(xx1,yy1,xx2,yy2);
    drawtempline(gctiled, NOW, xx1,yy1,xx2,yy2); 
    xpan2=mousex_snap;ypan2=mousey_snap;
    xx1=xpan;yy1=ypan;xx2=xpan2;yy2=ypan2;
    ORDER(xx1,yy1,xx2,yy2);
    drawtempline(gc[SELLAYER], NOW, xx1,yy1,xx2,yy2); 
 }
 if(what & BEGIN)
 {
    rubber |= STARTPAN;
    xpan=mousex_snap;ypan=mousey_snap;xpan2=xpan;ypan2=ypan;
 }
 if(what & END)
 {
    rubber &= ~STARTPAN;
    xorigin+=-xpan+mousex_snap;yorigin+=-ypan+mousey_snap;
    draw();
 }
}

// 20150927 select=1: select objects, select=0: unselect objects
void select_rect(int what, int select)
{
 static double xrect,yrect,xrect2,yrect2;
 static double xx1,xx2,yy1,yy2;
 static int sel;

 if(what & RUBBER)
 {
    xx1=xrect;xx2=xrect2;yy1=yrect;yy2=yrect2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    xrect2=mousex_snap;yrect2=mousey_snap;

    // 20171026 update unselected objects while dragging
    rebuild_selected_array();
    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xx1, yy1, xx2, yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    if(!sel) select_inside(xx1, yy1, xx2, yy2, sel);
    bbox(END,0.0, 0.0, 0.0, 0.0);

    xx1=xrect;xx2=xrect2;yy1=yrect;yy2=yrect2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[SELLAYER],NOW, xx1,yy1,xx2,yy2);
 }
 if(what & BEGIN)
 {
    sel = select; // 20150927
    rubber |= STARTSELECT;
    xrect=xrect2=mousex_snap;
    yrect=yrect2=mousey_snap;
 }
 if(what & END)
 {
    RECTORDER(xrect,yrect,xrect2,yrect2);
    drawtemprect(gctiled, NOW, xrect,yrect,xrect2,yrect2);
    draw_selection(gc[SELLAYER], 0);
    select_inside(xrect,yrect,xrect2,yrect2, sel);
    rubber &= ~STARTSELECT;
 }
}

