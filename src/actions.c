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
    my_snprintf(str, S(str), "alert_ {snap: %.16g, default: %.16g} {}", cadsnap, CADSNAP);
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
  XWindowAttributes wattr;
  if(has_x) {
    i = XGetWindowAttributes(display, window, &wattr); // should call only when resized
     					      // to avoid server roundtrip replies
    if(!i) { // 20171105
      return;
    }
    //if(wattr.map_state==IsUnmapped) return;

    xschem_w=wattr.width;
    xschem_h=wattr.height;
    areax2 = xschem_w+2*lw;
    areay2 = xschem_h+2*lw;
    areax1 = -2*lw;
    areay1 = -2*lw;
    areaw = areax2-areax1;
    areah = areay2-areay1;

    if( xschem_w !=xrect[0].width || xschem_h !=xrect[0].height) { //20171123 avoid unnecessary work if no resize
      if(debug_var>=1) fprintf(errfp, "resetwin(): x=%d y=%d   xschem_w=%d xschem_h=%d\n",
                       wattr.x, wattr.y, xschem_w,xschem_h);
      if(debug_var>=1) fprintf(errfp, "resetwin(): changing size\n\n");
      xrect[0].x = 0;
      xrect[0].y = 0; 
      xrect[0].width = xschem_w;
      xrect[0].height = xschem_h;
  
      XFreePixmap(display,save_pixmap);
      save_pixmap = XCreatePixmap(display, window, xschem_w, xschem_h, depth); // 20171111
      XSetTile(display,gctiled, save_pixmap);

      #ifdef HAS_CAIRO
      cairo_destroy(save_ctx);
      cairo_surface_destroy(save_sfc);

      #if HAS_XRENDER==1
      #if HAS_XCB==1
      save_sfc = cairo_xcb_surface_create_with_xrender_format(xcbconn, screen_xcb, save_pixmap, 
           &format_rgb, xschem_w, xschem_h);
      #else
      save_sfc = cairo_xlib_surface_create_with_xrender_format(display, save_pixmap, 
           DefaultScreenOfDisplay(display), format, xschem_w, xschem_h);
      #endif //HAS_XCB
      #else
      save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, xschem_w, xschem_h);
      #endif //HAS_XRENDER

      if(cairo_surface_status(save_sfc)!=CAIRO_STATUS_SUCCESS) {
        fprintf(errfp, "ERROR: invalid cairo xcb surface\n");
         exit(-1);
      }
      save_ctx = cairo_create(save_sfc);
      cairo_set_line_width(save_ctx, 1);
      cairo_set_line_join(save_ctx, CAIRO_LINE_JOIN_ROUND);
      cairo_set_line_cap(save_ctx, CAIRO_LINE_CAP_ROUND);
      cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (save_ctx, 20);
      //// 20171125 select xlib or xcb :-)
      #if HAS_XCB==1
      cairo_xcb_surface_set_size(sfc, xschem_w, xschem_h); // 20171123
      #else
      cairo_xlib_surface_set_size(sfc, xschem_w, xschem_h); // 20171123
      #endif //HAS_XCB
      #endif //HAS_CAIRO

    } 

    if(pending_fullzoom) {
      zoom_full(0);
      pending_fullzoom=0;
    } 
    //debug ...
    if(debug_var>=1) fprintf(errfp, "resetwin(): Window reset\n");
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
  areax1 = -2*lw;
  areay1 = -2*lw;
  areax2 = xrect[0].width+2*lw;
  areay2 = xrect[0].height+2*lw;
  areaw = areax2-areax1;
  areah = areay2 - areay1;

  if(dr) {
    // resetwin();
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
           if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_schematic(NULL);
         } else {
           save_ok = save_schematic(NULL);
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
        save_schematic(res);
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
        save_schematic(res);
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
     my_strncpy(schematic[currentsch], Tcl_GetStringResult(interp), S(schematic[currentsch]));
     //clear_drawing();
     remove_symbols();
     if(strstr(name,".sym")) load_symbol( NULL);
     else load_schematic(1, NULL,1);
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
  for(c=0;c<cadlayers;c++) // 20171115
  {
   for(i=0;i<instdef[j].polygons[c];i++)
   {
     if(instdef[j].polygonptr[c][i].prop_ptr != NULL) {
       my_free(instdef[j].polygonptr[c][i].prop_ptr);
       instdef[j].polygonptr[c][i].prop_ptr=NULL;
     }
     my_free(instdef[j].polygonptr[c][i].x);
     instdef[j].polygonptr[c][i].x=NULL;
     my_free(instdef[j].polygonptr[c][i].y);
     instdef[j].polygonptr[c][i].y=NULL;
     my_free(instdef[j].polygonptr[c][i].selected_point);
     instdef[j].polygonptr[c][i].selected_point=NULL;
   }
   if(instdef[j].polygonptr[c]) {my_free(instdef[j].polygonptr[c]);instdef[j].polygonptr[c]=NULL;}

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
   if(instdef[j].txtptr[i].font != NULL)
   {
    my_free(instdef[j].txtptr[i].font);
    instdef[j].txtptr[i].font=NULL;
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
  if(debug_var>=1) fprintf(errfp, "remove_symbols(): done\n");
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
  if(textelement[i].font!=NULL) 
    {my_free(textelement[i].font);textelement[i].font=NULL;}
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
  for(j=0;j<lastpolygon[i]; j++) {// 20171115
    if(polygon[i][j].x!=NULL) { my_free(polygon[i][j].x); polygon[i][j].x=NULL;}
    if(polygon[i][j].y!=NULL) { my_free(polygon[i][j].y); polygon[i][j].y=NULL;}
    if(polygon[i][j].prop_ptr!=NULL) { my_free(polygon[i][j].prop_ptr); polygon[i][j].prop_ptr=NULL;}
    if(polygon[i][j].selected_point!=NULL) { 
      my_free(polygon[i][j].selected_point); 
      polygon[i][j].selected_point=NULL;
    }
  }
  lastline[i] = 0;
  lastrect[i] = 0;
  lastpolygon[i] = 0;// 20171115
 }
 if(debug_var>=1) fprintf(errfp, "clear drawing(): deleted data structures, now deleting hash\n");
 free_hash();
}

void attach_labels_to_inst() // offloaded from callback.c 20171005
{
  Instdef *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0; 
  int flip, rot, rot1 ; // 20101129 
  Box *rect;
  static char *labname=NULL;
  static char *prop=NULL; // 20161122 overflow safe
  char symname_pin[] = "devices/lab_pin";
  char symname_wire[] = "devices/lab_wire"; // 20171005
  static char *type=NULL;
  int dir;
  int k,ii, skip;
  int do_all_inst=0; // 20171206
  const char *rot_txt;
  int rotated_text=-1; // 20171208

  struct wireentry *wptr; // 20171214
  struct instpinentry *iptr;
  int sqx, sqy;
  int first_call;

  rebuild_selected_array();
  k = lastselected;
  first_call=1; // 20171214 for place_symbol--> new_prop_string
  prepare_netlist_structs(1);
  if(k) push_undo(); // 20150327
  bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
  for(j=0;j<k;j++) if(selectedgroup[j].type==ELEMENT) {


    my_strdup(&prop, inst_ptr[selectedgroup[j].n].instname);
    my_strcat(&prop, "_");
    Tcl_SetVar(interp,"custom_label_prefix", prop, TCL_GLOBAL_ONLY);
    // 20171005
    if(!do_all_inst) {
      tkeval("attach_labels_to_inst");
      if(!strcmp(Tcl_GetVar(interp, "rcode", TCL_GLOBAL_ONLY),"") ) {
        bbox(END, 0., 0., 0., 0.);
        return;
      }
    }

    rot_txt = Tcl_GetVar(interp,"rotated_text",TCL_GLOBAL_ONLY);
    if(strcmp(rot_txt,"")) rotated_text=atoi(rot_txt); // 20171208

    // 20111030 skip labels / pins
    // my_strdup(&type,get_tok_value((inst_ptr[selectedgroup[j].n].ptr+instdef)->prop_ptr,"type",0)); // 20150409
    my_strdup(&type,(inst_ptr[selectedgroup[j].n].ptr+instdef)->type); // 20150409
    if( type && (strcmp(type,"label") && strcmp(type,"ipin")&&strcmp(type,"opin")&&strcmp(type,"iopin") )==0)
      continue;
    // /20111030

    if(!do_all_inst && !strcmp(Tcl_GetVar(interp,"do_all_inst",TCL_GLOBAL_ONLY),"1")) do_all_inst=1; // 20171206

    if(debug_var>=1) fprintf(errfp, " 200711 1--> %s %.16g %.16g   %s\n", 
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
       my_strdup(&labname,get_tok_value(rect[i].prop_ptr,"name",0));
       if(debug_var>=1) fprintf(errfp,"200711 2 --> labname=%s\n", labname);
       
       pinx0 = (rect[i].x1+rect[i].x2)/2;
       piny0 = (rect[i].y1+rect[i].y2)/2;

       if(strcmp(get_tok_value(rect[i].prop_ptr,"dir",0),"in")) dir=1; // out or inout pin
       else dir=0; // input pin

       // opin or iopin on left of symbol--> reverse orientation 20171205
       if(rotated_text ==-1 && dir==1 && pinx0<0) dir=0;
      
       ROTATION(0.0, 0.0, pinx0, piny0, pinx0, piny0); // 20101129
 
       pinx0 += x0;
       piny0 += y0;

       get_square(pinx0, piny0, &sqx, &sqy);
       iptr=instpintable[sqx][sqy];
       wptr=wiretable[sqx][sqy];

       skip=0;
       while(iptr) {
         ii = iptr->n;
         if(ii == selectedgroup[j].n) {
           iptr = iptr->next;
           continue;
         }
  
         if( iptr->x0 == pinx0 && iptr->y0 == piny0 ) {
           skip=1;
           break;
         }
         iptr = iptr->next;
       }
       while(wptr) {
         if( touch(wire[wptr->n].x1, wire[wptr->n].y1,
             wire[wptr->n].x2, wire[wptr->n].y2, pinx0, piny0) ) {
           skip=1;
           break;
         }
         wptr = wptr->next;
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
         if(rotated_text ==-1) {
           rot1=rot;
           if(rot1==1 || rot1==2) { dir=!dir;rot1 = (rot1+2) %4;}  // 20111103
         } else {
           rot1=(rot+rotated_text)%4; // 20111103 // 20171208 text_rotation
         }
         if(!strcmp(Tcl_GetVar(interp,"use_lab_wire",TCL_GLOBAL_ONLY),"0")) {
           place_symbol(-1,symname_pin, pinx0, piny0, rot1, dir, prop, 0, first_call);
           first_call=0;
         } else {
           place_symbol(-1,symname_wire, pinx0, piny0, rot1, dir, prop, 0, first_call);
           first_call=0;
         }


       }
       if(debug_var>=1) fprintf(errfp, "%d   %.16g %.16g %s\n", i, pinx0, piny0,labname);
    }
  }
  // draw things 
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
}

// draw_sym==3 start bbox end bbox, draw placed symbols 
// draw_sym==1 end bbox, draw placed symbols 
// draw_sym==2 start  bbox
// draw_sym==0 dont draw
//
// first_call: set to 1 on first invocation for a given set of symbols (same prefix)
// set to 0 on next calls, this speeds up searching for unique names in prop string
void place_symbol(int pos,char *symbol_name, double x, double y, int rot, int flip, 
                   char *inst_props, int draw_sym, int first_call)
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
    new_prop_string(&inst_ptr[n].prop_ptr, inst_props,!first_call); // 20171214 first_call
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
  modified=1;
  prepared_hash_objects=0; // 20171224
  prepared_netlist_structs=0;
  prepared_hilight_structs=0;
  if(draw_sym&1) {
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
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
    my_strncpy(schematic[currentsch], Tcl_GetStringResult(interp), S(schematic[currentsch]));
    if(!strcmp(schematic[currentsch],"")) return;
    save_ok = save_schematic(schematic[currentsch]);

    if(save_ok==-1) return; // 20171020
  }

  if(debug_var>0) fprintf(errfp, "type of instance: %s\n", (inst_ptr[selectedgroup[0].n].ptr+instdef)->type);

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

  my_strncpy(schematic[currentsch+1],inst_ptr[selectedgroup[0].n].name, S(schematic[currentsch+1]));
  //clear_drawing();
  previous_instance[currentsch]=selectedgroup[0].n;
  zoom_array[currentsch].x=xorigin;
  zoom_array[currentsch].y=yorigin;
  zoom_array[currentsch].zoom=zoom;
  hilight_child_pins(previous_instance[currentsch]);
  currentsch++;
  unselect_all();
  remove_symbols();
  load_schematic(1,NULL,1);
  if(hilight_nets) 
  {
    prepare_netlist_structs(1);
    if(enable_drill) drill_hilight(); // 20171212
  }
  if(debug_var>0) fprintf(errfp, "descend(): before zoom(): prepared_hash_objects=%d\n", prepared_hash_objects);
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
       if(!strcmp(Tcl_GetStringResult(interp), "yes") ) save_ok = save_schematic(NULL);
       else if(!strcmp(Tcl_GetStringResult(interp), "") ) return;
     } else {
       save_ok = save_schematic(NULL);
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
  my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
  currentsch--;
  remove_symbols();
  load_schematic(1,NULL,1);
  unselect_all();

  if(prev_curr_type==SCHEMATIC) {
    hilight_parent_pins();
  }
  xorigin=zoom_array[currentsch].x;
  yorigin=zoom_array[currentsch].y;
  zoom=zoom_array[currentsch].zoom;
  mooz=1/zoom;

  set_linewidth();
  // resetwin();
  draw();

  current_type=SCHEMATIC;
  if(debug_var>=1) fprintf(errfp, "go_back(): current path: %s\n", sch_prefix[currentsch]);
 }
}

void set_linewidth()
{
 int i,j;
   if(change_lw)  {
     lw_double=1/zoom*1.5;		// on some servers zero width
     //if(lw_double > 50.) lw_double=50; // clamp max width?
   }
    
   lw=j=lw_double;			// draws fast but not good...
   if(lw==0) lw=1;			// lw used to calculate bound boxes
   if(debug_var>=1) fprintf(errfp, "set_linewidth(): lw=%d, lw_double=%.16g\n", lw, lw_double);
   if(has_x) {
      for(i=0;i<cadlayers;i++) {
          XSetLineAttributes (display, gc[i], j, LineSolid, CapRound , JoinRound);
      }
      XSetLineAttributes (display, gctiled, j, LineSolid, CapRound , JoinRound); // j as linewidth 20171105
   }
   areax1 = -2*lw;
   areay1 = -2*lw;
   areax2 = xrect[0].width+2*lw;
   areay2 = xrect[0].height+2*lw;
   areaw = areax2-areax1;
   areah = areay2 - areay1;
}

void calc_drawing_bbox(Box *boundbox)
{

 Box tmp;
 int c, i; 
 int count=0;
 #ifdef HAS_CAIRO
 int customfont;
 #endif

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

  for(i=0;i<lastpolygon[c];i++) // 20171115
  {
    double x1=0., y1=0., x2=0., y2=0.;
    int k;
    count++;
    for(k=0; k<polygon[c][i].points; k++) {
      //fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]);
      if(k==0 || polygon[c][i].x[k] < x1) x1 = polygon[c][i].x[k];
      if(k==0 || polygon[c][i].y[k] < y1) y1 = polygon[c][i].y[k];
      if(k==0 || polygon[c][i].x[k] > x2) x2 = polygon[c][i].x[k];
      if(k==0 || polygon[c][i].y[k] > y2) y2 = polygon[c][i].y[k];
    }
    tmp.x1=x1;tmp.y1=y1;tmp.x2=x2;tmp.y2=y2;
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
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&textelement[i]);
   #endif
   if(text_bbox(textelement[i].txt_ptr, textelement[i].xscale,
         textelement[i].yscale,textelement[i].rot, textelement[i].flip,
         textelement[i].x0, textelement[i].y0,
         &tmp.x1,&tmp.y1, &tmp.x2,&tmp.y2) ) {
     count++;
     updatebbox(count,boundbox,&tmp);
   }
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(ctx);
   #endif
 }
 for(i=0;i<lastinst;i++)
 {
  // symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2); // cpu hog 20171206
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

  if(!has_x) return;
  if(change_lw) lw = lw_double=1.;
  areax1 = -2*lw;
  areay1 = -2*lw;
  areax2 = xrect[0].width+2*lw;
  areay2 = xrect[0].height+2*lw;
  areaw = areax2-areax1;
  areah = areay2 - areay1;

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
   if(change_lw) set_linewidth();

   draw(); // 20121111
  }
}


void view_unzoom(double z)
{
    double factor;
    // int i;
    factor = z!=0.0 ? z : CADZOOMSTEP;
    if(zoom<CADMINZOOM) return;
    zoom/= factor;
    mooz=1/zoom;
    xorigin=-mousex_snap+(mousex_snap+xorigin)/factor;
    yorigin=-mousey_snap+(mousey_snap+yorigin)/factor;
    set_linewidth();
    draw();
}

void view_zoom(double z)
{
  double factor;
  // int i;
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(zoom>CADMAXZOOM) return;
  zoom*= factor;
  mooz=1/zoom;
  xorigin=xorigin+areaw*zoom*(1-1/factor)/2;
  yorigin=yorigin+areah*zoom*(1-1/factor)/2;
  set_linewidth();
  //// performance hit 20171130
  // for(i=0;i<lastinst;i++) { // 20171127
  //   if(inst_ptr[i].ptr <0) continue;
  //   symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1,
  //                     &inst_ptr[i].x2, &inst_ptr[i].y2);
  // }
  // resetwin();
  draw();
}

void zoom_box(int what)
{
  static double x1,y1,x2,y2;
  static double xx1,yy1,xx2,yy2;

  if( (what & BEGIN) )
  {
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    ui_state |= STARTZOOM;
  }
  if( what & END)
  {
    ui_state &= ~STARTZOOM;
    RECTORDER(x1,y1,x2,y2);
    drawtemprect(gctiled, NOW, xx1,yy1,xx2,yy2);
    xorigin=-x1;yorigin=-y1;
    zoom=(x2-x1)/(areaw-4*lw);
    yy1=(y2-y1)/(areah-4*lw);
    if(yy1>zoom) zoom=yy1;
    mooz=1/zoom;
    set_linewidth();
    // resetwin();
    draw();
    if(debug_var>=1) fprintf(errfp, "zoom_box(): coord: %.16g %.16g %.16g %.16g zoom=%.16g\n",x1,y1,mousex_snap, mousey_snap,zoom);
  }
  if(what & RUBBER)
  {
    xx1=x1;yy1=y1;xx2=x2;yy2=y2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    x2=mousex_snap;y2=mousey_snap;


    // 20171211 update selected objects while dragging
    rebuild_selected_array();
    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xx1, yy1, xx2, yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    bbox(END,0.0, 0.0, 0.0, 0.0);

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
     if( (x1!=x2 || y1!=y2) && (ui_state & STARTWIRE) ) {
       ORDER(x1,y1,x2,y2);
       push_undo();
       storeobject(-1, x1,y1,x2,y2,WIRE,0,0,NULL);
       drawline(WIRELAYER,NOW, x1,y1,x2,y2);
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

     ui_state |= STARTWIRE;
   }
   if( what & END) {
     ui_state &= ~STARTWIRE;
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
     if(type==POLYGON && polygon[c][n].sel==SELECTED) {
        store_polygon(-1, polygon[c][n].x, polygon[c][n].y, polygon[c][n].points, rectcolor, 0, polygon[c][n].prop_ptr);
     }
     else if(type==RECT && rect[c][n].sel==SELECTED) {
       x1 = rect[c][n].x1;
       y1 = rect[c][n].y1;
       x2 = rect[c][n].x2;
       y2 = rect[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,RECT,rectcolor, 0, rect[c][n].prop_ptr);
     }
   }
   if(lastselected) delete_only_rect_and_line_and_poly();
   unselect_all();
}

void new_line(int what)
{
 static double x1,y1,x2,y2;
 static double xx1,yy1,xx2,yy2;

   if( (what & PLACE) )
   {
    if( (x1!=x2 || y1!=y2) && (ui_state & STARTLINE) )
    {
     ORDER(x1,y1,x2,y2);
     push_undo();
     drawline(rectcolor, NOW, x1,y1,x2,y2);
     storeobject(-1, x1,y1,x2,y2,LINE,rectcolor, 0, NULL);
    }
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    ui_state |= STARTLINE;
   }
   if( what & END)
   {
    ui_state &= ~STARTLINE;
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
    if( (x1!=x2 || y1!=y2) && (ui_state & STARTRECT) )
    {
     RECTORDER(x1,y1,x2,y2); 
     push_undo();
     drawrect(rectcolor, NOW, x1,y1,x2,y2);
     filledrect(rectcolor, NOW, x1,y1,x2,y2);
     storeobject(-1, x1,y1,x2,y2,RECT,rectcolor, 0, NULL);
    }
    x1=x2=mousex_snap;y1=y2=mousey_snap;
    ui_state |= STARTRECT;
   }
   if( what & END)
   {
    ui_state &= ~STARTRECT;
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


void new_polygon(int what) // 20171115
{
 static double *x=NULL, *y=NULL;
 static int points=0;
 static int maxpoints=0;

   if( what & PLACE ) points=0; // start new polygon placement

   if(points >= maxpoints-1) {	// check storage for 2 points
     maxpoints = (1+points / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(&x, sizeof(double)*maxpoints);
     my_realloc(&y, sizeof(double)*maxpoints);
   }

   if( what & PLACE )
   {
     //fprintf(errfp, "new_poly: PLACE, points=%d\n", points);
     y[points]=mousey_snap;
     x[points]=mousex_snap;
     points++;
     x[points]=x[points-1];
     y[points] = y[points-1];
     //fprintf(errfp, "added point: %.16g %.16g\n", x[points-1], y[points-1]);
     ui_state |= STARTPOLYGON;
   }
   if( what & ADD)
   {
     if(what & END) {
       drawtemppolygon(gctiled, NOW, x, y, points+1);
       x[points] = x[0]; 
       y[points] = y[0]; // close the polygon path by user request
     } else {
       x[points] = mousex_snap;
       y[points] = mousey_snap;
     }
     points++;
     //fprintf(errfp, "added point: %.16g %.16g\n", x[points-1], y[points-1]);
     x[points]=x[points-1];y[points]=y[points-1]; // prepare next point for rubber
     //fprintf(errfp, "new_poly: ADD, points=%d\n", points);
     if( x[points-1] == x[0] && y[points-1] == y[0]) { // closed polygon --> END
       push_undo();
       store_polygon(-1, x, y, points, rectcolor, 0, NULL);
       //fprintf(errfp, "new_poly: finish: points=%d\n", points);
       ui_state &= ~STARTPOLYGON;
       drawpolygon(rectcolor, NOW, x, y, points);
     }
   }
   if(what & RUBBER)
   {
     //fprintf(errfp, "new_poly: RUBBER\n");
     drawtemppolygon(gctiled, NOW, x, y, points+1);
     y[points] = mousey_snap;
     x[points] = mousex_snap;
     drawtemppolygon(gc[rectcolor], NOW, x, y, points+1);
   }
}

#ifdef HAS_CAIRO
int text_bbox(char *str, double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
{
  int c=0, length =0;
  char *str_ptr;
  double size;
  cairo_text_extents_t ext;
  cairo_font_extents_t fext;
  double ww, hh;

  if(!has_x) return 0;
  if(!str) return 0;
  size = (xscale+yscale)*26.*cairo_font_scale;
  if(size*mooz>800.) {
    return 0;
  }
  cairo_set_font_size (ctx, size*mooz);
  cairo_font_extents(ctx, &fext);

  ww=0.; hh=1.;
  c=0;
  cairo_longest_line=0;
  cairo_lines=1;
  str_ptr = str;
  while( str[c] ) {
    if(str[c] == '\n') {
      str[c]='\0';
      hh++;
      cairo_lines++;
      length=0;
      if(str_ptr[0]!='\0') {
        cairo_text_extents(ctx, str_ptr, &ext);
        if(ext.x_advance > ww) ww= ext.x_advance;
      }
      str[c]='\n';
      str_ptr = str+c+1;
    } else {
      length++;
    }
    if(length > cairo_longest_line) {
      cairo_longest_line = length;
    }
    c++;
  }
  if(str_ptr[0]!='\0') {
    cairo_text_extents(ctx, str_ptr, &ext);
    if(ext.x_advance > ww) ww= ext.x_advance;
  }
  hh = hh*fext.height*cairo_font_line_spacing;
  cairo_longest_line = ww;

  *rx1=x1;*ry1=y1; 
  ROTATION(0.0,0.0, ww*zoom,hh*zoom,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  if     (rot==0) {*ry1-=cairo_vert_correct; *ry2-=cairo_vert_correct;}
  else if(rot==1) {*rx1+=cairo_vert_correct; *rx2+=cairo_vert_correct;}
  else if(rot==2) {*ry1+=cairo_vert_correct; *ry2+=cairo_vert_correct;}
  else if(rot==3) {*rx1-=cairo_vert_correct; *rx2-=cairo_vert_correct;}
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
  return 1;
}
int text_bbox_nocairo(char * str,double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
#else
int text_bbox(char * str,double xscale, double yscale,
    int rot, int flip, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2)
#endif
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
  return 1;
}

void place_text(int draw_text, double mx, double my)
{
  char *txt;
  int textlayer;
  char *strlayer;

  // 20171112
  #ifdef HAS_CAIRO
  char  *textfont;
  #endif

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
  textelement[lasttext].font=NULL;  // 20171206
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
   my_strdup(&textelement[lasttext].prop_ptr, (char *)Tcl_GetVar(interp,"props",TCL_GLOBAL_ONLY));
  // debug ...
  // textelement[lasttext].prop_ptr=NULL;
  if(debug_var>=1) fprintf(errfp, "place_text(): done text input\n");
  strlayer = get_tok_value(textelement[lasttext].prop_ptr, "layer", 0);
  if(strlayer[0]) textelement[lasttext].layer = atoi(strlayer);
  else textelement[lasttext].layer = -1;
  my_strdup(&textelement[lasttext].font, get_tok_value(textelement[lasttext].prop_ptr, "font", 0));//20171206
  textlayer = TEXTLAYER;
  #ifdef HAS_CAIRO
  textlayer = textelement[lasttext].layer;
  if(textlayer < 0 || textlayer >= cadlayers) textlayer = TEXTLAYER;
  textfont = textelement[lasttext].font;
  if(textfont && textfont[0]) {
    cairo_save(ctx);
    cairo_save(save_ctx);
    cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_select_font_face (save_ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  }
  #endif
  if(draw_text) draw_string(textlayer, NOW, textelement[lasttext].txt_ptr, 0, 0, 
              textelement[lasttext].x0,textelement[lasttext].y0,
              textelement[lasttext].xscale, textelement[lasttext].yscale);
  #ifdef HAS_CAIRO
  if(textfont && textfont[0]) {
    cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_restore(ctx);
    cairo_restore(save_ctx);
  }
  #endif
  if(x_initialized) drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  if(x_initialized) drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
  select_text(lasttext, SELECTED, 0);
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
    ui_state |= STARTPAN;
    xpan=mousex_snap;ypan=mousey_snap;xpan2=xpan;ypan2=ypan;
 }
 if(what & END)
 {
    ui_state &= ~STARTPAN;
    xorigin+=-xpan+mousex_snap;yorigin+=-ypan+mousey_snap;
    draw();
 }
}

// 20150927 select=1: select objects, select=0: unselect objects
void select_rect(int what, int select)
{
 static double xr,yr,xr2,yr2;
 static double xx1,xx2,yy1,yy2;
 static int sel;
 static int semaphore=0; // 20171130

 if(what & RUBBER)
 {
    if(semaphore==0) {
      fprintf(errfp, "ERROR: select_rect() RUBBER called before BEGIN\n");
      tkeval("alert_ {ERROR: select_rect() RUBBER called before BEGIN} {}"); // 20171020
    }
    xx1=xr;xx2=xr2;yy1=yr;yy2=yr2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gctiled,NOW, xx1,yy1,xx2,yy2);
    xr2=mousex_snap;yr2=mousey_snap;

    // 20171026 update unselected objects while dragging
    rebuild_selected_array();
    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xx1, yy1, xx2, yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    if(!sel) select_inside(xx1, yy1, xx2, yy2, sel);
    bbox(END,0.0, 0.0, 0.0, 0.0);
    xx1=xr;xx2=xr2;yy1=yr;yy2=yr2;
    RECTORDER(xx1,yy1,xx2,yy2);
    drawtemprect(gc[SELLAYER],NOW, xx1,yy1,xx2,yy2);
 }
 else if(what & BEGIN)
 {
    if(semaphore==1) {
      fprintf(errfp, "ERROR: reentrant call of select_rect()\n");
      tkeval("alert_ {ERROR: reentrant call of select_rect()} {}"); // 20171020
    }
    sel = select; // 20150927
    ui_state |= STARTSELECT;

    // use m[xy]_double_save instead of mouse[xy]_snap
    // to avoid delays in setting the start point of a
    // selection rectangle, this is noticeable and annoying on
    // networked / slow X servers. 20171218
    //xr=xr2=mousex_snap;
    //yr=yr2=mousey_snap;
    xr=xr2=mx_double_save;
    yr=yr2=my_double_save;
    semaphore=1;
 }
 else if(what & END)
 {
    RECTORDER(xr,yr,xr2,yr2);
    drawtemprect(gctiled, NOW, xr,yr,xr2,yr2);
    // draw_selection(gc[SELLAYER], 0);
    select_inside(xr,yr,xr2,yr2, sel);

    // 20171219
    bbox(BEGIN,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xr, yr, xr2, yr2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(gc[SELLAYER], 0);
    bbox(END,0.0, 0.0, 0.0, 0.0);
    // /20171219

    ui_state &= ~STARTSELECT;
    semaphore=0;
 }
}

