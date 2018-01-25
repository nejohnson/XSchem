/* File: vhdl_netlist.c
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


void global_vhdl_netlist(int global)  // netlister driver
{
 FILE *fd;
 char *str_tmp;
 static char *dir_tmp = NULL;
 static char *sig_type = NULL;
 static char *port_value = NULL;
 int i,j, tmp;
 char netl[1024];   // overflow safe 20161122
 char netl2[1024];  // 20081202 overflow safe 20161122
 char netl3[1024];  // 20081202 overflow safe 20161122
 static char *type=NULL;

 statusmsg("",2);  // clear infowindow
 // top sch properties used for library use declarations and type definitions
 // to be printed before any entity declarations

 if(current_type==SYMBOL) return;
 netlist_count=0;
 if(!strcmp(schematic[currentsch],""))
 {
   char name[1024];
   my_snprintf(name, S(name), "savefile %s.sch .sch",schematic[currentsch]);
   if(debug_var>=1) fprintf(errfp, "global_spice_netlist(): saving: %s\n",name);
   tkeval(name);
   my_strncpy(schematic[currentsch], Tcl_GetStringResult(interp), S(schematic[currentsch]));
   if(!strcmp(schematic[currentsch],"")) return;
   save_schematic(schematic[currentsch]);
 }
 my_snprintf(netl, S(netl), "%s/%s", netlist_dir, skip_dir(schematic[currentsch]) );
 fd=fopen(netl, "w"); 
 my_snprintf(netl3, S(netl3), "%s", skip_dir(schematic[currentsch]));

 if(fd==NULL){ 
   if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): problems opening netlist file\n");
   return;
 }
 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): opening %s for writing\n",netl);

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level packages\n");
  for(i=0;i<lastinst;i++)
  {
   if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
   if(inst_ptr[i].ptr<0) continue;
   my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
   if( type && (strcmp(type,"package"))==0)
   {
    if(inst_ptr[i].prop_ptr) {				//
      fprintf(fd, "//// begin package\n");		// 20080213 mark start of packages
      fprintf(fd, "%s\n", inst_ptr[i].prop_ptr);
      fprintf(fd, "//// end package\n");		// 20080213 mark end of packages
    }							//
   }
  }

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level use statements\n");
  for(i=0;i<lastinst;i++)
  {
   if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
   if(inst_ptr[i].ptr<0) continue;
   my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
   if( type && (strcmp(type,"use"))==0)
   {
    if(inst_ptr[i].prop_ptr) fprintf(fd, "%s\n", inst_ptr[i].prop_ptr);
   }
  }

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level entity\n");

 // 20071015 already done in print_generic()
 // fprintf(fd,">>> entity %s is\n", skip_dir( schematic[currentsch]) );
 // 20071015 end

 // flush data structures (remove unused symbols)
 if(modified) save_schematic(NULL); // save and flush unused symbols
 remove_symbols();  // removed 25122002, readded 04112003.. this removes unused symbols
 load_schematic(1,NULL,0); 


 // 20071009 print top level generics if defined in symbol
 load_symbol_definition( schematic[currentsch] );
 print_generic(fd,"entity", lastinstdef-1);  // added print top level params
 remove_symbol();
 // load_schematic(1,NULL,0); 
 // 20071009 end


 // 20071009 get generics from symbol
/*
 // print top subckt generics
 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level generic pins\n");
 tmp=0;
 for(i=0;i<lastinst;i++)
 {
  if(inst_ptr[i].ptr<0) continue;
  my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
  my_strdup(&sig_type,get_tok_value(inst_ptr[i].prop_ptr,"generic_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
  if( type && (strcmp(type,"generic"))==0)
  {
   my_strdup(&port_value,get_tok_value(inst_ptr[i].prop_ptr,"value",2));
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   if(!tmp)  fprintf(fd,"generic (\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s : %s", str_tmp ? str_tmp : "(NULL)", sig_type );
   if(port_value &&port_value[0])
      fprintf(fd," := %s", port_value);

   tmp=1;
  }
 }
 if(tmp) fprintf(fd, "\n);\n");
*/



 // print top subckt ipin/opins
 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level out pins\n");
 tmp=0;
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
  if(inst_ptr[i].ptr<0) continue;
  my_strdup(&sig_type,get_tok_value(inst_ptr[i].prop_ptr,"sig_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
  my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"opin"))==0)
  {
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   if(!tmp)  fprintf(fd,"port(\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s : out %s", str_tmp ? str_tmp : "(NULL)", sig_type );
   tmp=1;
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level inout pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
  if(inst_ptr[i].ptr<0) continue;
  my_strdup(&sig_type,get_tok_value(inst_ptr[i].prop_ptr,"sig_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
  my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"iopin"))==0)
  {
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   if(!tmp)  fprintf(fd,"port(\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s : inout %s", str_tmp ? str_tmp : "(NULL)", sig_type );
   tmp=1;
  }
 }

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level input pins\n");
 for(i=0;i<lastinst;i++)
 {
  if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
  if(inst_ptr[i].ptr<0) continue;
  my_strdup(&sig_type,get_tok_value(inst_ptr[i].prop_ptr,"sig_type",0));
  if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
  my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
  if( type && (strcmp(type,"ipin"))==0)
  {
   str_tmp = get_tok_value(inst_ptr[i].prop_ptr,"lab",0);
   if(!tmp)  fprintf(fd,"port(\n");
   if(tmp) fprintf(fd," ;\n");
   fprintf(fd, "  %s :  in %s", str_tmp ? str_tmp : "<NULL>", sig_type );
   tmp=1;
  }
 }
 if(tmp) fprintf(fd,"\n);\n");

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level port attributes\n");
  for(i=0;i<lastinst;i++)
  {
   if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
   if(inst_ptr[i].ptr<0) continue;
   my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
   if( type && (strcmp(type,"port_attributes"))==0)
   {
    if(inst_ptr[i].prop_ptr) fprintf(fd, "%s\n", inst_ptr[i].prop_ptr);
   }
  }

 fprintf(fd,"end %s ;\n\n", skip_dir( schematic[currentsch]) );
 fprintf(fd,"architecture arch_%s of %s is\n\n",  
	skip_dir( schematic[currentsch]) , skip_dir( schematic[currentsch]));

 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): printing top level used components\n");
 // print all components
 for(j=0;j<lastinstdef;j++)
 { 
  if( strcmp(get_tok_value(instdef[j].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20070726
  ////if(get_tok_value(instdef[j].prop_ptr,"vhdl_format",2)[0] != '\0') continue; // 20080611
  if(strcmp(instdef[j].type,"subcircuit")!=0) continue; //20080611
  if((
      strcmp(instdef[j].type,"subcircuit")==0 ||
      strcmp(instdef[j].type,"primitive")==0 
     ) && check_lib(instdef[j].name)
    ) 
  {
   // component generics
   print_generic(fd,"component", j);

   // component ports
   tmp=0;
   for(i=0;i<instdef[j].rects[PINLAYER];i++)
   {
     my_strdup(&sig_type,get_tok_value(
               instdef[j].boxptr[PINLAYER][i].prop_ptr,"sig_type",0));
     my_strdup(&port_value, get_tok_value(
               instdef[j].boxptr[PINLAYER][i].prop_ptr,"value",2) );
     if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
     my_strdup(&dir_tmp, get_tok_value(instdef[j].boxptr[PINLAYER][i].prop_ptr,"dir",0) );
     str_tmp = get_tok_value(instdef[j].boxptr[PINLAYER][i].prop_ptr,"name",0);
     if(!tmp) fprintf(fd, "port (\n");
     if(tmp) fprintf(fd, " ;\n");
     fprintf(fd,"  %s : %s %s",str_tmp ? str_tmp : "<NULL>", 
                          dir_tmp ? dir_tmp : "<NULL>", sig_type);
     if(port_value &&port_value[0])
       fprintf(fd," := %s", port_value);
     tmp=1;
   }
   if(tmp) fprintf(fd, "\n);\n");
   fprintf(fd, "end component ;\n\n");
  }
 }
 
 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): netlisting  top level\n");
 vhdl_netlist(fd, 0);
 fprintf(fd,"//// begin user architecture code\n");
 // 20180124
 for(i=0;i<lastinst;i++) {
   if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
   if(inst_ptr[i].ptr<0) continue;
   if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "vhdl_ignore",0 ), "true") ) {
     continue;
   }
   my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
   if(type && !strcmp(type,"netlist_commands")) {
     fprintf(fd, "%s\n", get_tok_value(inst_ptr[i].prop_ptr,"value",2)); // 20180124
   }
 }

 if(schvhdlprop && schvhdlprop[0]) fprintf(fd, "%s\n", schvhdlprop); 
 fprintf(fd, "end arch_%s ;\n\n", skip_dir( schematic[currentsch]) );

 if(split_files) { // 20081204
   fclose(fd);
   my_snprintf(netl2, S(netl2), "netlist %s noshow %s.vhdl", netl3, netl3);
   tkeval(netl2);
   if(debug_var==0) unlink(netl);
 }
 netlist_count++;

 if(global)
 {
   if(modified) save_schematic(NULL); // 20160302 prepare_netlist_structs (called above from verilog_netlist() 
                                 // may change wire node labels, so save.

   remove_symbols(); // 20161205 ensure all unused symbols purged before descending hierarchy
   load_schematic(1,NULL,0);

   currentsch++;
    if(debug_var>=2) fprintf(errfp, "global_vhdl_netlist(): last defined symbol=%d\n",lastinstdef);
   for(i=0;i<lastinstdef;i++)
   {
    if( strcmp(get_tok_value(instdef[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20070726
    if(strcmp(instdef[i].type,"subcircuit")==0 && 
       check_lib(instdef[i].name))
    {
      if( split_files && strcmp(get_tok_value(instdef[i].prop_ptr,"verilog_netlist",0),"true")==0 )
        verilog_block_netlist(fd, i); // 20081209
      else if( split_files && strcmp(get_tok_value(instdef[i].prop_ptr,"spice_netlist",0),"true")==0 )
        spice_block_netlist(fd, i); // 20081209
      else
        vhdl_block_netlist(fd, i);  // 20081205
    }
   }
   my_strncpy(schematic[currentsch] , "", S(schematic[currentsch]));
   currentsch--;
   remove_symbols();
   load_schematic(1,NULL,0);
 }
 if(debug_var>=1) fprintf(errfp, "global_vhdl_netlist(): starting awk on netlist!\n");
 if(!split_files) {
   fclose(fd);
   if(netlist_show) {
    my_snprintf(netl2, S(netl2), "netlist %s show %s.vhdl", netl3, netl3);
    tkeval(netl2); // 20081202
   }
   else {
    my_snprintf(netl2, S(netl2), "netlist %s noshow %s.vhdl", netl3, netl3);
    tkeval(netl2); // 20081202
   }
   if(!debug_var) unlink(netl);
 }
}


void  vhdl_block_netlist(FILE *fd, int i)  //20081204
{
 int j,k,l, tmp, found;
 int vhdl_stop=0;
 static char *dir_tmp = NULL;
 static char *sig_type = NULL;
 static char *port_value = NULL;
 static char *type=NULL;
 char netl[1024];
 char netl2[1024];  // 20081202
 char netl3[1024];  // 20081202
 char *str_tmp;

     if(!strcmp( get_tok_value(instdef[i].prop_ptr,"vhdl_stop",0),"true") ) 
       vhdl_stop=1;
     else
       vhdl_stop=0;

     if(split_files) { // 20081202
       my_snprintf(netl, S(netl), "%s/%s", netlist_dir,  skip_dir(instdef[i].name) );
       if(debug_var>=1)  fprintf(errfp, "vhdl_block_netlist(): split_files: netl=%s\n", netl);
       fd=fopen(netl, "w");
       my_snprintf(netl3, S(netl3), "%s", skip_dir(instdef[i].name) );
     }

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist(): expanding %s\n",  instdef[i].name);
     fprintf(fd, "\n-- expanding   symbol:  %s # of pins=%d\n\n", 
           instdef[i].name,instdef[i].rects[PINLAYER] );
     my_strncpy(schematic[currentsch],instdef[i].name, S(schematic[currentsch]));
     // load symbol schematic in order to print use/package before declaring entity





     //// 20150403  I dont see any difference just using the original unconditioned load_schematic()
     ////           below line introduces **HUGE** performance issues in 
     ////           load_schematic-> link_symbols_to_instances-> type=get_sym_type(symfilename) executed 
     ////           within a for(;i<lastinst;) loop
     ////
     // vhdl_stop? load_schematic(2,NULL,0) :  load_schematic(1,NULL,0);  // mmmmh lets try it out 04112003
     load_schematic(1,NULL,0); // !stop_vhdl does not print use/packages !!!!!! 27052002


     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       packages\n");
     for(l=0;l<lastinst;l++)
     {
      if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
      if(inst_ptr[l].ptr<0) continue;
      if( !strcmp((inst_ptr[l].ptr+instdef)->type, "package") )
       fprintf(fd, "%s\n", inst_ptr[l].prop_ptr);
     }

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       use statements\n");
     for(l=0;l<lastinst;l++)
     {
      if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
      if(inst_ptr[l].ptr<0) continue;
      if( !strcmp((inst_ptr[l].ptr+instdef)->type, "use") )
       fprintf(fd, "%s\n", inst_ptr[l].prop_ptr);
     }

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       entity generics\n");
     // print entity generics
     print_generic(fd, "entity", i);

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       entity ports\n");
     // print entity ports
     tmp=0;
     for(j=0;j<instdef[i].rects[PINLAYER];j++)
     {
       my_strdup(&sig_type,get_tok_value(
                 instdef[i].boxptr[PINLAYER][j].prop_ptr,"sig_type",0));
       my_strdup(&port_value, get_tok_value(
                 instdef[i].boxptr[PINLAYER][j].prop_ptr,"value",2) );
       if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
       my_strdup(&dir_tmp, get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"dir",0) );
       str_tmp = get_tok_value(instdef[i].boxptr[PINLAYER][j].prop_ptr,"name",0);
       if(tmp) fprintf(fd, " ;\n");
       if(!tmp)  fprintf(fd,"port (\n");
       fprintf(fd,"  %s : %s %s",str_tmp ? str_tmp : "<NULL>", 
		                          dir_tmp ? dir_tmp : "<NULL>", sig_type);
       if(port_value &&port_value[0])
         fprintf(fd," := %s", port_value);
       tmp=1;
     }
     if(tmp) fprintf(fd, "\n);\n");

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       port attributes\n");
     for(l=0;l<lastinst;l++)
     {
      if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
      if(inst_ptr[l].ptr<0) continue;
      my_strdup(&type,(inst_ptr[l].ptr+instdef)->type);
      if( type && (strcmp(type,"port_attributes"))==0)
      {
       if(inst_ptr[l].prop_ptr) fprintf(fd, "%s\n", inst_ptr[l].prop_ptr);
      }
     }
     fprintf(fd,"end %s ;\n\n", skip_dir(instdef[i].name) );

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       architecture\n");
     fprintf(fd,"architecture arch_%s of %s is\n\n",  
	skip_dir( schematic[currentsch]), skip_dir( schematic[currentsch]));
     // load current schematic to print used components

     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():       used components\n");
     // print all components
     if(!vhdl_stop)
       for(j=0;j<lastinstdef;j++)
       {
        if(strcmp(instdef[j].type,"subcircuit")==0 && 
           check_lib(instdef[j].name))
        {
         // only print component declaration if used in current subcircuit
         found=0;
         for(l=0;l<lastinst;l++)
         {
          if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
          if(!strcmp(instdef[j].name,inst_ptr[l].name))
          { 
           found=1; break;
          }
         }
         if(!found) continue;
  
         // component generics
         print_generic(fd, "component",j);
  
         // component ports
         tmp=0;
         for(k=0;k<instdef[j].rects[PINLAYER];k++)
         {
          my_strdup(&sig_type,get_tok_value(
                    instdef[j].boxptr[PINLAYER][k].prop_ptr,"sig_type",0));
          my_strdup(&port_value, get_tok_value(
                    instdef[j].boxptr[PINLAYER][k].prop_ptr,"value",2) );
  
          if(!sig_type || sig_type[0]=='\0') my_strdup(&sig_type,"std_logic");
           my_strdup(&dir_tmp, get_tok_value(instdef[j].boxptr[PINLAYER][k].prop_ptr,"dir",0) );
           str_tmp = get_tok_value(instdef[j].boxptr[PINLAYER][k].prop_ptr,"name",0);
           if(!tmp) fprintf(fd, "port (\n");
           if(tmp) fprintf(fd, " ;\n");
           fprintf(fd,"  %s : %s %s",str_tmp ? str_tmp : "<NULL>", 
    		                          dir_tmp ? dir_tmp : "<NULL>", sig_type);
           if(port_value &&port_value[0])
             fprintf(fd," := %s", port_value);
           tmp=1;
         }
         if(tmp) fprintf(fd, "\n);\n");
         fprintf(fd, "end component ;\n\n");
        }
       }
     if(debug_var>=1) fprintf(errfp, "vhdl_block_netlist():  netlisting %s\n", 
        skip_dir( schematic[currentsch]));
     vhdl_netlist(fd, vhdl_stop);
     fprintf(fd,"//// begin user architecture code\n");


     // 20180124
     for(l=0;l<lastinst;l++) {
       if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
       if(inst_ptr[l].ptr<0) continue;
       if(!strcmp(get_tok_value( (inst_ptr[l].ptr+instdef)->prop_ptr, "vhdl_ignore",0 ), "true") ) {
         continue;
       }                                             
       if(netlist_count &&
         !strcmp(get_tok_value(inst_ptr[l].prop_ptr, "only_toplevel", 0), "true")) continue; // 20160418

       my_strdup(&type,(inst_ptr[l].ptr+instdef)->type);
       if(type && !strcmp(type,"netlist_commands")) { 
         fprintf(fd, "%s\n", get_tok_value(inst_ptr[l].prop_ptr,"value",2)); // 20180124
       }
     }


     if(schvhdlprop && schvhdlprop[0]) fprintf(fd, "%s\n", schvhdlprop);
     fprintf(fd, "end arch_%s ;\n\n", skip_dir( schematic[currentsch]) );
     if(split_files) { // 20081204
       fclose(fd);
       my_snprintf(netl2, S(netl2), "netlist %s noshow %s.vhdl", netl3, netl3);
       tkeval(netl2);
       if(debug_var==0) unlink(netl);
     }
     netlist_count++;
} 

void vhdl_netlist(FILE *fd , int vhdl_stop)
{
 int i,l;
 static char *type=NULL;

 prepare_netlist_structs(0);
 modified=1; // 20160302 prepare_netlist_structs could change schematic (wire node naming for example)
 traverse_node_hash();  // print all warnings about unconnected floatings etc


 if(debug_var>=1) fprintf(errfp, "vhdl_netlist():       architecture declarations\n");
 fprintf(fd, "//// begin user declarations\n");
 for(l=0;l<lastinst;l++)
 {
  if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
  if(inst_ptr[l].ptr<0) continue;
  if(!strcmp((inst_ptr[l].ptr+instdef)->type, "arch_declarations") )
   fprintf(fd, "%s\n", inst_ptr[l].prop_ptr?  inst_ptr[l].prop_ptr: "");
 }
 fprintf(fd, "//// end user declarations\n");

 if(debug_var>=1) fprintf(errfp, "vhdl_netlist():       print erc checks\n");
 if(!vhdl_stop) print_vhdl_signals(fd);
 if(debug_var>=1) fprintf(errfp, "vhdl_netlist():       done print erc checks\n");

 if(debug_var>=1) fprintf(errfp, "vhdl_netlist():       attributes\n");
 fprintf(fd, "//// begin user attributes\n");
 for(l=0;l<lastinst;l++)
 {
  if( strcmp(get_tok_value(inst_ptr[l].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
  if(inst_ptr[l].ptr<0) continue;
  my_strdup(&type,(inst_ptr[l].ptr+instdef)->type);
  if( type && (strcmp(type,"attributes"))==0)
  {
   if(inst_ptr[l].prop_ptr) fprintf(fd, "\n%s\n", inst_ptr[l].prop_ptr);
  }
 }
 fprintf(fd, "//// end user attributes\n");


 fprintf(fd, "begin\n"); // begin reintroduced 09122003
 if(!vhdl_stop)
 {
   for(i=0;i<lastinst;i++) // ... print all element except ipin opin labels use package
   {			   // dont print elements with vhdl_ignore=true set in symbol
    if( strcmp(get_tok_value(inst_ptr[i].prop_ptr,"vhdl_ignore",0),"true")==0 ) continue; // 20140416
    if(inst_ptr[i].ptr<0) continue;
    if(!strcmp(get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr, "vhdl_ignore",0 ), "true") ) { //20070726
      continue; 										  //20070726
    }												  //20070726
    if(debug_var>=2) fprintf(errfp, "vhdl_netlist():       into the netlisting loop\n");
    my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);
    if( type && 
       ( strcmp(type,"label")&&
         strcmp(type,"ipin")&&
         strcmp(type,"opin")&&
         strcmp(type,"iopin")&&
         strcmp(type,"use")&&
         strcmp(type,"netlist_commands")&& // 20180124
         strcmp(type,"timescale")&&
         strcmp(type,"package")  &&
         strcmp(type,"attributes")  &&
         strcmp(type,"port_attributes")  &&
         strcmp(type,"arch_declarations") 
       ))
    {
     if(lastselected) 
     {
      if(inst_ptr[i].sel==SELECTED) {
        print_vhdl_element(fd, i) ;
      }
     } else {
        print_vhdl_element(fd, i) ;
     }
    }
   }
 }
 if(debug_var>=1) fprintf(errfp, "vhdl_netlist():       end\n");
 if(!netlist_count) draw_hilight_net();
 //delete_netlist_structs(); // 20161222 done in prepare_netlist_structs() when needed

}
