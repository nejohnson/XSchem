/* File: scheduler.c
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

void statusmsg(char str[],int n)
{
 static char *s=NULL;     // overflow safe
 if(n==2)
 {
  my_strdup(&s,"infowindow {");
  my_strcat(&s,str);
  if(!strcmp(str, "") ) my_strcat(&s,"}");
  else my_strcat(&s,"\n}");
   if(debug_var>=2) fprintf(errfp, "statusmsg(): %s\n", s);
  Tcl_EvalEx(interp, s, -1, TCL_EVAL_GLOBAL);
 }
 else
 {
  my_strdup(&s,".statusbar.1 configure -text {");
  my_strcat(&s,str);
  my_strcat(&s,"}");
  Tcl_EvalEx(interp, s, -1, TCL_EVAL_GLOBAL);  
 }
}

// can be used to reach C functions from the Tk shell.
int xschem(ClientData clientdata, Tcl_Interp *interp, int argc, char * argv[])
{	
 int i;
 char name[1024]; // overflow safe 20161122
  if(argc<2) return TCL_ERROR;
 if(debug_var>=3) fprintf(errfp, "xschem(): argv[1]=%s\n", argv[1]);
// sample function
// if(argc == 2) if(debug_var>=1) fprintf(errfp, " xschem(): %s\n", argv[1]);
// Tcl_ResetResult(interp);
// Tcl_AppendResult(interp, argv[1],argv[1],NULL); // test:apppend twice argv[1]
 if(!strcmp(argv[1],"callback") )
 {
  callback( atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), (KeySym)atol(argv[5]),
            atoi(argv[6]), atoi(argv[7]), atoi(argv[8]) );
 }

 else if(!strcmp(argv[1],"netlist") ) {
    if( set_netlist_dir(0) ) {
      if(netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(1);                  // 1 means global netlist
      else if(netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(1);
      else if(netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(1);
      else if(netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(1);
    }
 }

 else if(!strcmp(argv[1],"simulate") ) {
   if( set_netlist_dir(0) ) {

     // 20140404 added CAD_SPICE_NETLIST
     if(netlist_type==CAD_VERILOG_NETLIST) {
       my_snprintf(name, S(name), "simulate %s.v",
                skip_dir(schematic[currentsch]));
       tkeval(name);
     } else if(netlist_type==CAD_SPICE_NETLIST) {
       my_snprintf(name, S(name), "simulate %s.spice",
                skip_dir(schematic[currentsch]));
       tkeval(name);
     } else if(netlist_type==CAD_VHDL_NETLIST) {
       my_snprintf(name, S(name), "simulate %s.vhdl",
                skip_dir(schematic[currentsch]));
       tkeval(name);
     }
     // /20140404
   }
 }

 else if(!strcmp(argv[1],"fill_symbol_editprop_form") ) {
   fill_symbol_editprop_form(0);
 }
 else if(!strcmp(argv[1],"update_symbol") ) {
   update_symbol(argv[2],0);
 }
 else if(!strcmp(argv[1],"clear_netlist_dir") ) {
   my_strdup(&netlist_dir, "");
 }

 else if(!strcmp(argv[1],"edit_file") ) {
    rebuild_selected_array();
    if(lastselected==0 ) {
      save_schematic(NULL); // sync data with disk file before editing file
      my_snprintf(name, S(name), "edit_file %s/%s.sch",
            Tcl_GetVar(interp, "XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
            schematic[currentsch]);
      tkeval(name);
    }
    else if(selectedgroup[0].type==ELEMENT) {
      my_snprintf(name, S(name), "edit_file %s/%s.sch",
            Tcl_GetVar(interp, "XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
            inst_ptr[selectedgroup[0].n].name);
      tkeval(name);

    }
 }
 else if(!strcmp(argv[1],"print") ) { // 20171022 added png, svg
   if(argc==2 || (argc==3 && !strcmp(argv[2],"pdf")) ) {
     ps_draw();
   }
   else if(argc==3 && !strcmp(argv[2],"png") ) {
     print_image();
   }
   else if(argc==3 && !strcmp(argv[2],"svg") ) {
     svg_draw();
   }
 }

 else if(!strcmp(argv[1],"inst_ptr"))
 {
  i=atol(argv[2]);
  printf("xschem(): inst_ptr[%d].prop_ptr=%s\n",i,inst_ptr[i].prop_ptr);
  printf("xschem(): inst_ptr[%d].flags=%d\n",i,inst_ptr[i].flags);
 }

 else if(!strcmp(argv[1],"load_symbol") )
 {
    char s[1024]; // overflow safe 20161122

    if(argc==3) {
      if(debug_var>=1) fprintf(errfp, "xschem(): load: filename=%s\n", argv[2]);
      delete_hilight_net();
      currentsch = 0;
      my_snprintf(s, S(s), "get_cell {%s}", argv[2]);
      tkeval(s);
      my_strncpy(schematic[currentsch],Tcl_GetStringResult(interp), S(schematic[currentsch]));
      //clear_drawing();
      remove_symbols();
      load_symbol(NULL);
      my_strdup(&sch_prefix[currentsch],".");
      zoom_full(1);
    }
 }

 else if(!strcmp(argv[1],"load") )
 {
    char s[1024]; // overflow safe 20161122

    if(argc==3) {
      if(debug_var>=1) fprintf(errfp, "xschem(): load: filename=%s\n", argv[2]);
      delete_hilight_net();
      currentsch = 0;
      my_snprintf(s, S(s), "get_cell {%s}", argv[2]);
      tkeval(s);
      my_strncpy(schematic[currentsch],Tcl_GetStringResult(interp), S(schematic[currentsch]));
      //clear_drawing();
      remove_symbols();
      load_schematic(1, NULL,1);
      my_strdup(&sch_prefix[currentsch],".");
      zoom_full(1);
    }
    else if(argc==2) { 
      ask_new_file();
    }
 }

 else if(!strcmp(argv[1],"test"))
 {
  //XSetWMHints(display, topwindow, hints_ptr);
 }

 else if(!strcmp(argv[1],"set_fill") && argc==3)
 {
  set_fill(atoi(argv[2]));
 }

 else if(!strcmp(argv[1],"redraw"))
 {
  draw();
 }

 else if(!strcmp(argv[1],"push_undo"))
 {
  push_undo();
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

 else if(!strcmp(argv[1],"edit_in_new_window"))
 {
  edit_in_new_window();
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
  Tcl_SetVar(interp,"netlist_type",argv[2],TCL_GLOBAL_ONLY);
 }

 else if(!strcmp(argv[1],"unselect"))
 {
  if(argc==2) unselect_all();
 }

 else if(!strcmp(argv[1],"view_prop"))
 {
  edit_property(2);
 }

 else if(!strcmp(argv[1],"edit_prop"))
 {
  edit_property(0);
 }

 else if(!strcmp(argv[1],"edit_vi_prop"))
 {
  edit_property(1);
 }

 else if(!strcmp(argv[1],"collapse_wires")) // 20171022
 {
   push_undo();
   collapse_wires();
   draw();
 }

 else if(!strcmp(argv[1],"delete"))
 {
  if(argc==2) delete();
 }

 else if(!strcmp(argv[1],"unhilight"))
 {
    unhilight_net();
    draw();
 }

 else if(!strcmp(argv[1],"hilight"))
 {
    hilight_net();
    draw_hilight_net();
 }

 else if(!strcmp(argv[1],"delete_hilight_net"))
 {
    delete_hilight_net();
    draw();
 }

 else if(!strcmp(argv[1],"search"))
 {
   //   0      1       2      3   4   5
   // xschem search [no]sub  0|1 tok val 
  int select;
  if(argc==6) {
    select = atoi(argv[3]);
    if( !strcmp(argv[2],"nosub") )  search_inst(argv[4],argv[5],0,select, NOW);
    if( !strcmp(argv[2],"sub") )  search_inst(argv[4],argv[5],1,select, NOW);
  }
 }

 // 20171010 allows to retrieve name of n-th parent schematic
 else if(!strcmp(argv[1],"get") && !strcmp(argv[2],"schname") && argc==4) {
   int x;
   x = atoi(argv[3]);
   if(x<0 && currentsch+x>=0) {
     Tcl_AppendResult(interp, schematic[currentsch+x], NULL);
   }
 }
 else if(!strcmp(argv[1],"get") && argc==3)
 {
  Tcl_ResetResult(interp);
  if(!strcmp(argv[2],"current_type"))  { // 20171025
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
  else if(!strcmp(argv[2],"auto_hilight"))  { // 20160413
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
  else if(!strcmp(argv[2],"hspice_netlist"))  {
     if( hspice_netlist != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"enable_stretch"))  {
     if( enable_stretch != 0 )
        Tcl_AppendResult(interp, "1",NULL);
     else
        Tcl_AppendResult(interp, "0",NULL);
  }
  else if(!strcmp(argv[2],"help"))  {	// 20140406
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
  else if(!strcmp(argv[2],"only_probes"))  {  // 20110112
     if( only_probes != 0 )
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
  else if(!strcmp(argv[2],"schpath"))  {
        Tcl_AppendResult(interp, schematic[currentsch],NULL);
  }
  else if(!strcmp(argv[2],"cadsnap_default"))  { // 20161212
        char s[30]; // overflow safe 20161212
        my_snprintf(s, S(s), "%.9g",CADSNAP);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"cadsnap"))  { // 20161212
        char s[30]; // overflow safe 20161212
        my_snprintf(s, S(s), "%.9g",cadsnap);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"rectcolor"))  {
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",rectcolor);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"debug_var"))  {
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",debug_var);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"semaphore"))  {
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",semaphore);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"change_lw"))  {
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",change_lw);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"ui_state"))  {
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",ui_state);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"netlist_dir"))  {
        Tcl_AppendResult(interp, netlist_dir,NULL);
  }
  else if(!strcmp(argv[2],"pinlayer"))  {  // 20121121
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",PINLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"wirelayer"))  {  // 20121121
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",WIRELAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else if(!strcmp(argv[2],"textlayer"))  {  // 20121121
        char s[30]; // overflow safe 20161122
        my_snprintf(s, S(s), "%d",TEXTLAYER);
        Tcl_AppendResult(interp, s,NULL);
  }
  else {
    fprintf(errfp, "xschem get %s: invalid command.\n", argv[2]);
  }
 }
 else if(!strcmp(argv[1],"set") && argc==3) { // 20171023
  if(!strcmp(argv[2],"horizontal_move"))  { // 20171023
    horizontal_move = atoi(Tcl_GetVar(interp, "horizontal_move", TCL_GLOBAL_ONLY));
    if(horizontal_move) {
      vertical_move=0;
      Tcl_EvalEx(interp,"set vertical_move 0" , -1, TCL_EVAL_GLOBAL);
    }
  }
  else if(!strcmp(argv[2],"vertical_move"))  { // 20171023
    vertical_move = atoi(Tcl_GetVar(interp, "vertical_move", TCL_GLOBAL_ONLY));
    if(vertical_move) {
      horizontal_move=0;
      Tcl_EvalEx(interp,"set horizontal_move 0" , -1, TCL_EVAL_GLOBAL);
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
  if(!strcmp(argv[2],"no_undo"))  { // 20171204
    int s = atoi(argv[3]);
    no_undo=s;
  }
  else if(!strcmp(argv[2],"cairo_font_scale"))  {
    double s = atof(argv[3]);
    if(s>0.1 && s<10.0) cairo_font_scale = s;
  }
  else if(!strcmp(argv[2],"cairo_font_line_spacing"))  {
    double s = atof(argv[3]);
    if(s>0.1 && s<10.0) cairo_font_line_spacing = s;
  }
  else if(!strcmp(argv[2],"cairo_vert_correct"))  {
    double s = atof(argv[3]);
    if(s>-20. && s<20.) cairo_vert_correct = s;
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
  else if(!strcmp(argv[2],"current_type")) { // 20171025
    if(!strcmp(argv[3],"SYMBOL")) {
      current_type=SYMBOL;
    } else {
      current_type=SCHEMATIC;
    }
  }
  else if(!strcmp(argv[2],"netlist_dir"))  {
        my_strdup(&netlist_dir, argv[3]);
  }
  else if(!strcmp(argv[2],"incr_hilight"))  {
        incr_hilight=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"auto_hilight"))  { // 20160413
        auto_hilight=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"netlist_show"))  {
        netlist_show=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"semaphore"))  {
        semaphore=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"cadsnap"))  { // 20161212
        set_snap( atof(argv[3]) );
  }
  else if(!strcmp(argv[2],"cadsnap_noalert"))  { // 20161212
        cadsnap = atof( argv[3] );
  }
  else if(!strcmp(argv[2],"flat_netlist"))  {
        flat_netlist=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"split_files"))  {
        split_files=atoi(argv[3]);
  }
  else if(!strcmp(argv[2],"hspice_netlist"))  {
        hspice_netlist=atoi(argv[3]);
        my_snprintf(name, S(name), "%d", hspice_netlist);
        Tcl_SetVar(interp,"hspice_netlist",name,TCL_GLOBAL_ONLY);
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
  else if(!strcmp(argv[2],"only_probes"))  {  // 20110112
        only_probes=atoi(argv[3]);
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
  else if(!strcmp(argv[2],"ui_state"))  {
     ui_state=atoi(argv[3]);
  }
  else {
    fprintf(errfp, "xschem set %s %s : invalid command.\n", argv[2], argv[3]);
  }
 }

 else if(!strcmp(argv[1],"set_netlist_dir") && argc==3) {
     char s[30]; // overflow safe 20161122
     int force;
   
     force = atoi(argv[2]);
     my_snprintf(s, S(s), "%d", set_netlist_dir(force) );
     Tcl_AppendResult(interp, s,NULL);
 }

 else if(!strcmp(argv[1],"copy"))
 {
    rebuild_selected_array();
    save_selection(2);
 }

 else if(!strcmp(argv[1],"toggle_colorscheme"))
 {
   dark_colorscheme=!dark_colorscheme;
   build_colors(0, 0.0);
   draw();
 }

 else if(!strcmp(argv[1],"color_dim"))
 {
   if(argc==3)
   build_colors(skip_dim_background, atof(argv[2]));
   draw();
 }

 else if(!strcmp(argv[1],"cut"))
 {
    rebuild_selected_array();
    save_selection(2);
    delete();
 }

 else if(!strcmp(argv[1],"only_probes")) { // 20110112
   toggle_only_probes();
 }

 else if(!strcmp(argv[1],"fullscreen"))
 {
   if(debug_var>=1) fprintf(errfp, "scheduler: xschem fullscreen, fullscreen=%d\n", fullscreen);
   toggle_fullscreen();
 }

 else if(!strcmp(argv[1],"windowid")) // used by xschem.tcl for configure events
 {
   windowid();
 }

 else if(!strcmp(argv[1],"paste"))
 {
    merge_file(2,".sch");
 }

 else if(!strcmp(argv[1],"merge"))
 {
    if(argc<3) {
      merge_file(0,"");  // 2nd param not used for merge 25122002
    }
    else {			// 20071215
      merge_file(0,argv[2]);
    }
 }
 else if(!strcmp(argv[1],"attach_pins")) // attach pins to selected component 20171005
 {
   attach_labels_to_inst();
 }
 else if(!strcmp(argv[1],"make_sch")) // make schematic from selected symbol 20171004
 {
   create_sch_from_sym();
 }
 else if(!strcmp(argv[1],"make_symbol"))
 {
   tkeval("tk_messageBox -type okcancel -message {do you want to make symbol view ?}");
   if(strcmp(Tcl_GetStringResult(interp),"ok")==0) 
      if(current_type==SCHEMATIC)
      {
       save_schematic(NULL);
       make_symbol();
      }
 }

 else if(!strcmp(argv[1],"descend"))
 {
    descend();
 }

 else if(!strcmp(argv[1],"edit_symbol"))
 {
    edit_symbol();
 }

 else if(!strcmp(argv[1],"go_back"))
 {
    go_back(1);
 }

 else if(!strcmp(argv[1],"zoom_full"))
 {
    zoom_full(1);
 }

 else if(!strcmp(argv[1],"zoom_box"))
 {
    ui_state|=MENUSTARTZOOM;
 }

 else if(!strcmp(argv[1],"place_symbol"))
 {
   place_symbol(-1,NULL,0.0, 0.0, 0, 0, NULL, 3, 1);
   move_objects(BEGIN,0,0,0);
 }

 else if(!strcmp(argv[1],"place_text"))
 {
   ui_state |= MENUSTARTTEXT; // 20161201
   // place_text(0,mousex_snap, mousey_snap);
   // move_objects(BEGIN,0,0,0);
 }

 else if(!strcmp(argv[1],"zoom_out"))
 {
    view_zoom(0.0);
 }

 else if(!strcmp(argv[1],"line_width") && argc==3)
 {
    change_linewidth(atof(argv[2]), 0);
 }

 else if(!strcmp(argv[1],"sleep") && argc==3)
 {
    usleep(atoi(argv[2])*1000);
 }

 else if(!strcmp(argv[1],"select_all"))
 {
    select_all();
 }

 else if(!strcmp(argv[1],"zoom_in"))
 {
    view_unzoom(0.0);
 }

 else if(!strcmp(argv[1],"copy_objects"))
 {
   copy_objects(BEGIN); 
 }

 else if(!strcmp(argv[1],"move_objects"))
 {
   if(argc==4) {
     move_objects(BEGIN,0,0,0);
     move_objects( END,0,atof(argv[2]), atof(argv[3]));
   }
   else move_objects(BEGIN,0,0,0); 
 }

 else if(!strcmp(argv[1],"log"))
 {
  static int opened=0;
  if(argc==3 && opened==0  )  { errfp = fopen(argv[2], "w");opened=1; } // added check to avoid multiple open 07102004
  else if(argc==2 && opened==1) { fclose(errfp); errfp=stderr;opened=0; }
 }

 else if(!strcmp(argv[1],"exit"))
 {
  if(modified) {
    tkeval("tk_messageBox -type okcancel -message {UNSAVED data: want to exit?}");
    if(strcmp(Tcl_GetStringResult(interp),"ok")==0) Tcl_Eval(interp, "exit");
  }
  else Tcl_Eval(interp, "exit");
 }

 else if(!strcmp(argv[1],"clear"))
 {  
  int cancel;

  cancel=save(1);
  if(!cancel){  // 20161209
     currentsch = 0;
     my_strncpy(schematic[currentsch], "", S(schematic[currentsch]));
     clear_drawing();
     remove_symbols();
     if(argc>=3 && !strcmp(argv[2],"SYMBOL")) { // 20171025
       current_type=SYMBOL;
     } else {
       current_type=SCHEMATIC;
     }
     draw();
     modified=0; // 20171025
     prepared_hash_objects=0;
     prepared_hash_wires=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
 
  }
 }

 else if(!strcmp(argv[1],"reload"))
 {
   remove_symbols();
   load_schematic(1,NULL,1);
   zoom_full(1);
 }

 else if(!strcmp(argv[1],"gensch"))
 {
   char str[1024]; // overflow safe 20161122
   if(argc==3) my_snprintf(str, S(str), "gensch %s", argv[2]);
   else if(argc==4) my_snprintf(str, S(str), "gensch %s %s", argv[2], argv[3]);
   Tcl_EvalEx(interp, str, -1, TCL_EVAL_GLOBAL);
 }

 else if(!strcmp(argv[1],"debug"))
 {
  if(argc==3)  {
     debug_var=atoi(argv[2]);
     Tcl_SetVar(interp,"tcl_debug",argv[2],TCL_GLOBAL_ONLY);
  }

 } else if( !strcmp(argv[1],"getprop")) { // 20171028
   if(argc!=5) {
     Tcl_AppendResult(interp, "xschem getprop needs 3 additional arguments", NULL);
     return TCL_ERROR;
   }
   int i, p, found=0, mult=0, no_of_pins=0;
   char *str_ptr=NULL;
   Tcl_ResetResult(interp);
   if(!strcmp(argv[2],"instance_n")) {
     i=atol(argv[3]);
     if(i<0 || i>lastinst) {
       Tcl_AppendResult(interp, "Index out of range", NULL);
       return TCL_ERROR;
     }
     Tcl_AppendResult(interp, get_tok_value(inst_ptr[i].prop_ptr, argv[4], 0), NULL);
   } else if(!strcmp(argv[2],"instance")) {
     for(i=0;i<lastinst;i++) {
       if(!strcmp(inst_ptr[i].instname, argv[3])) {
         found=1;
         break;
       }
     }
     if(!found) {
       Tcl_AppendResult(interp, "Instance not found", NULL);
       return TCL_ERROR;
     }
     Tcl_AppendResult(interp, get_tok_value(inst_ptr[i].prop_ptr, argv[4], 0), NULL);
   } else if(!strcmp(argv[2], "symbol")) {
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
     Tcl_AppendResult(interp, get_tok_value(instdef[i].prop_ptr, argv[4], 0), NULL);
   } else if(!strcmp(argv[2],"instance_net")) { // 20171029
     for(i=0;i<lastinst;i++) {
       if(!strcmp(inst_ptr[i].instname, argv[3])) {
         found=1;
         break;
       }
     }
     if(!found) {
       Tcl_AppendResult(interp, "Instance not found", NULL);
       return TCL_ERROR;
     }
     prepare_netlist_structs(1);
     no_of_pins= (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
     found=0;
     for(p=0;p<no_of_pins;p++) {
       if(!strcmp( get_tok_value((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][p].prop_ptr,"name",0), argv[4])) {
         //str_ptr =  inst_ptr[i].node[p] ? inst_ptr[i].node[p]: "<UNCONNECTED PIN>";
         str_ptr =  pin_node(i,p,&mult, 0);
         break;
       }
     } // /20171029
     Tcl_AppendResult(interp, str_ptr, NULL);
   }
 } else if(!strcmp(argv[1],"select")) {
   if(argc<3) return TCL_ERROR;
   drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
 
   if(!strcmp(argv[2],"instance") && argc==4) {
     int n=atol(argv[3]);
     int i,found=0;
      
     // 20171006 find by instance name
     for(i=0;i<lastinst;i++) {
       if(!strcmp(inst_ptr[i].instname, argv[3])) {
         select_element(i, SELECTED,0);
         found=1;
         break;
       }
     }
     if(!found && n<lastinst) select_element(n, SELECTED,0);
   }
   else if(!strcmp(argv[2],"wire") && argc==4) {
     int n=atol(argv[3]);
     if(n<lastwire) select_wire(atol(argv[3]), SELECTED, 0);
   }
   else if(!strcmp(argv[2],"text") && argc==4) {
     int n=atol(argv[3]);
     if(n<lasttext) select_text(atol(argv[3]), SELECTED, 0);
   }
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 } else if(!strcmp(argv[1],"instance")) {
   if(argc==7)
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]),NULL, 3, 1);
   if(argc==8)
     place_symbol(-1, argv[2], atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7], 3, 1);
 } else if(!strcmp(argv[1],"snap_wire")) { // 20171022
   ui_state |= MENUSTARTSNAPWIRE;
 } else if(!strcmp(argv[1],"wire")) {   
   double x1,y1,x2,y2;
   int pos;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,WIRE,0,0,NULL);
     drawline(WIRELAYER,NOW, x1,y1,x2,y2);
   }
   else ui_state |= MENUSTARTWIRE;
 } else if(!strcmp(argv[1],"line")) {    
   double x1,y1,x2,y2;
   int pos;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,LINE,rectcolor,0,NULL);
     drawline(rectcolor,NOW, x1,y1,x2,y2);
    
   } 
   else ui_state |= MENUSTARTLINE;
 } else if(!strcmp(argv[1],"rect")) {
   double x1,y1,x2,y2;
   int pos;
   if(argc>=6) {
     x1=atof(argv[2]);
     y1=atof(argv[3]);
     x2=atof(argv[4]);
     y2=atof(argv[5]);
     ORDER(x1,y1,x2,y2);
     pos=-1;
     if(argc==7) pos=atol(argv[6]);
     storeobject(pos, x1,y1,x2,y2,RECT,rectcolor,0,NULL);
     drawrect(rectcolor,NOW, x1,y1,x2,y2);
   }  
   else ui_state |= MENUSTARTRECT;
 } else if(!strcmp(argv[1],"polygon")) {
   ui_state |= MENUSTARTPOLYGON;
 } else if(!strcmp(argv[1],"align")) {
    push_undo();
    round_schematic_to_grid(cadsnap);
    modified=1;
    prepared_hash_objects=0;
    prepared_hash_wires=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
    draw();
 } else if(!strcmp(argv[1],"saveas")) {
  saveas();
 } else if(!strcmp(argv[1],"save")) {
    if(current_type==SYMBOL)
    {
      save_symbol(NULL);
    }
    else
    {
      if(debug_var>=1) fprintf(errfp, "xschem(): saving: current schematic\n");

      if(!strcmp(schematic[currentsch],"")) {   // 20170622 check if unnamed schematic, use saveas in this case...
        saveas();
      } else {
        save(0);
      }
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
  printf("currentsch=%d\n", currentsch);
  for(i=0;i<currentsch;i++)
  {
    printf("previous_instance[%d]=%d\n",i,previous_instance[i]);
    printf("sch_prefix[%d]=%s\n",i,sch_prefix[i]? sch_prefix[i]:"<NULL>");
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
  printf("      xschem redraw\n");
  printf("                   Redraw the window\n");
  printf("      xschem new_window library/cell\n");
  printf("                   start a new window optionally with specified cell\n");
  printf("      xschem edit_in_new_window \n");
  printf("                   start a new window with selected element schematic\n");
  printf("      xschem symbol_in_new_window \n");
  printf("                   start a new window with selected element schematic\n");
  printf("      xschem globals\n");
  printf("                   print information about global variables\n");
  printf("      xschem inst_ptr n\n");
  printf("                   return inst_ptr of inst_ptr[n]\n");
  printf("      xschem gensch\n");
  printf("                   create new schematic cell from pin names\n");
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
  printf("      xschem edit_symbol\n");
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
  printf("      xschem delete_hilight_net\n");
  printf("                   unhilight  all nets/pins\n");
  printf("      xschem print [color]\n");
  printf("                   print schematic (optionally in color)\n");
  printf("      xschem search [sub]token value\n");
  printf("                   hilight instances which match tok=val property,\n");
  printf("                   substring search if sub given\n");
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
 else {
   fprintf(errfp, "xschem %s: invalid command.\n", argv[1]);
 }
 return TCL_OK;
}

void tkeval(char str[])
{
 char string[1024];	// overflow safe

 my_strncpy(string, str,1023);
 Tcl_EvalEx(interp, string, -1, TCL_EVAL_GLOBAL);
}
