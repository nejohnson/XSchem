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
static char svg_font_family[80] = "Sans-Serif"; /* Serif, Monospace, Helvetica, Arial */
static char svg_font_style[80] = "normal"; /* normal, italic, oblique */
static double svg_linew;      /* current width of lines / rectangles */

static void svg_restore_lw(void)
{
   svg_linew = xctx->lw*1.2;
}

static void svg_xdrawline(int layer, int bus, double x1, double y1, double x2, double y2, int dash)
{
 fprintf(fd,"<path class=\"l%d\" ", layer);
 if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
 if(bus) fprintf(fd, "style=\"stroke-width:%g;\" ", BUS_WIDTH * svg_linew);
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
  if( !rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }
  fprintf(fd, "<path class=\"l%d\" ", c);
  if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
  if(!fill) {
    fprintf(fd,"style=\"fill:none;\" ");
  }
  fprintf(fd, "d=\"");
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
  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
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

  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
    fprintf(fd, "<circle class=\"l%d\" cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n", gc, xx, yy, rr);
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

  if( rectclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,&x1,&y1,&x2,&y2) )
  {
    if(b == 360.) {
      fprintf(fd, "<circle class=\"l%d\" cx=\"%g\" cy=\"%g\" r=\"%g\" ", gc, xx, yy, rr);
      if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
      if(!fillarc) fprintf(fd, "style=\"fill:none;\"");
      fprintf(fd, "/>\n");
    } else {
      xx1 = rr * cos(a * XSCH_PI / 180.) + xx;
      yy1 = -rr * sin(a * XSCH_PI / 180.) + yy;
      xx2 = rr * cos((a + b) * XSCH_PI / 180.) + xx;
      yy2 = -rr * sin((a + b) * XSCH_PI / 180.) + yy;
      fa = b > 180 ? 1 : 0;
      fs = b > 0 ? 0 : 1;

      fprintf(fd,"<path class=\"l%d\" ", gc);
      if(dash) fprintf(fd, "stroke-dasharray=\"%g,%g\" ", 1.4*dash/xctx->zoom, 1.4*dash/xctx->zoom);
      if(!fillarc) {
        fprintf(fd,"style=\"fill:none;\" ");
        fprintf(fd, "d=\"M%g %g A%g %g 0 %d %d %g %g\"/>\n", xx1, yy1, rr, rr, fa, fs, xx2, yy2);
      } else {
        fprintf(fd, "d=\"M%g %g A%g %g 0 %d %d %g %gL%g %gz\"/>\n", xx1, yy1, rr, rr, fa, fs, xx2, yy2, xx, yy);
     }
    }
  }
}

static void svg_drawline(int gc, int bus, double linex1,double liney1,double linex2,double liney2, int dash)
{
 double x1,y1,x2,y2;

  x1=X_TO_SVG(linex1);
  y1=Y_TO_SVG(liney1);
  x2=X_TO_SVG(linex2);
  y2=Y_TO_SVG(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   svg_xdrawline(gc, bus, x1, y1, x2, y2, dash);
  }
}

static void svg_draw_string_line(int layer, char *s, double x, double y, double size,
            short rot, short flip, int lineno, double fontheight, double fontascent,
            double fontdescent, int llength, int no_of_lines, int longest_line)
{
  double ix, iy;
  short rot1;
  int line_delta;
  double lines;
  char col[20];
  if(color_ps) 
    my_snprintf(col, S(col), "#%02x%02x%02x",
      svg_colors[layer].red, svg_colors[layer].green, svg_colors[layer].blue);
  else if(dark_colorscheme)
    my_snprintf(col, S(col), "#%02x%02x%02x", 255, 255, 255);
  else
    my_snprintf(col, S(col), "#%02x%02x%02x", 0, 0, 0);
  if(s==NULL) return;
  if(llength==0) return;
  
  line_delta = lineno*fontheight;
  lines = (no_of_lines-1)*fontheight;
  
  ix=X_TO_SVG(x);
  iy=Y_TO_SVG(y);
  if(rot&1) {
    rot1=3;
  } else rot1=0;
  
  if(     rot==0 && flip==0) {iy+=line_delta+fontascent;}
  else if(rot==1 && flip==0) {iy+=longest_line;ix=ix-fontheight+fontascent-lines+line_delta;}
  else if(rot==2 && flip==0) {iy=iy-fontheight-lines+line_delta+fontascent; ix=ix-longest_line;}
  else if(rot==3 && flip==0) {ix+=line_delta+fontascent;}
  else if(rot==0 && flip==1) {ix=ix-longest_line;iy+=line_delta+fontascent;}
  else if(rot==1 && flip==1) {ix=ix-fontheight+line_delta-lines+fontascent;}
  else if(rot==2 && flip==1) {iy=iy-fontheight-lines+line_delta+fontascent;}
  else if(rot==3 && flip==1) {iy=iy+longest_line;ix+=line_delta+fontascent;}
  
  fprintf(fd,"<text fill=\"%s\"  xml:space=\"preserve\" font-size=\"%g\" ", col, size*xctx->mooz);
  if(strcmp(svg_font_weight, "normal")) fprintf(fd, "font-weight=\"%s\" ", svg_font_weight);
  if(strcmp(svg_font_style, "normal")) fprintf(fd, "font-style=\"%s\" ", svg_font_style);
  if(strcmp(svg_font_family, svg_font_name)) fprintf(fd, "style=\"font-family:%s;\" ", svg_font_family);
  if(rot1) fprintf(fd, "transform=\"translate(%g, %g) rotate(%d)\" ", ix, iy, rot1*90);
  else fprintf(fd, "transform=\"translate(%g, %g)\" ", ix, iy);
  fprintf(fd, ">");
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

static void svg_draw_string(int layer, const char *str, short rot, short flip, int hcenter, int vcenter,
                 double x,double y, double xscale, double yscale)
{
  char *tt, *ss, *sss=NULL;
  double textx1,textx2,texty1,texty2;
  char c;
  int lineno=0;
  double size, height, ascent, descent;
  int llength=0, no_of_lines, longest_line;

  if(str==NULL) return;
  size = xscale*52.;
  height =  size*xctx->mooz * 1.147;
  ascent =  size*xctx->mooz * 0.908;
  descent = size*xctx->mooz * 0.219;
  text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y,
            &textx1,&texty1,&textx2,&texty2, &no_of_lines, &longest_line);
  if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,textx1,texty1,textx2,texty2)) {
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
  my_strdup2(465, &sss, str);
  tt=ss=sss;
  for(;;) {
    c=*ss;
    if(c=='\n' || c==0) {
      *ss='\0';
      svg_draw_string_line(layer, tt, x, y, size, rot, flip, lineno, height,
               ascent, descent, llength, no_of_lines, longest_line);
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
  my_free(1154, &sss);
}



static void old_svg_draw_string(int layer, const char *str,
                 short rot, short flip, int hcenter, int vcenter,
                 double x,double y,
                 double xscale, double yscale)
{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i, no_of_lines, longest_line;

 if(str==NULL) return;
 #if HAS_CAIRO==1
 text_bbox_nocairo(str, xscale, yscale, rot, flip, hcenter, vcenter,
                   x,y, &rx1,&ry1,&rx2,&ry2, &no_of_lines, &longest_line);
 #else
 text_bbox(str, xscale, yscale, rot, flip, hcenter, vcenter, x,y,
           &rx1,&ry1,&rx2,&ry2, &no_of_lines, &longest_line);
 #endif
 xscale*=nocairo_font_xscale;
 yscale*=nocairo_font_yscale;
 if(!textclip(xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2,rx1,ry1,rx2,ry2)) return;
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
   svg_drawline(layer, 0, rx1, ry1, rx2, ry2, 0);
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
 if(y>xctx->areay1 && y<xctx->areay2)
 {
  svg_xdrawline(GRIDLAYER, 0, xctx->areax1+1,(int)y, xctx->areax2-1, (int)y, 0);
 }
 if(x>xctx->areax1 && x<xctx->areax2)
 {
  svg_xdrawline(GRIDLAYER, 0, (int)x,xctx->areay1+1, (int)x, xctx->areay2-1, 0);
 }
 tmp = floor((xctx->areay1+1)/delta)*delta-fmod(-xctx->yorigin* xctx->mooz,delta);
 for(x=floor((xctx->areax1+1)/delta)*delta-fmod(-xctx->xorigin* xctx->mooz,delta);x<xctx->areax2;x+=delta)
 {
  for(y=tmp;y<xctx->areay2;y+=delta)
  {
   svg_xdrawpoint(GRIDLAYER, (int)(x), (int)(y));
  }
 }
}



static void svg_draw_symbol(int c, int n,int layer,short tmp_flip, short rot,
        double xoffset, double yoffset)
                            /* draws current layer only, should be called within  */
{                           /* a "for(i=0;i<cadlayers;i++)" loop */
  int j;
  double x0,y0,x1,y1,x2,y2;
  short flip;
  int textlayer;
  xLine line;
  xRect box;
  xText text;
  xArc arc;
  xPoly polygon;
  xSymbol *symptr;
  char *textfont;

  if(xctx->inst[n].ptr == -1) return;
  if( (layer != PINLAYER && !enable_layer[layer]) ) return;
  if(layer==0) {
    x1=X_TO_SVG(xctx->inst[n].x1);
    x2=X_TO_SVG(xctx->inst[n].x2);
    y1=Y_TO_SVG(xctx->inst[n].y1);
    y2=Y_TO_SVG(xctx->inst[n].y2);
    if(OUTSIDE(x1,y1,x2,y2,xctx->areax1,xctx->areay1,xctx->areax2,xctx->areay2)) {
      xctx->inst[n].flags|=1;
      return;
    }
    else xctx->inst[n].flags&=~1;
  }
  else if(xctx->inst[n].flags&1) {
    dbg(1, "draw_symbol(): skipping inst %d\n", n);
    return;
  }
  flip = xctx->inst[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (xctx->inst[n].rot + rot ) & 0x3;
  x0=xctx->inst[n].x0 + xoffset;
  y0=xctx->inst[n].y0 + yoffset;
  symptr = (xctx->inst[n].ptr+ xctx->sym);
  for(j=0;j< symptr->lines[layer];j++) {
    line = (symptr->line[layer])[j];
    ROTATION(rot, flip, 0.0,0.0,line.x1,line.y1,x1,y1);
    ROTATION(rot, flip, 0.0,0.0,line.x2,line.y2,x2,y2);
    ORDER(x1,y1,x2,y2);
    svg_drawline(c, line.bus, x0+x1, y0+y1, x0+x2, y0+y2, line.dash);
  }
  for(j=0;j< symptr->polygons[layer];j++) {
    polygon = (symptr->poly[layer])[j];
    { /* scope block so we declare some auxiliary arrays for coord transforms. 20171115 */
      int k;
      double *x = my_malloc(417, sizeof(double) * polygon.points);
      double *y = my_malloc(418, sizeof(double) * polygon.points);
      for(k=0;k<polygon.points;k++) {
        ROTATION(rot, flip, 0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
        x[k]+= x0;
        y[k] += y0;
      }
      svg_drawpolygon(c, NOW, x, y, polygon.points, polygon.fill, polygon.dash);
      my_free(961, &x);
      my_free(962, &y);
    }
  }
  for(j=0;j< symptr->arcs[layer];j++) {
    double angle;
    arc = (symptr->arc[layer])[j];
    if(flip) {
      angle = 270.*rot+180.-arc.b-arc.a;
    } else {
      angle = arc.a+rot*270.;
    }
    angle = fmod(angle, 360.);
    if(angle<0.) angle+=360.;
    ROTATION(rot, flip, 0.0,0.0,arc.x,arc.y,x1,y1);
    svg_drawarc(c, arc.fill, x0+x1, y0+y1, arc.r, angle, arc.b, arc.dash);
  }

  if( enable_layer[layer] ) for(j=0;j< symptr->rects[layer];j++) {
    box = (symptr->rect[layer])[j];
    ROTATION(rot, flip, 0.0,0.0,box.x1,box.y1,x1,y1);
    ROTATION(rot, flip, 0.0,0.0,box.x2,box.y2,x2,y2);
    RECTORDER(x1,y1,x2,y2);
    svg_filledrect(c, x0+x1, y0+y1, x0+x2, y0+y2, box.dash);
  }
  if( (layer==TEXTWIRELAYER  && !(xctx->inst[n].flags&2) ) ||
      (sym_txt && (layer==TEXTLAYER)   && (xctx->inst[n].flags&2) ) ) {
    const char *txtptr;
    for(j=0;j< symptr->texts;j++) {
      text = symptr->text[j];
      /* if(text.xscale*FONTWIDTH* xctx->mooz<1) continue; */
      txtptr= translate(n, text.txt_ptr);
      ROTATION(rot, flip, 0.0,0.0,text.x0,text.y0,x1,y1);
      textlayer = c;
      /* do not allow custom text color on PINLAYER hilighted instances */
      if( !(xctx->inst[n].color == PINLAYER)) {
        textlayer = symptr->text[j].layer;
        if(textlayer < 0 || textlayer >= cadlayers) textlayer = c;
      }
      /* display PINLAYER colored instance texts even if PINLAYER disabled */
      if(xctx->inst[n].color == PINLAYER ||  enable_layer[textlayer]) {
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
  static char lastdir[PATH_MAX] = "";
  const char *r, *textfont;
  int *unused_layer;
  int color;
  struct hilight_hashentry *entry;
  
  if(!lastdir[0]) my_strncpy(lastdir, pwd_dir, S(lastdir));
  if(!plotfile[0]) {
    Tcl_VarEval(interp, "tk_getSaveFile -title {Select destination file} -initialfile ",
      get_cell(xctx->sch[xctx->currsch], 0) , ".svg -initialdir ", lastdir, NULL);
    r = tclresult();
    if(r[0]) {
      my_strncpy(plotfile, r, S(plotfile));
      Tcl_VarEval(interp, "file dirname ", plotfile, NULL);
      my_strncpy(lastdir, tclresult(), S(lastdir));
    }
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
  dx=xctx->xschem_w;
  dy=xctx->xschem_h;
  dbg(1, "svg_draw(): dx=%g  dy=%g\n", dx, dy);
 
  if(plotfile[0]) {
    fd=fopen(plotfile, "w");
    if(!fd) { 
      dbg(0, "can not open file: %s\n", plotfile);
      return;
    }
  } else {
    fd=fopen("plot.svg", "w");
    if(!fd) { 
      dbg(0, "can not open file: %s\n", "plot.svg");
      return;
    }
  }
  my_strncpy(plotfile,"", S(plotfile));

  unused_layer = my_calloc(873, cadlayers, sizeof(int));
  #if 0
  /* Determine used layers. Disabled since we want hilight colors */
  for(c=0;c<cadlayers;c++) unused_layer[c] = 1;
  unused_layer[0] = 0; /* background */
  for(i=0;i<xctx->texts;i++)
  {
    textlayer = xctx->text[i].layer;
    if(textlayer < 0 ||  textlayer >= cadlayers) textlayer = TEXTLAYER;
    unused_layer[textlayer] = 0;
  }
  for(c=0;c<cadlayers;c++)
  {
    xSymbol symptr = (xctx->inst[i].ptr + xctx->sym);
    if(xctx->lines[c] || xctx->rects[c] || xctx->arcs[c] || xctx->polygons[c]) unused_layer[c] = 0;
    if(xctx->wires) unused_layer[WIRELAYER] = 0;
    for(i=0;i<xctx->instances;i++) {
      if( (c == PINLAYER || enable_layer[c]) && symptr->lines[c] ) unused_layer[c] = 0;
      if( (c == PINLAYER || enable_layer[c]) && symptr->polygons[c] ) unused_layer[c] = 0;
      if( (c == PINLAYER || enable_layer[c]) && symptr->arcs[c] ) unused_layer[c] = 0;
      if( (c != PINLAYER || enable_layer[c]) && symptr->rects[c] ) unused_layer[c] = 0;
      if( (c==TEXTWIRELAYER  && !(xctx->inst[i].flags&2) ) ||
          (sym_txt && (c==TEXTLAYER)   && (xctx->inst[i].flags&2) ) )
      {
        int j;
        for(j=0;j< symptr->texts;j++)
        {
          textlayer = c;
          if( !(xctx->inst[i].color == PINLAYER)) {
            textlayer = symptr->text[j].layer;
            if(textlayer < 0 || textlayer >= cadlayers) textlayer = c;
          }
          /* display PINLAYER colored instance texts even if PINLAYER disabled */
          if(xctx->inst[i].color == PINLAYER ||  enable_layer[textlayer]) {
            used_layer[textlayer] = 0;
          }
        }
      }
    }
    dbg(1, "used_layer[%d] = %d\n", c, used_layer[c]);
  }
  /* End determine used layer */
  #endif

  fprintf(fd, "<svg xmlns=\"http://www.w3.org/2000/svg\""
              " width=\"%g\" height=\"%g\" version=\"1.1\">\n", dx, dy);
 
  fprintf(fd, "<style type=\"text/css\">\n");  /* use css stylesheet 20121119 */
  for(i=0;i<cadlayers;i++){
    if(unused_layer[i]) continue;
    fprintf(fd, ".l%d{\n", i);
    if(fill_type[i] == 1) 
      fprintf(fd, " fill: #%02x%02x%02x;\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
    else if(fill_type[i] == 2) 
      fprintf(fd, " fill: #%02x%02x%02x; fill-opacity: 0.5;\n", 
         svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
    else
       fprintf(fd, "  fill: none;\n");
    fprintf(fd, "  stroke: #%02x%02x%02x;\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
    fprintf(fd, "  stroke-linecap:round;\n");
    fprintf(fd, "  stroke-linejoin:round;\n");
    fprintf(fd, "  stroke-width: %g;\n", svg_linew);
    if(i == 0 && transparent_svg) {
      fprintf(fd, "  fill-opacity: 0;\n");
      fprintf(fd, "  stroke-opacity: 0;\n");
    }
    fprintf(fd, "}\n");
  }
 
  fprintf(fd, "text {font-family: %s;}\n", svg_font_name);
  fprintf(fd, "</style>\n");
 
    /* background */
    fprintf(fd, "<rect class=\"l0\" x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\"/>\n", 0.0, 0.0, dx, dy);
    svg_drawgrid();
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
     for(i=0;i<xctx->lines[c];i++)
      svg_drawline(c, xctx->line[c][i].bus, xctx->line[c][i].x1, xctx->line[c][i].y1,
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
       color = c;
       if(xctx->inst[i].color != -10000) color = get_color(xctx->inst[i].color);
       svg_draw_symbol(color,i,c,0,0,0.0,0.0);
     }
    }
    prepare_netlist_structs(0); /* NEEDED: data was cleared by trim_wires() */
    for(i=0;i<xctx->wires;i++)
    {
      color = WIRELAYER;
      if(xctx->hilight_nets && (entry=bus_hilight_lookup( xctx->wire[i].node, 0, XLOOKUP))) {
        color = get_color(entry->value);
      }
      svg_drawline(color, xctx->wire[i].bus, xctx->wire[i].x1, 
       xctx->wire[i].y1,xctx->wire[i].x2,xctx->wire[i].y2, 0);
    }
    {
      double x1, y1, x2, y2;
      struct wireentry *wireptr;
      int i;
      struct iterator_ctx ctx;
      update_conn_cues(0, 0);
      /* draw connecting dots */
      x1 = X_TO_XSCHEM(xctx->areax1);
      y1 = Y_TO_XSCHEM(xctx->areay1);
      x2 = X_TO_XSCHEM(xctx->areax2);
      y2 = Y_TO_XSCHEM(xctx->areay2);
      for(init_wire_iterator(&ctx, x1, y1, x2, y2); ( wireptr = wire_iterator_next(&ctx) ) ;) {
        i = wireptr->n;
        color = WIRELAYER;
        if(xctx->hilight_nets && (entry=bus_hilight_lookup( xctx->wire[i].node, 0, XLOOKUP))) {
          color = get_color(entry->value);
        }
        if( xctx->wire[i].end1 >1 ) { /* 20150331 draw_dots */
          svg_drawcircle(color, 1, xctx->wire[i].x1, xctx->wire[i].y1, cadhalfdotsize, 0, 360);
        }
        if( xctx->wire[i].end2 >1 ) { /* 20150331 draw_dots */
          svg_drawcircle(color, 1, xctx->wire[i].x2, xctx->wire[i].y2, cadhalfdotsize, 0, 360);
        }
      }
    }
  dbg(1, "svg_draw(): INT_WIDTH(lw)=%d\n",INT_WIDTH(xctx->lw));
  fprintf(fd, "</svg>\n");
  fclose(fd);

  draw_grid=old_grid;
  my_free(964, &svg_colors);
  my_free(1217, &unused_layer);
  Tcl_SetResult(interp,"",TCL_STATIC);
}

