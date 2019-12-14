/* File: scheduler.c
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2019 Stefan Frederik Schippers
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

void statusmsg(char str[],int n)
{
 static char s[2048];

 if(!has_x) return;
 tclsetvar("infowindow_text", str);
 if(n==2)
 {
  if(debug_var>=3) fprintf(errfp, "statusmsg(): n = 2, s = %s\n", s);
  tcleval("infowindow");
 }
 else
 {
  my_snprintf(s, S(s), ".statusbar.1 configure -text $infowindow_text", str);
  if(debug_var>=3) fprintf(errfp, "statusmsg(): n = %d, %s\n", n, s);
  if(debug_var>=3) fprintf(errfp, "           --> $infowindow_text = %s\n", tclgetvar("infowindow_text"));

  tcleval(s);  
 }
}

int get_instance(const char *s)
{
     int i, found=0;
     for(i=0;i<lastinst;i++) {
       if(!strcmp(inst_ptr[i].instname, s)) {
         found=1;
         break;
       }
     }
     if(debug_var>=1) fprintf(errfp, "get_instance(): found=%d, i=%d\n", found, i);
     if(!found) {
       if(!isonlydigit(s)) return -1;
       i=atol(s);
     }
     if(i<0 || i>lastinst) {
       Tcl_AppendResult(interp, "Index out of range", NULL);
       return -1;
     }
     return i;
}


/* can be used to reach C functions from the Tk shell. */
int xschem(ClientData clientdata, Tcl_Interp *interp, int argc, const char * argv[])
{       
 int i;
 char name[1024]; /* overflow safe 20161122 */
 if(argc<2) return TCL_ERROR;
 if(debug_var>=2) {
   int i;
   fprintf(errfp, "xschem():");
   for(i=0; i<argc; i++) {
     fprintf(errfp, "%s ", argv[i]);
   } 
   fprintf(errfp, "\n");
 }
 Tcl_ResetResult(interp);

 /*
  * ********** xschem commands
  */

 if(!strcmp(argv[1],"callback") )
 {
  callback( atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), (KeySym)atol(argv[5]),
            atoi(argv[6]), atoi(argv[7]), atoi(argv[8]) );
 }

 else if(!strcmp(argv[1],"set_netlist_dir") && argc==3) {
     if(debug_var>=1) fprintf(errfp, "xschem set_netlist_dir: argv[2] = %s\n", argv[2]);
     my_strdup(0, &netlist_dir, argv[2]);
 }

 else if(!strcmp(argv[1],"copy"))
 {
    rebuild_selected_array();
    save_selection(2);
 }

 else if(!strcmp(argv[1],"change_colors"))
 {
   build_colors(color_dim);
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"toggle_colorscheme"))
 {
   dark_colorscheme=!dark_colorscheme;
   tclsetvar("dark_colorscheme", dark_colorscheme ? "1" : "0");
   color_dim=0.0;
   build_colors(color_dim);
   draw();
   Tcl_ResetResult(interp);

 }

 else if(!strcmp(argv[1],"color_dim"))
 {
   double d;

   if(argc==3) {
     d = atof(argv[2]);
     build_colors(d);
     color_dim = d;
     draw();
     Tcl_ResetResult(interp);
   }
 }

 else if(!strcmp(argv[1],"cut"))
 {
    rebuild_selected_array();
    save_selection(2);
    delete();
    Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"only_probes")) { /* 20110112 */
   toggle_only_probes();
 }

 else if(!strcmp(argv[1],"fullscreen"))
 {
   if(debug_var>=1) fprintf(errfp, "scheduler: xschem fullscreen, fullscreen=%d\n", fullscreen);
   if(fullscreen>=1) fullscreen=2;
   toggle_fullscreen();
 }

 else if(!strcmp(argv[1],"windowid")) /* used by xschem.tcl for configure events */
 {
   windowid();
 }

 else if(!strcmp(argv[1],"preview_window"))
 {
   if(argc == 3) preview_window(argv[2], "{}","{}");
   else if(argc == 5) preview_window(argv[2], argv[3], argv[4]);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"paste"))
 {
    merge_file(2,".sch");
 }

 else if(!strcmp(argv[1],"merge"))
 {
    if(argc<3) {
      merge_file(0,"");  /* 2nd param not used for merge 25122002 */
    }
    else {                      /* 20071215 */
      merge_file(0,argv[2]);
    }
 }
 else if(!strcmp(argv[1],"attach_pins")) /* attach pins to selected component 20171005 */
 {
   attach_labels_to_inst();
 }
 else if(!strcmp(argv[1],"make_sch")) /* make schematic from selected symbol 20171004 */
 {
   create_sch_from_sym();
 }
 else if(!strcmp(argv[1],"make_symbol"))
 {
   tcleval("tk_messageBox -type okcancel -message {do you want to make symbol view ?}");
   if(strcmp(Tcl_GetStringResult(interp),"ok")==0) 
      if(current_type==SCHEMATIC)
      {
       save_schematic(schematic[currentsch]);
       make_symbol();
      }
 }

 else if(!strcmp(argv[1],"descend"))
 {
    descend_schematic();
 }

 else if(!strcmp(argv[1],"descend_symbol"))
 {
    descend_symbol();
 }

 else if(!strcmp(argv[1],"go_back"))
 {
    go_back(1);
 }

 else if(!strcmp(argv[1],"zoom_full"))
 {
    zoom_full(1, 0);
 }

 else if(!strcmp(argv[1],"zoom_selected"))
 {
    zoom_full(1, 1);
 }

 else if(!strcmp(argv[1],"zoom_hilighted"))
 {
    zoom_full(1, 2);
 }

 else if(!strcmp(argv[1],"zoom_box"))
 {
    double x1, y1, x2, y2, yy1, factor;
    if(debug_var>=1) fprintf(errfp, "xschem zoom_box: argc=%d, argv[2]=%s\n", argc, argv[2]);
    if(argc==6 || argc == 7) {
      x1 = atof(argv[2]);
      y1 = atof(argv[3]);
      x2 = atof(argv[4]);
      y2 = atof(argv[5]);
      if(argc == 7) factor = atof(argv[6]);
      else          factor = 1.;
      if(factor == 0.) factor = 1.;
      RECTORDER(x1,y1,x2,y2);
      xorigin=-x1;yorigin=-y1;
      zoom=(x2-x1)/(areaw-4*lw);
      yy1=(y2-y1)/(areah-4*lw);
      if(yy1>zoom) zoom=yy1;
      mooz=1/zoom;
      xorigin=xorigin+areaw*zoom*(1-1/factor)/2;
      yorigin=yorigin+areah*zoom*(1-1/factor)/2;
      zoom*= factor;
      mooz=1/zoom;

      change_linewidth(-1.);
      draw();
    }

 }

 else if(!strcmp(argv[1],"place_symbol"))
 {
   int ret;
   semaphore++;
   mx_double_save = mousex_snap;
   my_double_save = mousey_snap;
   if(argc == 4) {
     ret = place_symbol(-1,argv[2],mousex_snap, mousey_snap, 0, 0, argv[3], 4, 1);
   } else if(argc == 3) {
     ret = place_symbol(-1,argv[2],mousex_snap, mousey_snap, 0, 0, NULL, 4, 1);
   } else {
     ret = place_symbol(-1,NULL,mousex_snap, mousey_snap, 0, 0, NULL, 4, 1);
   }

   if(ret) {
     mousey_snap = my_double_save;
     mousex_snap = mx_double_save;
     move_objects(BEGIN,0,0,0);
     ui_state |= PLACE_SYMBOL;
   }

   semaphore--;
 }

 else if(!strcmp(argv[1],"place_text"))
 {
   ui_state |= MENUSTARTTEXT; /* 20161201 */
   /* place_text(0,mousex_snap, mousey_snap); */
   /* move_objects(BEGIN,0,0,0); */
 }

 else if(!strcmp(argv[1],"zoom_out"))
 {
    view_unzoom(0.0);
 }

 else if(!strcmp(argv[1],"line_width") && argc==3)
 {
    change_linewidth(atof(argv[2]));
 }

 else if(!strcmp(argv[1],"sleep") && argc==3)
 {
    /* usleep(atoi(argv[2])*1000); */ /*portability issues */
    
 }

 else if(!strcmp(argv[1],"select_all"))
 {
    select_all();
    Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"zoom_in"))
 {
    view_zoom(0.0);
    Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"copy_objects"))
 {
   copy_objects(BEGIN); 
    Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"move_objects"))
 {
   if(argc==4) {
     move_objects(BEGIN,0,0,0);
     move_objects( END,0,atof(argv[2]), atof(argv[3]));
   }
   else move_objects(BEGIN,0,0,0); 
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"log"))
 {
  static int opened=0;
  if(argc==3 && opened==0  )  { errfp = fopen(argv[2], "w");opened=1; } /* added check to avoid multiple open 07102004 */
  else if(argc==2 && opened==1) { fclose(errfp); errfp=stderr;opened=0; }
 }

 else if(!strcmp(argv[1],"exit"))
 {
  if(modified) {
    tcleval("tk_messageBox -type okcancel -message {UNSAVED data: want to exit?}");
    if(strcmp(Tcl_GetStringResult(interp),"ok")==0) tcleval( "exit");
  }
  else tcleval( "exit");
 }
 else if(!strcmp(argv[1], "rebuild_connectivity")) {
     prepared_hash_instances=0;
     prepared_hash_wires=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
     prepare_netlist_structs(1);
 }
 else if(!strcmp(argv[1],"clear"))
 {  
  int cancel;

  cancel=save(1);
  if(!cancel){  /* 20161209 */
     currentsch = 0;
     unselect_all(); /* 20180929 */
     remove_symbols();
     clear_drawing();
     if(argc>=3 && !strcmp(argv[2],"SYMBOL")) { /* 20171025 */
       my_strncpy(schematic[currentsch], "untitled.sym", S(schematic[currentsch]));
       current_type=SYMBOL;
     } else {
       my_strncpy(schematic[currentsch], "untitled.sch", S(schematic[currentsch]));
       current_type=SCHEMATIC;
     }
     draw();
     set_modify(0); /* 20171025 */
     prepared_hash_instances=0;
     prepared_hash_wires=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
     if(has_x) {
       tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\""); /* 20150417 set window and icon title */
       tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\"");
     }
  }
 }

 else if(!strcmp(argv[1],"set_modify"))
 {
   set_modify(1);
 }

 else if(!strcmp(argv[1],"reload"))
 {
   unselect_all(); /* 20180929 */
   remove_symbols();
   load_schematic(0, 1, schematic[currentsch], 1);
   zoom_full(1, 0);
 }
 
 else if(!strcmp(argv[1],"debug"))
 {
  if(argc==3)  {
     debug_var=atoi(argv[2]);
     tclsetvar("tcl_debug",argv[2]);
  }

 } else if(!strcmp(argv[1], "bbox")) {
   if(argc == 3) {
     if(!strcmp(argv[2], "end")) {
       bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
       draw();
       bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
     } else if(!strcmp(argv[2], "begin")) {
       bbox(BEGIN,0.0, 0.0, 0.0, 0.0); 
     }
   }
 } else if(!strcmp(argv[1], "setprop")) {
   int inst, fast=0;

   if(argc >= 6) {
     if(!strcmp(argv[5], "fast")) {
       fast = 1;
       argc = 5;
     }
   }
   else if(argc >= 5) {
     if(!strcmp(argv[4], "fast")) {
       fast = 1;
       argc = 4;
     }
   }

   if(argc < 4) {
     Tcl_AppendResult(interp, "xschem setprop needs 2 or 3 additional arguments", NULL);
     return TCL_ERROR;
   }
   if( (inst = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem setprop: instance not found", NULL);
     return TCL_ERROR;
   } else {
     bbox(BEGIN,0.0,0.0,0.0,0.0);
     symbol_bbox(inst, &inst_ptr[inst].x1, &inst_ptr[inst].y1, &inst_ptr[inst].x2, &inst_ptr[inst].y2);
     bbox(ADD, inst_ptr[inst].x1, inst_ptr[inst].y1, inst_ptr[inst].x2, inst_ptr[inst].y2);
     push_undo();
     set_modify(1);
     if(!fast) {
       prepared_hash_instances=0;
       prepared_netlist_structs=0;
       prepared_hilight_structs=0;
     }
     hash_all_names(inst);
     if(argc >= 5) {
       new_prop_string(inst, subst_token(inst_ptr[inst].prop_ptr, argv[3], argv[4]),0, disable_unique_names); 
   
     } else {/* assume argc == 4 */
       new_prop_string(inst, subst_token(inst_ptr[inst].prop_ptr, argv[3], NULL),0, disable_unique_names); 
     }
     my_strdup2(367, &inst_ptr[inst].instname, get_tok_value(inst_ptr[inst].prop_ptr, "name",0));
     /* new symbol bbox after prop changes (may change due to text length) */
     symbol_bbox(inst, &inst_ptr[inst].x1, &inst_ptr[inst].y1, &inst_ptr[inst].x2, &inst_ptr[inst].y2);
     bbox(ADD, inst_ptr[inst].x1, inst_ptr[inst].y1, inst_ptr[inst].x2, inst_ptr[inst].y2);
     /* redraw symbol with new props */
     bbox(SET,0.0,0.0,0.0,0.0);
     draw();
     bbox(END,0.0,0.0,0.0,0.0);
   }
   Tcl_ResetResult(interp);

 } else if(!strcmp(argv[1], "replace_symbol")) {
   int inst, fast = 0;
   if(argc == 6) {
     if(!strcmp(argv[5], "fast")) {
       fast = 1;
       argc = 5;
     }
   }
   if(argc!=5) {
     Tcl_AppendResult(interp, "xschem replace_symbol needs 3 additional arguments", NULL);
     return TCL_ERROR;
   }
   if(!strcmp(argv[2],"instance")) {
     if( (inst = get_instance(argv[3])) < 0 ) {
       Tcl_AppendResult(interp, "xschem replace_symbol: instance not found", NULL);
       return TCL_ERROR;
     } else {
       char symbol[PATH_MAX];
       int sym_number, prefix, cond;
       char *type;
       static char *name=NULL;
       static char *ptr=NULL;
       static char *template=NULL;
  
       bbox(BEGIN,0.0,0.0,0.0,0.0);
       my_strncpy(symbol, argv[4], S(symbol));
       push_undo();
       set_modify(1);
       if(!fast) {
         prepared_hash_instances=0; /* 20171224 */
         prepared_netlist_structs=0;
         prepared_hilight_structs=0;
       }
       sym_number=match_symbol(symbol);
       if(sym_number>=0)
       {
         my_strdup(368, &template, (instdef+sym_number)->templ); /* 20150409 */
         prefix=(get_tok_value(template, "name",0))[0]; /* get new symbol prefix  */
       }
       else prefix = 'x';
       delete_inst_node(inst); /* 20180208 fix crashing bug: delete node info if changing symbol */
                            /* if number of pins is different we must delete these data *before* */
                            /* changing ysmbol, otherwise i might end up deleting non allocated data. */
       my_strdup(369, &inst_ptr[inst].name,symbol);
       inst_ptr[inst].ptr=sym_number;
       bbox(ADD, inst_ptr[inst].x1, inst_ptr[inst].y1, inst_ptr[inst].x2, inst_ptr[inst].y2);
  
       my_strdup(370, &name, inst_ptr[inst].instname);
       if(name && name[0] )  /* 30102003 */
       {
         /* 20110325 only modify prefix if prefix not NUL */
         if(prefix) name[0]=prefix; /* change prefix if changing symbol type; */
  
         my_strdup(371, &ptr,subst_token(inst_ptr[inst].prop_ptr, "name", name) );
         hash_all_names(inst);
         new_prop_string(inst, ptr,0, disable_unique_names); /* set new prop_ptr */
         my_strdup2(372, &inst_ptr[inst].instname, get_tok_value(inst_ptr[inst].prop_ptr, "name",0)); /* 20150409 */
  
         type=instdef[inst_ptr[inst].ptr].type; /* 20150409 */
         cond= !type || (strcmp(type,"label") && strcmp(type,"ipin") &&
             strcmp(type,"opin") &&  strcmp(type,"iopin"));
         if(cond) inst_ptr[inst].flags|=2;
         else inst_ptr[inst].flags &=~2;
       }
       /* new symbol bbox after prop changes (may change due to text length) */
       symbol_bbox(inst, &inst_ptr[inst].x1, &inst_ptr[inst].y1, &inst_ptr[inst].x2, &inst_ptr[inst].y2);
       bbox(ADD, inst_ptr[inst].x1, inst_ptr[inst].y1, inst_ptr[inst].x2, inst_ptr[inst].y2);
       /* redraw symbol */
       bbox(SET,0.0,0.0,0.0,0.0);
       draw();
       bbox(END,0.0,0.0,0.0,0.0);
     }
   }

 } else if( !strcmp(argv[1],"getprop")) { /* 20171028 */


   if( argc > 2 && !strcmp(argv[2], "instance")) {
     int i;
     char *tmp;
     if(argc!=5 && argc !=4) {
       Tcl_AppendResult(interp, "'xschem getprop instance' needs 1 or 2 additional arguments", NULL);
       return TCL_ERROR;
     }
     Tcl_ResetResult(interp);
     if( (i = get_instance(argv[3])) < 0 ) {
       Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
       return TCL_ERROR;
     }
     if(argc == 4) {
       Tcl_AppendResult(interp, inst_ptr[i].prop_ptr, NULL);
     } else if(!strcmp(argv[4],"cell::name")) {
       tmp = inst_ptr[i].name;
       Tcl_AppendResult(interp, tmp, NULL);
     } else if(strstr(argv[4], "cell::") ) {
       tmp = get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, argv[4]+6, 0);
       if(debug_var>=1) fprintf(errfp, "xschem getprop: looking up instance %d prop cell::|%s| : |%s|\n", i, argv[4]+6, tmp);
       Tcl_AppendResult(interp, tmp, NULL);
     } else {
       Tcl_AppendResult(interp, get_tok_value(inst_ptr[i].prop_ptr, argv[4], 0), NULL);
     }
   } else if(argc > 2 && !strcmp(argv[2], "instance_pin")) {
     /*   0       1        2         3   4       5     */
     /* xschem getprop instance_pin X10 PLUS pin_attr  */
     /* xschem getprop instance_pin X10  1   pin_attr  */
     int inst, n=-1, tmp;
     char *subtok=NULL, *value=NULL;
     Tcl_ResetResult(interp);
     if(argc != 6 && argc != 5) {
       Tcl_AppendResult(interp, "xschem getprop instance_pin needs 2 or 3 additional arguments", NULL);
       return TCL_ERROR;
     }

     if( (inst = get_instance(argv[3])) < 0 ) {
       Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
       return TCL_ERROR;
     }
     if(isonlydigit(argv[4])) {
       n = atoi(argv[4]);
     }
     else {
       for(n = 0; n < (inst_ptr[inst].ptr+instdef)->rects[PINLAYER]; n++) {
         if(!strcmp(get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr,"name",0), argv[4])) break;
       }
     }
     if(n>=0  && n < (inst_ptr[inst].ptr+instdef)->rects[PINLAYER]) {
       if(argc == 5) {
        Tcl_AppendResult(interp, (inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr, NULL);
       } else {
         tmp = 100 + strlen(argv[4]) + strlen(argv[5]);
         subtok = my_malloc(83,tmp);
         my_snprintf(subtok, tmp, "%s(%s)", argv[5], argv[4]);
         value = get_tok_value(inst_ptr[inst].prop_ptr,subtok,0);
         if(!value[0]) {
           my_snprintf(subtok, tmp, "%s(%d)", argv[5], n);
           value = get_tok_value(inst_ptr[inst].prop_ptr,subtok,0);
         }
         if(!value[0]) {
           value = get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr,argv[5],0);
         }
         if(value[0] != 0) {
           char *ss;
           int slot;
           if( (ss = strchr(inst_ptr[inst].instname, ':')) ) {
             sscanf(ss + 1, "%d", &slot);
             if(strstr(value, ":")) value = find_nth(value, ':', slot);
           }
           Tcl_AppendResult(interp, value, NULL);
         }
         my_free(&subtok);
       }
     }
   } else if( !strcmp(argv[2],"symbol")) { /* 20171028 */
     int i, found=0;
     if(argc!=5 && argc !=4) {
       Tcl_AppendResult(interp, "xschem getprop needs 2 or 3 additional arguments", NULL);
       return TCL_ERROR;
     }
     Tcl_ResetResult(interp);
     for(i=0; i<lastinstdef; i++) {
       if(!strcmp(instdef[i].name,argv[3])){
         found=1;
         break;
       }
     }
     if(!found) {
       Tcl_AppendResult(interp, "Symbol not found", NULL);
       return TCL_ERROR;
     }
     if(argc == 4) 
       Tcl_AppendResult(interp, instdef[i].prop_ptr, NULL);
     else 
       Tcl_AppendResult(interp, get_tok_value(instdef[i].prop_ptr, argv[4], 0), NULL);
   }
 } else if(!strcmp(argv[1],"pinlist")) { /* 20171029 */
   int i, p, no_of_pins;
   if( (i = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
     return TCL_ERROR;
   }
   no_of_pins= (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
   for(p=0;p<no_of_pins;p++) {
     char s[10];
     my_snprintf(s, S(s), "%d", p);
     if(argc == 4 && argv[3][0]) {
       Tcl_AppendResult(interp, "{ {", s, "} {",
         get_tok_value((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][p].prop_ptr, argv[3], 0),
         "} } ", NULL);
     } else {
       Tcl_AppendResult(interp, "{ {", s, "} {", (inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][p].prop_ptr, "} } ", NULL);
     }
     
   }
 } else if(!strcmp(argv[1],"instance_net")) { /* 20171029 */
   int no_of_pins, i, p, mult;
   const char *str_ptr;

   if( argc <4) {
     Tcl_AppendResult(interp, "xschem instance_net needs 2 additional arguments", NULL);
     return TCL_ERROR;
   }
   if( (i = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem getprop: instance not found", NULL);
     return TCL_ERROR;
   }
   prepare_netlist_structs(1);
   no_of_pins= (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
   for(p=0;p<no_of_pins;p++) {
     if(!strcmp( get_tok_value((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][p].prop_ptr,"name",0), argv[3])) {
       /*str_ptr =  inst_ptr[i].node[p] ? inst_ptr[i].node[p]: "<UNCONNECTED PIN>"; */
       str_ptr =  pin_node(i,p,&mult, 0);
       break;
     }
   } /* /20171029 */
   if(p>=no_of_pins) {
     Tcl_AppendResult(interp, "Pin not found", NULL);
     return TCL_ERROR;
   } 
   Tcl_AppendResult(interp, str_ptr, NULL);
 } else if(!strcmp(argv[1],"selected_set")) {
   int n, i;
   char *res = NULL;
   rebuild_selected_array();
   for(n=0; n < lastselected; n++) {
     if(selectedgroup[n].type == ELEMENT) {
       i = selectedgroup[n].n;
       my_strcat(645, &res, inst_ptr[i].instname);
       if(n < lastselected-1) my_strcat(646, &res, " ");
     }
   }
   Tcl_AppendResult(interp, res, NULL);
   my_free(&res);
 } else if(!strcmp(argv[1],"select")) {
   if(argc<3) return TCL_ERROR;
   drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
 
   if(!strcmp(argv[2],"instance") && argc==4) {
     int n=atol(argv[3]);
     int i,found=0;
      
     /* 20171006 find by instance name */
     for(i=0;i<lastinst;i++) {
       if(!strcmp(inst_ptr[i].instname, argv[3])) {
         select_element(i, SELECTED, 0, 0);
         found=1;
         break;
       }
     }
     if(!found && n<lastinst) select_element(n, SELECTED, 0, 0);
   }
   else if(!strcmp(argv[2],"wire") && argc==4) {
     int n=atol(argv[3]);
     if(n<lastwire) select_wire(atol(argv[3]), SELECTED, 0);
   }
   else if(!strcmp(argv[2],"text") && argc==4) {
     int n=atol(argv[3]);
     if(n<lasttext) select_text(atol(argv[3]), SELECTED, 0);
   }
   drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 } else if(!strcmp(argv[1],"instance")) {
   if(argc==7)
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]),NULL, 3, 1);
   else if(argc==8)
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 3, 1);
   else if(argc==9) {
     int x = !(atoi(argv[8]));
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 0, x);
   }
 } else if(!strcmp(argv[1],"arc")) { /* 20171022 */
   ui_state |= MENUSTARTARC;
 } else if(!strcmp(argv[1],"circle")) { /* 20171022 */
   ui_state |= MENUSTARTCIRCLE;
 } else if(!strcmp(argv[1],"snap_wire")) { /* 20171022 */
   ui_state |= MENUSTARTSNAPWIRE;
 } else if(!strcmp(argv[1],"wire")) {   
   double x1,y1,x2,y2;
   int pos, save;
   const char *prop;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc >= 7) pos=atol(argv[6]);
     if(argc == 8) prop = argv[7]; 
     else prop = NULL;
     push_undo();
     storeobject(pos, x1,y1,x2,y2,WIRE,0,0,prop);
     save = draw_window; draw_window = 1;
     drawline(WIRELAYER,NOW, x1,y1,x2,y2);
     draw_window = save;
   }
   else ui_state |= MENUSTARTWIRE;
 } else if(!strcmp(argv[1],"line")) {    
   double x1,y1,x2,y2;
   int pos, save;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,LINE,rectcolor,0,NULL);
     save = draw_window; draw_window = 1;
     drawline(rectcolor,NOW, x1,y1,x2,y2);
     draw_window = save;
   } 
   else ui_state |= MENUSTARTLINE;
 } else if(!strcmp(argv[1],"rect")) {
   double x1,y1,x2,y2;
   int pos, save;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,RECT,rectcolor,0,NULL);
     save = draw_window; draw_window = 1;
     drawrect(rectcolor,NOW, x1,y1,x2,y2);
     draw_window = save;
   }  
   else ui_state |= MENUSTARTRECT;
 } else if(!strcmp(argv[1],"polygon")) {
   ui_state |= MENUSTARTPOLYGON;
 } else if(!strcmp(argv[1],"align")) {
    push_undo();
    round_schematic_to_grid(cadsnap);
    set_modify(1);
    prepared_hash_instances=0;
    prepared_hash_wires=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
    draw();
 } else if(!strcmp(argv[1],"saveas")) {
   if(argc == 3) saveas(argv[2]);
   else saveas(NULL);
   Tcl_ResetResult(interp);
 } else if(!strcmp(argv[1],"save")) {
   if(debug_var>=1) fprintf(errfp, "xschem(): saving: current schematic\n");

   if(!strcmp(schematic[currentsch],"")) {   /* 20170622 check if unnamed schematic, use saveas in this case... */
     saveas(NULL);
   } else {
     save(0);
   }
 } else if(!strcmp(argv[1],"windows")) {
  printf("top win:%lx\n", Tk_WindowId(Tk_Parent(Tk_MainWindow(interp))));
 } else if(!strcmp(argv[1],"globals")) {
  printf("*******global variables:*******\n");
  printf("lw=%d\n", lw);
  printf("lastwire=%d\n", lastwire);
  printf("lastinst=%d\n", lastinst);
  printf("lastinstdef=%d\n", lastinstdef);
  printf("lastselected=%d\n", lastselected);
  printf("lasttext=%d\n", lasttext);
  printf("max_texts=%d\n", max_texts);
  printf("max_wires=%d\n", max_wires);
  printf("max_instances=%d\n", max_instances);
  printf("max_selected=%d\n", max_selected);
  printf("zoom=%.16g\n", zoom);
  printf("xorigin=%.16g\n", xorigin);
  printf("yorigin=%.16g\n", yorigin);
  for(i=0;i<cadlayers;i++)
  {
    printf("lastrect[%d]=%d\n", i, lastrect[i]);
    printf("lastline[%d]=%d\n", i, lastline[i]);
    printf("max_rects[%d]=%d\n", i, max_rects[i]);
    printf("max_lines[%d]=%d\n", i, max_lines[i]);
  }
  for(i=0;i<cadlayers;i++) {
    printf("enable_layer[%d]=%d\n", i, enable_layer[i]);
  }
  printf("currentsch=%d\n", currentsch);
  printf("schematic[%d]=%s\n", currentsch, schematic[currentsch]);
  for(i=0;i<currentsch;i++)
  {
    printf("previous_instance[%d]=%d\n",i,previous_instance[i]);
    printf("sch_prefix[%d]=%s\n",i,sch_prefix[i]? sch_prefix[i]:"<NULL>");
    printf("schematic[%d]=%s\n",i,schematic[i]);
  }
  printf("modified=%d\n", modified);
  printf("color_ps=%d\n", color_ps);
  printf("a3page=%d\n", a3page);
  printf("need_rebuild_selected_array=%d\n", need_rebuild_selected_array);
  printf("******* end global variables:*******\n");
 } else if(!strcmp(argv[1],"help")) {
  printf("xschem : function used to communicate with the C program\n");
  printf("Usage:\n");
  printf("      xschem callback X-event_type mousex mousey Xkeysym mouse_button Xstate\n");
  printf("                   can be used to send any event to the application\n");
  printf("      xschem netlist\n");
  printf("                   generates a netlist in the selected format for the current schematic\n");
  printf("      xschem simulate\n");
  printf("                   launches the currently set simulator on the generated netlist\n");
  printf("      xschem redraw\n");
  printf("                   Redraw the window\n");
  printf("      xschem new_window library/cell\n");
  printf("                   start a new window optionally with specified cell\n");
  printf("      xschem schematic_in_new_window \n");
  printf("                   start a new window with selected element schematic\n");
  printf("      xschem symbol_in_new_window \n");
  printf("                   start a new window with selected element schematic\n");
  printf("      xschem globals\n");
  printf("                   print information about global variables\n");
  printf("      xschem inst_ptr n\n");
  printf("                   return inst_ptr of inst_ptr[n]\n");
  printf("      xschem netlist\n");
  printf("                   perform a global netlist on current schematic\n");
  printf("      xschem netlist_type type\n");
  printf("                   set netlist type to <type>, currently spice, vhdl, verilog or tedax\n");
  printf("      xschem netlist_show yes|no\n");
  printf("                   show or not netlist in a window\n");
  printf("      xschem save [library/name]\n");
  printf("                   save current schematic, optionally a lib/name can be given\n");
  printf("      xschem saveas\n");
  printf("                   save current schematic, asking for a filename\n");
  printf("      xschem load library/cell\n");
  printf("                   load specified cell from library\n");
  printf("      xschem load_symbol library/cell\n");
  printf("                   load specified cell symbol view  from library\n");
  printf("      xschem reload\n");
  printf("                   reload current cell from library\n");
  printf("      xschem instance library/cell x y rot flip [property string]\n");
  printf("                   place instance cell of the given library at x,y, rot, flip\n");
  printf("                   can also be given a property string\n");
  printf("      xschem rect x1 y1 x2 y2 [pos]\n");
  printf("                   place rectangle, optionally at pos (position in database)\n");
  printf("      xschem line x1 y1 x2 y2 [pos]\n");
  printf("                   place line, optionally at pos (position in database)\n");
  printf("      xschem wire x1 y1 x2 y2 [pos]\n");
  printf("                   place wire, optionally at pos (position in database)\n");
  printf("      xschem select instance|wire|text n\n");
  printf("                   select instance or text or wire number n\n");
  printf("      xschem select_all\n");
  printf("                   select all objects\n");
  printf("      xschem descend\n");
  printf("                   descend into schematic of selected element\n");
  printf("      xschem descend_symbol\n");
  printf("                   descend into symbol of selected element\n");
  printf("      xschem go_back\n");
  printf("                   back from selected element\n");
  printf("      xschem unselect\n");
  printf("                   unselect selected objects\n");
  printf("      xschem set_fill n\n");
  printf("                   set fill style of current layer (rectcolor) to fill pattern n (pixdata)\n");
  printf("      xschem zoom_out\n");
  printf("                   zoom out\n");
  printf("      xschem zoom_in\n");
  printf("                   zoom in\n");
  printf("      xschem zoom_full\n");
  printf("                   zoom full\n");
  printf("      xschem zoom_box\n");
  printf("                   zoom box\n");
  printf("      xschem paste\n");
  printf("                   paste selection from clipboard\n");
  printf("      xschem merge\n");
  printf("                   merge external file into current schematic\n");
  printf("      xschem cut\n");
  printf("                   cut selection to clipboard\n");
  printf("      xschem copy\n");
  printf("                   copy selection to clipboard\n");
  printf("      xschem copy_objects\n");
  printf("                   duplicate selected objects\n");
  printf("      xschem move_objects [deltax deltay]\n");
  printf("                   move selected objects\n");
  printf("      xschem line_width n\n");
  printf("                   set line width to (float) n\n");
  printf("      xschem delete\n");
  printf("                   delete selected objects\n");
  printf("      xschem unhilight\n");
  printf("                   unlight selected nets/pins\n");
  printf("      xschem hilight\n");
  printf("                   hilight selected nets/pins\n");
  printf("      xschem clear_hilights\n");
  printf("                   unhilight  all nets/pins\n");
  printf("      xschem print [color]\n");
  printf("                   print schematic (optionally in color)\n");
  printf("      xschem search regex|exact <select> token value\n");
  printf("                   hilight instances which match tok=val property,\n");
  printf("                   exact search or regex\n");
  printf("                   select: 0-> highlight, 1-> select, -1-> unselect\n");
  printf("      xschem log file\n");
  printf("                   open a log file to write messages to\n");
  printf("      xschem get variable\n");
  printf("                   return  global variable\n");
  printf("      xschem set variable value\n");
  printf("                   set global variable\n");
  printf("      xschem clear\n");
  printf("                   clear current schematic\n");
  printf("      xschem exit\n");
  printf("                   exit program gracefully\n");
  printf("      xschem view_prop\n");
  printf("                   view properties of currently selected element\n");
  printf("      xschem edit_prop\n");
  printf("                   edit properties of currently selected element\n");
  printf("      xschem edit_vi_prop\n");
  printf("                   edit properties of currently selected element in a vim window\n");
  printf("      xschem place_symbol\n");
  printf("                   place new symbol, asking filename\n");
  printf("      xschem make_symbol\n");
  printf("                   make symbol view from current schematic\n");
  printf("      xschem place_text\n");
  printf("                   place new text\n");
  printf("      xschem sleep #ms\n");
  printf("                   sleep some ms\n");
  printf("      xschem debug  n\n");
  printf("                   set debug level to n: 1, 2, 3 for C Program \n");
  printf("                                        -1,-2,-3 for Tcl frontend\n");
 }

 else if(!strcmp(argv[1],"netlist") ) {
    if( set_netlist_dir(0, NULL) ) {
      if(netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(1);                  /* 1 means global netlist */
      else if(netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(1);
      else if(netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(1);
      else if(netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(1);
    }
 }

 else if(!strcmp(argv[1],"simulate") ) {
   if( set_netlist_dir(0, NULL) ) {

     /* 20140404 added CAD_SPICE_NETLIST */
     if(netlist_type==CAD_VERILOG_NETLIST) {
       my_snprintf(name, S(name), "simulate {%s.v}",
                skip_dir(schematic[currentsch]));
       tcleval(name);
     } else if(netlist_type==CAD_SPICE_NETLIST) {
       my_snprintf(name, S(name), "simulate {%s.spice}",
                skip_dir(schematic[currentsch]));
       tcleval(name);
     } else if(netlist_type==CAD_VHDL_NETLIST) {
       my_snprintf(name, S(name), "simulate {%s.vhdl}",
                skip_dir(schematic[currentsch]));
       tcleval(name);
     }
     /* /20140404 */
   }
 }

 else if(!strcmp(argv[1],"fill_symbol_editprop_form") ) {
   fill_symbol_editprop_form(0);
 }
 else if(!strcmp(argv[1],"update_symbol") ) {
   update_symbol(argv[2],0);
 }
 else if(!strcmp(argv[1],"clear_netlist_dir") ) {
   my_strdup(373, &netlist_dir, "");
 }

 else if(!strcmp(argv[1],"edit_file") ) {
    rebuild_selected_array();
    if(lastselected==0 ) {
      save_schematic(schematic[currentsch]); /* sync data with disk file before editing file */
      my_snprintf(name, S(name), "edit_file {%s}", 
          abs_sym_path(schematic[currentsch], ""));
      tcleval(name);
    }
    else if(selectedgroup[0].type==ELEMENT) {
      my_snprintf(name, S(name), "edit_file {%s}", 
          abs_sym_path(inst_ptr[selectedgroup[0].n].name, ""));
      tcleval(name);

    }
 }
 else if(!strcmp(argv[1],"print") ) { /* 20171022 added png, svg */
   if(argc==2 || (argc==3 && !strcmp(argv[2],"pdf")) ) {
     ps_draw();
     Tcl_ResetResult(interp);
   }
   else if(argc==3 && !strcmp(argv[2],"png") ) {
     print_image();
     Tcl_ResetResult(interp);
   }
   else if(argc==3 && !strcmp(argv[2],"svg") ) {
     svg_draw();
     Tcl_ResetResult(interp);
   }
 }

 else if(!strcmp(argv[1],"load_symbol") )
 {
    if(argc==3) {
      if(debug_var>=1) fprintf(errfp, "xschem(): load: filename=%s\n", argv[2]);
      delete_hilight_net();
      currentsch = 0;
      unselect_all(); /* 20180929 */
      remove_symbols();
      /* load_symbol(argv[2]); */
      load_schematic(1, 0, argv[2], 1);
      my_strdup(374, &sch_prefix[currentsch],".");
      zoom_full(1, 0);
    }
 }

 else if(!strcmp(argv[1],"load") )
 {
    if(argc==3) {
      if(debug_var>=1) fprintf(errfp, "xschem(): load: filename=%s\n", argv[2]);
      delete_hilight_net();
      currentsch = 0;
      unselect_all(); /* 20180929 */
      remove_symbols();
      load_schematic(0, 1, abs_sym_path(argv[2], ""), 1);
      my_strdup(375, &sch_prefix[currentsch],".");
      zoom_full(1, 0);
    }
    else if(argc==2) { 
      ask_new_file();
    }
 }

 else if(!strcmp(argv[1],"test"))
 {
  /*XSetWMHints(display, topwindow, hints_ptr); */
 }

 else if(!strcmp(argv[1],"set_fill") && argc==3)
 {
  set_fill(atoi(argv[2]));
 }

 else if(!strcmp(argv[1],"redraw"))
 {
  draw();
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"break_wires"))
 {
  break_wires_at_pins();
 }

 else if(!strcmp(argv[1],"collapse_wires"))
 {
  push_undo(); /* 20150327 */
  trim_wires();
  draw();
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"push_undo"))
 {
  push_undo();
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"check_unique_names")) {
   if(!strcmp(argv[2],"1")) {
     check_unique_names(1);
   } else {
     check_unique_names(0);
   }
 }

 else if(!strcmp(argv[1],"new_window"))
 {
  if(argc==2) new_window("",0);
  else new_window(argv[2],0);
 }

 else if(!strcmp(argv[1],"symbol_in_new_window"))
 {
  symbol_in_new_window();
 }

 else if(!strcmp(argv[1],"schematic_in_new_window"))
 {
  schematic_in_new_window();
 }

 else if(!strcmp(argv[1],"netlist_show"))
 {
  if(!strcmp(argv[2],"yes")) netlist_show=1;
  else netlist_show=0;
 }

 else if(!strcmp(argv[1],"netlist_type"))
 {
  if(!strcmp(argv[2],"vhdl")) {
    netlist_type=CAD_VHDL_NETLIST;
  }
  else if(!strcmp(argv[2],"verilog")) {
    netlist_type=CAD_VERILOG_NETLIST;
  }
  else if(!strcmp(argv[2],"tedax")) {
    netlist_type=CAD_TEDAX_NETLIST;
  }
  else {
    netlist_type=CAD_SPICE_NETLIST;
  }
  tclsetvar("netlist_type",argv[2]);
 }

 else if(!strcmp(argv[1],"unselect_all"))
 {
  unselect_all();
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"clear_drawing"))
 {
  if(argc==2) clear_drawing();
 }

 else if(!strcmp(argv[1],"remove_symbols"))
 {
  if(argc==2) remove_symbols();
 }

 else if(!strcmp(argv[1],"view_prop"))
 {
  edit_property(2);
 }

 else if(!strcmp(argv[1],"undo"))
 {
  pop_undo(0);
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"redo"))
 {
  pop_undo(1);
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"edit_prop"))
 {
  edit_property(0);
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"edit_vi_prop"))
 {
  edit_property(1);
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"trim_wires")) /* 20171022 */
 {
   push_undo();
   trim_wires();
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"delete"))
 {
  if(argc==2) delete();
  Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"enable_layers"))
 {
  Tcl_ResetResult(interp);
  enable_layers(); 
 }

 else if(!strcmp(argv[1],"select_connected_nets"))
 {
   select_connected_nets();
 }
 else if(!strcmp(argv[1],"unhilight"))
 {
    unhilight_net();
    draw();
    Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"hilight"))
 {
   hilight_net();
   draw_hilight_net(1);
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"clear_hilights"))
 {
   delete_hilight_net();
   draw();
   Tcl_ResetResult(interp);
 }

 else if(!strcmp(argv[1],"search"))
 {
   /*   0      1         2        3       4   5   */
   /*                           select            */
   /* xschem search regex|exact 0|1|-1   tok val  */
  int select, what;
  what = NOW;
  if(argc == 7) {
    if(!strcmp(argv[6], "ADD")) what = ADD;
    else if(!strcmp(argv[6], "END")) what = END;
    else if(!strcmp(argv[6], "NOW")) what = NOW;
    argc = 6;
  }
  if(argc==6) {
    select = atoi(argv[3]);
    if( !strcmp(argv[2],"regex") )  search(argv[4],argv[5],0,select, what);
    else  search(argv[4],argv[5],1,select, what);
  }
 }

 /* 20171010 allows to retrieve name of n-th parent schematic */
 else if(argc == 4 && !strcmp(argv[1],"get") && !strcmp(argv[2],"schname") ) {
   int x;
   x = atoi(argv[3]);
   if(x<0 && currentsch+x>=0) {
     Tcl_AppendResult(interp, schematic[currentsch+x], NULL);
   }
 }
 else if(!strcmp(argv[1],"instance_bbox")) {
   int i;
   char s[200];
   for(i=0;i<lastinst;i++) {
     if(!strcmp(inst_ptr[i].instname, argv[2])) {
       break;
     }
   }
   if(i<lastinst) {
   my_snprintf(s, S(s), "Instance: %g %g %g %g", inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
   Tcl_AppendResult(interp, s, NULL);
   my_snprintf(s, S(s), "\nSymbol: %g %g %g %g", 
	(inst_ptr[i].ptr+instdef)->minx, 
	(inst_ptr[i].ptr+instdef)->miny, 
	(inst_ptr[i].ptr+instdef)->maxx, 
	(inst_ptr[i].ptr+instdef)->maxy 
              );
   Tcl_AppendResult(interp, s, NULL);
   }
 }

 else if(!strcmp(argv[1],"instance_pos")) {
   int i;
   char s[30];
   for(i=0;i<lastinst;i++) {
     if(!strcmp(inst_ptr[i].instname, argv[2])) {
       break;
     }
   }
   if(i==lastinst) i = -1;
   my_snprintf(s, S(s), "%d", i);
   Tcl_AppendResult(interp, s, NULL);
 }
 else if(!strcmp(argv[1],"instance_pins")) {
   char *pins = NULL;
   int p, i, no_of_pins;
   prepare_netlist_structs(1);

   if( (i = get_instance(argv[2])) < 0 ) {
     Tcl_AppendResult(interp, "xschem instance_pins: instance not found", NULL);
     return TCL_ERROR;
   }
   no_of_pins= (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
   for(p=0;p<no_of_pins;p++) {
     char *pin;
     pin = get_tok_value((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][p].prop_ptr,"name",0);
     if(!pin[0]) pin = "--ERROR--";
     my_strcat(376, &pins, pin);
     if(p< no_of_pins-1) my_strcat(377, &pins, " ");
   }
   Tcl_AppendResult(interp, pins, NULL);
   my_free(&pins);
 }

 /*
  * ********** xschem get subcommands
  */

 else if(argc == 4 && !strcmp(argv[1],"get") && !strcmp(argv[2],"expandlabel"))  {  /* 20121121 */
   int tmp, llen;
   char *result=NULL;
   const char *l;

   l = expandlabel(argv[3], &tmp);
   llen = strlen(l);
   result = my_malloc(378, llen + 30);
   my_snprintf(result, llen + 30, "%s %d", l, tmp);
   Tcl_AppendResult(interp, result, NULL);
   my_free(&result);
 }
 else if(!strcmp(argv[1],"get") && argc==3)
 {
  Tcl_ResetResult(interp);
  if(!strcmp(argv[2],"current_type"))  { /* 20171025 */
     if( current_type == SYMBOL )
        Tcl_AppendResult(interp, "SYMBOL",NULL);
     else 
        Tcl_AppendResult(interp, "SCHEMATIC",NULL);
  }
  else if(!strcmp(argv[2],"netlist_type"))  {
     if( netlist_type == CAD_VHDL_NETLIST )
        Tcl_AppendResult(interp, "vhdl",NULL);
     else if( netlist_type == CAD_SPICE_NETLIST )
        Tcl_AppendResult(interp, "spice",NULL);
     else if( netlist_type == CAD_TEDAX_NETLIST )
        Tcl_AppendResult(interp, "tedax",NULL);
     else 
        Tcl_AppendResult(interp, "verilog",NULL);
  }
  else if(!strcmp(argv[2],"incr_hilight"))  {
     if( incr_hilight != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"auto_hilight"))  { /* 20160413 */
     if( auto_hilight != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"netlist_show"))  {
     if( netlist_show != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"flat_netlist"))  {
     if( flat_netlist != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"split_files"))  {
     if( split_files != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"bbox_selected"))  {
    Box boundbox;
    char res[2048];
    calc_drawing_bbox(&boundbox, 1);
    my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    Tcl_AppendResult(interp, res, NULL);
  }
  else if(!strcmp(argv[2],"bbox_hilighted"))  {
    Box boundbox;
    char res[2048];
    calc_drawing_bbox(&boundbox, 2);
    my_snprintf(res, S(res), "%g %g %g %g", boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    Tcl_AppendResult(interp, res, NULL);
  }
  else if(!strcmp(argv[2],"enable_stretch"))  {
     if( enable_stretch != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"help"))  {   /* 20140406 */
     if( help != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"color_ps"))  {
     if( color_ps != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"only_probes"))  {  /* 20110112 */
     if( only_probes != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"event_reporting"))  {
     if( event_reporting != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"a3page"))  {
     if( a3page != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"draw_grid"))  {
     if( draw_grid != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"sym_txt"))  {
     if( sym_txt != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"schname"))  {
        Tcl_AppendResult(interp, schematic[currentsch], NULL);
  }
  else if(!strcmp(argv[2],"cadsnap_default"))  { /* 20161212 */
        Tcl_AppendResult(interp, tclgetvar("snap"),NULL);
  }
  else if(!strcmp(argv[2],"cadsnap"))  { /* 20161212 */
        char s[30]; /* overflow safe 20161212 */
        my_snprintf(s, S(s), "%.9g",cadsnap);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"rectcolor"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",rectcolor);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"debug_var"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",debug_var);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"semaphore"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",semaphore);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"change_lw"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",change_lw);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"draw_window"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",draw_window);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"ui_state"))  {
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",ui_state);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"netlist_dir"))  {
        Tcl_AppendResult(interp, netlist_dir,NULL);
  }
  else if(!strcmp(argv[2],"sch_path"))  {  /* 20121121 */
        Tcl_AppendResult(interp, sch_prefix[currentsch], NULL);
  }
  else if(!strcmp(argv[2],"dim"))  {  /* 20121121 */
        char s[40];
        my_snprintf(s, S(s), "%.2g", color_dim);
        Tcl_AppendResult(interp, s, NULL);
  }
  else if(!strcmp(argv[2],"lastinst"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",lastinst);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"pinlayer"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",PINLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"wirelayer"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",WIRELAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"textlayer"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",TEXTLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"sellayer"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",SELLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"gridlayer"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",GRIDLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"backlayer"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%d",BACKLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"version"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "XSCHEM V%s",XSCHEM_VERSION);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"userconfdir"))  {  /* 20121121 */
        char s[30]; /* overflow safe 20161122 */
        my_snprintf(s, S(s), "%s",USER_CONF_DIR);
        Tcl_AppendResult(interp, s,NULL);
  }
  else {
    fprintf(errfp, "xschem get %s: invalid command.\n", argv[2]);
  }
 }

 /* 
  * ********** xschem  set subcommands
  */

 else if(!strcmp(argv[1],"set") && argc==3) { /* 20171023 */
  if(!strcmp(argv[2],"horizontal_move"))  { /* 20171023 */
    horizontal_move = atoi(tclgetvar("horizontal_move"));
    if(horizontal_move) {
      vertical_move=0;
      tcleval("set vertical_move 0" );
    }
  }
  else if(!strcmp(argv[2],"vertical_move"))  { /* 20171023 */
    vertical_move = atoi(tclgetvar("vertical_move"));
    if(vertical_move) {
      horizontal_move=0;
      tcleval("set horizontal_move 0" );
    }
  }
  else {
    fprintf(errfp, "xschem set %s : invalid command.\n", argv[2]);
  }
 } else if(!strcmp(argv[1],"set") && argc==4)
 {
  #ifdef HAS_CAIRO
  if(!strcmp(argv[2],"cairo_font_name"))  {
    if( strlen(argv[3]) < sizeof(cairo_font_name) ) {
      my_strncpy(cairo_font_name, argv[3], S(cairo_font_name));
      cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    }
  } else
  #endif
  if(!strcmp(argv[2],"no_undo"))  { /* 20171204 */
    int s = atoi(argv[3]);
    no_undo=s;
  }
  else if(!strcmp(argv[2],"no_draw"))  { /* 20171204 */
    int s = atoi(argv[3]);
    no_draw=s;
  }
  else if(!strcmp(argv[2],"dim"))  {
    double s = atof(argv[3]);
    color_dim = s;
  }
  else if(!strcmp(argv[2],"cairo_font_scale"))  {
    double s = atof(argv[3]);
    if(s>0.1 && s<10.0) cairo_font_scale = s;
  }
  else if(!strcmp(argv[2],"nocairo_font_xscale"))  {
    double s = atof(argv[3]);
    if(s>0.1 && s<10.0) nocairo_font_xscale = s;
  }
  else if(!strcmp(argv[2],"nocairo_font_yscale"))  {
    double s = atof(argv[3]);
    if(s>0.1 && s<10.0) nocairo_font_yscale = s;
  }
  else if(!strcmp(argv[2],"cairo_font_line_spacing"))  {
    double s = atof(argv[3]);
    if(s>0.1 && s<10.0) cairo_font_line_spacing = s;
  }
  else if(!strcmp(argv[2],"cairo_vert_correct"))  {
    double s = atof(argv[3]);
    if(s>-20. && s<20.) cairo_vert_correct = s;
  }
  else if(!strcmp(argv[2],"nocairo_vert_correct"))  {
    double s = atof(argv[3]);
    if(s>-20. && s<20.) nocairo_vert_correct = s;
  }
  else if(!strcmp(argv[2],"netlist_type"))  {
    if(!strcmp(argv[3],"vhdl")) {
     netlist_type = CAD_VHDL_NETLIST ;
    } else if(!strcmp(argv[3],"verilog")) {
     netlist_type = CAD_VERILOG_NETLIST ;
    } else if(!strcmp(argv[3],"tedax")) {
     netlist_type = CAD_TEDAX_NETLIST ;
    } else {
     netlist_type = CAD_SPICE_NETLIST ;
    }
  }
  else if(!strcmp(argv[2],"current_type")) { /* 20171025 */
    if(!strcmp(argv[3],"SYMBOL")) {
      current_type=SYMBOL;
    } else {
      current_type=SCHEMATIC;
    }
  }
  else if(!strcmp(argv[2],"persistent_command")) { /* 20171025 */
    if(!strcmp(argv[3],"1")) {
      persistent_command=1;
    } else {
      persistent_command=0;
    }
  }
  else if(!strcmp(argv[2],"disable_unique_names")) { /* 20171025 */
    if(!strcmp(argv[3],"1")) {
      disable_unique_names=1;
    } else {
      disable_unique_names=0;
    }
  }
  else if(!strcmp(argv[2],"incr_hilight"))  {
        incr_hilight=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"auto_hilight"))  { /* 20160413 */
        auto_hilight=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"netlist_show"))  {
        netlist_show=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"semaphore"))  {
        semaphore=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"cadsnap"))  { /* 20161212 */
        set_snap( atof(argv[3]) );
  }
  else if(!strcmp(argv[2],"cadgrid"))  { /* 20161212 */
        set_grid( atof(argv[3]) );
  }
  else if(!strcmp(argv[2],"flat_netlist"))  {
        flat_netlist=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"split_files"))  {
        split_files=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"enable_stretch"))  {
        enable_stretch=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"a3page"))  {
        a3page=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"color_ps"))  {
        color_ps=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"only_probes"))  {  /* 20110112 */
        only_probes=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"event_reporting"))  {  /* 20110112 */
        event_reporting=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"draw_grid"))  {
        draw_grid=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"sym_txt"))  {
        sym_txt=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"rectcolor"))  {
     rectcolor=atoi(argv[3]);
     rebuild_selected_array();
     if(lastselected) {
       change_layer();
     }
  }
  else if(!strcmp(argv[2],"change_lw"))  {
     change_lw=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"draw_window"))  {
     draw_window=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"ui_state"))  {
     ui_state=atoi(argv[3]);
  }
  else {
    fprintf(errfp, "xschem set %s %s : invalid command.\n", argv[2], argv[3]);
  }
 }

 else {
   fprintf(errfp, "xschem %s: invalid command.\n", argv[1]);
 }
 return TCL_OK;
}

const char *tclgetvar(const char *s)
{
  if(debug_var>=2) fprintf(errfp, "tclgetvar(): %s\n", s);
  return Tcl_GetVar(interp,s, TCL_GLOBAL_ONLY);
}

void tcleval(const char str[])
{
  if(debug_var>=2) fprintf(errfp, "tcleval(): %s\n", str);
  /* if( Tcl_EvalEx(interp, str, -1, TCL_EVAL_GLOBAL) != TCL_OK) {*/
  if( Tcl_Eval(interp, str) != TCL_OK) {
    fprintf(errfp, "tcleval(): evaluation of script: %s failed\n", str);
  }
}

void tclsetvar(const char *s, const char *value)
{
  if(debug_var>=2) fprintf(errfp, "tclsetvar(): %s to %s\n", s, value);
  if(!Tcl_SetVar(interp, s, value, TCL_GLOBAL_ONLY)) {
    fprintf(errfp, "tclsetvar(): error setting variable %s to %s\n", s, value);
  }
}
