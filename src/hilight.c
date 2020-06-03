/* File: hilight.c
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

static struct hilight_hashentry *table[HASHSIZE];
static int nelements=0; /* 20161221 */

static int *inst_color=NULL;

static unsigned int hash(char *tok)
{
  unsigned int hash = 0;
  char *str;
  int c;

  str=sch_prefix[currentsch];
  while ( (c = *tok++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  while ( (c = *str++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  return hash;
}

static struct hilight_hashentry *free_hilight_entry(struct hilight_hashentry *entry)
{
  struct hilight_hashentry *tmp;
  while(entry) {
    tmp = entry->next;
    my_free(&entry->token);
    my_free(&entry->path);
    my_free(&entry);
    entry = tmp;
  }
  return NULL;
}

void free_hilight_hash(void) /* remove the whole hash table  */
{
 int i;

 if(debug_var>=2) fprintf(errfp, "free_hilight_hash(): removing hash table\n");
 for(i=0;i<HASHSIZE;i++)
 {
  table[i] = free_hilight_entry( table[i] );
 }
 if(debug_var>=2) fprintf(errfp, "free_hilight_hash(): : nelements=%d\n", nelements);
 nelements=0; /* 20161221 */
}


int get_color(int value) 
{
  int x;

  x = value%(n_active_layers);
  return active_layer[x];
}

/* print all highlight signals which are not ports (in/out/inout). */
void create_ngspice_plot_cmd()
{
  int i, c, idx, first;
  struct hilight_hashentry *entry;
  struct node_hashentry *node_entry;
  char *tok;
  char plotfile[PATH_MAX];
  FILE *fd;


  my_snprintf(plotfile, S(plotfile), "%s/xplot", netlist_dir);
  if(!(fd = fopen(plotfile, "w"))) {
    fprintf(errfp, "print_all_hilights(): error opening xplot file for writing\n");
    return;
  }
  fprintf(fd, "*ngspice plot file\n.control\n");



  idx = 1;
  for(i=0;i<HASHSIZE;i++) /* set ngspice colors */
  {
    entry = table[i];
    while(entry) {
      tok = entry->token;
      node_entry = bus_hash_lookup(tok, "",2, 0, "", "", "", "");
      if(node_entry && !strcmp(sch_prefix[currentsch], entry->path) && node_entry->d.port == 0) {
        c = get_color(entry->value);
        idx++;
        if(idx > 22) idx = 2;
        fprintf(fd, "set color%d=rgb:%02x/%02x/%02x\n", idx, xcolor_array[c].red>>8, xcolor_array[c].green>>8, xcolor_array[c].blue>>8);
      }
      entry = entry->next;
    }
  }

  /* plot signals */
  first = 1;
  for(i=0;i<HASHSIZE;i++)
  {
    entry = table[i];
    while(entry) {

      tok = entry->token;
      node_entry = bus_hash_lookup(tok, "",2, 0, "", "", "", "");
      if(tok[0] == '#') tok++;

      if(node_entry && !strcmp(sch_prefix[currentsch], entry->path) && node_entry->d.port == 0) {
        if(first) {
          fprintf(fd, "plot ");
          first = 0;
        }
        if(!strcmp(entry->path, ".") )
          fprintf(fd, "%s ", tok);
        else
          fprintf(fd, "%s%s ", (entry->path)+1, tok);
      }
      entry = entry->next;
    }
  }
  fprintf(fd, "\n.endc\n");
  fclose(fd);
}

struct hilight_hashentry *hilight_lookup(char *token, int value, int remove)
/*    token           remove    ... what ... */
/* -------------------------------------------------------------------------- */
/* "whatever"         0       insert in hash table if not in and return NULL */
/*                                      if already present just return entry address  */
/*                                      return NULL otherwise */
/* */
/* "whatever"         1       delete entry if found return NULL */
/* "whatever"         2       only look up element, dont insert */
{
 unsigned int hashcode, index;
 struct hilight_hashentry *entry, *saveptr, **preventry;
 char *ptr;
 int s ;
 int depth=0; /* 20161221 */

 if(token==NULL) return NULL;
 hashcode=hash(token);
 index=hashcode % HASHSIZE;
 entry=table[index];
 preventry=&table[index];
 depth=0; /* 20161221 */
 while(1)
 {
  if( !entry )                  /* empty slot */
  {
   if( remove==0 )              /* insert data */
   {
    s=sizeof( struct hilight_hashentry );
    ptr= my_malloc(137, s ); 
    entry=(struct hilight_hashentry *)ptr;
    entry->next = NULL;
    entry->token = NULL;
    my_strdup(138, &(entry->token),token);
    entry->path = NULL;
    my_strdup(139, &(entry->path),sch_prefix[currentsch]);
    entry->value=value;
    entry->hash=hashcode;
    *preventry=entry;
    hilight_nets=1; /* some nets should be hilighted ....  07122002 */
    nelements++; /* 20161221 */
   }
   return NULL; /* whether inserted or not return NULL since it was not in */
  }
  if( entry -> hash==hashcode && !strcmp(token,entry->token) &&
      !strcmp(sch_prefix[currentsch], entry->path)  ) /* found matching tok */
  {
   if(remove==1)                /* remove token from the hash table ... */
   {
    saveptr=entry->next;
    my_free(&entry->token);
    my_free(&entry->path);
    my_free(&entry);
    *preventry=saveptr;
    return NULL;
   }
   else /* found matching entry, return the address */
   {
    return entry;
   }
  }
  preventry=&entry->next; /* descend into the list. */
  entry = entry->next;
  depth++; /* 20161221 */
  if(debug_var>=2) 
    if(depth>200) 
      fprintf(errfp, "hilight_lookup(): deep into the list: %d, index=%d, token=%s, hashcode=%d\n", 
              depth, index, token, hashcode);
 }
}

/* warning, in case of buses return only pointer to first bus element */
struct hilight_hashentry *bus_hilight_lookup(const char *token, int value, int remove)
{
 char *start, *string_ptr, c;
 static char *string=NULL;
 struct hilight_hashentry *ptr1=NULL, *ptr2=NULL;
 int mult;

 if(token==NULL) return NULL;
 if( token[0] == '#') {
   my_strdup(140, &string, token);
 }
 else {
   my_strdup(141, &string, expandlabel(token,&mult));
 }

 if(string==NULL) return NULL;
 string_ptr = start = string;
 while(1) {
  c=(*string_ptr);
  if(c==','|| c=='\0')
  {
    *string_ptr='\0';  /* set end string at comma position.... */
    /* insert one bus element at a time in hash table */
    if(debug_var>=2) fprintf(errfp, "bus_hilight_lookup: inserting: %s, value:%d\n", start,value);
    ptr1=hilight_lookup(start, value, remove);
    if(ptr1 && !ptr2) {
      ptr2=ptr1; /*return first non null entry */
      if(remove==2) break; /* 20161221 no need to go any further if only looking up element */
    }
    *string_ptr=c;     /* ....restore original char */
    start=string_ptr+1;
  }
  if(c==0) break;
  string_ptr++;
 }
 /* if something found return first pointer */
 return ptr2;
}

void delete_hilight_net(void)
{
 int i;
 free_hilight_hash();
 if(event_reporting) {
   printf("xschem clear_hilights\n");
   fflush(stdout);
 }

 hilight_nets=0;
 for(i=0;i<lastinst;i++) {
   inst_ptr[i].flags &= ~4 ;
 }
 my_free(&inst_color);
 hilight_color=0;
}

void hilight_parent_pins(void)
{
 int rects, i,j;
 char *pin_node;
 int save_currentsch;
 struct hilight_hashentry *entry;
 
 if(!hilight_nets) return;
 prepare_netlist_structs(0);
 save_currentsch=currentsch;
 i=previous_instance[currentsch];
 if(debug_var>=1) fprintf(errfp, "hilight_parent_pins(): previous_instance=%d\n", previous_instance[currentsch]);

 rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER];

 currentsch=save_currentsch+1;
 for(j=0;j<rects;j++)
 {
  pin_node=get_tok_value((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][j].prop_ptr,"name",0);
  if( (entry = bus_hilight_lookup(pin_node,0,2)) )
  {
    currentsch=save_currentsch;
    bus_hilight_lookup(inst_ptr[i].node[j], entry->value,0);
    currentsch=save_currentsch+1;
  }
  else
  {
    currentsch=save_currentsch;
    bus_hilight_lookup(inst_ptr[i].node[j], 0,1);
    currentsch=save_currentsch+1;
  }
 }

 currentsch=save_currentsch;

}

void hilight_child_pins(int i)
{
 int j,rects;
 char *pin_node;
 struct hilight_hashentry *entry;
 int save_currentsch;

 if(!hilight_nets) return;
 save_currentsch=currentsch;
 prepare_netlist_structs(0);
 rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
 for(j=0;j<rects;j++)
 {
  currentsch=save_currentsch;
  pin_node=get_tok_value((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][j].prop_ptr,"name",0);
  if( (entry = bus_hilight_lookup(inst_ptr[i].node[j], 0,2)) )
  {
    currentsch++;
    bus_hilight_lookup(pin_node, entry->value,0); 
  }
  else
  {
    currentsch++;
    bus_hilight_lookup(pin_node, 0,1); 
  }
 }
 currentsch=save_currentsch;
}


int bus_search(const char*s) 
{ 
 int c, bus=0;
 while( (c=*s++) ) {
   if(c=='[')  bus=1;
   if( (c==':') || (c==',') ) {bus=0; break;}
 }
 return bus;
}

void search(const char *tok, const char *val, int sub, int sel, int what)
{
 int save_draw, hilight_layer = 7;
 int i,c, col = 7,tmp,bus=0;
 const char *str;
 char *type; 
 int has_token;
 const char empty_string[] = "";
 static char *tmpname=NULL;
#ifdef __unix__
 regex_t re;
#endif

 if(!val) {
   fprintf(errfp, "search(): warning: null val key\n");
   return;
 }
 save_draw = draw_window;
 draw_window=1;
#ifdef __unix__
 if(regcomp(&re, val , REG_EXTENDED)) return;
#endif
 if(debug_var>=1) fprintf(errfp, "search():val=%s\n", val);
 if(sel==1) {
   drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
 }
 if(what==ADD || what==NOW) {
   
    if(!sel) { /* 20190525 */
      col=hilight_color;
      hilight_layer = get_color(col);
      if(incr_hilight) hilight_color++;
    }
    has_token = 0;
    prepare_netlist_structs(0);
    bus=bus_search(val);
    for(i=0;i<lastinst;i++) {
      if(!strcmp(tok,"cell::name")) {
        has_token = (inst_ptr[i].name != NULL) && inst_ptr[i].name[0];
        str = inst_ptr[i].name;
      } else if(!strncmp(tok,"cell::", 6)) {
        my_strdup(142, &tmpname,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,tok+6,0)); /* flexible cell__ search 20140408 */
        has_token = get_tok_size;
        if(tmpname) {
          str = tmpname;
        } else {
          str = empty_string;
        }
      } else if(!strcmp(tok,"propstring")) { /* 20170408 */
        has_token = (inst_ptr[i].prop_ptr != NULL) && inst_ptr[i].prop_ptr[0];
        str = inst_ptr[i].prop_ptr;
      } else {
        str = get_tok_value(inst_ptr[i].prop_ptr, tok,0);
        has_token = get_tok_size;
      }
      if(debug_var>=1) fprintf(errfp, "search(): inst=%d, tok=%s, val=%s \n", i,tok, str);
     
      if(bus && sub) {
       if(debug_var>=1) fprintf(errfp, "search(): doing substr search on bus sig:%s inst=%d tok=%s val=%s\n", str,i,tok,val);
       str=expandlabel(str,&tmp);
      }
      if(str && has_token) {
#ifdef __unix__
        if( (!regexec(&re, str,0 , NULL, 0) && !sub) ||           /* 20071120 regex instead of strcmp */
            (!strcmp(str,val) && sub) ) 
#else
        if (!strcmp(str, val) && sub)
#endif
        {
          if(!sel) { /*20190525 */
            type = (inst_ptr[i].ptr+instdef)->type;
            if( type && 
                !(strcmp(type,"label") && strcmp(type,"ipin") &&
                  strcmp(type,"iopin") && strcmp(type,"opin")) 
              ) {
              if(!bus_hilight_lookup(inst_ptr[i].node[0], col,0)) hilight_nets = 1;
              if(what==NOW) for(c=0;c<cadlayers;c++)
                draw_symbol(NOW, hilight_layer, i,c,0,0,0.0,0.0);
            }
            else {
              if(debug_var>=1) fprintf(errfp, "search(): setting hilight flag on inst %d\n",i);
              hilight_nets=1;
              inst_ptr[i].flags |= 4;
              if(what==NOW) for(c=0;c<cadlayers;c++)
                draw_symbol(NOW, hilight_layer, i,c,0,0,0.0,0.0);  /* 20150804 */
            }
          }
          if(sel==1) {
            select_element(i, SELECTED, 1, 0);
            ui_state|=SELECTION;
          }
          
          if(sel==-1) { /* 20171211 unselect */
            select_element(i, 0, 1, 0);
         }
        }
      }
      
    }
    for(i=0;i<lastwire;i++) {
      str = get_tok_value(wire[i].prop_ptr, tok,0);
      if(get_tok_size ) {
#ifdef __unix__
        if(   (!regexec(&re, str,0 , NULL, 0) && !sub )  ||       /* 20071120 regex instead of strcmp */
              ( !strcmp(str, val) &&  sub ) )
#else
        if (!strcmp(str, val) && sub)
#endif
        {
            if(debug_var>=2) fprintf(errfp, "search(): wire=%d, tok=%s, val=%s \n", i,tok, wire[i].node);
            if(!sel) {
              bus_hilight_lookup(wire[i].node, col, 0);
              if(what == NOW) {
                if(wire[i].bus) /* 20171201 */
                  drawline(hilight_layer, THICK,
                     wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
                else
                  drawline(hilight_layer, NOW,
                     wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
                if(cadhalfdotsize*mooz>=0.7) {
                  if( wire[i].end1 >1 ) {
                    filledarc(hilight_layer, NOW, wire[i].x1, wire[i].y1, cadhalfdotsize, 0, 360);
                  }
                  if( wire[i].end2 >1 ) {
                    filledarc(hilight_layer, NOW, wire[i].x2, wire[i].y2, cadhalfdotsize, 0, 360);
                  }
                }
              }
            }
            if(sel==1) {
              select_wire(i,SELECTED, 1);
              ui_state|=SELECTION;
            }
            if(sel==-1) {
              select_wire(i,0, 1);
            }
        }
        else {
          if(debug_var>=2) fprintf(errfp, "search():  not found wire=%d, tok=%s, val=%s search=%s\n", i,tok, str,val);
        }
      }
    }
    if(sel) for(c = 0; c < cadlayers; c++) for(i=0;i<lastline[c];i++) {
      str = get_tok_value(line[c][i].prop_ptr, tok,0);
      if(get_tok_size) {
#ifdef __unix__
        if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
            ( !strcmp(str, val) &&  sub ))
#else
        if ((!strcmp(str, val) && sub))
#endif
        {
            if(sel==1) {
              select_line(c, i,SELECTED, 1);
              ui_state|=SELECTION;
            }
            if(sel==-1) {
              select_line(c, i,0, 1);
            }
        }
        else {
          if(debug_var>=2) fprintf(errfp, "search(): not found line=%d col=%d, tok=%s, val=%s search=%s\n", 
                              i, c, tok, str, val);
        }
      }
    }
    if(sel) for(c = 0; c < cadlayers; c++) for(i=0;i<lastrect[c];i++) {
      str = get_tok_value(rect[c][i].prop_ptr, tok,0);
      if(get_tok_size) {
#ifdef __unix__
        if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
            ( !strcmp(str, val) &&  sub ))
#else
        if ((!strcmp(str, val) && sub))
#endif
        {
            if(sel==1) {
              select_box(c, i,SELECTED, 1);
              ui_state|=SELECTION;
            }
            if(sel==-1) {
              select_box(c, i,0, 1);
            }
        }
        else {
          if(debug_var>=2) fprintf(errfp, "search(): not found rect=%d col=%d, tok=%s, val=%s search=%s\n", 
                              i, c, tok, str, val);
        }
      }
    }
 }
 else if(what==END) redraw_hilights(); /* draw_hilight_net(1); */
 if(sel) {
   drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 }
#ifdef __unix__
 regfree(&re);
#endif
 draw_window = save_draw;

 if(event_reporting) {
   printf("xschem search %s %d %s %s\n", (sub ? "exact" : "regex"), sel, tok, val);
   fflush(stdout);
 }
}



/* 20171211 */
/* "drill" option (pass through resistors or pass gates or whatever elements with  */
/* 'propagate_to' properties set on pins) */
void drill_hilight(void)
{
  static char *netname=NULL, *propagated_net=NULL;
  int mult=0;
  int found;
  Instdef *symbol;
  Box *rect;
  int i, j, npin;
  char *propagate_str;
  int propagate;
  struct hilight_hashentry *entry, *propag_entry;
  int count;

  prepare_netlist_structs(0);
  count=0;
  while(1) { 
    found=0;
    count++;
    for(i=0; i<lastinst;i++) {
      symbol = inst_ptr[i].ptr+instdef;
      npin = symbol->rects[PINLAYER];
      rect=symbol->boxptr[PINLAYER];
      for(j=0; j<npin;j++) {
        my_strdup(143, &netname, pin_node(i, j, &mult, 1));
        propagate_str=get_tok_value(rect[j].prop_ptr, "propagate_to", 0);
        if(propagate_str[0] && (entry=bus_hilight_lookup(netname, 0, 2))) {
          propagate = atoi(propagate_str);
          my_strdup(144, &propagated_net, pin_node(i, propagate, &mult, 1)); /* get net to propagate highlight to... */
          propag_entry = bus_hilight_lookup(propagated_net, entry->value, 0); /* add net to highlight list */
          if(!propag_entry) {
            /* fprintf(errfp, "inst %s: j=%d  count=%d propagate=%d --> net %s, propagate to --> %s color %d\n",  */
            /*   inst_ptr[i].instname, j, count, propagate, netname, pin_node(i, propagate, &mult, 1), entry->value); */
            found=1; /* keep looping until no more nets are found. */
          }
          
        }
      } /* for(j...) */
    } /* for(i...) */
    if(!found) break;
  } /* while(1) */
}


void hilight_net(void)
{
  int i, n;
  char *type;

  prepare_netlist_structs(0);
  if(debug_var>=1) fprintf(errfp, "hilight_net(): entering\n");
  rebuild_selected_array();
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case WIRE: 
        if(event_reporting) {
          char s[PATH_MAX];
          printf("xschem search exact %d lab %s\n", 0, escape_chars(s, wire[n].node, PATH_MAX));
          fflush(stdout);
        }
        hilight_nets=1;
        if(!bus_hilight_lookup(wire[n].node, hilight_color,0)) {
          if(incr_hilight) hilight_color++;
        }
     break;

    case ELEMENT:
     type = (inst_ptr[n].ptr+instdef)->type;
     if( type &&
         inst_ptr[n].node &&   /* instance must have a pin! */
         !(strcmp(type,"label") && strcmp(type,"ipin") &&
           strcmp(type,"iopin") && strcmp(type,"opin") )
       )
     {
       if(event_reporting) {
         char s[PATH_MAX];
         printf("xschem search exact %d lab %s\n", 0, escape_chars(s, inst_ptr[n].node[0], PATH_MAX));
         fflush(stdout);
       }
       if(!bus_hilight_lookup(inst_ptr[n].node[0], hilight_color,0)) {
         hilight_nets=1;
         if(incr_hilight) hilight_color++;
       }
     } else {
       if(event_reporting) {
         char s[PATH_MAX];
         printf("xschem search exact %d name %s\n", 0, escape_chars(s, inst_ptr[n].instname, PATH_MAX));
         fflush(stdout);
       }
       if(debug_var>=1) fprintf(errfp, "hilight_net(): setting hilight flag on inst %d\n",n);
       hilight_nets=1;
       inst_ptr[n].flags |= 4;
     }
     break;
    default:
     break;
   }
  }
  if(!incr_hilight) hilight_color++;
  if(enable_drill) {
    drill_hilight();
    /*traverse_schematic(); */
  }
}

void unhilight_net(void)
{
  int i,n;
  char *type;

  prepare_netlist_structs(0);
  if(debug_var>=1) fprintf(errfp, "unhilight_net(): entering\n");
  rebuild_selected_array();
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case WIRE: 
      if(event_reporting) {
        char s[PATH_MAX];
        printf("xschem search exact %d lab %s\n", 1, escape_chars(s, wire[n].node, PATH_MAX));
        printf("xschem unhilight\n");
        fflush(stdout);
      }
      bus_hilight_lookup(wire[n].node, hilight_color,1);
     break;
    case ELEMENT:
     type = (inst_ptr[n].ptr+instdef)->type;
     if( type &&
         inst_ptr[n].node &&   /* instance must have a pin! */
         !(strcmp(type,"label") && strcmp(type,"ipin") &&
           strcmp(type,"iopin") && strcmp(type,"opin") )
       )
     {
      if(event_reporting) {
        printf("xschem unhilight\n");
        fflush(stdout);
      }
      bus_hilight_lookup(inst_ptr[n].node[0], hilight_color,1);
     } else {
      if(event_reporting) {
        char s[PATH_MAX];
        printf("xschem search exact %d name %s\n", 1, escape_chars(s, inst_ptr[n].instname, PATH_MAX));
        printf("xschem unhilight\n");
        fflush(stdout);
      }
     }
     inst_ptr[n].flags &= ~4;
     break;
    default:
     break;
   }
  }
  unselect_all();
}

void redraw_hilights(void)
{
  Box boundbox;
  if(!has_x) return;
  calc_drawing_bbox(&boundbox, 2);
  bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
  bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
}

void draw_hilight_net(int on_window)
{
 int save_draw;
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2; /* 20150409 */
 Instdef *symptr; /* 20160414 */
 int use_hash;
 struct wireentry *wireptr;
 int hilight_connected_inst;

 if(!hilight_nets) return;
 prepare_netlist_structs(0);
 save_draw = draw_window; /* 20181009 */
 draw_window = on_window;
 x1 = X_TO_XSCHEM(areax1);
 y1 = Y_TO_XSCHEM(areay1);
 x2 = X_TO_XSCHEM(areax2);
 y2 = Y_TO_XSCHEM(areay2);
 use_hash = (lastwire> 2000 || lastinst > 2000 ) &&  (x2 - x1  < ITERATOR_THRESHOLD);
 if(use_hash) {
   hash_wires();
 }
 if(!use_hash) for(i=0;i<lastwire;i++)
 {
      if( (entry = bus_hilight_lookup(wire[i].node, 0,2)) ) {
        if(wire[i].bus) /* 20171201 */
          drawline(get_color(entry->value), THICK,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        else
          drawline(get_color(entry->value), NOW,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        if(cadhalfdotsize*mooz>=0.7) {
          if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, wire[i].x1, wire[i].y1, cadhalfdotsize, 0, 360);
          }
          if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, wire[i].x2, wire[i].y2, cadhalfdotsize, 0, 360);
          }
        }
      }
 } else for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
    i = wireptr->n;
      if( (entry = bus_hilight_lookup(wire[i].node, 0,2)) ) {
        if(wire[i].bus) /* 20171201 */
          drawline(get_color(entry->value), THICK,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        else
          drawline(get_color(entry->value), NOW,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        if(cadhalfdotsize*mooz>=0.7) {
          if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, wire[i].x1, wire[i].y1, cadhalfdotsize, 0, 360);
          }
          if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(get_color(entry->value), NOW, wire[i].x2, wire[i].y2, cadhalfdotsize, 0, 360);
          }
        }
      }
 }
 my_realloc(145, &inst_color,lastinst*sizeof(int)); 
 for(i=0;i<lastinst;i++)
 {
   /* 20150409 */
   x1=X_TO_SCREEN(inst_ptr[i].x1);
   x2=X_TO_SCREEN(inst_ptr[i].x2);
   y1=Y_TO_SCREEN(inst_ptr[i].y1);
   y2=Y_TO_SCREEN(inst_ptr[i].y2);
   inst_color[i]=0;
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
   /* /20150409 */

  type = (inst_ptr[i].ptr+instdef)->type; /* 20150409 */

  hilight_connected_inst = !strcmp(get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr, "highlight", 0), "true");
  if( inst_ptr[i].flags & 4) {
    if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance %d flags &4 true\n", i);
    inst_color[i]=PINLAYER;
  }
  else if(hilight_connected_inst) {
    int rects, j;
    if(debug_var >= 2) fprintf(errfp, "draw_hilight_net(): hilight_connected_inst inst=%d, node=%s\n", i, inst_ptr[i].node[0]);
    if( (rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER]) > 0 ) {
      for(j=0;j<rects;j++) {
        if( inst_ptr[i].node && inst_ptr[i].node[j]) {
          entry=bus_hilight_lookup(inst_ptr[i].node[j], 0, 2);
          if(entry) {
            inst_color[i]=get_color(entry->value);
            break;
          }
        }
      }
    }
  } else if( type &&
      !(strcmp(type,"label") && strcmp(type,"ipin") &&
        strcmp(type,"iopin") && strcmp(type,"opin") )
    )
  {
   entry=bus_hilight_lookup( get_tok_value(inst_ptr[i].prop_ptr,"lab",0) , 0, 2 );
   if(entry) inst_color[i]=get_color(entry->value);
  }
 }

 for(c=0;c<cadlayers;c++) {
   /* 20160414 from draw() */
   if(draw_single_layer!=-1 && c != draw_single_layer) continue; /* 20151117 */
 
   for(i=0;i<lastinst;i++)
   {
     if(inst_color[i] )
     {
      /* 20150409 */
      x1=X_TO_SCREEN(inst_ptr[i].x1);
      x2=X_TO_SCREEN(inst_ptr[i].x2);
      y1=Y_TO_SCREEN(inst_ptr[i].y1);
      y2=Y_TO_SCREEN(inst_ptr[i].y2);
      if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
      if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance:%d\n",i);
      drawline(inst_color[i], BEGIN, 0.0, 0.0, 0.0, 0.0);
      drawrect(inst_color[i], BEGIN, 0.0, 0.0, 0.0, 0.0);
      filledrect(inst_color[i], BEGIN, 0.0, 0.0, 0.0, 0.0);
      drawarc(inst_color[i], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
      /* 20160414 from draw() */
      symptr = (inst_ptr[i].ptr+instdef);
      if( c==0 || /*draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
          symptr->lines[c] ||
          symptr->rects[c] ||
          symptr->arcs[c] ||
          symptr->polygons[c] ||
          ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
        draw_symbol(ADD, inst_color[i], i,c,0,0,0.0,0.0);
      }
      filledrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
      drawarc(inst_color[i], END, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
      drawrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
      drawline(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
     }
   }
 }
 draw_window = save_draw;
}

/* show == 0   ==> create pins from highlight nets */
void print_hilight_net(int show)
{
 int i;
 FILE *fd;
 struct hilight_hashentry *entry;
 struct node_hashentry *node_entry;
 char cmd[2*PATH_MAX];  /* 20161122 overflow safe */
 char cmd2[2*PATH_MAX];  /* 20161122 overflow safe */
 char cmd3[2*PATH_MAX];  /* 20161122 overflow safe */
 char a[] = "create_pins";
 char b[] = "add_lab_prefix";
 char b1[] = "add_lab_no_prefix";
 static char *filetmp1 = NULL;
 static char *filetmp2 = NULL;
 char *filename_ptr;


 /* 20111116 20111201 */
 prepare_netlist_structs(1); /* use full prepare_netlist_structs(1)  to recognize pin direction */
                             /* when creating pins from hilight nets 20171221 */

 /* 20180924 */
 if(!(fd = open_tmpfile("hilight_", &filename_ptr)) ) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   return;
 }
 my_strdup(147, &filetmp2, filename_ptr);
 fclose(fd);
 if(!(fd = open_tmpfile("hilight_", &filename_ptr))) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   return;
 }
 my_strdup(148, &filetmp1, filename_ptr);

 if(show == 3) {
   tclsetvar("filetmp2",filetmp1);
 } else {
   tclsetvar("filetmp2",filetmp2);
 }
 tclsetvar("filetmp1",filetmp1);

 if(  filetmp1[0] == 0 || filetmp2[0] == 0 ) {
   if(debug_var>=1) fprintf(errfp, "print_hilight_net(): problems creating tmpfiles\n");
   return;
 }
 if(fd==NULL){ 
    if(debug_var>=1) fprintf(errfp, "print_hilight_net(): problems opening netlist file\n");
    return;
 }
 my_snprintf(cmd, S(cmd), "\"%s/order_labels.awk\"", tclgetvar("XSCHEM_SHAREDIR"));
 my_snprintf(cmd2, S(cmd2), "%s %s > %s", cmd, filetmp1, filetmp2);
 my_snprintf(cmd3, S(cmd3), "\"%s/sort_labels.awk\" %s", tclgetvar("XSCHEM_SHAREDIR"), filetmp1);
 for(i=0;i<HASHSIZE;i++) {
   entry=table[i];
   while(entry) {
     /* 20111116 */
     node_entry = bus_hash_lookup(entry->token, "",2, 0, "", "", "", "");

     /* 20170926 test for not null node_entry, this may happen if a hilighted net name has been changed */
     /* before invoking this function, in this case --> skip */
     if(node_entry && !strcmp(sch_prefix[currentsch], entry->path)) {
       if(show==3) {
         
         fprintf(fd, "%s%s\n", !strcmp(entry->path, ".") ? "" : entry->path, entry->token); /* 20111106 */

       } else if(show==1) { /* 20120926 */
         fprintf(fd, "%s\n",  entry->token); /* 20120926 */
       } else {
         if(node_entry->d.out==0 && node_entry->d.inout==0 )
           fprintf(fd, "%s   %s\n",  entry->token, "ipin");
         else if(node_entry->d.in==0 && node_entry->d.inout==0 )
           fprintf(fd, "%s   %s\n",  entry->token, "opin");
         else
           fprintf(fd, "%s   %s\n",  entry->token, "iopin");
       }
     }
     entry = entry ->next ;
   }
 }
 fclose(fd);
 if(system(cmd2)==-1) {
   fprintf(errfp, "print_hilight_net(): error executing cmd2\n");
 }
 if(show==2) {
   tcleval(b);
 }
 if(show==4) { /* 20120913 create labels from hilight pins without 'i' prefix */
   tcleval(b1);
 }
 if(show==1) {
   my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp2);
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==3) {
   if(system(cmd3)==-1) {
     fprintf(errfp, "print_hilight_net(): error executing cmd3\n");
   }

   my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp1);
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==0)  {
   tcleval(a);
 }
 xunlink(filetmp2);
 xunlink(filetmp1);

 /* 20170323 this delete_netlist_structs is necessary, without it segfaults when going back (ctrl-e)  */
 /* from a schematic after placing pins (ctrl-j) and changing some pin direction (ipin -->iopin) */
 prepared_hilight_structs=0; /* 20171212 */
 prepared_netlist_structs=0; /* 20171212 */
 /* delete_netlist_structs(); */
}

