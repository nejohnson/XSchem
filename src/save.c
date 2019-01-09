/* File: save.c
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
#include <sys/wait.h>  /* waitpid */

/* skip one line of text from file, stopping at first '\n' or EOF */
/* return NULL if eof encountered */
char *read_line(FILE *fp)
{
  static char s[100];
  char *a;
  size_t l;
  while( (a=fgets(s, 100, fp)) ) {
    l = strlen(s);
    if(s[l-1]=='\n') {
      break;
    }
  }
  return a;
}

/* */
/* 20180923 */
/* return "/<prefix><random string of random_size characters>" */
/* example: "/xschem_undo_dj5hcG38T2" */
/* */
const char *random_string(const char *prefix)
{
  static const char charset[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static int random_size=10;
  static char str[PATH_MAX];
  int prefix_size;
  static unsigned short once=1;
  int i;
  int idx;
  if(once) {
    srand((unsigned short) time(NULL));
    once=0;
  }
  prefix_size = strlen(prefix);
  str[0]='/';
  memcpy(str+1, prefix, prefix_size);
  for(i=prefix_size+1; i < prefix_size + random_size+1; i++) {
    idx = rand()%(sizeof(charset)-1);
    str[i] = charset[idx];
  }
  str[i] ='\0';
  return str;
}


/* */
/* 20180923 */
/* try to create a tmp directory in $HOME */
/* ${HOME}/<prefix><trailing random chars> */
/* after 5 unsuccessfull attemps give up */
/* and return NULL */
/* */
const char *create_tmpdir(char *prefix)
{
  static char str[PATH_MAX];
  int i;
  struct stat buf;
  for(i=0; i<5;i++) {
    my_snprintf(str, S(str), "%s%s", tclgetvar("XSCHEM_TMP_DIR"), random_string(prefix));
    if(stat(str, &buf) && !mkdir(str, 0700) ) { /* dir must not exist */
      if(debug_var>=1) fprintf(errfp, "created dir: %s\n", str);
      return str;
      break;
    }
    if(debug_var>=1) fprintf(errfp, "failed to create %s\n", str);
  }
  fprintf(errfp, "create_tmpdir(): failed to create %s, aborting\n", str);
  return NULL; /* failed to create random dir 5 times */
}

/* */
/* 20180924 */
/* try to create a tmp file in $HOME */
/* ${HOME}/<prefix><trailing random chars> */
/* after 5 unsuccessfull attemps give up */
/* and return NULL */
/* */
FILE *open_tmpfile(char *prefix, char **filename)
{
  static char str[PATH_MAX];
  int i;
  FILE *fd;
  struct stat buf;
  for(i=0; i<5;i++) {
    my_snprintf(str, S(str), "%s%s", tclgetvar("XSCHEM_TMP_DIR"), random_string(prefix));
    *filename = str;
    if(stat(str, &buf) && (fd = fopen(str, "w")) ) { /* file must not exist */
      if(debug_var>=1) fprintf(errfp, "created file: %s\n", str);
      return fd;
      break;
    }
    if(debug_var>=1) fprintf(errfp, "failed to create %s\n", str);
  }
  fprintf(errfp, "open_tmpfile(): failed to create %s, aborting\n", str);
  return NULL; /* failed to create random filename 5 times */
}

void updatebbox(int count, Box *boundbox, Box *tmp)
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
}

void save_embedded_symbol(Instdef *s, FILE *fd, int brackets)
{
  int c, i, j;
  
  if(brackets) fprintf(fd, "[\n");
  fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
  fprintf(fd, "G ");
  save_ascii_string(s->prop_ptr,fd);
  fputc('\n' ,fd);
  fprintf(fd, "V {}\n");
  fprintf(fd, "S {}\n");
  fprintf(fd, "E {}\n"); /* 20180912 */
  for(c=0;c<cadlayers;c++)
  {
   Line *ptr;
   ptr=s->lineptr[c];
   for(i=0;i<s->lines[c];i++)
   {
    fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
     ptr[i].y2 );
    save_ascii_string(ptr[i].prop_ptr,fd);
    fputc('\n' ,fd);
   }
  }
  for(c=0;c<cadlayers;c++)
  {
   Box *ptr;
   ptr=s->boxptr[c];
   for(i=0;i<s->rects[c];i++)
   {
    fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
     ptr[i].y2);
    save_ascii_string(ptr[i].prop_ptr,fd);
    fputc('\n' ,fd);
   }
  }
  for(c=0;c<cadlayers;c++)
  {
   Arc *ptr;
   ptr=s->arcptr[c];
   for(i=0;i<s->arcs[c];i++)
   {
    fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c,ptr[i].x, ptr[i].y,ptr[i].r,
     ptr[i].a, ptr[i].b);
    save_ascii_string(ptr[i].prop_ptr,fd);
    fputc('\n' ,fd);
   }
  }
  for(i=0;i<s->texts;i++)
  {
   Text *ptr;
   ptr = s->txtptr;
   fprintf(fd, "T ");
   save_ascii_string(ptr[i].txt_ptr,fd);
   fprintf(fd, " %.16g %.16g %d %d %.16g %.16g ",
    ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip, ptr[i].xscale,
     ptr[i].yscale);
   save_ascii_string(ptr[i].prop_ptr,fd);
   fputc('\n' ,fd);
  }
  for(c=0;c<cadlayers;c++)
  {
   Polygon *ptr;
   ptr=s->polygonptr[c];
   for(i=0;i<s->polygons[c];i++)
   {
    fprintf(fd, "P %d %d ", c,ptr[i].points);
    for(j=0;j<ptr[i].points;j++) {
      fprintf(fd, "%.16g %.16g ", ptr[i].x[j], ptr[i].y[j]);
    }
    save_ascii_string(ptr[i].prop_ptr,fd);
    fputc('\n' ,fd);
   }
  }
  if(brackets) fprintf(fd, "]\n");
}

void save_inst(FILE *fd)
{
 int i;
 Instance *ptr;

 ptr=inst_ptr;
 for(i=0;i<lastinstdef;i++) instdef[i].flags=0;
 for(i=0;i<lastinst;i++)
 {
  fprintf(fd, "C ");
  
  save_ascii_string(ptr[i].name, fd);
  fprintf(fd, " %.16g %.16g %d %d ",ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip ); 
  save_ascii_string(ptr[i].prop_ptr,fd);
  fputc('\n' ,fd);
  if( !strcmp(get_tok_value(ptr[i].prop_ptr, "embed", 0), "true") 
     && instdef[ptr[i].ptr].flags==0
    ) {
    save_embedded_symbol( instdef+ptr[i].ptr, fd, 1);
    instdef[ptr[i].ptr].flags = EMBEDDED;
  }
 }
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

void save_text(FILE *fd)
{
 int i;
 Text *ptr;
 ptr=textelement;
 for(i=0;i<lasttext;i++)
 {
  fprintf(fd, "T ");
  save_ascii_string(ptr[i].txt_ptr,fd);
  fprintf(fd, " %.16g %.16g %d %d %.16g %.16g ",
   ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip, ptr[i].xscale,
    ptr[i].yscale);
  save_ascii_string(ptr[i].prop_ptr,fd);
  fputc('\n' ,fd);
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

void save_arc(FILE *fd)
{
    int c, i;
    Arc *ptr;
    for(c=0;c<cadlayers;c++)
    {
     ptr=arc[c];
     for(i=0;i<lastarc[c];i++)
     {
      fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c,ptr[i].x, ptr[i].y,ptr[i].r,
       ptr[i].a, ptr[i].b);
      save_ascii_string(ptr[i].prop_ptr,fd);
      fputc('\n' ,fd);
     }
    }
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

void write_xschem_file(FILE *fd, int type)
{
  fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
  if(type==SYMBOL) {
    fprintf(fd, "G ");
    /* 20171025 for symbol only put G {} field and look for format or type props in the 3 global prop strings. */
    if(schvhdlprop && (strstr(schvhdlprop,"type=") || strstr(schvhdlprop,"format="))) {
      save_ascii_string(schvhdlprop,fd);
    }
    else if(schtedaxprop && (strstr(schtedaxprop,"type=") || strstr(schtedaxprop,"format="))) {
      save_ascii_string(schtedaxprop,fd);
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
    fprintf(fd, "E {}\n"); /* 20180912 */
  }
  else {
    fprintf(fd, "G ");
    save_ascii_string(schvhdlprop,fd);
    fputc('\n', fd);
    fprintf(fd, "V ");
    save_ascii_string(schverilogprop,fd);  /*09112003 */
    fputc('\n', fd);
    fprintf(fd, "S ");
    save_ascii_string(schprop,fd);  /* 20100217 */
    fputc('\n', fd);
    fprintf(fd, "E ");
    save_ascii_string(schtedaxprop,fd);  /* 20100217 */
    fputc('\n', fd);
  }
  save_line(fd);
  save_box(fd);
  save_arc(fd);
  save_polygon(fd);
  save_text(fd);
  save_wire(fd);
  save_inst(fd);
}

static void load_text(FILE *fd)
{
  int i;
  char *strlayer;
   if(debug_var>=3) fprintf(errfp, "load_text(): start\n");
   check_text_storage();
   i=lasttext;
   textelement[i].txt_ptr=NULL;
   load_ascii_string(&textelement[i].txt_ptr,fd);
   if(fscanf(fd, "%lf %lf %d %d %lf %lf ",
     &textelement[i].x0, &textelement[i].y0, &textelement[i].rot,
     &textelement[i].flip, &textelement[i].xscale,
     &textelement[i].yscale)<6) {
     fprintf(errfp,"WARNING:  missing fields for TEXT object, ignoring\n");
     read_line(fd);
     return;
   }
   textelement[i].prop_ptr=NULL;
   textelement[i].font=NULL;
   textelement[i].sel=0;
   load_ascii_string(&textelement[i].prop_ptr,fd);
   my_strdup(318, &textelement[i].font, get_tok_value(textelement[i].prop_ptr, "font", 0));/*20171206 */
   strlayer = get_tok_value(textelement[i].prop_ptr, "layer", 0); /*20171206 */
   if(strlayer[0]) textelement[i].layer = atoi(strlayer);
   else textelement[i].layer = -1;
   lasttext++;
}

static void load_wire(FILE *fd)
{

    double x1,y1,x2,y2;
    char *ptr=NULL;
    if(debug_var>=3) fprintf(errfp, "load_wire(): start\n");
    if(fscanf(fd, "%lf %lf %lf %lf",&x1, &y1, &x2, &y2 )<4) {
      fprintf(errfp,"WARNING:  missing fields for WIRE object, ignoring\n");
      read_line(fd);
    } else {
      load_ascii_string( &ptr, fd);
      ORDER(x1, y1, x2, y2);
      storeobject(-1, x1,y1,x2,y2,WIRE,0,0,ptr);
    }
    my_free(&ptr);
}

static void load_inst(int k, FILE *fd)
{
    int i;
    char *prop_ptr=NULL;

    if(debug_var>=3) fprintf(errfp, "load_inst(): start\n");
    i=lastinst;
    check_inst_storage();
    inst_ptr[i].name=NULL;
    load_ascii_string(&inst_ptr[i].name, fd);
    if(fscanf(fd, "%lf %lf %d %d",&inst_ptr[i].x0, &inst_ptr[i].y0,&inst_ptr[i].rot, &inst_ptr[i].flip) < 4) {
      fprintf(errfp,"WARNING: missing fields for INSTANCE object, ignoring.\n"); 
      read_line(fd);
    } else {
      inst_ptr[i].flags=0;
      inst_ptr[i].sel=0;
      inst_ptr[i].ptr=-1; /*04112003 was 0 */
      inst_ptr[i].prop_ptr=NULL;
      inst_ptr[i].instname=NULL; /* 20150409 */
      inst_ptr[i].node=NULL;
      load_ascii_string(&prop_ptr,fd);
      if(renumber_instances) {
        new_prop_string(&inst_ptr[i].prop_ptr, prop_ptr, k);
      } else {
        my_strdup(319, &inst_ptr[i].prop_ptr, prop_ptr);
      }
      my_strdup2(320, &inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name", 0)); /* 20150409 */
      hash_proplist(inst_ptr[i].prop_ptr,0);
      if(debug_var>=2) fprintf(errfp, "load_inst(): n=%d name=%s prop=%s\n",
            i, inst_ptr[i].name? inst_ptr[i].name:"<NULL>", inst_ptr[i].prop_ptr? inst_ptr[i].prop_ptr:"<NULL>");
      lastinst++;
    }
    my_free(&prop_ptr);
}

static void load_polygon(FILE *fd)
{
    int i,c, j, points;
    Polygon *ptr;

    if(debug_var>=3) fprintf(errfp, "load_polygon(): start\n");
    if(fscanf(fd, "%d %d",&c, &points)<2) {
      fprintf(errfp,"WARNING: missing fields for POLYGON object, ignoring.\n"); 
      read_line(fd);
      return;
    }
    if(c<0 || c>=cadlayers) {
      fprintf(errfp,"WARNING: wrong layer number for POLYGON object, ignoring.\n"); 
      read_line(fd);
      return;
    }
    check_polygon_storage(c);
    i=lastpolygon[c];
    ptr=polygon[c];
    ptr[i].x=NULL;
    ptr[i].y=NULL;
    ptr[i].selected_point=NULL;
    ptr[i].prop_ptr=NULL;
    ptr[i].x = my_calloc(321, points, sizeof(double));
    ptr[i].y = my_calloc(322, points, sizeof(double));
    ptr[i].selected_point= my_calloc(323, points, sizeof(unsigned short));
    ptr[i].points=points;
    ptr[i].sel=0;
    for(j=0;j<points;j++) {
      if(fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]))<2) {
        fprintf(errfp,"WARNING: missing fields for POLYGON points, ignoring.\n"); 
        my_free(&ptr[i].x);
        my_free(&ptr[i].y);
        my_free(&ptr[i].selected_point);
        read_line(fd);
        return;
      }
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    /* 20180914 */
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;

    lastpolygon[c]++;
}

static void load_arc(FILE *fd)
{
    int i,c;
    Arc *ptr;

    if(debug_var>=3) fprintf(errfp, "load_arc(): start\n");
    fscanf(fd, "%d",&c);
    if(c<0 || c>=cadlayers) {
      fprintf(errfp,"WARNING: wrong layer number for ARC object, ignoring.\n"); 
      read_line(fd);
      return;
    }
    check_arc_storage(c);
    i=lastarc[c];
    ptr=arc[c];
    if(fscanf(fd, "%lf %lf %lf %lf %lf ",&ptr[i].x, &ptr[i].y,
           &ptr[i].r, &ptr[i].a, &ptr[i].b) < 5) {
      fprintf(errfp,"WARNING:  missing fields for ARC object, ignoring\n");
      read_line(fd);
      return;
    }
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string(&ptr[i].prop_ptr, fd);
    lastarc[c]++;
}

static void load_box(FILE *fd)
{
    int i,c;
    Box *ptr;

    if(debug_var>=3) fprintf(errfp, "load_box(): start\n");
    fscanf(fd, "%d",&c);
    if(c<0 || c>=cadlayers) {
      fprintf(errfp,"WARNING: wrong layer number for RECT object, ignoring.\n"); 
      read_line(fd);
      return;
    }
    check_box_storage(c);
    i=lastrect[c];
    ptr=rect[c];
    if(fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, 
       &ptr[i].x2, &ptr[i].y2) < 4) {
      fprintf(errfp,"WARNING:  missing fields for RECT object, ignoring\n");
      read_line(fd);
      return;
    }
    RECTORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2); /* 20180108 */
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    lastrect[c]++;
}

static void load_line(FILE *fd)
{
    int i,c;
    Line *ptr;

    if(debug_var>=3) fprintf(errfp, "load_line(): start\n");
    fscanf(fd, "%d",&c);
    if(c<0 || c>=cadlayers) {
      fprintf(errfp,"WARNING: Wrong layer number for LINE object, ignoring\n");
      read_line(fd);
      return;
    } 
    check_line_storage(c);
    i=lastline[c];
    ptr=line[c];
    if(fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, 
       &ptr[i].x2, &ptr[i].y2) < 4) {
      fprintf(errfp,"WARNING:  missing fields for LINE object, ignoring\n");
      read_line(fd);
      return;
    }
    ORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2); /* 20180108 */
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    lastline[c]++;
}

void read_xschem_file(FILE *fd) /* 20180912 */
{
  int i, found, endfile;
  char name_embedded[PATH_MAX];
  char c[1];
  int inst_cnt;

  if(debug_var>=3) fprintf(errfp, "read_xschem_file(): start\n");
  inst_cnt = endfile = 0;
  while(!endfile)
  {
    if(fscanf(fd," %c",c)==EOF) break;
    switch(c[0])
    {
     case 'v':
      load_ascii_string(&xschem_version_string, fd);
      break;
     case 'E':
      load_ascii_string(&schtedaxprop,fd); /*20100217 */
      break;
     case 'S':
      load_ascii_string(&schprop,fd); /*20100217 */
      break;
     case 'V':
      load_ascii_string(&schverilogprop,fd); /*09112003 */
       break;
     case 'G':
      load_ascii_string(&schvhdlprop,fd);
      if(debug_var>=2) fprintf(errfp, "read_xschem_file(): schematic property:%s\n",schvhdlprop?schvhdlprop:"<NULL>");
      break;
     case 'L':
      load_line(fd);
      break;
     case 'P':
      load_polygon(fd);
      break;
     case 'A':
      load_arc(fd);
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
      load_inst(inst_cnt++, fd);
      break;
     case '[':
      my_strdup(324, &inst_ptr[lastinst-1].prop_ptr, subst_token(inst_ptr[lastinst-1].prop_ptr, "embed", "true"));


      my_snprintf(name_embedded, S(name_embedded),
         "%s/.xschem_embedded_%d_%s.sym", tclgetvar("XSCHEM_TMP_DIR"), getpid(), get_cell(inst_ptr[lastinst-1].name, 0));
      found=0;
      /* if loading file coming back from embedded symbol delete temporary file */
      for(i=0;i<lastinstdef;i++)
      {
       if(strcmp(name_embedded, instdef[i].name) == 0)
       {
        my_strdup(325, &instdef[i].name, inst_ptr[lastinst-1].name);
        unlink(name_embedded);
        found=1;break;
       }
      }
      if(!found) load_symbol_definition(inst_ptr[lastinst-1].name, fd);
      else {
        while( strcmp( read_line(fd) , "]\n") ) ; /* skip embedded [ ... ] */
      }
      break;
     default:
      /*if(debug_var>=2) fprintf(errfp, "read_xschem_file(): end file reached\n"); */
      /*endfile=1; */
      fprintf(errfp, "read_xschem_file(): skipping: %s", read_line(fd)); /* read rest of line and discard */
      break;
    }
  }
}

void load_ascii_string(char **ptr, FILE *fd)
{
 int c, escape=0;
 int i=0, begin=0;
 char *str=NULL;
 int strlength=0;

 for(;;)
 {
  if(i+5>strlength) my_realloc(326, &str,(strlength+=CADCHUNKALLOC));
  c=fgetc(fd);
  if(c==EOF) {
    fprintf(errfp, "EOF reached, malformed {...} string input, missing close brace\n");
    my_strdup(327, ptr, NULL);
    my_free(&str);
    return;
  }
  if(begin) {
    if(c=='\\') {
     escape=1;
     c=fgetc(fd);
     if(c==EOF) {
       fprintf(errfp, "EOF reached, malformed {...} string input, missing close brace\n");
       my_strdup(328, ptr, NULL);
       my_free(&str);
       return;
     }
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
 fscanf(fd, " ");
 my_strdup(329, ptr, str);
 if(debug_var>=2) fprintf(errfp, "load_ascii_string(): loaded %s\n",*ptr? *ptr:"<NULL>");
 my_free(&str);
}

void make_symbol(void)
{
 char name[1024]; /* overflow safe 20161122 */

 if( strcmp(schematic[currentsch],"") )
 {
  my_snprintf(name, S(name), "make_symbol {%s}", schematic[currentsch] );
  if(debug_var>=1) fprintf(errfp, "make_symbol(): making symbol: name=%s\n", name);
  tcleval(name);
 }
         
}


int save_symbol(char *schname) /* 20171020 added return value */
{
  FILE *fd;
  int i;
  int symbol;
  char name[PATH_MAX];  /* overflow safe 20161122 */

  if(strcmp(schname,"") ) my_strncpy(schematic[currentsch], schname, S(schematic[currentsch]));
  else return -1;
  if(debug_var>=1) fprintf(errfp, "save_symbol(): currentsch=%d name=%s\n",currentsch, schname);

  my_strncpy(name, abs_sym_path(schematic[currentsch], ".sym"), S(name));
  if(has_x) {
    tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\"");    /* 20161207 */
    tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\""); /* 20161207 */
  }
  if(!(fd=fopen(name,"w")) ) {
     fprintf(errfp, "save_symbol(): problems opening file %s \n", name);
     tcleval("alert_ {file opening for write failed!} {}"); /* 20171020 */
     return -1;
  }
  unselect_all();
  write_xschem_file(fd, SYMBOL);
  fclose(fd);
  remove_symbols();
  for(i=0;i<lastinst;i++)
  {
   symbol = match_symbol(inst_ptr[i].name);
   inst_ptr[i].ptr = symbol;
  } 
  prepared_hilight_structs=0; /* 20171212 */
  prepared_netlist_structs=0; /* 20171212 */
  prepared_hash_instances=0; /* 20171224 */
  prepared_hash_wires=0; /* 20171224 */

  /* if an embedded symbol is saved ito temp file don't clear modified flag */
  /* as it needs to be propagated to parent schematic for saving. */
  if(!strstr(schematic[currentsch], ".xschem_embedded_")) {
     set_modify(0);
  }
  return 0;
}

int save_schematic(char *schname) /* 20171020 added return value */
{
    FILE *fd;
    char name[PATH_MAX]; /* overflow safe 20161122 */

    if( strcmp(schname,"") ) my_strncpy(schematic[currentsch], schname, S(schematic[currentsch]));
    else return -1;
    if(debug_var>=1) fprintf(errfp, "save_schematic(): currentsch=%d name=%s\n",currentsch, schname);
    my_strncpy(name, abs_sym_path(schematic[currentsch], ".sch"), S(name));
    if(has_x) {
      tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\"");    /* 20161207 */
      tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\""); /* 20161207 */
    }
    if(!(fd=fopen(name,"w")))
    {
      fprintf(errfp, "save_schematic(): problems opening file %s \n",name);
      tcleval("alert_ {file opening for write failed!} {}"); /* 20171020 */
      return -1;
    }
    unselect_all();
    write_xschem_file(fd, SCHEMATIC);
    fclose(fd);
    prepared_hilight_structs=0; /* 20171212 */
    prepared_netlist_structs=0; /* 20171212 */
    prepared_hash_instances=0; /* 20171224 */
    prepared_hash_wires=0; /* 20171224 */
    set_modify(0);
    return 0;
}

void link_symbols_to_instances(void) /* 20150326 separated from load_schematic() */
{
  int i,symbol, missing;
  static char *type=NULL; /* 20150407 added static  */
  char *symfilename;
  int cond;

  missing = 0;
  for(i=0;i<lastinst;i++)
  {
    symfilename=inst_ptr[i].name; /*05112003 */

    if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): inst=%d\n", i);
    if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): matching inst %d name=%s \n",i, inst_ptr[i].name);
    if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): -------\n");
    
    symbol = match_symbol(symfilename);
    if(symbol == -1) 
    {
      if(debug_var>=1) fprintf(errfp, "link_symbols_to_instances(): missing symbol, skipping...\n");
      hash_proplist(inst_ptr[i].prop_ptr , 1); /* 06052001 remove props from hash table  */
      my_strdup(330, &inst_ptr[i].prop_ptr, NULL);  /* 06052001 remove properties */
      delete_inst_node(i);
      my_strdup(331, &inst_ptr[i].name, NULL);      /* 06052001 remove symname */
      my_strdup(332, &inst_ptr[i].instname, NULL);  /* 20150409 */
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
      inst_ptr[i].instname=NULL; /* 20150409 */
      inst_ptr[i].node=NULL;
      inst_ptr[i].ptr=-1;  /*04112003 was 0 */
      inst_ptr[i].flags=0;
      inst_ptr[i].name=NULL;
    }
  } 
  lastinst -= missing;
  for(i=0;i<lastinst;i++) {
    if(inst_ptr[i].ptr <0) continue;
    symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1,
                      &inst_ptr[i].x2, &inst_ptr[i].y2);
    type=instdef[inst_ptr[i].ptr].type; /* 20150409 */
    cond= !type || (strcmp(type,"label") && strcmp(type,"ipin") &&
         strcmp(type,"opin") &&  strcmp(type,"iopin"));
    if(cond) inst_ptr[i].flags|=2;
    else inst_ptr[i].flags &=~2;
  }

}

void load_schematic(int load_symbols, const char *abs_name, int reset_undo) /* 20150327 added reset_undo */
{
  FILE *fd;
  char name[PATH_MAX];
  static char msg[PATH_MAX+100];
  /* char cmd[PATH_MAX+200]; */

  current_type=SCHEMATIC;
  prepared_hilight_structs=0; /* 20171212 */
  prepared_netlist_structs=0; /* 20171212 */
  prepared_hash_instances=0; /* 20171224 */
  prepared_hash_wires=0; /* 20171224 */
  if(reset_undo) clear_undo();

  if(abs_name && abs_name[0]) {

    my_strncpy(name, abs_sym_path(abs_name, ".sch"), S(name));
    my_strncpy(schematic[currentsch], rel_sym_path(name), S(schematic[currentsch]));
  
    if(debug_var>=1) fprintf(errfp, "load_schematic(): opening file for loading:%s, abs_name=%s\n", name, abs_name);
  
    if(!name[0]) return;
    if( (fd=fopen(name,"r"))== NULL) {
      fprintf(errfp, "load_schematic(): unable to open file: %s, abs_name=%s\n", 
          name, abs_name ? abs_name : "(null)");
      /* my_snprintf(msg, S(msg), "tk_messageBox -type ok -message {Unable to open file: %s}", abs_name ? abs_name: "(null)");*/
      my_snprintf(msg, S(msg), "alert_ {Unable to open file: %s}", abs_name ? abs_name: "(null)");
      tcleval(msg);
      clear_drawing();
    } else {
      clear_drawing();
      my_snprintf(msg, S(msg), "set current_dirname \"[file dirname %s]\"", name);
      tcleval(msg);
      if(debug_var>=1) fprintf(errfp, "load_schematic(): reading file: %s\n", name);
      read_xschem_file(fd);
      fclose(fd); /* 20150326 moved before load symbols */
      set_modify(0);
      if(debug_var>=2) fprintf(errfp, "load_schematic(): loaded file:wire=%d inst=%d\n",lastwire , lastinst);
      if(load_symbols) link_symbols_to_instances();
    }
    if(debug_var>=1) fprintf(errfp, "load_schematic(): %s, returning\n", schematic[currentsch]);
  } else {
    set_modify(0);
    clear_drawing();
    my_strncpy(schematic[currentsch], "untitled.sch", S(schematic[currentsch]));
  }
  if(has_x) { /* 20161207 moved after if( (fd=..))  */
    if(strcmp(get_cell(schematic[currentsch],1), "systemlib/font")) {
      tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\""); /* 20150417 set window and icon title */
      tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\"");
    }
  }
  update_conn_cues(0, 0);
}

#ifndef IN_MEMORY_UNDO

void delete_undo(void)  /* 20150327 */
{
  int i;
  char diff_name[PATH_MAX]; /* overflow safe 20161122 */

  for(i=0; i<max_undo; i++) {
    my_snprintf(diff_name, S(diff_name), "%s/undo%d",undo_dirname, i);
    unlink(diff_name);
  }
  rmdir(undo_dirname);
  my_free(&undo_dirname);
}
    
void clear_undo(void) /* 20150327 */
{
  cur_undo_ptr = 0;
  tail_undo_ptr = 0;
  head_undo_ptr = 0;
}

void push_undo(void) /* 20150327 */
{
    #if HAS_PIPE==1
    int pd[2];
    pid_t pid;
    FILE *diff_fd;
    #endif
    FILE *fd;
    char diff_name[PATH_MAX+100]; /* overflow safe 20161122 */

    if(no_undo)return;
    if(debug_var>=1) fprintf(errfp, "push_undo(): cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n", 
       cur_undo_ptr, tail_undo_ptr, head_undo_ptr);


    #if HAS_POPEN==1
    my_snprintf(diff_name, S(diff_name), "gzip --fast -c > %s/undo%d", undo_dirname, cur_undo_ptr%max_undo);
    fd = popen(diff_name,"w");
    if(!fd) {
      fprintf(errfp, "push_undo(): failed to open write pipe %s\n", diff_name);
      no_undo=1;
      return;
    }
    #elif HAS_PIPE==1
    my_snprintf(diff_name, S(diff_name), "%s/undo%d", undo_dirname, cur_undo_ptr%max_undo);
    pipe(pd); 
    if((pid = fork()) ==0) {                                    /* child process */
      static char f[PATH_MAX] = "";
      close(pd[1]);                                     /* close write side of pipe */
      if(!(diff_fd=freopen(diff_name,"w", stdout)))     /* redirect stdout to file diff_name */
      {
        if(debug_var>=1) fprintf(errfp, "push_undo(): problems opening file %s \n",diff_name);
        Tcl_Eval(interp, "exit");
      }

      /* the following 2 statements are a replacement for dup2() which is not c89
       * however these are not atomic, if another thread takes stdin 
       * in between we are in trouble */
      close(0); /* close stdin */
      dup(pd[0]); /* duplicate read side of pipe to stdin */
      if(!f[0]) my_strncpy(f, get_file_path("gzip"), S(f));
      execl(f, f, "-c", NULL);       /* replace current process with comand */
      /* never gets here */
      fprintf(errfp, "push_undo(): problems with execl\n");
      Tcl_Eval(interp, "exit");
    }
    close(pd[0]);                                       /* close read side of pipe */
    fd=fdopen(pd[1],"w");
    #else /* uncompressed undo */
    my_snprintf(diff_name, S(diff_name), "%s/undo%d", undo_dirname, cur_undo_ptr%max_undo);
    fd = fopen(diff_name,"w");
    if(!fd) {
      fprintf(errfp, "push_undo(): failed to open undo file %s\n", diff_name);
      no_undo=1;
      return;
    }
    #endif
    write_xschem_file(fd, SCHEMATIC);
    cur_undo_ptr++;
    head_undo_ptr = cur_undo_ptr;
    tail_undo_ptr = head_undo_ptr <= max_undo? 0: head_undo_ptr-max_undo;
    #if HAS_POPEN==1
    pclose(fd);
    #elif HAS_PIPE==1
    fclose(fd);
    waitpid(pid, NULL,0);
    #else
    fclose(fd);
    #endif
}

void pop_undo(int redo)  /* 20150327 */
{
  FILE *fd;
  char diff_name[PATH_MAX+12];
  #if HAS_PIPE==1
  int pd[2];
  pid_t pid;
  FILE *diff_fd;
  #endif

  if(no_undo)return;
  if(redo) { 
    if(cur_undo_ptr < head_undo_ptr) {
      if(debug_var>=1) fprintf(errfp, "pop_undo(): redo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n", 
         cur_undo_ptr, tail_undo_ptr, head_undo_ptr);
      cur_undo_ptr++;
    } else {
      return;
    }
  } else {  /*redo=0 (undo) */
    if(cur_undo_ptr == tail_undo_ptr) return;
    if(debug_var>=1) fprintf(errfp, "pop_undo(): undo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n", 
       cur_undo_ptr, tail_undo_ptr, head_undo_ptr);
    if(head_undo_ptr == cur_undo_ptr) {
      push_undo();
      head_undo_ptr--;
      cur_undo_ptr--;
    }
    if(cur_undo_ptr<=0) return; /* check undo tail */
    cur_undo_ptr--;
  }
  clear_drawing();
  unselect_all();

  #if HAS_POPEN==1
  my_snprintf(diff_name, S(diff_name), "gunzip -c %s/undo%d", undo_dirname, cur_undo_ptr%max_undo);
  fd=popen(diff_name, "r");
  if(!fd) {
    fprintf(errfp, "pop_undo(): failed to open read pipe %s\n", diff_name);
    no_undo=1;
    return;
  }
  #elif HAS_PIPE==1
  my_snprintf(diff_name, S(diff_name), "%s/undo%d", undo_dirname, cur_undo_ptr%max_undo);
  pipe(pd);
  if((pid = fork())==0) {                                     /* child process */
    static char f[PATH_MAX] = "";
    close(pd[0]);                                    /* close read side of pipe */
    if(!(diff_fd=freopen(diff_name,"r", stdin)))     /* redirect stdin from file name */
    {
      if(debug_var>=1) fprintf(errfp, "pop_undo(): problems opening file %s \n",diff_name);
      Tcl_Eval(interp, "exit");
    }
    /* connect write side of pipe to stdout */
    close(1);    /* close stdout */
    dup(pd[1]);  /* write side of pipe --> stdout */
    if(!f[0]) my_strncpy(f, get_file_path("gunzip"), S(f));
    execl(f, f, "-c", NULL);       /* replace current process with command */
    /* never gets here */
    if(debug_var>=1) fprintf(errfp, "pop_undo(): problems with execl\n");
    Tcl_Eval(interp, "exit");
  }
  close(pd[1]);                                       /* close write side of pipe */
  fd=fdopen(pd[0],"r");
  #else /* uncompressed undo */
  my_snprintf(diff_name, S(diff_name), "%s/undo%d", undo_dirname, cur_undo_ptr%max_undo);
  fd=fopen(diff_name, "r");
  if(!fd) {
    fprintf(errfp, "pop_undo(): failed to open read pipe %s\n", diff_name);
    no_undo=1;
    return;
  }
  #endif

  read_xschem_file(fd);

  #if HAS_POPEN==1
  pclose(fd); /* 20150326 moved before load symbols */
  #elif HAS_PIPE==1
  fclose(fd);
  waitpid(pid, NULL, 0);
  #else
  fclose(fd);
  #endif
  if(debug_var>=2) fprintf(errfp, "pop_undo(): loaded file:wire=%d inst=%d\n",lastwire , lastinst);
  link_symbols_to_instances();
  set_modify(1);
  prepared_hash_instances=0; /* 20171224 */
  prepared_hash_wires=0; /* 20171224 */
  prepared_netlist_structs=0; /* 20171224 */
  prepared_hilight_structs=0; /* 20171224 */
  update_conn_cues(0, 0);
  if(debug_var>=2) fprintf(errfp, "pop_undo(): returning\n");
}

#endif /* ifndef IN_MEMORY_UNDO */

int load_symbol_definition(char *name, FILE *embed_fd)
{
  FILE *fd;
  char name3[PATH_MAX];  /* 20161122 overflow safe */
  Box tmp,boundbox;
  int i,c,count=0, k, poly_points; /* 20171115 polygon stuff */
  static char *aux_ptr=NULL;
  double aux_double;
  int aux_int;
  char aux_str[1]; /* overflow safe 20161122 */
  int *lastl = my_malloc(333, cadlayers * sizeof(lastl)); 
  int *lastr = my_malloc(334, cadlayers * sizeof(int));
  int *lastp = my_malloc(335, cadlayers * sizeof(int));
  int *lasta = my_malloc(336, cadlayers * sizeof(int));
  Line **ll = my_malloc(337, cadlayers * sizeof(Line *));
  Box **bb = my_malloc(338, cadlayers * sizeof(Box *));
  Arc **aa = my_malloc(339, cadlayers * sizeof(Arc *));
  Polygon **pp = my_malloc(340, cadlayers * sizeof(Polygon *));
  int lastt; /* 20171115 lastp */
  Text *tt;
  int endfile;
  char *strlayer;

  if(debug_var>=1) fprintf(errfp, "load_symbol_definition(): name=%s\n", name);

  if(!embed_fd) {
    my_strncpy(name3, abs_sym_path(name, ".sym"), S(name3));
    if(debug_var>=1) fprintf(errfp, "load_symbol_definition(): trying: %s\n",name3);
    if((fd=fopen(name3,"r"))==NULL)
    {
      if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): Symbol not found: %s\n",name3);
      /*return -1; */
      my_snprintf(name3, S(name3), "%s/%s.sym", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/missing");
      if((fd=fopen(name3,"r"))==NULL) 
      { 
       fprintf(errfp, "load_symbol_definition(): systemlib/missing.sym missing, I give up\n");
       tcleval( "exit");
      }
    }
  } else {
    fd = embed_fd;
  }

  endfile=0;
  for(c=0;c<cadlayers;c++) 
  {
   lasta[c]=lastl[c]=lastr[c]=lastp[c]=0; /* 20171115 lastp */
   ll[c]=NULL;
   bb[c]=NULL;
   pp[c]=NULL;
   aa[c]=NULL;
  }
  lastt=0;
  tt=NULL;
  check_symbol_storage();
  instdef[lastinstdef].prop_ptr = NULL;
  instdef[lastinstdef].type = NULL; /* 20150409 */
  instdef[lastinstdef].templ = NULL; /* 20150409 */

   while(!endfile)
   {
    if(fscanf(fd," %c",aux_str)==EOF) break;
    switch(aux_str[0])
    {
     case 'v':
      load_ascii_string(&aux_ptr, fd);
      break;
     case 'E':
      load_ascii_string(&aux_ptr,fd);
      break;
     case 'V': /*09112003 */
      load_ascii_string(&aux_ptr,fd);
      break;
     case 'S': /*20100217 */
      load_ascii_string(&aux_ptr,fd);
      break;
     case 'G':
      load_ascii_string(&instdef[lastinstdef].prop_ptr,fd);
      my_strdup2(341, &instdef[lastinstdef].templ, get_tok_value(instdef[lastinstdef].prop_ptr, "template",2)); /* 20150409 */
      my_strdup2(342, &instdef[lastinstdef].type, get_tok_value(instdef[lastinstdef].prop_ptr, "type",0)); /* 20150409 */
      if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded symbol prop: \"%s\"\n", 
        instdef[lastinstdef].prop_ptr);
      break;
     case 'L':
      fscanf(fd, "%d",&c);
      if(c < 0 || c>=cadlayers) {
        fprintf(errfp,"WARNING: wrong line layer\n");
        read_line(fd);
        continue;
      } /* 20150408 */
      i=lastl[c];
      my_realloc(343, &ll[c],(i+1)*sizeof(Line));
      if(fscanf(fd, "%lf %lf %lf %lf ",&ll[c][i].x1, &ll[c][i].y1, 
         &ll[c][i].x2, &ll[c][i].y2) < 4 ) {
        fprintf(errfp,"WARNING:  missing fields for LINE object, ignoring\n");
        read_line(fd);
        continue;
      }
      ORDER(ll[c][i].x1, ll[c][i].y1, ll[c][i].x2, ll[c][i].y2); /* 20180108 */

      ll[c][i].prop_ptr=NULL;
      load_ascii_string( &ll[c][i].prop_ptr, fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded line: ptr=%lx\n", (unsigned long)ll[c]);
      lastl[c]++;
      break;
     case 'P': /* 20171115 */
      if(fscanf(fd, "%d %d",&c, &poly_points) < 2 ) {
        fprintf(errfp,"WARNING: missing fields for POLYGON object, ignoring\n");
        read_line(fd);
        continue;
      }
      if(c < 0 || c>=cadlayers) {
        fprintf(errfp,"WARNING: wrong polygon layer\n");
        read_line(fd);
        continue;
      } /* 20150408 */
      i=lastp[c];
      my_realloc(344, &pp[c],(i+1)*sizeof(Polygon));
      pp[c][i].x = my_calloc(345, poly_points, sizeof(double));
      pp[c][i].y = my_calloc(346, poly_points, sizeof(double));
      pp[c][i].selected_point = my_calloc(347, poly_points, sizeof(unsigned short));
      pp[c][i].points = poly_points;
      for(k=0;k<poly_points;k++) {
        if(fscanf(fd, "%lf %lf ",&(pp[c][i].x[k]), &(pp[c][i].y[k]) ) < 2 ) {
          fprintf(errfp,"WARNING: missing fields for POLYGON object\n");
        }
      }
      pp[c][i].prop_ptr=NULL;
      load_ascii_string( &pp[c][i].prop_ptr, fd);
      /* 20180914 */
      if( !strcmp(get_tok_value(pp[c][i].prop_ptr,"fill",0),"true") )
        pp[c][i].fill =1;
      else
        pp[c][i].fill =0;

      if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded polygon: ptr=%lx\n", (unsigned long)pp[c]);
      lastp[c]++;
      break;
     case 'A':
      fscanf(fd, "%d",&c);
      if(c < 0 || c>=cadlayers) {
        fprintf(errfp,"Wrong arc layer\n");
        read_line(fd);
        continue;
      }
      i=lasta[c];
      my_realloc(348, &aa[c],(i+1)*sizeof(Arc));
      if( fscanf(fd, "%lf %lf %lf %lf %lf ",&aa[c][i].x, &aa[c][i].y,
         &aa[c][i].r, &aa[c][i].a, &aa[c][i].b) < 5 ) {
        fprintf(errfp,"WARNING: missing fields for ARC object, ignoring\n");
        read_line(fd);
        continue;
      }

      aa[c][i].prop_ptr=NULL;
      load_ascii_string( &aa[c][i].prop_ptr, fd);
      if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded arc: ptr=%lx\n", (unsigned long)aa[c]);
      lasta[c]++;
      break;
     case 'B':
      fscanf(fd, "%d",&c);
      if(c>=cadlayers) {
        fprintf(errfp,"FATAL: box layer > defined cadlayers, increase cadlayers\n");
        tcleval( "exit");
      } /* 20150408 */
      i=lastr[c];
      my_realloc(349, &bb[c],(i+1)*sizeof(Box));
      fscanf(fd, "%lf %lf %lf %lf ",&bb[c][i].x1, &bb[c][i].y1, 
         &bb[c][i].x2, &bb[c][i].y2);
      RECTORDER(bb[c][i].x1, bb[c][i].y1, bb[c][i].x2, bb[c][i].y2); /* 20180108 */
      bb[c][i].prop_ptr=NULL;
      load_ascii_string( &bb[c][i].prop_ptr, fd);
      if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded rect: ptr=%lx\n", (unsigned long)bb[c]);
      lastr[c]++;
      break;
     case 'T':
      i=lastt;
      my_realloc(350, &tt,(i+1)*sizeof(Text));
      tt[i].txt_ptr=NULL;
      tt[i].font=NULL;
      load_ascii_string(&tt[i].txt_ptr,fd);
      fscanf(fd, "%lf %lf %d %d %lf %lf ",&tt[i].x0, &tt[i].y0, &tt[i].rot,
         &tt[i].flip, &tt[i].xscale, &tt[i].yscale);
      tt[i].prop_ptr=NULL;
      load_ascii_string(&tt[i].prop_ptr,fd);
       if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): loaded text\n");

      my_strdup(351, &tt[i].font, get_tok_value(tt[i].prop_ptr, "font", 0));/*20171206 */
      strlayer = get_tok_value(tt[i].prop_ptr, "layer", 0); /*20171206 */
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
      if(fscanf(fd, "%lf %lf %d %d", &aux_double,&aux_double, &aux_int, &aux_int) < 4) {
        fprintf(errfp,"WARNING: missing fields for COMPONENT object, ignoring\n");
        read_line(fd);
        continue;
      }
      load_ascii_string(&aux_ptr,fd);
      break;
     case ']':
      read_line(fd);
      endfile=1;
      break;
     default:
      /* if(debug_var>=1) fprintf(errfp, "load_symbol_definition(): unknown line, assuming EOF\n"); */
      /* endfile=1; */
      fprintf(errfp, "load_symbol_definition(): skipping: %s", read_line(fd)); /* read rest of line and discard */
      break;
    }
   }
   if(!embed_fd) {
     fclose(fd);
   }
   if(embed_fd || strstr(name, ".xschem_embedded_")) {
     instdef[lastinstdef].flags = EMBEDDED;
   } else {
     instdef[lastinstdef].flags = 0;
   }
   if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): finished parsing file\n");
   for(c=0;c<cadlayers;c++)
   {
    instdef[lastinstdef].arcs[c] = lasta[c];
    instdef[lastinstdef].lines[c] = lastl[c];
    instdef[lastinstdef].rects[c] = lastr[c];
    instdef[lastinstdef].polygons[c] = lastp[c];
    instdef[lastinstdef].arcptr[c] = aa[c];
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
    for(i=0;i<lasta[c];i++)
    {
     count++;
     arc_bbox(aa[c][i].x, aa[c][i].y, aa[c][i].r, aa[c][i].a, aa[c][i].b, 
              &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
     /* printf("arc bbox: %g %g %g %g\n", tmp.x1, tmp.y1, tmp.x2, tmp.y2); */
     updatebbox(count,&boundbox,&tmp);
    }
    for(i=0;i<lastr[c];i++)
    {
     count++;
     tmp.x1=bb[c][i].x1;tmp.y1=bb[c][i].y1;tmp.x2=bb[c][i].x2;tmp.y2=bb[c][i].y2;
     updatebbox(count,&boundbox,&tmp);
    }
    for(i=0;i<lastp[c];i++) /* 20171115 */
    {
      double x1=0., y1=0., x2=0., y2=0.;
      int k;
      count++;
      for(k=0; k<pp[c][i].points; k++) {
        /*fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]); */
        if(k==0 || pp[c][i].x[k] < x1) x1 = pp[c][i].x[k];
        if(k==0 || pp[c][i].y[k] < y1) y1 = pp[c][i].y[k];
        if(k==0 || pp[c][i].x[k] > x2) x2 = pp[c][i].x[k];
        if(k==0 || pp[c][i].y[k] > y2) y2 = pp[c][i].y[k];
      }
      tmp.x1=x1;tmp.y1=y1;tmp.x2=x2;tmp.y2=y2;
      updatebbox(count,&boundbox,&tmp);
    }
   }
/*
 *   do not include symbol text in bounding box, since text length
 *   is variable from one instance to another.
 *
 *   for(i=0;i<lastt;i++)
 *   { 
 *    count++;
 *    rot=tt[i].rot;flip=tt[i].flip;
 *    text_bbox(tt[i].txt_ptr, tt[i].xscale, tt[i].yscale, rot, flip,
 *    tt[i].x0, tt[i].y0, &rx1,&ry1,&rx2,&ry2);
 *    tmp.x1=rx1;tmp.y1=ry1;tmp.x2=rx2;tmp.y2=ry2;
 *    updatebbox(count,&boundbox,&tmp);
 *  }
 */
   instdef[lastinstdef].minx = boundbox.x1;
   instdef[lastinstdef].maxx = boundbox.x2;
   instdef[lastinstdef].miny = boundbox.y1;
   instdef[lastinstdef].maxy = boundbox.y2;
   instdef[lastinstdef].name=NULL;
   my_strdup(352, &instdef[lastinstdef].name,name); 
   lastinstdef++;

   if(debug_var>=2) fprintf(errfp, "load_symbol_definition(): returning\n");
   my_free(&lastl);
   my_free(&lastr);
   my_free(&lastp);
   my_free(&lasta);
   my_free(&ll);
   my_free(&bb);
   my_free(&aa);
   my_free(&pp);
   return 1;
}

/* 20171004 */
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
  char schname[PATH_MAX];
  static char *savecmd=NULL;
  char *sub_prop;
  static char *sub2_prop=NULL;
  static char *str=NULL;
  struct stat buf;
  int ln;

  rebuild_selected_array();
  if(lastselected > 1)  return;
  if(lastselected==1 && selectedgroup[0].type==ELEMENT)
  {
    my_snprintf(schname, S(schname), "%s", 
                abs_sym_path(inst_ptr[selectedgroup[0].n].name, ".sch"));
    if( !stat(schname, &buf) ) {
      my_strdup(353, &savecmd, "ask_save \" create schematic file: ");
      my_strcat(354, &savecmd, schname);
      my_strcat(355, &savecmd, " ?\nWARNING: This schematic file already exists, it will be overwritten\"");
      tcleval(savecmd);
      if(strcmp(Tcl_GetStringResult(interp), "yes") ) return;
    }
    if(!(fd=fopen(schname,"w")))
    {
      fprintf(errfp, "create_sch_from_sym(): problems opening file %s \n",schname);
      tcleval("alert_ {file opening for write failed!} {}"); /* 20171020 */
      return;
    }
    fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
    fprintf(fd, "G {}");
    fputc('\n', fd);
    fprintf(fd, "V {}");
    fputc('\n', fd);
    fprintf(fd, "E {}"); /* 20180912 */
    fputc('\n', fd);
    fprintf(fd, "S {}");
    fputc('\n', fd);
    ptr = inst_ptr[selectedgroup[0].n].ptr+instdef;
    npin = ptr->rects[GENERICLAYER];
    rect = ptr->boxptr[GENERICLAYER];
    ypos=0;
    for(i=0;i<npin;i++) {
      my_strdup(356, &prop, rect[i].prop_ptr);
      if(!prop) continue;
      sub_prop=strstr(prop,"name=")+5;
      if(!sub_prop) continue;
      x=-120.0;
      ln = 100+strlen(sub_prop);
      my_realloc(357, &str, ln);
      my_snprintf(str, ln, "name=g%d lab=%s", p++, sub_prop);
      fprintf(fd, "C {devices/generic_pin} %.16g %.16g %.16g %.16g ", x, 20.0*(ypos++), 0.0, 0.0 );
      save_ascii_string(str, fd);
      fputc('\n' ,fd);
    } /* for(i) */
    npin = ptr->rects[PINLAYER];
    rect = ptr->boxptr[PINLAYER];
    for(j=0;j<3;j++) {
      if(j==1) ypos=0;
      for(i=0;i<npin;i++) {
        my_strdup(358, &prop, rect[i].prop_ptr);
        if(!prop) continue;
        sub_prop=strstr(prop,"name=")+5;
        if(!sub_prop) continue;
        /* remove dir=... from prop string 20171004 */
        if( strstr(sub_prop, " dir=")) {
          my_strndup(359, &sub2_prop, sub_prop, strstr(sub_prop, " dir=")-sub_prop);
          my_strcat(360, &sub2_prop, strstr(strstr(sub_prop, "dir=")," "));
        }

        my_strdup(361, &dir, get_tok_value(rect[i].prop_ptr,"dir",0));
        if(!dir) continue;
        if(j==0) x=-120.0; else x=120.0;
        if(!strcmp(dir, pindir[j])) { 
          ln = 100+strlen(sub2_prop);
          my_realloc(362, &str, ln);
          my_snprintf(str, ln, "name=g%d lab=%s", p++, sub2_prop);
          fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", pinname[j], x, 20.0*(ypos++), 0.0, 0.0);
          save_ascii_string(str, fd);
          fputc('\n' ,fd);
        } /* if() */
      } /* for(i) */
    }  /* for(j) */
    fclose(fd);
  } /* if(lastselected...) */
}

void descend_symbol(void)
{
  char *str=NULL;
  FILE *fd;
  char name[PATH_MAX];   /* overflow safe 20161122 */
  char name_embedded[PATH_MAX];
  rebuild_selected_array();
  if(lastselected > 1)  return; /*20121122 */
  if(lastselected==1 && selectedgroup[0].type==ELEMENT) {
   if(modified) { /* 20161209 */
     if(save(1)) return;
   }

   /* fprintf(errfp, "instname selected: %s\n", inst_ptr[selectedgroup[0].n].instname); */
   my_snprintf(name, S(name), "%s", inst_ptr[selectedgroup[0].n].name);
   /* dont allow descend in the default missing symbol */
   if(!strcmp( 
        (inst_ptr[selectedgroup[0].n].ptr+instdef)->type,"missing" /* 20150409 */
      )
     ) return;
  }
  else return;

  /* build up current hierarchy path */
  my_strdup(363,  &str, inst_ptr[selectedgroup[0].n].instname);  /* 20180911 */
  my_strdup(364, &sch_prefix[currentsch+1], sch_prefix[currentsch]);
  my_strcat(365, &sch_prefix[currentsch+1], str);
  my_strcat(366, &sch_prefix[currentsch+1], ".");
  my_free(&str);
  previous_instance[currentsch]=selectedgroup[0].n;
  zoom_array[currentsch].x=xorigin;
  zoom_array[currentsch].y=yorigin;
  zoom_array[currentsch].zoom=zoom;
  ++currentsch;
  if((inst_ptr[selectedgroup[0].n].ptr+instdef)->flags == EMBEDDED ||
    !strcmp(get_tok_value(inst_ptr[selectedgroup[0].n].prop_ptr,"embed", 0), "true")) {
    my_snprintf(name_embedded, S(name_embedded),
      "%s/.xschem_embedded_%d_%s.sym", tclgetvar("XSCHEM_TMP_DIR"), getpid(), get_cell(name, 0));
    if(!(fd = fopen(name_embedded, "w")) ) {
      fprintf(errfp, "descend_symbol(): problems opening file %s \n", name_embedded);
    }
    save_embedded_symbol(inst_ptr[selectedgroup[0].n].ptr+instdef, fd, 0);
    fclose(fd);
    load_symbol(name_embedded);
  } else {
    load_symbol(name);
  }
  zoom_full(1);
}


void load_symbol(const char *abs_name) /* function called when opening a symbol */
{
  FILE *fd;
  char name[PATH_MAX];   /* overflow safe 20161122 */
  /* char cmd[PATH_MAX+200]; */

  current_type=SYMBOL;
  clear_undo();

  my_snprintf(name, S(name), "%s", abs_sym_path(abs_name, ".sym"));
  my_strncpy(schematic[currentsch], rel_sym_path(abs_name), S(schematic[currentsch]));

  if(debug_var>=1) fprintf(errfp, "load_symbol(): opening file for loading:%s\n",name);

  unselect_all();
  clear_drawing();
  prepared_hash_instances=0; /* 20171224 */
  prepared_hilight_structs=0; /* 20171212 */
  prepared_netlist_structs=0; /* 20171212 */
  prepared_hash_wires=0; /* 20171224 */
  if(!name[0]) return;
  if( (fd=fopen(name,"r"))== NULL) {
    fprintf(errfp, "load_symbol(): can not open file: %s\n", name);
    return;
  }
  if(has_x) {
    tcleval( "wm title . \"xschem - [file tail [xschem get schname]]\""); /* 20150417 set window and icon title */
    tcleval( "wm iconname . \"xschem - [file tail [xschem get schname]]\"");
  }
  read_xschem_file(fd);
  fclose(fd);
  link_symbols_to_instances(); /* 20180921 */
  set_modify(0);
}


/* 20111023 align selected object to current grid setting */
#define SNAP_TO_GRID(a)  (a=ROUND(( a)/cadsnap)*cadsnap )
void round_schematic_to_grid(double cadsnap)
{
 int i, c, n, p;
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

     case ARC:
       SNAP_TO_GRID(arc[c][n].x);
       SNAP_TO_GRID(arc[c][n].y);
     break;
     
     case POLYGON:
       for(p=0;p<polygon[c][n].points; p++) {
         SNAP_TO_GRID(polygon[c][n].x[p]);
         SNAP_TO_GRID(polygon[c][n].y[p]);
       }
     break;

     case ELEMENT:
       SNAP_TO_GRID(inst_ptr[n].x0);
       SNAP_TO_GRID(inst_ptr[n].y0);
       /*20111111 */
       symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1, &inst_ptr[n].x2, &inst_ptr[n].y2);
     break;

     default:
     break;
   }
 }


}

/* what: */
/*                      1: save selection */
/*                      2: save clipboard */
void save_selection(int what)
{
 FILE *fd;
 int i, c, n, k;
 char name[PATH_MAX];

 if(debug_var>=3) fprintf(errfp, "save_selection():\n");
 if(what==1)
   my_snprintf(name, S(name), "%s/%s.sch",home_dir , ".xschem_selection"); /* 20150502 changed PWD to HOME */
 else /* what=2 */
   my_snprintf(name, S(name), "%s/%s.sch",home_dir , ".xschem_clipboard"); /* 20150502 changed PWD to HOME */

 if(!(fd=fopen(name,"w")))
 {
    fprintf(errfp, "save_selection(): problems opening file %s \n", name);
    tcleval("alert_ {file opening for write failed!} {}"); /* 20171020 */
    return;
 }
 fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
 fprintf(fd, "G { %.16g %.16g }\n", mousex_snap, mousey_snap);
 for(i=0;i<lastselected;i++)
 {
   c = selectedgroup[i].col;n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
     case TEXT:
      fprintf(fd, "T ");
      save_ascii_string(textelement[n].txt_ptr,fd);
      fprintf(fd, " %.16g %.16g %d %d %.16g %.16g ",
       textelement[n].x0, textelement[n].y0, textelement[n].rot, textelement[n].flip, 
       textelement[n].xscale, textelement[n].yscale);
      save_ascii_string(textelement[n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;
    
     case ARC:
      fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c, arc[c][n].x, arc[c][n].y, arc[c][n].r,
       arc[c][n].a, arc[c][n].b);
      save_ascii_string(arc[c][n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     case RECT:
      fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,rect[c][n].x1, rect[c][n].y1,rect[c][n].x2,
       rect[c][n].y2);
      save_ascii_string(rect[c][n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     case POLYGON: /* 20171117 */
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
      fprintf(fd, " %.16g %.16g %d %d ",inst_ptr[n].x0, inst_ptr[n].y0, inst_ptr[n].rot, inst_ptr[n].flip ); 
      save_ascii_string(inst_ptr[n].prop_ptr,fd);
      fputc('\n' ,fd);
     break;

     default:
     break;
   }
 }
 fclose(fd);

}

