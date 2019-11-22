#
#  File: change_index.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2019 Stefan Frederik Schippers
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


# increments index of bussed label by $incr
# works if only one net label/pin  is selected
proc change_index {incr} {

  set sel [xschem selected_set]
  foreach i $sel {
    set mylabel [xschem getprop instance $i lab]
    regsub {.*\[} $mylabel {} myindex
    regsub {\].*} $myindex {} myindex
    regsub {\[.*} $mylabel {} mybasename
    if { [regexp {^[1-9][0-9]*$} $myindex] } {
      set myindex [expr $myindex + $incr]
      set mylabel "$mybasename\[$myindex\]"
      xschem setprop instance $i lab $mylabel
    }
  }
}

bind .drw + {change_index 1}
bind .drw - {change_index -1}
