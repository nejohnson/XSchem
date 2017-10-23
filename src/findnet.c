/* File: findnet.c
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
#include <float.h>
static double distance;
static Selected sel;

void find_closest_net(double mx,double my)
// returns the net that is closest to the mouse pointer
// if there are nets and distance < CADWIREMINDIST
{
 double tmp;
 static double threshold = CADWIREMINDIST * CADWIREMINDIST;
 int i,w=-1;

 for(i=0;i<lastwire;i++)
 {
  if( (tmp = dist(wire[i].x1,wire[i].y1,wire[i].x2,wire[i].y2,mx,my)) < distance )
  {
   w = i; distance = tmp;
  }
 }
 if( distance <= threshold && w!=-1)
 {
  sel.n = w; sel.type = WIRE;
 }
}

void find_closest_line(double mx,double my)
// returns the line that is closest to the mouse pointer
// if there are lines and distance < CADWIREMINDIST
{
 double tmp;
 static double threshold = CADWIREMINDIST * CADWIREMINDIST;
 int i,c,l=-1;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastline[c];i++)
  {
   if( (tmp = dist(line[c][i].x1,line[c][i].y1,line[c][i].x2,line[c][i].y2,mx,my)) 
         < distance )
   {
    l = i; distance = tmp;sel.col = c;
    if(debug_var>=1) fprintf(errfp, "find_closest_line(): distance=%g  n=%d\n", distance, i);
   }
  } // end for i
 } // end for c
 if( distance <= threshold && l!=-1)
 {
  sel.n = l; sel.type = LINE; 
 }
}

// 20171022 snap wire to closest pin or net endpoint
void find_closest_net_or_symbol_pin(double mx,double my, double *x, double *y)
{
  int i, j, no_of_pin_rects;
  double x0, x1, x2, y0, y1, y2, xx, yy, dist, min_dist_x=0, min_dist_y=0;
  Box box;
  int rot, flip;
  static char *type=NULL;

  distance = DBL_MAX;
  for(i=0;i<lastinst;i++) {
    x0=inst_ptr[i].x0;
    y0=inst_ptr[i].y0;
    rot = inst_ptr[i].rot;
    flip = inst_ptr[i].flip;
    my_strdup(&type,(inst_ptr[i].ptr+instdef)->type);


    no_of_pin_rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER];
    if( !strcmp(type, "label") ) no_of_pin_rects=1;
    if( !strcmp(type, "ipin") ) no_of_pin_rects=1;
    if( !strcmp(type, "opin") ) no_of_pin_rects=1;
    if( !strcmp(type, "iopin") ) no_of_pin_rects=1;
    for(j=0; j<no_of_pin_rects; j++) {
      box = ((inst_ptr[i].ptr+instdef)->boxptr[PINLAYER])[j];
      ROTATION(0.0,0.0,box.x1,box.y1,x1,y1);
      ROTATION(0.0,0.0,box.x2,box.y2,x2,y2);
      x1 += x0;
      y1 += y0;
      x2 += x0;
      y2 += y0;
      xx = (x1+x2)/2;
      yy = (y1+y2)/2;
      dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
      if(dist < distance) {
        distance = dist; 
        min_dist_x = xx;
        min_dist_y = yy;
      }
    }
  }
  for(i=0;i<lastwire; i++) {
    xx=wire[i].x1; 
    yy = wire[i].y1;
    dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
    if(dist < distance) {
      distance = dist;
      min_dist_x = xx;
      min_dist_y = yy;
    }
    xx=wire[i].x2; 
    yy = wire[i].y2;
    dist = (mx - xx) * (mx - xx) + (my - yy) * (my - yy);
    if(dist < distance) {
      distance = dist;
      min_dist_x = xx;
      min_dist_y = yy;
    }
  }
  *x = min_dist_x;
  *y = min_dist_y;
}



void find_closest_box(double mx,double my)
{
 double tmp;
 int i,c,r=-1;
 for(c=0;c<cadlayers;c++)
 {
  for(i=0;i<lastrect[c];i++)
  {
   if( POINTINSIDE(mx,my,rect[c][i].x1,rect[c][i].y1,rect[c][i].x2,rect[c][i].y2) )
   {
    tmp=dist_from_rect(mx,my,rect[c][i].x1,rect[c][i].y1,
                                  rect[c][i].x2,rect[c][i].y2);
    if(tmp < distance)
    {
     r = i; distance = tmp;sel.col = c;
    }
   }
  } // end for i
 } // end for c
 if(debug_var>=1) fprintf(errfp, "find_closest_box(): distance=%g\n", distance);
 if( r!=-1)
 {
  sel.n = r; sel.type = RECT;
 }
}

void find_closest_element(double mx,double my)
{
 double tmp;
 int i,r=-1;
 for(i=0;i<lastinst;i++)
 {
  if( POINTINSIDE(mx,my,inst_ptr[i].x1,inst_ptr[i].y1,inst_ptr[i].x2,inst_ptr[i].y2) )
  {
   tmp=pow(mx-inst_ptr[i].x0, 2)+pow(my-inst_ptr[i].y0, 2);
   if(tmp*.1 < distance)
   {
    r = i; distance = tmp;
   }
    if(debug_var>=2) fprintf(errfp, "find_closest_element(): finding closest element, lastinst=%d, dist=%g\n",i,tmp);
  }
 } // end for i
 if( r!=-1 )
 {
  sel.n = r; sel.type = ELEMENT;
 }
}

void find_closest_text(double mx,double my)
{
 int rot,flip;
 double xx1,xx2,yy1,yy2;
 static double threshold = CADWIREMINDIST * CADWIREMINDIST;
 int i,r=-1;
  for(i=0;i<lasttext;i++)
  {
   rot = textelement[i].rot;
   flip = textelement[i].flip;
   text_bbox(textelement[i].txt_ptr, 
             textelement[i].xscale, textelement[i].yscale, rot, flip,
             textelement[i].x0, textelement[i].y0,
             &xx1,&yy1, &xx2,&yy2);
   if(POINTINSIDE(mx,my,xx1,yy1, xx2, yy2))
   {
    r = i; distance = 0;  //<<<<<<<<<<<<<<<<< dist=0 !!!!!!!!
     if(debug_var>=2) fprintf(errfp, "find_closest_text(): finding closest text, lasttext=%d, dist=%g\n",i,distance);
   }
  } // end for i
 if( distance <= threshold && r!=-1)
 {
  sel.n = r; sel.type = TEXT;
 }
}               

Selected find_closest_obj(double mx,double my)
{
 sel.n = 0L; sel.col = 0; sel.type = 0;
 distance = DBL_MAX;
 find_closest_line(mx,my);
 find_closest_box(mx,my);
 find_closest_text(mx,my);
 find_closest_net(mx,my);
 find_closest_element(mx,my);
 return sel;    //sel.type = 0 if nothing found
}


