/* File: move.c
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
static double rx1, rx2, ry1, ry2;
static int rot = 0;
static int  flip = 0;
static double x1,y_1,x2,y_2,deltax = 0.0, deltay = 0.0;
static int i,c,n,k;
static int lastsel;


void rebuild_selected_array() // can be used only if new selected set is lower
			      // that is, selectedgroup[] size can not increase
{
 int i,c;

 if(!need_rebuild_selected_array) return;
 lastselected=0;
 for(i=0;i<lasttext;i++)
  if(textelement[i].sel) 
  {
   check_selected_storage();
   selectedgroup[lastselected].type = TEXT;
   selectedgroup[lastselected].n = i;
   selectedgroup[lastselected++].col = TEXTLAYER;
  }
 for(i=0;i<lastinst;i++)
  if(inst_ptr[i].sel) 
  {
   check_selected_storage();
   selectedgroup[lastselected].type = ELEMENT;
   selectedgroup[lastselected].n = i;
   selectedgroup[lastselected++].col = WIRELAYER;
  }
 for(i=0;i<lastwire;i++)
  if(wire[i].sel) 
  {
   check_selected_storage();
   selectedgroup[lastselected].type = WIRE;
   selectedgroup[lastselected].n = i;
   selectedgroup[lastselected++].col = WIRELAYER;
  }
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastrect[c];i++)
   if(rect[c][i].sel) 
   {
    check_selected_storage();
    selectedgroup[lastselected].type = RECT;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
  for(i=0;i<lastline[c];i++)
   if(line[c][i].sel) 
   {
    check_selected_storage();
    selectedgroup[lastselected].type = LINE;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
 }
 need_rebuild_selected_array=0;
}

void check_collapsing_objects()
{
  int  j,i;
  int found=0;

  j=0;
  for(i=0;i<lastwire;i++)
  {
   if(wire[i].x1==wire[i].x2 && wire[i].y1 == wire[i].y2)
   {
    found=1;
    j++; 
    continue;
   }
   if(j) 
   {
    my_strdup(&wire[i-j].prop_ptr, NULL);
    my_strdup(&wire[i-j].node, NULL);
    wire[i-j] = wire[i];
    wire[i].prop_ptr=NULL;
    wire[i].node=NULL;
   }
  }
  lastwire -= j; 

 // option: remove degenerated lines 
   for(c=0;c<cadlayers;c++)
   {
    j = 0;
    for(i=0;i<lastline[c];i++)
    {
     if(line[c][i].x1==line[c][i].x2 && line[c][i].y1 == line[c][i].y2)
     {
      found=1;
      j++;
      continue;
     }
     if(j) 
     {
      my_strdup(&line[c][i-j].prop_ptr, NULL);
      line[c][i-j] = line[c][i];
      line[c][i].prop_ptr=NULL;
     }
    }
    lastline[c] -= j;
   } 
   for(c=0;c<cadlayers;c++)
   { 
    j = 0;
    for(i=0;i<lastrect[c];i++)
    {
     if(rect[c][i].x1==rect[c][i].x2 || rect[c][i].y1 == rect[c][i].y2)
     {
      found=1;
      j++;
      continue;
     }
     if(j) 
     {
      my_strdup(&rect[c][i-j].prop_ptr, NULL);
      rect[c][i-j] = rect[c][i];
      rect[c][i].prop_ptr=NULL;
     }
    }
    lastrect[c] -= j;
   }  

  if(found) rebuild_selected_array();
}


void draw_selection(GC g, int interruptable)
{
  drawtempline(g, BEGIN, 0.0, 0.0, 0.0, 0.0);
  drawtemprect(g, BEGIN, 0.0, 0.0, 0.0, 0.0);

  if(g == gc[SELLAYER]) lastsel = lastselected;
  for(i=0;i<lastsel;i++)
  {
   c = selectedgroup[i].col;n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case TEXT:
     ROTATION(x1, y_1, textelement[n].x0, textelement[n].y0, rx1,ry1);
     draw_temp_string(g,ADD, textelement[n].txt_ptr,
      (textelement[n].rot +
      ( (flip && (textelement[n].rot & 1) ) ? rot+2 : rot) ) & 0x3,
       textelement[n].flip^flip,
       rx1+deltax, ry1+deltay,
       textelement[n].xscale, textelement[n].yscale);
     break;
    case RECT:
     ROTATION(x1, y_1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
     ROTATION(x1, y_1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
     if(rect[c][n].sel==SELECTED)
     {
       RECTORDER(rx1,ry1,rx2,ry2);
       drawtemprect(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(rect[c][n].sel==SELECTED1)
     {
      rx1+=deltax;
      ry1+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==SELECTED2)
     {
      rx2+=deltax;
      ry1+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==SELECTED3)
     {
      rx1+=deltax;
      ry2+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==SELECTED4)
     {
      rx2+=deltax;
      ry2+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==(SELECTED1|SELECTED2))
     {
      ry1+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==(SELECTED3|SELECTED4))
     {
      ry2+=deltay;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==(SELECTED1|SELECTED3))
     {
      rx1+=deltax;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     else if(rect[c][n].sel==(SELECTED2|SELECTED4))
     {
      rx2+=deltax;
      RECTORDER(rx1,ry1,rx2,ry2);
      drawtemprect(g, ADD, rx1, ry1, rx2, ry2);
     }
     break;
    case WIRE:
     ROTATION(x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);
     ROTATION(x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);
     ORDER(rx1,ry1,rx2,ry2);
     if(wire[n].sel==SELECTED)
     {
      if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
        drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      else
        drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(wire[n].sel==SELECTED1)
     {
      if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        //26122004
        drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2, ry2);
      else
        drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2, ry2);
     }
     else if(wire[n].sel==SELECTED2)
     {
      if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
        drawtempline(g, THICK, rx1, ry1, rx2+deltax, ry2+deltay);
      else
        drawtempline(g, ADD, rx1, ry1, rx2+deltax, ry2+deltay);
     }
     break;
    case LINE:
     ROTATION(x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);
     ROTATION(x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);
     ORDER(rx1,ry1,rx2,ry2);
     if(line[c][n].sel==SELECTED)
     {
      drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(line[c][n].sel==SELECTED1)
     {
      drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2, ry2);
     }
     else if(line[c][n].sel==SELECTED2)
     {
      drawtempline(g, ADD, rx1, ry1, rx2+deltax, ry2+deltay);
     }
     break;
    case ELEMENT:
     ROTATION(x1, y_1, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
     for(k=0;k<cadlayers;k++)
      draw_temp_symbol_outline(ADD, g, n, k, flip,
       ( flip && (inst_ptr[n].rot & 1) ) ? rot+2 : rot,
       rx1-inst_ptr[n].x0+deltax,ry1-inst_ptr[n].y0+deltay);
     break;
   }
   drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
   if(XPending(display) && interruptable)
   {
    lastsel = i+1;
    return;
   }
  }
  lastsel = i;
}

void copy_objects(int what)
{
 int i;
 int tmp;
 static char *str = NULL; // 20161122 overflow safe
 double tmpx, tmpy;

 if(what & BEGIN)
 {
  if(debug_var>=1) fprintf(errfp, "copy_objects(): BEGIN copy\n");
  rebuild_selected_array();
  save_selection(1);
  deltax = deltay = 0.0;
  lastsel = lastselected;
  x1=mousex_snap;y_1=mousey_snap;
   flip = 0;rot = 0;
  rubber|=STARTCOPY;
 }
 if(what & ABORT)				// draw objects while moving
 {
  draw_selection(gctiled,0);
  rot=flip=deltax=deltay=0;
  rubber&=~STARTCOPY;
  my_strdup(&str, getenv("HOME"));
  my_strcat(&str, "/.selection.sch");
  unlink(str);

 }
 if(what & RUBBER)				// draw objects while moving
 {
  x2=mousex_snap;y_2=mousey_snap;
  draw_selection(gctiled,0);
  deltax = x2-x1; deltay = y_2 - y_1;
  draw_selection(gc[SELLAYER],1);
 }
 if(what & ROTATE)
 {
  draw_selection(gctiled,0);
  rot= (rot+1) & 0x3;
 } 
 if(what & FLIP)
 {
  draw_selection(gctiled,0);
  flip = !flip;
 }
 if(what & END)					// copy selected objects
 {
  draw_selection(gctiled,0);
  tmp=0;
  modified=1; push_undo(); // 20150327 push_undo

  for(k=0;k<cadlayers;k++)
  {
   drawline(gc[k], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawrect(gc[k], BEGIN, 0.0, 0.0, 0.0, 0.0);
   if(fill) filledrect(gcstipple[k], BEGIN, 0.0, 0.0, 0.0, 0.0);
   for(i=0;i<lastselected;i++)
   {
    c = selectedgroup[i].col;n = selectedgroup[i].n;
    switch(selectedgroup[i].type)
    {
     case WIRE:
      if(k!=WIRELAYER) break;
      check_wire_storage();
      ROTATION(x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);   
      ROTATION(x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);   

      if( wire[n].sel & (SELECTED|SELECTED1) ) //<<<<<<<<<<
      {
       rx1+=deltax;
       ry1+=deltay;
      }
      if( wire[n].sel & (SELECTED|SELECTED2) )
      {
       rx2+=deltax;
       ry2+=deltay;
      }
      tmpx=rx1; // used as temporary storage
      tmpy=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( tmpx == rx2 &&  tmpy == ry2)
      {
       if(wire[n].sel == SELECTED1) wire[n].sel = SELECTED2;
       else if(wire[n].sel == SELECTED2) wire[n].sel = SELECTED1;
      }
      if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
        drawline(gc[k], THICK, rx1,ry1,rx2,ry2);
      else
        drawline(gc[k], ADD, rx1,ry1,rx2,ry2);
      selectedgroup[i].n=lastwire;
      storeobject(-1, rx1,ry1,rx2,ry2,WIRE,0,wire[n].sel,wire[n].prop_ptr);
      wire[n].sel=0;
      break;
     case LINE:
      if(c!=k) break;
      ROTATION(x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);   
      ROTATION(x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);   
      if( line[c][n].sel & (SELECTED|SELECTED1) ) //<<<<<<<<<<
      {
       rx1+=deltax;
       ry1+=deltay;
      }
      if( line[c][n].sel & (SELECTED|SELECTED2) )
      {
       rx2+=deltax;
       ry2+=deltay;
      }
      tmpx=rx1;
      tmpy=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( tmpx == rx2 &&  tmpy == ry2)
      {
       if(line[c][n].sel == SELECTED1) line[c][n].sel = SELECTED2;
       else if(line[c][n].sel == SELECTED2) line[c][n].sel = SELECTED1;
      }
      drawline(gc[k], ADD, rx1,ry1,rx2,ry2);
      selectedgroup[i].n=lastline[c];
      storeobject(-1, rx1, ry1, rx2, ry2, LINE, c, line[c][n].sel, line[c][n].prop_ptr);
      line[c][n].sel=0;
      break;
     case TEXT:
      if(k!=TEXTLAYER) break;
      check_text_storage();
      ROTATION(x1, y_1, textelement[n].x0, textelement[n].y0, rx1,ry1);   
      textelement[lasttext].txt_ptr=NULL;
      my_strdup(&textelement[lasttext].txt_ptr,textelement[n].txt_ptr);
      textelement[n].sel=0;
       if(debug_var>=2) fprintf(errfp, "copy_objects(): current str=%s\n",
        textelement[lasttext].txt_ptr);
      textelement[lasttext].x0=rx1+deltax;
      textelement[lasttext].y0=ry1+deltay;
      textelement[lasttext].rot=(textelement[n].rot +
       ( (flip && (textelement[n].rot & 1) ) ? rot+2 : rot) ) & 0x3;
      textelement[lasttext].flip=flip^textelement[n].flip;
      textelement[lasttext].sel=SELECTED;
      textelement[lasttext].prop_ptr=NULL;
      my_strdup(&textelement[lasttext].prop_ptr, textelement[n].prop_ptr);
      textelement[lasttext].xscale=textelement[n].xscale;
      textelement[lasttext].yscale=textelement[n].yscale;
      draw_string(gc[k], ADD, textelement[lasttext].txt_ptr,        // draw moved txt
       textelement[lasttext].rot, textelement[lasttext].flip,
       rx1+deltax,ry1+deltay,
       textelement[lasttext].xscale, textelement[lasttext].yscale);
      selectedgroup[i].n=lasttext;
      lasttext++;
       if(debug_var>=2) fprintf(errfp, "copy_objects(): done copy string\n");
      break;
     case RECT:
      if(c!=k) break;
      ROTATION(x1, y_1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);   
      ROTATION(x1, y_1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);   
      RECTORDER(rx1,ry1,rx2,ry2);
      rect[c][n].sel=0;
      drawrect(gc[k], ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      if(fill) filledrect(gcstipple[k], ADD, rx1+deltax, 
               ry1+deltay, rx2+deltax, ry2+deltay);
      selectedgroup[i].n=lastrect[c];
      storeobject(-1, rx1+deltax, ry1+deltay, 
                 rx2+deltax, ry2+deltay,RECT, c, SELECTED, rect[c][n].prop_ptr);
      break;
     case ELEMENT:
      if(k==0)
      {
       check_inst_storage();
       ROTATION(x1, y_1, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
       inst_ptr[lastinst] = inst_ptr[n];
       inst_ptr[lastinst].prop_ptr=NULL;
       inst_ptr[lastinst].instname=NULL; // 20150409
       inst_ptr[lastinst].node=NULL;
       inst_ptr[lastinst].name=NULL;
       my_strdup(&inst_ptr[lastinst].name, inst_ptr[n].name); //<<<<<<
       my_strdup(&inst_ptr[lastinst].prop_ptr, inst_ptr[n].prop_ptr);
       my_strdup2(&inst_ptr[lastinst].instname, get_tok_value(inst_ptr[n].prop_ptr, "name",0)); // 20150409
       inst_ptr[n].sel=0;
       inst_ptr[lastinst].x0 = rx1+deltax;
       inst_ptr[lastinst].y0 = ry1+deltay;
       inst_ptr[lastinst].sel = SELECTED;
       inst_ptr[lastinst].rot = (inst_ptr[lastinst].rot + 
          ( (flip && (inst_ptr[lastinst].rot & 1) ) ? rot+2 : rot) ) & 0x3;
       inst_ptr[lastinst].flip = (flip? !inst_ptr[n].flip:inst_ptr[n].flip);
       new_prop_string(&inst_ptr[lastinst].prop_ptr, inst_ptr[n].prop_ptr,tmp++);
       my_strdup2(&inst_ptr[lastinst].instname, get_tok_value(inst_ptr[lastinst].prop_ptr, "name", 0)); // 20150409
       // the final tmp argument is zero for the 1st call and used in 
       // new_prop_string() for cleaning some internal caches.
       hash_proplist(inst_ptr[lastinst].prop_ptr , 0);
       n=selectedgroup[i].n=lastinst;
       symbol_bbox(lastinst, &inst_ptr[lastinst].x1, &inst_ptr[lastinst].y1,
                         &inst_ptr[lastinst].x2, &inst_ptr[lastinst].y2);
       lastinst++;
      }
      draw_symbol_outline(ADD,gc[k], gcstipple[k], n,k, 0, 0, 0.0, 0.0);
      break;
    }
   }
   drawline(gc[k], END, 0.0, 0.0, 0.0, 0.0);
   drawrect(gc[k], END, 0.0, 0.0, 0.0, 0.0);
   if(fill) filledrect(gcstipple[k], END, 0.0, 0.0, 0.0, 0.0);
  } // end for(k ...
  rubber &= ~STARTCOPY;
  check_collapsing_objects();
  rot=flip=deltax=deltay=0;
  need_rebuild_selected_array=1;
 }
 draw_selection(gc[SELLAYER], 0);
}


void move_objects(int what, int merge, double dx, double dy)
{
 int k;
 double tx1,ty1; // temporaries for swapping coordinates 20070302

 if(what & BEGIN)
 {
  deltax = deltay = 0.0;
  rebuild_selected_array();
  lastsel = lastselected;
   if(merge) x1=y_1=0.0;
   else {x1=mousex_snap;y_1=mousey_snap;}
   flip = 0;rot = 0;
  rubber|=STARTMOVE;
 }
 if(what & ABORT)				// draw objects while moving
 {
  draw_selection(gctiled,0);
  rot=flip=deltax=deltay=0;
  rubber&=~STARTMOVE;
 }
 if(what & RUBBER)				// abort operation
 {
  x2=mousex_snap;y_2=mousey_snap;
  draw_selection(gctiled,0);
  deltax = x2-x1; deltay = y_2 - y_1;
  draw_selection(gc[SELLAYER],1);
 }
 if(what & ROTATE)
 {
  draw_selection(gctiled,0);
  rot= (rot+1) & 0x3;
 } 
 if(what & FLIP)
 {
  draw_selection(gctiled,0);
  flip = !flip;
 }
 if(what & END)					// move selected objects
 {
  draw_selection(gctiled,0);
  bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
  modified=1; 
  if(! ( rubber & STARTMERGE) ) push_undo(); // 20150327 push_undo

  if(dx!=0.0 || dy!=0.0) {
    deltax = dx;
    deltay = dy;
  }
  for(k=0;k<cadlayers;k++)
  {
   drawline(gc[k], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawrect(gc[k], BEGIN, 0.0, 0.0, 0.0, 0.0);
   if(fill) filledrect(gcstipple[k], BEGIN, 0.0, 0.0, 0.0, 0.0); 
   for(i=0;i<lastselected;i++)
   {
    c = selectedgroup[i].col;n = selectedgroup[i].n;
    switch(selectedgroup[i].type)
    {
     case WIRE:
      if(k!=WIRELAYER) break;
      if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])   // 26122004
        bbox(ADD, wire[n].x1-BUS_WIDTH, wire[n].y1-BUS_WIDTH , wire[n].x2+BUS_WIDTH , wire[n].y2+BUS_WIDTH );
      else
        bbox(ADD, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2);
      ROTATION(x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);   
      ROTATION(x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);   
      if( wire[n].sel & (SELECTED|SELECTED1) )
      {
       rx1+=deltax;
       ry1+=deltay;
      }
      if( wire[n].sel & (SELECTED|SELECTED2) )
      {
       rx2+=deltax;
       ry2+=deltay;
      }
      wire[n].x1=rx1;
      wire[n].y1=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( wire[n].x1 == rx2 &&  wire[n].y1 == ry2)
      {
       if(wire[n].sel == SELECTED1) wire[n].sel = SELECTED2;
       else if(wire[n].sel == SELECTED2) wire[n].sel = SELECTED1;
      }
      wire[n].x1=rx1;
      wire[n].y1=ry1;
      wire[n].x2=rx2;
      wire[n].y2=ry2;
      if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
        drawline(gc[k], THICK, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2);
      else
        drawline(gc[k], ADD, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2);
      break;
     case LINE:
      if(c!=k) break;   
      bbox(ADD, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2);
      ROTATION(x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);   
      ROTATION(x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);   
      if( line[c][n].sel & (SELECTED|SELECTED1) )
      {
       rx1+=deltax;
       ry1+=deltay;
      }
      if( line[c][n].sel & (SELECTED|SELECTED2) )
      {
       rx2+=deltax;
       ry2+=deltay;
      }
      line[c][n].x1=rx1;
      line[c][n].y1=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( line[c][n].x1 == rx2 &&  line[c][n].y1 == ry2)
      {
       if(line[c][n].sel == SELECTED1) line[c][n].sel = SELECTED2;
       else if(line[c][n].sel == SELECTED2) line[c][n].sel = SELECTED1;
      }
      line[c][n].x1=rx1;
      line[c][n].y1=ry1;
      line[c][n].x2=rx2;
      line[c][n].y2=ry2;
      drawline(gc[k], ADD, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2);
      break;

     case RECT:
      if(c!=k) break;
      bbox(ADD, rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2);
      ROTATION(x1, y_1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);   
      ROTATION(x1, y_1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);   

      if( rect[c][n].sel == SELECTED) {
        rx1+=deltax;
        ry1+=deltay;
        rx2+=deltax;
        ry2+=deltay;
      }
      else if( rect[c][n].sel == SELECTED1) {	// 20070302 stretching on rectangles
        rx1+=deltax;
        ry1+=deltay;
      }
      else if( rect[c][n].sel == SELECTED2) {
        rx2+=deltax;
        ry1+=deltay;
      }
      else if( rect[c][n].sel == SELECTED3) {
        rx1+=deltax;
        ry2+=deltay;
      }
      else if( rect[c][n].sel == SELECTED4) {
        rx2+=deltax;
        ry2+=deltay;
      }
      else if(rect[c][n].sel==(SELECTED1|SELECTED2))
      {
        ry1+=deltay;
      }
      else if(rect[c][n].sel==(SELECTED3|SELECTED4))
      {
        ry2+=deltay;
      }
      else if(rect[c][n].sel==(SELECTED1|SELECTED3))
      {
        rx1+=deltax;
      } 
      else if(rect[c][n].sel==(SELECTED2|SELECTED4))
      {
        rx2+=deltax;
      }

      tx1 = rx1;
      ty1 = ry1;
      RECTORDER(rx1,ry1,rx2,ry2);
   
      if( rx2 == tx1) {	//20070302
        if(rect[c][n].sel==SELECTED1) rect[c][n].sel = SELECTED2;
        else if(rect[c][n].sel==SELECTED2) rect[c][n].sel = SELECTED1;
        else if(rect[c][n].sel==SELECTED3) rect[c][n].sel = SELECTED4;
        else if(rect[c][n].sel==SELECTED4) rect[c][n].sel = SELECTED3;
      }
      if( ry2 == ty1) {
        if(rect[c][n].sel==SELECTED1) rect[c][n].sel = SELECTED3;
        else if(rect[c][n].sel==SELECTED3) rect[c][n].sel = SELECTED1;
        else if(rect[c][n].sel==SELECTED2) rect[c][n].sel = SELECTED4;
        else if(rect[c][n].sel==SELECTED4) rect[c][n].sel = SELECTED2;
      }
     
      rect[c][n].x1 = rx1;
      rect[c][n].y1 = ry1;
      rect[c][n].x2 = rx2;
      rect[c][n].y2 = ry2;
      drawrect(gc[k], ADD, rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2);
      if(fill) filledrect(gcstipple[c], ADD, rect[c][n].x1, rect[c][n].y1, 
                 rect[c][n].x2, rect[c][n].y2);
      break;

     case TEXT:
      if(k!=TEXTLAYER) break;
      text_bbox(textelement[n].txt_ptr, textelement[n].xscale,
   	     textelement[n].yscale, textelement[n].rot,textelement[n].flip,
                textelement[n].x0, textelement[n].y0,
                &rx1,&ry1, &rx2,&ry2);
      bbox(ADD, rx1, ry1, rx2, ry2 );
 
      ROTATION(x1, y_1, textelement[n].x0, textelement[n].y0, rx1,ry1);   
      textelement[n].x0=rx1+deltax;
      textelement[n].y0=ry1+deltay;
      textelement[n].rot=(textelement[n].rot +
       ( (flip && (textelement[n].rot & 1) ) ? rot+2 : rot) ) & 0x3;
      textelement[n].flip=flip^textelement[n].flip;
 
      draw_string(gc[k], ADD, textelement[n].txt_ptr,        // draw moved txt
       textelement[n].rot, textelement[n].flip,
       textelement[n].x0, textelement[n].y0,
       textelement[n].xscale, textelement[n].yscale);
      break;
 
     case ELEMENT:
      if(k==0)
      {
       bbox(ADD, inst_ptr[n].x1, inst_ptr[n].y1, inst_ptr[n].x2, inst_ptr[n].y2 );
       ROTATION(x1, y_1, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
       inst_ptr[n].x0 = rx1+deltax;
       inst_ptr[n].y0 = ry1+deltay;
       inst_ptr[n].rot = (inst_ptr[n].rot + 
        ( (flip && (inst_ptr[n].rot & 1) ) ? rot+2 : rot) ) & 0x3;
       inst_ptr[n].flip = flip ^ inst_ptr[n].flip;
       symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1,
                         &inst_ptr[n].x2, &inst_ptr[n].y2);
      }
      draw_symbol_outline(ADD,gc[k],gcstipple[k], n,k, 0, 0, 0.0, 0.0);
      break;
    }
   }
   drawline(gc[k], END, 0.0, 0.0, 0.0, 0.0);
   drawrect(gc[k], END, 0.0, 0.0, 0.0, 0.0);
   if(fill) filledrect(gcstipple[k], END, 0.0, 0.0, 0.0, 0.0); 
  } //end for(k ...
  rubber &= ~STARTMOVE;
  rubber &= ~STARTMERGE;
  check_collapsing_objects();
  rot=flip=deltax=deltay=0;
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0); 
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  need_rebuild_selected_array=1;
 }
 draw_selection(gc[SELLAYER], 0);
}

