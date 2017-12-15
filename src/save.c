/* File: save.c
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
#include <sys/wait.h>


inline void updatebbox(int count, Box *boundbox, Box *tmp)
{
 if(count==1)  *boundbox = *tmp;
 else
 {
  if(tmp->x1<boundbox->x1) boundbox->x1 = tmp->x1;
  if(tmp->x2>boundbox->x2) boundbox->x2 = tmp->x2;
  if(tmp->y1<boundbox->y1) boundbox->y1 = tmp->y1;
  if(tmp->y2>boundbox->y2) boundbox->y2 = tmp->y2;
 }
}    

void load_ascii_string(char **ptr, FILE *fd)
{
 int c, escape=0;
 int i=0, begin=0;
 static char *str=NULL;
 static int strlength=0;

 for(;;)
 {
  if(i+5>strlength) my_realloc(&str,(strlength+=CADCHUNKALLOC));
  c=fgetc(fd);
  if(begin) {
    if(c=='\\') {
     escape=1;
     c=fgetc(fd);
    } else escape=0;
    str[i]=c;
    if(c=='}' && !escape) {
     str[i]='\0';
     break;
    }
    i++;
  } else if(c=='{') begin=1;
 }
 if(debug_var>=2) fprintf(errfp, "load_ascii_string(): string read=%s\n",str? str:"<NULL>");

 my_strdup(ptr, str);
 // my_realloc(&str,(strlength=CADCHUNKALLOC));  // removed 20150402
 if(debug_var>=2) fprintf(errfp, "load_ascii_string(): loaded %s\n",*ptr? *ptr:"<NULL>");
}

void save_ascii_string(char *ptr, FILE *fd)
{
 int i=0;
 int c;
 fputc('{',fd);
 while( ptr && (c=ptr[i++]) )
 {
  if( c=='\\' || c=='{' || c=='}') fputc('\\',fd);
  fputc(c,fd);
 }
 fputc('}',fd);
 fputc(' ',fd); // add space to separate from next field
}


void save_text(FILE *fd)
{
 int i;
 Text *ptr;
 ptr=textelement;
 for(i=0;i<lasttext;i++)
 {
  fprintf(fd, "T ");
  save_ascii_string(ptr[i].txt_ptr,fd);
  fprintf(fd, "%.16g %.16g %d %d %.16g %.16g ",
   ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip, ptr[i].xscale,
    ptr[i].yscale);
  save_ascii_string(ptr[i].prop_ptr,fd);
  fputc('\n' ,fd);
 }
}

void load_text(FILE *fd)
{
   int i;
   char *strlayer;
    check_text_storage();
    i=lasttext;
     textelement[i].txt_ptr=NULL;
     load_ascii_string(&textelement[i].txt_ptr,fd);
     fscanf(fd, "%lf %lf %d %d %lf %lf ",
      &textelement[i].x0, &textelement[i].y0, &textelement[i].rot,
      &textelement[i].flip, &textelement[i].xscale,
      &textelement[i].yscale);
     textelement[i].prop_ptr=NULL;
     textelement[i].font=NULL;
     textelement[i].sel=0;
     load_ascii_string(&textelement[i].prop_ptr,fd);
     my_strdup(&textelement[i].font, get_tok_value(textelement[i].prop_ptr, "font", 0));//20171206
     strlayer = get_tok_value(textelement[i].prop_ptr, "layer", 0); //20171206
     if(strlayer[0]) textelement[i].layer = atoi(strlayer);
     else textelement[i].layer = -1;
     lasttext++;
}

void save_wire(FILE *fd)
{
 int i;
 Wire *ptr;

 ptr=wire;
 for(i=0;i<lastwire;i++)
 {
  fprintf(fd, "N %.16g %.16g %.16g %.16g ",ptr[i].x1, ptr[i].y1, ptr[i].x2,
     ptr[i].y2);
  save_ascii_string(ptr[i].prop_ptr,fd);
  fputc('\n' ,fd);
 }
}  



void load_wire(FILE *fd)
{

    double x1,y1,x2,y2;
    static char *ptr=NULL;
    fscanf(fd, "%lf %lf %lf %lf",&x1, &y1, &x2, &y2 );
    load_ascii_string( &ptr, fd);
    storeobject(-1, x1,y1,x2,y2,WIRE,0,0,ptr);
    modified=0; // 20140116 storeobject sets modified flag , but we are loading here ...
}

void save_inst(FILE *fd)
{
 int i;
 Instance *ptr;

 ptr=inst_ptr;
 for(i=0;i<lastinst;i++)
 {
  fprintf(fd, "C ");
  
  save_ascii_string(ptr[i].name,fd);
  fprintf(fd, "%.16g %.16g %d %d ",ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip ); 
  save_ascii_string(ptr[i].prop_ptr,fd);
  fputc('\n' ,fd);
 }
}




void load_inst(FILE *fd)
{
    int i;

    Instance *ptr;
    i=lastinst;
    check_inst_storage();
    ptr=inst_ptr;
    ptr[i].name=NULL;
    load_ascii_string(&ptr[i].name,fd);
    fscanf(fd, "%lf %lf %d %d",&ptr[i].x0, &ptr[i].y0,&ptr[i].rot,
     &ptr[i].flip);
    ptr[i].flags=0;
    ptr[i].sel=0;
    ptr[i].ptr=-1; //04112003 was 0
    ptr[i].prop_ptr=NULL;
    ptr[i].instname=NULL; // 20150409
    ptr[i].node=NULL;
    load_ascii_string(&ptr[i].prop_ptr,fd);
    my_strdup2(&ptr[i].instname, get_tok_value(ptr[i].prop_ptr, "name", 0)); // 20150409
    hash_proplist(ptr[i].prop_ptr,0);


    if(debug_var>=2) fprintf(errfp, "load_inst(): n=%d name=%s prop=%s\n",
            i, ptr[i].name? ptr[i].name:"<NULL>", ptr[i].prop_ptr? ptr[i].prop_ptr:"<NULL>");
    lastinst++;
}

void save_box(FILE *fd)
{
    int c, i;
    Box *ptr; 
    for(c=0;c<cadlayers;c++)
    {
     ptr=rect[c];
     for(i=0;i<lastrect[c];i++)
     {
      fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
       ptr[i].y2);
      save_ascii_string(ptr[i].prop_ptr,fd);
      fputc('\n' ,fd);
     }
    }
}

void save_polygon(FILE *fd)
{
    int c, i, j;
    Polygon *ptr;
    for(c=0;c<cadlayers;c++)
    {
     ptr=polygon[c];
     for(i=0;i<lastpolygon[c];i++)
     {
      fprintf(fd, "P %d %d ", c,ptr[i].points);
      for(j=0;j<ptr[i].points;j++) {
        fprintf(fd, "%.16g %.16g ", ptr[i].x[j], ptr[i].y[j]);
      }
      save_ascii_string(ptr[i].prop_ptr,fd);
      fputc('\n' ,fd);
     }
    }
}

void load_polygon(FILE *fd)
{
    int i,c, j, points;
    Polygon *ptr;

    fscanf(fd, "%d %d",&c, &points);
    if(c>=cadlayers) {
      fprintf(errfp,"FATAL: polygon layer > defined cadlayers, increase cadlayers\n"); 
      Tcl_Eval(interp, "exit");
    } // 20150408
    check_polygon_storage(c);
    i=lastpolygon[c];
    ptr=polygon[c];
    ptr[i].x=NULL;
    ptr[i].y=NULL;
    ptr[i].selected_point=NULL;
    ptr[i].prop_ptr=NULL;
    ptr[i].x = my_calloc(points, sizeof(double));
    ptr[i].y = my_calloc(points, sizeof(double));
    ptr[i].selected_point= my_calloc(points, sizeof(unsigned short));
    ptr[i].points=points;
    ptr[i].sel=0;
    for(j=0;j<points;j++) {
      fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]));
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    lastpolygon[c]++;
}


void load_box(FILE *fd)
{
    int i,c;
    Box *ptr;

    fscanf(fd, "%d",&c);
    if(c>=cadlayers) {
      fprintf(errfp,"FATAL: box layer > defined cadlayers, increase cadlayers\n");
      Tcl_Eval(interp, "exit");
    } // 20150408
    check_box_storage(c);
    i=lastrect[c];
    ptr=rect[c];
    fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, 
       &ptr[i].x2, &ptr[i].y2);
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    lastrect[c]++;
}

void save_line(FILE *fd)
{
    int c, i;
    Line *ptr; 
    for(c=0;c<cadlayers;c++)
    {
     ptr=line[c];
     for(i=0;i<lastline[c];i++)
     {
      fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
       ptr[i].y2 );
      save_ascii_string(ptr[i].prop_ptr,fd);
      fputc('\n' ,fd);
     }
    }
}

void load_line(FILE *fd)
{
    int i,c;
    Line *ptr;

    fscanf(fd, "%d",&c);
    if(c>=cadlayers) {fprintf(errfp,"FATAL: line layer > defined cadlayers, increase cadlayers\n"); Tcl_Eval(interp, "exit");} // 20150408
    check_line_storage(c);
    i=lastline[c];
    ptr=line[c];
    fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, 
       &ptr[i].x2, &ptr[i].y2);
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    lastline[c]++;
}

void make_symbol(void)
{
 char name[1024]; // overflow safe 20161122

 if( strcmp(schematic[currentsch],"") )
 {
  my_snprintf(name, S(name), "make_symbol %s", schematic[currentsch] );
  if(debug_var>=1) fprintf(errfp, "make_symbol(): making symbol: name=%s\n", name);
  tkeval(name);
 }
         
}


int save_symbol(char *schname) // 20171020 aded return value
{
  FILE *fd;
  int i;
  int symbol;
  char name[4096];  // overflow safe 20161122
  if(schname!=NULL) 
  {
   if(strcmp(schname,"") ) my_strncpy(schematic[currentsch], schname, S(schematic[currentsch]));
   else return -1;
  }
  if(!strcmp(schematic[currentsch],"")) return -1;
  my_snprintf(name, S(name), "%s/%s.sym",Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
   schematic[currentsch]);
   if(debug_var>=2) fprintf(errfp, "save_symbol(): currentsch=%d name=%s\n",currentsch, 
          schematic[currentsch]);
  if(!(fd=fopen(name,"w")) ) {
     tkeval("alert_ {file opening for write failed!} {}"); // 20171020
     return -1;
  }
  unselect_all();
  fprintf(fd, "G ");
  // 20171025 for symbol only put G {} field and look for format or type props in the 3 global prop strings.
  if(schvhdlprop && (strstr(schvhdlprop,"type=") || strstr(schvhdlprop,"format="))) { 
    save_ascii_string(schvhdlprop,fd);
  }
  else if(schprop && (strstr(schprop,"type=") || strstr(schprop,"format="))) {
    save_ascii_string(schprop,fd);
  }
  else if(schverilogprop && (strstr(schverilogprop,"type=") || strstr(schverilogprop,"format="))) {
    save_ascii_string(schverilogprop,fd);
  } else {
    fprintf(fd, "{}");
  }
  fputc('\n', fd);
  fprintf(fd, "V {}\n");
  fprintf(fd, "S {}\n");
  save_line(fd);
  save_box(fd);
  save_text(fd);
  save_polygon(fd);
  save_wire(fd);
  save_inst(fd);        
  fclose(fd);
  remove_symbols();
  for(i=0;i<lastinst;i++)
  {
   symbol = match_symbol(inst_ptr[i].name);
   inst_ptr[i].ptr = symbol;
  } 
  prepared_hilight_structs=0; // 20171212
  prepared_netlist_structs=0; // 20171212
  // delete_netlist_structs(); // 20161222

  modified=0;
  return 0;
}



int save_schematic(char *schname) // 20171020 added return value
{
    FILE *fd;
    char name[4096]; // overflow safe 20161122
    if(schname!=NULL)
    {
      if( strcmp(schname,"") ) my_strncpy(schematic[currentsch], schname, S(schematic[currentsch]));
      else return -1;
    }

    //// useless 20171119 if no correct filename given enforce a correct one or Cancel the save
    // my_snprintf(name, S(name), "check_valid_filename %s", schematic[currentsch]); // 20121111
    // Tcl_Eval(interp, name); 
    // if( strcmp(Tcl_GetStringResult(interp), "ok") ) { 
    //      return -1 ; // 20170622
    // }

    if(!strcmp(schematic[currentsch],"")) {
      if(debug_var>=1) fprintf(errfp, "save_schematic(): no name specified , cant save\n");

      // 20121111
      if(has_x) {
        Tcl_Eval(interp,
          "tk_messageBox -icon error -type ok -message \"save_schematic(): no name specified , cant save\"");
      }

      return -1;
    }
    if(schname !=NULL)
    if(debug_var>=1) fprintf(errfp, "save_schematic(): currentsch=%d name=%s\n",currentsch, schname);
    my_snprintf(name, S(name), "%s/%s.sch",Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
     schematic[currentsch]);
    if(has_x) {
      Tcl_Eval(interp, "wm title . [file tail [xschem get schpath]]");    // 20161207
      Tcl_Eval(interp, "wm iconname . [file tail [xschem get schpath]]"); // 20161207
    }

    if(!(fd=fopen(name,"w")))
    {
      if(debug_var>=1) fprintf(errfp, "save_schematic(): problems opening file %s \n",name);
      tkeval("alert_ {file opening for write failed!} {}"); // 20171020
      return -1;
    }
    unselect_all();
    fprintf(fd, "G ");
    save_ascii_string(schvhdlprop,fd);
    fputc('\n', fd);
    fprintf(fd, "V ");
    save_ascii_string(schverilogprop,fd);  //09112003
    fputc('\n', fd);
    fprintf(fd, "S ");
    save_ascii_string(schprop,fd);  // 20100217
    fputc('\n', fd);
    save_line(fd);
    save_box(fd);
    save_polygon(fd);
    save_text(fd);
    save_wire(fd);
    save_inst(fd);
    fclose(fd);
    prepared_hilight_structs=0; // 20171212
    prepared_netlist_structs=0; // 20171212
    // delete_netlist_structs(); // 20161222
    modified=0;
    return 0;
}

char *get_sym_type(char *name)
{
 int endfile=0;
 FILE *fd;
 char c[256], fullname[4096]; // overflow safe 20161126
 static char *prop=NULL;
 static char *type=NULL;

 my_snprintf(fullname, S(fullname), "%s/%s.sym", Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
    name);
 if( (fd=fopen(fullname,"r"))!= NULL)
 {
        while(!endfile)
        {
         if(fscanf(fd,"%255s",c)==EOF) break;
         switch(c[0])
         {
          case 'G':
           load_ascii_string(&prop,fd);
           break;

          default:
           endfile=1;
           break;
         }
        }
        fclose(fd);
        my_strdup(&type, get_tok_value(prop,"type",0));
        return type;
 }
 return NULL;
}

void link_symbols_to_instances(void) // 20150326 separated from load_schematic()
{
  int i,symbol, missing;
  static char *type=NULL; // 20150407 added static 
  char *symfilename;
  int cond;

  missing = 0;
  for(i=0;i<lastinst;i++)
  {
     symfilename=inst_ptr[i].name; //05112003

     //type=get_sym_type(symfilename); // 20150403 not used anymore for vhdl_block_netlist()
                                       // ************* huge execution time hog !!!! ********

     if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): inst=%d\n", i);
     if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): matching inst %d name=%s \n",i, inst_ptr[i].name);
     if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): -------\n");
     
     symbol = match_symbol(symfilename);
     if(symbol == -1) 
     {
      if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): missing symbol, skipping...\n");
      hash_proplist(inst_ptr[i].prop_ptr , 1); // 06052001 remove props from hash table 
      my_strdup(&inst_ptr[i].prop_ptr, NULL);  // 06052001 remove properties
      delete_inst_node(i);
      my_strdup(&inst_ptr[i].name, NULL);      // 06052001 remove symname
      my_strdup(&inst_ptr[i].instname, NULL);  // 20150409
      missing++;
      continue;
     }
      if(debug_var>=2) fprintf(errfp, "link_symbols_to_instances(): \n");
     inst_ptr[i].ptr = symbol;
      if(debug_var>=2) fprintf(errfp, "link_symbols_to_instances(): missing=%d\n",missing);
     if(missing) 
     {
      inst_ptr[i-missing] = inst_ptr[i];
      inst_ptr[i].prop_ptr=NULL;
      inst_ptr[i].instname=NULL; // 20150409
      inst_ptr[i].node=NULL;
      inst_ptr[i].ptr=-1;  //04112003 was 0
      inst_ptr[i].flags=0;
      inst_ptr[i].name=NULL;
     }
  } 
  lastinst -= missing;
  for(i=0;i<lastinst;i++) {
    if(inst_ptr[i].ptr <0) continue;
    symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1,
                      &inst_ptr[i].x2, &inst_ptr[i].y2);
    // type=get_tok_value(instdef[inst_ptr[i].ptr].prop_ptr,"type",0); // 20150409
    type=instdef[inst_ptr[i].ptr].type; // 20150409
    cond= strcmp(type,"label") && strcmp(type,"ipin") &&
         strcmp(type,"opin") &&  strcmp(type,"iopin");
    if(cond) inst_ptr[i].flags|=2;
    else inst_ptr[i].flags &=~2;
  }

}

void load_schematic(int load_symbols, char *abs_name, int reset_undo) // 20150327 added reset_undo
{
  char c[256]; // overflow safe 20161122
  int endfile=0;
  FILE *fd;
  char name[4096];   // overflow safe 20161122

  current_type=SCHEMATIC;
  if(reset_undo) clear_undo();
  if(strcmp(schematic[currentsch], "")==0)
  {
   if(debug_var>=1) fprintf(errfp, "load_schematic(): Error: schematic[currentsch] not set\n");
   if(debug_var>=1) fprintf(errfp, "load_schematic():        nothing loaded\n");
   unselect_all();
   clear_drawing();
   return;
  }
  if(!abs_name) {
    my_snprintf(name, S(name), "%s/%s.sch", Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY), schematic[currentsch]);
  } else {
    my_snprintf(name, S(name), "%s", abs_name);
  }
  if(debug_var>=2) fprintf(errfp, "load_schematic(): opening file for loading:%s\n",name);

  clear_drawing();
  //free_hash();  // delete whole hash table
  unselect_all();
  if( (fd=fopen(name,"r"))!= NULL)
  {
    while(!endfile)
    {
      if(fscanf(fd,"%255s",c)==EOF) break;
      switch(c[0])
      {
       case 'S':
        load_ascii_string(&schprop,fd); //20100217
        break;
       case 'V':
        load_ascii_string(&schverilogprop,fd); //09112003
         break;
       case 'G':
        load_ascii_string(&schvhdlprop,fd);
        if(debug_var>=2) fprintf(errfp, "load_schematic(): schematic property:%s\n",schvhdlprop?schvhdlprop:"<NULL>");
        break;
       case 'L':
        load_line(fd);
        break;
       case 'P':
        load_polygon(fd);
        break;
       case 'B':
        load_box(fd);
        break;
       case 'T':
        load_text(fd);
        break;
       case 'N':
        load_wire(fd);
        break;
       case 'C':
        load_inst(fd);
        break;
       default:
        if(debug_var>=2) fprintf(errfp, "load_schematic(): end file reached\n");
        endfile=1;
        break;
      }
    }
    fclose(fd); // 20150326 moved before load symbols

    if(debug_var>=2) fprintf(errfp, "load_schematic(): loaded file:wire=%d inst=%d\n",lastwire , lastinst);
    if(load_symbols) link_symbols_to_instances();
    prepared_hilight_structs=0; // 20171212
    prepared_netlist_structs=0; // 20171212
    // delete_netlist_structs(); // 20161222
    modified=0;

  }  // end if(fd=fopen(name,"r"))!= NULL)
  if(has_x) { // 20161207 moved after if( (fd=..)) 
    Tcl_Eval(interp, "wm title . [file tail [xschem get schpath]]"); // 20150417 set window and icon title
    Tcl_Eval(interp, "wm iconname . [file tail [xschem get schpath]]");
  }
  if(debug_var>=2) fprintf(errfp, "load_schematic(): returning\n");
}

void delete_undo(void)  // 20150327
{
  int i;
  char diff_name[4096]; // overflow safe 20161122

  for(i=0; i<max_undo; i++) {
    my_snprintf(diff_name, S(diff_name), "%s/undo%d",undo_dirname, i);
    unlink(diff_name);
  }
  rmdir(undo_dirname);
  my_free(undo_dirname);
}
    
void clear_undo(void) // 20150327
{
  cur_undo_ptr = 0;
  tail_undo_ptr = 0;
  head_undo_ptr = 0;
}

void push_undo(void) // 20150327
{
    FILE *fd, *diff_fd;
    int pd[2];
    char diff_name[4096]; // overflow safe 20161122
    pid_t pid;

    if(no_undo)return;
    if(debug_var>=1) fprintf(errfp, "push_undo(): cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n", 
       cur_undo_ptr, tail_undo_ptr, head_undo_ptr);
    my_snprintf(diff_name, S(diff_name), "%s/undo%d", undo_dirname, cur_undo_ptr%max_undo);

    pipe(pd); 

    if((pid = fork()) ==0) {                        		// child process
      close(pd[1]);                        		// close write side of pipe
      if(!(diff_fd=freopen(diff_name,"w", stdout)))	// redirect stdout to file diff_name
      {
        if(debug_var>=1) fprintf(errfp, "push_undo(): problems opening file %s \n",diff_name);
        Tcl_Eval(interp, "exit");
      }
      dup2(pd[0],0);                       		// connect read side of pipe to stdin
      execlp("gzip", "gzip", "-c", NULL);	// replace current process with comand
      // never gets here
      if(debug_var>=1) fprintf(errfp, "push_undo(): problems with execlp\n");
      Tcl_Eval(interp, "exit");
    }
    close(pd[0]);                        		// close read side of pipe
   
    fd=fdopen(pd[1],"w");

    // unselect_all();
    fprintf(fd, "G ");
    save_ascii_string(schvhdlprop,fd);
    fputc('\n', fd);
    fprintf(fd, "V ");
    save_ascii_string(schverilogprop,fd);  //09112003
    fputc('\n', fd);
    fprintf(fd, "S ");
    save_ascii_string(schprop,fd);  //20100217
    fputc('\n', fd);
    save_line(fd);
    save_polygon(fd);
    save_box(fd);
    save_text(fd);
    save_wire(fd);
    save_inst(fd);
    fclose(fd);
    cur_undo_ptr++;
    head_undo_ptr = cur_undo_ptr;
    tail_undo_ptr = head_undo_ptr <= max_undo? 0: head_undo_ptr-max_undo;
    waitpid(pid, NULL,0);

}

void pop_undo(int redo)  // 20150327
{
  char c[256]; // overflow safe 20161122
  int endfile=0;
  FILE *fd, *diff_fd;
  int pd[2];
  char diff_name[4096];
  pid_t pid;

  if(no_undo)return;
  if(redo) { 
    if(cur_undo_ptr < head_undo_ptr) {
      if(debug_var>=1) fprintf(errfp, "pop_undo(): redo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n", 
         cur_undo_ptr, tail_undo_ptr, head_undo_ptr);
      cur_undo_ptr++;
    } else {
      return;
    }
  } else {  //redo=0 (undo)
    if(cur_undo_ptr == tail_undo_ptr) return;
    if(debug_var>=1) fprintf(errfp, "pop_undo(): undo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n", 
       cur_undo_ptr, tail_undo_ptr, head_undo_ptr);
    if(head_undo_ptr == cur_undo_ptr) {
      push_undo();
      head_undo_ptr--;
      cur_undo_ptr--;
    }
    if(cur_undo_ptr<=0) return; // check undo tail
    cur_undo_ptr--;
  }

  my_snprintf(diff_name, S(diff_name), "%s/undo%d", undo_dirname, cur_undo_ptr%max_undo);

  // current_type=SCHEMATIC; // removed 20151104

  clear_drawing();
  //free_hash();  // delete whole hash table
  unselect_all();

  pipe(pd);

  if((pid = fork())==0) {                                     // child process
    close(pd[0]);                                     // close read side of pipe
    if(!(diff_fd=freopen(diff_name,"r", stdin)))     // redirect stdin from file name
    {
      if(debug_var>=1) fprintf(errfp, "pop_undo(): problems opening file %s \n",diff_name);
      Tcl_Eval(interp, "exit");
    }
    dup2(pd[1],1);                                    // connect write side of pipe to stdout
    execlp("gunzip", "gunzip", "-c", NULL);       // replace current process with command
    // never gets here
    if(debug_var>=1) fprintf(errfp, "pop_undo(): problems with execlp\n");
    Tcl_Eval(interp, "exit");
  }
  close(pd[1]);                                       // close write side of pipe

  fd=fdopen(pd[0],"r");

  while(!endfile)
  {
    if(fscanf(fd,"%255s",c)==EOF) break;
    switch(c[0])
    {
     case 'S':
      load_ascii_string(&schprop,fd); //20100217
      break;
     case 'V':
      load_ascii_string(&schverilogprop,fd); //09112003
       break;
     case 'G':
      load_ascii_string(&schvhdlprop,fd);
      if(debug_var>=2) fprintf(errfp, "load_schematic(): schematic property:%s\n",schvhdlprop?schvhdlprop:"<NULL>");
      break;
     case 'L':
      load_line(fd);
      break;
     case 'P':
      load_polygon(fd);
      break;
     case 'B':
      load_box(fd);
      break;
     case 'T':
      load_text(fd);
      break;
     case 'N':
      load_wire(fd);
      break;
     case 'C':
      load_inst(fd);
      break;
     default:
      if(debug_var>=2) fprintf(errfp, "load_schematic(): end file reached\n");
      endfile=1;
      break;
    }
  }
  fclose(fd); // 20150326 moved before load symbols

  if(debug_var>=2) fprintf(errfp, "load_schematic(): loaded file:wire=%d inst=%d\n",lastwire , lastinst);
  link_symbols_to_instances();
  modified=1;
  if(debug_var>=2) fprintf(errfp, "load_schematic(): returning\n");
  waitpid(pid, NULL, 0);
}



int load_symbol_definition(char *name)
{
  FILE *fd;
  char name3[4096];  // 20161122 overflow safe
  Box tmp,boundbox;
  int i,c,count=0, k, poly_points; // 20171115 polygon stuff
  static char *aux_ptr=NULL;
  double aux_double;
  int aux_int;
  char aux_str[256]; // overflow safe 20161122
  int lastl[cadlayers], lastr[cadlayers], lastp[cadlayers], lastt; // 20171115 lastp
  Line *ll[cadlayers];
  Box *bb[cadlayers];
  Polygon *pp[cadlayers]; // 20171115
  Text *tt;
  int endfile=0;
  char *strlayer;

  if(debug_var>=1) fprintf(errfp, "load_symbol_definition(): name=%s\n", name);
  my_snprintf(name3, S(name3), "%s/%s.sym", Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
    name);
   if(debug_var>=1) fprintf(errfp, "load_symbol_definition(): trying: %s\n",name3);
  if((fd=fopen(name3,"r"))==NULL)
  {
     if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): Symbol not found: %s\n",name3);
    //return -1;
    my_snprintf(name3, S(name3), "%s/%s.sym", Tcl_GetVar(interp,"XSCHEM_HOME_DIR", TCL_GLOBAL_ONLY),
    "systemlib/missing");
    if((fd=fopen(name3,"r"))==NULL) 
    { 
     if(debug_var>=1) fprintf(errfp, "load_symbol_definition(): systemlib/missing.sym missing, I give up\n");
     Tcl_Eval(interp, "exit");
    }
  }

  // modified=0; 09122002: loading a symbol does not mean that the schematic that instantiates it
  //             is in a non modified state.
  for(c=0;c<cadlayers;c++) 
  {
   lastl[c]=lastr[c]=lastp[c]=0; // 20171115 lastp
   ll[c]=NULL;
   bb[c]=NULL;
   pp[c]=NULL;
  }
  lastt=0;
  tt=NULL;
  check_symbol_storage();
  instdef[lastinstdef].prop_ptr = NULL;
  instdef[lastinstdef].type = NULL; // 20150409
  instdef[lastinstdef].templ = NULL; // 20150409

   while(!endfile)
   {
    if(fscanf(fd,"%255s",aux_str)==EOF) break;
    switch(aux_str[0])
    {
     case 'V': //09112003
      load_ascii_string(&aux_ptr,fd);
      break;
     case 'S': //20100217
      load_ascii_string(&aux_ptr,fd);
      break;
     case 'G':
      load_ascii_string(&instdef[lastinstdef].prop_ptr,fd);
      my_strdup2(&instdef[lastinstdef].templ, get_tok_value(instdef[lastinstdef].prop_ptr, "template",2)); // 20150409
      my_strdup2(&instdef[lastinstdef].type, get_tok_value(instdef[lastinstdef].prop_ptr, "type",0)); // 20150409
      if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded symbol prop: \"%s\"\n", 
        instdef[lastinstdef].prop_ptr);
      break;
     case 'L':
      fscanf(fd, "%d",&c);
      if(c>=cadlayers) {
        fprintf(errfp,"FATAL: line layer > defined cadlayers, increase cadlayers\n");
        Tcl_Eval(interp, "exit");
      } // 20150408
      i=lastl[c];
      my_realloc(&ll[c],(i+1)*sizeof(Line));
      fscanf(fd, "%lf %lf %lf %lf ",&ll[c][i].x1, &ll[c][i].y1, 
         &ll[c][i].x2, &ll[c][i].y2);
      ll[c][i].prop_ptr=NULL;
      load_ascii_string( &ll[c][i].prop_ptr, fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded line: ptr=%lu\n", (unsigned long)ll[c]);
      lastl[c]++;
      break;
     case 'P': // 20171115
      fscanf(fd, "%d %d",&c, &poly_points);
      // fprintf(errfp, "load_symbol_definition(): polygon, points=%d\n", poly_points);
      if(c>=cadlayers) {
        fprintf(errfp,"FATAL: line layer > defined cadlayers, increase cadlayers\n");
        Tcl_Eval(interp, "exit");
      } // 20150408
      i=lastp[c];
      my_realloc(&pp[c],(i+1)*sizeof(Polygon));
      pp[c][i].x = my_calloc(poly_points, sizeof(double));
      pp[c][i].y = my_calloc(poly_points, sizeof(double));
      pp[c][i].selected_point = my_calloc(poly_points, sizeof(unsigned short));
      pp[c][i].points = poly_points;
      for(k=0;k<poly_points;k++) {
        fscanf(fd, "%lf %lf ",&(pp[c][i].x[k]), &(pp[c][i].y[k]) );
      }
      pp[c][i].prop_ptr=NULL;
      load_ascii_string( &pp[c][i].prop_ptr, fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded polygon: ptr=%lu\n", (unsigned long)pp[c]);
      lastp[c]++;
      break;

     case 'B':
      fscanf(fd, "%d",&c);
      if(c>=cadlayers) {
        fprintf(errfp,"FATAL: box layer > defined cadlayers, increase cadlayers\n");
        Tcl_Eval(interp, "exit");
      } // 20150408
      i=lastr[c];
      my_realloc(&bb[c],(i+1)*sizeof(Box));
      fscanf(fd, "%lf %lf %lf %lf ",&bb[c][i].x1, &bb[c][i].y1, 
         &bb[c][i].x2, &bb[c][i].y2);
      bb[c][i].prop_ptr=NULL;
      load_ascii_string( &bb[c][i].prop_ptr, fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded rect: ptr=%lu\n", (unsigned long)bb[c]);
      lastr[c]++;
      break;
     case 'T':
      i=lastt;
      my_realloc(&tt,(i+1)*sizeof(Text));
      tt[i].txt_ptr=NULL;
      tt[i].font=NULL;
      load_ascii_string(&tt[i].txt_ptr,fd);
      fscanf(fd, "%lf %lf %d %d %lf %lf ",&tt[i].x0, &tt[i].y0, &tt[i].rot,
         &tt[i].flip, &tt[i].xscale, &tt[i].yscale);
      tt[i].prop_ptr=NULL;
      load_ascii_string(&tt[i].prop_ptr,fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded text\n");

      my_strdup(&tt[i].font, get_tok_value(tt[i].prop_ptr, "font", 0));//20171206
      strlayer = get_tok_value(tt[i].prop_ptr, "layer", 0); //20171206
      if(strlayer[0]) tt[i].layer = atoi(strlayer);
      else tt[i].layer = -1;

      lastt++;
      break;
     case 'N':
      fscanf(fd, "%lf %lf %lf %lf %d",&aux_double,&aux_double,
         &aux_double,&aux_double, &aux_int);
      load_ascii_string( &aux_ptr, fd);
      break;
     case 'C':
      load_ascii_string(&aux_ptr,fd);
      fscanf(fd, "%lf %lf %d %d", &aux_double,&aux_double, &aux_int, &aux_int); 
      load_ascii_string(&aux_ptr,fd);
      break;
     default:
      if(debug_var>=1) fprintf(errfp, "load_schematic(): unknown line, assuming EOF\n");
      endfile=1;
      break;
    }
   }
   fclose(fd);



    if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): finished parsing file\n");
   for(c=0;c<cadlayers;c++)
   {
    instdef[lastinstdef].lines[c] = lastl[c];
    instdef[lastinstdef].rects[c] = lastr[c];
    instdef[lastinstdef].polygons[c] = lastp[c];
    instdef[lastinstdef].lineptr[c] = ll[c];
    instdef[lastinstdef].polygonptr[c] = pp[c];
    instdef[lastinstdef].boxptr[c] = bb[c];
   }
   instdef[lastinstdef].texts = lastt;
   instdef[lastinstdef].txtptr = tt;
    

   boundbox.x1 = boundbox.x2 = boundbox.y1 = boundbox.y2 = 0;
   for(c=0;c<cadlayers;c++)
   {
    for(i=0;i<lastl[c];i++)
    {
     count++;
     tmp.x1=ll[c][i].x1;tmp.y1=ll[c][i].y1;tmp.x2=ll[c][i].x2;tmp.y2=ll[c][i].y2;
     updatebbox(count,&boundbox,&tmp);
    }
    for(i=0;i<lastr[c];i++)
    {
     count++;
     tmp.x1=bb[c][i].x1;tmp.y1=bb[c][i].y1;tmp.x2=bb[c][i].x2;tmp.y2=bb[c][i].y2;
     updatebbox(count,&boundbox,&tmp);
    }
    for(i=0;i<lastp[c];i++) // 20171115
    {
      double x1=0., y1=0., x2=0., y2=0.;
      int k;
      count++;
      for(k=0; k<pp[c][i].points; k++) {
        //fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]);
        if(k==0 || pp[c][i].x[k] < x1) x1 = pp[c][i].x[k];
        if(k==0 || pp[c][i].y[k] < y1) y1 = pp[c][i].y[k];
        if(k==0 || pp[c][i].x[k] > x2) x2 = pp[c][i].x[k];
        if(k==0 || pp[c][i].y[k] > y2) y2 = pp[c][i].y[k];
      }
      tmp.x1=x1;tmp.y1=y1;tmp.x2=x2;tmp.y2=y2;
      updatebbox(count,&boundbox,&tmp);
    }
   }
//   do not include symbol text in bounding box, since text length
//   is variable from one instance to another.

//   for(i=0;i<lastt;i++)
//   { 
//    count++;
//    rot=tt[i].rot;flip=tt[i].flip;
//    text_bbox(tt[i].txt_ptr, tt[i].xscale, tt[i].yscale, rot, flip,
//    tt[i].x0, tt[i].y0, &rx1,&ry1,&rx2,&ry2);
//    tmp.x1=rx1;tmp.y1=ry1;tmp.x2=rx2;tmp.y2=ry2;
//    updatebbox(count,&boundbox,&tmp);
//   }
   instdef[lastinstdef].minx = boundbox.x1;
   instdef[lastinstdef].maxx = boundbox.x2;
   instdef[lastinstdef].miny = boundbox.y1;
   instdef[lastinstdef].maxy = boundbox.y2;
   instdef[lastinstdef].name=NULL;
   my_strdup(&instdef[lastinstdef].name,name); 
   lastinstdef++;

   if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): returning\n");
  return 1;
}

// 20171004
void create_sch_from_sym(void)
{
  Instdef *ptr;
  int i, j, npin, ypos;
  double x;
  int p=0;
  Box *rect;
  FILE *fd;
  char *pindir[3] = {"in", "out", "inout"};
  char *pinname[3] = {"devices/ipin", "devices/opin", "devices/iopin"};
  static char *dir = NULL;
  static char *prop = NULL;
  char schname[4096];
  static char *savecmd=NULL;
  char *sub_prop;
  static char *sub2_prop=NULL;
  static char *str=NULL;
  struct stat buf;

  rebuild_selected_array();
  if(lastselected > 1)  return;
  if(lastselected==1 && selectedgroup[0].type==ELEMENT)
  {
    my_snprintf(schname, S(schname), "%s/%s.sch", 
                Tcl_GetVar(interp, "XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY), 
                inst_ptr[selectedgroup[0].n].name);
    if( !stat(schname, &buf) ) {
      my_strdup(&savecmd, "ask_save \" create schematic file: ");
      my_strcat(&savecmd, schname);
      my_strcat(&savecmd, " ?\nWARNING: This schematic file already exists, it will be overwritten\"");
      tkeval(savecmd);
      if(strcmp(Tcl_GetStringResult(interp), "yes") ) return;
    }
    if(!(fd=fopen(schname,"w")))
    {
      if(debug_var>=1) fprintf(errfp, "create_sch_from_sym(): problems opening file %s \n",schname);
      tkeval("alert_ {file opening for write failed!} {}"); // 20171020
      return;
    }
    fprintf(fd, "G {}");
    fputc('\n', fd);
    fprintf(fd, "V {}");
    fputc('\n', fd);
    fprintf(fd, "S {}");
    fputc('\n', fd);
    ptr = inst_ptr[selectedgroup[0].n].ptr+instdef;
    npin = ptr->rects[GENERICLAYER];
    rect = ptr->boxptr[GENERICLAYER];
    ypos=0;
    for(i=0;i<npin;i++) {
      my_strdup(&prop, rect[i].prop_ptr);
      if(!prop) continue;
      sub_prop=strstr(prop,"name=")+5;
      if(!sub_prop) continue;
      x=-120.0;
      my_realloc(&str, 100+strlen(sub_prop));
      sprintf(str, "name=g%d lab=%s", p++, sub_prop);
      fprintf(fd, "C {devices/generic_pin} %.16g %.16g %.16g %.16g ", x, 20.0*(ypos++), 0.0, 0.0 );
      save_ascii_string(str, fd);
      fputc('\n' ,fd);
    } // for(i)
    npin = ptr->rects[PINLAYER];
    rect = ptr->boxptr[PINLAYER];
    for(j=0;j<3;j++) {
      if(j==1) ypos=0;
      for(i=0;i<npin;i++) {
        my_strdup(&prop, rect[i].prop_ptr);
        if(!prop) continue;
        sub_prop=strstr(prop,"name=")+5;
        if(!sub_prop) continue;
        // remove dir=... from prop string 20171004
        if( strstr(sub_prop, " dir=")) {
          my_strndup(&sub2_prop, sub_prop, strstr(sub_prop, " dir=")-sub_prop);
          my_strcat(&sub2_prop, strstr(strstr(sub_prop, "dir=")," "));
        }

        my_strdup(&dir, get_tok_value(rect[i].prop_ptr,"dir",0));
        if(!dir) continue;
        if(j==0) x=-120.0; else x=120.0;
        if(!strcmp(dir, pindir[j])) { 
          my_realloc(&str, 100+strlen(sub2_prop));
          sprintf(str, "name=g%d lab=%s", p++, sub2_prop);
          fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", pinname[j], x, 20.0*(ypos++), 0.0, 0.0);
          save_ascii_string(str, fd);
          fputc('\n' ,fd);
        } // if()
      } // for(i)
    }  // for(j)
    fclose(fd);
  } // if(lastselected...)
}

void edit_symbol(void)
{
  static char *str=NULL;
  FILE *fd;
  int endfile=0;
  char name[4096];   // overflow safe 20161122
  char name2[4096];   // overflow safe 20161122
  // char s[1024]; // 20121121 overflow safe 20161122 // commented out 20161210
  rebuild_selected_array();
  if(lastselected > 1)  return; //20121122
  if(lastselected==1 && selectedgroup[0].type==ELEMENT) {
   if(modified) { // 20161209
     if(save(1)) return;
   }
   my_strdup( &str,
       get_tok_value(inst_ptr[selectedgroup[0].n].prop_ptr,"name",0)  // moved before ask_save 20121129
								      // because save_schematic clears selection
   );
   my_snprintf(name, S(name), "%s", inst_ptr[selectedgroup[0].n].name);
   // dont allow descend in the default missing symbol
   if(!strcmp( 
        // get_tok_value( (inst_ptr[selectedgroup[0].n].ptr+instdef)->prop_ptr, "type",0),"missing" // 20150409
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type,"missing" // 20150409
      )
     ) return;
  }
  else
  {
   return; // 20161210 safer not to allow descend into unplaced symbol; 
           // problems if this is done *from* a symbol view, when popping back
           // it will return in schematic view(.sch) not into the original .sym view.
           // so code below is commented out.
   /* 
   if(modified) { // 20161209
     if(save(1)) return;
   }
   my_strdup( &str, "nopath" );		 // set some fictious path if entering an unplaced symbol 20121201
   tkeval("loadfile .sym");
   my_snprintf(s, S(s), "get_cell {%s}", Tcl_GetStringResult(interp));  // 20121121 remove .sym
   tkeval(s);					 // 20121121

   if(strcmp(Tcl_GetStringResult(interp),"")==0) {
     if(debug_var>=1) fprintf(errfp, " edit_symbol(): cancel button pressed\n");
     return;
   }
   my_snprintf(name, S(name), "%s",  Tcl_GetStringResult(interp));
   */
  }

  // build up current hierarchy path
  my_strdup(&sch_prefix[currentsch+1], sch_prefix[currentsch]);
  my_strcat(&sch_prefix[currentsch+1], str);
  my_strcat(&sch_prefix[currentsch+1], ".");
  previous_instance[currentsch]=selectedgroup[0].n;
  zoom_array[currentsch].x=xorigin;
  zoom_array[currentsch].y=yorigin;
  zoom_array[currentsch].zoom=zoom;
 
  unselect_all();
  // porzione (XX) spostata sopra 01112002
  ++currentsch;

  current_type=SYMBOL; 	// 20070823  change current type only if we really are editing a symbol
			// previously this line was at the beginning of the function, changing
  clear_drawing();	// the current type also if user cancels the operation.
  my_strncpy(schematic[currentsch], name, S(schematic[currentsch]));
  my_snprintf(name2, S(name2), "%s/%s.sym",Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
   name);
  if(debug_var>=1) fprintf(errfp, "edit_symbol(): filename choosen: %s\n", name2); // 20121121
  if( (fd=fopen(name2,"r"))== NULL) // ok if file is not existing we are making a new symbol 20121122
  {                                 // we are done.
   zoom_full(1); //20121122
   return ;      //20121122
  }
  modified=0;
  prepared_hilight_structs=0; // 20171212
  prepared_netlist_structs=0; // 20171212
  // delete_netlist_structs(); // 20161222
  while(!endfile)
  {
   if(fscanf(fd,"%4095s",name)==EOF) break;
   switch(name[0])
   {
    case 'S':
     load_ascii_string(&schprop,fd); //20100217
     break;
    case 'V':
     load_ascii_string(&schverilogprop,fd); //09112003
     break;
    case 'G':
     load_ascii_string(&schvhdlprop,fd);
     if(debug_var>=2) fprintf(errfp, "load_schematic(): schematic property:%s\n",schvhdlprop);
     break;
    case 'L':
     load_line(fd);
     break;
    case 'P':
     load_polygon(fd);
     break;
    case 'B':
     load_box(fd);
     break;
    case 'T':
     load_text(fd);
     break;
    case 'N':
     load_wire(fd);
     break;
    case 'C':
     load_inst(fd);
     break;
    default:
     if(debug_var>=1) fprintf(errfp, "edit_symbol(): unknown line, assuming EOF\n");
     endfile=1;
     break;
   }
  }
  fclose(fd);
  Tcl_Eval(interp, "wm title . [file tail [xschem get schpath]]"); // 20150417 set window and icon title
  Tcl_Eval(interp, "wm iconname . [file tail [xschem get schpath]]");

  zoom_full(1);
}


void load_symbol(char *abs_name)
{
  FILE *fd;
  int endfile=0;
  char name[4096];   // overflow safe 20161122
  current_type=SYMBOL;
  if(strcmp(schematic[currentsch], "")==0)
  {
   if(debug_var>=1) fprintf(errfp, "load_symbol(): Error: schematic[currentsch] not set\n");
   if(debug_var>=1) fprintf(errfp, "load_symbol():        nothing loaded\n");
   unselect_all();
   clear_drawing();
   return;
  }
  if(!abs_name)
    my_snprintf(name, S(name), "%s/%s.sym", Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY), schematic[currentsch]);
  else
    my_snprintf(name, S(name), "%s", abs_name);
  if(debug_var>=2) fprintf(errfp, "load_symbol(): opening file for loading:%s\n",name);
  clear_drawing();
  unselect_all();
  if( (fd=fopen(name,"r"))!= NULL)
  {
    modified=0;
    prepared_hilight_structs=0; // 20171212
    prepared_netlist_structs=0; // 20171212
    // delete_netlist_structs(); // 20161222
    while(!endfile)
    {
     if(fscanf(fd,"%4095s",name)==EOF) break;
     switch(name[0])
     {
      case 'V':
       load_ascii_string(&schverilogprop,fd); //09112003
       break;
      case 'S':
       load_ascii_string(&schprop,fd); //20100217
       break;
      case 'G':
       load_ascii_string(&schvhdlprop,fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol(): schematic property:%s\n",schvhdlprop);
       break;
      case 'P':
       load_polygon(fd);
       break;
      case 'L':
       load_line(fd);
       break;
      case 'B':
       load_box(fd);
       break;
      case 'T':
       load_text(fd);
       break;
      case 'N':
       load_wire(fd);
       break;
      case 'C':
       load_inst(fd);
       break;
      default:
       if(debug_var>=1) fprintf(errfp, "edit_symbol(): unknown line, assuming EOF\n");
       endfile=1;
       break;
     }
    }
    fclose(fd);
    //zoom_full(1);
    Tcl_Eval(interp, "wm title . [file tail [xschem get schpath]]"); // 20150417 set window and icon title
    Tcl_Eval(interp, "wm iconname . [file tail [xschem get schpath]]");
  }
}


// 20111023 align selected object to current grid setting
#define SNAP_TO_GRID(a)  a=rint(( a)/cadsnap)*cadsnap 
void round_schematic_to_grid(double cadsnap)
{
 int i, c, n;

 rebuild_selected_array();
 for(i=0;i<lastselected;i++)
 {
   c = selectedgroup[i].col;n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
     case TEXT:
       SNAP_TO_GRID(textelement[n].x0);
       SNAP_TO_GRID(textelement[n].y0);
     break;

     case RECT:
       SNAP_TO_GRID(rect[c][n].x1);
       SNAP_TO_GRID(rect[c][n].y1);
       SNAP_TO_GRID(rect[c][n].x2);
       SNAP_TO_GRID(rect[c][n].y2);
     break;

     case WIRE:
       SNAP_TO_GRID(wire[n].x1);
       SNAP_TO_GRID(wire[n].y1);
       SNAP_TO_GRID(wire[n].x2);
       SNAP_TO_GRID(wire[n].y2);
     break;

     case LINE:
       SNAP_TO_GRID(line[c][n].x1);
       SNAP_TO_GRID(line[c][n].y1);
       SNAP_TO_GRID(line[c][n].x2);
       SNAP_TO_GRID(line[c][n].y2);
     break;

     case ELEMENT:
       SNAP_TO_GRID(inst_ptr[n].x0);
       SNAP_TO_GRID(inst_ptr[n].y0);
       //20111111
       symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1, &inst_ptr[n].x2, &inst_ptr[n].y2);
     break;

     default:
     break;
   }
 }


}

// what:
//                      1: save selection
//                      2: save clipboard
void save_selection(int what)
{
 FILE *fd;
 int i, c, n, k;
 char name[4096];

 if(what==1)
   my_snprintf(name, S(name), "%s/%s.sch",getenv("HOME"), ".selection"); // 20150502 changed PWD to HOME
 else // what=2
   my_snprintf(name, S(name), "%s/%s.sch",getenv("HOME"), ".clipboard"); // 20150502 changed PWD to HOME

 if(!(fd=fopen(name,"w")))
 {
    if(debug_var>=1) fprintf(errfp, "save_selection(): problems opening file  \n");
    tkeval("alert_ {file opening for write failed!} {}"); // 20171020
    return;
 }
 fprintf(fd, "G { %.16g %.16g }\n", mousex_snap, mousey_snap);
 for(i=0;i<lastselected;i++)
 {
   c = selectedgroup[i].col;n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
     case TEXT:
      fprintf(fd, "T ");
      save_ascii_string(textelement[n].txt_ptr,fd);
      fprintf(fd, "%.16g %.16g %d %d %.16g %.16g ",
       textelement[n].x0, textelement[n].y0, textelement[n].rot, textelement[n].flip, 
       textelement[n].xscale, textelement[n].yscale);
      save_ascii_string(textelement[n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;
    
     case RECT:
      fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,rect[c][n].x1, rect[c][n].y1,rect[c][n].x2,
       rect[c][n].y2);
      save_ascii_string(rect[c][n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     case POLYGON: // 20171117
      fprintf(fd, "P %d %d ", c, polygon[c][n].points);
      for(k=0; k<polygon[c][n].points; k++) {
        fprintf(fd, "%.16g %.16g ", polygon[c][n].x[k], polygon[c][n].y[k]);
      }
      save_ascii_string(polygon[c][n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;
     
     case WIRE:
      fprintf(fd, "N %.16g %.16g %.16g %.16g ",wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2);
      save_ascii_string(wire[n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     case LINE:
      fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,line[c][n].x1, line[c][n].y1,line[c][n].x2,
       line[c][n].y2 );
      save_ascii_string(line[c][n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     case ELEMENT:
      fprintf(fd, "C ");
      save_ascii_string(inst_ptr[n].name,fd);
      fprintf(fd, "%.16g %.16g %d %d ",inst_ptr[n].x0, inst_ptr[n].y0, inst_ptr[n].rot, inst_ptr[n].flip ); 
      save_ascii_string(inst_ptr[n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     default:
     break;
   }
 }
 fclose(fd);

}

