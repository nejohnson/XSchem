#!/usr/bin/wish
#
#  File: xschem.tcl
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

###
### set var with $val if var Not existing
###
proc set_ne { var val } {
    upvar #0 $var v
    if { ![ info exists v ] } {
      set v $val
    }
}



###
### Tk procedures
###
# tk_exec service function
  proc tk_exec_fileevent {id} {
    global tk_exec_id
    global tk_exec_pipe
    global tk_exec_data
    global tk_exec_cond

    if {[eof $tk_exec_pipe($id)]} {
        fileevent $tk_exec_pipe($id) readable ""
        set tk_exec_cond($id) 1
        #puts ">>>> eof process: $id"
        return
    }
    append tk_exec_data($id) [read $tk_exec_pipe($id) 1024]
  }

# equivalent to the 'exec' tcl function but keeps the event loop
# responding, so widgets get updated properly
# while waiting for process to end.
  proc tk_exec {args} {
    global tk_exec_id
    global tk_exec_data
    global tk_exec_cond
    global tk_exec_pipe

    if {![info exists tk_exec_id]} {
        set tk_exec_id 0
    } else {
        incr tk_exec_id
    }

    set pipe [open "|$args" r]
    set tk_exec_pipe($tk_exec_id) $pipe
    set tk_exec_data($tk_exec_id) ""
    set tk_exec_cond($tk_exec_id) 0

    set id $tk_exec_id
    fconfigure $pipe -blocking 0
    fileevent $pipe readable "tk_exec_fileevent $id"
    vwait tk_exec_cond($id)
    set data [string trimright $tk_exec_data($id) \n]

    if {[catch {close $tk_exec_pipe($id)} err]} {
        error "pipe error: $err"
    }
    unset tk_exec_pipe($id)
    unset tk_exec_data($id)
    unset tk_exec_cond($id)
    #puts ">>>> exiting process: $id"
    return $data
  }


proc netlist {source_file show netlist_file} {
 global XSCHEM_HOME_DIR XSCHEM_DESIGN_DIR env flat_netlist hspice_netlist netlist_dir
 global verilog_2001


 if [regexp {\.spice} $netlist_file ] {
   if { $hspice_netlist == 1 } {
     set hspice {-hspice}
   } else {
     set hspice {}
   }
   if $flat_netlist==0 then {
     eval exec "${XSCHEM_HOME_DIR}/spice.awk -- $hspice $netlist_dir/$source_file \
             | ${XSCHEM_HOME_DIR}/break.awk > $netlist_dir/$netlist_file"
   } else {
     eval exec "${XSCHEM_HOME_DIR}/spice.awk -- $hspice $netlist_dir/$source_file \
             | ${XSCHEM_HOME_DIR}/flatten.awk | ${XSCHEM_HOME_DIR}/break.awk > $netlist_dir/$netlist_file"
   }
   if ![string compare $show "show"] {
      textwindow $netlist_dir/$netlist_file
   }
 } 
 if [regexp {\.vhdl} $netlist_file ] {
   eval exec "${XSCHEM_HOME_DIR}/vhdl.awk $netlist_dir/$source_file \
              > $netlist_dir/$netlist_file"
   if ![string compare $show "show"] {
     textwindow $netlist_dir/$netlist_file
   }
 }
 if [regexp {\.v$} $netlist_file ] {
   eval exec "${XSCHEM_HOME_DIR}/verilog.awk $netlist_dir/$source_file \
              > $netlist_dir/$netlist_file"

   # 20140409
   if { $verilog_2001==1 } { 
     eval exec ${XSCHEM_HOME_DIR}/convert_to_verilog2001.awk $netlist_dir/$netlist_file > $netlist_dir/${netlist_file}vv
     eval exec mv $netlist_dir/${netlist_file}vv $netlist_dir/$netlist_file
   }
   if ![string compare $show "show"] {
     textwindow $netlist_dir/$netlist_file
   }
 }

 if ![string compare $netlist_file "netlist"] {
         textwindow $netlist_dir/netlist
 }
 return {}
}

# 20161216 execute task in other work dir
proc task { cmd {dir .}  {background fg}} {
  global task_output task_error
  if {$background eq {bg}} {
    set task_error [catch {exec sh -c "cd $dir; exec $cmd" &} task_output]
  } elseif {$background eq {fg}} {
    set task_error [catch {exec sh -c "cd $dir; exec $cmd"} task_output]
  } elseif {$background eq {tk_exec}} {
    set task_error [catch {tk_exec sh -c "cd $dir; exec $cmd"} task_output]
  }
}


# 20161121
proc convert_to_pdf {filename} {
  global a3page
  if { $a3page == 1 } { set paper a3 } else { set paper a4 }
  if { ![catch "exec ps2pdf -sPAPERSIZE=$paper $filename" msg] } {
    # ps2pdf succeeded, so remove original .ps file
    file delete $filename
  }
}

# 20161121
proc convert_to_png {filename} {
  global to_png
  set destfile [file rootname $filename].png
  # puts "---> $to_png $filename $destfile"
  if { ![catch "exec $to_png $filename $destfile" msg] } {
    # conversion succeeded, so remove original .xpm file
    file delete $filename
  }
}

proc edit_file {filename} {
 
 global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_type editor
 eval exec $editor  $filename & ;# 20161119
 return {}
}

proc simulate {filename} {
 global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type
 global task_output task_error
 global iverilog_path vvp_path hspice_path hspicerf_path spice_simulator 
 global modelsim_path verilog_simulator
 global vhdl_simulator ghdl_path ghdl_elaborate_opts ghdl_run_opts
 global finesim_path finesim_opts
 global utile_cmd_path terminal
 global iverilog_opts ;# 20161118
 global computerfarm ;# 20151007

 if { [xschem set_netlist_dir 0] ne "" } {

   if { $netlist_type=="verilog" } {
     # 20150916 added modelsim
     if { $verilog_simulator == "iverilog" } { ;# icarus verilog
       task "$iverilog_path $iverilog_opts -o .verilog_object $filename" $netlist_dir fg
       if {$task_error} {viewdata $task_output; return}
       task  "$vvp_path $netlist_dir/.verilog_object" $netlist_dir fg
       if {$task_error} {viewdata $task_output; return}
       write_data $task_output $netlist_dir/.sim_output.txt
       task "$terminal  -e less $netlist_dir/.sim_output.txt" $netlist_dir bg
     } elseif { $verilog_simulator =="modelsim" } { ;# modelsim
       #puts {start compile}
       task "${modelsim_path}/vlog +acc $filename" $netlist_dir fg
       if {$task_error} {viewdata $task_output; return}
       #puts {start simulation}
       task "${modelsim_path}/vsim -c -do \"run -all\" [file rootname $filename]" $netlist_dir fg
       if {$task_error} {viewdata $task_output; return}
       #puts {end simulation}
       write_data $task_output $netlist_dir/.sim_output.txt
       #puts {end log file}
       task "$terminal  -e less $netlist_dir/.sim_output.txt" $netlist_dir bg
     } else {
       alert_ "ERROR: undefined verilog simulator: $verilog_simulator"
     }

   } elseif { $netlist_type=="spice" } { 
     ## run utile before firing simulator if any UTILE stimuli file found
     set schname [ file tail [ file rootname $filename] ]
     if { [file exists stimuli.$schname ] } {
       task "$utile_cmd_path stimuli.$schname" $netlist_dir fg
     }
     if { $spice_simulator == "hspicerf" } {
       # added computerfarm
       ## 20161119 $terminal does not fit here, hspice wants only xterm !
       task "xterm -e \"$computerfarm $hspicerf_path $filename ; bash\"" $netlist_dir bg
     } elseif { $spice_simulator == "hspice"} {
       task "xterm -e \"$computerfarm $hspice_path -i $filename | tee hspice.out ; bash\""  $netlist_dir bg
     } elseif {$spice_simulator == "finesim"} {
       task "xterm -e \"$computerfarm $finesim_path $finesim_opts $filename ; bash \""  $netlist_dir bg
       # 20170410
     } else {
       alert_ "ERROR: undefined SPICE simulator: $spice_simulator"
     }
   } elseif { $netlist_type=="vhdl" } { 
     set schname [ file tail [ file rootname $filename] ]
     if { $vhdl_simulator == "modelsim" } { 
       set old $env(PWD)
       task "${modelsim_path}/vsim -i" $netlist_dir bg
     #20170921 added ghdl
     } elseif { $vhdl_simulator == "ghdl" } { 
       task  "$ghdl_path -c $ghdl_elaborate_opts $filename -r $ghdl_run_opts $schname --wave=${schname}.ghw" $netlist_dir fg
       if {$task_error} {viewdata $task_output; return}
       write_data $task_output $netlist_dir/.sim_output.txt
       task "$terminal  -e less $netlist_dir/.sim_output.txt" $netlist_dir bg
     } 
   } else { 
     alert_ "ERROR: netlist_type: $netlist_type , filename: $filename"
   }
 }
 return {}
}

proc modelsim {schname} {
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type
  global iverilog_path vvp_path hspice_path modelsim_path
  task "${modelsim_path}/vsim -i" $netlist_dir bg
}

proc utile_translate {schname} { 
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
  global utile_gui_path utile_cmd_path
  task "$utile_cmd_path stimuli.$schname" $netlist_dir fg
}

proc utile_gui {schname} { 
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
  global utile_gui_path utile_cmd_path
  task "$utile_gui_path stimuli.$schname" $netlist_dir bg
}

proc utile_edit {schname} { 
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug editor
  global utile_gui_path utile_cmd_path 
  task "sh -c \"$editor stimuli.$schname ; $utile_cmd_path stimuli.$schname\"" $netlist_dir bg 
}

proc waveview {schname} {
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
  global waveview_path
  task "$waveview_path -k -x $schname.sx" $netlist_dir tk_exec
}

proc cosmoscope { schname } {
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
  global cscope_path
  task "$cscope_path" $netlist_dir bg
}


proc gtkwave {schname} {
  global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
  global gtkwave_path
  task "$gtkwave_path 2>/dev/null" $netlist_dir bg
}

proc waves {schname} {

 global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
 global cscope_path gtkwave_path analog_viewer waveview_path

 if { [xschem set_netlist_dir 0] ne "" } {
   if { $netlist_type=="verilog" } {
     task "$gtkwave_path dumpfile.vcd $schname.sav 2>/dev/null" $netlist_dir bg

   } elseif { $netlist_type=="spice" } {

     if { $analog_viewer == "cosmoscope" } { 
       task $cscope_path $netlist_dir bg
     } elseif { $analog_viewer == "waveview" } { 
       task "$waveview_path -k -x $schname.sx" $netlist_dir bg ; # 20170415 bg instead of tk_init exec mode
     } else {
       alert_ { Unsupported default wiever... } 
     }
   } elseif { $netlist_type=="vhdl" } { 
     task "$gtkwave_path ${schname}.ghw $schname.sav 2>/dev/null" $netlist_dir bg
   }

 }
 return {}
}

proc get_shell { curpath } {
 global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
 global cscope_path gtkwave_path analog_viewer waveview_path terminal

 task "$terminal" $curpath bg
}

proc edit_netlist {schname } {
 global XSCHEM_HOME_DIR entry1 XSCHEM_DESIGN_DIR env netlist_dir netlist_type tcl_debug
 global cscope_path gtkwave_path analog_viewer waveview_path editor terminal

 if { [regexp vim $editor] } { set ftype "-c \":set filetype=$netlist_type\"" } else { set ftype {} }
 if { [xschem set_netlist_dir 0] ne "" } {
   # puts "edit_netlist: \"$editor $ftype  ${schname}.v\" $netlist_dir bg"
   if { $netlist_type=="verilog" } {
     task "$editor $ftype  ${schname}.v" $netlist_dir bg
   } elseif { $netlist_type=="spice" } {
     task "$editor $ftype ${schname}.spice" $netlist_dir bg
   } elseif { $netlist_type=="vhdl" } { 
     task "$editor $ftype ${schname}.vhdl" $netlist_dir bg
   }
 }
 return {}
}

proc check_valid_filename { s} { 
  global XSCHEM_DESIGN_DIR
  set ppp [file dirname $XSCHEM_DESIGN_DIR/$s]
  if { [regexp "$XSCHEM_DESIGN_DIR/.+" $ppp] } { return ok }
  return {}
}

# 20161207
proc fileload { msg {initialfile {}} {confirm 0}  } {
  global FILESELECT_CURR_DIR XSCHEM_DESIGN_DIR  entry1 tcl_version
  while {1} {
    set r [tk_getOpenFile  -title $msg -initialfile $initialfile -initialdir $FILESELECT_CURR_DIR ]
    set dir [file dirname $r]
    set entry1 [ file extension $r]
    set a [ get_cell $r]$entry1
    if { ![string compare $r {} ] } { break }
    # 20170921 resolve symlinks
    set designdir $XSCHEM_DESIGN_DIR
    if { [ file type $XSCHEM_DESIGN_DIR ] == "link" } { 
      set designdir [ file readlink $XSCHEM_DESIGN_DIR ] 
    }
    if { [regexp "${designdir}/.+" $dir] } { break }
  }
  return $a

}


# 20121111
proc filesave { msg {initialfile {}} {confirm 0}  } {
  global FILESELECT_CURR_DIR XSCHEM_DESIGN_DIR  entry1 tcl_version
  while {1} {
    if {$tcl_version > 8.4} {
      set r [tk_getSaveFile  -title $msg -initialfile $initialfile -initialdir $FILESELECT_CURR_DIR -confirmoverwrite $confirm]
    } else {
      set r [tk_getSaveFile  -title $msg -initialfile $initialfile -initialdir $FILESELECT_CURR_DIR ]
    }
    set dir [file dirname $r]
    set entry1 [ file extension $r] 
    set a [ get_cell $r]$entry1
    if { ![string compare $r {} ] } { break } 
    # 20170622 check for dirname after $XSCHEM_DESIGN_DIR and filename before .sch or .sym
    #                               /dirname/filename
    if { [regexp "$XSCHEM_DESIGN_DIR/\[^.\]+/\[^.\]+\." $r] } { break } 
  }
  return $a
  
}

# used in scheduler.c  20121111
# get last 2 path components: example /aaa/bbb/ccc/ddd.sch -> ccc/ddd
proc get_cell {s} {
  set slist [file split $s]
  if { [llength $slist] >1 } {
    return [lindex $slist end-1]/[file rootname [lindex $slist end]]
  } else {
    return [file rootname [lindex $slist end]]
  }
}


proc loadinst {ext} {
 global XSCHEM_HOME_DIR XSCHEM_DESIGN_DIR INITIALINSTDIR FILESELECT_CURR_DIR tcl_debug
 set FILESELECT_CURR_DIR $INITIALINSTDIR
 set a [file rootname [fileload {LOAD INSTANCE}]].sym
  if $tcl_debug<=-1 then {puts "a=$a"}
 if { [string compare $a ""] && [file exists $XSCHEM_DESIGN_DIR/$a] } {
  set INITIALINSTDIR $XSCHEM_DESIGN_DIR/[file dirname $a]
  if $tcl_debug<=-1 then {puts "INITIALINSTDIR=$INITIALINSTDIR"}
 } else {return {} }
 return  [file rootname $a]
}

proc loadfile {ext} {
 global XSCHEM_HOME_DIR XSCHEM_DESIGN_DIR INITIALLOADDIR FILESELECT_CURR_DIR
 set FILESELECT_CURR_DIR $INITIALLOADDIR
 set a [fileload {LOAD FILE}]
 if { $a == [file rootname $a] } { set a $a$ext }
 if { [string compare $a ""] } {
   if { [file exists $XSCHEM_DESIGN_DIR/$a] } {
     set INITIALLOADDIR $XSCHEM_DESIGN_DIR/[file dirname $a]
   }
 } else {
   return {}
 }
 return $a
}

proc savefile {name ext} {
 global XSCHEM_HOME_DIR XSCHEM_DESIGN_DIR INITIALDIR FILESELECT_CURR_DIR entry1
 set nn [file tail $name]
 set FILESELECT_CURR_DIR $XSCHEM_DESIGN_DIR/[file dirname $name]
 set a [filesave {SAVE FILE} $nn 1]
 set entry1 [ file extension $a] 
 return [file rootname $a]
}

proc delete_files { dir } { 
 if  { [ info tclversion]  >=8.4} {
   set x [tk_getOpenFile -title "DELETE FILES" -multiple 1 -initialdir [file dirname $dir] ]
 } else {
   set x [tk_getOpenFile -title "DELETE FILES" -initialdir [file dirname $dir] ]
 }
 foreach i  $x {
   file delete $i
 }
}

proc create_pins {} {
  global env entry1 
  global filetmp1 filetmp2

  set entry1 [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $entry1 {[} entry1 
  regsub -all {>} $entry1 {]} entry1 
  set lines [split $entry1 \n]

  # viewdata $entry1
  set pcnt 0
  set y 0
  set fd [open $env(PWD)/.clipboard.sch "w"]
  foreach i $lines { 
    puts $fd "C \{devices/[lindex $i 1]\} 0 [set y [expr $y-20]]  0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
  }
  close $fd
  xschem merge $env(PWD)/.clipboard.sch
}

# 20120913
proc add_lab_no_prefix {} { 
  global env entry1
  global filetmp1 filetmp2

  set entry1 [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $entry1 {[} entry1
  regsub -all {>} $entry1 {]} entry1
  set lines [split $entry1 \n]
  # viewdata $entry1
  set pcnt 0
  set y 0
  set fd [open $env(PWD)/.clipboard.sch "w"]
  foreach i $lines {
    puts $fd "C \{devices/lab_pin\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
  }
  close $fd
  xschem merge $env(PWD)/.clipboard.sch
}


proc add_lab_prefix {} {
  global env entry1
  global filetmp1 filetmp2

  set entry1 [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $entry1 {[} entry1
  regsub -all {>} $entry1 {]} entry1
  set lines [split $entry1 \n]
  # viewdata $entry1
  set pcnt 0
  set y 0
  set fd [open $env(PWD)/.clipboard.sch "w"]
  foreach i $lines {
    puts $fd "C \{devices/lab_pin\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
  }
  close $fd
  xschem merge $env(PWD)/.clipboard.sch
}


proc make_symbol {name} {
 global XSCHEM_HOME_DIR XSCHEM_DESIGN_DIR symbol_width
 eval exec "${XSCHEM_HOME_DIR}/make_sym.awk $symbol_width ${XSCHEM_DESIGN_DIR}/${name}.sch"
 return {}
}

proc select_dir {} {
   global netlist_dir env

   if { $netlist_dir ne {} }  { 
     set initdir $netlist_dir
   } else {
     set initdir  $env(PWD) 
   }
   # 20140409 do not change netlist_dir if user Cancels action
   set new_dir [tk_chooseDirectory -initialdir $initdir -parent . -title {Select netlist DIR} -mustexist false]

   if {$new_dir ne {} } {
     if {![file exist $new_dir]} {
       file mkdir $new_dir
     }
     set netlist_dir $new_dir  
   }
   return $netlist_dir
}

proc enter_text {textlabel} {
   global txt rcode
   set rcode {}
   toplevel .t -class Dialog

   ## not honored by fvwm ... 20110322
   # wm attributes .t -topmost 1
   ## ... use alternate method instead 20110322
   # bind .t <Visibility> { if { [regexp Obscured %s] } {raise .t; wm attributes  .t -topmost 1} }
   ## 


   set X [expr [winfo pointerx .t] - 30]
   set Y [expr [winfo pointery .t] - 25]

   # 20100203
   if { $::wm_fix } { tkwait visibility .t }
   wm geometry .t "+$X+$Y"
   label .t.txtlab -text $textlabel
   text .t.txt -width 100 -height 12
   .t.txt delete 1.0 end
   .t.txt insert 1.0 $txt
   pack .t.txtlab .t.txt -side top -fill x
   pack .t.txt -side top -fill  both -expand yes
   frame .t.edit
     frame .t.edit.lab
     frame .t.edit.entries
     pack  .t.edit.lab -side left 
     pack  .t.edit.entries -side left -fill x  -expand yes
     pack .t.edit  -side top  -fill x 
       entry .t.edit.entries.hsize -relief sunken -textvariable hsize -width 20
       entry .t.edit.entries.vsize -relief sunken -textvariable vsize -width 20
       entry .t.edit.entries.props -relief sunken -textvariable props -width 20 
       pack .t.edit.entries.hsize .t.edit.entries.vsize  \
        .t.edit.entries.props -side top  -fill x -expand yes
       label .t.edit.lab.hlab -text "hsize:"
       label .t.edit.lab.vlab -text "vsize:"
       label .t.edit.lab.proplab -text "props:"
       pack .t.edit.lab.hlab .t.edit.lab.vlab  \
        .t.edit.lab.proplab -side top
   frame .t.buttons
   button .t.buttons.ok -text "OK" -command  \
   {
    set txt [.t.txt get 1.0 {end - 1 chars}]
    set rcode {ok}
    destroy .t 
   }
   button .t.buttons.cancel -text "Cancel" -command  \
   {
    set txt {}
    set rcode {}
    destroy .t 
   }
   button .t.buttons.b3 -text "Load" -command \
   {
     global INITIALPROPDIR
     set a [tk_getOpenFile -parent .t -initialdir $INITIALPROPDIR ]
     if [string compare $a ""] {
      set INITIALPROPDIR [file dirname $a]
      read_data_window  .t.txt  $a
     }
   }
   button .t.buttons.b4 -text "Del" -command \
   {
     .t.txt delete 1.0 end
   }

   pack .t.buttons.ok  -side left -fill x -expand yes
   pack .t.buttons.cancel  -side left -fill x -expand yes
   pack .t.buttons.b3  -side left -fill x -expand yes
   pack .t.buttons.b4  -side left -fill x -expand yes
   pack .t.buttons -side bottom -fill x
   #grab set .t
   tkwait window .t
   return $txt
}

# evaluate a tcl command from GUI
proc tclcmd {} {
  toplevel .tclcmd -class dialog
  label .tclcmd.txtlab -text {Enter TCL expression:}
  label .tclcmd.result -text {Result:}
  text .tclcmd.t -width 100 -height 8
  text .tclcmd.r -width 100 -height 6 -yscrollcommand ".tclcmd.yscroll set" 
  scrollbar .tclcmd.yscroll -command  ".tclcmd.r yview"

  frame .tclcmd.b
  button .tclcmd.b.close -text Close -command {
    destroy .tclcmd
  }
  button .tclcmd.b.ok -text Evaluate -command {
    set txt [.tclcmd.t get 1.0 end]
    set res [eval $txt]
    .tclcmd.r delete 1.0 end
    .tclcmd.r insert 1.0 $res
  }

  pack .tclcmd.txtlab -side top -fill x
  pack .tclcmd.t -side top -fill  both -expand yes
  pack .tclcmd.result -side top -fill x
  pack .tclcmd.b -side bottom -fill x
  pack .tclcmd.yscroll -side right -fill y
  pack .tclcmd.r -side top -fill  both -expand yes

  pack .tclcmd.b.ok -side left -expand yes -fill x
  pack .tclcmd.b.close -side left -expand yes -fill x
}


proc property_search {} {
  global search_value
  global search_substring
  global search_select
  global custom_token

  toplevel .lw -class Dialog
  ## not honored by fvwm ... 20110322
  # wm attributes .lw -topmost 1
  ## ... use alternate method instead 20110322
  bind .lw <Visibility> { if { [regexp Obscured %s] } {raise .lw; if { $tcl_version > 8.4 } {wm attributes  .lw -topmost 1} } }
  ## 


  # 20100408
  set X [expr [winfo pointerx .lw] - 60]
  set Y [expr [winfo pointery .lw] - 35]
  wm geometry .lw "+$X+$Y"

  frame .lw.custom 
  label .lw.custom.l -text "Token"
  entry .lw.custom.e -width 32
  .lw.custom.e insert 0 $custom_token
  pack .lw.custom.e .lw.custom.l -side right

  frame .lw.val 
  label .lw.val.l -text "Value"
  entry .lw.val.e -width 32
  .lw.val.e insert 0 $search_value
  pack .lw.val.e .lw.val.l -side right

  frame .lw.but
  button .lw.but.ok -text OK -command {
        set search_value [.lw.val.e get]
        set custom_token [.lw.custom.e get]
        if $tcl_debug<=-1 then { puts stderr "|$custom_token|" }
	set token $custom_token
        if { $search_substring==1 } { xschem search sub $search_select $token $search_value
        } else { xschem search nosub $search_select $token $search_value }
        destroy .lw 
  }
  button .lw.but.cancel -text Cancel -command { destroy .lw }
  checkbutton .lw.but.sub -text Substring -variable search_substring
  checkbutton .lw.but.sel -text Select -variable search_select
  pack .lw.but.ok  -anchor w -side left
  pack .lw.but.sub  -side left
  pack .lw.but.sel  -side left
  pack .lw.but.cancel -anchor e

  pack .lw.custom  -anchor e
  pack .lw.val  -anchor e
  pack .lw.but -expand yes -fill x

  focus  .lw
  bind .lw <Return> {.lw.but.ok invoke}
  grab set .lw
  tkwait window .lw
  return {}
}

proc ask_save { {ask {save file?}} } {
   global rcode
   toplevel .ent2 -class Dialog

   ## not honored by fvwm ... 20110322
   # wm attributes .ent2 -topmost 1
   ## ... use alternate method instead 20110322
   bind .ent2 <Visibility> { if { [regexp Obscured %s] } {raise .ent2; if { $tcl_version > 8.4 } {wm attributes  .ent2 -topmost 1} } }
   ## 

   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent2 }

   wm geometry .ent2 "+$X+$Y"
   label .ent2.l1  -text $ask
   frame .ent2.f1
   button .ent2.f1.b1 -text {Yes} -command\
   {
    set rcode {yes}
    destroy .ent2
   }
   button .ent2.f1.b2 -text {Cancel} -command\
   {
    set rcode {}
    destroy .ent2
   }
   button .ent2.f1.b3 -text {No} -command\
   {
    set rcode {no}
    destroy .ent2
   }
   pack .ent2.l1 .ent2.f1 -side top -fill x
   pack .ent2.f1.b1 .ent2.f1.b2 .ent2.f1.b3 -side left -fill x -expand yes
   # needed, otherwise problems when descending with double clixk 23012004
   tkwait visibility .ent2
   grab set .ent2
   tkwait window .ent2
   return $rcode
}


proc edit_vi_prop {txtlabel} {
 global XSCHEM_DESIGN_DIR entry1 symbol prev_symbol rcode rbutton1 rbutton2 tcl_debug netlist_type editor

 # 20150914
 global user_wants_copy_cell
 set user_wants_copy_cell 0

 set rcode {}
# set rbutton1 0  ## 20090518 commented...
# set rbutton2 0
 set filename .edit_file.[pid]
 if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
 set filename $filename.$suffix

 write_data $entry1 $XSCHEM_DESIGN_DIR/$filename
 eval tk_exec $editor $XSCHEM_DESIGN_DIR/$filename ;# 20161119

 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\nentry1=$entry1\n---------\n"}
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\nsymbol=$symbol\n---------\n"}
 set tmp [read_data $XSCHEM_DESIGN_DIR/$filename]
 file delete $XSCHEM_DESIGN_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
 if [string compare $tmp $entry1] {
        set entry1 $tmp
        if $tcl_debug<=-1 then {puts "modified"}
        set rcode ok
        return  $rcode
 } else {
        set rcode {}
        return $rcode
 }
}



proc edit_vi_netlist_prop {txtlabel} {
 global XSCHEM_DESIGN_DIR entry1 rcode rbutton1 rbutton2 tcl_debug netlist_type editor


 # 20150914
 global user_wants_copy_cell
 set user_wants_copy_cell 0

 # set rbutton1 0 ;# commented 20121206
 # set rbutton2 0 ;# commented 20121206
 set filename .edit_file.[pid]
 if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
 set filename $filename.$suffix
 
 regsub -all {\\?"} $entry1 {"} entry1
 write_data $entry1 $XSCHEM_DESIGN_DIR/$filename
 if { [regexp vim $editor] } { set ftype "\{-c :set filetype=$netlist_type\}" } else { set ftype {} }
 eval tk_exec $editor  $ftype $XSCHEM_DESIGN_DIR/$filename

 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$entry1\n---------\n"}

 set tmp [read_data $XSCHEM_DESIGN_DIR/$filename] ;# 20161204 dont remove newline even if it is last char
 # set tmp [read_data_nonewline $XSCHEM_DESIGN_DIR/$filename]
 file delete $XSCHEM_DESIGN_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
 if [string compare $tmp $entry1] {
        set entry1 $tmp

        regsub -all {\\?"} $entry1 {\\"} entry1
        set entry1 "\"${entry1}\"" 
        if $tcl_debug<=-1 then {puts "modified"}
        set rcode ok
        return  $rcode
 } else {
        set rcode {}
        return $rcode
 }
}



proc edit_prop {txtlabel} {
   global edit_prop_default_geometry
   global prev_symbol entry1 symbol rcode rbutton1 rbutton2 copy_cell tcl_debug editprop_semaphore
   global user_wants_copy_cell
   set user_wants_copy_cell 0
   set rcode {}
   set editprop_semaphore 1
   if $tcl_debug<=-1 then {puts " edit_prop{}: entry1=$entry1"}
   toplevel .ent2  -class Dialog 
   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent2 }

   wm geometry .ent2 "${edit_prop_default_geometry}+$X+$Y"

   ## not honored by fvwm ... 20110322
   # wm attributes .ent2 -topmost 1
   ## ... use alternate method instead 20110322
   bind .ent2 <Visibility> { if { [regexp Obscured %s] } {raise .ent2; if { $tcl_version > 8.4 } {wm attributes  .ent2 -topmost 1} } } 
   ## 

   # 20160325 change and remember widget size
   bind .ent2 <Configure> { 
     # puts [wm geometry .ent2]
     set geom [wm geometry .ent2]
     regsub {\+.*} $geom {} geom
     set edit_prop_default_geometry $geom
   }

   set prev_symbol $symbol
   label .ent2.l1  -text $txtlabel
   text .ent2.e1   -yscrollcommand ".ent2.yscroll set" -setgrid 1 \
                   -xscrollcommand ".ent2.xscroll set" -wrap none
     .ent2.e1 delete 1.0 end
     .ent2.e1 insert 1.0 $entry1

   scrollbar .ent2.yscroll -command  ".ent2.e1 yview"
   scrollbar .ent2.xscroll -command ".ent2.e1 xview" -orient horiz
   frame .ent2.f1
   frame .ent2.f2
   label .ent2.f1.l2 -text "Symbol"
   entry .ent2.f1.e2 -width 30
   .ent2.f1.e2 insert 0 $symbol
   button .ent2.f1.b1 -text "OK" -command   {
     set entry1 [.ent2.e1 get 1.0 {end - 1 chars}] 
     set symbol [ .ent2.f1.e2 get]
     set rcode {ok}
     set editprop_semaphore 0
     set user_wants_copy_cell $copy_cell
     if { ($symbol ne $prev_symbol) && $copy_cell } {

       if { [file exists "${XSCHEM_DESIGN_DIR}/${prev_symbol}.sch"] } {
         if { ! [file exists "${XSCHEM_DESIGN_DIR}/${symbol}.sch"] } {
           file copy "${XSCHEM_DESIGN_DIR}/${prev_symbol}.sch" "${XSCHEM_DESIGN_DIR}/${symbol}.sch"
         }
       }

       if { [file exists "${XSCHEM_DESIGN_DIR}/${prev_symbol}.sym"] } {
         if { ! [file exists "${XSCHEM_DESIGN_DIR}/${symbol}.sym"] } {
           file copy "${XSCHEM_DESIGN_DIR}/${prev_symbol}.sym" "${XSCHEM_DESIGN_DIR}/${symbol}.sym"
         }
       }
     }
     set copy_cell 0 ;# 20120919
   }
   button .ent2.f1.b2 -text "Cancel" -command  {
     set rcode {}
     set editprop_semaphore 0
   }
   button .ent2.f1.b3 -text "Load" -command {
     global INITIALPROPDIR
     set a [tk_getOpenFile -parent .ent2 -initialdir $INITIALPROPDIR ]
     if [string compare $a ""] {
      set INITIALPROPDIR [file dirname $a]
      read_data_window  .ent2.e1  $a
     }
   }
   button .ent2.f1.b4 -text "Del" -command {
     .ent2.e1 delete 1.0 end
   }
   checkbutton .ent2.f2.r1 -text "no change properties" -variable rbutton1 -state normal
   checkbutton .ent2.f2.r2 -text "preserve unchanged props" -variable rbutton2 -state normal
   checkbutton .ent2.f2.r3 -text "copy cell" -variable copy_cell -state normal

   pack .ent2.f1.l2 .ent2.f1.e2 .ent2.f1.b1 .ent2.f1.b2 .ent2.f1.b3 .ent2.f1.b4 -side left -expand 1
   pack .ent2.l1 -side top -fill x 
   pack .ent2.f1 .ent2.f2 -side top -fill x 
   pack .ent2.f2.r1 -side left
   pack .ent2.f2.r2 -side left
   pack .ent2.f2.r3 -side left
   pack .ent2.yscroll -side right -fill y 
   pack .ent2.xscroll -side bottom -fill x
   pack .ent2.e1  -fill both -expand yes
   #tkwait visibility .ent2
   #grab set .ent2
   #focus .ent2.e1
   #tkwait window .ent2
   while {1} {
     tkwait  variable editprop_semaphore
     if { $editprop_semaphore == 2 } {
       set entry1 [.ent2.e1 get 1.0 {end - 1 chars}] 
       set symbol [ .ent2.f1.e2 get]
       xschem update_symbol ok 
       set editprop_semaphore 1
       xschem fill_symbol_editprop_form
    
       # 20160423 no more setected stuff--> close
       if {$editprop_semaphore==0 } {
         break
       }
       # 20110325 update symbol variable after clicking another element to avoid 
       #          modified flag to be set even when nothing changed
       ## set symbol [ .ent2.f1.e2 get]

       .ent2.e1 delete 1.0 end
       .ent2.e1 insert 1.0 $entry1
       .ent2.f1.e2  delete 0 end
       .ent2.f1.e2 insert 0 $symbol
     } else {
       break
     }
    
   }
   destroy .ent2
   set editprop_semaphore 0
   return $rcode
}

proc read_data_nonewline {f} {
 set fid [open $f "r"]
 set data [read -nonewline $fid]
 close $fid
 return $data
}

proc read_data {f} {
 set fid [open $f "r"]
 set data [read $fid]
 close $fid
 return $data
}

proc read_data_window {w f} {
 set fid [open $f "r"]
 set t [read $fid]
 #  $w delete 0.0 end
 $w insert 0.0 $t
 close $fid
}

proc write_data {data f} {
 set fid [open $f "w"]
 puts  -nonewline $fid $data
 close $fid
 return {}
}

proc text_line {txtlabel clear} {
   global text_line_default_geometry
   global entry1 rcode tcl_debug
   if $clear==1 then {set entry1 ""}
   if $tcl_debug<=-1 then {puts " text_line{}: clear=$clear"}
   if $tcl_debug<=-1 then {puts " text_line{}: entry1=$entry1"}
   toplevel .ent2  -class Dialog
   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]

   ## not honored by fvwm ... 20110322
   # wm attributes .ent2 -topmost 1
   ## ... use alternate method instead 20110322
   bind .ent2 <Visibility> { if { [regexp Obscured %s] } {raise .ent2; if { $tcl_version > 8.4 } {wm attributes  .ent2 -topmost 1} } }
   ## 

   # 20160325 change and remember widget size
   bind .ent2 <Configure> {
     # puts [wm geometry .ent2]
     set geom [wm geometry .ent2]
     regsub {\+.*} $geom {} geom
     set text_line_default_geometry $geom
   }

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent2 }
   wm geometry .ent2 "${text_line_default_geometry}+$X+$Y"

   frame .ent2.f1
   label .ent2.l1  -text $txtlabel

   text .ent2.e1 -relief sunken -bd 2 -yscrollcommand ".ent2.yscroll set" -setgrid 1 \
        -xscrollcommand ".ent2.xscroll set" -wrap none -width 90 -height 40
   scrollbar .ent2.yscroll -command  ".ent2.e1 yview"
   scrollbar .ent2.xscroll -command ".ent2.e1 xview" -orient horiz
   .ent2.e1 delete 1.0 end
   .ent2.e1 insert 1.0 $entry1
   button .ent2.f1.b1 -text "OK" -command  \
   {
     set entry1 [.ent2.e1 get 1.0 {end - 1 chars}] 
     destroy .ent2
     set rcode {ok}
   }
   button .ent2.f1.b2 -text "Cancel" -command  \
   {
     set entry1 [.ent2.e1 get 1.0 {end - 1 chars}]
     set rcode {}
     destroy .ent2
   }
   button .ent2.f1.b3 -text "Load" -command \
   {
     global INITIALPROPDIR
     set a [tk_getOpenFile -parent .ent2 -initialdir $INITIALPROPDIR ]
     if [string compare $a ""] {
      set INITIALPROPDIR [file dirname $a]
      read_data_window  .ent2.e1  $a
     }
   }
   button .ent2.f1.b4 -text "Del" -command \
   {
     .ent2.e1 delete 1.0 end
   }
   pack .ent2.l1 
   pack .ent2.f1  -fill x
   pack .ent2.f1.b1 -side left -fill x -expand yes
   pack .ent2.f1.b2 -side left -fill x -expand yes
   pack .ent2.f1.b3 -side left -fill x -expand yes
   pack .ent2.f1.b4 -side left -fill x -expand yes
 

   pack .ent2.yscroll -side right -fill y 
   pack .ent2.xscroll -side bottom -fill x
   pack .ent2.e1   -expand yes -fill both
   #tkwait visibility .ent2
   #grab set .ent2
   #focus .ent2.e1

   # 20100208
   set rcode {}   

   tkwait window .ent2
   return $rcode
}

proc entry_line {txtlabel} {
   global entry1
   toplevel .ent2 -class Dialog

   ## not honored by fvwm ... 20110322
   # wm attributes .ent2 -topmost 1
   ## ... use alternate method instead 20110322
   bind .ent2 <Visibility> { if { [regexp Obscured %s] } {raise .ent2; if { $tcl_version > 8.4 } {wm attributes  .ent2 -topmost 1} } }
   ## 



   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent2 }

   wm geometry .ent2 "+$X+$Y"
   label .ent2.l1  -text $txtlabel
   entry .ent2.e1   -width 40 
     .ent2.e1 delete 0 end
     .ent2.e1 insert 0 $entry1
   button .ent2.b1 -text "OK" -command  \
   {
     set entry1 [.ent2.e1 get ] 
     destroy .ent2
   }                                          
   bind .ent2 <Return> {
     set entry1 [.ent2.e1 get ] 
     destroy .ent2
   }                                          
   pack .ent2.l1 -side top -fill x 
   pack .ent2.e1  -side top -fill both -expand yes
   pack .ent2.b1 -side top -fill x 
   grab set .ent2
   focus .ent2.e1
   tkwait window .ent2
   return $entry1
}

proc alert_ {txtlabel {position +200+300}} {
   toplevel .ent3 -class Dialog
   set X [expr [winfo pointerx .ent3] - 60]
   set Y [expr [winfo pointery .ent3] - 60]

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent3 }

   if { [string compare $position ""] != 0 } {
     wm geometry .ent3 $position
   } else {
     wm geometry .ent3 "+$X+$Y"
   }
   
   label .ent3.l1  -text $txtlabel             
   button .ent3.b1 -text "OK" -command  \
   {
     destroy .ent3
   } 
   pack .ent3.l1 -side top -fill x
   pack .ent3.b1 -side top -fill x
   grab set .ent3
   focus .ent3.b1
   bind .ent3.b1 <Return> { destroy .ent3 }

   tkwait window .ent3  
   return {}
}


proc infowindow {infotxt} {
 set z {.infotext}

  if ![string compare $infotxt ""] { 
     if [winfo exists $z] {
       $z.text delete 1.0 end
     }
  }
  if ![winfo exists $z] {
   toplevel $z
   wm title $z {Info window}
   wm  geometry $z 90x24+0+400
   wm iconname $z {Info window}
   wm withdraw $z
   wm protocol .infotext WM_DELETE_WINDOW {wm withdraw .infotext; set show_infowindow 0}
#   button $z.dismiss -text Dismiss -command  "destroy $z"
   text $z.text -relief sunken -bd 2 -yscrollcommand "$z.yscroll set" -setgrid 1 \
        -height 6 -width 50  -xscrollcommand "$z.xscroll set" -wrap none
   scrollbar $z.yscroll -command "$z.text yview" -orient v 
   scrollbar $z.xscroll -command "$z.text xview" -orient h 
   grid $z.text - $z.yscroll -sticky nsew
   grid $z.xscroll - -sticky ew
#   grid $z.dismiss - -
   grid rowconfig $z 0 -weight 1
   grid columnconfig $z 0 -weight 1
  }
#  $z.text delete 1.0 end 
#  $z.text insert 1.0 $infotxt
  $z.text insert end $infotxt
  $z.text see end
  return {}
}
proc textwindow {filename} {
   global wcounter
   global w
   set wcounter [expr $wcounter+1]
   set w .win$wcounter
   catch [destroy $w]


   global fff
   global fileid
   set fff $filename
   toplevel $w
   wm title $w $filename
   wm iconname $w $filename
  frame $w.buttons
   pack $w.buttons -side bottom -fill x -pady 2m
   button $w.buttons.dismiss -text Dismiss -command  \
    " destroy $w; set wcounter [expr $wcounter-1] "
   button $w.buttons.save -text "Save" -command \
    { 
     set fileid [open $fff "w"]
     puts -nonewline $fileid [$w.text get 1.0 {end - 1 chars}]
     close $fileid 
     destroy $w
    }
   pack $w.buttons.dismiss $w.buttons.save  -side left -expand 1

   text $w.text -relief sunken -bd 2 -yscrollcommand "$w.yscroll set" -setgrid 1 \
        -xscrollcommand "$w.xscroll set" -wrap none -height 30
   scrollbar $w.yscroll -command  "$w.text yview" 
   scrollbar $w.xscroll -command "$w.text xview" -orient horiz
   pack $w.yscroll -side right -fill y
   pack $w.text -expand yes -fill both
   pack $w.xscroll -side bottom -fill x
   set fileid [open $filename "r"]
   $w.text insert 0.0 [read $fileid]
   close $fileid
   return {}
}


proc viewdata {data} {
   global wcounter  rcode
   global w
   set wcounter [expr $wcounter+1]
   set rcode {}
   set w .win$wcounter
   catch [destroy $w]
   toplevel $w
   frame $w.buttons
   pack $w.buttons -side bottom -fill x -pady 2m

   # 20111106
   button $w.buttons.saveas -text {Save As} -command  {
     set fff [tk_getSaveFile -initialdir $env(PWD) ]
     if { $fff != "" } {
       set fileid [open $fff "w"]
       puts -nonewline $fileid [$w.text get 1.0 {end - 1 chars}]
       close $fileid
     }
   } 
   button $w.buttons.dismiss -text Dismiss -command  \
    " destroy $w; set wcounter [expr $wcounter-1] " 
   pack $w.buttons.dismiss   $w.buttons.saveas  -side left -expand 1

   text $w.text -relief sunken -bd 2 -yscrollcommand "$w.yscroll set" -setgrid 1 \
        -xscrollcommand "$w.xscroll set" -wrap none -height 30
   scrollbar $w.yscroll -command  "$w.text yview" 
   scrollbar $w.xscroll -command "$w.text xview" -orient horiz
   pack $w.yscroll -side right -fill y
   pack $w.text -expand yes -fill both
   pack $w.xscroll -side bottom -fill x
   $w.text insert 0.0 $data
   return $rcode
}

proc event_process {s1 s2 s3 s4 s5 s6 s7} {
  global tcl_debug ps_colors svg_colors
  if $tcl_debug<=-2 then {puts "event_process{}: xschem callback $s1 $s2 $s3 $s4 $s5 $s6 $s7"}
  xschem callback $s1 $s2 $s3 $s4 $s5 $s6 $s7
  return {}
}

################## GENSCH procedure  #########################

# gensch bonelli/0_top hiace5/hmicro_rom_ptrif

proc gensch {cell {selected {}} } {
  global gensch_res gensch_body gensch_aggressive cellname
  global gensch_i_pin gensch_o_pin gensch_io_pin 
  global gensch_pinarray gensch_names gensch_coord

  array unset gensch_pinarray
  array unset gensch_names
  array unset gensch_coord
  set gensch_aggressive 0
  set gensch_body {}
  set gensch_res {}
  toplevel .gensch -class Dialog 

  ## not honored by fvwm ... 20110322
  # wm attributes .gensch -topmost 1
  ## ... use alternate method instead 20110322
  #bind .gensch <Visibility> { if { [regexp Obscured %s] } {raise .gensch; wm attributes  .gensch -topmost 1} }
  ## 


  frame .gensch.but 
  frame .gensch.ipin
  frame .gensch.opin
  frame .gensch.iopin
  entry .gensch.name  
  label .gensch.ipin.ipindescr   -text "IPIN:" -width 8
  label .gensch.opin.opindescr   -text "OPIN:" -width 8
  label .gensch.iopin.iopindescr -text "IOPIN:" -width 8
  entry .gensch.ipin.ipin   -width 60
  entry .gensch.opin.opin   -width 60
  entry .gensch.iopin.iopin -width 60
  regsub  {\/.*} $cell {/} cell
  if { [ string compare $selected {} ] } { 
    .gensch.name insert 0 $selected
    load_sym
  } else {
    .gensch.name insert 0 ${cell}xxxx
  }
  button .gensch.but.create -text CREATE -command {
    set gensch_i_pin [.gensch.ipin.ipin get] 
    set gensch_o_pin [.gensch.opin.opin get] 
    set gensch_io_pin [.gensch.iopin.iopin get] 
    set cellname [.gensch.name get]
    create_sym
  }
  button .gensch.but.load -text LOAD -command load_sym
  button .gensch.but.canc -text Cancel -command { set gensch_res {}; destroy .gensch; }
  checkbutton .gensch.but.aggr -text Aggressive -variable gensch_aggressive
  pack .gensch.name

  pack .gensch.ipin.ipin -side right -fill x -expand y
  pack .gensch.ipin.ipindescr  -side left
  pack .gensch.opin.opin -side right  -fill x -expand y
  pack .gensch.opin.opindescr  -side left
  pack .gensch.iopin.iopin -side right -fill x -expand y
  pack .gensch.iopin.iopindescr -side left
  pack .gensch.ipin .gensch.opin .gensch.iopin -fill x

  pack .gensch.but.create .gensch.but.canc .gensch.but.load .gensch.but.aggr -side right 
  pack .gensch.but 
  tkwait window .gensch
  return $gensch_res

}

#C {devices/ipin} 190 80 0 0 {name=p29 sig_type=std_logic lab=ADD_P1[9:0] }
proc load_sym {} {
  global  XSCHEM_DESIGN_DIR gensch_res gensch_pinarray gensch_body gensch_names gensch_coord
  set cellname [.gensch.name get]
  set gensch_body {}
  .gensch.ipin.ipin delete 0 end
  .gensch.opin.opin delete 0 end
  .gensch.iopin.iopin delete 0 end
  array unset gensch_pinarray
  array unset gensch_names
  array unset gensch_coord
  set fname "$XSCHEM_DESIGN_DIR/${cellname}.sch"
  if { [ file exists $fname] } {
    set data  [read_data_nonewline $fname]
    set lines [split $data \n]
    for { set i 0 } { $i < [llength $lines] } { incr i } {
      set line [lindex $lines $i]
      ###regsub -all -- {[[:space:]]+} $line " " line   ;# 20120926
      set linelist [split $line]

      if { [regexp "^C.* \{name=(\[^ \}\]*)\[ \}\]"  $line allmatch name] } {
        array set gensch_names [list $name 1]
        array set gensch_coord [list [join [lrange $linelist 2 3]] 1]
        # puts "---> [lrange $line 2 3]"
      }


      if { [regexp "^C \{devices/(i|o|io)pin\}.* lab=(\[^ \}\]*)\[ \}\]" $line allmatch type lab] } {
        array set gensch_pinarray [list $lab $line]
        if { ![string compare $type i] } {
          .gensch.ipin.ipin insert end "$lab "
        } elseif { ![string compare $type o] } {
          .gensch.opin.opin insert end "$lab "
        } elseif { ![string compare $type io] } {
          .gensch.iopin.iopin insert end "$lab "
        }
        
      } else {
        if { [string compare $gensch_body {} ]} { 
          set gensch_body "$gensch_body\n$line"
        } else {
          set gensch_body "$line"
        }
      }
      .gensch.ipin.ipin xview moveto 1 
      .gensch.opin.opin xview moveto 1 
      .gensch.iopin.iopin xview moveto 1 
    }
  }
}

proc create_sym {} {
  global  XSCHEM_DESIGN_DIR gensch_res gensch_pinarray gensch_body gensch_names gensch_coord
  global gensch_aggressive cellname create_sym_interactive
  global gensch_i_pin gensch_o_pin gensch_io_pin 
  set gensch_i [llength $gensch_i_pin ]
  set gensch_o [llength $gensch_o_pin ]
  set gensch_io [llength $gensch_io_pin ]
  set gensch_res $cellname
  set fname "$XSCHEM_DESIGN_DIR/${cellname}.sch"
  set symfname "$XSCHEM_DESIGN_DIR/${cellname}.sym"
  if { $create_sym_interactive && ([file exists $fname] || [file exists $symfname]) }  {
   set ask_s [ask_save "File exists: overwrite ? "]
   if { [string compare $ask_s yes] } {
     set gensch_res {}
     return
   }
  }
  set fp [open $fname w] 
  if { [ string compare $gensch_body {} ] } {
   puts $fp $gensch_body
  } else {
    puts $fp "G \{\}"
    puts $fp "V \{\}"
  }

  set pin 0
  set y1 0
  for { set l 0 } { $l < $gensch_i } { incr l } {
      set name [lindex $gensch_i_pin $l]
      regsub -all {<} $name {[} name
      regsub -all {>} $name {]} name
      regsub -all {([\[\]])} $name {\\\1} name_esc
      if { !$gensch_aggressive && [ string compare  [ array names gensch_pinarray $name_esc ] {} ] } {
        puts $fp $gensch_pinarray($name)
      } else {
        while { [ string compare  [ array names gensch_names p$pin ] {} ] } {
          incr pin
        }
        set y1 [expr $y1+20]

        while { [ string compare  [ array names gensch_coord "-500 *${y1}" ] {} ] } {
          set y1 [expr $y1+20]
        }
        puts $fp "C \{devices/ipin\} -500 $y1 0 0 \{name=p$pin lab=$name\}"
      }
      incr pin
  }
  set y1 0
  for { set l 0 } { $l < $gensch_o } { incr l } {
      set name [lindex $gensch_o_pin $l]
      regsub -all {<} $name {[} name
      regsub -all {>} $name {]} name
      regsub -all {([\[\]])} $name {\\\1} name_esc
      if { !$gensch_aggressive && [ string compare  [ array names gensch_pinarray $name_esc ] {} ] } {
        puts $fp $gensch_pinarray($name)
      } else {
        while { [ string compare  [ array names gensch_names p$pin ] {} ] } {
          incr pin
        }
        set y1 [expr $y1+20]

        while { [ string compare  [ array names gensch_coord "-200 *${y1}" ] {} ] } {
          set y1 [expr $y1+20]
        }
        puts $fp "C \{devices/opin\} -200 $y1 0 0 \{name=p$pin lab=$name\}"
      }
      incr pin
  }
  for { set l 0 } { $l < $gensch_io } { incr l } {
      set name [lindex $gensch_io_pin $l]
      regsub -all {<} $name {[} name
      regsub -all {>} $name {]} name
      regsub -all {([\[\]])} $name {\\\1} name_esc
      if { !$gensch_aggressive && [ string compare  [ array names gensch_pinarray $name_esc ] {} ] } {
        puts $fp $gensch_pinarray($name)
      } else {
        while { [ string compare  [ array names gensch_names p$pin ] {} ] } {
          incr pin
        }
        set y1 [expr $y1+20]

        while { [ string compare  [ array names gensch_coord "-200 *${y1}" ] {} ] } {
          set y1 [expr $y1+20]
        }
        puts $fp "C \{devices/iopin\} -200 $y1 0 0 \{name=p$pin lab=$name\}"
      }
      incr pin
  }
  close $fp     
  destroy .gensch
}

################## END GENSCH        #########################

proc input_number {txt cmd} {
          global xx
          toplevel .lw -class Dialog
          set X [expr [winfo pointerx .lw] - 60]
          set Y [expr [winfo pointery .lw] - 35]
          # 20100203
          if { $::wm_fix } { tkwait visibility .lw }
          wm geometry .lw "+$X+$Y"

          ## not honored by fvwm ... 20110322
          # wm attributes .lw -topmost 1
          ## ... use alternate method instead 20110322
          bind .lw <Visibility> { if { [regexp Obscured %s] } {raise .lw; if { $tcl_version > 8.4 } {wm attributes  .lw -topmost 1} } }
          ## 


          set xx $cmd
          frame .lw.f1
          label .lw.f1.l -text $txt
          entry .lw.f1.e -width 12
          pack .lw.f1.l .lw.f1.e -side left
          frame .lw.f2
          button .lw.f2.ok -text OK -command {  eval [subst "$xx [.lw.f1.e get]"] ; destroy .lw }
          button .lw.f2.cancel -text Cancel -command { destroy .lw }
          pack .lw.f2.ok  -anchor w -side left
          pack .lw.f2.cancel -anchor e
          pack .lw.f1
          pack .lw.f2 -expand yes -fill x
          grab set .lw
          focus .lw
          tkwait window .lw
}

## 20161102
proc launcher {} {
  global launcher_var launcher_default_program launcher_program 
  global XSCHEM_HOME_DIR XSCHEM_DESIGN_DIR env
  
  ## puts ">>> $launcher_program $launcher_var &"
  # 20170413
  if { $launcher_program eq {} } { set launcher_program $launcher_default_program}

  eval exec  $launcher_program $launcher_var &
}

### 
###   MAIN PROGRAM
###

# tcl variable XSCHEM_DESIGN_DIR  should already be set in ~/.xschem,    20121110
# if this is not the case try to get it from the environment ...
#


  


if { ![info exists XSCHEM_DESIGN_DIR] } {
   if { [info exists env(XSCHEM_DESIGN_DIR)] } {
     set XSCHEM_DESIGN_DIR $env(XSCHEM_DESIGN_DIR)
   }
}

# ... if still undefined then its time to complain and give up           20121110
if { ![info exists XSCHEM_DESIGN_DIR] } {
  puts {FATAL: XSCHEM_DESIGN_DIR not defined, set it into ~/.xschem}
  if { ![info exists no_x] } {
    tk_messageBox -icon error -type ok -message {FATAL: XSCHEM_DESIGN_DIR not defined, set it into ~/.xschem}
  }
  exit
}




if { [xschem get help ]} {
  set fd [open ${XSCHEM_HOME_DIR}/xschem.help r]
  set helpfile [read $fd]
  puts $helpfile
  close $fd
  exit
}

set env(XSCHEM_HOME_DIR) ${XSCHEM_HOME_DIR} ;# 20161204

set_ne xschem_libs {}
set_ne tcl_debug 0
# used to activate debug from menu
set_ne menu_tcl_debug 0
set wcounter 1
set entry1 ""
set prev_symbol ""
set symbol ""

# 20100204 flag to enable fix for dialog box positioning,  issues with some wm
set wm_fix 0 


###
### user preferences: set default values
###
set_ne netlist_dir {}
set_ne hspice_netlist 0
set_ne verilog_2001 1
set_ne spice_simulator hspice
set_ne finesim_opts {}
set_ne verilog_simulator iverilog
set_ne vhdl_simulator ghdl ;# 20170921
set_ne split_files 0
set_ne flat_netlist 0
set_ne netlist_type vhdl
set_ne netlist_show 0
set_ne color_ps 0
set_ne only_probes 0  ; # 20110112
set_ne a3page 0
set_ne fullscreen 0
set_ne change_lw 0
set_ne line_width 0
set_ne incr_hilight 1
set_ne enable_stretch 0
set_ne draw_grid 1
set_ne sym_txt 1
set_ne show_infowindow 0 
set_ne symbol_width 150
set_ne editprop_semaphore 0
set_ne editor {gvim -f}
set_ne create_sym_interactive 1
set_ne rainbow_colors 0
set_ne initial_geometry {700x448+10+10}
#20161102
set_ne launcher_var {}
set_ne launcher_default_program {xdg-open}
set_ne launcher_program {}
#20160413
set_ne auto_hilight 0
## 20161121 xpm to png conversion
set_ne to_png {gm convert} 

## 20160325 remember edit_prop widget size
set_ne edit_prop_default_geometry 80x12
set_ne text_line_default_geometry 80x12
set_ne terminal xterm

set_ne analog_viewer cosmoscope
set_ne computerfarm {} ;# 20151007
## icarus verilog compiler and simulator 20140404
set_ne iverilog_path $env(HOME)/verilog/bin/iverilog
set_ne vvp_path $env(HOME)/verilog/bin/vvp
set_ne iverilog_opts {} ;# 20161118 allows to add -g2012 for example 
## ghdl 20170921
set_ne ghdl_path $env(HOME)/ghdl/bin/ghdl
set_ne ghdl_elaborate_opts {}
set_ne ghdl_run_opts {}
## gtkwave
set_ne gtkwave_path $env(HOME)/gtkwave/bin/gtkwave

## waveview
set_ne waveview_path $env(HOME)/waveview/bin/wv

## utile
set_ne utile_gui_path $env(HOME)/utile3/utile3
set_ne utile_cmd_path $env(HOME)/utile3/utile

## modelsim
set_ne modelsim_path $env(HOME)/modeltech/bin

## hspice license 20140404
set_ne env(LM_LICENSE_FILE) $env(HOME)/hspice/license.dat
## hspice simulator path 20140404
set_ne hspice_path $env(HOME)/hspice/hspice_2010.03
set_ne hspicerf_path $env(HOME)/hspice/hspicerf_2010.03

## cscope waveform viewer
set_ne cscope_path $env(HOME)/cosmoscope/linux/ai_bin/cscope




##### set colors
if {!$rainbow_colors} {

  set_ne cadlayers 22
  set_ne colors {
   "#000000" "#00ccee" "#5f5f5f" "#aaaaaa" "#88dd00" 
   "#bb2200" "#00ccee" "#ff0000" "#ffff00" "#ffffff" "#ff00ff" 
   "#00ff00" "#0000cc" "#aaaa00" "#aaccaa" "#ff7777" 
   "#bfff81" "#00ffcc" "#ce0097" "#d2d46b" 
   "#ef6158" "#fdb200" }

  set_ne ps_colors {
    0x000000 0x0000ee 0x7f7f7f 0x000000 0x338800 
    0xbb2200 0x0000ee 0xff0000 0xffff00 0x000000 0xff00ff 
    0x00ff00 0x0000cc 0xaaaa00 0xaaccaa 0xff7777 
    0xbfff81 0x00ffcc 0xce0097 0xd2d46b 
    0xef6158 0xfdb200 
  }
} else {
  # rainbow colors for bitmapping
  # skip if colors defined in ~/.xschem 20121110
  set_ne cadlayers 35
  set_ne colors {
    "#000000" "#00ccee" "grey50"  "#ffffff" "#88dd00" 
    "#bb2200" "#0000e0" "#2000e0" "#4000e0" "#6000e0"
    "#8000e0" "#a000e0" "#c000e0" "#e000e0" "#e000c0"
    "#e000a0" "#e00080" "#e00060" "#e00040" "#e00020"
    "#e00000" "#e02000" "#e04000" "#e06000" "#e08000"
    "#e0a000" "#e0c000" "#e0e000" "#e0e020" "#e0e040"
    "#e0e060" "#e0e080" "#e0e0a0" "#e0e0c0" "#e0e0e0"
  }
  set_ne ps_colors {
    0x000000 0x0000ee 0x7f7f7f 0x000000 0x338800 
    0xbb2200 0x0000e0 0x2000e0 0x4000e0 0x6000e0
    0x8000e0 0xa000e0 0xc000e0 0xe000e0 0xe000c0
    0xe000a0 0xe00080 0xe00060 0xe00040 0xe00020
    0xe00000 0xe02000 0xe04000 0xe06000 0xe08000
    0xe0a000 0xe0c000 0xe0e000 0xe0e020 0xe0e040
    0xe0e060 0xe0e080 0xe0e0a0 0xe0e0c0 0xe0e0e0
  }
}
# 20150408 check max # of layers
if { $cadlayers < [llength $colors] } {
  set colors [lrange $colors 0 [expr $cadlayers-1]]
}

# 20150408 check max # of layers
if { $cadlayers < [llength $ps_colors] } {
  set ps_colors [lrange $ps_colors 0 [expr $cadlayers-1]]
}

##### end set colors


# 20111005 added missing initialization of globals...
set rbutton1 0
set rbutton2 0


# gensch variables
array unset gensch_pinarray
array unset gensch_names
array unset gensch_coord
set gensch_aggressive 0
set gensch_body {}
set gensch_res {}

# 20111106 these vars are overwritten by caller with mktemp file names
set filetmp1 $env(PWD)/.tmp1
set filetmp2 $env(PWD)/.tmp2
# /20111106


# if set to 0 create_sym does not ask confirmation when overwriting a symbol file
# used by import_opus_symbols.awk

# flag bound to a checkbutton in symbol editprop form
# if set cell is copied when renaming it
set_ne copy_cell 0


# for svg draw 20121108
regsub -all {\"} $colors  {} svg_colors
regsub -all {#} $svg_colors  {0x} svg_colors


# schematic to preload in new windows 20090708
set_ne XSCHEM_START_WINDOW {}

###
### now its time to load user preferences ...20121110
###
# if { [file exists $env(HOME)/.xschem]} {  
#   source $env(HOME)/.xschem
# }

set INITIALDIR $XSCHEM_DESIGN_DIR
set INITIALLOADDIR $XSCHEM_DESIGN_DIR
set INITIALINSTDIR $XSCHEM_DESIGN_DIR
set INITIALPROPDIR $XSCHEM_DESIGN_DIR
set FILESELECT_CURR_DIR $XSCHEM_DESIGN_DIR
set txt ""
#. configure -cursor left_ptr
# used in the tools-search menu
set custom_token {lab}
set search_value {}
set search_substring 0

# 20121111
xschem set netlist_dir $netlist_dir


###
### build Tk widgets
###
if { [string length   [lindex [array get env DISPLAY] 1] ] > 0 
     && ![info exist no_x]} {

   . configure -cursor left_ptr
#   option add *Button*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Menubutton*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Menu*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Listbox*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Entry*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   #option add *Text*font "-*-courier-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Label*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile

   tk scaling 1.0
   infowindow {}
   #proc unknown  {comm args} { puts "unknown command-> \<$comm\> $args" }
   frame .menubar -relief raised -bd 2 
   
   menubutton .menubar.help -text "Help" -menu .menubar.help.menu
   menu .menubar.help.menu -tearoff 0
   .menubar.help.menu add command -label "help" -command "textwindow ${XSCHEM_HOME_DIR}/xschem.help" \
	-accelerator {?}
   .menubar.help.menu add command -label "keys" -command "textwindow ${XSCHEM_HOME_DIR}/keys.help"
   
   menubutton .menubar.file -text "File" -menu .menubar.file.menu
   menu .menubar.file.menu -tearoff 0
   .menubar.file.menu add command -label "New" \
     -command {
       xschem clear
     }
   .menubar.file.menu add command -label "Open" -command "xschem load" -accelerator L
   .menubar.file.menu add command -label "Save" -command "xschem save" -accelerator S
   .menubar.file.menu add command -label "Save" -command "xschem saveas" -accelerator s
   .menubar.file.menu add command -label "Merge" -command "xschem merge" -accelerator b
   .menubar.file.menu add command -label "Reload" -accelerator A-s \
     -command {
      if { [string compare [tk_messageBox -type okcancel -message {sure wanna reload?}] ok]==0 } {
              xschem reload
         }
     }
   .menubar.file.menu add command -label "Save as" -command "xschem saveas" -accelerator s
   .menubar.file.menu add command -label "Print" -command "xschem print" -accelerator {*}
   .menubar.file.menu add separator
   .menubar.file.menu add command -label "Exit" -command {exit} -accelerator C-d
   
   menubutton .menubar.option -text "Options" -menu .menubar.option.menu
   menu .menubar.option.menu -tearoff 0
   .menubar.option.menu add checkbutton -label "show info win" -variable show_infowindow \
     -command {
	if { $show_infowindow != 0 } {wm deiconify .infotext
        } else { destroy .infotext }
      }
   .menubar.option.menu add checkbutton -label "color postscript" -variable color_ps \
      -command {
         if { $color_ps==1 } {xschem set color_ps 1} else { xschem set color_ps 0}
      }
   .menubar.option.menu add checkbutton -label "A3 page" -variable a3page \
      -command {
         if { $a3page==1 } {xschem set a3page 1} else { xschem set a3page 0}
      }
   .menubar.option.menu add checkbutton -label "Debug mode" -variable menu_tcl_debug \
      -command {
         if { $menu_tcl_debug==1 } {xschem debug 1} else { xschem debug 0}
      }
   .menubar.option.menu add checkbutton -label "Fullscreen" -variable fullscreen \
      -command {
         xschem fullscreen
      }
   .menubar.option.menu add checkbutton -label "enable stretch" -variable enable_stretch \
      -accelerator y \
      -command {
         if { $enable_stretch==1 } {xschem set enable_stretch 1} else { xschem set enable_stretch 0} 
      }
   .menubar.option.menu add checkbutton -label "show netlist win" -variable netlist_show \
      -accelerator A \
      -command {
         if { $netlist_show==1 } {xschem set netlist_show 1} else { xschem set netlist_show 0} 
      }
   .menubar.option.menu add checkbutton -label "flat netlist" -variable flat_netlist \
      -accelerator : \
      -command {
         if { $flat_netlist==1 } {xschem set flat_netlist 1} else { xschem set flat_netlist 0} 
      }
   .menubar.option.menu add checkbutton -label "split netlist" -variable split_files \
      -accelerator : \
      -command {
         if { $split_files==1 } {xschem set split_files 1} else { xschem set split_files 0} 
      }
   .menubar.option.menu add checkbutton -label "hspice netlist" -variable hspice_netlist \
      -accelerator : \
      -command {
         if { $hspice_netlist==1 } {xschem set hspice_netlist 1} else { xschem set hspice_netlist 0} 
      }
   .menubar.option.menu add checkbutton -label "Verilog 2001 netlist variant" -variable verilog_2001 \
   
   .menubar.option.menu add checkbutton -label "draw grid" -variable draw_grid \
      -accelerator {%} \
      -command {
        if { $draw_grid == 1} { xschem set draw_grid 1; xschem redraw} else { xschem set draw_grid 0; xschem redraw}
      }
   .menubar.option.menu add checkbutton -label "Symbol text" -variable sym_txt \
      -accelerator {C-b} \
      -command {
        if { $sym_txt == 1} { xschem set sym_txt 1; xschem redraw} else { xschem set sym_txt 0; xschem redraw}
      }
   .menubar.option.menu add checkbutton -label "variable line width" -variable change_lw \
      -accelerator {_-toggle} \
      -command {
        if { $change_lw == 1} { xschem set change_lw 1} else { xschem set change_lw 0}
      }
   .menubar.option.menu add checkbutton -label "Increment Hilight Color" -variable incr_hilight \
      -command {
        if { $incr_hilight == 1} { xschem set incr_hilight 1} else { xschem set incr_hilight 0}
      }
   
   .menubar.option.menu add command -label "Set line width" \
	-command {
          input_number "Enter linewidth (float):" "xschem line_width"
	}
   .menubar.option.menu add command -label "Set symbol width" \
	-command {
          input_number "Enter Symbol width ($symbol_width)" "set symbol_width"
	}

   .menubar.option.menu add separator
   .menubar.option.menu add radiobutton -label "Vhdl netlist" -variable netlist_type -value vhdl \
	-accelerator {V-toggle} \
	-command "xschem netlist_type vhdl"
   .menubar.option.menu add radiobutton -label "Verilog netlist" -variable netlist_type -value verilog \
	-accelerator {V-toggle} \
	-command "xschem netlist_type verilog"
   .menubar.option.menu add radiobutton -label "Spice netlist" -variable netlist_type -value spice \
        -accelerator {V-toggle} \
	-command "xschem netlist_type spice"
   menubutton .menubar.edit -text "Edit" -menu .menubar.edit.menu
   menu .menubar.edit.menu -tearoff 0
   .menubar.edit.menu add command -label "Undo" -state disabled -accelerator u
   .menubar.edit.menu add command -label "Redo" -state disabled -accelerator C-r
   .menubar.edit.menu add command -label "Copy" -command "xschem copy" -accelerator C-c
   .menubar.edit.menu add command -label "Cut" -command "xschem cut"   -accelerator C-x
   .menubar.edit.menu add command -label "Paste" -command "xschem paste" -accelerator C-v
   .menubar.edit.menu add command -label "Delete" -command "xschem delete" -accelerator Del
   .menubar.edit.menu add command -label "Select all" -command "xschem select_all" -accelerator C-a
   .menubar.edit.menu add command -label "edit selected element" -command "xschem edit_in_new_window" -accelerator A-e
   .menubar.edit.menu add command -label "edit selected symbol" -command "xschem symbol_in_new_window" -accelerator A-i
   .menubar.edit.menu add command -label "Duplicate" -command "xschem copy_objects" -accelerator c
   .menubar.edit.menu add command -label "Move" -command "xschem move_objects" -accelerator m
   .menubar.edit.menu add command -label "Push schematic" -command "xschem descend" -accelerator e
   .menubar.edit.menu add command -label "Push symbol" -command "xschem edit_symbol" -accelerator i
   .menubar.edit.menu add command -label "Pop" -command "xschem go_back" -accelerator C-e
   button .menubar.netlist -text "Netlist"  -bd 0 -activebackground red  -takefocus 0\
     -command {
       xschem netlist
      }
   button .menubar.simulate -text "Simulate"  -bd 0 -activebackground red  -takefocus 0\
     -command {
       set oldbg [.menubar.simulate cget -bg]
       .menubar.simulate configure -bg red
       xschem set semaphore [expr [xschem get semaphore] +1]
       xschem simulate
       xschem set semaphore [expr [xschem get semaphore] -1]
       .menubar.simulate configure -bg $oldbg
      }
   button .menubar.waves -text "Waves"  -bd 0 -activebackground red  -takefocus 0\
     -command {
       waves [xschem get schname]
      }
   menubutton .menubar.layers -text "Layers" -menu .menubar.layers.menu \
    -background [lindex $colors 4]
   menu .menubar.layers.menu -tearoff 0
   set j 0
   foreach i $colors {
     ## 20121121
     if {  $j == [xschem get pinlayer] } { 
       set laylab PIN
       set layfg white
     } elseif { $j == [xschem get wirelayer] } { 
       set laylab WIRE
       set layfg black
     } elseif { $j == [xschem get textlayer] } { ;# 20161206
       set laylab TEXT
       set layfg black
     } else {
       set laylab {        }
       set layfg black
     }

     .menubar.layers.menu add command  -label $laylab  -foreground $layfg -background $i -activebackground $i \
        -command " xschem set rectcolor $j;  .menubar.layers configure -background $i "
     if { [expr $j%10] == 0 } { .menubar.layers.menu entryconfigure $j -columnbreak 1 }
     incr j
     
   }
   menubutton .menubar.zoom -text "View" -menu .menubar.zoom.menu
   menu .menubar.zoom.menu -tearoff 0
   .menubar.zoom.menu add command -label "Redraw" -command "xschem redraw" -accelerator Esc
   .menubar.zoom.menu add command -label "Full" -command "xschem zoom_full" -accelerator f
   .menubar.zoom.menu add command -label "In" -command "xschem zoom_in" -accelerator Z
   .menubar.zoom.menu add command -label "Out" -command "xschem zoom_out" -accelerator o
   .menubar.zoom.menu add command -label "Zoom box" -command "xschem zoom_box" -accelerator z
   .menubar.zoom.menu add command -label "Half Snap Threshold" -accelerator g -command {
     xschem set cadsnap [expr [xschem get cadsnap] / 2.0 ]
   }
   .menubar.zoom.menu add command -label "Double Snap Threshold" -accelerator G -command {
     xschem set cadsnap [expr [xschem get cadsnap] * 2.0 ]
   }
   .menubar.zoom.menu add command -label "Set snap value" \
      -command {
       input_number "Enter snap value ( default: [xschem get cadsnap_default] current: [xschem get cadsnap])" \
         "xschem set cadsnap_noalert"
	}
# 20110112
   .menubar.zoom.menu add checkbutton -label "View only Probes" -variable only_probes \
      -accelerator {5} \
      -command {
         xschem only_probes
      }
# /20110112

   menubutton .menubar.prop -text "Properties" -menu .menubar.prop.menu
   menu .menubar.prop.menu -tearoff 0
   .menubar.prop.menu add command -label "edit" -command "xschem edit_prop" -accelerator q
   .menubar.prop.menu add command -label "edit with editor" -command "xschem edit_vi_prop" -accelerator Q
   .menubar.prop.menu add command -label "view" -command "xschem view_prop" -accelerator C-q
   .menubar.prop.menu add command -background red -label "edit file (danger!)" -command "xschem edit_file" -accelerator A-q

   menubutton .menubar.sym -text "Symbol" -menu .menubar.sym.menu
   menu .menubar.sym.menu -tearoff 0
   .menubar.sym.menu add command -label "make symbol " -command "xschem make_symbol" -accelerator a

   menubutton .menubar.tools -text "Tools" -menu .menubar.tools.menu
   menu .menubar.tools.menu -tearoff 0
   .menubar.tools.menu add command -label "Insert symbol" -command "xschem place_symbol" -accelerator Ins
   .menubar.tools.menu add command -label "Insert text" -command "xschem place_text" -accelerator t
   .menubar.tools.menu add command -label "Insert wire" -command "xschem wire" -accelerator w
   .menubar.tools.menu add command -label "Insert line" -command "xschem line" -accelerator l
   .menubar.tools.menu add command -label "Insert rect" -command "xschem rect" -accelerator r
   .menubar.tools.menu add command -label "Search" -accelerator C-f -command  property_search
   .menubar.tools.menu add command -label "Align to Grid" -accelerator A-u -command  "xschem align"
   .menubar.tools.menu add command -label "Execute TCL command" -command  "tclcmd"

   menubutton .menubar.hilight -text "Hilight" -menu .menubar.hilight.menu
   menu .menubar.hilight.menu -tearoff 0
   .menubar.hilight.menu add command -label {Hilight selected net/pins} -command "xschem hilight" -accelerator k
   .menubar.hilight.menu add command -label {Un-hilight all net/pins} \
	-command "xschem delete_hilight_net" -accelerator K
   .menubar.hilight.menu add command -label {Un-hilight selected net/pins} \
	-command "xschem unhilight" -accelerator C-k
   # 20160413
   .menubar.hilight.menu add checkbutton -label {Auto-hilight net/pins} -variable auto_hilight \
      -command {
        if { $auto_hilight == 1} {
          xschem set auto_hilight 1
        } else {
          xschem set auto_hilight 0
        }
      }

   menubutton .menubar.simulation -text "Simulation" -menu .menubar.simulation.menu
   menu .menubar.simulation.menu -tearoff 0
   .menubar.simulation.menu add command -label "Set netlist Dir" \
     -command {
           # xschem clear_netlist_dir
           xschem set_netlist_dir 1
     }
   .menubar.simulation.menu add command -label {CosmoScope} -command {cosmoscope [xschem get schname]}
   .menubar.simulation.menu add command -label {WaveView} -command {waveview [xschem get schname]}
   .menubar.simulation.menu add command -label {Gtkwave} -command {gtkwave [xschem get schname]}
   .menubar.simulation.menu add command -label {Utile Stimuli Editor (GUI)} -command {utile_gui [xschem get schname]}
   .menubar.simulation.menu add command -label {Utile Stimuli Editor (Vim)} -command {utile_edit [xschem get schname]}
   .menubar.simulation.menu add command -label {Utile Stimuli Translate)} -command {utile_translate [xschem get schname]}
   .menubar.simulation.menu add command -label {Modelsim} -command {modelsim [xschem get schname]}
   .menubar.simulation.menu add command -label {Shell [current schematic library path]} \
      -command {get_shell [file dirname $XSCHEM_DESIGN_DIR/[xschem get schpath]]}
   .menubar.simulation.menu add command -label {Shell [simulation path]} \
      -command {
         if { [xschem set_netlist_dir 0] ne "" } {
           get_shell $netlist_dir
         }
       }
   .menubar.simulation.menu add command -label {Edit Netlist} -command {edit_netlist [xschem get schname]}
   .menubar.simulation.menu add separator
   .menubar.simulation.menu add radiobutton -label "CosmoScope viewer" -variable analog_viewer -value cosmoscope
   .menubar.simulation.menu add radiobutton -label "WaveView viewer" -variable analog_viewer -value waveview
   .menubar.simulation.menu add separator
   .menubar.simulation.menu add radiobutton -label "Modelsim Verilog simulator" -variable verilog_simulator -value modelsim
   .menubar.simulation.menu add radiobutton -label "Icarus Verilog simulator" -variable verilog_simulator -value iverilog
   #20170921
   .menubar.simulation.menu add separator
   .menubar.simulation.menu add radiobutton -label "Modelsim VHDL simulator" -variable vhdl_simulator -value modelsim
   .menubar.simulation.menu add radiobutton -label "GHDL VHDL simulator" -variable vhdl_simulator -value ghdl
   #20170410
   .menubar.simulation.menu add separator
   .menubar.simulation.menu add radiobutton -label "Hspicerf Spice simulator" -variable spice_simulator -value hspicerf
   .menubar.simulation.menu add radiobutton -label "Hspice Spice simulator" -variable spice_simulator -value hspice
   .menubar.simulation.menu add radiobutton -label "Finesim Spice simulator" -variable spice_simulator -value finesim

   pack .menubar.file -side left
   pack .menubar.edit -side left
   pack .menubar.option -side left
   pack .menubar.zoom -side left
   pack .menubar.prop -side left
   pack .menubar.layers -side left
   pack .menubar.tools -side left
   pack .menubar.sym -side left
   pack .menubar.hilight -side left
   pack .menubar.simulation -side left
   pack .menubar.help -side right
   pack .menubar.netlist -side right
   pack .menubar.simulate -side right
   pack .menubar.waves -side right

   frame .drw -background "" -takefocus 1

   wm  title . "XSCHEM"
   wm iconname . "XSCHEM"
   . configure  -background ""
   wm  geometry . $initial_geometry
   #wm maxsize . 1600 1200
   wm protocol . WM_DELETE_WINDOW {xschem exit}
   focus .drw

###
### Tk event handling
###
   bind .drw <Double-Button-1> {event_process -3 %x %y 0 %b 0 %s}
   bind .drw <Double-Button-2> {event_process -3 %x %y 0 %b 0 %s}
   bind .drw <Double-Button-3> {event_process -3 %x %y 0 %b 0 %s}
   bind .drw <Expose> {event_process %T %x %y 0 %w %h %s}
   bind .drw <Configure> {xschem windowid; event_process %T %x %y 0 %w %h 0}
   bind .drw <ButtonPress> {event_process %T %x %y 0 %b 0 %s}
   bind .drw <ButtonRelease> {event_process %T %x %y 0 %b 0 %s}
   bind .drw <KeyPress> {event_process %T %x %y %N 0 0 %s}
   bind .drw <KeyRelease> {event_process %T %x %y %N 0 0 %s} ;# 20161118

   bind .drw <Motion> {event_process %T %x %y 0 0 0 %s}
   #bind .drw <Motion> {puts  "xschem.tcl: Motion %T %x %y 0 0 %s"}
   #bind .drw <KeyPress> {puts "xschem.tcl: KeyPress %T %x %y %N 0 %s"}
   bind .drw  <Enter> {
    event_process %T %x %y 0 0 0 0
   }
   bind .drw <Leave> {
   }
   bind .drw <Unmap> {
    wm withdraw .infotext
    set show_infowindow 0
   }
   bind .drw  "?" { textwindow ${XSCHEM_HOME_DIR}/xschem.help }


   frame .statusbar  
   label .statusbar.1   -text "STATUS BAR 1"  
   pack .statusbar.1 -side left -fill x



   pack .drw -anchor n -side top -fill both -expand true
   pack .menubar -anchor n -side top -fill x  -before .drw
   pack .statusbar -after .drw -anchor sw  -fill x 
}

# example of bind key substitutions w <--> r
#bind .drw  "w" "event_process %T %x %y [scan r %c] 0 %s 0"
#bind .drw  "r" "event_process %T %x %y [scan w %c] 0 %s 0"



