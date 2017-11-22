/* File: draw.c
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

static double textx1,textx2,texty1,texty2;
 
int textclip(int x1,int y1,int x2,int y2,
          double xa,double ya,double xb,double yb)
// check if some of (xa,ya-xb,yb) is inside (x1,y1-x2,y2)
// coordinates should be ordered, x1<x2,ya<yb and so on...
{
 if(debug_var>=2) fprintf(errfp, "textclip(): %g %g %g %g - %d %d %d %d\n",
 (xa+xorigin*mooz),(ya+yorigin*mooz),
 (xb+xorigin*mooz),(yb+yorigin*mooz),x1,y1,x2,y2);
 // drawtemprect(gc[WIRELAYER],xa,ya,xb,yb);
 if          ((xa+xorigin)*mooz>x2) return 0;
 else if     ((ya+yorigin)*mooz>y2) return 0;
 else if     ((xb+xorigin)*mooz<x1) return 0;
 else if     ((yb+yorigin)*mooz<y1) return 0;
 return 1;
}           

void print_image()
{
  int w, h, tmp, ww, hh;
  int modified_save; // 20161121
  if(!has_x) return ;

  modified_save=modified; // 20161121 save state
  push_undo(); // 20161121
  collapse_wires();    // 20161121 add connection boxes on wires but undo at end

  XUnmapWindow(display, window);

  ww=areaw-4*lw; hh=areah-4*lw;
 
  ////  use this if you want huge pixmap
  //   w=1280*4; h=1024*4;
  w=ww; h=hh;  // 20121122 

  xrect[0].x = 0;
  xrect[0].y = 0; 
  xrect[0].width = w;
  xrect[0].height = h;
  areax2 = w+2*lw;
  areay2 = h+2*lw;
  areax1 = -2*lw;
  areay1 = -2*lw;
  areaw = areax2-areax1;
  areah = areay2-areay1;
  XFreePixmap(display,save_pixmap);
  // save_pixmap = XCreatePixmap(display,window,areaw,areah,depth);
  save_pixmap = XCreatePixmap(display,window,w,h,depth); // 20161119 pixmap should be exact size of 
                                                         // cliprectangle to avoid random borders
  XSetTile(display,gctiled, save_pixmap);


  #ifdef HAS_CAIRO
  cairo_destroy(save_ctx);
  cairo_surface_destroy(save_sfc);
  save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, w, h);
  save_ctx = cairo_create(save_sfc);
  cairo_set_line_width(save_ctx, 1);
  cairo_set_line_join(save_ctx, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap(save_ctx, CAIRO_LINE_CAP_ROUND);
  cairo_select_font_face (save_ctx, cairo_font_name, 
       CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (save_ctx, 20);
  #endif

  for(tmp=0;tmp<cadlayers;tmp++)
  {
    XSetClipRectangles(display, gc[tmp], 0,0, xrect, 1, Unsorted);
    XSetClipRectangles(display, gcstipple[tmp], 0,0, xrect, 1, Unsorted);
  }
  XSetClipRectangles(display, gctiled, 0,0, xrect, 1, Unsorted);

  draw_grid=0;
  draw_pixmap=1;
  //zoom_full(0); // 20121127 leave zoom as it is, we probably dont want to always print full...
  draw();

  XpmWriteFileFromPixmap(display, "plot.xpm", save_pixmap,0, NULL ); // .gz ????
  if(debug_var>=1) fprintf(errfp, "print_image(): Window image saved\n");

  Tcl_Eval(interp, "convert_to_png plot.xpm"); // 20161121
  pop_undo(0); // 20161121 restore state
  modified=modified_save;  // 20161121

  w=ww;h=hh;
  xrect[0].x = 0;
  xrect[0].y = 0; 
  xrect[0].width = w;
  xrect[0].height = h;
  areax2 = w+2*lw;
  areay2 = h+2*lw;
  areax1 = -2*lw;
  areay1 = -2*lw;
  areaw = areax2-areax1;
  areah = areay2-areay1;
  XFreePixmap(display,save_pixmap);
  save_pixmap = XCreatePixmap(display,window,areaw,areah,depth);
  XSetTile(display,gctiled, save_pixmap);

  #ifdef HAS_CAIRO
  cairo_destroy(save_ctx);
  cairo_surface_destroy(save_sfc);
  save_sfc = cairo_xlib_surface_create(display, save_pixmap, visual, w, h);
  save_ctx = cairo_create(save_sfc);
  cairo_set_line_width(save_ctx, 1);
  cairo_set_line_join(save_ctx, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap(save_ctx, CAIRO_LINE_CAP_ROUND);
  cairo_select_font_face (save_ctx, cairo_font_name, 
     CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (save_ctx, 20);
  #endif

  for(tmp=0;tmp<cadlayers;tmp++)
  {
    XSetClipMask(display, gc[tmp], None); //20171110 no need to clip, already done in software
    XSetClipMask(display, gcstipple[tmp], None); //20171110 
    //XSetClipRectangles(display, gc[tmp], 0,0, xrect, 1, Unsorted);
    //XSetClipRectangles(display, gcstipple[tmp], 0,0, xrect, 1, Unsorted);
  }
  XSetClipMask(display, gctiled, None); //20171110 
  //XSetClipRectangles(display, gctiled, 0,0, xrect, 1, Unsorted);

  XMapWindow(display, window);
  draw_grid=1;
  draw_pixmap=1;
  // zoom_full(0); // 20121127 leave zoom as it was...
  draw();
}

#ifdef HAS_CAIRO
// remember to call cairo_restore(ctx) when done !!
int set_text_custom_font(Text *txt) // 20171122 for correct text_bbox calculation
{
  char *textfont;

  textfont = get_tok_value(txt->prop_ptr, "font", 0);
  if(textfont[0]) {
    cairo_save(ctx);
    cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    return 1;
  }
  return 0;
}
#else
int set_text_custom_font(Text *txt)
{
  return 0;
}
#endif


#ifdef HAS_CAIRO
void cairo_draw_string_line(cairo_t *ctx, char *s,
    double x, double y, double size, int rot, int flip, 
    int lineno, double fontheight, double fontascent, double fontdescent, int llength)
{
  double ix, iy; 
  int rot1;
  cairo_text_extents_t ext;
  int line_delta;
  int line_offset;
  double xadvance;
  double lines;
  double vc; // 20171121 vert correct
  //int rx1, ry1, rx2, ry2, save_rot, save_flip;
  // GC gcclear;
  if(s==NULL) return;
  if(llength==0) return;
  cairo_text_extents(ctx, s, &ext);
  xadvance = ext.x_advance;

  line_delta = lineno*fontheight*cairo_font_line_spacing;
  lines = (cairo_lines-1)*fontheight*cairo_font_line_spacing;
  line_offset=cairo_longest_line-xadvance;
  if(xadvance>14000) return; // too big: close to short overflow
  ix=(x+xorigin)*mooz;
  iy=(y+yorigin)*mooz;
  if(rot&1) {
    rot1=3;
  } else rot1=0;

  vc = cairo_vert_correct*mooz; // converted to device (pixel) space

  if(     rot==0 && flip==0) {iy+=line_delta+fontascent-vc;}
  else if(rot==1 && flip==0) {iy+=xadvance+line_offset;ix=ix-fontheight+fontascent+vc-lines+line_delta;}
  else if(rot==2 && flip==0) {iy=iy-fontheight-lines+line_delta+fontascent+vc; ix=ix-xadvance-line_offset;}
  else if(rot==3 && flip==0) {ix+=line_delta+fontascent-vc;}
  else if(rot==0 && flip==1) {ix=ix-xadvance-line_offset;iy+=line_delta+fontascent-vc;}
  else if(rot==1 && flip==1) {ix=ix-fontheight+line_delta-lines+fontascent+vc;}
  else if(rot==2 && flip==1) {iy=iy-fontheight-lines+line_delta+fontascent+vc;}
  else if(rot==3 && flip==1) {iy=iy+xadvance+line_offset;ix+=line_delta+fontascent-vc;}

// clear area before drawing antialiased text.
/* 
  save_rot=rot;
  save_flip=flip;
  rot=rot1;
  flip=0;

  if(rot==0 || rot==2) {
    ROTATION(ix, iy-ext.height, ix+ext.width, iy, rx2, ry2);
    rx1 = ix;
    ry1 = iy-ext.height;
  } else {
    ROTATION(ix, iy, ix+ext.width, iy+ext.height, rx2, ry2);
    rx1 = ix-ext.height;
    rx2 = rx2-ext.height;
    ry1 = iy;
  }
  RECTORDER(rx1,ry1,rx2,ry2);
 
  if(hilight_nets) gcclear=gc[BACKLAYER]; // clear string draw area when overstriking 
  else gcclear=gctiled;                   // with highlighted labels
  if(ctx == save_ctx) {
    XFillRectangle(display, save_pixmap, gcclear,  (int)rx1, (int)ry1,
        (unsigned int)rx2 - (unsigned int)rx1,
        (unsigned int)ry2 - (unsigned int)ry1);
  } else {
    XFillRectangle(display, window, gcclear,  (int)rx1, (int)ry1,
        (unsigned int)rx2 - (unsigned int )rx1,
        (unsigned int)ry2 - (unsigned int)ry1);
  }
  rot=save_rot;
  flip=save_flip;
*/
  cairo_save(ctx);
  cairo_translate(ctx, ix, iy);
  cairo_rotate(ctx, M_PI/2*rot1);

  // fprintf(errfp, "string_line: |%s|, y_bearing: %f descent: %f ascent: %f height: %f\n",
  //     s, ext.y_bearing, fontdescent, fontascent, fontheight);
  cairo_move_to(ctx, 0,0);

  cairo_show_text(ctx, s);
  cairo_restore(ctx);
}

// CAIRO version
void draw_string(int layer, int what, char *s, int rot, int flip, double x, double y, double xscale, double yscale)
{
  char *tt, *ss;
  char c;
  int lineno=0; 
  double size;
  cairo_font_extents_t fext;
  int llength=0;

  (void)what; // UNUSED in cairo version, avoid compiler warning
  if(s==NULL || !has_x ) return;
  size = (xscale+yscale)*26.*cairo_font_scale;
  //fprintf(errfp, "size=%g\n", size*mooz);
  if(size*mooz<3.0) return; // too small
  if(size*mooz>800) return; // too big

  text_bbox(s, xscale, yscale, rot, flip, x,y, &textx1,&texty1,&textx2,&texty2);
  if(!textclip(areax1,areay1,areax2,areay2,textx1,texty1,textx2,texty2)) return;

  cairo_set_source_rgb(ctx,
    (double)xcolor_array[layer].red/65535.0,
    (double)xcolor_array[layer].green/65535.0,
    (double)xcolor_array[layer].blue/65535.0);
  cairo_set_source_rgb(save_ctx,
    (double)xcolor_array[layer].red/65535.0,
    (double)xcolor_array[layer].green/65535.0,
    (double)xcolor_array[layer].blue/65535.0);

  cairo_set_font_size (ctx, size*mooz);
  cairo_set_font_size (save_ctx, size*mooz);
  cairo_font_extents(ctx, &fext);
  //fprintf(errfp, "cairo_draw_string(): s=%s lines=%d\n", s, cairo_lines);
  llength=0;
  tt=ss=s;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      //fprintf(errfp, "cairo_draw_string(): tt=%s, longest line: %d\n", tt, cairo_longest_line);
      cairo_draw_string_line(ctx, tt, x, y, size, rot, flip, 
         lineno, fext.height, fext.ascent, fext.descent, llength);
      cairo_draw_string_line(save_ctx, tt, x, y, size, rot, flip, 
         lineno, fext.height, fext.ascent, fext.descent, llength);
      lineno++;
      if(c==0) break;
      *ss='\n';
      tt=ss+1;
      llength=0;
    } else {
      llength++;
    }
    ss++;
  }
}

// CAIRO version
void draw_temp_string(GC gctext, int what, char *s, int rot, int flip, double x, double y, 
     double xscale, double yscale)
{

 if(!has_x) return;
 if(!text_bbox(s, xscale, yscale, rot, flip, x,y, &textx1,&texty1,&textx2,&texty2)) return;
 drawtemprect(gctext,what, textx1,texty1,textx2,texty2);
}

#else // !HAS_CAIRO

// no CAIRO version
inline void draw_string(int layer, int what, char *str, int rot, int flip, 
                 double x1,double y1, double xscale, double yscale)  
{
 double a=0.0,yy;
 register double rx1=0,rx2=0,ry1=0,ry2=0;
 double curr_x1,curr_y1,curr_x2,curr_y2;
 register double zx1, invxscale;
 register int pos=0,pos2=0;
 register unsigned int cc;
 register double *char_ptr_x1,*char_ptr_y1,*char_ptr_x2,*char_ptr_y2;
 register int i,lines; 

 if(str==NULL || !has_x ) return;
 if(debug_var>=2) fprintf(errfp, "draw_string(): string=%s\n",str);
 if(xscale*FONTWIDTH*mooz<1) {
   if(debug_var>=1) fprintf(errfp, "draw_string(): xscale=%g zoom=%g \n",xscale,zoom);
   return;
 }
 else {
  text_bbox(str, xscale, yscale, rot, flip, x1,y1, &textx1,&texty1,&textx2,&texty2);
  if(!textclip(areax1,areay1,areax2,areay2,textx1,texty1,textx2,texty2)) return;
  x1=textx1;y1=texty1;
  if(rot&1) {y1=texty2;rot=3;}
  else rot=0;
  flip = 0; yy=y1;
  invxscale=1/xscale;
  while(str[pos2]) {
     cc = (unsigned int)str[pos2++];
     if(cc>127) cc= '?';
     if(cc=='\n') {
        yy+=(FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)* yscale;
        pos=0;
        a=0.0;
        continue;
     } 
     lines=character[cc][0]*4;
     char_ptr_x1=character[cc]+1;
     char_ptr_y1=character[cc]+2;
     char_ptr_x2=character[cc]+3;
     char_ptr_y2=character[cc]+4;
     zx1=a+x1*invxscale;
     for(i=0;i<lines;i+=4) {
        curr_x1 = ( char_ptr_x1[i]+ zx1 ) * xscale ;
        curr_y1 = ( char_ptr_y1[i] ) * yscale+yy;
        curr_x2 = ( char_ptr_x2[i]+ zx1 ) * xscale ;
        curr_y2 = ( char_ptr_y2[i] ) * yscale+yy;
        ROTATION(x1,y1,curr_x1,curr_y1,rx1,ry1);
        ROTATION(x1,y1,curr_x2,curr_y2,rx2,ry2);
        ORDER(rx1,ry1,rx2,ry2);
        drawline(layer, what, rx1, ry1, rx2, ry2);
     }
     pos++;
     a += FONTWIDTH+FONTWHITESPACE;
  }
 }
}

// no CAIRO version
void draw_temp_string(GC gctext, int what, char *str, int rot, int flip, 
                 double x1,double y1, double xscale, double yscale)  
{
 if(!has_x) return;
 if(debug_var>=2) fprintf(errfp, "draw_string(): string=%s\n",str);
 if(!text_bbox(str, xscale, yscale, rot, flip, x1,y1, &textx1,&texty1,&textx2,&texty2)) return;
 drawtemprect(gctext,what, textx1,texty1,textx2,texty2);
}
#endif // HAS_CAIRO

void draw_symbol_outline(int what,int c, int n,int layer,int tmp_flip, int rot, 
	double xoffset, double yoffset) 
			    // draws current layer only, should be called within 
{		            // a "for(i=0;i<cadlayers;i++)" loop
 register int j;
 register double x0,y0,x1,y1,x2,y2;
 int flip;
 Line line;
 Box box;
 Polygon polygon; // 20171115
 Text text;
 register Instdef *symptr; //20150408
 int textlayer;

 // 20171112
 #ifdef HAS_CAIRO
 char *textprop, *textfont;
 #endif
  if(!has_x) return;
  if(layer==0) {
   x1=(inst_ptr[n].x1+xoffset+xorigin)*mooz;  // 20150729 added xoffset, yoffset
   x2=(inst_ptr[n].x2+xoffset+xorigin)*mooz;
   y1=(inst_ptr[n].y1+yoffset+yorigin)*mooz;
   y2=(inst_ptr[n].y2+yoffset+yorigin)*mooz;
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2)) 
   {
    inst_ptr[n].flags|=1;
    return;
   }
   else inst_ptr[n].flags&=~1;

   // following code handles different text color for labels/pins 06112002

  } else if(inst_ptr[n].flags&1) {  
   if(debug_var>=2) fprintf(errfp, "draw_symbol_outline(): skippinginst %d\n", n);
   return;
  }

  flip = inst_ptr[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (inst_ptr[n].rot + rot ) & 0x3;
 
  x0=inst_ptr[n].x0 + xoffset;
  y0=inst_ptr[n].y0 + yoffset;
  symptr = (inst_ptr[n].ptr+instdef);
  for(j=0;j< symptr->lines[layer];j++)
  {
    line = (symptr->lineptr[layer])[j];
    ROTATION(0.0,0.0,line.x1,line.y1,x1,y1);
    ROTATION(0.0,0.0,line.x2,line.y2,x2,y2);
    ORDER(x1,y1,x2,y2);
    drawline(c,what, x0+x1, y0+y1, x0+x2, y0+y2);
  }
  for(j=0;j< symptr->polygons[layer];j++) // 20171115
  { 
    polygon = (symptr->polygonptr[layer])[j];
    {   // scope block so we declare some auxiliary arrays for coord transforms. 20171115
      int k;
      double x[polygon.points];
      double y[polygon.points];
      for(k=0;k<polygon.points;k++) {
        ROTATION(0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
        x[k]+= x0;
        y[k] += y0;
      }
      drawpolygon(c, NOW, x, y, polygon.points);
    }

  }
  for(j=0;j< symptr->rects[layer];j++)
  {
    box = (symptr->boxptr[layer])[j];
    ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
    ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
    RECTORDER(x1,y1,x2,y2); 
    drawrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2);
    filledrect(c,what, x0+x1, y0+y1, x0+x2, y0+y2); // 20070323 added fill check
  }
  if( (layer==TEXTWIRELAYER && !(inst_ptr[n].flags&2) ) || 
      (sym_txt && (layer==TEXTLAYER) && (inst_ptr[n].flags&2) ) ) {
    for(j=0;j< symptr->texts;j++)
    {
      text = symptr->txtptr[j];
      if(text.xscale*FONTWIDTH*mooz<1) continue;
      text.txt_ptr= translate(n, text.txt_ptr);
      ROTATION(0.0,0.0,text.x0,text.y0,x1,y1);

      textlayer = c;
      #ifdef HAS_CAIRO
      textprop = get_tok_value(symptr->txtptr[j].prop_ptr, "layer", 0);
      if(textprop[0]!=0) {
        textlayer = atoi(textprop);
        if(textlayer < 0 || textlayer >= cadlayers) textlayer = layer;
      }
      textfont = get_tok_value(symptr->txtptr[j].prop_ptr, "font", 0);
      if(textfont[0]) {
        cairo_save(ctx);
        cairo_save(save_ctx);
        cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_select_font_face (save_ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      }
      #endif
      draw_string(textlayer, what, text.txt_ptr,
        (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
        flip^text.flip,
        x0+x1, y0+y1, text.xscale, text.yscale);                    
      #ifdef HAS_CAIRO
      if(textfont[0]) {
        //cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        //cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_restore(ctx);
        cairo_restore(save_ctx);
      }
      #endif
    }
  }
}

void draw_temp_symbol_outline(int what, GC gc, int n,int layer,int tmp_flip, int rot,
        double xoffset, double yoffset)
                            // draws current layer only, should be called within
{                           // a "for(i=0;i<cadlayers;i++)" loop
 int j;
 double x0,y0,x1,y1,x2,y2;
 int flip;
 Line line;
 Polygon polygon;
 Box box;
 Text text;
 register Instdef *symptr; //20150408
 #ifdef HAS_CAIRO
 int customfont;
 #endif

 if(!has_x) return;
 if(layer==0) { // 20150424
   x1=(inst_ptr[n].x1+xoffset+xorigin)*mooz;  // 20150729 added xoffset, yoffset
   x2=(inst_ptr[n].x2+xoffset+xorigin)*mooz;
   y1=(inst_ptr[n].y1+yoffset+yorigin)*mooz;
   y2=(inst_ptr[n].y2+yoffset+yorigin)*mooz;
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2))
   {
    inst_ptr[n].flags|=1;
    return;
   }
   else inst_ptr[n].flags&=~1;

   // following code handles different text color for labels/pins 06112002

 } else if(inst_ptr[n].flags&1) {
   if(debug_var>=2) fprintf(errfp, "draw_symbol_outline(): skippinginst %d\n", n);
   return;
 } // /20150424

 flip = inst_ptr[n].flip;
 if(tmp_flip) flip = !flip;
 rot = (inst_ptr[n].rot + rot ) & 0x3;

 x0=inst_ptr[n].x0 + xoffset;
 y0=inst_ptr[n].y0 + yoffset;
 symptr = (inst_ptr[n].ptr+instdef);
 for(j=0;j< symptr->lines[layer];j++)
 {
  line = (symptr->lineptr[layer])[j];
  ROTATION(0.0,0.0,line.x1,line.y1,x1,y1);
  ROTATION(0.0,0.0,line.x2,line.y2,x2,y2);
  ORDER(x1,y1,x2,y2);
  drawtempline(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 for(j=0;j< symptr->polygons[layer];j++) // 20171115
 {
   //fprintf(errfp, "draw_temp_symbol_outline: polygon\n");
   polygon = (symptr->polygonptr[layer])[j];

   {   // scope block so we declare some auxiliary arrays for coord transforms. 20171115
     int k;
     double x[polygon.points];
     double y[polygon.points];
     for(k=0;k<polygon.points;k++) {
       ROTATION(0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
       x[k]+= x0;
       y[k] += y0;
     }
     drawtemppolygon(gc, NOW, x, y, polygon.points);
    }

 }
 
 for(j=0;j< symptr->rects[layer];j++)
 {
  box = (symptr->boxptr[layer])[j];
  ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
  ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
  RECTORDER(x1,y1,x2,y2);
  drawtemprect(gc,what, x0+x1, y0+y1, x0+x2, y0+y2);
 }
 if(layer==PROPERTYLAYER && sym_txt)
 {
  for(j=0;j< symptr->texts;j++)
  {
   text = symptr->txtptr[j];
   if(text.xscale*FONTWIDTH*mooz<1) continue;
   text.txt_ptr= 
     translate(n, text.txt_ptr);
   ROTATION(0.0,0.0,text.x0,text.y0,x1,y1);
   #ifdef HAS_CAIRO
   customfont = set_text_custom_font(&text);
   #endif
   draw_temp_string(gc, what, text.txt_ptr,
     (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
     flip^text.flip, x0+x1, y0+y1, text.xscale, text.yscale);                    
   #ifdef HAS_CAIRO
   if(customfont) cairo_restore(ctx);
   #endif

  }
 }
}

void drawgrid()
{
 double x,y;
 double delta,tmp;
 int i=0;
 if( !draw_grid || !has_x) return;
 delta=CADGRID*mooz;
 while(delta<CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;	// <-- to be improved,but works
 x = xorigin*mooz;y = yorigin*mooz;
 if(y>areay1 && y<areay2)
 {
  XDrawLine(display, window, gc[SELLAYER],areax1+1,(int)y, areax2-1, (int)y);
  if(draw_pixmap)
    XDrawLine(display, save_pixmap, gc[SELLAYER],areax1+1,(int)y, areax2-1, (int)y);
 }
 if(x>areax1 && x<areax2)
 {
  XDrawLine(display, window, gc[SELLAYER],(int)x,areay1+1, (int)x, areay2-1);
  if(draw_pixmap)
    XDrawLine(display, save_pixmap, gc[SELLAYER],(int)x,areay1+1, (int)x, areay2-1);
 }
 tmp = floor((areay1+1)/delta)*delta-fmod(-yorigin*mooz,delta);
 for(x=floor((areax1+1)/delta)*delta-fmod(-xorigin*mooz,delta);x<areax2;x+=delta)
 {
  for(y=tmp;y<areay2;y+=delta)
  {
   if(i>=CADMAXGRIDPOINTS)
   {
    XDrawPoints(display,window,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
    if(draw_pixmap)
      XDrawPoints(display,save_pixmap,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
    i=0;
   }
   gridpoint[i].x=(int)(x);gridpoint[i++].y=(int)(y);
  }
 }

 XDrawPoints(display,window,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
 if(draw_pixmap)
   XDrawPoints(display,save_pixmap,gc[GRIDLAYER],gridpoint,i,CoordModeOrigin);
// debug ...
// XFlush(display);
}


void drawline(int c, int what, double linex1, double liney1, double linex2, double liney2)
{
 static int i=0;
 static XSegment r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;
 register int ii;
 register XSegment *rr;

 if(!has_x) return;
 ii=i;
 rr=r;
 if(what==ADD)
 {
  if(ii>=CADDRAWBUFFERSIZE)
  {
   XDrawSegments(display, window, gc[c], rr,ii);
   if(draw_pixmap)
     XDrawSegments(display, save_pixmap, gc[c], rr,ii);
   ii=i=0;
  }
  x1=(linex1+xorigin)*mooz;
  y1=(liney1+yorigin)*mooz;
  x2=(linex2+xorigin)*mooz;
  y2=(liney2+yorigin)*mooz;
  if( clip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   rr[i].x1=(short)x1; 
   rr[i].y1=(short)y1;
   rr[i].x2=(short)x2;
   rr[i].y2=(short)y2;
   i++;
  }
 }
 else if(what==NOW)
 {
  x1=(linex1+xorigin)*mooz;
  y1=(liney1+yorigin)*mooz;
  x2=(linex2+xorigin)*mooz;
  y2=(liney2+yorigin)*mooz;
  if( clip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawLine(display, window, gc[c], x1, y1, x2, y2);
   if(draw_pixmap) 
    XDrawLine(display, save_pixmap, gc[c], x1, y1, x2, y2);
  }
 } 

 else if(what==THICK)		// 26122004
 { 
  x1=(linex1+xorigin)*mooz;
  y1=(liney1+yorigin)*mooz;
  x2=(linex2+xorigin)*mooz;
  y2=(liney2+yorigin)*mooz;
  if( clip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XSetLineAttributes (display, gc[c], BUS_WIDTH , LineSolid, CapRound , JoinRound);

   XDrawLine(display, window, gc[c], x1, y1, x2, y2);
   if(draw_pixmap) XDrawLine(display, save_pixmap, gc[c], x1, y1, x2, y2);
   XSetLineAttributes (display, gc[c], lw==1? 0:lw, LineSolid, CapRound , JoinRound);
  }
 }


 else if(what==BEGIN) i=0;
 else if(what==END)
 {
  XDrawSegments(display, window, gc[c], rr,ii);
  if(draw_pixmap)
    XDrawSegments(display, save_pixmap, gc[c], rr,ii);
  i=0;
 }
}

void drawtempline(GC gc, int what, double linex1,double liney1,double linex2,double liney2)
{
 static int i=0;
 static XSegment r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(what==ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XDrawSegments(display, window, gc, r,i);
   i=0;
  }
  x1=(linex1+xorigin)*mooz;
  y1=(liney1+yorigin)*mooz;
  x2=(linex2+xorigin)*mooz;
  y2=(liney2+yorigin)*mooz;
  if( clip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x1=(short)x1;
   r[i].y1=(short)y1;
   r[i].x2=(short)x2;
   r[i].y2=(short)y2;
   i++;
  }
 }
 else if(what==NOW)
 {
  x1=(linex1+xorigin)*mooz;
  y1=(liney1+yorigin)*mooz;
  x2=(linex2+xorigin)*mooz;
  y2=(liney2+yorigin)*mooz;
  if( clip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawLine(display, window, gc, x1, y1, x2, y2);
  }
 } 
 else if(what==THICK)           // 26122004
 {
  x1=(linex1+xorigin)*mooz;
  y1=(liney1+yorigin)*mooz;
  x2=(linex2+xorigin)*mooz;
  y2=(liney2+yorigin)*mooz;
  if( clip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   // XSetLineAttributes (display, gc, (unsigned int)(5*(lw_double+1.0)), LineSolid, CapRound , JoinRound); // 20150410
   XSetLineAttributes (display, gc, BUS_WIDTH, LineSolid, CapRound , JoinRound); // 20150410

   XDrawLine(display, window, gc, x1, y1, x2, y2);
   if(gc==gctiled) 
     XSetLineAttributes (display, gc, lw, LineSolid, CapRound , JoinRound);
   else
     XSetLineAttributes (display, gc, lw==1? 0:lw, LineSolid, CapRound , JoinRound);
  }
 }

 else if(what==BEGIN) i=0;
 else if(what==END)
 {
  XDrawSegments(display, window, gc, r,i);
  i=0;
 }
}




void filledrect(int c, int what, double rectx1,double recty1,double rectx2,double recty2)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(!fill || !fill_type[c]) return;
 if(what==NOW)
 {
  x1=(rectx1+xorigin)*mooz;
  y1=(recty1+yorigin)*mooz;
  x2=(rectx2+xorigin)*mooz;
  y2=(recty2+yorigin)*mooz;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XFillRectangle(display, window, gcstipple[c], (int)x1, (int)y1, 
    (unsigned int)x2 - (unsigned int)x1,
    (unsigned int)y2 - (unsigned int)y1);
   if(draw_pixmap)
     XFillRectangle(display, save_pixmap,gcstipple[c],  (int)x1, (int)y1, 
      (unsigned int)x2 - (unsigned int)x1,
      (unsigned int)y2 - (unsigned int)y1);
  }
 }
 else if(what==BEGIN) i=0;
 else if(what==ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XFillRectangles(display, window, gcstipple[c], r,i);
   if(draw_pixmap)
     XFillRectangles(display, save_pixmap, gcstipple[c], r,i);
   i=0;
  }
  x1=(rectx1+xorigin)*mooz;
  y1=(recty1+yorigin)*mooz;
  x2=(rectx2+xorigin)*mooz;
  y2=(recty2+yorigin)*mooz;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1; 
   r[i].y=(short)y1;
   r[i].width=(unsigned short)x2-r[i].x;
   r[i].height=(unsigned short)y2-r[i].y;
   i++;
  }
 }
 else if(what==END)
 {
  XFillRectangles(display, window, gcstipple[c], r,i);
  if(draw_pixmap)
    XFillRectangles(display, save_pixmap, gcstipple[c], r,i);
  i=0;
 }
}

void polygon_bbox(double *x, double *y, int points, double *bx1, double *by1, double *bx2, double *by2)
{
  int j;
  for(j=0; j<points; j++) {
    if(j==0 || x[j] < *bx1) *bx1 = x[j];
    if(j==0 || y[j] < *by1) *by1 = y[j];
    if(j==0 || x[j] > *bx2) *bx2 = x[j];
    if(j==0 || y[j] > *by2) *by2 = y[j];
  }
}


// Convex Nonconvex Complex
#define Polygontype Nonconvex
void drawpolygon(int c, int what, double *x, double *y, int points)
{
  double x1,y1,x2,y2;
  XPoint p[points];
  int i;
  if(!has_x) return;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=(x1+xorigin)*mooz;
  y1=(y1+yorigin)*mooz;
  x2=(x2+xorigin)*mooz;
  y2=(y2+yorigin)*mooz;
  if( !rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }

  for(i=0;i<points; i++) {
    p[i].x = (x[i] + xorigin)*mooz;
    p[i].y = (y[i] + yorigin)*mooz;
  }
  XDrawLines(display, window, gc[c], p, points, CoordModeOrigin);
  if(draw_pixmap)
    XDrawLines(display, save_pixmap, gc[c], p, points, CoordModeOrigin);
  if(!fill || !fill_type[c]) return;
  XFillPolygon(display, window, gcstipple[c], p, points, Polygontype, CoordModeOrigin);
  if(draw_pixmap)
     XFillPolygon(display, save_pixmap, gcstipple[c], p, points, Polygontype, CoordModeOrigin);
}

void drawtemppolygon(GC gc, int what, double *x, double *y, int points)
{
  double x1,y1,x2,y2;
  XPoint p[points];
  int i;
  if(!has_x) return;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=(x1+xorigin)*mooz;
  y1=(y1+yorigin)*mooz;
  x2=(x2+xorigin)*mooz;
  y2=(y2+yorigin)*mooz;
  if( !rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }
  for(i=0;i<points; i++) {
    p[i].x = (x[i] + xorigin)*mooz;
    p[i].y = (y[i] + yorigin)*mooz;
  }
  XDrawLines(display, window, gc, p, points, CoordModeOrigin);
}

void drawrect(int c, int what, double rectx1,double recty1,double rectx2,double recty2)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(what==NOW)
 {
  x1=(rectx1+xorigin)*mooz;
  y1=(recty1+yorigin)*mooz;
  x2=(rectx2+xorigin)*mooz;
  y2=(recty2+yorigin)*mooz;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawRectangle(display, window, gc[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,	// buggy, 23102001
    (unsigned int)y2 - (unsigned int)y1);	// buggy, 23102001
//    (unsigned int)(x2 - x1),			//23102001
//    (unsigned int)(y2 - y1) );			//23102001
   if(draw_pixmap)
   {
    XDrawRectangle(display, save_pixmap, gc[c], (int)x1, (int)y1,
    (unsigned int)x2 - (unsigned int)x1,	// buggy, 23102001
    (unsigned int)y2 - (unsigned int)y1);	// buggy, 23102001
//    (unsigned int)(x2 - x1),			//23102001
//    (unsigned int)(y2 - y1) );			//23102001
   }
  }
 }
 else if(what==BEGIN) i=0;
 else if(what==ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XDrawRectangles(display, window, gc[c], r,i);
   if(draw_pixmap)
     XDrawRectangles(display, save_pixmap, gc[c], r,i);
   i=0;
  }
  x1=(rectx1+xorigin)*mooz;
  y1=(recty1+yorigin)*mooz;
  x2=(rectx2+xorigin)*mooz;
  y2=(recty2+yorigin)*mooz;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1; 
   r[i].y=(short)y1;
   r[i].width=(unsigned short)x2-r[i].x;	//buggy, 23102001
   r[i].height=(unsigned short)y2-r[i].y;	//buggy, 23102001
   //r[i].width=(unsigned short)(x2-x1);		//23102001
   //r[i].height=(unsigned short)(y2-y1);		//23102001
   
   i++;
  }
 }
 else if(what==END)
 {
  XDrawRectangles(display, window, gc[c], r,i);
  if(draw_pixmap)
    XDrawRectangles(display, save_pixmap, gc[c], r,i);
  i=0;
 }
}

void drawtemprect(GC gc, int what, double rectx1,double recty1,double rectx2,double recty2)
{
 static int i=0;
 static XRectangle r[CADDRAWBUFFERSIZE];
 double x1,y1,x2,y2;

 if(!has_x) return;
 if(what==NOW)
 {
  x1=(rectx1+xorigin)*mooz;
  y1=(recty1+yorigin)*mooz;
  x2=(rectx2+xorigin)*mooz;
  y2=(recty2+yorigin)*mooz;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   XDrawRectangle(display, window, gc, (int)x1, (int)y1, 
    (unsigned int)x2 - (unsigned int)x1,	// buggy, 23102001
    (unsigned int)y2 - (unsigned int)y1);	// buggy, 23102001
//    (unsigned int)(x2 - x1),			//23102001
//    (unsigned int)(y2 - y1) );			//23102001
  }
 }
 else if(what==BEGIN) i=0;
 else if(what==ADD)
 {
  if(i>=CADDRAWBUFFERSIZE)
  {
   XDrawRectangles(display, window, gc, r,i);
   i=0;
  }
  x1=(rectx1+xorigin)*mooz;
  y1=(recty1+yorigin)*mooz;
  x2=(rectx2+xorigin)*mooz;
  y2=(recty2+yorigin)*mooz;
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   r[i].x=(short)x1; 
   r[i].y=(short)y1;
   r[i].width=(unsigned short)x2-r[i].x;	// buggy,23102001
   r[i].height=(unsigned short)y2-r[i].y;	// buggy,23102001
   //r[i].width=(unsigned short)(x2-x1);		//23102001
   //r[i].height=(unsigned short)(y2-y1);		//23102001
   i++;
  }
 }
 else if(what==END)
 {
  XDrawRectangles(display, window, gc, r,i);
  i=0;
 }
}

void draw(void)
{
 register int c,i; 
 register Instdef *symptr; // 20150408
 int textlayer;

  // 20171112
  #ifdef HAS_CAIRO
  char *textprop, *textfont;
  #endif

 rebuild_selected_array();
 if(has_x) {
    if(ui_state & SELECTION) draw_dots=0; // 20171114
    if(draw_pixmap) {
      XFillRectangle(display, save_pixmap, gc[BACKLAYER], 
             areax1, areay1, areaw, areah);// clear pixmap
    }
    XFillRectangle(display, window, gc[BACKLAYER], areax1, areay1, areaw, areah);// clear window
    if(debug_var>=1) fprintf(errfp, "draw(): window: %d %d %d %d\n",areax1, areay1, areaw, areah);
    drawgrid();
    if(!only_probes) { // 20110112
        if(draw_single_layer==-1 || draw_single_layer==WIRELAYER){ // 20151117
          drawline(WIRELAYER,BEGIN, 0.0, 0.0, 0.0, 0.0);
          filledrect(WIRELAYER, BEGIN, 0.0, 0.0, 0.0, 0.0);
          for(i=0;i<lastwire;i++)
          {
            if(get_tok_value(wire[i].prop_ptr,"bus",0)[0]) {	// 26122004
              drawline(WIRELAYER, THICK, wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2);
            }
            else
              drawline(WIRELAYER, ADD, wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2);
            if(wire[i].end1 >1 && draw_dots) { // 20150331 draw_dots
               filledrect(WIRELAYER, ADD, wire[i].x1-CADHALFDOTSIZE,wire[i].y1-CADHALFDOTSIZE,
                        wire[i].x1+CADHALFDOTSIZE,wire[i].y1+CADHALFDOTSIZE );
            }
            if(wire[i].end2 >1 && draw_dots) { // 20150331 draw_dots
               filledrect(WIRELAYER, ADD, wire[i].x2-CADHALFDOTSIZE,wire[i].y2-CADHALFDOTSIZE,
                        wire[i].x2+CADHALFDOTSIZE,wire[i].y2+CADHALFDOTSIZE );
            }
          }
          drawline(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0);
          filledrect(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0);
        }
     
        for(c=0;c<cadlayers;c++)
        {
          if(draw_single_layer!=-1 && c != draw_single_layer) continue; // 20151117
          drawline(c, BEGIN, 0.0, 0.0, 0.0, 0.0);
          drawrect(c, BEGIN, 0.0, 0.0, 0.0, 0.0);
          filledrect(c, BEGIN, 0.0, 0.0, 0.0, 0.0);
      
          for(i=0;i<lastline[c];i++) 
            drawline(c, ADD, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
          for(i=0;i<lastrect[c];i++) 
          {
            drawrect(c, ADD, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
            filledrect(c, ADD, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
          }
          for(i=0;i<lastpolygon[c];i++) {
            drawpolygon(c, NOW, polygon[c][i].x, polygon[c][i].y, polygon[c][i].points);
          }
          for(i=0;i<lastinst;i++) {
            symptr = (inst_ptr[i].ptr+instdef);
            if( c==0 || //20150408 draw_symbol_outline call is needed on layer 0 to avoid redundant work (outside check)
                symptr->lines[c] ||  // 20150408
                symptr->rects[c] ||   // 20150408
                symptr->polygons[c] ||   // 20150408
                ((c==TEXTWIRELAYER || c==TEXTLAYER) && symptr->texts)) {  // 20150408
              draw_symbol_outline(ADD, c, i,c,0,0,0.0,0.0);
            }  // 20150408
          }
      
          drawline(c, END, 0.0, 0.0, 0.0, 0.0);
          drawrect(c, END, 0.0, 0.0, 0.0, 0.0);
          filledrect(c, END, 0.0, 0.0, 0.0, 0.0);
        }
  
        if(draw_single_layer ==-1 || draw_single_layer==TEXTLAYER) { // 20151117

          #ifndef HAS_CAIRO
          drawline(TEXTLAYER,BEGIN, 0.0, 0.0, 0.0, 0.0);
          drawrect(TEXTLAYER,BEGIN, 0.0, 0.0, 0.0, 0.0);
          #endif
          for(i=0;i<lasttext;i++) 
          {
            textlayer = TEXTLAYER;
            if(debug_var >=1) fprintf(errfp, "draw(): drawing string %d = %s\n",i, textelement[i].txt_ptr);

            #ifdef HAS_CAIRO
            textprop = get_tok_value(textelement[i].prop_ptr, "layer", 0);
            if(textprop[0]!=0) {
              textlayer = atoi(textprop);
              if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
            }
            textfont = get_tok_value(textelement[i].prop_ptr, "font", 0);
            if(textfont[0]) {
              cairo_save(ctx);
              cairo_save(save_ctx);
              cairo_select_font_face (ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
              cairo_select_font_face (save_ctx, textfont, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
            }
            #endif
            draw_string(textlayer, ADD, textelement[i].txt_ptr,
              textelement[i].rot, textelement[i].flip,
              textelement[i].x0,textelement[i].y0,
              textelement[i].xscale, textelement[i].yscale); 
            #ifdef HAS_CAIRO
            if(textfont[0]) {
              // cairo_select_font_face (ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
              // cairo_select_font_face (save_ctx, cairo_font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
              cairo_restore(ctx);
              cairo_restore(save_ctx);
            }
            #endif
          }
          #ifndef HAS_CAIRO
          drawline(TEXTLAYER, END, 0.0, 0.0, 0.0, 0.0);
          drawrect(TEXTLAYER, END, 0.0, 0.0, 0.0, 0.0);
          #endif
        }
        draw_selection(gc[SELLAYER], 0);
    } // !only_probes, 20110112
    draw_hilight_net();
    if(debug_var>=1) fprintf(errfp, "draw(): lw=%d\n",lw);
 } // if(has_x)
}
