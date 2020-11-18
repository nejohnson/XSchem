/* File: svgdraw.c
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
#define X_TO_SVG(x) ( (x+xctx->xorigin)* xctx->mooz )
#define Y_TO_SVG(y) ( (y+xctx->yorigin)* xctx->mooz )

static FILE *fd;


typedef struct {
 int red;
 int green;
 int blue;
} Svg_color;

static Svg_color *svg_colors;
static char svg_font_weight[80] = "normal"; /* normal, bold, bolder, lighter */
static char svg_font_family[80] = "sans-serif"; /* serif, monospace, Helvetica, Arial */
static char svg_font_style[80] = "normal"; /* normal, italic, oblique */
static double svg_linew;      /* current width of lines / rectangles */
static Svg_color svg_stroke;

static void svg_restore_lw(void)
{
   svg_linew = xctx->lw*1.2;
}

static void set_svg_colors(unsigned int pixel)
{
   if(color_ps) {
     svg_stroke.red = svg_colors[pixel].red;
     svg_stroke.green = svg_colors[pixel].green;
     svg_stroke.blue = svg_colors[pixel].blue;
   } else {
     if(dark_colorscheme == 0 ) {
       svg_stroke.red = 0;
       svg_stroke.green = 0;
       svg_stroke.blue = 0;
     } else {
       svg_stroke.red = 255;
       svg_stroke.green = 255;
       svg_stroke.blue = 255;
     }
   }

}

static void svg_xdrawline(int layer, double x1, double y1, double x2, double y2, int dash)
{
 fprintf(fd,"<path class=\"l%d\" ", layer);
 if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
 fprintf(fd,"d=\"M%g %gL%g %g\"/>\n", x1, y1, x2, y2);
}

static void svg_xdrawpoint(int layer, double x1, double y1)
{
 fprintf(fd,"<path class=\"l%d\" d=\"", layer);
 fprintf(fd,"M%g %gL%g %gL%g%gL%g %gL%g %gz\"/>\n", x1, y1, x1+1.0, y1, x1+1.0, y1+1.0, x1, y1+1.0, x1, y1);
}

static void svg_xfillrectangle(int layer, double x1, double y1, double x2, double y2, int dash)
{
 fprintf(fd,"<path class=\"l%d\" ", layer);
 if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
 fprintf(fd,"d=\"M%g %gL%g %gL%g %gL%g %gL%g %gz\"/>\n", x1, y1, x2, y1, x2, y2, x1, y2, x1, y1);
}

static void svg_drawpolygon(int c, int what, double *x, double *y, int points, int fill, int dash)
{
  double x1,y1,x2,y2;
  double xx, yy;
  int i;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_SVG(x1);
  y1=Y_TO_SVG(y1);
  x2=X_TO_SVG(x2);
  y2=Y_TO_SVG(y2);
  if( !rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }
  fprintf(fd, "<path class=\"l%d\" ", c);
  if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
  if(fill) {
    /* if(fill_type[c] == 2) fprintf(fd, "fill-opacity=\"0.5\" "); */
    fprintf(fd,"style=\"fill:#%02x%02x%02x;\" d=\"", svg_stroke.red, svg_stroke.green, svg_stroke.blue);
  } else {
    fprintf(fd,"style=\"fill:none;\" d=\"");
  }
  for(i=0;i<points; i++) {
    xx = X_TO_SVG(x[i]);
    yy = Y_TO_SVG(y[i]);
    if(i==0) fprintf(fd, "M%g %g", xx, yy);
    else fprintf(fd, "L%g %g", xx, yy);
  }
  /* fprintf(fd, "z\"/>\n"); */
  fprintf(fd, "\"/>\n");
}

static void svg_filledrect(int gc, double rectx1,double recty1,double rectx2,double recty2, int dash)
{
 double x1,y1,x2,y2;

  x1=X_TO_SVG(rectx1);
  y1=Y_TO_SVG(recty1);
  x2=X_TO_SVG(rectx2);
  y2=Y_TO_SVG(recty2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   svg_xfillrectangle(gc, x1,y1,x2,y2, dash);
  }
}

static void svg_drawcircle(int gc, int fillarc, double x,double y,double r,double a, double b)
{
 double xx,yy,rr;
 double x1, y1, x2, y2;

  xx=X_TO_SVG(x);
  yy=Y_TO_SVG(y);
  rr=r*xctx->mooz;
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SVG(x1);
  y1=Y_TO_SVG(y1);
  x2=X_TO_SVG(x2);
  y2=Y_TO_SVG(y2);

  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
    fprintf(fd,
       "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" stroke=\"rgb(%d,%d,%d)\" fill=\"rgb(%d,%d,%d)\" stroke-width=\"%g\"/>\n",
       xx, yy, rr, svg_stroke.red, svg_stroke.green, svg_stroke.blue,
                   svg_stroke.red, svg_stroke.green, svg_stroke.blue, svg_linew);
  }
}

static void svg_drawarc(int gc, int fillarc, double x,double y,double r,double a, double b, int dash)
{
 double xx,yy,rr;
 double x1, y1, x2, y2;
 double xx1, yy1, xx2, yy2;
 int fs, fa;

  xx=X_TO_SVG(x);
  yy=Y_TO_SVG(y);
  rr=r*xctx->mooz;
  arc_bbox(x, y, r, a, b, &x1,&y1,&x2,&y2);
  x1=X_TO_SVG(x1);
  y1=Y_TO_SVG(y1);
  x2=X_TO_SVG(x2);
  y2=Y_TO_SVG(y2);

  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
    if(b == 360.) {
      fprintf(fd, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" ", xx, yy, rr);
      if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
      if(fillarc)
        fprintf(fd,
         "fill=\"rgb(%d,%d,%d)\" stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%g\"/>\n",
           svg_stroke.red, svg_stroke.green, svg_stroke.blue,
           svg_stroke.red, svg_stroke.green, svg_stroke.blue, svg_linew);
      else
        fprintf(fd, "stroke=\"rgb(%d,%d,%d)\" fill=\"none\" stroke-width=\"%g\"/>\n",
           svg_stroke.red, svg_stroke.green, svg_stroke.blue, svg_linew);
    } else {
      xx1 = rr * cos(a * XSCH_PI / 180.) + xx;
      yy1 = -rr * sin(a * XSCH_PI / 180.) + yy;
      xx2 = rr * cos((a + b) * XSCH_PI / 180.) + xx;
      yy2 = -rr * sin((a + b) * XSCH_PI / 180.) + yy;
      fa = b > 180 ? 1 : 0;
      fs = b > 0 ? 0 : 1;

      fprintf(fd,"<path class=\"l%d\" ", gc);
      if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
      if(fillarc)
        fprintf(fd,"style=\"fill:%02x%02x%02x;\" d=\"", svg_stroke.red, svg_stroke.green, svg_stroke.blue);
      else
        fprintf(fd,"style=\"fill:none;\" d=\"");
      fprintf(fd, "M%g %g A%g %g 0 %d %d %g %g\"/>\n", xx1, yy1, rr, rr, fa, fs, xx2, yy2);
    }
  }
}


static void svg_drawline(int gc, double linex1,double liney1,double linex2,double liney2, int dash)
{
 double x1,y1,x2,y2;

  x1=X_TO_SVG(linex1);
  y1=Y_TO_SVG(liney1);
  x2=X_TO_SVG(linex2);
  y2=Y_TO_SVG(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   svg_xdrawline(gc, x1, y1, x2, y2, dash);
  }
}

static double textx1,textx2,texty1,texty2;

static void svg_draw_string_line(int layer, char *s, double x, double y, double size, int rot, int flip,
    int lineno, double fontheight, double fontascent, double fontdescent, int llength)
{
  double ix, iy;
  int rot1;
  int line_delta;
  int line_offset;
  double lines;
  char col[20];
  my_snprintf(col, S(col), "#%02x%02x%02x",
    xcolor_array[layer].red >> 8, xcolor_array[layer].green >> 8, xcolor_array[layer].blue >> 8);
  if(s==NULL) return;
  if(llength==0) return;
  
  line_delta = lineno*fontheight;
  lines = (cairo_lines-1)*fontheight;
  line_offset=cairo_longest_line;
  
  ix=X_TO_SVG(x);
  iy=Y_TO_SVG(y);
  if(rot&1) {
    rot1=3;
  } else rot1=0;
  
  if(     rot==0 && flip==0) {iy+=line_delta+fontascent;}
  else if(rot==1 && flip==0) {iy+=line_offset;ix=ix-fontheight+fontascent-lines+line_delta;}
  else if(rot==2 && flip==0) {iy=iy-fontheight-lines+line_delta+fontascent; ix=ix-line_offset;}
  else if(rot==3 && flip==0) {ix+=line_delta+fontascent;}
  else if(rot==0 && flip==1) {ix=ix-line_offset;iy+=line_delta+fontascent;}
  else if(rot==1 && flip==1) {ix=ix-fontheight+line_delta-lines+fontascent;}
  else if(rot==2 && flip==1) {iy=iy-fontheight-lines+line_delta+fontascent;}
  else if(rot==3 && flip==1) {iy=iy+line_offset;ix+=line_delta+fontascent;}
  
  fprintf(fd,"<text fill=\"%s\"  xml:space=\"preserve\" font-family=\"%s\" font-size=\"%g\" "
    "font-weight=\"%s \" font-style=\"%s\" "
    "transform=\"translate(%g, %g) rotate(%d)\">",
     col, svg_font_family, size*xctx->mooz, svg_font_weight, svg_font_style, ix, iy, rot1*90);

  while(*s) {
    switch(*s) {
      case '<':
        fputs("&lt;", fd); 
        break;
      case '>':
        fputs("&gt;", fd); 
        break;
      case '&':
        fputs("&amp;", fd); 
        break;
      default:
       fputc(*s, fd);
    }
    s++;
  }
  fprintf(fd, "</text>\n");
}

static void svg_draw_string(int layer, const char *str, int rot, int flip, int hcenter, int vcenter,
                 double x,double y, double xscale, double yscale)
{
  char *tt, *ss, *sss=NULL;
  char c;
  int lineno=0;
  double size, height, ascent, descent;
  int llength=0;
  if(str==NULL || !has_x ) return;
  size = xscale*52.;
  if(size*xctx->mooz<3.0) return; /* too small */
  if(size*xctx->mooz>1600) return; /* too big */

  height =  size*xctx->mooz * 1.147;
  ascent =  size*xctx->mooz * 0.908;
  descent = size*xctx->mooz * 0.219;

  text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y, &textx1,&texty1,&textx2,&texty2);
  if(!textclip(areax1,areay1,areax2,areay2,textx1,texty1,textx2,texty2)) {
    return;
  }
  if(hcenter) {
    if(rot == 0 && flip == 0 ) { x=textx1;}
    if(rot == 1 && flip == 0 ) { y=texty1;}
    if(rot == 2 && flip == 0 ) { x=textx2;}
    if(rot == 3 && flip == 0 ) { y=texty2;}
    if(rot == 0 && flip == 1 ) { x=textx2;}
    if(rot == 1 && flip == 1 ) { y=texty2;}
    if(rot == 2 && flip == 1 ) { x=textx1;}
    if(rot == 3 && flip == 1 ) { y=texty1;}
  }
  if(vcenter) {
    if(rot == 0 && flip == 0 ) { y=texty1;}
    if(rot == 1 && flip == 0 ) { x=textx2;}
    if(rot == 2 && flip == 0 ) { y=texty2;}
    if(rot == 3 && flip == 0 ) { x=textx1;}
    if(rot == 0 && flip == 1 ) { y=texty1;}
    if(rot == 1 && flip == 1 ) { x=textx2;}
    if(rot == 2 && flip == 1 ) { y=texty2;}
    if(rot == 3 && flip == 1 ) { x=textx1;}
  }
  llength=0;
  my_strdup2(73, &sss, str);
  tt=ss=sss;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      svg_draw_string_line(layer, tt, x, y, size, rot, flip, lineno, height, ascent, descent, llength);
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
  my_free(1157, &sss);
}



static void old_svg_draw_string(int layer, const char *str,
                 int rot, int flip, int hcenter, int vcenter,
                 double x,double y,
                 double xscale, double yscale)
{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i;

 if(str==NULL) return;
 #ifdef HAS_CAIRO
 text_bbox_nocairo(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y, &rx1,&ry1,&rx2,&ry2);
 #else
 text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y, &rx1,&ry1,&rx2,&ry2);
 #endif
 xscale*=nocairo_font_xscale;
 yscale*=nocairo_font_yscale;
 if(!textclip(areax1,areay1,areax2,areay2,rx1,ry1,rx2,ry2)) return;
 x=rx1;y=ry1;
 if(rot&1) {y=ry2;rot=3;}
 else rot=0;
 flip = 0; yy=y;
 while(str[pos2])
 {
  cc = (unsigned char)str[pos2++];
  if(cc>127) cc= '?';
  if(cc=='\n')
  {
   yy+=(FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)* yscale;
   pos=0;
   continue;
  }
  a = pos*(FONTWIDTH+FONTWHITESPACE);
  for(i=0;i<character[cc][0]*4;i+=4)
  {
   curr_x1 = ( character[cc][i+1]+ a ) * xscale + x;
   curr_y1 = ( character[cc][i+2] ) * yscale+yy;
   curr_x2 = ( character[cc][i+3]+ a ) * xscale + x;
   curr_y2 = ( character[cc][i+4] ) * yscale+yy;
   ROTATION(rot, flip, x,y,curr_x1,curr_y1,rx1,ry1);
   ROTATION(rot, flip, x,y,curr_x2,curr_y2,rx2,ry2);
   ORDER(rx1,ry1,rx2,ry2);
   svg_drawline(layer,  rx1, ry1, rx2, ry2, 0);
  }
  pos++;
 }
}


static void svg_drawgrid()
{
 double x,y;
 double delta,tmp;
 if(!draw_grid) return;
 delta=cadgrid* xctx->mooz;
 while(delta<CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;  /* <-- to be improved,but works */
 x = xctx->xorigin* xctx->mooz;y = xctx->yorigin* xctx->mooz;
 set_svg_colors(GRIDLAYER);
 if(y>areay1 && y<areay2)
 {
  svg_xdrawline(GRIDLAYER,areax1+1,(int)y, areax2-1, (int)y, 0);
 }
 if(x>areax1 && x<areax2)
 {
  svg_xdrawline(GRIDLAYER,(int)x,areay1+1, (int)x, areay2-1, 0);
 }
 set_svg_colors(GRIDLAYER);
 tmp = floor((areay1+1)/delta)*delta-fmod(-xctx->yorigin* xctx->mooz,delta);
 for(x=floor((areax1+1)/delta)*delta-fmod(-xctx->xorigin* xctx->mooz,delta);x<areax2;x+=delta)
 {
  for(y=tmp;y<areay2;y+=delta)
  {
   svg_xdrawpoint(GRIDLAYER,(int)(x), (int)(y));
  }
 }
}



static void svg_draw_symbol(int n,int layer,int tmp_flip, int rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
 int j;
 double x0,y0,x1,y1,x2,y2;
 int flip, textlayer;
 xLine line;
 xRect box;
 xText text;
 xArc arc;
 xPoly polygon;
 xSymbol *symptr;
 char *textfont;

  if(xctx->inst[n].ptr == -1) return;
  if( (layer != PINLAYER && !enable_layer[layer]) ) return;
  if(layer==0)
  {
   x1=X_TO_SVG(xctx->inst[n].x1);
   x2=X_TO_SVG(xctx->inst[n].x2);
   y1=Y_TO_SVG(xctx->inst[n].y1);
   y2=Y_TO_SVG(xctx->inst[n].y2);
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2))
   {
    xctx->inst[n].flags|=1;
    return;
   }
   else xctx->inst[n].flags&=~1;

   /* following code handles different text color for labels/pins 06112002 */

  }
  else if(xctx->inst[n].flags&1)
  {
   dbg(1, "draw_symbol(): skippinginst %d\n", n);
   return;
  }

  flip = xctx->inst[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (xctx->inst[n].rot + rot ) & 0x3;

  x0=xctx->inst[n].x0 + xoffset;
  y0=xctx->inst[n].y0 + yoffset;
  symptr = (xctx->inst[n].ptr+ xctx->sym);
   for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->lines[layer];j++)
   {
    line = ((xctx->inst[n].ptr+ xctx->sym)->line[layer])[j];
    ROTATION(rot, flip, 0.0,0.0,line.x1,line.y1,x1,y1);
    ROTATION(rot, flip, 0.0,0.0,line.x2,line.y2,x2,y2);
    ORDER(x1,y1,x2,y2);
    svg_drawline(layer, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
   }

   for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->polygons[layer];j++)
   {
     polygon = ((xctx->inst[n].ptr+ xctx->sym)->poly[layer])[j];
     {   /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
       int k;
       double *x = my_malloc(417, sizeof(double) * polygon.points);
       double *y = my_malloc(418, sizeof(double) * polygon.points);
       for(k=0;k<polygon.points;k++) {
         ROTATION(rot, flip, 0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
         x[k]+= x0;
         y[k] += y0;
       }
       svg_drawpolygon(layer, NOW, x, y, polygon.points, polygon.fill, polygon.dash);
       my_free(961, &x);
       my_free(962, &y);
     }
   }
   for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->arcs[layer];j++)
   {
     double angle;
     arc = ((xctx->inst[n].ptr+ xctx->sym)->arc[layer])[j];
     if(flip) {
       angle = 270.*rot+180.-arc.b-arc.a;
     } else {
       angle = arc.a+rot*270.;
     }
     angle = fmod(angle, 360.);
     if(angle<0.) angle+=360.;
     ROTATION(rot, flip, 0.0,0.0,arc.x,arc.y,x1,y1);
     svg_drawarc(layer, arc.fill, x0+x1, y0+y1, arc.r, angle, arc.b, arc.dash);
   }

   if( (layer != PINLAYER || enable_layer[layer]) ) for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->rects[layer];j++)
   {
    box = ((xctx->inst[n].ptr+ xctx->sym)->rect[layer])[j];
    ROTATION(rot, flip, 0.0,0.0,box.x1,box.y1,x1,y1);
    ROTATION(rot, flip, 0.0,0.0,box.x2,box.y2,x2,y2);
    RECTORDER(x1,y1,x2,y2);
    svg_filledrect(layer, x0+x1, y0+y1, x0+x2, y0+y2, box.dash);
   }
   if(  (layer==TEXTWIRELAYER  && !(xctx->inst[n].flags&2) ) ||
        (sym_txt && (layer==TEXTLAYER)   && (xctx->inst[n].flags&2) ) )
   {
    const char *txtptr;
    for(j=0;j< (xctx->inst[n].ptr+ xctx->sym)->texts;j++)
    {
     text = (xctx->inst[n].ptr+ xctx->sym)->text[j];
     /* if(text.xscale*FONTWIDTH* xctx->mooz<1) continue; */
     txtptr= translate(n, text.txt_ptr);
     ROTATION(rot, flip, 0.0,0.0,text.x0,text.y0,x1,y1);
     textlayer = layer;
     if( !(layer == PINLAYER && (xctx->inst[n].flags & 4))) {
       textlayer = (xctx->inst[n].ptr+ xctx->sym)->text[j].layer;
       if(textlayer < 0 || textlayer >= cadlayers) textlayer = layer;
     }
     my_snprintf(svg_font_family, S(svg_font_family), svg_font_name);
     my_snprintf(svg_font_style, S(svg_font_style), "normal");
     my_snprintf(svg_font_weight, S(svg_font_weight), "normal");

     textfont = symptr->text[j].font;
     if( (textfont && textfont[0])) {
       my_snprintf(svg_font_family, S(svg_font_family), textfont);
     }


     if( symptr->text[j].flags & TEXT_BOLD)
       my_snprintf(svg_font_weight, S(svg_font_weight), "bold");
     if( symptr->text[j].flags & TEXT_ITALIC)
       my_snprintf(svg_font_style, S(svg_font_style), "italic");
     if( symptr->text[j].flags & TEXT_OBLIQUE)
       my_snprintf(svg_font_style, S(svg_font_style), "oblique");

     if((layer == PINLAYER && xctx->inst[n].flags & 4) ||  enable_layer[textlayer]) {
       if(text_svg) 
         svg_draw_string(textlayer, txtptr,
           (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
           flip^text.flip, text.hcenter, text.vcenter,
           x0+x1, y0+y1, text.xscale, text.yscale);
       else
         old_svg_draw_string(textlayer, txtptr,
           (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
           flip^text.flip, text.hcenter, text.vcenter,
           x0+x1, y0+y1, text.xscale, text.yscale);
     }
    }
   }
   Tcl_SetResult(interp,"",TCL_STATIC);

}


static void fill_svg_colors()
{
 char s[200]; /* overflow safe 20161122 */
 unsigned int i,c;
 if(debug_var>=1) {
   tcleval( "puts $svg_colors");
 }
 for(i=0;i<cadlayers;i++) {
   if(color_ps) {
     my_snprintf(s, S(s), "lindex $svg_colors %d", i);
     tcleval( s);
     sscanf(tclresult(),"%x", &c);
     svg_colors[i].red   = (c & 0xff0000) >> 16;
     svg_colors[i].green = (c & 0x00ff00) >> 8;
     svg_colors[i].blue  = (c & 0x0000ff);
   } else if(dark_colorscheme) {
     svg_colors[i].red   = 255;
     svg_colors[i].green = 255;
     svg_colors[i].blue  = 255;
   } else {
     svg_colors[i].red   = 0;
     svg_colors[i].green = 0;
     svg_colors[i].blue  = 0;
   }
   if(debug_var>=1) {
     fprintf(errfp, "svg_colors: %d %d %d\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
   }
 }

}


void svg_draw(void)
{
 double dx, dy;
 int c,i, textlayer;
 int old_grid;
 int modified_save;
 char *tmpstring=NULL;
 const char *r, *textfont;


 if(!plotfile[0]) {
   my_strdup(61, &tmpstring, "tk_getSaveFile -title {Select destination file} -initialdir [pwd]");
   tcleval(tmpstring);
   r = tclresult();
   my_free(963, &tmpstring);
   if(r[0]) my_strncpy(plotfile, r, S(plotfile));
   else return;
 }

 svg_restore_lw();

 svg_colors=my_calloc(419, cadlayers, sizeof(Svg_color));
 if(svg_colors==NULL){
   fprintf(errfp, "svg_draw(): calloc error\n");tcleval( "exit");
 }

 fill_svg_colors();

 old_grid=draw_grid;
 draw_grid=0;

 dx=areax2-areax1;
 dy=areay2-areay1;
 dbg(1, "svg_draw(): dx=%g  dy=%g\n", dx, dy);


 modified_save=modified;
 push_undo();
 trim_wires();    /* 20161121 add connection boxes on wires but undo at end */


 if(plotfile[0]) fd=fopen(plotfile, "w");
 else fd=fopen("plot.svg", "w");
 my_strncpy(plotfile,"", S(plotfile));


 fprintf(fd, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%g\" height=\"%g\" version=\"1.1\">\n", dx, dy);

 fprintf(fd, "<style type=\"text/css\">\n");  /* use css stylesheet 20121119 */
 for(i=0;i<cadlayers;i++){
   fprintf(fd, ".l%d{\n", i);
   if(fill_type[i] == 1) 
      fprintf(fd, "  fill: #%02x%02x%02x;\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
   else if(fill_type[i] == 2) 
      fprintf(fd, "  fill: #%02x%02x%02x; fill-opacity: 0.5;\n", 
        svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
   else
      fprintf(fd, "  fill: none;\n");
   fprintf(fd, "  stroke: #%02x%02x%02x;\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
   fprintf(fd, "  stroke-linecap:round;\n");
   fprintf(fd, "  stroke-linejoin:round;\n");
   fprintf(fd, "  stroke-width: %g;\n", svg_linew);
   fprintf(fd, "}\n");
 }
 fprintf(fd, "</style>\n");

 if(dark_colorscheme) {
   /* black background */
   fprintf(fd,
     "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"rgb(%d,%d,%d)\" "
     "stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%g\" />\n",
                   0.0, 0.0, dx, dy, 0, 0, 0, 0, 0, 0, svg_linew);
 } else {
   /* white background */
   fprintf(fd,
     "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"rgb(%d,%d,%d)\" "
     "stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%g\" />\n",
                   0.0, 0.0, dx, dy, 255, 255, 255, 255, 255, 255, svg_linew);

 }
   svg_drawgrid();
   set_svg_colors(TEXTLAYER);
   for(i=0;i<xctx->texts;i++)
   {
     textlayer = xctx->text[i].layer;
     if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;


     my_snprintf(svg_font_family, S(svg_font_family), svg_font_name);
     my_snprintf(svg_font_style, S(svg_font_style), "normal");
     my_snprintf(svg_font_weight, S(svg_font_weight), "normal");

     textfont = xctx->text[i].font;
     if( (textfont && textfont[0])) {
       my_snprintf(svg_font_family, S(svg_font_family), textfont);
     }
     if( xctx->text[i].flags & TEXT_BOLD)
       my_snprintf(svg_font_weight, S(svg_font_weight), "bold");
     if( xctx->text[i].flags & TEXT_ITALIC)
       my_snprintf(svg_font_style, S(svg_font_style), "italic");
     if( xctx->text[i].flags & TEXT_OBLIQUE)
       my_snprintf(svg_font_style, S(svg_font_style), "oblique");

     if(text_svg) 
       svg_draw_string(textlayer, xctx->text[i].txt_ptr,
         xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
         xctx->text[i].x0,xctx->text[i].y0,
         xctx->text[i].xscale, xctx->text[i].yscale);
     else
       old_svg_draw_string(textlayer, xctx->text[i].txt_ptr,
         xctx->text[i].rot, xctx->text[i].flip, xctx->text[i].hcenter, xctx->text[i].vcenter,
         xctx->text[i].x0,xctx->text[i].y0,
         xctx->text[i].xscale, xctx->text[i].yscale);
   }

   for(c=0;c<cadlayers;c++)
   {
    set_svg_colors(c);
    for(i=0;i<xctx->lines[c];i++)
     svg_drawline(c, xctx->line[c][i].x1, xctx->line[c][i].y1,
                     xctx->line[c][i].x2, xctx->line[c][i].y2, xctx->line[c][i].dash);
    for(i=0;i<xctx->rects[c];i++)
    {
     svg_filledrect(c, xctx->rect[c][i].x1, xctx->rect[c][i].y1,
                       xctx->rect[c][i].x2, xctx->rect[c][i].y2, xctx->rect[c][i].dash);
    }
    for(i=0;i<xctx->arcs[c];i++)
    {
      svg_drawarc(c, xctx->arc[c][i].fill, xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r,
                   xctx->arc[c][i].a, xctx->arc[c][i].b, xctx->arc[c][i].dash);
    }
    for(i=0;i<xctx->polygons[c];i++) {
      svg_drawpolygon(c, NOW, xctx->poly[c][i].x, xctx->poly[c][i].y, xctx->poly[c][i].points,
                      xctx->poly[c][i].fill, xctx->poly[c][i].dash);
    }
    for(i=0;i<xctx->instances;i++) {
      svg_draw_symbol(i,c,0,0,0.0,0.0);
    }
   }


   set_svg_colors(WIRELAYER);
   for(i=0;i<xctx->wires;i++)
   {
      svg_drawline(WIRELAYER, xctx->wire[i].x1,xctx->wire[i].y1,xctx->wire[i].x2,xctx->wire[i].y2, 0);
   }
   {
     double x1, y1, x2, y2;
     struct wireentry *wireptr;
     int i;
     update_conn_cues(0, 0);
     /* draw connecting dots */
     x1 = X_TO_XSCHEM(areax1);
     y1 = Y_TO_XSCHEM(areay1);
     x2 = X_TO_XSCHEM(areax2);
     y2 = Y_TO_XSCHEM(areay2);
     for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
       i = wireptr->n;
       if( xctx->wire[i].end1 >1 ) { /* 20150331 draw_dots */
         svg_drawcircle(WIRELAYER, 1, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360);
       }
       if( xctx->wire[i].end2 >1 ) { /* 20150331 draw_dots */
         svg_drawcircle(WIRELAYER, 1, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360);
       }
     }
   }

 dbg(1, "svg_draw(): INT_WIDTH(xctx->lw)=%d\n",INT_WIDTH(xctx->lw));
 fprintf(fd, "</svg>\n");
 fclose(fd);
 draw_grid=old_grid;
 my_free(964, &svg_colors);

 pop_undo(0);
 modified=modified_save;

}

