#!/usr/bin/awk  -f
#
#  File: break.awk
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2016 Stefan Frederik Schippers
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#break VERY long lines

BEGIN{ quote=0 }

{
 pos=0
 if(NF==0) print ""
 first = substr($0,1,1)
 # 20151203 faster executionif no {}' present
 if($0 ~/[{}']/ || quote) {
   l = length($0)
   for(i=1;i<=l;i++) {
     pos++
     c = substr($0,i,1)
     if(c ~/[{}']/) quote=!quote 
     if(pos> 100 && !quote && (c ~/[ \t]/)) {
       if(first=="*") 
         c = "\n*+" c
       else
         c = "\n+" c
       pos=0
     }
     printf "%s",c
   }
   printf "\n" 
 } else {  #20151203
   for(i=1;i<=NF;i++) {
     pos += length($i)+1
     if(pos>100) {
       if(first=="*") {
         printf "%s", "\n*+"
       } else {
         printf "%s", "\n+"
       }
       pos=0
     } else if(i>1) {
       printf " "
     }
     printf "%s", $i
   }
   printf "\n"
 }
}
