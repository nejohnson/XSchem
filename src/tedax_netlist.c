/* File: tedax_netlist.c
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

void global_tedax_netlist(int global)  // netlister driver
{
 FILE *fd;
 int i;
 char netl[4096]; // overflow safe 20161122
 char netl2[4096]; // 20081211 overflow safe 20161122
 char netl3[4096]; // 20081211 overflow safe 20161122


 if(current_type==SYMBOL) return;
 statusmsg("",2);  // clear infowindow
 netlist_count=0;
 if(!strcmp(schematic[currentsch],""))
 {
   char name[1024];
   my_snprintf(name, S(name), "savefile %s.sch .sch",schematic[currentsch]);
   if(debug_var>=1) fprintf(errfp, "global_tedax_netlist(): saving: %s\n",name);
   tkeval(name);
   my_strncpy(schematic[currentsch], Tcl_GetStringResult(interp), S(schematic[currentsch]));
   if(!strcmp(schematic[currentsch],"")) return;
   save_schematic(schematic[currentsch]);
 }

 my_snprintf(netl, S(netl), "%s/%s", netlist_dir, skip_dir(schematic[currentsch]) );
 fd=fopen(netl, "w");
 my_snprintf(netl3, S(netl3), "%s", skip_dir(schematic[currentsch]));

 if(fd==NULL){ 
   if(debug_var>=1) fprintf(errfp, "global_tedax_netlist(): problems opening netlist file\n");
   return;
 }
 if(debug_var>=1) fprintf(errfp, "global_tedax_netlist(): opening %s for writing\n",netl);
 fprintf(fd,"tEDAx v1\nbegin netlist v1 %s\n", skip_dir( schematic[currentsch]) );

 tedax_netlist(fd, 0);

 //// 20100217
 //fprintf(fd,"**** begin user architecture code\n");
 //netlist_count++;
 //if(schprop && schprop[0]) fprintf(fd, "%s\n", schprop);
 //fprintf(fd,"**** end user architecture code\n");
 //// /20100217

 fprintf(fd, "end netlist\n");
 if(modified) save_schematic(NULL);

 if(0) // was if(global) ... 20180901 no hierarchical tEDAx netlist for now
 {
   remove_symbols(); // 20161205 ensure all unused symbols purged before descending hierarchy
   load_schematic(1,NULL,0);

   currentsch++;
    if(debug_var>=2) fprintf(errfp, "global_tedax_netlist(): last defined symbol=%d\n",lastinstdef);
   for(i=0;i<lastinstdef;i++)
   {
    if( strcmp(get_tok_value(instdef[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue; // 20070726

    // if(strcmp(get_tok_value(instdef[i].prop_ptr,"type",0),"subcircuit")==0 && check_lib(instdef[i].name)) // 20150409
    if(strcmp(instdef[i].type,"subcircuit")==0 && check_lib(instdef[i].name)) // 20150409
    {
      tedax_block_netlist(fd, i); // 20081205
    }
   }
   //clear_drawing();
   my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
   currentsch--;
   remove_symbols();
   load_schematic(1,NULL,0);
 }

 // print globals nodes found in netlist 28032003
 record_global_node(0,fd,NULL);

 if(debug_var>=1) fprintf(errfp, "global_tedax_netlist(): starting awk on netlist!\n");

 fclose(fd);
 if(netlist_show) {
  my_snprintf(netl2, S(netl2), "netlist %s show %s.tdx", netl3, netl3);
  tkeval(netl2);
 }
 else {
  my_snprintf(netl2, S(netl2), "netlist %s noshow %s.tdx", netl3, netl3);
  tkeval(netl2);
 }
 if(!debug_var) unlink(netl);
}


void tedax_block_netlist(FILE *fd, int i)  //20081223
{
 int j;
 int tedax_stop=0; // 20111113
 char netl[4096];
 char netl2[4096];  // 20081202
 char netl3[4096];  // 20081202
 char *str_tmp;
 int mult;
 static char *extra=NULL;

     // 20111113
     if(!strcmp( get_tok_value(instdef[i].prop_ptr,"tedax_stop",0),"true") )
        tedax_stop=1;
     else
        tedax_stop=0;

     fprintf(fd, "\n* expanding   symbol:  %s # of pins=%d\n\n", 
           instdef[i].name,instdef[i].rects[PINLAYER] );
  
     fprintf(fd, ".subckt %s ",skip_dir(instdef[i].name));
     for(j=0;j<instdef[i].rects[PINLAYER];j++)
     {
       str_tmp=
         expandlabel(get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"name",0), &mult);
       if(str_tmp)
         fprintf(fd,"%s ",str_tmp);
       else 
         fprintf(fd,"<NULL> ");
     }
     my_strdup(&extra, get_tok_value(instdef[i].prop_ptr,"extra",0) ); // 20081206
     fprintf(fd, "%s ", extra ? extra : "" );
     
     // 20081206 new get_sym_template does not return token=value pairs where token listed in extra
     // fprintf(fd, "%s", get_sym_template(get_tok_value(instdef[i].prop_ptr,"template",2), extra)); // 20150409
     fprintf(fd, "%s", get_sym_template(instdef[i].templ, extra)); // 20150409
     fprintf(fd, "\n");
  
     //clear_drawing();
     my_strncpy(schematic[currentsch],instdef[i].name, S(schematic[currentsch]));
     load_schematic(1,NULL,0);
     tedax_netlist(fd, tedax_stop);  // 20111113 added tedax_stop
     netlist_count++;

     // 20100217
     fprintf(fd,"**** begin user architecture code\n");
     if(schprop && schprop[0]) fprintf(fd, "%s\n", schprop);
     fprintf(fd,"**** end user architecture code\n");
     // /20100217

     fprintf(fd, ".ends\n\n");
     if(split_files) { // 20081204
       fclose(fd);
       my_snprintf(netl2, S(netl2), "netlist %s noshow %s.tdx", netl3, netl3);
       tkeval(netl2);
       if(debug_var==0) unlink(netl);
     }

}

void tedax_netlist(FILE *fd, int tedax_stop )
{
 int i;
 static char *type=NULL;
 static char *place=NULL;  // 20121223

 prepare_netlist_structs(0);
 modified=1; // 20160302 prepare_netlist_structs could change schematic (wire node naming for example)
 traverse_node_hash();  // print all warnings about unconnected floatings etc


 if(!tedax_stop) {  // 20111113
   for(i=0;i<lastinst;i++) // print first ipin/opin defs ...
   {
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue; // 20140416
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "tedax_ignore",0 ), "true") ) {
      continue;
    }
    // my_strdup(&type,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"type",0)); // 20150409
    my_strdup(&type,(inst_ptr[i].ptr+instdef)->type); // 20150409
    my_strdup(&place,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"place",0));  // 20121223
    if( type && (/*strcmp(type,"label") && */ strcmp(type,"ipin")&&strcmp(type,"opin")&&strcmp(type,"iopin") )==0)
    {
      print_tedax_element(fd, i) ;  // this is the element line 
    }
   }
  
   for(i=0;i<lastinst;i++) // ... then print other lines
   {
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"tedax_ignore",0),"true")==0 ) continue; // 20140416
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "tedax_ignore",0 ), "true") ) {//20070726
      continue;                                                                                   //20070726
    }                                                                                             //20070726
  
    // my_strdup(&type,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"type",0)); // 20150409
    my_strdup(&type,(inst_ptr[i].ptr+instdef)->type); // 20150409
    my_strdup(&place,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"place",0));  // 20121223
    if( type && (strcmp(type,"label")&&strcmp(type,"ipin")&& strcmp(type,"opin")&&strcmp(type,"iopin")))
    {
      if(!strcmp(type,"netlist_commands") && netlist_count==0) continue; // already done in global_tedax_netlist
      if(netlist_count && 
         !strcmp(get_tok_value(inst_ptr[i].prop_ptr, "only_toplevel", 0), "true")) continue; // 20160418
      print_tedax_element(fd, i) ;  // this is the element line 
    }
   }
 }
 if(!netlist_count) draw_hilight_net();
 //delete_netlist_structs(); // 20161222 done in prepare_netlist_structs() when needed

}

