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
#define MAXDIST 1.0e100
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
 distance = MAXDIST;
 find_closest_line(mx,my);
 find_closest_box(mx,my);
 find_closest_text(mx,my);
 find_closest_net(mx,my);
 find_closest_element(mx,my);
 return sel;    //sel.type = 0 if nothing found
}


