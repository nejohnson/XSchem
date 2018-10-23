/* File: svgdraw.c
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

static FILE *fd; 


typedef struct {
 int red;
 int green;
 int blue;
} Svg_color;

static Svg_color *svg_colors;

static double svg_linew;      // current width of lines / rectangles
static Svg_color svg_stroke;

static void restore_lw(void) 
{
 if(lw_double==0.0)
   svg_linew=1.0;
 else
   svg_linew = lw_double;
}

static void set_svg_colors(unsigned int pixel)
{
   if(color_ps) {
     svg_stroke.red = svg_colors[pixel].red;
     svg_stroke.green = svg_colors[pixel].green;
     svg_stroke.blue = svg_colors[pixel].blue;
   } else {
     svg_stroke.red = 0;
     svg_stroke.green = 0;
     svg_stroke.blue = 0;
   }

}

int currentlayer=-1;

static void svg_xdrawline(int layer, double x1, double y1, double x2, 
                  double y2)
{

 if( currentlayer !=-1 && layer!= currentlayer)  fprintf(fd,"\"/>\n");
 if(layer!= currentlayer)  fprintf(fd,"<path class=\"l%d\" d=\"", layer);
 fprintf(fd,"M%g %gL%g %g\n", x1, y1, x2, y2);
 currentlayer = layer;
}

static void svg_xdrawpoint(int layer, double x1, double y1)
{
  if( currentlayer !=-1 && layer!= currentlayer)  fprintf(fd,"\"/>\n");
  if(layer!= currentlayer)  fprintf(fd,"<path class=\"l%d\" d=\"", layer);
 fprintf(fd,"M%g %gL%g %gL%g%gL%g %gL%g %gz\n", x1, y1, x1+1.0, y1, x1+1.0, y1+1.0, x1, y1+1.0, x1, y1);
 currentlayer = layer;
}

static void svg_xfillrectangle(int layer, double x1, double y1, double x2, double y2)
{
 if( currentlayer !=-1 && layer!= currentlayer)  fprintf(fd,"\"/>\n");
 if(layer!= currentlayer)  fprintf(fd,"<path class=\"l%d\" d=\"", layer);
 fprintf(fd,"M%g %gL%g %gL%g %gL%g %gL%g %gz\n", x1, y1, x2, y1, x2, y2, x1, y2, x1, y1);
 currentlayer = layer;
}

static void svg_drawpolygon(int c, int what, double *x, double *y, int points)
{
  double x1,y1,x2,y2;
  double xx, yy;
  int i;
  polygon_bbox(x, y, points, &x1,&y1,&x2,&y2);
  x1=X_TO_SCREEN(x1);
  y1=Y_TO_SCREEN(y1);
  x2=X_TO_SCREEN(x2);
  y2=Y_TO_SCREEN(y2);
  if( !rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) ) {
    return;
  }

 if( currentlayer !=-1 && c!= currentlayer)  fprintf(fd,"\"/>\n");
 if(c!= currentlayer)  fprintf(fd,"<path class=\"l%d\" d=\"", c);
  for(i=0;i<points; i++) {
    xx = X_TO_SCREEN(x[i]);
    yy = Y_TO_SCREEN(y[i]);
    if(i==0) fprintf(fd, "M%g %g", xx, yy);
    else fprintf(fd, "L%g %g", xx, yy);
  }
  fprintf(fd, "z\n");
  currentlayer = c;
}

static void svg_filledrect(int gc, double rectx1,double recty1,double rectx2,double recty2)
{
 double x1,y1,x2,y2;

  x1=X_TO_SCREEN(rectx1);
  y1=Y_TO_SCREEN(recty1);
  x2=X_TO_SCREEN(rectx2);
  y2=Y_TO_SCREEN(recty2);
  if( rectclip(areax1,areay1,areax2,areay2,&x1,&y1,&x2,&y2) )
  {
   svg_xfillrectangle(gc, x1,y1,x2,y2);
  }
}

static void svg_drawline(int gc, double linex1,double liney1,double linex2,double liney2)
{
 double x1,y1,x2,y2;

  x1=X_TO_SCREEN(linex1);
  y1=Y_TO_SCREEN(liney1);
  x2=X_TO_SCREEN(linex2);
  y2=Y_TO_SCREEN(liney2);
  if( clip(&x1,&y1,&x2,&y2) )
  {
   svg_xdrawline(gc, x1, y1, x2, y2);
  }
}

static void svg_draw_string(int gctext,  char *str, 
                 int rot, int flip, 
                 double x1,double y1,
                 double xscale, double yscale)  

{
 double a,yy,curr_x1,curr_y1,curr_x2,curr_y2,rx1,rx2,ry1,ry2;
 int pos=0,cc,pos2=0;
 int i; 

if(str==NULL) return;
// if(xscale*FONTWIDTH* mooz<1)
// {
//   if(debug_var>=2) fprintf(errfp, "svg_draw_string(): xscale=%g zoom=%g \n",xscale,zoom);
//  //svg_drawrect(gctext, rx1,ry1,rx2,ry2);
// }
 else
 {
  #ifdef HAS_CAIRO
  text_bbox_nocairo(str, xscale, yscale, rot, flip, x1,y1, &rx1,&ry1,&rx2,&ry2);
  #else
  text_bbox(str, xscale, yscale, rot, flip, x1,y1, &rx1,&ry1,&rx2,&ry2);
  #endif

  if(!textclip(areax1,areay1,areax2,areay2,rx1,ry1,rx2,ry2)) return;
  x1=rx1;y1=ry1;
  if(rot&1) {y1=ry2;rot=3;}
  else rot=0;
  flip = 0; yy=y1;
  while(str[pos2])
  {
   cc = (int)str[pos2++];
   if(cc=='\n') 
   {
    yy+=(FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)*
     yscale;
    pos=0;
    continue;
   } 
   a = pos*(FONTWIDTH+FONTWHITESPACE);
   for(i=0;i<character[cc][0]*4;i+=4)
   {
    curr_x1 = ( character[cc][i+1]+ a ) * xscale + x1;
    curr_y1 = ( character[cc][i+2] ) * yscale+yy;
    curr_x2 = ( character[cc][i+3]+ a ) * xscale + x1;
    curr_y2 = ( character[cc][i+4] ) * yscale+yy;
    ROTATION(x1,y1,curr_x1,curr_y1,rx1,ry1);
    ROTATION(x1,y1,curr_x2,curr_y2,rx2,ry2);
    ORDER(rx1,ry1,rx2,ry2);
    svg_drawline(gctext,  rx1, ry1, rx2, ry2);
   }
   pos++;
  }
 }
}


static void svg_drawgrid()
{
 double x,y;
 double delta,tmp;
 if(!draw_grid) return;
 delta=CADGRID* mooz;
 while(delta<CADGRIDTHRESHOLD) delta*=CADGRIDMULTIPLY;	// <-- to be improved,but works
 x = xorigin* mooz;y = yorigin* mooz;
 set_svg_colors(SELLAYER);
 if(y>areay1 && y<areay2)
 {
  svg_xdrawline(SELLAYER,areax1+1,(int)y, areax2-1, (int)y);
 }
 if(x>areax1 && x<areax2)
 {
  svg_xdrawline(SELLAYER,(int)x,areay1+1, (int)x, areay2-1);
 }
 set_svg_colors(GRIDLAYER);
 tmp = floor((areay1+1)/delta)*delta-fmod(-yorigin* mooz,delta);
 for(x=floor((areax1+1)/delta)*delta-fmod(-xorigin* mooz,delta);x<areax2;x+=delta)
 {
  for(y=tmp;y<areay2;y+=delta)
  {
   svg_xdrawpoint(GRIDLAYER,(int)(x), (int)(y));
  }
 }
}



static void svg_draw_symbol_outline(int n,int layer,int tmp_flip, int rot, 
	double xoffset, double yoffset) 
			    // draws current layer only, should be called within 
{		            // a "for(i=0;i<cadlayers;i++)" loop
 int j;
 double x0,y0,x1,y1,x2,y2;
 int flip;
 Line line;
 Box box;
 Text text;
 Polygon polygon;

  if(layer==0)
  {
   x1=X_TO_SCREEN(inst_ptr[n].x1);
   x2=X_TO_SCREEN(inst_ptr[n].x2);
   y1=Y_TO_SCREEN(inst_ptr[n].y1);
   y2=Y_TO_SCREEN(inst_ptr[n].y2);
   if(OUTSIDE(x1,y1,x2,y2,areax1,areay1,areax2,areay2))
   {
    inst_ptr[n].flags|=1;
    return;
   }
   else inst_ptr[n].flags&=~1;

   // following code handles different text color for labels/pins 06112002

  }
  else if(inst_ptr[n].flags&1)
  {
   if(debug_var>=1) fprintf(errfp, "draw_symbol_outline(): skippinginst %d\n", n);
   return;
  }

  flip = inst_ptr[n].flip;
  if(tmp_flip) flip = !flip;
  rot = (inst_ptr[n].rot + rot ) & 0x3;
 
  x0=inst_ptr[n].x0 + xoffset;
  y0=inst_ptr[n].y0 + yoffset;
   for(j=0;j< (inst_ptr[n].ptr+instdef)->lines[layer];j++)
   {
    line = ((inst_ptr[n].ptr+instdef)->lineptr[layer])[j];
    ROTATION(0.0,0.0,line.x1,line.y1,x1,y1);
    ROTATION(0.0,0.0,line.x2,line.y2,x2,y2);
    ORDER(x1,y1,x2,y2);
    svg_drawline(layer, x0+x1, y0+y1, x0+x2, y0+y2);
   }

   for(j=0;j< (inst_ptr[n].ptr+instdef)->polygons[layer];j++) // 20171115
   {
     polygon = ((inst_ptr[n].ptr+instdef)->polygonptr[layer])[j];
     {   // scope block so we declare some auxiliary arrays for coord transforms. 20171115
       int k;
       double x[polygon.points];
       double y[polygon.points];
       for(k=0;k<polygon.points;k++) {
         ROTATION(0.0,0.0,polygon.x[k],polygon.y[k],x[k],y[k]);
         x[k]+= x0;
         y[k] += y0;
       }
       svg_drawpolygon(layer, NOW, x, y, polygon.points);
     }

   }

   for(j=0;j< (inst_ptr[n].ptr+instdef)->rects[layer];j++)
   {
    box = ((inst_ptr[n].ptr+instdef)->boxptr[layer])[j];
    ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
    ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
    RECTORDER(x1,y1,x2,y2); 
    svg_filledrect(layer, x0+x1, y0+y1, x0+x2, y0+y2);
   }
   if(  (layer==TEXTWIRELAYER  && !(inst_ptr[n].flags&2) ) || 
        (sym_txt && (layer==TEXTLAYER)   && (inst_ptr[n].flags&2) ) )
   {
    for(j=0;j< (inst_ptr[n].ptr+instdef)->texts;j++)
    {
     text = (inst_ptr[n].ptr+instdef)->txtptr[j];
     // if(text.xscale*FONTWIDTH* mooz<1) continue;
     text.txt_ptr= 
       translate(n, text.txt_ptr);
     ROTATION(0.0,0.0,text.x0,text.y0,x1,y1);
     svg_draw_string(layer, text.txt_ptr,
       (text.rot + ( (flip && (text.rot & 1) ) ? rot+2 : rot) ) & 0x3,
       flip^text.flip,
       x0+x1, y0+y1, text.xscale, text.yscale);                    
    }
    restore_lw();
   }
   Tcl_SetResult(interp,"",TCL_STATIC);  //26102003

}


static void fill_svg_colors()
{
 char s[200]; // overflow safe 20161122
 unsigned int i,c;
 if(debug_var>=1) {
   tcleval( "puts $svg_colors"); 
 }
 for(i=0;i<cadlayers;i++) {
   my_snprintf(s, S(s), "lindex $svg_colors %d", i);
   tcleval( s);
   sscanf(Tcl_GetStringResult(interp),"%x", &c);
   svg_colors[i].red   = (c & 0xff0000) >> 16;
   svg_colors[i].green = (c & 0x00ff00) >> 8;
   svg_colors[i].blue  = (c & 0x0000ff);
   if(debug_var>=1) {
     fprintf(errfp, "svg_colors: %d %d %d\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue); 
   }
 }
 
}


void svg_draw(void)
{
 Box bb; 
 double dx, dy;
 int c,i; 
 int filledrect;
 int old_grid;
 int modified_save; // 20161121

 svg_colors=my_calloc(cadlayers, sizeof(Svg_color));
 if(svg_colors==NULL){
   fprintf(errfp, "svg_draw(): calloc error\n");tcleval( "exit");
 } 
 
 fill_svg_colors();

 old_grid=draw_grid;
 draw_grid=0;

 calc_drawing_bbox(&bb);
 dx=areax2-areax1;
 dy=areay2-areay1;
 if(debug_var>=1) fprintf(errfp, "svg_draw(): dx=%g  dy=%g\n", dx, dy);


 modified_save=modified;
 push_undo(); // 20161121
 trim_wires();    // 20161121 add connection boxes on wires but undo at end
 
  
 fd=fopen("plot.svg", "w");

 fprintf(fd, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%g\" height=\"%g\" version=\"1.1\">\n", dx, dy);

 fprintf(fd, "<style type=\"text/css\">\n");  // use css stylesheet 20121119
 for(i=0;i<cadlayers;i++){
   
   filledrect=0;
   if( (i==PINLAYER || i==WIRELAYER || i==4) && fill) {
     filledrect=1;
   }
   svg_linew = lw_double;
   svg_linew*=0.7;   // have a little smaller line widths
   fprintf(fd, ".l%d{\n", i);
   if(filledrect) fprintf(fd, "  fill: #%02x%02x%02x;\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
   else           fprintf(fd, "  fill: none;\n");
   fprintf(fd, "  stroke: #%02x%02x%02x;\n", svg_colors[i].red, svg_colors[i].green, svg_colors[i].blue);
   fprintf(fd, "  stroke-linecap:round;\n");
   fprintf(fd, "  stroke-linejoin:round;\n");
   fprintf(fd, "  stroke-width: %g;\n", svg_linew);
   fprintf(fd, "}\n");
 }
 fprintf(fd, "</style>\n");

 if(color_ps) {
   // black background
   fprintf(fd,"<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"rgb(%d,%d,%d)\" stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%g\" />\n",
                   0.0,      0.0,      dx,           dy,    0, 0, 0,
                                                            0, 0, 0, svg_linew);
 } else {
   // white background
   fprintf(fd,"<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"rgb(%d,%d,%d)\" stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%g\" />\n",
                   0.0,      0.0,      dx,           dy,    255, 255, 255,
                                                            255, 255, 255, svg_linew);

 }

 restore_lw();


   svg_drawgrid();
   set_svg_colors(WIRELAYER);
   for(i=0;i<lastwire;i++)
   {
      svg_drawline(WIRELAYER, wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2);
      if(wire[i].end1 >1)
       svg_filledrect(WIRELAYER, wire[i].x1-CADHALFDOTSIZE,wire[i].y1-CADHALFDOTSIZE,
                  wire[i].x1+CADHALFDOTSIZE,wire[i].y1+CADHALFDOTSIZE );
      if(wire[i].end2 >1)
       svg_filledrect(WIRELAYER, wire[i].x2-CADHALFDOTSIZE,wire[i].y2-CADHALFDOTSIZE,
                  wire[i].x2+CADHALFDOTSIZE,wire[i].y2+CADHALFDOTSIZE );
   }

   set_svg_colors(TEXTLAYER);
   for(i=0;i<lasttext;i++) 
   {
     svg_draw_string(TEXTLAYER, textelement[i].txt_ptr,
       textelement[i].rot, textelement[i].flip,
       textelement[i].x0,textelement[i].y0,
       textelement[i].xscale, textelement[i].yscale); 
   }
   restore_lw();

   for(c=0;c<cadlayers;c++)
   {
    set_svg_colors(c);
    for(i=0;i<lastpolygon[c];i++) {
      svg_drawpolygon(c, NOW, polygon[c][i].x, polygon[c][i].y, polygon[c][i].points);
    }
    for(i=0;i<lastline[c];i++) 
     svg_drawline(c, line[c][i].x1, line[c][i].y1, line[c][i].x2, line[c][i].y2);
    for(i=0;i<lastrect[c];i++) 
    {
     svg_filledrect(c, rect[c][i].x1, rect[c][i].y1, rect[c][i].x2, rect[c][i].y2);
    }
    for(i=0;i<lastinst;i++)
     svg_draw_symbol_outline(i,c,0,0,0.0,0.0);

   }
 if(debug_var>=1) fprintf(errfp, "svg_draw(): lw=%d\n",lw);
 fprintf(fd, "\"/>\n</svg>\n");
 fclose(fd);
 draw_grid=old_grid;
 my_free(&svg_colors);

 pop_undo(0); // 20161121
 modified=modified_save;  // 20161121

}

