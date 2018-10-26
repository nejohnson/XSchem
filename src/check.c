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

void trim_wires(void)
// wire coordinates must be ordered.
{
 int j,i, changed;
 unsigned short parallel,breaks,broken,touches,included,includes;
 double xt=0,yt=0;
 int loops=0;

 do { 
   loops++;
   for(i=0;i<lastwire;i++)  wire[i].end1=wire[i].end2=0;
   changed=0;
   for(i=0;i<lastwire;i++)
   {
    for(j=i+1;j<lastwire;j++)
    {
      check_touch(i,j, 
       &parallel,&breaks,&broken,&touches,&included,&includes, &xt,&yt);
   
      if(included) 
      {
         freenet_nocheck(i);
         i--;
         changed=1;
         break;
      }
      if(includes)
      {
         freenet_nocheck(j);
         changed=1;
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
                  j--;
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
                  j--;
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
           if(get_tok_value(wire[lastwire].prop_ptr,"bus",0)[0]) // 20171201
             wire[lastwire].bus=1;
           else
             wire[lastwire].bus=0;
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
           if(get_tok_value(wire[lastwire].prop_ptr,"bus",0)[0]) // 20171201
             wire[lastwire].bus=1;
           else
             wire[lastwire].bus=0;
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
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
     prepared_hash_wires=0;
   }
  } while( changed ) ;
  if(debug_var>=1) fprintf(errfp, "trim_wires:loops=%d\n", loops);
  draw_dots=1;
}
 
void break_wires_at_pins(void)
{
  int i, j, rects, rot, flip, sqx, sqy;
  struct wireentry *wptr;
  Box *rect;
  double x0, y0, rx1, ry1;

  delete_netlist_structs();
  hash_wires();
  for(i=0;i<lastinst;i++) {
   if(inst_ptr[i].ptr<0) continue;
   delete_inst_node(i);
   rects=(inst_ptr[i].ptr+instdef)->rects[PINLAYER] +
         (inst_ptr[i].ptr+instdef)->rects[GENERICLAYER];
   if( rects > 0 ) {
     for(j=0;j<rects;j++)
     {
       hash_inst_pin(i,j);
     }
   }
  }
  for(i=0;i<lastinst;i++) // ... assign node fields on all (non label) instances
  {
    if( (rects = (inst_ptr[i].ptr+instdef)->rects[PINLAYER]) > 0 )
    {
      for(j=0;j<rects;j++)
      {
        rect=(inst_ptr[i].ptr+instdef)->boxptr[PINLAYER];
        x0=(rect[j].x1+rect[j].x2)/2;
        y0=(rect[j].y1+rect[j].y2)/2;
        rot=inst_ptr[i].rot;
        flip=inst_ptr[i].flip;
        ROTATION(0.0,0.0,x0,y0,rx1,ry1);
        x0=inst_ptr[i].x0+rx1;
        y0=inst_ptr[i].y0+ry1;
        get_square(x0, y0, &sqx, &sqy);
  
        // name instance nodes that touch named nets
        wptr=wiretable[sqx][sqy];
        while(wptr) {
          if( touch(wire[wptr->n].x1, wire[wptr->n].y1,
                    wire[wptr->n].x2, wire[wptr->n].y2, x0,y0) )
          {
             if( (x0!=wire[wptr->n].x1 && x0!=wire[wptr->n].x2) ||
                 (y0!=wire[wptr->n].y1 && y0!=wire[wptr->n].y2) ) {
               // <<<<
               drawtemprect(gc[7], NOW, x0-10, y0-10, x0+10, y0+10);
             }
             
          }
          wptr=wptr->next;
        }
      }
    }
  }
}
