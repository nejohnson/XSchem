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
static double x1=0.0, y_1=0.0, x2=0.0, y_2=0.0, deltax = 0.0, deltay = 0.0;
// static int i,c,n,k;
static int lastsel;
static int rotatelocal=0; // 20171208


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
  for(i=0;i<lastarc[c];i++)
   if(arc[c][i].sel) 
   {
    check_selected_storage();
    selectedgroup[lastselected].type = ARC;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
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
  for(i=0;i<lastpolygon[c];i++) // 20171115
   if(polygon[c][i].sel) 
   {
    check_selected_storage();
    selectedgroup[lastselected].type = POLYGON;
    selectedgroup[lastselected].n = i;
    selectedgroup[lastselected++].col = c;
   }
 }
 need_rebuild_selected_array=0;
}

void check_collapsing_objects()
{
  int  j,i, c;
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

  if(found) {
    need_rebuild_selected_array=1;
    rebuild_selected_array();
  }
}


void draw_selection(GC g, int interruptable)
{
  int i, c, k, n;
  double  angle; // arc
  drawtemparc(g, BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
  drawtempline(g, BEGIN, 0.0, 0.0, 0.0, 0.0);
  drawtemprect(g, BEGIN, 0.0, 0.0, 0.0, 0.0);
  #ifdef HAS_CAIRO
  int customfont;
  #endif

  if(g == gc[SELLAYER]) lastsel = lastselected;
  for(i=0;i<lastsel;i++)
  {
   c = selectedgroup[i].col;n = selectedgroup[i].n;
   switch(selectedgroup[i].type)
   {
    case TEXT:
     if(rotatelocal) {
       ROTATION(textelement[n].x0, textelement[n].y0, textelement[n].x0, textelement[n].y0, rx1,ry1);
     } else {
       ROTATION(x1, y_1, textelement[n].x0, textelement[n].y0, rx1,ry1);
     }
     #ifdef HAS_CAIRO
     customfont =  set_text_custom_font(&textelement[n]);
     #endif
     draw_temp_string(g,ADD, textelement[n].txt_ptr,
      (textelement[n].rot +
      ( (flip && (textelement[n].rot & 1) ) ? rot+2 : rot) ) & 0x3,
       textelement[n].flip^flip,
       rx1+deltax, ry1+deltay,
       textelement[n].xscale, textelement[n].yscale);
     #ifdef HAS_CAIRO
     if(customfont) cairo_restore(ctx);
     #endif

     break;
    case RECT:
     if(rotatelocal) {
       ROTATION(rect[c][n].x1, rect[c][n].y1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
       ROTATION(rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
     } else {
       ROTATION(x1, y_1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
       ROTATION(x1, y_1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
     }
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
    case POLYGON: // 20171115
     {
      double x[polygon[c][n].points];
      double y[polygon[c][n].points];
      if(polygon[c][n].sel==SELECTED || polygon[c][n].sel==SELECTED1) {
        for(k=0;k<polygon[c][n].points; k++) {
          if( polygon[c][n].sel==SELECTED || polygon[c][n].selected_point[k]) {
            if(rotatelocal) {
              ROTATION(polygon[c][n].x[0], polygon[c][n].y[0], polygon[c][n].x[k], polygon[c][n].y[k], rx1,ry1);
            } else {
              ROTATION(x1, y_1, polygon[c][n].x[k], polygon[c][n].y[k], rx1,ry1);
            }
            x[k] = rx1 + deltax;
            y[k] = ry1 + deltay;
          } else {
            x[k] = polygon[c][n].x[k];
            y[k] = polygon[c][n].y[k];
          }
        }
        drawtemppolygon(g, NOW, x, y, polygon[c][n].points);
      }
     }
     break;

    case WIRE:
     if(rotatelocal) {
       ROTATION(wire[n].x1, wire[n].y1, wire[n].x1, wire[n].y1, rx1,ry1);
       ROTATION(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, rx2,ry2);
     } else {
       ROTATION(x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);
       ROTATION(x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);
     }
  
     ORDER(rx1,ry1,rx2,ry2);
     if(wire[n].sel==SELECTED)
     {
      //if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
      if(wire[n].bus) // 20171201
        drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      else
        drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
     }
     else if(wire[n].sel==SELECTED1)
     {
      //if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        //26122004
      if(wire[n].bus) // 20171201
        drawtempline(g, THICK, rx1+deltax, ry1+deltay, rx2, ry2);
      else
        drawtempline(g, ADD, rx1+deltax, ry1+deltay, rx2, ry2);
     }
     else if(wire[n].sel==SELECTED2)
     {
      //if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
      if(wire[n].bus) // 20171201
        drawtempline(g, THICK, rx1, ry1, rx2+deltax, ry2+deltay);
      else
        drawtempline(g, ADD, rx1, ry1, rx2+deltax, ry2+deltay);
     }
     break;
    case LINE:
     if(rotatelocal) {
       ROTATION(line[c][n].x1, line[c][n].y1, line[c][n].x1, line[c][n].y1, rx1,ry1);
       ROTATION(line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2, rx2,ry2);
     } else {
       ROTATION(x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);
       ROTATION(x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);
     }
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
    case ARC:
     if(rotatelocal) {
       // rotate center wrt itself: do nothing
       rx1 = arc[c][n].x; 
       ry1 = arc[c][n].y;
     } else {
       ROTATION(x1, y_1, arc[c][n].x, arc[c][n].y, rx1,ry1);
     }
     angle = arc[c][n].a;
     if(flip) {
       angle = 270.*rot+180.-arc[c][n].b-arc[c][n].a;
     } else {
       angle = arc[c][n].a+rot*270.;
     }
     angle = fmod(angle, 360.);
     if(angle<0.) angle+=360.;
     if(arc[c][n].sel==SELECTED) {
       drawtemparc(g, ADD, rx1+deltax, ry1+deltay, arc[c][n].r, angle, arc[c][n].b);
     } else if(arc[c][n].sel==SELECTED1) {
       drawtemparc(g, ADD, rx1, ry1, fabs(arc[c][n].r+deltax), angle, arc[c][n].b);
     } else if(arc[c][n].sel==SELECTED3) {
       angle = round(fmod(atan2(-deltay, deltax)*180./XSCH_PI+arc[c][n].b, 360.));
       if(angle<0.) angle +=360.;
       if(angle==0) angle=360.;
       drawtemparc(g, ADD, rx1, ry1, arc[c][n].r, arc[c][n].a, angle);
     } else if(arc[c][n].sel==SELECTED2) {
       angle = round(fmod(atan2(-deltay, deltax)*180./XSCH_PI+angle, 360.));
       if(angle<0.) angle +=360.;
       drawtemparc(g, ADD, rx1, ry1, arc[c][n].r, angle, arc[c][n].b);
     }
     break;
    case ELEMENT:
     if(rotatelocal) {
       ROTATION(inst_ptr[n].x0, inst_ptr[n].y0, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
     } else {
       ROTATION(x1, y_1, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
     }
     for(k=0;k<cadlayers;k++)
      draw_temp_symbol_outline(ADD, g, n, k, flip,
       ( flip && (inst_ptr[n].rot & 1) ) ? rot+2 : rot,
       rx1-inst_ptr[n].x0+deltax,ry1-inst_ptr[n].y0+deltay);
     break;
   }
   if(XPending(display) && interruptable)
   {
    drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
    drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
    lastsel = i+1;
    return;
   }
  }
  drawtemparc(g, END, 0.0, 0.0, 0.0, 0.0, 0.0);
  drawtemprect(g, END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(g, END, 0.0, 0.0, 0.0, 0.0);
  lastsel = i;
}

void copy_objects(int what)
{
 int c, i, n, k;
 Box tmp;
 double angle;
 int newpropcnt;
 static char *str = NULL; // 20161122 overflow safe
 double tmpx, tmpy;
 int textlayer;
 char *strlayer;

 // 20171112
 #ifdef HAS_CAIRO
 char *textfont;
 int customfont; // 20181009
 #endif

 if(what & BEGIN)
 {
  rotatelocal=0; //20171208
  if(debug_var>=1) fprintf(errfp, "copy_objects(): BEGIN copy\n");
  rebuild_selected_array();
  save_selection(1);
  deltax = deltay = 0.0;
  lastsel = lastselected;
  x1=mousex_snap;y_1=mousey_snap;
   flip = 0;rot = 0;
  ui_state|=STARTCOPY;
 }
 if(what & ABORT)				// draw objects while moving
 {
  draw_selection(gctiled,0);
  rot=flip=deltax=deltay=0;
  ui_state&=~STARTCOPY;
  my_strdup(&str, home_dir);
  my_strcat(&str, "/.xschem_selection.sch");
  unlink(str);

 }
 if(what & RUBBER)				// draw objects while moving
 {
  x2=mousex_snap;y_2=mousey_snap;
  draw_selection(gctiled,0);
  deltax = x2-x1; deltay = y_2 - y_1;
  draw_selection(gc[SELLAYER],1);
 }
 if(what & ROTATELOCAL ) { //20171208
  rotatelocal=1;
 } 
 if(what & ROTATE) {
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
  int save_draw; // 20181009
  save_draw = draw_window;
  draw_window=1; // temporarily re-enable draw to window together with pixmap
  draw_selection(gctiled,0);
  bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0); // 20181009
  newpropcnt=0;
  modified=1; push_undo(); // 20150327 push_undo
  prepared_hash_instances=0; // 20171224
  prepared_hash_wires=0; // 20171224
  prepared_netlist_structs=0;
  prepared_hilight_structs=0;

  for(k=0;k<cadlayers;k++)
  {
   drawarc(k, BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawline(k, BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawrect(k, BEGIN, 0.0, 0.0, 0.0, 0.0);
   filledrect(k, BEGIN, 0.0, 0.0, 0.0, 0.0);
   for(i=0;i<lastselected;i++)
   {
    c = selectedgroup[i].col;n = selectedgroup[i].n;
    switch(selectedgroup[i].type)
    {
     case WIRE:
      if(k!=WIRELAYER) break;
      check_wire_storage();
      if(wire[n].bus){ // 20171201
        bbox(ADD, wire[n].x1-BUS_WIDTH, wire[n].y1-BUS_WIDTH , wire[n].x2+BUS_WIDTH , wire[n].y2+BUS_WIDTH );
        int ov, y1, y2;
        ov = BUS_WIDTH> CADHALFDOTSIZE ? BUS_WIDTH : CADHALFDOTSIZE;
        if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
        else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
        bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
      } else {
        int ov, y1, y2;
        ov = CADHALFDOTSIZE;
        if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
        else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
        bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
      }
      if(rotatelocal) {
        ROTATION(wire[n].x1, wire[n].y1, wire[n].x1, wire[n].y1, rx1,ry1);
        ROTATION(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, rx2,ry2);
      } else {
        ROTATION(x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);
        ROTATION(x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);
      }
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
      tmpx=rx1; // used as temporary storage
      tmpy=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( tmpx == rx2 &&  tmpy == ry2)
      {
       if(wire[n].sel == SELECTED1) wire[n].sel = SELECTED2;
       else if(wire[n].sel == SELECTED2) wire[n].sel = SELECTED1;
      }
      // if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
      if(wire[n].bus) // 20171201
        drawline(k, THICK, rx1,ry1,rx2,ry2);
      else
        drawline(k, ADD, rx1,ry1,rx2,ry2);
        
      selectedgroup[i].n=lastwire;
      storeobject(-1, rx1,ry1,rx2,ry2,WIRE,0,wire[n].sel,wire[n].prop_ptr);
      wire[n].sel=0;
      break;
     case LINE:
      if(c!=k) break; 
      bbox(ADD, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2); // 20181009
      if(rotatelocal) {
        ROTATION(line[c][n].x1, line[c][n].y1, line[c][n].x1, line[c][n].y1, rx1,ry1);
        ROTATION(line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2, rx2,ry2);
      } else {
        ROTATION(x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);
        ROTATION(x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);
      }
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
      tmpx=rx1;
      tmpy=ry1;
      ORDER(rx1,ry1,rx2,ry2);
      if( tmpx == rx2 &&  tmpy == ry2)
      {
       if(line[c][n].sel == SELECTED1) line[c][n].sel = SELECTED2;
       else if(line[c][n].sel == SELECTED2) line[c][n].sel = SELECTED1;
      }
      drawline(k, ADD, rx1,ry1,rx2,ry2);
      selectedgroup[i].n=lastline[c];
      storeobject(-1, rx1, ry1, rx2, ry2, LINE, c, line[c][n].sel, line[c][n].prop_ptr);
      line[c][n].sel=0;
      break;

     case POLYGON: // 20171115
      if(c!=k) break;
      {
        double bx1, by1, bx2, by2;
        double x[polygon[c][n].points];
        double y[polygon[c][n].points];
        int j;
        for(j=0; j<polygon[c][n].points; j++) {
          if(j==0 || polygon[c][n].x[j] < bx1) bx1 = polygon[c][n].x[j];
          if(j==0 || polygon[c][n].y[j] < by1) by1 = polygon[c][n].y[j];
          if(j==0 || polygon[c][n].x[j] > bx2) bx2 = polygon[c][n].x[j];
          if(j==0 || polygon[c][n].y[j] > by2) by2 = polygon[c][n].y[j];
          if( polygon[c][n].sel==SELECTED || polygon[c][n].selected_point[j]) {
            if(rotatelocal) {
              ROTATION(polygon[c][n].x[0], polygon[c][n].y[0], polygon[c][n].x[k], polygon[c][n].y[k], rx1,ry1);
            } else {
              ROTATION(x1, y_1, polygon[c][n].x[j], polygon[c][n].y[j], rx1,ry1);
            }
            x[j] = rx1+deltax;
            y[j] = ry1+deltay;
          } else {
            x[j] = polygon[c][n].x[j];
            y[j] = polygon[c][n].y[j];
          }
          bbox(ADD, bx1, by1, bx2, by2); // 20181009
        }
        drawpolygon(k,  NOW, x, y, polygon[c][n].points, polygon[c][n].fill); // 20180914 added fill
        selectedgroup[i].n=lastpolygon[c];
        store_polygon(-1, x, y, polygon[c][n].points, c, polygon[c][n].sel, polygon[c][n].prop_ptr);
        polygon[c][n].sel=0;
      }
      break;
     case ARC:
      if(c!=k) break;
      arc_bbox(arc[c][n].x, arc[c][n].y, arc[c][n].r, arc[c][n].a, arc[c][n].b,
               &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);

      if(rotatelocal) {
        // rotate center wrt itself: do nothing
        rx1 = arc[c][n].x;
        ry1 = arc[c][n].y;
      } else {
        ROTATION(x1, y_1, arc[c][n].x, arc[c][n].y, rx1,ry1);
      }
      angle = arc[c][n].a;
      if(flip) {
        angle = 270.*rot+180.-arc[c][n].b-arc[c][n].a;
      } else {
        angle = arc[c][n].a+rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;



      arc[c][n].sel=0;
      drawarc(k, ADD, rx1+deltax, ry1+deltay, arc[c][n].r, angle, arc[c][n].b);
      selectedgroup[i].n=lastarc[c];
      store_arc(-1, rx1+deltax, ry1+deltay,
                 arc[c][n].r, angle, arc[c][n].b, c, SELECTED, arc[c][n].prop_ptr);
      break;

     case RECT:
      if(c!=k) break;
      bbox(ADD, rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2); // 20181009
      if(rotatelocal) {
        ROTATION(rect[c][n].x1, rect[c][n].y1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
        ROTATION(rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
      } else {
        ROTATION(x1, y_1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
        ROTATION(x1, y_1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
      }
      RECTORDER(rx1,ry1,rx2,ry2);
      rect[c][n].sel=0;
      drawrect(k, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      filledrect(k, ADD, rx1+deltax, ry1+deltay, rx2+deltax, ry2+deltay);
      selectedgroup[i].n=lastrect[c];
      storeobject(-1, rx1+deltax, ry1+deltay, 
                 rx2+deltax, ry2+deltay,RECT, c, SELECTED, rect[c][n].prop_ptr);
      break;

     case TEXT:
      if(k!=TEXTLAYER) break;
      check_text_storage();
      // 20181009
      #ifdef HAS_CAIRO
      customfont = set_text_custom_font(&textelement[n]);
      #endif
      text_bbox(textelement[n].txt_ptr, textelement[n].xscale,
             textelement[n].yscale, textelement[n].rot,textelement[n].flip,
                textelement[n].x0, textelement[n].y0,
                &rx1,&ry1, &rx2,&ry2);
      #ifdef HAS_CAIRO
      if(customfont) cairo_restore(ctx);
      #endif
      bbox(ADD, rx1, ry1, rx2, ry2 );

      if(rotatelocal) {
        ROTATION(textelement[n].x0, textelement[n].y0, textelement[n].x0, textelement[n].y0, rx1,ry1);
      } else {
        ROTATION(x1, y_1, textelement[n].x0, textelement[n].y0, rx1,ry1);
      }
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
      textelement[lasttext].font=NULL;
      my_strdup(&textelement[lasttext].prop_ptr, textelement[n].prop_ptr);
      my_strdup(&textelement[lasttext].font, get_tok_value(textelement[lasttext].prop_ptr, "font", 0));//20171206
      strlayer = get_tok_value(textelement[lasttext].prop_ptr, "layer", 0); //20171206
      if(strlayer[0]) textelement[lasttext].layer = atoi(strlayer);
      else textelement[lasttext].layer = -1;

      textelement[lasttext].xscale=textelement[n].xscale;
      textelement[lasttext].yscale=textelement[n].yscale;

      textlayer = TEXTLAYER;
      #ifdef HAS_CAIRO
      textlayer = textelement[lasttext].layer; // 20171206
      if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
      textfont = textelement[lasttext].font; // 20171206
      if(textfont && textfont[0]) {
        cairo_save(ctx);
        cairo_save(save_ctx);
        cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_select_font_face (save_ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      }
      #endif
      draw_string(textlayer, ADD, textelement[lasttext].txt_ptr,        // draw moved txt
       textelement[lasttext].rot, textelement[lasttext].flip,
       rx1+deltax,ry1+deltay,
       textelement[lasttext].xscale, textelement[lasttext].yscale);
      #ifdef HAS_CAIRO
      if(textfont && textfont[0]) {
        // cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        // cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_restore(ctx);
        cairo_restore(save_ctx);
      }
      #endif
      selectedgroup[i].n=lasttext;
      lasttext++;
       if(debug_var>=2) fprintf(errfp, "copy_objects(): done copy string\n");
      break;
     case ELEMENT:
      if(k==0) {
       check_inst_storage();
       // 20181009
       symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1, &inst_ptr[n].x2, &inst_ptr[n].y2 ); // 20171201
       bbox(ADD, inst_ptr[n].x1, inst_ptr[n].y1, inst_ptr[n].x2, inst_ptr[n].y2 );

       if(rotatelocal) {
         ROTATION(inst_ptr[n].x0, inst_ptr[n].y0, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
       } else {
         ROTATION(x1, y_1, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
       }
       inst_ptr[lastinst] = inst_ptr[n];
       inst_ptr[lastinst].prop_ptr=NULL;
       inst_ptr[lastinst].instname=NULL; // 20150409
       inst_ptr[lastinst].node=NULL;
       inst_ptr[lastinst].name=NULL;
       my_strdup(&inst_ptr[lastinst].name, inst_ptr[n].name);
       my_strdup(&inst_ptr[lastinst].prop_ptr, inst_ptr[n].prop_ptr);
       my_strdup2(&inst_ptr[lastinst].instname, get_tok_value(inst_ptr[n].prop_ptr, "name",0)); // 20150409
       inst_ptr[n].sel=0;
       inst_ptr[lastinst].x0 = rx1+deltax;
       inst_ptr[lastinst].y0 = ry1+deltay;
       inst_ptr[lastinst].sel = SELECTED;
       inst_ptr[lastinst].rot = (inst_ptr[lastinst].rot + 
          ( (flip && (inst_ptr[lastinst].rot & 1) ) ? rot+2 : rot) ) & 0x3;
       inst_ptr[lastinst].flip = (flip? !inst_ptr[n].flip:inst_ptr[n].flip);
       new_prop_string(&inst_ptr[lastinst].prop_ptr, inst_ptr[n].prop_ptr,newpropcnt++);
       my_strdup2(&inst_ptr[lastinst].instname, get_tok_value(inst_ptr[lastinst].prop_ptr, "name", 0)); // 20150409
       // the final newpropcnt argument is zero for the 1st call and used in 
       // new_prop_string() for cleaning some internal caches.
       hash_proplist(inst_ptr[lastinst].prop_ptr , 0);
       n=selectedgroup[i].n=lastinst;
       symbol_bbox(lastinst, &inst_ptr[lastinst].x1, &inst_ptr[lastinst].y1,
                         &inst_ptr[lastinst].x2, &inst_ptr[lastinst].y2);
       lastinst++;
      }

      draw_symbol_outline(ADD,k, n,k, 0, 0, 0.0, 0.0);
      break;
    }
   }
   filledrect(k, END, 0.0, 0.0, 0.0, 0.0);
   drawarc(k, END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawrect(k, END, 0.0, 0.0, 0.0, 0.0);
   drawline(k, END, 0.0, 0.0, 0.0, 0.0);
   
  } // end for(k ...
  update_conn_cues(1, 1); 
  ui_state &= ~STARTCOPY;
  check_collapsing_objects();
  x1=y_1=x2=y_2=rot=flip=deltax=deltay=0;
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  need_rebuild_selected_array=1;
  rotatelocal=0;
  draw_window = save_draw;
 }
 draw_selection(gc[SELLAYER], 0);
}


void move_objects(int what, int merge, double dx, double dy)
{
 int c, i, n, k;
 Box tmp;
 double angle;
 double tx1,ty1; // temporaries for swapping coordinates 20070302
 int textlayer;
 #ifdef HAS_CAIRO
 int customfont;
 #endif
 // 20171112
 #ifdef HAS_CAIRO
 char  *textfont;
 #endif


 if(what & BEGIN)
 {
  rotatelocal=0; //20171208
  deltax = deltay = 0.0;
  rebuild_selected_array();
  lastsel = lastselected;
   if(merge) x1=y_1=0.0;
   else if(lastselected==1 && selectedgroup[0].type==ARC &&
           arc[c=selectedgroup[0].col][n=selectedgroup[0].n].sel!=SELECTED) {
     x1 = arc[c][n].x;
     y_1 = arc[c][n].y;
   } else {x1=mousex_snap;y_1=mousey_snap;}
   flip = 0;rot = 0;
  ui_state|=STARTMOVE;
 }
 if(what & ABORT)				// draw objects while moving
 {
  draw_selection(gctiled,0);
  rot=flip=deltax=deltay=0;
  ui_state&=~STARTMOVE;
 }
 if(what & RUBBER)				// abort operation
 {
  x2=mousex_snap;y_2=mousey_snap;
  draw_selection(gctiled,0);
  deltax = x2-x1; deltay = y_2 - y_1;
  draw_selection(gc[SELLAYER],1);
 }
 if(what & ROTATELOCAL) { //20171208
  rotatelocal=1;
 } 
 if(what & ROTATE) {
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
  int save_draw; // 20181009
  save_draw = draw_window;
  draw_window=1; // temporarily re-enable draw to window together with pixmap
  draw_selection(gctiled,0);
  bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
  modified=1; 
  prepared_hash_instances=0; // 20171224
  prepared_hash_wires=0; // 20171224
  prepared_netlist_structs=0;
  prepared_hilight_structs=0;

  if(! ( ui_state & STARTMERGE) ) push_undo(); // 20150327 push_undo

  if(dx!=0.0 || dy!=0.0) {
    deltax = dx;
    deltay = dy;
  }
  for(k=0;k<cadlayers;k++)
  {
   drawarc(k, BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawline(k, BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawrect(k, BEGIN, 0.0, 0.0, 0.0, 0.0);
   filledrect(k, BEGIN, 0.0, 0.0, 0.0, 0.0); 
   for(i=0;i<lastselected;i++)
   {
    c = selectedgroup[i].col;n = selectedgroup[i].n;
    switch(selectedgroup[i].type)
    {
     case WIRE:
      if(k!=WIRELAYER) break;
      // if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])   // 26122004
      if(wire[n].bus){ // 20171201
        bbox(ADD, wire[n].x1-BUS_WIDTH, wire[n].y1-BUS_WIDTH , wire[n].x2+BUS_WIDTH , wire[n].y2+BUS_WIDTH );
        int ov, y1, y2;
        ov = BUS_WIDTH> CADHALFDOTSIZE ? BUS_WIDTH : CADHALFDOTSIZE;
        if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
        else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
        bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
      } else {
        int ov, y1, y2;
        ov = CADHALFDOTSIZE;
        if(wire[n].y1 < wire[n].y2) { y1 = wire[n].y1-ov; y2 = wire[n].y2+ov; }
        else                        { y1 = wire[n].y1+ov; y2 = wire[n].y2-ov; }
        bbox(ADD, wire[n].x1-ov, y1 , wire[n].x2+ov , y2 );
      }
      if(rotatelocal) {
        ROTATION(wire[n].x1, wire[n].y1, wire[n].x1, wire[n].y1, rx1,ry1);
        ROTATION(wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2, rx2,ry2);
      } else {
        ROTATION(x1, y_1, wire[n].x1, wire[n].y1, rx1,ry1);
        ROTATION(x1, y_1, wire[n].x2, wire[n].y2, rx2,ry2);
      }

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
      // if(get_tok_value(wire[n].prop_ptr,"bus",0)[0])        // 26122004
      if(wire[n].bus) // 20171201
        drawline(k, THICK, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2);
      else
        drawline(k, ADD, wire[n].x1, wire[n].y1, wire[n].x2, wire[n].y2);
      break;
     case LINE:
      if(c!=k) break;   
      bbox(ADD, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2);
      if(rotatelocal) {
        ROTATION(line[c][n].x1, line[c][n].y1, line[c][n].x1, line[c][n].y1, rx1,ry1);
        ROTATION(line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2, rx2,ry2);
      } else {
        ROTATION(x1, y_1, line[c][n].x1, line[c][n].y1, rx1,ry1);
        ROTATION(x1, y_1, line[c][n].x2, line[c][n].y2, rx2,ry2);
      }

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
      drawline(k, ADD, line[c][n].x1, line[c][n].y1, line[c][n].x2, line[c][n].y2);
      break;

     case POLYGON: // 20171115
      if(c!=k) break;
      {
        double bx1=0., by1=0., bx2=0., by2=0.;
        int k;
        for(k=0; k<polygon[c][n].points; k++) {
          if(k==0 || polygon[c][n].x[k] < bx1) bx1 = polygon[c][n].x[k];
          if(k==0 || polygon[c][n].y[k] < by1) by1 = polygon[c][n].y[k];
          if(k==0 || polygon[c][n].x[k] > bx2) bx2 = polygon[c][n].x[k];
          if(k==0 || polygon[c][n].y[k] > by2) by2 = polygon[c][n].y[k];

          if( polygon[c][n].sel==SELECTED || polygon[c][n].selected_point[k]) {
            if(rotatelocal) {
              ROTATION(polygon[c][n].x[0], polygon[c][n].y[0], polygon[c][n].x[k], polygon[c][n].y[k], rx1,ry1);
            } else {
              ROTATION(x1, y_1, polygon[c][n].x[k], polygon[c][n].y[k], rx1,ry1);
            }

            polygon[c][n].x[k] =  rx1+deltax;
            polygon[c][n].y[k] =  ry1+deltay;
          }
           
        }
        bbox(ADD, bx1, by1, bx2, by2);
      }
      // 20180914 added fill
      drawpolygon(k,  NOW, polygon[c][n].x, polygon[c][n].y, polygon[c][n].points, polygon[c][n].fill);
      break;

     case ARC:
      if(c!=k) break;
      arc_bbox(arc[c][n].x, arc[c][n].y, arc[c][n].r, arc[c][n].a, arc[c][n].b,
               &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
      if(debug_var>=1) fprintf(errfp, "move_objects(): arc_bbox: %g %g %g %g\n", tmp.x1, tmp.y1, tmp.x2, tmp.y2);
      bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);

      if(rotatelocal) {
        // rotate center wrt itself: do nothing
        rx1 = arc[c][n].x;
        ry1 = arc[c][n].y;
      } else {
        ROTATION(x1, y_1, arc[c][n].x, arc[c][n].y, rx1,ry1);
      }
      angle = arc[c][n].a;
      if(flip) {
        angle = 270.*rot+180.-arc[c][n].b-arc[c][n].a;
      } else {
        angle = arc[c][n].a+rot*270.;
      }
      angle = fmod(angle, 360.);
      if(angle<0.) angle+=360.;

      if(arc[c][n].sel == SELECTED) {
        arc[c][n].x = rx1+deltax;
        arc[c][n].y = ry1+deltay;
        arc[c][n].a = angle;
      } else if(arc[c][n].sel == SELECTED1) {
        arc[c][n].x = rx1;
        arc[c][n].y = ry1;
        if(arc[c][n].r+deltax) arc[c][n].r = fabs(arc[c][n].r+deltax);
        arc[c][n].a = angle;
      } else if(arc[c][n].sel == SELECTED2) {
        angle = round(fmod(atan2(-deltay, deltax)*180./XSCH_PI+angle, 360.));
        if(angle<0.) angle +=360.;
        arc[c][n].x = rx1;
        arc[c][n].y = ry1;
        arc[c][n].a = angle;
      } else if(arc[c][n].sel==SELECTED3) {
        angle = round(fmod(atan2(-deltay, deltax)*180./XSCH_PI+arc[c][n].b, 360.));
        if(angle<0.) angle +=360.;
        if(angle==0) angle=360.;
        arc[c][n].x = rx1;
        arc[c][n].y = ry1;
        arc[c][n].b = angle;
      }
      drawarc(k, ADD, arc[c][n].x, arc[c][n].y, arc[c][n].r, arc[c][n].a, arc[c][n].b);
      break;

     case RECT:
      if(c!=k) break;
      bbox(ADD, rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2);
      if(rotatelocal) {
        ROTATION(rect[c][n].x1, rect[c][n].y1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
        ROTATION(rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
      } else {
        ROTATION(x1, y_1, rect[c][n].x1, rect[c][n].y1, rx1,ry1);
        ROTATION(x1, y_1, rect[c][n].x2, rect[c][n].y2, rx2,ry2);
      }

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
      drawrect(k, ADD, rect[c][n].x1, rect[c][n].y1, rect[c][n].x2, rect[c][n].y2);
      filledrect(c, ADD, rect[c][n].x1, rect[c][n].y1, 
                 rect[c][n].x2, rect[c][n].y2);
  
      break;

     case TEXT:
      if(k!=TEXTLAYER) break;
      #ifdef HAS_CAIRO
      customfont = set_text_custom_font(&textelement[n]);
      #endif
      text_bbox(textelement[n].txt_ptr, textelement[n].xscale,
   	     textelement[n].yscale, textelement[n].rot,textelement[n].flip,
                textelement[n].x0, textelement[n].y0,
                &rx1,&ry1, &rx2,&ry2);
      #ifdef HAS_CAIRO
      if(customfont) cairo_restore(ctx);
      #endif
      bbox(ADD, rx1, ry1, rx2, ry2 );
 
      if(rotatelocal) {
        ROTATION(textelement[n].x0, textelement[n].y0, textelement[n].x0, textelement[n].y0, rx1,ry1);
      } else {
        ROTATION(x1, y_1, textelement[n].x0, textelement[n].y0, rx1,ry1);
      }
 
      textelement[n].x0=rx1+deltax;
      textelement[n].y0=ry1+deltay;
      textelement[n].rot=(textelement[n].rot +
       ( (flip && (textelement[n].rot & 1) ) ? rot+2 : rot) ) & 0x3;
      textelement[n].flip=flip^textelement[n].flip;
 
      // 20171112
      textlayer = TEXTLAYER;
      #ifdef HAS_CAIRO
      textlayer = textelement[n].layer; // 20171206
      if(textlayer < 0 || textlayer >=  cadlayers) textlayer = TEXTLAYER;
      textfont = textelement[n].font; // 20171206
      if(textfont && textfont[0]) {
        cairo_save(ctx);
        cairo_save(save_ctx);
        cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_select_font_face (save_ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      }
      #endif
      draw_string(textlayer, ADD, textelement[n].txt_ptr,        // draw moved txt
       textelement[n].rot, textelement[n].flip,
       textelement[n].x0, textelement[n].y0,
       textelement[n].xscale, textelement[n].yscale);
      #ifdef HAS_CAIRO
      if(textfont && textfont[0]) {
        //cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        //cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_restore(ctx);
        cairo_restore(save_ctx);
      }
      #endif
      break;
 
     case ELEMENT:
      if(k==0) {
       symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1, &inst_ptr[n].x2, &inst_ptr[n].y2 ); // 20171201
       bbox(ADD, inst_ptr[n].x1, inst_ptr[n].y1, inst_ptr[n].x2, inst_ptr[n].y2 );
       if(rotatelocal) {
         ROTATION(inst_ptr[n].x0, inst_ptr[n].y0, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
       } else {
         ROTATION(x1, y_1, inst_ptr[n].x0, inst_ptr[n].y0, rx1,ry1);
       }

       inst_ptr[n].x0 = rx1+deltax;
       inst_ptr[n].y0 = ry1+deltay;
       inst_ptr[n].rot = (inst_ptr[n].rot + 
        ( (flip && (inst_ptr[n].rot & 1) ) ? rot+2 : rot) ) & 0x3;
       inst_ptr[n].flip = flip ^ inst_ptr[n].flip;
       symbol_bbox(n, &inst_ptr[n].x1, &inst_ptr[n].y1,
                         &inst_ptr[n].x2, &inst_ptr[n].y2);
      }
      
      draw_symbol_outline(ADD,k, n,k, 0, 0, 0.0, 0.0);
      break;
    }
   }
   

   filledrect(k, END, 0.0, 0.0, 0.0, 0.0); 
   drawarc(k, END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawrect(k, END, 0.0, 0.0, 0.0, 0.0);
   drawline(k, END, 0.0, 0.0, 0.0, 0.0);
  } //end for(k ...
  update_conn_cues(1, 1); 
  ui_state &= ~STARTMOVE;
  ui_state &= ~STARTMERGE;
  check_collapsing_objects();
  x1=y_1=x2=y_2=rot=flip=deltax=deltay=0;
  bbox(SET , 0.0 , 0.0 , 0.0 , 0.0); 
  if(debug_var>=1) fprintf(errfp, "move_objects(): bbox= %d %d %d %d\n", areax1, areay1, areaw, areah);
  draw();
  bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  rotatelocal=0;
  draw_window =save_draw;
 }
 draw_selection(gc[SELLAYER], 0);
}

