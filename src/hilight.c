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

static unsigned int hi_hash(const char *tok)
{
  register unsigned int hash = 0;
  register char *str;
  register int c;

  if(xctx->sch_path_hash[xctx->currsch] == 0) {
    str=xctx->sch_path[xctx->currsch];
    while ( (c = *str++) ) 
      hash = c + hash * 65599;
    xctx->sch_path_hash[xctx->currsch] = hash;
  } else { 
    hash = xctx->sch_path_hash[xctx->currsch];
  }
  while ( (c = *tok++) )
    hash = c + hash * 65599;
  return hash;
}

static struct hilight_hashentry *free_hilight_entry(struct hilight_hashentry *entry)
{
  struct hilight_hashentry *tmp;
  while(entry) {
    tmp = entry->next;
    my_free(757, &entry);
    entry = tmp;
  }
  return NULL;
}

void display_hilights(char **str)
{
  int i;
  int first = 1;
  struct hilight_hashentry *entry;
  for(i=0;i<HASHSIZE;i++) {
    entry = xctx->hilight_table[i];
    while(entry) {
      if(!first) my_strcat(93, str, " ");
      my_strcat(943, str,"{");
      my_strcat(1190, str, entry->path+1);
      my_strcat(1160, str, entry->token);
      my_strcat(562, str,"}");
      first = 0;
      entry = entry->next;
    }
  }
}

static int there_are_hilights()
{
  register int i;
  register xInstance * inst =  xctx->inst;
  register struct hilight_hashentry **hiptr = xctx->hilight_table;
  for(i=0;i<HASHSIZE;i++) {
    if(hiptr[i]) return 1;
  }
  for(i = 0; i < xctx->instances; i++) {
    if(inst[i].color != -10000) return 1; 
  }
  return 0;
}

void free_hilight_hash(void) /* remove the whole hash table  */
{
 int i;

 dbg(2, "free_hilight_hash(): removing hash table\n");
 for(i=0;i<HASHSIZE;i++)
 {
  xctx->hilight_table[i] = free_hilight_entry( xctx->hilight_table[i] );
 }
}

/* by default: 
 * active_layer[0] = 7
 * active_layer[1] = 8
 * active_layer[2] = 10  if 9 is disabled it is skipped
 * ...
 * if a layer is disabled (not viewable) it is skipped
 * n_active_layers is the total number of layers for hilights.
 * standard xschem conf: cadlayers=22, n_active_layers=15 if no disabled layers.
 */
int get_color(int value)
{
  int x;

  if(value < 0) return (-value) % cadlayers ;
  if(n_active_layers) {
    x = value%(n_active_layers);
    return active_layer[x];
  } else {
    return cadlayers > 5 ? 5 : cadlayers -1; /* desperate attempt to return a decent color */
  }
}


void incr_hilight_color(void)
{
  xctx->hilight_color = (xctx->hilight_color + 1) % (n_active_layers * cadlayers);
}

/* print all highlight signals which are not ports (in/out/inout). */
void create_plot_cmd(int viewer)
{
  int i, c, idx, first;
  struct hilight_hashentry *entry;
  struct node_hashentry *node_entry;
  char *tok;
  char plotfile[PATH_MAX];
  char color_str[18];
  FILE *fd = NULL;
  char *str = NULL;

  my_snprintf(plotfile, S(plotfile), "%s/xplot", netlist_dir);
  if(viewer == NGSPICE) {
    if(!(fd = fopen(plotfile, "w"))) {
      fprintf(errfp, "create_plot_cmd(): error opening xplot file for writing\n");
      return;
    }
    fprintf(fd, "*ngspice plot file\n.control\n");
  }
  if(viewer == GAW) tcleval("if { ![info exists gaw_fd] } { gaw_setup_tcp }\n");
  idx = 1;
  first = 1;
  for(i=0;i<HASHSIZE;i++) /* set ngspice colors */
  {
    entry = xctx->hilight_table[i];
    while(entry) {
      tok = entry->token;
      node_entry = bus_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
      if(tok[0] == '#') tok++;
      if(node_entry && !strcmp(xctx->sch_path[xctx->currsch], entry->path) &&
         (node_entry->d.port == 0 || !strcmp(entry->path, ".") )) {
        c = get_color(entry->value);
        sprintf(color_str, "%02x/%02x/%02x", 
          xcolor_array[c].red>>8, xcolor_array[c].green>>8, xcolor_array[c].blue>>8);
        idx++;
        if(viewer == NGSPICE) {
          if(idx > 9) {
            idx = 2;
            fprintf(fd, str);
            fprintf(fd, "\n");
            first = 1;
            my_free(758, &str);
          }
          fprintf(fd, "set color%d=rgb:%s\n", idx, color_str);
          if(first) {
            my_strcat(164, &str, "plot ");
            first = 0;
          }
          my_strcat(160, &str, "\"");
          my_strcat(161, &str, (entry->path)+1);
          my_strcat(162, &str, tok);
          my_strcat(163, &str, "\" ");
        }
        if(viewer == GAW) {
          char *t=NULL, *p=NULL;
          my_strdup(241, &t, tok);
          my_strdup2(245, &p, (entry->path)+1);
          Tcl_VarEval(interp, "puts $gaw_fd {copyvar v(", strtolower(p), strtolower(t),
                      ") p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
          my_free(759, &p);
          my_free(760, &t);
        }
      }
      entry = entry->next;
    }
  }
  if(viewer == NGSPICE) {
    fprintf(fd, str);
    fprintf(fd, "\nremcirc\n.endc\n");
    my_free(761, &str);
    fclose(fd);
  }
  if(viewer == GAW) {
     tcleval("vwait gaw_fd; close $gaw_fd; unset gaw_fd\n");
  }
}

struct hilight_hashentry *hilight_lookup(const char *token, int value, int what)
/*    token           what       ... what ...
 * --------------------------------------------------------------------------
 * "whatever"         XINSERT    insert in hash table if not in and return NULL. If already present update
 *                               value if not NULL, retun new value, return NULL otherwise
 * "whatever"  XINSERT_NOREPLACE same as XINSERT but do not replace existing value, return NULL if not found.
 * "whatever"         XDELETE    delete entry if found, return NULL
 * "whatever"         XLOOKUP    only look up element, dont insert, return NULL if not found.
 */
{
  unsigned int hashcode, index;
  struct hilight_hashentry *entry, *saveptr, **preventry;
  char *ptr;
  int s ;
  int depth=0;
 
  if(token==NULL) return NULL;
  hashcode=hi_hash(token);
  index=hashcode % HASHSIZE;
  entry=xctx->hilight_table[index];
  preventry=&xctx->hilight_table[index];
  depth=0;
  while(1) {
    if( !entry ) { /* empty slot */
      int lent = (strlen(token) + 8) & ~(size_t) 0x7; /* align to 8 byte boundaries */
      int lenp = strlen(xctx->sch_path[xctx->currsch]) + 1;
      if( what==XINSERT || what == XINSERT_NOREPLACE) { /* insert data */
        s=sizeof( struct hilight_hashentry ) + lent + lenp;
        ptr= my_malloc(137, s );
        entry=(struct hilight_hashentry *)ptr;
        entry->next = NULL;
        entry->token = ptr + sizeof( struct hilight_hashentry );
        memcpy(entry->token, token, lent);
        entry->path = entry->token + lent;
        memcpy(entry->path, xctx->sch_path[xctx->currsch], lenp);
        entry->oldvalue = value-1000; /* no old value, set different value anyway*/
        entry->value = value;
        entry->time = xctx->hilight_time;
        entry->hash = hashcode;
        *preventry = entry;
        xctx->hilight_nets = 1; /* some nets should be hilighted ....  07122002 */
      }
      return NULL; /* whether inserted or not return NULL since it was not in */
    }
    if( entry -> hash==hashcode && !strcmp(token,entry->token) &&
         !strcmp(xctx->sch_path[xctx->currsch], entry->path)  ) { /* found matching tok */
      if(what==XDELETE) {              /* remove token from the hash table ... */
        saveptr=entry->next;
        my_free(764, &entry);
        *preventry=saveptr;
      } else if(what == XINSERT ) {
        entry->oldvalue = entry->value;
        entry->value = value;
        entry->time=xctx->hilight_time;
      }
      return entry; /* found matching entry, return the address */
    }
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
    depth++;
  }
}

/* warning, in case of buses return only pointer to first found bus element */
struct hilight_hashentry *bus_hilight_lookup(const char *token, int value, int what)
{
  char *start, *string_ptr, c;
  char *string=NULL;
  struct hilight_hashentry *ptr1=NULL, *ptr2=NULL;
  int mult;
 
  if(token==NULL) return NULL;
  if( token[0] == '#' || !strpbrk(token, "*[],.:")) {
    ptr1=hilight_lookup(token, value, what);
    return ptr1;
  }
  my_strdup(141, &string, expandlabel(token,&mult));
  if(string==NULL) {
    return NULL;
  }
  string_ptr = start = string;
  while(1) {
    c=(*string_ptr);
    if(c==','|| c=='\0') {
      *string_ptr='\0';  /* set end string at comma position.... */
      /* insert one bus element at a time in hash table */
      dbg(2, "bus_hilight_lookup(): inserting: %s, value:%d\n", start,value);
      ptr1=hilight_lookup(start, value, what);
      if(ptr1 && !ptr2) {
        ptr2=ptr1; /*return first non null entry */
        if(what==XLOOKUP) break; /* 20161221 no need to go any further if only looking up element */
      }
      *string_ptr=c;     /* ....restore original char */
      start=string_ptr+1;
    }
    if(c==0) break;
    string_ptr++;
  }
  /* if something found return first pointer */
  my_free(765, &string);
  return ptr2;
}

void clear_all_hilights(void)
{
  int i;
  xctx->hilight_color=0;
  if(!xctx->hilight_nets) return;
  free_hilight_hash();
  xctx->hilight_nets=0;
  for(i=0;i<xctx->instances;i++) {
    xctx->inst[i].color = -10000 ;
  }
  dbg(1, "clear_all_hilights(): clearing\n");
}

void hilight_net_pin_mismatches(void)
{
  int i,j,k;
  xSymbol *symbol;
  int npin;
  char *type=NULL;
  char *labname=NULL;
  char *lab=NULL;
  char *netname=NULL;
  int mult;
  xRect *rct;

  rebuild_selected_array();
  prepare_netlist_structs(0);
  for(k=0; k<xctx->lastsel; k++) {
    if(xctx->sel_array[k].type!=ELEMENT) continue;
    j = xctx->sel_array[k].n ;
    my_strdup(23, &type,(xctx->inst[j].ptr+ xctx->sym)->type);
    if( type && IS_LABEL_SH_OR_PIN(type)) break;
    symbol = xctx->sym + xctx->inst[j].ptr;
    npin = symbol->rects[PINLAYER];
    rct=symbol->rect[PINLAYER];
    dbg(1, "hilight_net_pin_mismatches(): \n");
    for(i=0;i<npin;i++) {
      my_strdup(24, &labname,get_tok_value(rct[i].prop_ptr,"name",0));
      my_strdup(25, &lab, expandlabel(labname, &mult));
      my_strdup(26, &netname, net_name(j,i, &mult, 0, 0));
      dbg(1, "hilight_net_pin_mismatches(): i=%d labname=%s explabname = %s  net = %s\n", i, labname, lab, netname);
      if(netname && strcmp(lab, netname)) {
        dbg(1, "hilight_net_pin_mismatches(): hilight: %s\n", netname);
        bus_hilight_lookup(netname, xctx->hilight_color, XINSERT_NOREPLACE);
        if(incr_hilight) incr_hilight_color();
      }
    }

  }
  my_free(713, &type);
  my_free(714, &labname);
  my_free(715, &lab);
  my_free(716, &netname);
  propagate_hilights(1, 0, XINSERT_NOREPLACE);
  redraw_hilights(0);
}

void hilight_parent_pins(void)
{
 int rects, i, j, k;
 struct hilight_hashentry *entry;
 const char *pin_name;
 char *pin_node = NULL;
 char *net_node=NULL;
 int mult, net_mult, inst_number;

 if(!xctx->hilight_nets) return;
 prepare_netlist_structs(0);
 i=xctx->previous_instance[xctx->currsch];
 inst_number = xctx->sch_inst_number[xctx->currsch+1];
 dbg(1, "hilight_parent_pins(): previous_instance=%d\n", xctx->previous_instance[xctx->currsch]);
 dbg(1, "hilight_parent_pins(): inst_number=%d\n", inst_number);

 rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];

 for(j=0;j<rects;j++)
 {
  if(!xctx->inst[i].node[j]) continue;
  my_strdup(445, &net_node, expandlabel(xctx->inst[i].node[j], &net_mult));
  dbg(1, "hilight_parent_pins(): net_node=%s\n", net_node);
  pin_name = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0);
  if(!pin_name[0]) continue;
  my_strdup(450, &pin_node, expandlabel(pin_name, &mult));
  dbg(1, "hilight_parent_pins(): pin_node=%s\n", pin_node);

  for(k = 1; k<=mult; k++) {
    xctx->currsch++;
    entry = bus_hilight_lookup(find_nth(pin_node, ',', k), 0, XLOOKUP);
    xctx->currsch--;
    if(entry)
    {
      bus_hilight_lookup(find_nth(net_node, ',',
          ((inst_number - 1) * mult + k - 1) % net_mult + 1), entry->value, XINSERT);
    }
    else
    {
      bus_hilight_lookup(find_nth(net_node, ',',
          ((inst_number - 1) * mult + k - 1) % net_mult + 1), 0, XDELETE);
    }
   }
 }
 my_free(767, &pin_node);
 my_free(768, &net_node);
}

void hilight_child_pins(void)
{
 int j, k, rects;
 const char *pin_name;
 char *pin_node = NULL;
 char *net_node=NULL;
 struct hilight_hashentry *entry;
 int mult, net_mult, i, inst_number;

 i = xctx->previous_instance[xctx->currsch-1];
 if(!xctx->hilight_nets) return;
 prepare_netlist_structs(0);
 rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER];
 inst_number = xctx->sch_inst_number[xctx->currsch];
 for(j=0;j<rects;j++)
 {
  dbg(1, "hilight_child_pins(): inst_number=%d\n", inst_number);

  if(!xctx->inst[i].node[j]) continue;
  my_strdup(508, &net_node, expandlabel(xctx->inst[i].node[j], &net_mult));
  dbg(1, "hilight_child_pins(): net_node=%s\n", net_node);
  pin_name = get_tok_value((xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0);
  if(!pin_name[0]) continue;
  my_strdup(521, &pin_node, expandlabel(pin_name, &mult));
  dbg(1, "hilight_child_pins(): pin_node=%s\n", pin_node);
  for(k = 1; k<=mult; k++) {
    dbg(1, "hilight_child_pins(): looking nth net:%d, k=%d, inst_number=%d, mult=%d\n",
                               (inst_number-1)*mult+k, k, inst_number, mult);
    dbg(1, "hilight_child_pins(): looking net:%s\n",  find_nth(net_node, ',',
        ((inst_number - 1) * mult + k - 1) % net_mult + 1));
    xctx->currsch--;
    entry = bus_hilight_lookup(find_nth(net_node, ',', 
      ((inst_number - 1) * mult + k - 1) % net_mult + 1), 0, XLOOKUP);
    xctx->currsch++;
    if(entry) {
      bus_hilight_lookup(find_nth(pin_node, ',', k), entry->value, XINSERT_NOREPLACE);
      dbg(1, "hilight_child_pins(): inserting: %s\n", find_nth(pin_node, ',', k));
    }
    else {
      bus_hilight_lookup(find_nth(pin_node, ',', k), 0, XDELETE);
      dbg(1, "hilight_child_pins(): deleting: %s\n", find_nth(pin_node, ',', k));
    }
  } /* for(k..) */
 }
 my_free(769, &pin_node);
 my_free(770, &net_node);
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

/* sel: -1 --> unselect
 *       1 --> select
 *       0 --> highlight
 */
int search(const char *tok, const char *val, int sub, int sel)
{
 int save_draw;
 int i,c, col = 7,tmp,bus=0;
 const char *str;
 char *type;
 int has_token;
 const char empty_string[] = "";
 char *tmpname=NULL;
 int found = 0;
 xRect boundbox;
 int big =  xctx->wires> 2000 || xctx->instances > 2000;
#ifdef __unix__
 regex_t re;
#endif

 /* when unselecting selected area should be redrawn */
 if(sel == -1 && !big) {
   calc_drawing_bbox(&boundbox, 1);
 }
 if(!val) {
   fprintf(errfp, "search(): warning: null val key\n");
   return TCL_ERROR;
 }
 save_draw = draw_window;
 draw_window=1;
#ifdef __unix__
 if(regcomp(&re, val , REG_EXTENDED)) return TCL_ERROR;
#endif
 dbg(1, "search():val=%s\n", val);
 if(!sel) {
   col=xctx->hilight_color;
   if(incr_hilight) incr_hilight_color();
 }
 has_token = 0;
 prepare_netlist_structs(0);
 bus=bus_search(val);
 for(i=0;i<xctx->instances;i++) {
   if(!strcmp(tok,"cell::name")) {
     has_token = (xctx->inst[i].name != NULL) && xctx->inst[i].name[0];
     str = xctx->inst[i].name;
   } else if(!strcmp(tok,"cell::propstring")) {
     has_token = (str = (xctx->inst[i].ptr+ xctx->sym)->prop_ptr) ? 1 : 0;
   } else if(!strncmp(tok,"cell::", 6)) { /* cell::xxx looks for xxx in global symbol attributes */
     my_strdup(142, &tmpname,get_tok_value((xctx->inst[i].ptr+ xctx->sym)->prop_ptr,tok+6,0));
     has_token = xctx->get_tok_size;
     if(tmpname) {
       str = tmpname;
     } else {
       str = empty_string;
     }
   } else if(!strcmp(tok,"propstring")) {
     has_token = (xctx->inst[i].prop_ptr != NULL) && xctx->inst[i].prop_ptr[0];
     str = xctx->inst[i].prop_ptr;
   } else {
     str = get_tok_value(xctx->inst[i].prop_ptr, tok,0);
     has_token = xctx->get_tok_size;
   }
   dbg(1, "search(): inst=%d, tok=%s, val=%s \n", i,tok, str);

   if(bus && sub) {
    dbg(1, "search(): doing substr search on bus sig:%s inst=%d tok=%s val=%s\n", str,i,tok,val);
    str=expandlabel(str,&tmp);
   }
   if(str && has_token) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub) ||           /* 20071120 regex instead of strcmp */
         (!strcmp(str, val) && sub && !bus) || (strstr(str,val) && sub && bus))
     #else
     if ((!strcmp(str, val) && sub && !bus) || (strstr(str,val) && sub && bus))
     #endif
     {
       if(!sel) {
         type = (xctx->inst[i].ptr+ xctx->sym)->type;
         if( type && xctx->inst[i].node && IS_LABEL_SH_OR_PIN(type) ) {
           bus_hilight_lookup(xctx->inst[i].node[0], col, XINSERT_NOREPLACE); /* sets xctx->hilight_nets=1; */
         } else {
           dbg(1, "search(): setting hilight flag on inst %d\n",i);
           xctx->hilight_nets=1;
           xctx->inst[i].color = col;
         }
       }
       if(sel==1) {
         xctx->inst[i].sel = SELECTED;
         xctx->need_reb_sel_arr=1;
       }
       if(sel==-1) { /* 20171211 unselect */
         xctx->inst[i].sel = 0;
         xctx->need_reb_sel_arr=1;
      }
      found  = 1;
     }
   }
 }
 for(i=0;i<xctx->wires;i++) {
   str = get_tok_value(xctx->wire[i].prop_ptr, tok,0);
   if(xctx->get_tok_size ) {
     #ifdef __unix__
     if(   (!regexec(&re, str,0 , NULL, 0) && !sub )  ||       /* 20071120 regex instead of strcmp */
           ( !strcmp(str, val) &&  sub ) )
     #else
     if (!strcmp(str, val) && sub)
     #endif
     {
       if(!sel) {
         bus_hilight_lookup(xctx->wire[i].node, col, XINSERT_NOREPLACE); /* sets xctx->hilight_nets = 1 */
       }
       if(sel==1) {
         xctx->wire[i].sel = SELECTED;
         xctx->need_reb_sel_arr=1;
       }
       if(sel==-1) {
         xctx->wire[i].sel = 0;
         xctx->need_reb_sel_arr=1;
       }
       found = 1;
     }
     else {
       dbg(2, "search():  not found wire=%d, tok=%s, val=%s search=%s\n", i,tok, str,val);
     }
   }
 }
 if(!sel) propagate_hilights(1, 0, XINSERT_NOREPLACE);
 if(sel) for(c = 0; c < cadlayers; c++) for(i=0;i<xctx->lines[c];i++) {
   str = get_tok_value(xctx->line[c][i].prop_ptr, tok,0);
   if(xctx->get_tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !strcmp(str, val) &&  sub ))
     #else
     if ((!strcmp(str, val) && sub))
     #endif
     {
       if(sel==1) {
         xctx->line[c][i].sel = SELECTED;
         xctx->need_reb_sel_arr=1;
       }
       if(sel==-1) {
         xctx->line[c][i].sel = 0;
         xctx->need_reb_sel_arr=1;
       }
       found = 1;
     }
     else {
       dbg(2, "search(): not found line=%d col=%d, tok=%s, val=%s search=%s\n",
                           i, c, tok, str, val);
     }
   }
 }
 if(sel) for(c = 0; c < cadlayers; c++) for(i=0;i<xctx->rects[c];i++) {
   str = get_tok_value(xctx->rect[c][i].prop_ptr, tok,0);
   if(xctx->get_tok_size) {
     #ifdef __unix__
     if( (!regexec(&re, str,0 , NULL, 0) && !sub ) ||
         ( !strcmp(str, val) &&  sub ))
     #else
     if ((!strcmp(str, val) && sub))
     #endif
     {
         if(sel==1) {
           xctx->rect[c][i].sel = SELECTED;
           xctx->need_reb_sel_arr=1;
         }
         if(sel==-1) {
           xctx->rect[c][i].sel = 0;
           xctx->need_reb_sel_arr=1;
         }
         found = 1;
     }
     else {
       dbg(2, "search(): not found rect=%d col=%d, tok=%s, val=%s search=%s\n",
                           i, c, tok, str, val);
     }
   }
 }
 if(found) {
   if(sel == -1) {
     if(!big) {
       bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
       bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
       bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
     }
     draw();
     if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
   }
   if(sel) {
     rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
     draw_selection(gc[SELLAYER], 0);
   }
   else redraw_hilights(0);
 }
 #ifdef __unix__
 regfree(&re);
 #endif
 draw_window = save_draw;
 my_free(771, &tmpname);
 return found;
}

/* "drill" option (pass through resistors or pass gates or whatever elements with  */
/* 'propag' properties set on pins) */
void drill_hilight(int mode)
{
  char *netname=NULL, *propagated_net=NULL;
  int mult=0;
  int found;
  xSymbol *symbol;
  xRect *rct;
  int i, j, npin;
  char *propagate_str = NULL;
  int propagate, hilight_connected_inst;
  struct hilight_hashentry *entry, *propag_entry;

  prepare_netlist_structs(0);
  while(1) {
    found=0;
    for(i=0; i<xctx->instances;i++) {
      symbol = xctx->inst[i].ptr+xctx->sym;
      npin = symbol->rects[PINLAYER];
      rct=symbol->rect[PINLAYER];
      hilight_connected_inst = en_hilight_conn_inst &&
           ( (xctx->inst[i].flags & 4) || ((xctx->inst[i].ptr+ xctx->sym)->flags & 4) );
      for(j=0; j<npin;j++) {
        my_strdup(143, &netname, net_name(i, j, &mult, 1, 0));
        entry=bus_hilight_lookup(netname, 0, XLOOKUP);
        if(entry && (hilight_connected_inst || (symbol->type && IS_LABEL_SH_OR_PIN(symbol->type))) ) {
          xctx->inst[i].color = entry->value;
        }
        my_strdup(1225, &propagate_str, get_tok_value(rct[j].prop_ptr, "propag", 0));
        if(propagate_str) {
          int n = 1;
          const char *propag;
          dbg(1, "propagate_logic(): inst=%d propagate_str=%s\n", i, propagate_str);
          while(1) {
            propag = find_nth(propagate_str, ',', n);
            n++;
            if(!propag[0]) break;
            if(entry) {
              propagate = atoi(propag);
              if(propagate < 0 || propagate >= npin) {
                 dbg(0, "Error: inst: %s, pin %d, goto set to %s <<%d>>\n",
                   xctx->inst[i].instname, j, propagate_str, propagate);
                   continue;
              }
              /* get net to propagate hilight to...*/
              my_strdup(144, &propagated_net, net_name(i, propagate, &mult, 1, 0));
              /* add net to highlight list */
              propag_entry = bus_hilight_lookup(propagated_net, entry->value, mode);
              if(!propag_entry) found=1; /* keep looping until no more nets are found. */
            }
          }
        }
      } /* for(j...) */
    } /* for(i...) */
    if(!found) break;
  } /* while(1) */
  my_free(772, &netname);
  my_free(773, &propagated_net);
  my_free(1226, &propagate_str);
}

int hilight_netname(const char *name)
{
  struct node_hashentry *node_entry;
  prepare_netlist_structs(0);
  dbg(1, "hilight_netname(): entering\n");
  rebuild_selected_array();
  node_entry = bus_hash_lookup(name, "", XLOOKUP, 0, "", "", "", "");
                    /* sets xctx->hilight_nets=1 */
  if(node_entry && !bus_hilight_lookup(name, xctx->hilight_color, XINSERT_NOREPLACE)) {
    if(incr_hilight) incr_hilight_color();
    propagate_hilights(1, 0, XINSERT_NOREPLACE);
    redraw_hilights(0);
  }
  return node_entry ? 1 : 0;
}

static void send_net_to_gaw(int simtype, const char *node)
{
  int c, k, tok_mult;
  struct node_hashentry *node_entry;
  const char *expanded_tok;
  const char *tok;
  char color_str[8];

  if(!node || !node[0]) return;
  tok = node;
  node_entry = bus_hash_lookup(tok, "", XLOOKUP, 0, "", "", "", "");
  if(tok[0] == '#') tok++;
  if(node_entry  && (node_entry->d.port == 0 || !strcmp(xctx->sch_path[xctx->currsch], ".") )) {
    char *t=NULL, *p=NULL;
    c = get_color(xctx->hilight_color);
    sprintf(color_str, "%02x%02x%02x", xcolor_array[c].red>>8, xcolor_array[c].green>>8,
                                       xcolor_array[c].blue>>8);
    expanded_tok = expandlabel(tok, &tok_mult);
    tcleval("if { ![info exists gaw_fd] } { gaw_setup_tcp }\n");
    for(k=1; k<=tok_mult; k++) {
      my_strdup(246, &t, find_nth(expanded_tok, ',', k));
      my_strdup2(254, &p, xctx->sch_path[xctx->currsch]+1);
      if(simtype == 0 ) { /* spice */
        Tcl_VarEval(interp, "puts $gaw_fd {copyvar v(", strtolower(p), strtolower(t),
                    ") p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
      } else { /* Xyce */
        char *c=p;
        while(*c){
          if(*c == '.') *c = ':'; /* Xyce uses : as path separator */
          c++;
        }
        Tcl_VarEval(interp, "puts $gaw_fd {copyvar ", strtoupper(p), strtoupper(t),
                    " p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
      }
    }
    my_free(774, &p);
    my_free(775, &t);
  }
}

static void send_current_to_gaw(int simtype, const char *node)
{
  int c, k, tok_mult;
  const char *expanded_tok;
  const char *tok;
  char color_str[8];
  char *t=NULL, *p=NULL;

  if(!node || !node[0]) return;
  tok = node;
  c = PINLAYER;
  sprintf(color_str, "%02x%02x%02x", xcolor_array[c].red>>8, xcolor_array[c].green>>8,
                                     xcolor_array[c].blue>>8);
  expanded_tok = expandlabel(tok, &tok_mult);
  tcleval("if { ![info exists gaw_fd] } { gaw_setup_tcp }\n");
  for(k=1; k<=tok_mult; k++) {
    my_strdup(1179, &t, find_nth(expanded_tok, ',', k));
    my_strdup2(1180, &p, xctx->sch_path[xctx->currsch]+1);
    if(!simtype) { /* spice */
      Tcl_VarEval(interp, "puts $gaw_fd {copyvar i(", xctx->currsch>0 ? "v." : "",
                  strtolower(p), strtolower(t),
                  ") p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
    } else {       /* Xyce */
      char *c=p;
      while(*c){
        if(*c == '.') *c = ':'; /* Xyce uses : as path separator */
        c++;
      }
      Tcl_VarEval(interp, "puts $gaw_fd {copyvar ", xctx->currsch>0 ? "V:" : "",
                  strtoupper(p), strtoupper( xctx->currsch>0 ? t+1 : t ), "#branch",
                  " p0 #", color_str, "}\nvwait gaw_fd\n", NULL);
    }
  }
  my_free(1181, &p);
  my_free(1182, &t);
}

/* hilight_instances: if set == 1 hilight non pin/label symbols with "highlight=true" attribute set */
void propagate_hilights(int set, int clear, int mode)
{
  int i, hilight_connected_inst;
  struct hilight_hashentry *entry;
  char *type;

  prepare_netlist_structs(0);
  for(i = 0; i < xctx->instances; i++) {
    if(xctx->inst[i].ptr < 0 ) {
      dbg(0, "propagate_hilights(): .ptr < 0, unbound symbol: inst %d, name=%s\n", i, xctx->inst[i].instname);
      continue;
    }
    type = (xctx->inst[i].ptr+ xctx->sym)->type;
    hilight_connected_inst = en_hilight_conn_inst && 
           ( (xctx->inst[i].flags & 4) || ((xctx->inst[i].ptr+ xctx->sym)->flags & 4) );
    if(hilight_connected_inst && type && !IS_LABEL_SH_OR_PIN(type)) {
      int rects, j, nohilight_pins;
      if( (rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) {
        nohilight_pins = 1;
        for(j=0;j<rects;j++) {
          if( xctx->inst[i].node && xctx->inst[i].node[j]) {
            entry=bus_hilight_lookup(xctx->inst[i].node[j], 0, XLOOKUP);
            if(entry) {
              if(set) {
                xctx->inst[i].color=entry->value;
              } else {
                nohilight_pins = 0; /* at least one connected net is hilighted: keep instance hilighted */
              }
              break;
            }
          }
        }
        if(nohilight_pins && clear) {
          xctx->inst[i].color=-10000;
        }
      }
    } else if(type && xctx->inst[i].node && IS_LABEL_SH_OR_PIN(type) ) {
      entry=bus_hilight_lookup( xctx->inst[i].node[0], 0, XLOOKUP);
      if(entry && set)        xctx->inst[i].color = entry->value;
      else if(!entry && clear) xctx->inst[i].color = -10000;
    }
  }
  xctx->hilight_nets = there_are_hilights();
  if(xctx->hilight_nets && enable_drill && set) drill_hilight(mode);
}

/* use negative values to bypass the normal hilight color enumeration */
#define LOGIC_0 -12  /* 0 */
#define LOGIC_1 -5   /* 1 */
#define LOGIC_X -1   /* 2 */
#define LOGIC_Z -13  /* 3 */
#define STACKMAX 200

int get_logic_value(int inst, int n)
{
  int /* mult, */ val;
  struct hilight_hashentry *entry;
  /* char *netname = NULL; */

  /* fast option: dont use net_name() (no expandlabel though) */
  /* THIS MUST BE DONE TO HANDLE VECTOR INSTANCES/BUSES */
  /* my_strdup(xxxx, &netname, net_name(inst, n, &mult, 1, 0)); */
  entry=hilight_lookup(xctx->inst[inst].node[n], 0, XLOOKUP);
  if(!entry) {
    val = 2; /* LOGIC_X */
  } else {
    val = entry->value;
    val = (val == LOGIC_0) ? 0 : (val == LOGIC_1) ? 1 : (val == LOGIC_Z) ? 3 : 2;
    /* dbg(1, "get_logic_value(): inst=%d pin=%d net=%s val=%d\n", inst, n, netname, val); */
  }
  /* my_free(xxxx, &netname); */
  return val;
}

void print_stack(int  *stack, int sp)
{
  int i;
  fprintf(errfp, "stack: ");
  for(i = 0; i < sp; i++) {
    fprintf(errfp, "%d ", stack[i]);
  }
  fprintf(errfp, "\n");
}

int eval_logic_expr(int inst, int output)
{
  int stack[STACKMAX];
  int pos = 0, i, s, sp = 0;
  char *str;
  int res = 0;

  str = xctx->simdata.inst[inst].pin[output].function;
  dbg(1, "eval_logic_expr(): inst=%d pin=%d function=%s\n", inst, output, str ? str : "NULL");
  if(!str) return 2; /* no logic function defined, return LOGIC_X */
  while(str[pos]) {
    switch(str[pos]) {
      case 'd': /* duplicate top element*/
        if(sp > 0 && sp < STACKMAX) {
          stack[sp] = stack[sp - 1];
          sp++;
        }
        break;
      case 'r': /* rotate down: bottom element goes to top */
        if(sp > 1) {
          s = stack[0];
          for(i = 0 ; i < sp - 1; i++) stack[i] = stack[i + 1];
          stack[sp - 1] = s;
        }
        break;
      case 'x': /* exchange top 2 operands */
        if(sp > 1) {
           s = stack[sp - 2];
           stack[sp - 2] =  stack[sp - 1];
           stack[sp - 1] = s;
        }
        break;
      case '~': /* negation operator */
        if(sp > 0) {
          sp--;
          if(!(stack[sp] & 2)) stack[sp] = !stack[sp];
          else stack[sp] = 2;
          ++sp;
        }
        break;
      case  'z': /* Tristate driver [signal,enable,'z']-> signal if z==1, Z (3) otherwise */
        if(sp > 1) {
          s = stack[sp - 1];
          stack[sp - 2] = (s & 2) ? 2 : (s == 1 ) ? stack[sp - 2] : 3;
          sp--;
        }
        break;
      case 'M': /* mux operator */
        s = stack[sp - 1];
        if(sp > 2) {
          if(!(s & 2) ) {
            stack[sp - 3] = (s == 0) ? stack[sp - 3]  : stack[sp - 2];
          }
          else stack[sp - 3] = 3;
          sp -=2;
        }
        break;
      case 'm': /* mux operator , lower priority*/
        s = stack[sp - 1];
        if(sp > 2) {
          if(!(s & 2) ) { /* reduce pessimism, avoid infinite loops */
            stack[sp - 3] = (s == 0) ? stack[sp - 3]  : stack[sp - 2];
          }
          else stack[sp - 3] = 4;
          sp -=2;
        }
        break;
      case '|': /* or operator */
        if(sp > 1) {
          res = 0;
          for(i = sp - 2; i < sp; i++) {
            if(stack[i] == 1) {
              res = 1;
              break;
            } else if(stack[i] & 2) {
              res = 2;
            }
          }
          stack[sp - 2] = res;
          sp--;
        }
        break;
      case '&': /* and operator */
        if(sp > 1) {
          res = 1;
          for(i = sp - 2; i < sp; i++) {
            if(stack[i] == 0) {
              res = 0;
              break;
            } else if(stack[i] & 2) {
              res = 2;
            }
          }
          stack[sp - 2] = res;
          sp--;
        }
        break;
      case '^': /* xor operator */
        if(sp > 1) {
          res = 0;
          for(i = sp - 2; i < sp; i++) {
            if(!(stack[i] & 2)) {
              res = res ^ stack[i];
            }
            else {
              res = 2;
              break;
            }
          }
          stack[sp - 2] = res;
          sp--;
        }
        break;
      case 'L': /* logic low (0) */
        if(sp < STACKMAX) {
          stack[sp++] = 0;
        }
        break;
      case 'H': /* logic high (1) */
        if(sp < STACKMAX) {
          stack[sp++] = 1;
        }
        break;
      case 'Z': /* logic Z (3) */
        if(sp < STACKMAX) {
          stack[sp++] = 3;
        }
        break;
      case 'U': /* Do not assign to node */
        if(sp < STACKMAX) {
          stack[sp++] = 4;
        }
        break;
      default:
        break;
    } /* switch */
    if(isdigit(str[pos])) {
      if(sp < STACKMAX) {
        char *num = str + pos;
        while(isdigit(str[++pos])) ;
        pos--; /* push back last non digit character */
        stack[sp++] = get_logic_value(inst, atoi(num));
      }
      else dbg(0, "eval_logic_expr(): stack overflow!\n");
    }
    pos++;
  } /* while */
  dbg(1, "eval_logic_expr(): inst %d output %d, returning %d\n", inst, output, stack[0]);
  return stack[0];
}

void create_simdata(void)
{
  int i, j;
  const char *str;
  free_simdata();
  xctx->simdata.inst = NULL;
  my_realloc(60, &xctx->simdata.inst, xctx->instances * sizeof(struct simdata_inst));
  xctx->simdata.ninst = xctx->instances;
  for(i = 0; i < xctx->instances; i++) {
    xSymbol *symbol = xctx->inst[i].ptr + xctx->sym;
    int npin = symbol->rects[PINLAYER];
    xctx->simdata.inst[i].pin = NULL;
    my_realloc(61, &xctx->simdata.inst[i].pin, npin * sizeof(struct simdata_pin));
    xctx->simdata.inst[i].npin = npin;
    for(j = 0; j < npin; j++) {
      char function[20];
      xctx->simdata.inst[i].pin[j].function=NULL;
      xctx->simdata.inst[i].pin[j].go_to=NULL;
      my_snprintf(function, S(function), "function%d", j);
      my_strdup(717, &xctx->simdata.inst[i].pin[j].function, get_tok_value(symbol->prop_ptr, function, 0));
      my_strdup(963, &xctx->simdata.inst[i].pin[j].go_to, 
                get_tok_value(symbol->rect[PINLAYER][j].prop_ptr, "goto", 0));
      str = get_tok_value(symbol->rect[PINLAYER][j].prop_ptr, "clock", 0);
      xctx->simdata.inst[i].pin[j].clock = str[0] ? str[0] - '0' : -1;
    }
  }
  xctx->simdata.valid = 1;
}

void free_simdata(void)
{
  int i, j;

  if(xctx->simdata.inst) {
    for(i = 0; i < xctx->simdata.ninst; i++) {
      int npin = xctx->simdata.inst[i].npin;
      for(j = 0; j < npin; j++) {
        my_free(1219, &xctx->simdata.inst[i].pin[j].function);
        my_free(1220, &xctx->simdata.inst[i].pin[j].go_to);
      }
      my_free(1221, &xctx->simdata.inst[i].pin);
    }
    my_free(1222, &xctx->simdata.inst);
  }
  xctx->simdata.valid = 0;
}

#define DELAYED_ASSIGN
void propagate_logic()
{
  /* char *propagated_net=NULL; */
  int found, iter = 0 /* , mult */;
  int i, j, npin;
  int propagate;
  int min_iter = 3; /* set to 3 to simulate up to 3 series bidirectional pass-devices */
  struct hilight_hashentry  *entry;
  int val, oldval, newval;
  static int map[] = {LOGIC_0, LOGIC_1, LOGIC_X, LOGIC_Z};
  #ifdef DELAYED_ASSIGN
  int *newval_arr = NULL;
  #endif

  prepare_netlist_structs(0);
  while(1) {
    found=0;
    for(i=0; i<xctx->simdata.ninst; i++) {
      npin = xctx->simdata.inst[i].npin;
      #ifdef DELAYED_ASSIGN
      my_realloc(778, &newval_arr, npin * sizeof(int));
      for(j=0; j<npin;j++) newval_arr[j] = -10000;
      #endif
      for(j=0; j<npin;j++) {
        if(xctx->simdata.inst && xctx->simdata.inst[i].pin && xctx->simdata.inst[i].pin[j].go_to) {
          int n = 1;
          const char *propag;
          int clock_pin, clock_val, clock_oldval;
          clock_pin = xctx->simdata.inst[i].pin[j].clock;
          if(clock_pin != -1) {
            /* no bus_hilight_lookup --> no bus expansion */
            entry = hilight_lookup(xctx->inst[i].node[j], 0, XLOOKUP); /* clock pin */
            clock_val =  (!entry) ? LOGIC_X : entry->value;
            clock_oldval =  (!entry) ? LOGIC_X : entry->oldvalue;
            if(entry) {
                if(clock_pin == 0) { /* clock falling edge */
                  if( clock_val == clock_oldval) continue;
                  if( clock_val != LOGIC_0) continue;
                  if(entry && entry->time != xctx->hilight_time) continue;
                } else if(clock_pin == 1) { /* clock rising edge */
                  if( clock_val == clock_oldval) continue;
                  if( clock_val != LOGIC_1) continue;
                  if(entry && entry->time != xctx->hilight_time) continue;
                } else if(clock_pin == 2) { /* set/clear active low */
                  if( clock_val != LOGIC_0) continue;
                } else if(clock_pin == 3) { /* set/clear active high */
                  if( clock_val != LOGIC_1) continue;
                }
            }
          }
          dbg(1, "propagate_logic(): inst=%d pin %d, goto=%s\n", i,j, xctx->simdata.inst[i].pin[j].go_to);
          while(1) {
            propag = find_nth(xctx->simdata.inst[i].pin[j].go_to, ',', n);
            n++;
            if(!propag[0]) break;
            propagate = atoi(propag);
            if(propagate < 0 || propagate >= npin) {
               dbg(0, "Error: inst: %s, pin %d, goto set to %s <<%d>>\n",
                 xctx->inst[i].instname, j, xctx->simdata.inst[i].pin[j].go_to, propagate);
                 continue;
            }
            if(!xctx->inst[i].node[propagate]) {
              dbg(0, "Error: inst %s, output in %d unconnected\n", xctx->inst[i].instname, propagate);
              break;
            }
            /* get net to propagate hilight to...*/
            /* fast option: dont use net_name() (no expandlabel though) */
            /* THIS MUST BE DONE TO HANDLE VECTOR INSTANCES/BUSES */
            /* my_strdup(xxx, &propagated_net, net_name(i, propagate, &mult, 1, 0)); */
            /* dbg(1, "propagate_logic(): inst %d pin %d propag=%s n=%d\n", i, j, propag, n);
             * dbg(1, "propagate_logic(): inst %d pin %d propagate=%d\n", i, j, propagate);
             * dbg(1, "propagate_logic(): propagated_net=%s\n", propagated_net); */
            /* add net to highlight list */
            /* no bus_hilight_lookup --> no bus expansion */
            entry = hilight_lookup(xctx->inst[i].node[propagate], 0, XLOOKUP); /* destination pin */
            oldval = (!entry) ? LOGIC_X : entry->value;
            newval = eval_logic_expr(i, propagate);
            val =  map[newval];
            if( newval !=4 && (iter < min_iter || (newval !=3 && oldval != val) )) {
              dbg(1, "propagate_logic(): inst %d pin %d oldval %d newval %d to pin %s\n",
                   i, j, oldval, val, xctx->inst[i].node[propagate]);

              #ifdef DELAYED_ASSIGN
              newval_arr[propagate] = val;
              #else 
              hilight_lookup(xctx->inst[i].node[propagate], val, XINSERT);
              #endif
              found=1; /* keep looping until no more nets are found. */
            }
          }
        }
      } /* for(j...) */
      #ifdef DELAYED_ASSIGN
      for(j=0;j<npin;j++) {
        if(newval_arr[j] != -10000) hilight_lookup(xctx->inst[i].node[j], newval_arr[j], XINSERT);
      }
      #endif
    } /* for(i...) */
    xctx->hilight_time++;
    if(!found) break;
    /* get out from infinite loops (circuit is oscillating) */
    Tcl_VarEval(interp, "update; if {$::tclstop == 1} {return 1} else {return 0}", NULL);
    if( tclresult()[0] == '1') break;
    iter++;
  } /* while(1) */
  #ifdef DELAYED_ASSIGN
  my_free(779, &newval_arr);
  #endif
  /* my_free(1222, &propagated_net); */
}

void logic_set(int value, int num)
{
  int i, j, n, newval;
  char *type;
  xRect boundbox;
  int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
  static int map[] = {LOGIC_0, LOGIC_1, LOGIC_X, LOGIC_Z};
  struct hilight_hashentry  *entry;
 
  prepare_netlist_structs(0);
  if(!xctx->simdata.valid) create_simdata();
  rebuild_selected_array();
  newval = value;
  if(!no_draw && !big) {
    calc_drawing_bbox(&boundbox, 2);
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
  }
  for(j = 0; j < num; j++) {
    for(i=0;i<xctx->lastsel;i++)
    {
      char *node = NULL;
      n = xctx->sel_array[i].n;
      switch(xctx->sel_array[i].type) {
        case WIRE:
          node = xctx->wire[n].node;
          break;
        case ELEMENT:
          type = (xctx->inst[n].ptr+ xctx->sym)->type;
          if( type && xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
            node = xctx->inst[n].node[0];
          }
          break;
        default:
          break;
      }
      if(node) {
        if(value == -1) {
          entry = bus_hilight_lookup(node, 0, XLOOKUP);
          if(entry)
            newval = (entry->value == LOGIC_1) ? 0 : (entry->value == LOGIC_0) ? 1 : 2;
          else newval = 2;
        }
        bus_hilight_lookup(node, map[newval], XINSERT);
      }
    }
    propagate_logic();
    propagate_hilights(1, 0, XINSERT);
  }
  if(!no_draw && !big) {
    calc_drawing_bbox(&boundbox, 2);
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  }
  draw();
  if(!no_draw && !big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  tcleval("if { [info exists gaw_fd] } {close $gaw_fd; unset gaw_fd}\n");
}


void hilight_net(int to_waveform)
{
  int i, n;
  char *type;
  int sim_is_xyce;

  prepare_netlist_structs(0);
  dbg(1, "hilight_net(): entering\n");
  rebuild_selected_array();
  tcleval("sim_is_xyce");
  sim_is_xyce = atoi( tclresult() );
  for(i=0;i<xctx->lastsel;i++) {
   n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type) {
    case WIRE:
         /* sets xctx->hilight_nets=1 */
     if(!bus_hilight_lookup(xctx->wire[n].node, xctx->hilight_color, XINSERT_NOREPLACE)) {
       if(to_waveform == GAW) send_net_to_gaw(sim_is_xyce, xctx->wire[n].node);
       if(incr_hilight) incr_hilight_color();
     }
     break;
    case ELEMENT:
     type = (xctx->inst[n].ptr+ xctx->sym)->type;
     if( type && xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
           /* sets xctx->hilight_nets=1 */
       if(!bus_hilight_lookup(xctx->inst[n].node[0], xctx->hilight_color, XINSERT_NOREPLACE)) {
         if(to_waveform == GAW) send_net_to_gaw(sim_is_xyce, xctx->inst[n].node[0]);
         if(incr_hilight) incr_hilight_color();
       }
     } else {
       dbg(1, "hilight_net(): setting hilight flag on inst %d\n",n);
       xctx->hilight_nets=1;
       xctx->inst[n].color = xctx->hilight_color;
       if(incr_hilight) incr_hilight_color();
     }
     if(type &&  (!strcmp(type, "current_probe") || !strcmp(type, "vsource")) ) {
       if(to_waveform == GAW) send_current_to_gaw(sim_is_xyce, xctx->inst[n].instname);
     }
     break;
    default:
     break;
   }
  }
  if(!incr_hilight) incr_hilight_color();
  propagate_hilights(1, 0, XINSERT_NOREPLACE);
  tcleval("if { [info exists gaw_fd] } {close $gaw_fd; unset gaw_fd}\n");
}

void unhilight_net(void)
{
  int i,n;
  char *type;
  xRect boundbox;
  int big =  xctx->wires> 2000 || xctx->instances > 2000 ;

  rebuild_selected_array();
  prepare_netlist_structs(0);
  if(!big) calc_drawing_bbox(&boundbox, 2);
  dbg(1, "unhilight_net(): entering\n");
  for(i=0;i<xctx->lastsel;i++) {
   n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type) {
    case WIRE:
      bus_hilight_lookup(xctx->wire[n].node, xctx->hilight_color, XDELETE);
     break;
    case ELEMENT:
     type = (xctx->inst[n].ptr+ xctx->sym)->type;
     if( type && xctx->inst[n].node && IS_LABEL_SH_OR_PIN(type) ) { /* instance must have a pin! */
      bus_hilight_lookup(xctx->inst[n].node[0], xctx->hilight_color, XDELETE);
     }
     xctx->inst[n].color = -10000;
     break;
    default:
     break;
   }
  }
  if(!big) {
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0); 
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  }
  propagate_hilights(0, 1, XINSERT_NOREPLACE);
  draw();
  if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);

  unselect_all();
}

/* redraws the whole affected rectangle, this avoids artifacts due to antialiased text */
void redraw_hilights(int clear)
{
  xRect boundbox;
  int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
  if(!has_x) return;
  if(!big) calc_drawing_bbox(&boundbox, 2);
  if(clear) clear_all_hilights();
  if(!big) {
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
    bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  }
  draw();
  if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
}


void select_hilight_net(void)
{
 char *type=NULL;
 int i;
 struct hilight_hashentry *entry;
 int hilight_connected_inst;

 if(!xctx->hilight_nets) return;
 prepare_netlist_structs(0);
 for(i=0;i<xctx->wires;i++) {
   if( (entry = bus_hilight_lookup(xctx->wire[i].node, 0, XLOOKUP)) ) {
      xctx->wire[i].sel = SELECTED;
   }
 }
 for(i=0;i<xctx->instances;i++) {

  type = (xctx->inst[i].ptr+ xctx->sym)->type;
  hilight_connected_inst = en_hilight_conn_inst && 
      ( (xctx->inst[i].flags & 4) || ((xctx->inst[i].ptr+ xctx->sym)->flags & 4) );
  if( xctx->inst[i].color != -10000) {
    dbg(1, "select_hilight_net(): instance %d flags &4 true\n", i);
     xctx->inst[i].sel = SELECTED;
  }
  else if(hilight_connected_inst) {
    int rects, j;
    if( (rects = (xctx->inst[i].ptr+ xctx->sym)->rects[PINLAYER]) > 0 ) {
      for(j=0;j<rects;j++) {
        if( xctx->inst[i].node && xctx->inst[i].node[j]) {
          entry=bus_hilight_lookup(xctx->inst[i].node[j], 0, XLOOKUP);
          if(entry) {
            xctx->inst[i].sel = SELECTED;
            break;
          }
        }
      }
    }
  } else if( type && xctx->inst[i].node && IS_LABEL_SH_OR_PIN(type) ) {
    entry=bus_hilight_lookup(xctx->inst[i].node[0], 0, XLOOKUP);
    if(entry) xctx->inst[i].sel = SELECTED;
  }
 }
 xctx->need_reb_sel_arr = 1;
 rebuild_selected_array(); /* sets or clears xctx->ui_state SELECTION flag */
 redraw_hilights(0);
 
}

void draw_hilight_net(int on_window)
{
 int save_draw;
 int i,c;
 double x1,y1,x2,y2;
 xSymbol *symptr;
 int use_hash;
 struct wireentry *wireptr;
 struct instentry *instanceptr;
 struct hilight_hashentry *entry;
 struct iterator_ctx ctx;

 if(!xctx->hilight_nets) return;
 dbg(3, "draw_hilight_net(): xctx->prep_hi_structs=%d\n", xctx->prep_hi_structs);
 prepare_netlist_structs(0);
 save_draw = draw_window;
 draw_window = on_window;
 x1 = X_TO_XSCHEM(xctx->areax1);
 y1 = Y_TO_XSCHEM(xctx->areay1);
 x2 = X_TO_XSCHEM(xctx->areax2);
 y2 = Y_TO_XSCHEM(xctx->areay2);
 use_hash = (xctx->wires> 2000 || xctx->instances > 2000 ) &&  (x2 - x1  < ITERATOR_THRESHOLD);
 if(use_hash) {
   hash_wires();
   hash_instances();
 }
 if(use_hash) init_wire_iterator(&ctx, x1, y1, x2, y2);
 else i = -1;
 while(1) {
   if(use_hash) {
     if( !(wireptr = wire_iterator_next(&ctx))) break;
     i = wireptr->n;
   }
   else {
     i++;
     if(i >= xctx->wires) break;
   }
   if( (entry = bus_hilight_lookup(xctx->wire[i].node, 0, XLOOKUP)) ) {
     if(xctx->wire[i].bus)
       drawline(get_color(entry->value), THICK,
          xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
     else
       drawline(get_color(entry->value), NOW,
          xctx->wire[i].x1, xctx->wire[i].y1, xctx->wire[i].x2, xctx->wire[i].y2, 0);
     if(cadhalfdotsize*xctx->mooz>=0.7) {
       if( xctx->wire[i].end1 >1 ) { /* 20150331 draw_dots */
         filledarc(get_color(entry->value), NOW, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360);
       }
       if( xctx->wire[i].end2 >1 ) { /* 20150331 draw_dots */
         filledarc(get_color(entry->value), NOW, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360);
       }
     }
   }
 }
 for(c=0;c<cadlayers;c++) {
   if(draw_single_layer!=-1 && c != draw_single_layer) continue;
   if(use_hash) init_inst_iterator(&ctx, x1, y1, x2, y2);
   else i = -1;
   while(1) {
     if(use_hash) {
       if( !(instanceptr = inst_iterator_next(&ctx))) break;
       i = instanceptr->n;
     }
     else {
       i++;
       if(i >= xctx->instances) break;
     }
     if(xctx->inst[i].color != -10000)
     {
      int col = get_color(xctx->inst[i].color);
      symptr = (xctx->inst[i].ptr+ xctx->sym);
      if( c==0 || /*draw_symbol call is needed on layer 0 to avoid redundant work (outside check) */
          symptr->lines[c] || symptr->rects[c] || symptr->arcs[c] || symptr->polygons[c] ||
          ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
        draw_symbol(ADD, col, i,c,0,0,0.0,0.0);
      }
      filledrect(col, END, 0.0, 0.0, 0.0, 0.0);
      drawarc(col, END, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);
      drawrect(col, END, 0.0, 0.0, 0.0, 0.0, 0);
      drawline(col, END, 0.0, 0.0, 0.0, 0.0, 0);
     }
   }
 }
 draw_window = save_draw;
}

/* show == 0   ==> create pins from highlight nets */
/* show == 1   ==> print list of highlight net */
/* show == 2   ==> create labels with i prefix from hilight nets */
/* show == 3   ==> print list of highlight net with path and label expansion  */
/* show == 4   ==> create labels without i prefix from hilight nets */
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
 char *filetmp1 = NULL;
 char *filetmp2 = NULL;
 char *filename_ptr;

 prepare_netlist_structs(1); /* use full prepare_netlist_structs(1)  to recognize pin direction */
                             /* when creating pins from hilight nets 20171221 */
 if(!(fd = open_tmpfile("hilight2_", &filename_ptr)) ) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   return;
 }
 my_strdup(147, &filetmp2, filename_ptr);
 fclose(fd);
 if(!(fd = open_tmpfile("hilight1_", &filename_ptr))) {
   fprintf(errfp, "print_hilight_net(): can not create tmpfile %s\n", filename_ptr);
   my_free(776, &filetmp2);
   return;
 }
 my_strdup(148, &filetmp1, filename_ptr);
 my_snprintf(cmd, S(cmd), "awk -f \"%s/order_labels.awk\"", tclgetvar("XSCHEM_SHAREDIR"));
 my_snprintf(cmd2, S(cmd2), "%s %s > %s", cmd, filetmp1, filetmp2);
 my_snprintf(cmd3, S(cmd3), "awk -f \"%s/sort_labels.awk\" %s", tclgetvar("XSCHEM_SHAREDIR"), filetmp1);
 for(i=0;i<HASHSIZE;i++) {
   entry=xctx->hilight_table[i];
   while(entry) {
     dbg(1, "print_hilight_net(): (hilight_hashentry *)entry->token=%s\n", entry->token);
     node_entry = bus_hash_lookup(entry->token, "", XLOOKUP, 0, "", "", "", "");
     /* 20170926 test for not null node_entry, this may happen if a hilighted net name has been changed */
     /* before invoking this function, in this case --> skip */
     if(node_entry && !strcmp(xctx->sch_path[xctx->currsch], entry->path)) {
       if(show==3) {
         if(netlist_type == CAD_SPICE_NETLIST) 
           fprintf(fd, ".save v(%s%s)\n", 
              entry->path + 1, 
              entry->token[0] == '#' ? entry->token + 1 : entry->token  );
         else
           fprintf(fd, "%s%s\n", 
              entry->path + 1, 
              entry->token[0] == '#' ? entry->token + 1 : entry->token  );
       } else if(show==1) {
         fprintf(fd, "%s\n",  entry->token);
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
 if(show != 3) {
   tclsetvar("filetmp",filetmp2);
   if(system(cmd2)==-1) { /* order_labels.awk filetmp1 > filetmp2 */
     fprintf(errfp, "print_hilight_net(): error executing cmd2\n");
   }
   if(show==2) { /* create labels from hilight pins with 'i' prefix */
     tcleval(b); /* add_lab_prefix */
   }
   if(show==4) { /* create labels from hilight pins without 'i' prefix */
     tcleval(b1); /* add_lab_no_prefix */
   }
   if(show==1) {
     my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp2);
     tcleval(cmd);
     tcleval("viewdata $::retval");
   }
 } else { /* show == 3 */
   tclsetvar("filetmp",filetmp1);
   if(system(cmd3)==-1) {  /* sort_labels.awk filetmp1 (writes changes into filetmp1) */
     fprintf(errfp, "print_hilight_net(): error executing cmd3\n");
   }
   my_snprintf(cmd, S(cmd), "set ::retval [ read_data_nonewline %s ]", filetmp1);
   tcleval(cmd);
   tcleval("viewdata $::retval");
 }
 if(show==0)  {
   tcleval(a); /* create_pins */
 }
 if(debug_var == 0 ) {
   xunlink(filetmp2);
   xunlink(filetmp1);
 }
 /* 20170323 this delete_netlist_structs is necessary, without it segfaults when going back (ctrl-e)  */
 /* from a schematic after placing pins (ctrl-j) and changing some pin direction (ipin -->iopin) */
 xctx->prep_hi_structs=0;
 xctx->prep_net_structs=0;

 my_free(781, &filetmp1);
 my_free(782, &filetmp2);
}

