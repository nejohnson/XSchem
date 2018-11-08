/* File: select.c
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

static int rot = 0;
static int  flip = 0;
static double xx1,yy1,xx2,yy2;



void symbol_bbox(int i, double *x1,double *y1, double *x2, double *y2)
{
   int j;
   Text text;
   char *tmp_txt;
   int rot,flip;
   double x0, y0 ;
   double text_x0, text_y0;
   int sym_rot, sym_flip;
   double xx1,yy1,xx2,yy2;

   #ifdef HAS_CAIRO
   int customfont;
   #endif
   /* symbol bbox */
   flip = inst_ptr[i].flip;
   rot = inst_ptr[i].rot;
   x0=inst_ptr[i].x0;
   y0=inst_ptr[i].y0;
   ROTATION(0.0,0.0,(inst_ptr[i].ptr+instdef)->minx,
                    (inst_ptr[i].ptr+instdef)->miny,*x1,*y1);
   ROTATION(0.0,0.0,(inst_ptr[i].ptr+instdef)->maxx,
                    (inst_ptr[i].ptr+instdef)->maxy,*x2,*y2);
   RECTORDER(*x1,*y1,*x2,*y2);
   *x1+=x0;*y1+=y0;
   *x2+=x0;*y2+=y0;
   inst_ptr[i].xx1 = *x1;               /* 20070314 added bbox without text */
   inst_ptr[i].yy1 = *y1;               /* for easier select */
   inst_ptr[i].xx2 = *x2;
   inst_ptr[i].yy2 = *y2;
    if(debug_var>=1) fprintf(errfp, "symbol_bbox(): instance=%d %.16g %.16g %.16g %.16g\n",i,*x1, *y1, *x2, *y2);
   
   /* strings bbox */
   for(j=0;j< (inst_ptr[i].ptr+instdef)->texts;j++)
   {
    sym_flip = flip;
    sym_rot = rot;
    text = (inst_ptr[i].ptr+instdef)->txtptr[j];
     if(debug_var>=2) fprintf(errfp, "symbol_bbox(): instance %d text n: %d text str=%s\n", 
           i,j, text.txt_ptr? text.txt_ptr:"NULL");

     tmp_txt = translate(i, text.txt_ptr);

      if(debug_var>=2) fprintf(errfp, "symbol_bbox(): translated text: %s\n", tmp_txt);
     ROTATION(0.0,0.0,text.x0, text.y0,text_x0,text_y0);
     #ifdef HAS_CAIRO
     customfont=set_text_custom_font(&text);
     #endif
     text_bbox(tmp_txt, text.xscale, text.yscale, 
       (text.rot + ( (sym_flip && (text.rot & 1) ) ? sym_rot+2 : sym_rot)) &0x3,
       sym_flip ^ text.flip,
       x0+text_x0,y0+text_y0, &xx1,&yy1,&xx2,&yy2);
     #ifdef HAS_CAIRO
     if(customfont) cairo_restore(ctx);
     #endif
     if(xx1<*x1) *x1=xx1;
     if(yy1<*y1) *y1=yy1;
     if(xx2>*x2) *x2=xx2;
     if(yy2>*y2) *y2=yy2;
      if(debug_var>=2) fprintf(errfp, "symbol_bbox(): instance=%d text=%d %.16g %.16g %.16g %.16g\n",i,j, *x1, *y1, *x2, *y2);

   }
}


static void del_rect_line_arc_poly(void)
{
 Box tmp;
 int c, j, i;

 for(c=0;c<cadlayers;c++)
 {
  j = 0; 
  for(i=0;i<lastrect[c];i++)
  {
   if(rect[c][i].sel == SELECTED)
   {
    j++; 
    bbox(ADD, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
    modified=1;
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
  j = 0;
  for(i=0;i<lastline[c];i++)
  {
   if(line[c][i].sel == SELECTED)
   {
    j++;
    bbox(ADD, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
    modified=1;
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

  j = 0;
  for(i=0;i<lastarc[c];i++)
  {
   if(arc[c][i].sel == SELECTED)
   {
    j++;
    arc_bbox(arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b,
             &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    bbox(ADD, tmp.x1, tmp.y1, tmp.x2, tmp.y2);
    modified=1;
    continue;
   }
   if(j)
   {
    my_strdup(&arc[c][i-j].prop_ptr, NULL);
    arc[c][i-j] = arc[c][i];
    arc[c][i].prop_ptr=NULL;
   }
  }
  lastarc[c] -= j;


  /* 20171115 */
  j = 0;
  for(i=0;i<lastpolygon[c];i++)
  {
   if(polygon[c][i].sel == SELECTED)
   { 
    int k;
    double x1=0., y1=0., x2=0., y2=0.;
    for(k=0; k<polygon[c][i].points; k++) {
      if(k==0 || polygon[c][i].x[k] < x1) x1 = polygon[c][i].x[k];
      if(k==0 || polygon[c][i].y[k] < y1) y1 = polygon[c][i].y[k];
      if(k==0 || polygon[c][i].x[k] > x2) x2 = polygon[c][i].x[k];
      if(k==0 || polygon[c][i].y[k] > y2) y2 = polygon[c][i].y[k];
    }
    /*fprintf(errfp, "bbox: %.16g %.16g %.16g %.16g\n", x1, y1, x2, y2); */
    j++;
    bbox(ADD, x1, y1, x2, y2);
    modified=1;
    continue;
   }
   if(j)
   {
    my_strdup(&polygon[c][i-j].prop_ptr, NULL);
    my_free(&polygon[c][i-j].x);
    my_free(&polygon[c][i-j].y);
    my_free(&polygon[c][i-j].selected_point);
    polygon[c][i-j] = polygon[c][i];
    polygon[c][i].prop_ptr=NULL;
    polygon[c][i].x=NULL;
    polygon[c][i].y=NULL;
    polygon[c][i].selected_point=NULL;
   }
  }
  lastpolygon[c] -= j;
 }
}


void delete(void)
{
 int i,j;
 #ifdef HAS_CAIRO
 int customfont;
 #endif

 j = 0;
 bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
 rebuild_selected_array();
 if(lastselected) push_undo(); /* 20150327 */
 for(i=0;i<lasttext;i++)
 {
  if(textelement[i].sel == SELECTED)
  {
   rot = textelement[i].rot;
   flip = textelement[i].flip;
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&textelement[i]);
   #endif
   text_bbox(textelement[i].txt_ptr, textelement[i].xscale,
             textelement[i].yscale, rot, flip,
             textelement[i].x0, textelement[i].y0,
             &xx1,&yy1, &xx2,&yy2);
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(ctx);
   #endif
   bbox(ADD, xx1, yy1, xx2, yy2 );
   my_strdup(&textelement[i].prop_ptr, NULL);
   my_strdup(&textelement[i].font, NULL);
   my_strdup(&textelement[i].txt_ptr, NULL);
   modified=1;
   j++;
   continue;
  }
  if(j)
  {
   if(debug_var>=1) fprintf(errfp, "select(); deleting string %d\n",i-j);
   textelement[i-j] = textelement[i];
   textelement[i].txt_ptr=NULL;
   textelement[i].prop_ptr=NULL;
   textelement[i].font=NULL;
   if(debug_var>=1) fprintf(errfp, "select(); new string %d = %s\n",i-j,textelement[i-j].txt_ptr);
  }
 }
 lasttext -= j;                
 j = 0;
 for(i=0;i<lastinst;i++)
 {
  if(inst_ptr[i].sel == SELECTED)
  {
   modified=1;
   prepared_hash_instances=0;
   prepared_netlist_structs=0;
   prepared_hilight_structs=0;

   symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2); /*20171201 */
   bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
   if(inst_ptr[i].prop_ptr != NULL) 
   {
    hash_proplist(inst_ptr[i].prop_ptr , 1); /* remove props from hash table */
    my_strdup(&inst_ptr[i].prop_ptr, NULL);
   }
   delete_inst_node(i);
   my_strdup(&inst_ptr[i].name, NULL);
   my_strdup(&inst_ptr[i].instname, NULL); /* 20150409 */
   j++;
   continue;
  }
  if(j) 
  {
   inst_ptr[i-j] = inst_ptr[i];
   inst_ptr[i].prop_ptr=NULL;
   inst_ptr[i].node=NULL;
   inst_ptr[i].name=NULL;        
   inst_ptr[i].instname=NULL;  /* 20150409 */
   inst_ptr[i].flags=0;        
  }
 }
 lastinst-=j;

  j = 0; 
  for(i=0;i<lastwire;i++)
  {
   if(wire[i].sel == SELECTED)
   {
    j++; 
    if(wire[i].bus){ /* 20171201 */
      int ov, y1, y2;
      ov = BUS_WIDTH> CADHALFDOTSIZE ? BUS_WIDTH : CADHALFDOTSIZE;
      if(wire[i].y1 < wire[i].y2) { y1 = wire[i].y1-ov; y2 = wire[i].y2+ov; }
      else                        { y1 = wire[i].y1+ov; y2 = wire[i].y2-ov; }
      bbox(ADD, wire[i].x1-ov, y1 , wire[i].x2+ov , y2 );
    } else {
      int ov, y1, y2;
      ov = CADHALFDOTSIZE;
      if(wire[i].y1 < wire[i].y2) { y1 = wire[i].y1-ov; y2 = wire[i].y2+ov; }
      else                        { y1 = wire[i].y1+ov; y2 = wire[i].y2-ov; }
      bbox(ADD, wire[i].x1-ov, y1 , wire[i].x2+ov , y2 );
    }

    modified=1;
    prepared_hash_wires=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
    
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

 del_rect_line_arc_poly();
 update_conn_cues(0, 0);
 lastselected = 0;
 bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
 draw();
 bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
 ui_state &= ~SELECTION;
}


void delete_only_rect_line_arc_poly(void)
{
 bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
 del_rect_line_arc_poly();
 lastselected = 0;
 bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
 draw();
 bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
 ui_state &= ~SELECTION;
}


void bbox(int what,double x1,double y1, double x2, double y2)
{
 int i;
 static int bbx1, bbx2, bby1, bby2; 
 static int savew, saveh, savex1, savex2, savey1, savey2;
 static int semaphore=0;

 /* fprintf(errfp, "bbox: what=%d\n", what); */
 switch(what)
 {
  case BEGIN:
   if(semaphore==1) {
     fprintf(errfp, "ERROR: rentrant bbox() call\n"); 
     tcleval("alert_ {ERROR: reentrant bbox() call} {}");/*20171215 */
   }
   bbx1 = 300000000;
   bbx2 = 0;
   bby1 = 300000000; 
   bby2 = 0;
   savex1 = areax1;
   savex2 = areax2;
   savey1 = areay1;
   savey2 = areay2;
   savew = areaw;
   saveh = areah;
   semaphore=1;
   break;
  case ADD:
   if(semaphore==0) {
     fprintf(errfp, "ERROR: bbox(ADD) call before bbox(BEGIN)\n"); 
     tcleval("alert_ {ERROR: bbox(ADD) call before bbox(BEGIN)} {}");/*20171215 */
   }
   x1=X_TO_SCREEN(x1);
   y1=Y_TO_SCREEN(y1);
   x2=X_TO_SCREEN(x2);
   y2=Y_TO_SCREEN(y2);
   x1=CLIP(x1,savex1,savex2);
   x2=CLIP(x2,savex1,savex2);
   y1=CLIP(y1,savey1,savey2);
   y2=CLIP(y2,savey1,savey2);
   if(x1 < bbx1) bbx1 = (int) x1;
   if(x2 > bbx2) bbx2 = (int) x2;
   if(y1 < bby1) bby1 = (int) y1;
   if(y2 > bby2) bby2 = (int) y2;
   if(y2 < bby1) bby1 = (int) y2;
   if(y1 > bby2) bby2 = (int) y1;
   break;
  case END:
   areax1 = savex1;
   areax2 = savex2;
   areay1 = savey1;
   areay2 = savey2;
   areaw =  savew;
   areah =  saveh;
   xrect[0].x = 0;
   xrect[0].y = 0;
   xrect[0].width = areaw-4*lw;
   xrect[0].height = areah-4*lw;

   XSetClipMask(display, gctiled, None); /* 20171110 optimization, clipping already done in software */

   for(i=0;i<cadlayers;i++)    
   {
    XSetClipMask(display, gc[i], None); /* 20171110 optimization, clipping already done in software */
    XSetClipMask(display, gcstipple[i], None); /* 20171110 optimization, clipping already done in software */
   }
   #ifdef HAS_CAIRO
   cairo_reset_clip(ctx);
   cairo_reset_clip(save_ctx);
   #endif
   semaphore=0;
   break;
  case SET:
   if(semaphore==0) {
     fprintf(errfp, "ERROR: bbox(SET) call before bbox(BEGIN)\n"); 
     tcleval("alert_ {ERROR: bbox(SET) call before bbox(BEGIN)} {}");/*20171215 */
   }
   areax1 = bbx1-2*lw;
   areax2 = bbx2+2*lw;
   areay1 = bby1-2*lw;
   areay2 = bby2+2*lw;
   areaw = (areax2-areax1);
   areah = (areay2-areay1);

   xrect[0].x = bbx1-lw;
   xrect[0].y = bby1-lw;
   xrect[0].width = bbx2-bbx1+2*lw;
   xrect[0].height = bby2-bby1+2*lw;

   for(i=0;i<cadlayers;i++)
   {
     XSetClipRectangles(display, gc[i], 0,0, xrect, 1, Unsorted);
     XSetClipRectangles(display, gcstipple[i], 0,0, xrect, 1, Unsorted);
   }
   XSetClipRectangles(display, gctiled, 0,0, xrect, 1, Unsorted);
   if(debug_var>=1) fprintf(errfp, "bbox(): bbox= %d %d %d %d\n",areax1,areay1,areax2,areay2);     
   #ifdef HAS_CAIRO
   cairo_rectangle(ctx, xrect[0].x, xrect[0].y, xrect[0].width, xrect[0].height);
   cairo_clip(ctx);
   cairo_rectangle(save_ctx, xrect[0].x, xrect[0].y, xrect[0].width, xrect[0].height);
   cairo_clip(save_ctx);
   #endif
   break;
  case DRAW:
   XCopyArea(display, save_pixmap, window, gctiled, xrect[0].x, xrect[0].y,
       xrect[0].width, xrect[0].height, xrect[0].x, xrect[0].y); /* 20181009 */

  break;
 }
}

void unselect_all(void)
{
 int i,c;
 char str[PATH_MAX];
 #ifdef HAS_CAIRO
 int customfont;
 #endif
    ui_state = 0; 
    lastselected = 0;
   
    drawtemparc(gctiled,BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtempline(gctiled,BEGIN, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(gctiled, BEGIN, 0.0, 0.0, 0.0, 0.0); 
   
     for(i=0;i<lastwire;i++)
     {
      if(wire[i].sel)
      {
       wire[i].sel = 0;
       {
         if(wire[i].bus) /* 20171201 */
           drawtempline(gctiled, THICK, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
         else
           drawtempline(gctiled, ADD, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
       }
      }
     }
   
    for(i=0;i<lastinst;i++)
    {
     if(inst_ptr[i].sel == SELECTED)
     {
      inst_ptr[i].sel = 0;
      for(c=0;c<cadlayers;c++)
        draw_temp_symbol_outline(ADD, gctiled, i, c,0,0,0.0,0.0);
     }
    }
    for(i=0;i<lasttext;i++)
    {
     if(textelement[i].sel == SELECTED)
     {
      textelement[i].sel = 0;
      #ifdef HAS_CAIRO
      customfont = set_text_custom_font(& textelement[i]); /* needed for bbox calculation */
      #endif
      draw_temp_string(gctiled,ADD, textelement[i].txt_ptr,
       textelement[i].rot, textelement[i].flip,
       textelement[i].x0, textelement[i].y0,
       textelement[i].xscale, textelement[i].yscale);
      #ifdef HAS_CAIRO
      if(customfont) cairo_restore(ctx);
      #endif
     }
    }          
    for(c=0;c<cadlayers;c++)
    {
     for(i=0;i<lastarc[c];i++)
     {
      if(arc[c][i].sel) 
      {
       arc[c][i].sel = 0;
       drawtemparc(gctiled, ADD, arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b);
      }
     }
     for(i=0;i<lastrect[c];i++)
     {
      if(rect[c][i].sel) 
      {
       rect[c][i].sel = 0;
       drawtemprect(gctiled, ADD, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
      }
     }
     for(i=0;i<lastline[c];i++)
     {
      if(line[c][i].sel)
      {
       line[c][i].sel = 0;
       drawtempline(gctiled, ADD, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
      }
     }
     for(i=0;i<lastpolygon[c];i++)
     {
      if(polygon[c][i].sel)
      {
       int k;
       for(k=0;k<polygon[c][i].points; k++) polygon[c][i].selected_point[k] = 0;
       polygon[c][i].sel = 0;
       drawtemppolygon(gctiled, NOW, polygon[c][i].x, polygon[c][i].y, polygon[c][i].points);
      }
     }
    } 
    drawtemparc(gctiled, END, 0.0, 0.0, 0.0, 0.0, 0.0); 
    drawtemprect(gctiled, END, 0.0, 0.0, 0.0, 0.0); 
    drawtempline(gctiled,END, 0.0, 0.0, 0.0, 0.0);
    ui_state &= ~SELECTION;
    /*\statusmsg("",2); */
    my_snprintf(str, S(str), "%s/%s", home_dir, ".xschem_selection.sch"); /* 20161115  PWD->HOME */
    unlink(str);
}

void select_wire(int i,unsigned short select_mode, int fast)
{
  char str[1024];       /* overflow safe */
  /*my_strncpy(s,wire[i].prop_ptr!=NULL?wire[i].prop_ptr:"<NULL>",256); */
  if( !fast )
  {
    my_snprintf(str, S(str), "selected wire: n=%d end1=%d end2=%d\nnode=%s",i,
           wire[i].end1, wire[i].end2, 
           wire[i].prop_ptr? wire[i].prop_ptr: "(null)");
    statusmsg(str,2);
   /* 20070323 */
   my_snprintf(str, S(str), "x = %.16g  y = %.16g  w = %.16g h = %.16g",wire[i].x1, wire[i].y1,
      wire[i].x2-wire[i].x1, wire[i].y2-wire[i].y1
   );
   statusmsg(str,1);
  }
  if( ((wire[i].sel|select_mode) == (SELECTED1|SELECTED2)) ||
      ((wire[i].sel == SELECTED) && select_mode) )
   wire[i].sel = SELECTED;
  else 
   wire[i].sel = select_mode;
  if(select_mode) {
   if(debug_var>=1) fprintf(errfp, "select(): wire[%d].end1=%d, ,end2=%d\n", i, wire[i].end1, wire[i].end2);
   if(wire[i].bus) /* 20171201 */
     drawtempline(gc[SELLAYER], THICK, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
   else
     drawtempline(gc[SELLAYER], ADD, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
  }
  else {
   /* if(get_tok_value(wire[i].prop_ptr,"bus",0)[0]) */
   if(wire[i].bus) /* 20171201 */
     drawtempline(gctiled, THICK, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
   else
     drawtempline(gctiled, NOW, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2);
  }
  need_rebuild_selected_array=1;
}

void select_element(int i,unsigned short select_mode, int fast)
{
  int c, j;
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  my_strncpy(s,inst_ptr[i].prop_ptr!=NULL?inst_ptr[i].prop_ptr:"<NULL>",S(s));
  if( !fast )
  {
   my_snprintf(str, S(str), "selected element %d: %s properties: %s", i, inst_ptr[i].name,s);
   statusmsg(str,2);
   my_snprintf(str, S(str), "symbol .name=%s", inst_ptr[i].name==NULL?"(null)":inst_ptr[i].name);
   statusmsg(str,2);
   for(j=0;j< (inst_ptr[i].ptr+instdef)->rects[PINLAYER] ;j++) 
   {
    /*                                         --------20170323 check prop_ptr---------------------------- */
    if(inst_ptr[i].node && inst_ptr[i].node[j] && (inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][j].prop_ptr)
    {
     my_snprintf(str, S(str), "pin:%s -> %s", 
       get_tok_value(
          (inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][j].prop_ptr,"name",0) ,
       inst_ptr[i].node[j]);
     statusmsg(str,2);
    }
   }
   my_snprintf(str, S(str), "x = %.16g  y = %.16g  w = %.16g h = %.16g",inst_ptr[i].xx1, inst_ptr[i].yy1,
      inst_ptr[i].xx2-inst_ptr[i].xx1, inst_ptr[i].yy2-inst_ptr[i].yy1
   );
   statusmsg(str,1);

  }
  inst_ptr[i].sel = select_mode;
  if(select_mode) {
   for(c=0;c<cadlayers;c++) {
    draw_temp_symbol_outline(ADD, gc[SELLAYER], i,c,0,0,0.0,0.0);
   }
  } else {
   for(c=0;c<cadlayers;c++) {
    draw_temp_symbol_outline(NOW, gctiled, i,c,0,0,0.0,0.0);
   }
  }
  need_rebuild_selected_array=1;
}

void select_text(int i,unsigned short select_mode, int fast)
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  if(!fast) {
    my_strncpy(s,textelement[i].prop_ptr!=NULL?textelement[i].prop_ptr:"<NULL>",S(s));
    my_snprintf(str, S(str), "selected text %d: properties: %s", i,s);
    statusmsg(str,2);
  }
  #ifdef HAS_CAIRO
  int customfont;
  #endif

  textelement[i].sel = select_mode;

  #ifdef HAS_CAIRO
  customfont = set_text_custom_font(&textelement[i]);
  #endif
  if(select_mode)
    draw_temp_string(gc[SELLAYER],ADD, textelement[i].txt_ptr,
     textelement[i].rot, textelement[i].flip,
     textelement[i].x0, textelement[i].y0,
     textelement[i].xscale, textelement[i].yscale);
  else
    draw_temp_string(gctiled,NOW, textelement[i].txt_ptr,
     textelement[i].rot, textelement[i].flip,
     textelement[i].x0, textelement[i].y0,
     textelement[i].xscale, textelement[i].yscale);
  #ifdef HAS_CAIRO
  if(customfont) cairo_restore(ctx);
  #endif
  need_rebuild_selected_array=1;
}

void select_box(int c, int i, unsigned short select_mode, int fast)
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,rect[c][i].prop_ptr!=NULL?rect[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "selected box : layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,2);
   /* 20070323 */
   my_snprintf(str, S(str), "x = %.16g  y = %.16g  w = %.16g h = %.16g",rect[c][i].x1, rect[c][i].y1,
      rect[c][i].x2-rect[c][i].x1, rect[c][i].y2-rect[c][i].y1
   );
   statusmsg(str,1);
  }
  if(select_mode) {
    if(select_mode==SELECTED) {
      rect[c][i].sel = select_mode; /*20070202 */
    } else {
      rect[c][i].sel |= select_mode; /*20070202 */
    }
    drawtemprect(gc[SELLAYER], ADD, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
  } else {
    rect[c][i].sel = 0; /*20070202 */
    drawtemprect(gctiled, NOW, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
  }

  if( rect[c][i].sel == (SELECTED1|SELECTED2|SELECTED3|SELECTED4)) rect[c][i].sel = SELECTED;

  need_rebuild_selected_array=1;
  /* fprintf(errfp, "select_box(): select_mode=%d, box#=%d, rect[].sel=%d\n", select_mode, i, rect[c][i].sel); */
}



void select_arc(int c, int i, unsigned short select_mode, int fast)
{
  char str[1024];   /* overflow safe */
  char s[256];    /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,rect[c][i].prop_ptr!=NULL?rect[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "selected arc : layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,2);
   /* 20070323 */
   my_snprintf(str, S(str), "x = %.16g  y = %.16g  r = %.16g a = %.16g b = %.16g",
      arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b);
   statusmsg(str,1);
  }
  if(select_mode) {
    arc[c][i].sel = select_mode;
    drawtemparc(gc[SELLAYER], ADD, arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b);
  } else {
    arc[c][i].sel = 0; /*20070202 */
    drawtemparc(gctiled, NOW, arc[c][i].x, arc[c][i].y, arc[c][i].r, arc[c][i].a, arc[c][i].b);
  }

  /*if( arc[c][i].sel == (SELECTED1|SELECTED2|SELECTED3|SELECTED4)) arc[c][i].sel = SELECTED; */

  need_rebuild_selected_array=1;
}



void select_polygon(int c, int i, unsigned short select_mode, int fast )
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,polygon[c][i].prop_ptr!=NULL?polygon[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "selected polygon: layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,2);
   /* 20070323 */
   my_snprintf(str, S(str), "x0 = %.16g  y0 = %.16g ...",polygon[c][i].x[0], polygon[c][i].y[0]);
   statusmsg(str,1);
  }
  polygon[c][i].sel = select_mode;
  if(select_mode) {
   drawtemppolygon(gc[SELLAYER], NOW, polygon[c][i].x, polygon[c][i].y, polygon[c][i].points);
  }
  else
   drawtemppolygon(gctiled, NOW, polygon[c][i].x, polygon[c][i].y, polygon[c][i].points);
  need_rebuild_selected_array=1;
}

void select_line(int c, int i, unsigned short select_mode, int fast )
{
  char str[1024];       /* overflow safe */
  char s[256];          /* overflow safe */
  if(!fast)
  {
   my_strncpy(s,line[c][i].prop_ptr!=NULL?line[c][i].prop_ptr:"<NULL>",S(s));
   my_snprintf(str, S(str), "selected line: layer=%d, n=%d properties: %s",c-4,i,s);
   statusmsg(str,2);
   /* 20070323 */
   my_snprintf(str, S(str), "x = %.16g  y = %.16g  w = %.16g h = %.16g",line[c][i].x1, line[c][i].y1,
      line[c][i].x2-line[c][i].x1, line[c][i].y2-line[c][i].y1
   );
   statusmsg(str,1);
  }
  if( ((line[c][i].sel|select_mode) == (SELECTED1|SELECTED2)) ||
      ((line[c][i].sel == SELECTED) && select_mode) )
   line[c][i].sel = SELECTED;
  else 
   line[c][i].sel = select_mode;
  if(select_mode) {
   drawtempline(gc[SELLAYER], ADD, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
  }
  else
   drawtempline(gctiled, NOW, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
  need_rebuild_selected_array=1;
}

/* 20160503 return type field */
unsigned short select_object(double mousex,double mousey, unsigned short select_mode)
{
   Selected sel;
   sel = find_closest_obj(mousex,mousey);

   drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);

   switch(sel.type)
   {
    case WIRE:
     select_wire(sel.n, select_mode, 0);
     break;
    case TEXT:
     select_text(sel.n, select_mode, 0);
     break;
    case LINE:
     select_line(sel.col, sel.n, select_mode,0);
     break;
    case POLYGON:
     select_polygon(sel.col, sel.n, select_mode,0);
     break;
    case RECT:
     select_box(sel.col,sel.n, select_mode,0);
     break;
    case ARC:
     select_arc(sel.col,sel.n, select_mode,0);
     break;
    case ELEMENT:
     select_element(sel.n,select_mode,0);
     break;
    default:
     break;
   } /*end switch */

   drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
   drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0); 
   drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);

   if(sel.type)  ui_state |= SELECTION;
   return sel.type;
}

void select_inside(double x1,double y1, double x2, double y2, int sel) /* 20150927 added unselect (sel param) */
{
 int c,i;
 double x, y, r, a, b, xa, ya, xb, yb; /* arc */
 Box tmp;
 #ifdef HAS_CAIRO
 int customfont;
 #endif
 drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
 drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0); 
 drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);

 for(i=0;i<lastwire;i++)
 {
  if(RECTINSIDE(wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2, x1,y1,x2,y2))
  {
   ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_wire(i,SELECTED, 1): select_wire(i,0, 1);
  } 
  else if( sel && enable_stretch && POINTINSIDE(wire[i].x1,wire[i].y1, x1,y1,x2,y2) )
  {
   ui_state |= SELECTION;
   select_wire(i,SELECTED1, 1);
  }
  else if( sel && enable_stretch && POINTINSIDE(wire[i].x2,wire[i].y2, x1,y1,x2,y2) )
  {
   ui_state |= SELECTION;
   select_wire(i,SELECTED2, 1);
  }
 }
 for(i=0;i<lasttext;i++)
 {
  rot = textelement[i].rot;
  flip = textelement[i].flip;
  #ifdef HAS_CAIRO
  customfont = set_text_custom_font(&textelement[i]);
  #endif
  text_bbox(textelement[i].txt_ptr, 
             textelement[i].xscale, textelement[i].yscale, rot, flip,
             textelement[i].x0, textelement[i].y0,
             &xx1,&yy1, &xx2,&yy2);
  #ifdef HAS_CAIRO
  if(customfont) cairo_restore(ctx);
  #endif
  if(RECTINSIDE(xx1,yy1, xx2, yy2,x1,y1,x2,y2))
  {
   ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_text(i, SELECTED, 1): select_text(i, 0, 1);
  }
 }                       
 for(i=0;i<lastinst;i++)
 {
  if(RECTINSIDE(inst_ptr[i].xx1, inst_ptr[i].yy1, inst_ptr[i].xx2, inst_ptr[i].yy2, x1,y1,x2,y2))
  {
   ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
   sel ? select_element(i,SELECTED,1): select_element(i,0,1);
  }
 }
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastpolygon[c]; i++) {  /* 20171115 */
    int k, selected_points, flag;

    polygon_bbox(polygon[c][i].x, polygon[c][i].y, polygon[c][i].points, &xa, &ya, &xb, &b);
    if(OUTSIDE(xa, ya, xb, yb, x1, y1, x2, y2)) continue;
    selected_points = 0;
    flag=0;
    for(k=0; k<polygon[c][i].points; k++) {
      if(polygon[c][i].sel==SELECTED) polygon[c][i].selected_point[k] = 1;
      if( POINTINSIDE(polygon[c][i].x[k],polygon[c][i].y[k], x1,y1,x2,y2)) {
        flag=1;
        polygon[c][i].selected_point[k] = sel;
      }
      if(polygon[c][i].selected_point[k]) selected_points++;
    }
    if(flag) {
      if(selected_points==0) {
        select_polygon(c, i, 0, 1);
      }
      if(selected_points==polygon[c][i].points) {
        ui_state |= SELECTION;
        select_polygon(c, i, SELECTED, 1);
      } else if(selected_points) {
        if(sel && enable_stretch) select_polygon(c, i, SELECTED1,1); /* for polygon, SELECTED1 means partial selection */
      }
    }
    
  }
  for(i=0;i<lastline[c];i++)
  {
   if(RECTINSIDE(line[c][i].x1,line[c][i].y1,line[c][i].x2,line[c][i].y2, x1,y1,x2,y2))
   {
    ui_state |= SELECTION;
    sel? select_line(c,i,SELECTED,1): select_line(c,i,0,1);
   }
   else if( sel && enable_stretch && POINTINSIDE(line[c][i].x1,line[c][i].y1, x1,y1,x2,y2) )
   {
    ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
    select_line(c, i,SELECTED1,1);
   }
   else if( sel && enable_stretch && POINTINSIDE(line[c][i].x2,line[c][i].y2, x1,y1,x2,y2) )
   {
    ui_state |= SELECTION;
    select_line(c, i,SELECTED2,1);
   }
  }
  for(i=0;i<lastarc[c];i++) {
    x = arc[c][i].x;
    y = arc[c][i].y;
    a = arc[c][i].a;
    b = arc[c][i].b;
    r = arc[c][i].r;
    xa = x + r * cos(a * XSCH_PI/180.);
    ya = y - r * sin(a * XSCH_PI/180.);
    xb = x + r * cos((a+b) * XSCH_PI/180.);
    yb = y - r * sin((a+b) * XSCH_PI/180.);
    arc_bbox(x, y, r, a, b, &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    if(RECTINSIDE(tmp.x1, tmp.y1, tmp.x2, tmp.y2, x1,y1,x2,y2)) {
      ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
      sel? select_arc(c, i, SELECTED,1): select_arc(c, i, 0,1);
    }
    else if( sel && enable_stretch && POINTINSIDE(x, y, x1, y1, x2, y2) )
    {
     ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
     select_arc(c, i,SELECTED1,1);
    }
    else if( sel && enable_stretch && POINTINSIDE(xb, yb, x1, y1, x2, y2) )
    {
     ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
     select_arc(c, i,SELECTED3,1);
    }
    else if( sel && enable_stretch && POINTINSIDE(xa, ya, x1, y1, x2, y2) )
    {
     ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
     select_arc(c, i,SELECTED2,1);
    }
  }
  for(i=0;i<lastrect[c];i++)
  {
   if(RECTINSIDE(rect[c][i].x1,rect[c][i].y1,rect[c][i].x2,rect[c][i].y2, x1,y1,x2,y2))
   {
    ui_state |= SELECTION; /* set ui_state to SELECTION also if unselecting by area ???? */
    sel? select_box(c,i, SELECTED, 1): select_box(c,i, 0, 1);
   }
   else { 
     if( sel && enable_stretch && POINTINSIDE(rect[c][i].x1,rect[c][i].y1, x1,y1,x2,y2) )
     {                                  /*20070302 added stretch select */
      ui_state |= SELECTION;
      select_box(c, i,SELECTED1,1);
     }
     if( sel && enable_stretch && POINTINSIDE(rect[c][i].x2,rect[c][i].y1, x1,y1,x2,y2) )
     {
      ui_state |= SELECTION;
      select_box(c, i,SELECTED2,1);
     }
     if( sel && enable_stretch && POINTINSIDE(rect[c][i].x1,rect[c][i].y2, x1,y1,x2,y2) )
     {
      ui_state |= SELECTION;
      select_box(c, i,SELECTED3,1);
     }
     if( sel && enable_stretch && POINTINSIDE(rect[c][i].x2,rect[c][i].y2, x1,y1,x2,y2) )
     {
      ui_state |= SELECTION;
      select_box(c, i,SELECTED4,1);
     }
   }

  } /* end for i */
 } /* end for c */
 drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0); 
 drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0); 
 drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
}

void select_all(void)
{
 int c,i;

 drawtemparc(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
 drawtempline(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0);
 drawtemprect(gc[SELLAYER], BEGIN, 0.0, 0.0, 0.0, 0.0); 
 ui_state |= SELECTION;
 for(i=0;i<lastwire;i++)
 {
   select_wire(i,SELECTED, 1);
 }
 for(i=0;i<lasttext;i++)
 {
   select_text(i, SELECTED, 1);
 }                       
 for(i=0;i<lastinst;i++)
 {
   select_element(i,SELECTED,1);
 }
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastpolygon[c];i++)
  {
    select_polygon(c,i,SELECTED,1);
  }
  for(i=0;i<lastline[c];i++)
  {
    select_line(c,i,SELECTED,1);
  }
  for(i=0;i<lastarc[c];i++)
  {
    select_arc(c,i, SELECTED, 1);
  }
  for(i=0;i<lastrect[c];i++)
  {
    select_box(c,i, SELECTED, 1);
  }
 } /* end for c */
 drawtemparc(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0); 
 drawtemprect(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0); 
 drawtempline(gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
}


