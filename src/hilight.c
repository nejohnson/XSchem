/* File: hilight.c
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

static struct hilight_hashentry *table[HASHSIZE];
static int nelements=0; // 20161221

static unsigned int hash(char *tok)
{
 register unsigned int h=0;
 register char *str;
 str=sch_prefix[currentsch];
 // while(*tok) h=(h<<1)+*tok++;
 // while(*str) h=(h<<1)+*str++;
 while(*str) { // 20161221
   h^=*str++; // xor
   h=(h>>5) | (h<<(8*sizeof(unsigned int)-5)); // 20161221 rotate

 }
 while(*tok) { // 20161221
   h^=*tok++; // xor
   h=(h>>5) | (h<<(8*sizeof(unsigned int)-5)); // 20161221 rotate
 }
 return h;
}

static struct hilight_hashentry *free_hilight_entry(struct hilight_hashentry *entry)
{
 if(entry)
 {
  entry->next = free_hilight_entry( entry->next );
    my_free(entry->token);
    my_free(entry->path);
  my_free(entry);
 }
 return NULL;
}

void free_hilight_hash(void) // remove the whole hash table 
{
 int i;

  if(debug_var>=2) fprintf(errfp, "free_hilight_hash(): removing hash table\n");
 for(i=0;i<HASHSIZE;i++)
 {
  table[i] = free_hilight_entry( table[i] );
 }
 if(debug_var>=2) fprintf(errfp, "free_hilight_hash(): : nelements=%d\n", nelements);
 nelements=0; // 20161221
}


struct hilight_hashentry *hilight_lookup(char *token, int value, int remove)
//    token           remove    ... what ...
// --------------------------------------------------------------------------
// "whatever"         0       insert in hash table if not in and return NULL
//                                      if already present just return entry address 
//                                      return NULL otherwise
//
// "whatever"         1       delete entry if found return NULL
// "whatever"         2       only look up element, dont insert
{
 unsigned int hashcode, index;
 struct hilight_hashentry *entry, *saveptr, **preventry;
 char *ptr;
 int s ;
 int depth=0; // 20161221

 if(token==NULL) return NULL;
 hashcode=hash(token);
 index=hashcode % HASHSIZE;
 entry=table[index];
 preventry=&table[index];
 depth=0; // 20161221
 while(1)
 {
  if( !entry )                  // empty slot
  {
   if( (remove==0) )            // insert data
   {
    s=sizeof( struct hilight_hashentry );
    ptr= my_malloc(s ); 
    entry=(struct hilight_hashentry *)ptr;
    entry->next = NULL;
    entry->token = NULL;
    my_strdup(&(entry->token),token);
    entry->path = NULL;
    my_strdup(&(entry->path),sch_prefix[currentsch]);
    entry->value=value;
    entry->hash=hashcode;
    *preventry=entry;
    hilight_nets=1; // some nets should be hilighted ....  07122002
    nelements++; // 20161221
   }
   return NULL; // whether inserted or not return NULL since it was not in
  }
  if( entry -> hash==hashcode && !strcmp(token,entry->token) &&
      !strcmp(sch_prefix[currentsch], entry->path)  ) // found matching tok
  {
   if(remove==1)                // remove token from the hash table ...
   {
    saveptr=entry->next;
    my_free(entry->token);
    my_free(entry->path);
    my_free(entry);
    *preventry=saveptr;
    return NULL;
   }
   else // found matching entry, return the address
   {
    return entry;
   }
  }
  preventry=&entry->next; // descend into the list.
  entry = entry->next;
  depth++; // 20161221
  if(debug_var>=2) 
    if(depth>200) 
      fprintf(errfp, "hilight_lookup(): deep into the list: %d, index=%d, token=%s, hashcode=%d\n", 
              depth, index, token, hashcode);
 }
}

// warning, in case of buses return only pointer to first bus element
struct hilight_hashentry *bus_hilight_lookup(char *token, int value, int remove)
{
 char *start, *string_ptr, c;
 static char *string=NULL;
 struct hilight_hashentry *ptr1=NULL, *ptr2=NULL;
 int mult;

 if(token==NULL) return NULL;
 if( token[0] == '#') {
   my_strdup(&string, token);
 }
 else {
   my_strdup(&string, expandlabel(token,&mult));
 }

 if(string==NULL) return NULL;
 string_ptr = start = string;
 while(1) {
  c=(*string_ptr);
  if(c==','|| c=='\0')
  {
    *string_ptr='\0';  // set end string at comma position....
    // insert one bus element at a time in hash table
    if(debug_var>=2) fprintf(errfp, "bus_hilight_lookup: inserting: %s, value:%d\n", start,value);
    ptr1=hilight_lookup(start, value, remove);
    if(ptr1 && !ptr2) {
      ptr2=ptr1; //return first non null entry
      if(remove==2) break; // 20161221 no need to go any further if only looking up element
    }
    *string_ptr=c;     // ....restore original char
    start=string_ptr+1;
  }
  if(c==0) break;
  string_ptr++;
 }
 // if something found return first pointer
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
 hilight_color=0;
}

void hilight_parent_pins(void)
{
 int rects, i,j;
 char *pin_node;
 int save_currentsch;
 struct hilight_hashentry *entry;
 
 if(!hilight_nets) return;
 prepare_netlist_structs();
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
 //delete_netlist_structs(); // 20161222 done in prepare_netlist_structs() when needed

}

void hilight_child_pins(int i)
{
 int j,rects;
 char *pin_node;
 struct hilight_hashentry *entry;
 int save_currentsch;;

 if(!hilight_nets) return;
 save_currentsch=currentsch;
 prepare_netlist_structs();
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
 //delete_netlist_structs(); // 20161222 done in prepare_netlist_structs() when needed
}


int bus_search(char*s) 
{ 
 int c, bus=0;
 while( (c=*s++) ) {
   if(c=='[')  bus=1;
   if( (c==':') || (c==',') ) {bus=0; break;}
 }
 return bus;
}

void search_inst(char *tok, char *val, int sub, int sel, int what)
{
 int i,c, col,tmp,bus=0;
 char *str;
 char empty_string[] = "";
 static char *tmpname=NULL;
 regex_t re;

 if(regcomp(&re, val , REG_EXTENDED)) return;
 if(debug_var>=1) fprintf(errfp, "search_inst():val=%s\n", val);
 if(sel) {
   if(x_initialized) drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   if(x_initialized) drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
 }
 if(what==ADD || what==NOW) {
   
    col=hilight_color;
    if(incr_hilight) hilight_color++;

    prepare_netlist_structs();
    bus=bus_search(val);
    for(i=0;i<lastinst;i++) {
      if(!strcmp(tok,"cell__name"))  str = inst_ptr[i].name;
      // 20111024 key 'celltype' to search for instances of symbols with given 'type' property
      else if(!strncmp(tok,"cell__", 6)) {
        my_strdup(&tmpname,get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,tok+6,0)); // flexible cell__ search 20140408
        if(tmpname) {
          str = tmpname;
        } else {
          str = empty_string;
        }
      }
      else str = get_tok_value(inst_ptr[i].prop_ptr, tok,0);
      if(debug_var>=1) fprintf(errfp, "search_inst(): inst=%d, tok=%s, val=%s \n", i,tok, str);
     
      if(bus && sub) {
       if(debug_var>=1) fprintf(errfp, "search_inst(): doing substr search on bus sig:%s inst=%d tok=%s val=%s\n", str,i,tok,val);
       str=expandlabel(str,&tmp);
      }
      //if( (!strcmp(str,val)  && !sub) || 
      if( (!regexec(&re, str,0 , NULL, 0) && !sub) || 		// 20071120 regex instead of strcmp
          (strstr(str,val) && sub) ) 
      {
        str = get_tok_value(inst_ptr[i].prop_ptr, "lab",0);
        if(str && str[0]) {
           bus_hilight_lookup(str, col,0);
           if(what==NOW) for(c=0;c<cadlayers;c++)
             draw_symbol_outline(NOW,gc[col%(cadlayers-7)+7], gcstipple[col%(cadlayers-7)+7], i,c,0,0,0.0,0.0);
        }
        else {
          if(debug_var>=1) fprintf(errfp, "search_inst(): setting hilight flag on inst %d\n",i);
          hilight_nets=1;
          inst_ptr[i].flags |= 4;
          if(what==NOW) for(c=0;c<cadlayers;c++)
            // draw_symbol_outline(NOW, gc[PINLAYER], gcstipple[PINLAYER], i,c,0,0,0.0,0.0);
            draw_symbol_outline(NOW,gc[col%(cadlayers-7)+7], gcstipple[col%(cadlayers-7)+7], i,c,0,0,0.0,0.0);  // 20150804

        }


        if(sel) {
          select_element(i, SELECTED, 1);
          rubber|=SELECTION;
        }
      }
      
    }
    for(i=0;i<lastwire;i++) {
      str = get_tok_value(wire[i].prop_ptr, tok,0);
      //if(   (!strcmp(str, val) && !sub )  ||
      if(   (!regexec(&re, str,0 , NULL, 0) && !sub )  || 	// 20071120 regex instead of strcmp
            ( strstr(str, val) &&  sub )
        ) {
          str = get_tok_value(wire[i].prop_ptr, "lab",0);
          if(debug_var>=2) fprintf(errfp, "search_inst(): wire=%d, tok=%s, val=%s \n", i,tok, str);
          if(str && str[0]) {
             bus_hilight_lookup(str, col,0);
             if(what==NOW) drawline(gc[col%(cadlayers-7)+7], NOW, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
          }
          if(sel) {
            select_wire(i,SELECTED);
            rubber|=SELECTION;
          }
      }
      else {
        if(debug_var>=2) fprintf(errfp, "search_inst():  not found wire=%d, tok=%s, val=%s search=%s\n", i,tok, str,val);
      }
    }
    // delete_netlist_structs(); // 20161222
 }
 if(sel) {
   if(x_initialized) drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
   if(x_initialized) drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
 }
 else if(what==END) draw_hilight_net();

 regfree(&re);
}




void hilight_net(void)
{
  int i,n;
  char *str;

  prepare_netlist_structs();
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
      //str = wire[n].node;
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
      //str = wire[n].node;
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
  // delete_netlist_structs(); // 20161222 done in prepare_netlist_structs() when needed
  // unselect_all(); // 20160413
}


void unhilight_net(void)
{
  int i,n;
  char *str;


  prepare_netlist_structs();
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
      //str = wire[n].node;
      if(str && str[0])
      {
       bus_hilight_lookup(str, hilight_color,1);
       // if(incr_hilight) hilight_color++; // 20160414

      }
     }
     break;

    case ELEMENT:
     if(inst_ptr[n].sel==SELECTED)
     {
      str = get_tok_value(inst_ptr[n].prop_ptr, "lab",0);
      //str = wire[n].node;
      inst_ptr[n].flags &= ~4;
      if(str && str[0])
      {
       bus_hilight_lookup(str, hilight_color,1);
       // if(incr_hilight) hilight_color++; // 20160414
      }
     }
     break;


    default:
     break;
   }
  }
  unselect_all();
  // delete_netlist_structs();  // 20111201 moved to end // 20161222 done in prepare_netlist_structs() when needed
}


void draw_hilight_net(void)
{
 char *str;
 static int *inst_color=NULL;
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2; // 20150409
 Instdef *symptr; // 20160414

  if(!hilight_nets) return;
  for(i=0;i<lastwire;i++)
  {
    // 20150409
    x1=(wire[i].x1-xorigin)*mooz;
    x2=(wire[i].x2-xorigin)*mooz;
    y1=(wire[i].y1-yorigin)*mooz;
    y2=(wire[i].y2-yorigin)*mooz;
    if( OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
    // /20150409

    str = get_tok_value(wire[i].prop_ptr, "lab",0);
    //str = wire[i].node;
    if(str && str[0]) {
      if( (entry = bus_hilight_lookup(str, 0,2)) ) {
        if(get_tok_value(wire[i].prop_ptr,"bus",0)[0])   // 26122004
          drawline(gc[7+entry->value%(cadlayers-7)], THICK, 
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
        else
          drawline(gc[7+entry->value%(cadlayers-7)], NOW, 
             wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
      }
    }

  }

 my_realloc(&inst_color,lastinst*sizeof(int)); 
 for(i=0;i<lastinst;i++)
 {
   // 20150409
   x1=(inst_ptr[i].x1-xorigin)*mooz;
   x2=(inst_ptr[i].x2-xorigin)*mooz;
   y1=(inst_ptr[i].y1-yorigin)*mooz;
   y2=(inst_ptr[i].y2-yorigin)*mooz;
   inst_color[i]=0;
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
   // /20150409

  // type = get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"type",0); // 20150409
  type = (inst_ptr[i].ptr+instdef)->type; // 20150409
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
  // 20160414 from draw()
  if(draw_single_layer!=-1 && c != draw_single_layer) continue; // 20151117

  for(i=0;i<lastinst;i++)
  {
    if(inst_color[i] )
    {
     // 20150409
     x1=(inst_ptr[i].x1-xorigin)*mooz;
     x2=(inst_ptr[i].x2-xorigin)*mooz;
     y1=(inst_ptr[i].y1-yorigin)*mooz;
     y2=(inst_ptr[i].y2-yorigin)*mooz;
     if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
     if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance:%d\n",i);
     drawline(gc[inst_color[i]], BEGIN, 0.0, 0.0, 0.0, 0.0);
     drawrect(gc[inst_color[i]], BEGIN, 0.0, 0.0, 0.0, 0.0);
     if(fill) filledrect(gcstipple[inst_color[i]], BEGIN, 0.0, 0.0, 0.0, 0.0);
     // 20160414 from draw()
     symptr = (inst_ptr[i].ptr+instdef);
     if( c==0 || //draw_symbol_outline call is needed on layer 0 to avoid redundant work (outside check)
         symptr->lines[c] ||
         symptr->rects[c] ||
         ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
       draw_symbol_outline(ADD, gc[inst_color[i]],gcstipple[inst_color[i]], i,c,0,0,0.0,0.0);
     }
     drawline(gc[inst_color[i]], END, 0.0, 0.0, 0.0, 0.0);
     drawrect(gc[inst_color[i]], END, 0.0, 0.0, 0.0, 0.0);
     if(fill) filledrect(gcstipple[inst_color[i]], END, 0.0, 0.0, 0.0, 0.0);
    }
  }
 }
}



void undraw_hilight_net(void) // 20160413
{
 char *str;
 static int *inst_color=NULL;
 char *type=NULL;
 int i,c;
 struct hilight_hashentry *entry;
 register double x1,y1,x2,y2; // 20150409
 Instdef *symptr; // 20160414


 for(i=0;i<lastwire;i++)
 {
   // 20150409
   x1=(wire[i].x1-xorigin)*mooz;
   x2=(wire[i].x2-xorigin)*mooz;
   y1=(wire[i].y1-yorigin)*mooz;
   y2=(wire[i].y2-yorigin)*mooz;
   if( OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
   // /20150409

   str = get_tok_value(wire[i].prop_ptr, "lab",0);
   //str = wire[i].node;
   if(str && str[0]) {
     if( (!bus_hilight_lookup(str, 0,2)) ) {
       if(get_tok_value(wire[i].prop_ptr,"bus",0)[0])   // 26122004
         drawline(gc[WIRELAYER], THICK, 
            wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
       else
         drawline(gc[WIRELAYER], NOW, 
            wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
     }
   }

 }
 my_realloc(&inst_color,lastinst*sizeof(int)); 
 for(i=0;i<lastinst;i++)
 {
   // 20150409
   x1=(inst_ptr[i].x1-xorigin)*mooz;
   x2=(inst_ptr[i].x2-xorigin)*mooz;
   y1=(inst_ptr[i].y1-yorigin)*mooz;
   y2=(inst_ptr[i].y2-yorigin)*mooz;
   inst_color[i]=0;
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) continue;
   // /20150409

  // type = get_tok_value((inst_ptr[i].ptr+instdef)->prop_ptr,"type",0); // 20150409
  type = (inst_ptr[i].ptr+instdef)->type; // 20150409
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
  // 20160414 from draw()
  if(draw_single_layer!=-1 && c != draw_single_layer) continue; // 20151117
  drawline(gc[c], BEGIN, 0.0, 0.0, 0.0, 0.0);
  drawrect(gc[c], BEGIN, 0.0, 0.0, 0.0, 0.0);
  if(fill) filledrect(gcstipple[c], BEGIN, 0.0, 0.0, 0.0, 0.0);
  for(i=0;i<lastinst;i++)
  {
    if(!inst_color[i] )
    {
     if(debug_var>=1) fprintf(errfp, "draw_hilight_net(): instance:%d\n",i);
     // 20160414 from draw()
     symptr = (inst_ptr[i].ptr+instdef);
     if( c==0 || //draw_symbol_outline call is needed on layer 0 to avoid redundant work (outside check)
         symptr->lines[c] ||
         symptr->rects[c] ||
         ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {
       draw_symbol_outline(ADD, gc[c],gcstipple[c], i,c,0,0,0.0,0.0);
     }
    }
  }
  drawline(gc[c], END, 0.0, 0.0, 0.0, 0.0);
  drawrect(gc[c], END, 0.0, 0.0, 0.0, 0.0);
  if(fill) filledrect(gcstipple[c], END, 0.0, 0.0, 0.0, 0.0);
 }
}

// show == 0   ==> create pins from highlight nets
void print_hilight_net(int show)
{
 int i;
 FILE *fd;
 struct hilight_hashentry *entry;
 struct node_hashentry *node_entry;
 static char *cmd = NULL;  // 20161122 overflow safe
 static char *cmd2 = NULL;  // 20161122 overflow safe
 static char *cmd3 = NULL;  // 20161122 overflow safe
 char a[] = "create_pins";
 char b[] = "add_lab_prefix";
 char b1[] = "add_lab_no_prefix";
 char filetmp1[] = "tmp1XXXXXX";
 char filetmp2[] = "tmp2XXXXXX";


 // 20111116 20111201
 prepare_netlist_structs();

// 20111106
 mkstemp(filetmp1);
 mkstemp(filetmp2);

 if(show == 3) {
   Tcl_SetVar(interp,"filetmp2",filetmp1,TCL_GLOBAL_ONLY);
 } else {
   Tcl_SetVar(interp,"filetmp2",filetmp2,TCL_GLOBAL_ONLY);
 }
 Tcl_SetVar(interp,"filetmp1",filetmp1,TCL_GLOBAL_ONLY);

 if(  filetmp1[0] == 0 || filetmp2[0] == 0 ) {
   if(debug_var>=1) fprintf(errfp, "print_hilight_net(): problems creating tmpfiles\n");
   return;
 }
 // /20111106
 my_strdup(&cmd, Tcl_GetVar(interp,"XSCHEM_HOME_DIR", TCL_GLOBAL_ONLY));
 my_strcat(&cmd, "/order_labels.awk");
 my_strdup(&cmd2, cmd);
 my_strcat(&cmd2," ");
 my_strcat(&cmd2,filetmp1);
 my_strcat(&cmd2,">");
 my_strcat(&cmd2,filetmp2);

 // 20111106
 my_strdup(&cmd3, Tcl_GetVar(interp,"XSCHEM_HOME_DIR", TCL_GLOBAL_ONLY));
 my_strcat(&cmd3, "/sort_labels.awk ");
 my_strcat(&cmd3, filetmp1);

 fd=fopen(filetmp1, "w");
 if(fd==NULL){ 
    if(debug_var>=1) fprintf(errfp, "print_hilight_net(): problems opening netlist file\n");
    return;
 }

 for(i=0;i<HASHSIZE;i++) {
   entry=table[i];
   while(entry) {
     // 20111116
     node_entry = bus_hash_lookup(entry->token, "",2, 0, "", "", "", "");

 
     if(!strcmp(sch_prefix[currentsch], entry->path)) {
       if(show==3) {
         fprintf(fd, "%s%s\n",  entry->path, entry->token); // 20111106

       } else if(show==1) { // 20120926
         fprintf(fd, "%s\n",  entry->token); // 20120926
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
   tkeval(b);
 }
 if(show==4) { // 20120913 create labels from hilight pins without 'i' prefix
   tkeval(b1);
 }
 if(show==1) {
   my_strdup(&cmd, "set ::entry1 [ read_data_nonewline ");
   my_strcat(&cmd, filetmp2);
   my_strcat(&cmd, " ]");
   tkeval(cmd);
   tkeval("viewdata $::entry1");
 }
 if(show==3) {
   system(cmd3);
   my_strdup(&cmd, "set ::entry1 [ read_data_nonewline ");
   my_strcat(&cmd, filetmp1);
   my_strcat(&cmd, " ]");
   tkeval(cmd);
   tkeval("viewdata $::entry1");
 }
 if(show==0)  {
   tkeval(a);
 }
 unlink(filetmp2);
 unlink(filetmp1);
 //delete_netlist_structs(); // 20161222 done in prepare_netlist_structs() when needed


}

