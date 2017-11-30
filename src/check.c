/* File: check.c
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


void check_touch(int i, int j,
         unsigned short *parallel,unsigned short *breaks,
         unsigned short *broken,unsigned short *touches,
         unsigned short *included, unsigned short *includes,
         double *xt, double *yt)
{
 int touch1=0,touch2=0,touch3=0,touch4=0;
 double delta1x,delta1y,delta2x,delta2y;
 double x1,y1,x2,y2;
 x1=wire[i].x1;
 x2=wire[i].x2;
 y1=wire[i].y1;
 y2=wire[i].y2;
 delta1x = x2-x1;delta1y = y1-y2 ;
 delta2x = wire[j].x2-wire[j].x1;delta2y = wire[j].y1-wire[j].y2 ;

 *included = 0;
 *includes = 0;
 *touches = 0;
 *broken = 0; 
 *breaks = 0;
 *parallel = (delta1x*delta2y == delta2x*delta1y ? 1:0);

 // the order of the following 4 if(touch...) is not don't care !!!
 if(touch(wire[j].x1,wire[j].y1,wire[j].x2,wire[j].y2,x1,y1) )
 {
  *touches = 1;touch3 =1;
  *xt = x1; *yt = y1;
  if((*xt > wire[j].x1 && *xt < wire[j].x2)||
   (*yt > wire[j].y1 && *yt < wire[j].y2)) *breaks = 1;
 }
 if(touch(wire[j].x1,wire[j].y1,wire[j].x2,wire[j].y2,x2,y2) )
 {
  *touches = 1;touch4 =1;
  *xt = x2; *yt = y2;
  if((*xt > wire[j].x1 && *xt < wire[j].x2)||
    (*yt > wire[j].y1 && *yt < wire[j].y2)) *breaks = 1;
 }
 if(touch3 && touch4) *included = 1;
 
 if(touch(x1,y1,x2,y2,wire[j].x1,wire[j].y1) )
 {
  *touches = 1;touch1=1;
  *xt = wire[j].x1; *yt = wire[j].y1;
  if((*xt > x1 && *xt < x2)||(*yt > y1 && *yt < y2)) *broken = 1;
 }
 if(touch(x1,y1,x2,y2,wire[j].x2,wire[j].y2) )
 {
  *touches = 1;touch2=1;
  *xt = wire[j].x2; *yt = wire[j].y2;
  if((*xt > x1 && *xt < x2)||(*yt > y1 && *yt < y2)) *broken = 1;
 }
 if(touch1 && touch2) *includes = 1;
  if(debug_var>=2) fprintf(errfp, "check_touch(): xt=%.16g, yt=%.16g\n",*xt, *yt);
}

void collapse_wires(void)
// wire coordinates must be ordered.
{
 int j,i, changed;
 unsigned short parallel,breaks,broken,touches,included,includes;
 double xt=0,yt=0;

 do { 
   for(i=0;i<lastwire;i++)  wire[i].end1=wire[i].end2=0;
   changed=0;
   for(i=0;i<lastwire;i++)
   {
    for(j=i+1;j<lastwire;j++)
    {
      if(i==j) continue;
  
      check_touch(i,j, 
       &parallel,&breaks,&broken,&touches,&included,&includes, &xt,&yt);
   
      if(included) 
      {
         freenet_nocheck(i);i--;
         changed=1;
         break;
  //     return; // nothing to do, wire already present.
      }
      if(includes)
      {
         freenet_nocheck(j);
         changed=1;
         if(j<i)i--;
         j--;
         continue;
  			// new net contains net, so delete that
      }		  	// net and repeat current iteration
      if(touches)
      { 
         if(parallel)
         {
            if(wire[j].x1 == xt && wire[j].y1 == yt) // touch in x1, y1
            {
               if(wire[i].end2 == 0)  // merge wire
               {
                  changed=1;
                  wire[i].x2 = wire[j].x2;wire[i].y2 = wire[j].y2;
                  wire[i].end2=wire[j].end2;
                  freenet_nocheck(j);
                  i--;
                  if(j<i) i--;
                  break;
               }
            }
            else  // touch in x2,y2
            {
               if(wire[i].end1 == 0)  // merge wire
               {
                  changed=1;
                  wire[i].x1 = wire[j].x1;wire[i].y1 = wire[j].y1;
                  wire[i].end1=wire[j].end1;
                  freenet_nocheck(j);
                  i--;
                  if(j<i) i--;
                  break;
               }
            }
         } // end if parallel
         if(broken)
         {
           check_wire_storage();
           changed=1;
           wire[lastwire].x1=wire[i].x1;
           wire[lastwire].y1=wire[i].y1;
           wire[lastwire].end1=wire[i].end1;
           wire[lastwire].end2=1;
           wire[lastwire].x2=xt;
           wire[lastwire].y2=yt;
           wire[lastwire].sel=0;
           wire[lastwire].prop_ptr=NULL;
           my_strdup(&wire[lastwire].prop_ptr, wire[i].prop_ptr);
           wire[lastwire].node=NULL;
           my_strdup(&wire[lastwire].node, wire[i].node);
           lastwire++;
  
           wire[i].x1 = xt;
           wire[i].y1 = yt;
           wire[i].end1 = 1;
         } // end if broken
         else if(breaks) //wire[i] breaks wire[j]
         {
           changed=1;
           if(wire[i].x1==xt && wire[i].y1==yt) wire[i].end1+=1;
           else if(wire[i].x2==xt && wire[i].y2==yt) wire[i].end2+=1;
  
           check_wire_storage();
           wire[lastwire].x1=wire[j].x1;
           wire[lastwire].y1=wire[j].y1;
           wire[lastwire].end1=wire[j].end1;
           wire[lastwire].end2=1;
           wire[lastwire].x2=xt;
           wire[lastwire].y2=yt;
           wire[lastwire].sel=0;
           wire[lastwire].prop_ptr=NULL;
           my_strdup(&wire[lastwire].prop_ptr, wire[j].prop_ptr);
           wire[lastwire].node=NULL;
           my_strdup(&wire[lastwire].node, wire[j].node);
           lastwire++;
  
           wire[j].x1 = xt;
           wire[j].y1 = yt;
           wire[j].end1 = 1;
         } // end else if breaks
         else  // wire[i] touches but does not break wire[j]
         {
          if(wire[i].x1==wire[j].x1 && wire[i].y1==wire[j].y1)
            {wire[i].end1++;wire[j].end1++;}
          else if(wire[i].x1==wire[j].x2 && wire[i].y1==wire[j].y2)
            {wire[i].end1++;wire[j].end2++;}
          else if(wire[i].x2==wire[j].x1 && wire[i].y2==wire[j].y1)
            {wire[i].end2++;wire[j].end1++;}
          else 
            {wire[i].end2++;wire[j].end2++;}
         }
      } // end if touches
    } // end for j
   } // end for i 
   if(changed) {
     modified=1;
   }
  } while( changed ) ;
  draw_dots=1;
}
 

