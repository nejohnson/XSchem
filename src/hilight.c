/* File: hilight.c
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2018 Stefan Frederik Schippers
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
   if( (remove==0) )            /* insert data */
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
 prepare_netlist_structs(1);
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
 prepare_netlist_structs(1);
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

void search_inst(const char *tok, const char *val, int sub, int sel, int what)
{
 int save_draw;
 int i,c, col,tmp,bus=0;
 const char *str;
 char *type; 
 int has_token;
 const char empty_string[] = "";
 static char *tmpname=NULL;
 regex_t re;

 if(!val) {
   fprintf(errfp, "search_inst(): warning: null val key\n");
   return;
 }
 save_draw = draw_window;
 draw_window=1;
 if(regcomp(&re, val , REG_EXTENDED)) return;
 if(debug_var>=1) fprintf(errfp, "search_inst():val=%s\n", val);
 if(sel==1) {
   drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
 }
 if(what==ADD || what==NOW) {
   
    col=hilight_color;
    if(incr_hilight) hilight_color++;

    has_token = 0;
    prepare_netlist_structs(1);
    bus=bus_search(val);
    for(i=0;i<lastinst;i++) {
      if(!strcmp(tok,"cell__name")) {
        has_token = (inst_ptr[i].name != NULL) && inst_ptr[i].name[0];
        str = inst_ptr[i].name;
      } else if(!strncmp(tok,"cell__", 6)) {
        has_token = (inst_ptr[i].ptr+instdef)->prop_ptr && strstr((inst_ptr[i].ptr+instdef)->prop_ptr, tok + 6) != NULL;
        my_strdup(142, &tmpname,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,tok+6,0)); /* flexible cell__ search 20140408 */
        if(tmpname) {
          str = tmpname;
        } else {
          str = empty_string;
        }
      } else if(!strcmp(tok,"propstring")) { /* 20170408 */
        has_token = (inst_ptr[i].prop_ptr != NULL) && inst_ptr[i].prop_ptr[0];
        str = inst_ptr[i].prop_ptr;
      } else {
        has_token =  inst_ptr[i].prop_ptr && strstr(inst_ptr[i].prop_ptr, tok) != NULL;
        str = get_tok_value(inst_ptr[i].prop_ptr, tok,0);
      }
      if(debug_var>=1) fprintf(errfp, "search_inst(): inst=%d, tok=%s, val=%s \n", i,tok, str);
     
      if(bus && sub) {
       if(debug_var>=1) fprintf(errfp, "search_inst(): doing substr search on bus sig:%s inst=%d tok=%s val=%s\n", str,i,tok,val);
       str=expandlabel(str,&tmp);
      }
      if(str && has_token) {
        if( (!regexec(&re, str,0 , NULL, 0) && !sub) ||           /* 20071120 regex instead of strcmp */
            (strstr(str,val) && sub) ) 
        {
          type = (inst_ptr[i].ptr+instdef)->type;
          if( type && 
              !(strcmp(type,"label") && strcmp(type,"ipin") &&
                strcmp(type,"iopin") && strcmp(type,"opin")) && (str = get_tok_value(inst_ptr[i].prop_ptr, "lab",0))[0] 
            ) {
            if(!bus_hilight_lookup(str, col,0)) hilight_nets = 1;
            if(what==NOW) for(c=0;c<cadlayers;c++)
              draw_symbol_outline(NOW,col%(cadlayers-7)+7, i,c,0,0,0.0,0.0);
          }
          else {
            if(debug_var>=1) fprintf(errfp, "search_inst(): setting hilight flag on inst %d\n",i);
            hilight_nets=1;
            inst_ptr[i].flags |= 4;
            if(what==NOW) for(c=0;c<cadlayers;c++)
              draw_symbol_outline(NOW,col%(cadlayers-7)+7, i,c,0,0,0.0,0.0);  /* 20150804 */
          }
  
          if(sel==1) {
            select_element(i, SELECTED, 1);
            ui_state|=SELECTION;
          }
          if(sel==-1) { /* 20171211 unselect */
            select_element(i, 0, 1);
         }
        }
      }
      
    }
    for(i=0;i<lastwire;i++) {
      has_token = strstr(wire[i].prop_ptr, tok) != NULL;
      str = get_tok_value(wire[i].prop_ptr, tok,0);
      if(has_token ) {
        if(   (!regexec(&re, str,0 , NULL, 0) && !sub )  ||       /* 20071120 regex instead of strcmp */
              ( strstr(str, val) &&  sub )
          ) {
            str = get_tok_value(wire[i].prop_ptr, "lab",0);
            if(debug_var>=2) fprintf(errfp, "search_inst(): wire=%d, tok=%s, val=%s \n", i,tok, str);
            if(str && str[0]) {
               bus_hilight_lookup(str, col,0);
               if(what==NOW) {
                 drawline(col%(cadlayers-7)+7, NOW, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
               }
            }
            if(sel) {
              select_wire(i,SELECTED, 1);
              ui_state|=SELECTION;
            }
        }
        else {
          if(debug_var>=2) fprintf(errfp, "search_inst():  not found wire=%d, tok=%s, val=%s search=%s\n", i,tok, str,val);
        }
      }
    }
 }
 else if(what==END) draw_hilight_net(1);
 if(sel) {
   drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 }

 regfree(&re);
 draw_window = save_draw;
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

  prepare_netlist_structs(1);
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
  char *str;

  prepare_netlist_structs(1);
  if(debug_var>=1) fprintf(errfp, "hilight_net(): entering\n");
  rebuild_selected_array();
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case WIRE: 
     if(wire[n].sel==SELECTED)
     {
      str = get_tok_value(wire[n].prop_ptr, "lab",0);
      if(str && str[0])
      {
        hilight_nets=1;
        if(!bus_hilight_lookup(str, hilight_color,0)) {
          if(incr_hilight) hilight_color++;
        }
      }
     }
     break;

    case ELEMENT:
     if(inst_ptr[n].sel==SELECTED)
     {
      str = get_tok_value(inst_ptr[n].prop_ptr, "lab",0);
      if(str && str[0])
      {
       if(!bus_hilight_lookup(str, hilight_color,0)) {
         hilight_nets=1;
         if(incr_hilight) hilight_color++;
       }
      }
      else {
       if(debug_var>=1) fprintf(errfp, "hilight_net(): setting hilight flag on inst %d\n",n);
       hilight_nets=1;
       inst_ptr[n].flags |= 4;
      }
     }
     break;


    default:
     break;
   }
  }
  if(enable_drill) {
    drill_hilight();
    /*traverse_schematic(); */
  }
}


void unhilight_net(void)
{
  int i,n;
  char *str;


  prepare_netlist_structs(1);
  if(debug_var>=1) fprintf(errfp, "unhilight_net(): entering\n");
  rebuild_selected_array();
  for(i=0;i<lastselected;i++)
  {
   n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case WIRE: 
     if(wire[n].sel==SELECTED)
     {
      str = get_tok_value(wire[n].prop_ptr, "lab",0);
      /*str = wire[n].node; */
      if(str && str[0])
      {
       bus_hilight_lookup(str, hilight_color,1);

      }
     }
     break;

    case ELEMENT:
     if(inst_ptr[n].sel==SELECTED)
     {
      str = get_tok_value(inst_ptr[n].prop_ptr, "lab",0);
      /*str = wire[n].node; */
      inst_ptr[n].flags &= ~4;
      if(str && str[0])
      {
       bus_hilight_lookup(str, hilight_color,1);
      }
     }
     break;


    default:
     break;
   }
  }
  unselect_all();
}


void draw_hilight_net(int on_window)
{
 char *str;
 int save_draw;
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2; /* 20150409 */
 Instdef *symptr; /* 20160414 */
 int use_hash;
 struct wireentry *wireptr;

 if(!hilight_nets) return;
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
    str = get_tok_value(wire[i].prop_ptr, "lab",0);
    if(str[0]) {
      if( (entry = bus_hilight_lookup(str, 0,2)) ) {
        if(wire[i].bus) /* 20171201 */
          drawline(7+entry->value%(cadlayers-7), THICK,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        else
          drawline(7+entry->value%(cadlayers-7), NOW,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        if(CADHALFDOTSIZE*mooz>=0.7) {
          if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(7+entry->value%(cadlayers-7), NOW, wire[i].x1, wire[i].y1, CADHALFDOTSIZE, 0, 360);
          }
          if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(7+entry->value%(cadlayers-7), NOW, wire[i].x2, wire[i].y2, CADHALFDOTSIZE, 0, 360);
          }
        }
      }
    }
 } else for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
    i = wireptr->n;
    str = get_tok_value(wire[i].prop_ptr, "lab",0);
    if(str[0]) {
      if( (entry = bus_hilight_lookup(str, 0,2)) ) {
        if(wire[i].bus) /* 20171201 */
          drawline(7+entry->value%(cadlayers-7), THICK,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        else
          drawline(7+entry->value%(cadlayers-7), NOW,
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        if(CADHALFDOTSIZE*mooz>=0.7) {
          if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(7+entry->value%(cadlayers-7), NOW, wire[i].x1, wire[i].y1, CADHALFDOTSIZE, 0, 360);
          }
          if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(7+entry->value%(cadlayers-7), NOW, wire[i].x2, wire[i].y2, CADHALFDOTSIZE, 0, 360);
          }
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
  if( type &&
      !(strcmp(type,"label") && strcmp(type,"ipin") &&
        strcmp(type,"iopin") && strcmp(type,"opin") )
    )
  {
   entry=bus_hilight_lookup( get_tok_value(inst_ptr[i].prop_ptr,"lab",0) , 0, 2 );
   if(entry) inst_color[i]=7+entry->value%(cadlayers-7);
  }
  else if( inst_ptr[i].flags & 4) {
    if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance %d flags &4 true\n", i);
    inst_color[i]=PINLAYER;
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
     drawarc(inst_color[i], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
     /* 20160414 from draw() */
     symptr = (inst_ptr[i].ptr+instdef);
     if( c==0 || /*draw_symbol_outline call is needed on layer 0 to avoid redundant work (outside check) */
         symptr->lines[c] ||
         symptr->rects[c] ||
         ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
       draw_symbol_outline(ADD, inst_color[i], i,c,0,0,0.0,0.0);
     }
     filledrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
     drawarc(inst_color[i], END, 0.0, 0.0, 0.0, 0.0, 0.0);
     drawrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
     drawline(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
    }
  }
 }
 draw_window = save_draw;
}


void xdraw_hilight_net(int on_window)
{
 char *str;
 int save_draw;
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2; /* 20150409 */
 Instdef *symptr; /* 20160414 */

  if(!hilight_nets) return;
  save_draw = draw_window; /* 20181009 */
  draw_window = on_window;
  for(i=0;i<lastwire;i++)
  {
    /* 20150409 */
    x1=X_TO_SCREEN(wire[i].x1);
    x2=X_TO_SCREEN(wire[i].x2);
    y1=Y_TO_SCREEN(wire[i].y1);
    y2=Y_TO_SCREEN(wire[i].y2);
    if( LINE_OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) {
      continue;
    }
    /* /20150409 */

    str = get_tok_value(wire[i].prop_ptr, "lab",0);
    /*str = wire[i].node; */
    if(str && str[0]) {
      if( (entry = bus_hilight_lookup(str, 0,2)) ) {
        if(wire[i].bus) /* 20171201 */
          drawline(7+entry->value%(cadlayers-7), THICK, 
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        else
          drawline(7+entry->value%(cadlayers-7), NOW, 
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
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
  if( type &&
      !(strcmp(type,"label") && strcmp(type,"ipin") &&
        strcmp(type,"iopin") && strcmp(type,"opin") )
    )
  {
   entry=bus_hilight_lookup( get_tok_value(inst_ptr[i].prop_ptr,"lab",0) , 0, 2 );
   if(entry) inst_color[i]=7+entry->value%(cadlayers-7);
  }
  else if( inst_ptr[i].flags & 4) {
    if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance %d flags &4 true\n", i);
    inst_color[i]=PINLAYER;
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
     drawarc(inst_color[i], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
     /* 20160414 from draw() */
     symptr = (inst_ptr[i].ptr+instdef);
     if( c==0 || /*draw_symbol_outline call is needed on layer 0 to avoid redundant work (outside check) */
         symptr->lines[c] ||
         symptr->rects[c] ||
         ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
       draw_symbol_outline(ADD, inst_color[i], i,c,0,0,0.0,0.0);
     }
     filledrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
     drawarc(inst_color[i], END, 0.0, 0.0, 0.0, 0.0, 0.0);
     drawrect(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
     drawline(inst_color[i], END, 0.0, 0.0, 0.0, 0.0);
    }
  }
 }
 draw_window = save_draw;
}


void undraw_hilight_net(int on_window) /* 20160413 */
{
 char *str;
 int save_draw; /* 20181009 */
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2; /* 20150409 */
 Instdef *symptr; /* 20160414 */

 save_draw = draw_window; /* 20181009 */
 draw_window = on_window;

 for(i=0;i<lastwire;i++)
 {
   /* 20150409 */
   x1=X_TO_SCREEN(wire[i].x1);
   x2=X_TO_SCREEN(wire[i].x2);
   y1=Y_TO_SCREEN(wire[i].y1);
   y2=Y_TO_SCREEN(wire[i].y2);
   if( LINE_OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
   /* /20150409 */

   str = get_tok_value(wire[i].prop_ptr, "lab",0);
   /*str = wire[i].node; */
   if(str && str[0]) {
     if( (!bus_hilight_lookup(str, 0,2)) ) {
       if(wire[i].bus)  /* 20171201 */
         drawline(WIRELAYER, THICK, 
            wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
       else
         drawline(WIRELAYER, NOW, 
            wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        if(CADHALFDOTSIZE*mooz>=0.7) {
          if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
            filledarc(WIRELAYER, NOW, wire[i].x1, wire[i].y1, CADHALFDOTSIZE, 0, 360);
          }
          if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
            filledarc(WIRELAYER, NOW, wire[i].x2, wire[i].y2, CADHALFDOTSIZE, 0, 360);
          }
        }
     }
   }

 }
 my_realloc(146, &inst_color,lastinst*sizeof(int)); 
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
  if( type &&
      !(strcmp(type,"label") && strcmp(type,"ipin") &&
        strcmp(type,"iopin") && strcmp(type,"opin") )
    )
  {
   entry=bus_hilight_lookup( get_tok_value(inst_ptr[i].prop_ptr,"lab",0) , 0, 2 );
   if(entry) inst_color[i]=7+entry->value%(cadlayers-7);
  }
  else if( inst_ptr[i].flags & 4) {
    inst_color[i]=PINLAYER;
  }
 }

 for(c=0;c<cadlayers;c++) {
  /* 20160414 from draw() */
  if(draw_single_layer!=-1 && c != draw_single_layer) continue; /* 20151117 */
  drawline(c, BEGIN, 0.0, 0.0, 0.0, 0.0);
  drawrect(c, BEGIN, 0.0, 0.0, 0.0, 0.0);
  filledrect(c, BEGIN, 0.0, 0.0, 0.0, 0.0);
  drawarc(c, BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
  for(i=0;i<lastinst;i++)
  {
    if(!inst_color[i] )
    {
     if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance:%d\n",i);
     /* 20160414 from draw() */
     symptr = (inst_ptr[i].ptr+instdef);
     if( c==0 || /*draw_symbol_outline call is needed on layer 0 to avoid redundant work (outside check) */
         symptr->lines[c] ||
         symptr->rects[c] ||
         ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
       draw_symbol_outline(ADD, c, i,c,0,0,0.0,0.0);
     }
    }
  }
  filledrect(c, END, 0.0, 0.0, 0.0, 0.0);
  drawarc(c, END, 0.0, 0.0, 0.0, 0.0, 0.0);
  drawrect(c, END, 0.0, 0.0, 0.0, 0.0);
  drawline(c, END, 0.0, 0.0, 0.0, 0.0);
 }
 if(ui_state & SELECTION) draw_selection(gc[SELLAYER], 0); /* 20171211 */
 draw_window = save_draw;
}

/* show == 0   ==> create pins from highlight nets */
void print_hilight_net(int show)
{
 int i;
 FILE *fd;
 struct hilight_hashentry *entry;
 struct node_hashentry *node_entry;
 static char *cmd = NULL;  /* 20161122 overflow safe */
 static char *cmd2 = NULL;  /* 20161122 overflow safe */
 static char *cmd3 = NULL;  /* 20161122 overflow safe */
 char a[] = "create_pins";
 char b[] = "add_lab_prefix";
 char b1[] = "add_lab_no_prefix";
 static char *filetmp1 = NULL;
 static char *filetmp2 = NULL;
 char *filename_ptr;


 /* 20111116 20111201 */
 prepare_netlist_structs(0); /* use full prepare_netlist_structs(0)  to recognize pin direction */
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
 /* /20111106 */
 my_strdup(149, &cmd, tclgetvar("XSCHEM_SHAREDIR"));
 my_strcat(150, &cmd, "/order_labels.awk");
 my_strdup(151, &cmd2, cmd);
 my_strcat(152, &cmd2," ");
 my_strcat(153, &cmd2,filetmp1);
 my_strcat(154, &cmd2,">");
 my_strcat(155, &cmd2,filetmp2);

 /* 20111106 */
 my_strdup(156, &cmd3, tclgetvar("XSCHEM_SHAREDIR"));
 my_strcat(157, &cmd3, "/sort_labels.awk ");
 my_strcat(158, &cmd3, filetmp1);

 /*fd=fopen(filetmp1, "w"); */
 if(fd==NULL){ 
    if(debug_var>=1) fprintf(errfp, "print_hilight_net(): problems opening netlist file\n");
    return;
 }

 for(i=0;i<HASHSIZE;i++) {
   entry=table[i];
   while(entry) {
     /* 20111116 */
     node_entry = bus_hash_lookup(entry->token, "",2, 0, "", "", "", "");

     /* 20170926 test for not null node_entry, this may happen if a hilighted net name has been changed */
     /* before invoking this function, in this case --> skip */
     if(node_entry && !strcmp(sch_prefix[currentsch], entry->path)) {
       if(show==3) {
         fprintf(fd, "%s%s\n",  entry->path, entry->token); /* 20111106 */

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
 system(cmd2);
 if(show==2) {
   tcleval(b);
 }
 if(show==4) { /* 20120913 create labels from hilight pins without 'i' prefix */
   tcleval(b1);
 }
 if(show==1) {
   my_strdup(159, &cmd, "set ::retval [ read_data_nonewline ");
   my_strcat(160, &cmd, filetmp2);
   my_strcat(161, &cmd, " ]");
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==3) {
   system(cmd3);
   my_strdup(162, &cmd, "set ::retval [ read_data_nonewline ");
   my_strcat(163, &cmd, filetmp1);
   my_strcat(164, &cmd, " ]");
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==0)  {
   tcleval(a);
 }
 unlink(filetmp2);
 unlink(filetmp1);

 /* 20170323 this delete_netlist_structs is necessary, without it segfaults when going back (ctrl-e)  */
 /* from a schematic after placing pins (ctrl-j) and changing some pin direction (ipin -->iopin) */
 prepared_hilight_structs=0; /* 20171212 */
 prepared_netlist_structs=0; /* 20171212 */
 /* delete_netlist_structs(); */


}

