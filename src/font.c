/* File: font.c
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

void compile_font(void)
{
 int code, i;
 static char *name=NULL; /* 20161122 overflow safe */

 currentsch = 0;
 my_strdup(&name, tclgetvar("XSCHEM_SHAREDIR"));
 my_strcat(&name, "/systemlib/font.sch");
 remove_symbols();
 load_schematic(1,name,1);
 for(code=0;code<127;code++)
 {
  unselect_all();
  select_inside(code*FONTOFFSET-1,-FONTHEIGHT-1,
                 code*FONTOFFSET+FONTWIDTH+1,FONTDESCENT+1, 1);
  rebuild_selected_array();
  character[code] = my_calloc(lastselected*4+1, sizeof(double));
  character[code][0] = (double)lastselected;
  if(debug_var >=2) fprintf(errfp, "compile_font(): character[%d][]={%.16g",code,character[code][0]);
  for(i=0;i<lastselected;i++)
  {
   character[code][i*4+1] = 
      line[selectedgroup[i].col][selectedgroup[i].n].x1-code*FONTOFFSET;
   character[code][i*4+2] = 
      line[selectedgroup[i].col][selectedgroup[i].n].y1+FONTHEIGHT;
   character[code][i*4+3] = 
      line[selectedgroup[i].col][selectedgroup[i].n].x2-code*FONTOFFSET;
   character[code][i*4+4] = 
      line[selectedgroup[i].col][selectedgroup[i].n].y2+FONTHEIGHT;
   if(debug_var>=2) fprintf(errfp, ",\n%.16g,%.16g,%.16g,%.16g",
    character[code][i*4+1],character[code][i*4+2],
    character[code][i*4+3],character[code][i*4+4]);
  }
  if(debug_var>=2) fprintf(errfp, "};\n");
 }
 clear_drawing();
 unselect_all();
 currentsch = 0;
 my_strncpy(schematic[currentsch], "", S(schematic[currentsch]));
}

