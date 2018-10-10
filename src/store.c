/* File: store.c
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


void check_wire_storage(void)
{
 if(lastwire >= max_wires)
 {
  max_wires=(1+lastwire / CADMAXWIRES)*CADMAXWIRES;
  my_realloc(&wire, sizeof(Wire)*max_wires);
 }
}

void check_selected_storage(void)
{
 if(lastselected >= max_selected)
 {
  max_selected=(1+lastselected / MAXGROUP) * MAXGROUP;
  my_realloc(&selectedgroup, sizeof(Selected)*max_selected);
 }
}

void check_text_storage(void)
{
 if(lasttext >= max_texts)
 {
  max_texts=(1 + lasttext / CADMAXTEXT) * CADMAXTEXT;
  my_realloc(&textelement, sizeof(Text)*max_texts);
 }
}

void check_symbol_storage(void)
{
 int i;
 if(lastinstdef >= max_symbols)
 {
  if(debug_var>=1) fprintf(errfp, "check_symbol_storage(): more than max_symbols, %s\n",
	schematic[currentsch] );
  max_symbols=(1 + lastinstdef / ELEMDEF) * ELEMDEF;
  my_realloc(&instdef, sizeof(Instdef)*max_symbols);
  for(i=lastinstdef;i<max_symbols;i++) {
    instdef[i].polygonptr=my_calloc(cadlayers, sizeof(Polygon *));
    if(instdef[i].polygonptr==NULL){
       fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  
    instdef[i].lineptr=my_calloc(cadlayers, sizeof(Line *));
    if(instdef[i].lineptr==NULL){
       fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  
    instdef[i].boxptr=my_calloc(cadlayers, sizeof(Line *));
    if(instdef[i].boxptr==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
   
    instdef[i].lines=my_calloc(cadlayers, sizeof(int));
    if(instdef[i].lines==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
   
    instdef[i].rects=my_calloc(cadlayers, sizeof(int));
    if(instdef[i].rects==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
    instdef[i].polygons=my_calloc(cadlayers, sizeof(int)); // 20171115
    if(instdef[i].polygons==NULL){
      fprintf(errfp, "check_symbol_storage(): calloc error\n");tcleval( "exit");
    }
  }
 }

}

void check_inst_storage(void)
{
 if(lastinst >= max_instances)
 {
  max_instances=(1 + lastinst / ELEMINST) * ELEMINST;
  my_realloc(&inst_ptr, sizeof(Instance)*max_instances);
 }
}

void check_box_storage(int c)
{
 if(lastrect[c] >= max_rects[c])
 {
  max_rects[c]=(1 + lastrect[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(&rect[c], sizeof(Box)*max_rects[c]);
 }
}

void check_line_storage(int c)
{
 if(lastline[c] >= max_lines[c])
 {
  max_lines[c]=(1 + lastline[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(&line[c], sizeof(Line)*max_lines[c]);
 }
}

void check_polygon_storage(int c) //20171115
{
 if(lastpolygon[c] >= max_polygons[c])
 {
  max_polygons[c]=(1 + lastpolygon[c] / CADMAXOBJECTS) * CADMAXOBJECTS;
  my_realloc(&polygon[c], sizeof(Polygon)*max_polygons[c]);
 }
}


void store_polygon(int pos, double *x, double *y, int points, unsigned int rectcolor, unsigned short sel, char *prop_ptr)
{
  int n, j;
  check_polygon_storage(rectcolor);
  if(pos==-1) n=lastpolygon[rectcolor];
  else
  {
   for(j=lastpolygon[rectcolor];j>pos;j--)
   {
    polygon[rectcolor][j]=polygon[rectcolor][j-1];
   }
   n=pos;
  }
  if(debug_var>=2) fprintf(errfp, "store_polygon(): storing POLYGON %d\n",n);
  
  polygon[rectcolor][n].x=NULL;
  polygon[rectcolor][n].y=NULL;
  polygon[rectcolor][n].selected_point=NULL;
  polygon[rectcolor][n].prop_ptr=NULL;
  polygon[rectcolor][n].x= my_calloc(points, sizeof(double));
  polygon[rectcolor][n].y= my_calloc(points, sizeof(double));
  polygon[rectcolor][n].selected_point= my_calloc(points, sizeof(unsigned short));
  my_strdup(&polygon[rectcolor][n].prop_ptr, prop_ptr);
  for(j=0;j<points; j++) {
    polygon[rectcolor][n].x[j] = x[j];
    polygon[rectcolor][n].y[j] = y[j];
  }
  polygon[rectcolor][n].points = points;
  polygon[rectcolor][n].sel = sel;

  /* 20181002 */
  if( !strcmp(get_tok_value(polygon[rectcolor][n].prop_ptr,"fill",0),"true") )
    polygon[rectcolor][n].fill =1;
  else
    polygon[rectcolor][n].fill =0;


  lastpolygon[rectcolor]++;
  modified=1;
}

void storeobject(int pos, double x1,double y1,double x2,double y2,
                 unsigned short type, unsigned int rectcolor,
		 unsigned short sel, char *prop_ptr)
{
 int n, j;
    if(type == LINE)
    {
     check_line_storage(rectcolor);

     if(pos==-1) n=lastline[rectcolor];
     else
     {
      for(j=lastline[rectcolor];j>pos;j--)
      {
       line[rectcolor][j]=line[rectcolor][j-1];
      }
      n=pos;
     }
     if(debug_var>=2) fprintf(errfp, "storeobject(): storing LINE %d\n",n);
     line[rectcolor][n].x1=x1;
     line[rectcolor][n].x2=x2;
     line[rectcolor][n].y1=y1;
     line[rectcolor][n].y2=y2;
     line[rectcolor][n].prop_ptr=NULL;
     my_strdup(&line[rectcolor][n].prop_ptr, prop_ptr);
     line[rectcolor][n].sel=sel;
     lastline[rectcolor]++;
     modified=1;
    }
    if(type == RECT)
    {
     check_box_storage(rectcolor);
     if(pos==-1) n=lastrect[rectcolor];
     else
     {
      for(j=lastrect[rectcolor];j>pos;j--)
      {
       rect[rectcolor][j]=rect[rectcolor][j-1];
      }
      n=pos;
     }
     if(debug_var>=2) fprintf(errfp, "storeobject(): storing LINE %d\n",n);
     rect[rectcolor][n].x1=x1;
     rect[rectcolor][n].x2=x2;
     rect[rectcolor][n].y1=y1;
     rect[rectcolor][n].y2=y2;
     rect[rectcolor][n].prop_ptr=NULL;
     my_strdup(&rect[rectcolor][n].prop_ptr, prop_ptr);
     rect[rectcolor][n].sel=sel;
     lastrect[rectcolor]++;
     modified=1;
    }
    if(type == WIRE)
    {
     check_wire_storage();
     if(pos==-1) n=lastwire;
     else
     {
      for(j=lastwire;j>pos;j--)
      {
       wire[j]=wire[j-1];
      }
      n=pos;
     }
     if(debug_var>=2) fprintf(errfp, "storeobject(): storing WIRE %d\n",n);
     wire[n].x1=x1;
     wire[n].y1=y1;
     wire[n].x2=x2;
     wire[n].y2=y2;
     wire[n].prop_ptr=NULL;
     wire[n].node=NULL;
     wire[n].end1=0;
     wire[n].end2=0;
     my_strdup(&wire[n].prop_ptr, prop_ptr);
     if(get_tok_value(wire[n].prop_ptr,"bus",0)[0]) wire[n].bus=1; // 20171201
     else wire[n].bus=0;

     wire[n].sel=sel;
     lastwire++;
     modified=1;
     prepared_hash_wires=0;
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
    }
}

void freenet_nocheck(int i)
{
 int j;
  my_strdup(&wire[i].prop_ptr, NULL);
  my_strdup(&wire[i].node, NULL);
  for(j=i+1;j<lastwire;j++)
  {
    wire[j-1] = wire[j];
    wire[j].prop_ptr=NULL;
    wire[j].node=NULL;
  } //end for j
  lastwire--;
}

