/* File: spice_netlist.c
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

void global_spice_netlist(int global)  /* netlister driver */
{
 int first;
 FILE *fd;
 const char *str_tmp;
 int mult;
 unsigned int *stored_flags;
 int i, save_ok;
 static char *type=NULL;
 static char *place=NULL; /*20121223 */
 char netl[PATH_MAX]; /* overflow safe 20161122 */
 char netl2[PATH_MAX]; /* 20081211 overflow safe 20161122 */
 char netl3[PATH_MAX]; /* 20081211 overflow safe 20161122 */

 if(current_type==SYMBOL) {
   tcleval("alert_ {This is a symbol, no netlisting can be done.\n"
           "If this is a schematic delete any 'type=...'\n"
           "from global properties, save and restart xschem}");
   return;
 }
 if(modified) {
   save_ok = save_schematic(schematic[currentsch]);
   if(save_ok == -1) return;
 }
 statusmsg("",2);  /* clear infowindow */
 record_global_node(2, NULL, NULL); /* delete list of global nodes */
 netlist_count=0;
 my_snprintf(netl, S(netl), "%s/%s", netlist_dir, skip_dir(schematic[currentsch]) );
 fd=fopen(netl, "w");
 my_snprintf(netl3, S(netl3), "%s", skip_dir(schematic[currentsch]));
 if(fd==NULL){ 
   if(debug_var>=0) fprintf(errfp, "global_spice_netlist(): problems opening netlist file\n");
   return;
 }
 if(debug_var>=1) fprintf(errfp, "global_spice_netlist(): opening %s for writing\n",netl);
 fprintf(fd,"**.subckt %s", skip_dir( schematic[currentsch]) );

 /* netlist_options */
 for(i=0;i<lastinst;i++)
 {
   if( (inst_ptr[i].ptr+instdef)->type && !strcmp((inst_ptr[i].ptr+instdef)->type,"netlist_options") ) {
     netlist_options(i);
   }
 }

 /* print top subckt ipin/opins */
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(380, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
  if(debug_var>=1) fprintf(errfp, "global_spice_netlist(): |%s|\n", type);
  if( type && !strcmp(type,"netlist_options") ) {
    continue;
  }
  if( type && !(strcmp(type,"ipin")&&strcmp(type,"opin")&&strcmp(type,"iopin")) )
  {
   str_tmp = expandlabel ( get_tok_value(inst_ptr[i].prop_ptr,"lab",0) ,&mult);
   if(debug_var>=1) fprintf(errfp, "global_spice_netlist(): |%s|\n", 
       get_tok_value(inst_ptr[i].prop_ptr,"lab",0));
   /*must handle  invalid node names */
   fprintf(fd, " %s", str_tmp ? str_tmp : "(NULL)" );
  }
 }
 fprintf(fd,"\n");

 spice_netlist(fd, 0);
 
 /* 20100217 */

 /* 20150922 */
 first = 0;
 for(i=0;i<lastinst;i++) /* print netlist_commands of top level cell with no 'place=end' property */
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "spice_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(381, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
  my_strdup(382, &place,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"place",0));  /* 20121223 */
  if( type && !strcmp(type,"netlist_commands") ) {
   if(!place || strcmp(place, "end" )) {  /* 20121223 */
     my_strdup(383, &place,get_tok_value(inst_ptr[i].prop_ptr,"place",0));  /*20160920 */
     if(!place || strcmp(place, "end" )) {  /*20160920 */
       if(first == 0) fprintf(fd,"**** begin user architecture code\n");
       first++;
       print_spice_element(fd, i) ;  /* this is the element line  */
     }
   }
  }
 }

 netlist_count++;

 if(schprop && schprop[0]) {
   if(first == 0) fprintf(fd,"**** begin user architecture code\n");
   first++;
   fprintf(fd, "%s\n", schprop);
 }
 if(first) fprintf(fd,"**** end user architecture code\n");
 /* /20100217 */

 fprintf(fd, "**.ends\n");


 if(split_files) { /* 20081205 */
   fclose(fd);
   my_snprintf(netl2, S(netl2), "netlist {%s} noshow {%s.spice}", netl3, netl3);
   tcleval(netl2);
   if(debug_var==0) unlink(netl);
 }

 /* preserve current level instance flags before descending hierarchy for netlisting, restore later */
 stored_flags = my_calloc(146, lastinst, sizeof(unsigned int));
 for(i=0;i<lastinst;i++) stored_flags[i] = inst_ptr[i].flags & 4;

 if(global)
 {
   unselect_all();
   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   load_schematic(1, schematic[currentsch], 0); /* 20180927 */

   currentsch++;
    if(debug_var>=2) fprintf(errfp, "global_spice_netlist(): last defined symbol=%d\n",lastinstdef);
   for(i=0;i<lastinstdef;i++)
   {
    if( strcmp(get_tok_value(instdef[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue; /* 20070726 */

    if(strcmp(instdef[i].type,"subcircuit")==0 && check_lib(instdef[i].name)) /* 20150409 */
    {
      if( split_files && strcmp(get_tok_value(instdef[i].prop_ptr,"vhdl_netlist",0),"true")==0 )
        vhdl_block_netlist(fd, i); /* 20081209 */
      else if(split_files && strcmp(get_tok_value(instdef[i].prop_ptr,"verilog_netlist",0),"true")==0 )
        verilog_block_netlist(fd, i); /* 20081209 */
      else
        if( strcmp(get_tok_value(instdef[i].prop_ptr,"spice_primitive",0),"true") )
          spice_block_netlist(fd, i); /* 20081205 */
    }
   }
   /*clear_drawing(); */
   my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
   currentsch--;
   unselect_all();
   /* remove_symbols(); */
   load_schematic(1, schematic[currentsch], 0); /* 20180927 */
   /* symbol vs schematic pin check, we do it here since now we have ALL symbols loaded */
   sym_vs_sch_pins();

   /* restore hilight flags from errors found analyzing top level before descending hierarchy */
   for(i=0;i<lastinst; i++) inst_ptr[i].flags |= stored_flags[i];
   my_free(&stored_flags);

   draw_hilight_net(1);
 }

 /* print globals nodes found in netlist 28032003 */
 record_global_node(0,fd,NULL);

 /* =================================== 20121223 */
 if(!split_files) {
   first = 0;
   for(i=0;i<lastinst;i++) /* print netlist_commands of top level cell with 'place=end' property */
   {
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue; /* 20140416 */
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "spice_ignore",0 ), "true") ) {
      continue;
    }
    my_strdup(384, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
    my_strdup(385, &place,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"place",0));  /* 20121223 */
    if( type && !strcmp(type,"netlist_commands") ) {
     if(place && !strcmp(place, "end" )) {  /* 20121223 */
       if(first == 0) fprintf(fd,"**** begin user architecture code\n");
       first++;
       print_spice_element(fd, i) ;  /* 20160920 */
     } else { /* 20160920 */
       my_strdup(386, &place,get_tok_value(inst_ptr[i].prop_ptr,"place",0));  /*20160920 */
       if(place && !strcmp(place, "end" )) {  /* 20160920 */
         if(first == 0) fprintf(fd,"**** begin user architecture code\n");
         first++;
         print_spice_element(fd, i) ;  /* 20160920 */
       }
     }
    } /* netlist_commands */
   }
   if(first) fprintf(fd,"**** end user architecture code\n");
 }
 /* =================================== /20121223 */

 /* 20150922 added split_files check */
 if(!split_files) fprintf(fd, ".end\n"); /* 20081202 */

 if(debug_var>=1) fprintf(errfp, "global_spice_netlist(): starting awk on netlist!\n");


 if(!split_files) {
   fclose(fd);
   if(netlist_show) {
    my_snprintf(netl2, S(netl2), "netlist {%s} show {%s.spice}", netl3, netl3);
    tcleval(netl2);
   }
   else {
    my_snprintf(netl2, S(netl2), "netlist {%s} noshow {%s.spice}", netl3, netl3);
    tcleval(netl2);
   }
   if(!debug_var) unlink(netl);
 }

}


void spice_block_netlist(FILE *fd, int i)  /*20081223 */
{
 int spice_stop=0; /* 20111113 */
 char netl[PATH_MAX];
 char netl2[PATH_MAX];  /* 20081202 */
 char netl3[PATH_MAX];  /* 20081202 */
 char filename[PATH_MAX];
 const char *str_tmp;
 /* int j; */
 /* int mult; */
 static char *extra=NULL;

     /* 20111113 */
     if(!strcmp( get_tok_value(instdef[i].prop_ptr,"spice_stop",0),"true") )
        spice_stop=1;
     else
        spice_stop=0;

     if(split_files) {          /* 20081203 */
       my_snprintf(netl, S(netl), "%s/%s", netlist_dir,  skip_dir(instdef[i].name) );
       if(debug_var>=1)  fprintf(errfp, "spice_block_netlist(): split_files: netl=%s\n", netl);
       fd=fopen(netl, "w");
       my_snprintf(netl3, S(netl3), "%s", skip_dir(instdef[i].name) );
     }

     fprintf(fd, "\n* expanding   symbol:  %s # of pins=%d\n\n", 
           instdef[i].name,instdef[i].rects[PINLAYER] );
  
     fprintf(fd, ".subckt %s ",skip_dir(instdef[i].name));
     print_spice_subckt(fd, i);

     my_strdup(387, &extra, get_tok_value(instdef[i].prop_ptr,"extra",0) ); /* 20081206 */
     fprintf(fd, "%s ", extra ? extra : "" );
     
     /* 20081206 new get_sym_template does not return token=value pairs where token listed in extra */
     fprintf(fd, "%s", get_sym_template(instdef[i].templ, extra)); /* 20150409 */
     fprintf(fd, "\n");
  
     if((str_tmp = get_tok_value(instdef[i].prop_ptr, "schematic",0 ))[0]) {
       my_strncpy(filename, abs_sym_path(str_tmp, ""), S(filename));
       load_schematic(1,filename, 0);
     } else {
       load_schematic(1, add_ext(abs_sym_path(instdef[i].name, ""), ".sch") ,0); /* 20190518 */
     }
     spice_netlist(fd, spice_stop);  /* 20111113 added spice_stop */
     netlist_count++;

     /* 20100217 */
     if(schprop && schprop[0]) {
       fprintf(fd,"**** begin user architecture code\n");
       fprintf(fd, "%s\n", schprop);
       fprintf(fd,"**** end user architecture code\n");
     }
     /* /20100217 */

     fprintf(fd, ".ends\n\n");
     if(split_files) { /* 20081204 */
       fclose(fd);
       my_snprintf(netl2, S(netl2), "netlist {%s} noshow {%s.spice}", netl3, netl3);
       tcleval(netl2);
       if(debug_var==0) unlink(netl);
     }

}

void spice_netlist(FILE *fd, int spice_stop )
{
 int i;
 static char *type=NULL;

 prepared_netlist_structs = 0;
 prepare_netlist_structs(1);
 /* set_modify(1); */ /* 20160302 prepare_netlist_structs could change schematic (wire node naming for example) */
 traverse_node_hash();  /* print all warnings about unconnected floatings etc */
 if(!spice_stop) {  /* 20111113 */
   for(i=0;i<lastinst;i++) /* print first ipin/opin defs ... */
   {
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue; /* 20140416 */
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "spice_ignore",0 ), "true") ) {
      continue;
    }
    my_strdup(388, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
    if( type && (/*strcmp(type,"label") && */ strcmp(type,"ipin")&&strcmp(type,"opin")&&strcmp(type,"iopin") )==0)
    {
      print_spice_element(fd, i) ;  /* this is the element line  */
    }
   }
  
   for(i=0;i<lastinst;i++) /* ... then print other lines */
   {
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"spice_ignore",0),"true")==0 ) continue; /* 20140416 */
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "spice_ignore",0 ), "true") ) {/*20070726 */
      continue;                                                                                   /*20070726 */
    }                                                                                             /*20070726 */
  
    my_strdup(390, &type,(inst_ptr[i].ptr+instdef)->type); /* 20150409 */
    if( type && (strcmp(type,"label")&&strcmp(type,"ipin")&& strcmp(type,"opin")&&strcmp(type,"iopin")))
    {
      if(!strcmp(type,"netlist_commands") && netlist_count==0) continue; /* already done in global_spice_netlist */
      if(netlist_count && 
         !strcmp(get_tok_value(inst_ptr[i].prop_ptr, "only_toplevel", 0), "true")) continue; /* 20160418 */
      if(!strcmp(type,"netlist_commands")) {
        fprintf(fd,"**** begin user architecture code\n");
        print_spice_element(fd, i) ;  /* this is the element line  */
        fprintf(fd,"**** end user architecture code\n");
      } else {
        print_spice_element(fd, i) ;  /* this is the element line  */
      }
    }
   }
 }
 if(!netlist_count) redraw_hilights(); /* draw_hilight_net(1); */
}


