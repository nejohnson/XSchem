/* File: verilog_netlist.c
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

void global_verilog_netlist(int global)  /* netlister driver */
{
 FILE *fd;
 char *str_tmp;
 static char *sig_type = NULL;
 static char *port_value = NULL;
 static char *tmp_string=NULL;
 int i, tmp;
 char netl[PATH_MAX];  /* overflow safe 20161122 */
 char netl2[PATH_MAX]; /* 20081203  overflow safe 20161122 */
 char netl3[PATH_MAX]; /* 20081203  overflow safe 20161122 */
 static char *type=NULL;

 if(current_type==SYMBOL) return;
 statusmsg("",2);  /* clear infowindow */
 netlist_count=0; 
 /* top sch properties used for library use declarations and type definitions */
 /* to be printed before any entity declarations */

 if(!strcmp(schematic[currentsch],""))
 {
   char name[PATH_MAX];
   my_snprintf(name, S(name), "savefile {%s.sch} .sch",schematic[currentsch]);
   if(debug_var>=1) fprintf(errfp, "global_spice_netlist(): saving: %s\n",name);
   tcleval(name);
   my_strncpy(schematic[currentsch], Tcl_GetStringResult(interp), S(schematic[currentsch]));
   if(!strcmp(schematic[currentsch],"")) return;
   save_schematic(schematic[currentsch]);
 }

 my_snprintf(netl, S(netl), "%s/%s", netlist_dir, skip_dir(schematic[currentsch]) );
 fd=fopen(netl, "w");
 my_snprintf(netl3, S(netl3), "%s", skip_dir(schematic[currentsch]));



 if(fd==NULL){ 
   if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): problems opening netlist file\n");
   return;
 }
 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): opening %s for writing\n",netl);



/* print verilog timescale 10102004 */
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(544, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"timescale"))==0)
  {
   str_tmp = get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr ,"format",0);
   my_strdup(545, &tmp_string, str_tmp);
   fprintf(fd, "%s\n", str_tmp ? translate(i, tmp_string) : "(NULL)");
  }
 }



 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level entity\n");
 fprintf(fd,"module %s (\n", skip_dir( schematic[currentsch]) );
 /* flush data structures (remove unused symbols) */
 if(modified) save_schematic(schematic[currentsch]);
 remove_symbols();  /* removed 25122002, readded 04112003 */
 load_schematic(1,schematic[currentsch],0);  /* 20180927 */


 /* print top subckt port directions */
 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level out pins\n");
 tmp=0;
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(546, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"opin"))==0)
  {
   if(tmp) fprintf(fd, " ,\n"); 
   tmp++;
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   fprintf(fd, "  %s", str_tmp ? str_tmp : "(NULL)");
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level inout pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(547, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"iopin"))==0)
  {
   if(tmp) fprintf(fd, " ,\n");
   tmp++;
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   fprintf(fd, "  %s", str_tmp ? str_tmp : "(NULL)");
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level input pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(548, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"ipin"))==0)
  {
   if(tmp) fprintf(fd, " ,\n");
   tmp++;
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   fprintf(fd, "  %s", str_tmp ? str_tmp : "<NULL>");
  }
 }

 fprintf(fd,"\n);\n");

 /* 20071006 print top level params if defined in symbol */
 load_symbol_definition( schematic[currentsch], NULL );
 print_verilog_param(fd,lastinstdef-1);  /* added print top level params */
 remove_symbol();
 /* 20071006 end */



 /* print top subckt port types */
 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level out pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(549, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"opin"))==0)
  {
   my_strdup(550, &port_value,get_tok_value(inst_ptr[i].prop_ptr,"value",0));
   my_strdup(551, &sig_type,get_tok_value(inst_ptr[i].prop_ptr,"verilog_type",0));
   if(!sig_type || sig_type[0]=='\0') my_strdup(552, &sig_type,"wire"); /* 20070720 changed reg to wire */
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   fprintf(fd, "  output %s ;\n", str_tmp ? str_tmp : "(NULL)");
   fprintf(fd, "  %s %s ", sig_type, str_tmp ? str_tmp : "(NULL)");
   /* 20140410 */
   if(port_value && port_value[0]) fprintf(fd," = %s", port_value);
   fprintf(fd, ";\n");
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level inout pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(553, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"iopin"))==0)
  {
   my_strdup(554, &port_value,get_tok_value(inst_ptr[i].prop_ptr,"value",0));
   my_strdup(555, &sig_type,get_tok_value(inst_ptr[i].prop_ptr,"verilog_type",0));
   if(!sig_type || sig_type[0]=='\0') my_strdup(556, &sig_type,"wire");
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   fprintf(fd, "  inout %s ;\n", str_tmp ? str_tmp : "(NULL)");
   fprintf(fd, "  %s %s ", sig_type, str_tmp ? str_tmp : "(NULL)");
   /* 20140410 */
   if(port_value && port_value[0]) fprintf(fd," = %s", port_value);
   fprintf(fd, ";\n");
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): printing top level input pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
  if(inst_ptr[i].ptr<0) continue;
  if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
    continue;
  }
  my_strdup(557, &type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"ipin"))==0)
  {
   my_strdup(558, &port_value,get_tok_value(inst_ptr[i].prop_ptr,"value",0));
   my_strdup(559, &sig_type,get_tok_value(inst_ptr[i].prop_ptr,"verilog_type",0));
   if(!sig_type || sig_type[0]=='\0') my_strdup(560, &sig_type,"wire");
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   fprintf(fd, "  input %s ;\n", str_tmp ? str_tmp : "<NULL>");
   fprintf(fd, "  %s %s ", sig_type, str_tmp ? str_tmp : "<NULL>");
   /* 20140410 */
   if(port_value && port_value[0]) fprintf(fd," = %s", port_value);
   fprintf(fd, ";\n");
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): netlisting  top level\n");
 verilog_netlist(fd, 0);
 netlist_count++;
 fprintf(fd,"---- begin user architecture code\n");

 /* 20180124 */
 for(i=0;i<lastinst;i++) {
   if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
   if(inst_ptr[i].ptr<0) continue;
   if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
     continue;
   }
   my_strdup(561, &type,(inst_ptr[i].ptr+instdef)->type);
   if(type && !strcmp(type,"netlist_commands")) {
     fprintf(fd, "%s\n", get_tok_value(inst_ptr[i].prop_ptr,"value",2)); /* 20180124 */
   }
 }


 if(schverilogprop && schverilogprop[0]) fprintf(fd, "%s\n", schverilogprop); 
 fprintf(fd,"---- end user architecture code\n");
 fprintf(fd, "endmodule\n");

 if(split_files) { /* 20081205 */
   fclose(fd);
   my_snprintf(netl2, S(netl2), "netlist {%s} noshow {%s.v}", netl3, netl3);
   tcleval(netl2);
   if(debug_var==0) unlink(netl);
 }


 if(global)
 {
   if(modified) save_schematic(schematic[currentsch]); /* 20160302 prepare_netlist_structs (called above from verilog_netlist()  */
                                 /* may change wire node labels, so save. */

   remove_symbols(); /* 20161205 ensure all unused symbols purged before descending hierarchy */
   load_schematic(1,schematic[currentsch],0); /* 20180927 */

   currentsch++;
   if(debug_var>=2) fprintf(errfp, "global_verilog_netlist(): last defined symbol=%d\n",lastinstdef);
   for(i=0;i<lastinstdef;i++)
   {
    if( strcmp(get_tok_value(instdef[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20070726 */

    if(strcmp(get_tok_value(instdef[i].prop_ptr,"type",0),"subcircuit")==0&& 
       check_lib(instdef[i].name))
    {
      if( split_files && strcmp(get_tok_value(instdef[i].prop_ptr,"vhdl_netlist",0),"true")==0 )
        vhdl_block_netlist(fd, i); /* 20081209 */
      else if(split_files && strcmp(get_tok_value(instdef[i].prop_ptr,"spice_netlist",0),"true")==0 )
        spice_block_netlist(fd, i); /* 20081209 */
      else 
        verilog_block_netlist(fd, i); /* 20081205 */
    }
   }
   my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
   currentsch--;
   remove_symbols();
   load_schematic(1,schematic[currentsch],0);
 }

 if(debug_var>=1) fprintf(errfp, "global_verilog_netlist(): starting awk on netlist!\n");
 if(!split_files) {
   fclose(fd);
   if(netlist_show) {
    my_snprintf(netl2, S(netl2), "netlist {%s} show {%s.v}", netl3, netl3);
    tcleval(netl2);
   }
   else {
    my_snprintf(netl2, S(netl2), "netlist {%s} noshow {%s.v}", netl3, netl3);
    tcleval(netl2);
   }
   if(debug_var == 0 ) unlink(netl);
 }
}


void verilog_block_netlist(FILE *fd, int i)  /*20081205 */
{
 int j, l, tmp;
 int verilog_stop=0;
 static char *dir_tmp = NULL;
 static char *sig_type = NULL;
 static char *port_value = NULL;
 static char *type = NULL; /* 20180124 */
 char netl[PATH_MAX];
 char netl2[PATH_MAX];  /* 20081202 */
 char netl3[PATH_MAX];  /* 20081202 */
 char *str_tmp;

     if(!strcmp( get_tok_value(instdef[i].prop_ptr,"verilog_stop",0),"true") ) 
        verilog_stop=1;
     else
        verilog_stop=0;


     if(split_files) {          /* 20081203 */
       my_snprintf(netl, S(netl), "%s/%s", netlist_dir,  skip_dir(instdef[i].name) );
       if(debug_var>=1)  fprintf(errfp, "global_vhdl_netlist(): split_files: netl=%s\n", netl);
       fd=fopen(netl, "w");
       my_snprintf(netl3, S(netl3), "%s", skip_dir(instdef[i].name) );

     }

     if(debug_var>=1) fprintf(errfp, "verilog_block_netlist(): expanding %s\n",  instdef[i].name);
     fprintf(fd, "\n// expanding   symbol:  %s # of pins=%d\n\n", 
           instdef[i].name,instdef[i].rects[PINLAYER] );

     verilog_stop? load_schematic(0,instdef[i].name,0) :  load_schematic(1,instdef[i].name,0);

     fprintf(fd, "module %s (\n", skip_dir(instdef[i].name));
     /*print_generic(fd, "entity", i); */ /* 02112003 */

     if(debug_var>=1) fprintf(errfp, "verilog_block_netlist():       entity ports\n");

     /* print ports directions */
     tmp=0;
     for(j=0;j<instdef[i].rects[PINLAYER];j++)
     {
       my_strdup(562, &port_value, get_tok_value(
                 instdef[i].boxptr[PINLAYER][j].prop_ptr,"value",2) );
       my_strdup(563, &dir_tmp, get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"dir",0) );
       str_tmp = get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"name",0);
       if(tmp) fprintf(fd, " ,\n"); 
       tmp++;
       fprintf(fd,"  %s", str_tmp ? str_tmp : "<NULL>");
     }
     fprintf(fd, "\n);\n");

     /*16112003 */
     if(debug_var>=1) fprintf(errfp, "verilog_block_netlist():       entity generics\n");
     /* print module  default parameters */
     print_verilog_param(fd,i);




     /* print port types */
     tmp=0;
     for(j=0;j<instdef[i].rects[PINLAYER];j++)
     {
       my_strdup(564, &sig_type,get_tok_value(
                 instdef[i].boxptr[PINLAYER][j].prop_ptr,"verilog_type",0));
       my_strdup(565, &port_value, get_tok_value(
                 instdef[i].boxptr[PINLAYER][j].prop_ptr,"value",2) );
       my_strdup(566, &dir_tmp, get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"dir",0) );
       if(strcmp(dir_tmp,"in")){
          if(!sig_type || sig_type[0]=='\0') my_strdup(567, &sig_type,"wire"); /* 20070720 changed reg to wire */
       } else {
          if(!sig_type || sig_type[0]=='\0') my_strdup(568, &sig_type,"wire");
       }
       str_tmp = get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"name",0);
       fprintf(fd,"  %s %s ;\n", 
         strcmp(dir_tmp,"in")? ( strcmp(dir_tmp,"out")? "inout" :"output"  ) : "input",
         str_tmp ? str_tmp : "<NULL>");
       fprintf(fd,"  %s %s", 
         sig_type, 
         str_tmp ? str_tmp : "<NULL>");
       if(port_value &&port_value[0])
         fprintf(fd," = %s", port_value);
       fprintf(fd," ;\n");
     }

     if(debug_var>=1) fprintf(errfp, "verilog_block_netlist():       netlisting %s\n", skip_dir( schematic[currentsch]));
     verilog_netlist(fd, verilog_stop);
     netlist_count++;
     fprintf(fd,"---- begin user architecture code\n");

     /* 20180124 */
     for(l=0;l<lastinst;l++) {
       if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
       if(inst_ptr[l].ptr<0) continue;
       if(!strcmp(get_tok_value( (inst_ptr[l].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) {
         continue;
       }
       if(netlist_count &&
         !strcmp(get_tok_value(inst_ptr[l].prop_ptr, "only_toplevel", 0), "true")) continue; /* 20160418 */

       my_strdup(569, &type,(inst_ptr[l].ptr+instdef)->type);
       if(type && !strcmp(type,"netlist_commands")) {
         fprintf(fd, "%s\n", get_tok_value(inst_ptr[l].prop_ptr,"value",2)); /* 20180124 */
       }
     }

     if(schverilogprop && schverilogprop[0]) fprintf(fd, "%s\n", schverilogprop);
     fprintf(fd,"---- end user architecture code\n");
     fprintf(fd, "endmodule\n");
     if(split_files) { /* 20081204 */
       fclose(fd);
       my_snprintf(netl2, S(netl2), "netlist {%s} noshow {%s.v}", netl3, netl3);
       tcleval(netl2);
       if(debug_var==0) unlink(netl);
     }


}

void verilog_netlist(FILE *fd , int verilog_stop)
{
 int i;
 static char *type=NULL;

 prepare_netlist_structs(0);
 set_modify(1); /* 20160302 prepare_netlist_structs could change schematic (wire node naming for example) */
 if(debug_var>=2) fprintf(errfp, "verilog_netlist(): end prepare_netlist_structs\n");
 traverse_node_hash();  /* print all warnings about unconnected floatings etc */

 if(debug_var>=2) fprintf(errfp, "verilog_netlist(): end traverse_node_hash\n");

 fprintf(fd,"---- begin signal list\n");
 if(!verilog_stop) print_verilog_signals(fd);
 fprintf(fd,"---- end signal list\n");


 if(!verilog_stop)
 {
   for(i=0;i<lastinst;i++) /* ... print all element except ipin opin labels use package */
   {
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"verilog_ignore",0),"true")==0 ) continue; /* 20140416 */
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "verilog_ignore",0 ), "true") ) { /*20070726 */
      continue;                                                                                   /*20070726 */
    }                                                                                             /*20070726 */

    if(debug_var>=2) fprintf(errfp, "verilog_netlist():       into the netlisting loop\n");
    my_strdup(570, &type,(inst_ptr[i].ptr+instdef)->type);
    if( type && 
       ( strcmp(type,"label")&&
         strcmp(type,"ipin")&&
         strcmp(type,"opin")&&
         strcmp(type,"iopin")&&
         strcmp(type,"use")&&
         strcmp(type,"netlist_commands")&& /* 20180124 */
         strcmp(type,"timescale")&&
         strcmp(type,"package")  &&
         strcmp(type,"attributes") &&
         strcmp(type,"port_attributes")  &&
         strcmp(type,"arch_declarations")
       ))
    {
     if(lastselected) 
     {
      if(inst_ptr[i].sel==SELECTED) print_verilog_element(fd, i) ;
     }
      else print_verilog_element(fd, i) ;  /* this is the element line  */
    }
   }
 }
 if(debug_var>=1) fprintf(errfp, "verilog_netlist():       end\n");
 if(!netlist_count) draw_hilight_net(1);

}
