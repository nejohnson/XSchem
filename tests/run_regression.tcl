#
#  File: run_regression.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2020 Stefan Frederik Schippers
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

set wcases [list "open_close"]
set tcases [list "create_save"]
set log_fn "results.txt"

proc summarize_all {fn fd} {
  puts $fd "$fn"
  set b [catch "open \"$fn\" r" fdread]
  set num_fail 0
  if (!$b) {
    while {[gets $fdread line] >=0} {
      if { [regexp {[FAIL]$} $line] || [regexp {[GOLD\?]$} $line]} { 
        puts $fd $line 
        incr num_fail
      } 
    } 
    puts $fd "Total num fail: $num_fail"
    close $fdread
  } else {
    puts $fd "Couldn't open $fn to read"
  }
}

set a [catch "open \"$log_fn\" w" fd]
if {!$a} {
foreach tc $tcases {
    puts "Start source ${tc}.tcl"
    eval exec {tclsh ${tc}.tcl}
    summarize_all ${tc}.log $fd
    puts "Finish source ${tc}.tcl"
  }
  foreach wc $wcases {
    puts "Start wish ${wc}.tcl"
    eval exec {wish ${wc}.tcl}
    summarize_all ${wc}.log $fd
    puts "Finish wish ${wc}.tcl"
  }
  close $fd
} else {
  puts "Couldn't open $log_fn to write.  Investigate please."
}