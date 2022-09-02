#
#  File: xschem.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2021 Stefan Frederik Schippers
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

### INUTILE integration (spice stimuli generator from a higher level description language)
proc inutile_line {txtlabel} {
   global retval
   toplevel .inutile_line -class Dialog
   set X [expr [winfo pointerx .inutile_line] - 60]
   set Y [expr [winfo pointery .inutile_line] - 35]
   wm geometry .inutile_line "+$X+$Y"
   label .inutile_line.l1  -text $txtlabel
   entry .inutile_line.e1   -width 60
   .inutile_line.e1 delete 0 end
   .inutile_line.e1 insert 0 $retval
   button .inutile_line.b1 -text "OK" -command  \
   {
     set retval [.inutile_line.e1 get ]
     destroy .inutile_line
   }
   bind .inutile_line <Return> {
     set retval [.inutile_line.e1 get ]
     destroy .inutile_line
   }
   pack .inutile_line.l1 -side top -fill x
   pack .inutile_line.e1  -side top -fill both -expand yes
   pack .inutile_line.b1 -side top -fill x
   grab set .inutile_line
   focus .inutile_line.e1
   tkwait window .inutile_line
   return $retval
}

proc inutile_write_data {w f} {
 set fid [open $f "w"]
 set t [$w get  0.0 {end - 1 chars}]
 puts  -nonewline $fid $t 
 close $fid
}
  
proc inutile_read_data {w f} {
 set fid [open $f "r"]
 set t [read $fid]
 $w delete 0.0 end
 $w insert 0.0 $t
 close $fid
}

proc inutile_template {w f} {
 set fid [open $f "r"]
 set t [read $fid]
 $w insert 0.0 $t
 close $fid
}

proc inutile_get_time {} {
 global netlist_dir
 set fileid [open "$netlist_dir/inutile.simulationtime"  "RDONLY"]
 .inutile.buttons.time delete 0 end
 .inutile.buttons.time insert 0 [read -nonewline $fileid]
 close $fileid
 file delete "$netlist_dir/inutile.simulationtime"
}
 
proc inutile_alias_window {w filename} {
 catch {destroy $w}
 toplevel $w
 wm title $w "(IN)UTILE ALIAS FILE: $filename"
 wm iconname $w "ALIAS"

 set fileid [open $filename "RDONLY CREAT"]
 set testo [read $fileid]
 close $fileid
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30
 scrollbar $w.scroll -command "$w.text yview"
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.save -text Save -command "inutile_write_data $w.text \"$filename\""
 button $w.buttons.load -text Reload -command "inutile_read_data $w.text \"$filename\""
 pack $w.buttons.dismiss $w.buttons.save $w.buttons.load -side left -expand 1
 
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 $w.text insert 0.0 $testo
} 

proc inutile_help_window {w filename} {
 catch {destroy $w}
 toplevel $w
 wm title $w "(IN)UTILE ALIAS FILE"
 wm iconname $w "ALIAS"
 
 frame $w.buttons
 pack $w.buttons -side bottom -fill x -pady 2m
 button $w.buttons.dismiss -text Dismiss -command "destroy $w"
 button $w.buttons.save -text Save -command "inutile_write_data $w.text \"$filename\""
 pack $w.buttons.dismiss  $w.buttons.save -side left -expand 1
 
 text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" -setgrid 1 \
	 -height 30 -width 90
 scrollbar $w.scroll -command "$w.text yview"
 pack $w.scroll -side right -fill y
 pack $w.text -expand yes -fill both
 set fileid [open $filename "RDONLY CREAT"]
 $w.text insert 0.0 [read $fileid]
 close $fileid
} 

proc inutile_translate {f} {
  global XSCHEM_SHAREDIR netlist_dir
  set p $XSCHEM_SHAREDIR/utile
  set savedir [pwd]
  cd $netlist_dir
  eval exec awk -f $p/preprocess.awk \"$f\" | awk -f $p/expand_alias.awk | awk -f $p/param.awk | awk -f $p/clock.awk | awk -f $p/stimuli.awk
  cd $savedir
}

proc inutile { {filename {}}} {
  global XSCHEM_SHAREDIR retval netlist_dir

  toplevel .inutile
  wm title .inutile "(IN)UTILE (Stefan Schippers, sschippe)"
  wm iconname .inutile "(IN)UTILE"
  set utile_path $XSCHEM_SHAREDIR/utile
  set savedir [pwd]
  cd $netlist_dir
  set filename [file normalize $filename]
  cd $savedir
  if { ![string compare $filename  ""]  } then {
   wm withdraw .inutile
   tk_messageBox -type ok -message "Please give a file name as argument"
   exit
  }
  set retval {}
  frame .inutile.buttons
  pack .inutile.buttons -side bottom -fill x -pady 2m
  button .inutile.buttons.translate -text Translate -command "
    inutile_write_data .inutile.text \"$filename\"
    inutile_translate \"$filename\"
    inutile_get_time"
  button .inutile.buttons.dismiss -text Dismiss -command "destroy .inutile"
  button .inutile.buttons.code -text "Help" -command "inutile_help_window .inutile.help $utile_path/utile.txt"
  text .inutile.text -relief sunken -bd 2 -yscrollcommand ".inutile.scroll set" -setgrid 1 -height 30
  scrollbar .inutile.scroll -command {.inutile.text yview}
  button .inutile.buttons.save -text Save -command "
    set retval \"$filename\"
    set filename \[inutile_line {Filename}\]
    inutile_write_data .inutile.text \"$filename\""
  button .inutile.buttons.load -text Reload -command "
    set retval \"$filename\"
    set filename \[inutile_line {Filename}\]
    inutile_read_data .inutile.text \"$filename\""
  button .inutile.buttons.send -text "Template" -command "
    if { !\[string compare \[.inutile.text get 0.0 {end - 1 chars}\]  {}\]} {
      template  .inutile.text  $utile_path/template.stimuli}"
  label .inutile.buttons.timelab -text "time:"
  entry .inutile.buttons.time  -width  11
  pack .inutile.buttons.dismiss .inutile.buttons.code \
       .inutile.buttons.load .inutile.buttons.save .inutile.buttons.translate \
       .inutile.buttons.send .inutile.buttons.timelab \
       .inutile.buttons.time  -side left -expand 1
  pack .inutile.scroll -side right -fill y
  pack .inutile.text -expand yes -fill both
  if { [file exists $filename] }  {  
    set fileid [open $filename "RDONLY"]
    .inutile.text insert 0.0 [read $fileid]
    close $fileid
  }
  set tmp [.inutile.text index end]
  regsub {\..*$} $tmp {} lines
  for {set i 1} {$i <= $lines} {incr i} {
   set tmp [.inutile.text get $i.0 "$i.0 lineend"]
   if [regexp {^(include)|(\.include)} $tmp  ] { 
    inutile_alias_window .inutile.tw$i [lindex $tmp 1] 
   }
  } 
}

### End INUTILE integration

### for tclreadline: disable customcompleters
proc completer { text start end line } { return {}}

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
# execute service function
proc execute_fileevent {id} {
  global execute
  append execute(data,$id) [read $execute(pipe,$id) 1024]
  if {[eof $execute(pipe,$id)]} {
      fileevent $execute(pipe,$id) readable ""
      if { $execute(status,$id) } {
        # setting pipe to blocking before closing allows to see if pipeline failed
        # do not ask status for processes that close stdout/stderr, as eof might
        # occur before process ends and following close blocks until process terminates.
        fconfigure $execute(pipe,$id) -blocking 1
        set status 0
        if { [ info tclversion]  > 8.4} {
          set catch_return [eval catch [list {close $execute(pipe,$id)} err options] ]
        } else {
          set catch_return [eval catch [list {close $execute(pipe,$id)} err] ]
        }
        if {$catch_return} {
          if {[info tclversion] > 8.4} {
            set details [dict get $options -errorcode]
            if {[lindex $details 0] eq "CHILDSTATUS"} {
              set status [lindex $details 2]
              viewdata "Failed: $execute(cmd,$id)\nstderr:\n$err\ndata:\n$execute(data,$id)" ro
            } else {
              set status 1
              if {$execute(status,$id) } {
                viewdata "Completed: $execute(cmd,$id)\nstderr:\n$err\ndata:\n$execute(data,$id)" ro
              }
            }
          } else {
            set status 1
            if {$execute(status,$id) } {
              viewdata "Completed: $execute(cmd,$id)\nstderr:\n$err\ndata:\n$execute(data,$id)" ro
            }
          }
        }
        if {$status == 0} {
          if {$execute(status,$id) } {
            viewdata "Completed: $execute(cmd,$id)\ndata:\n$execute(data,$id)" ro
          }
        }
      } else {
        # nonblocking close always succeed 
        close $execute(pipe,$id)
      }
      if {[info exists execute(callback,$id)]} {
        eval $execute(callback,$id)
        unset execute(callback,$id)
      } 
      unset execute(pipe,$id)
      unset execute(data,$id)
      unset execute(status,$id)
      unset execute(cmd,$id)
  }
}


proc execute_wait {status args} {
  global execute 
  set id [eval execute $status $args]
  if {$id == -1} {
    return -1
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  vwait execute(pipe,$id)
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $id
}

# equivalent to the 'exec' tcl function but keeps the event loop
# responding, so widgets get updated properly
# while waiting for process to end.
proc execute {status args} {
  global execute has_x
  if {![info exists execute(id)]} {
      set execute(id) 0
  } else {
      incr execute(id)
  }
  set id $execute(id)
  if { [info exists execute(callback)] } {
    set execute(callback,$id) $execute(callback)
    unset execute(callback)
  }
  if { [catch {open "|$args" r} err] } {
    puts stderr "Proc execute error: $err"
    if { [info exists has_x]} {
        tk_messageBox -message  \
          "Can not execute '$args': ensure it is available on the system. Error: $err" \
           -icon error -parent [xschem get topwindow] -type ok
    }
    return -1
  } else {
    set pipe $err
  }
  set execute(status,$id) $status
  set execute(pipe,$id) $pipe
  set execute(cmd,$id) $args
  set execute(data,$id) ""
  fconfigure $pipe -blocking 0
  fileevent $pipe readable "execute_fileevent $id"
  return $id
}

#### Scrollable frame 
proc scrollyview {container args} {
  global ${container}_vpos ;# global to remember scrollbar position
  set_ne ${container}_vpos 0
  if {[lindex $args 0] eq {place}} {
    place ${container}.f.scrl -in $container.f -x 0 -y 0 -relwidth 1
    update ;# without this vpos of scrollbar will not be remembered when reopening toplevel
  }
  set ht [winfo height $container.f]
  set hs [winfo height $container.f.scrl]
  set frac [expr {double($ht)/$hs}]
  if { [lindex $args 0] eq {scroll}} { ;# mouse wheel
    set ${container}_vpos [expr {[set ${container}_vpos] + [lindex $args 1] *(1.0/$frac)/5}]
  } elseif { [lindex $args 0] eq {moveto}} { ;# scrollbar slider
    set ${container}_vpos [lindex $args 1]
  }
  if { [set ${container}_vpos] < 0.0 } {set ${container}_vpos 0.0}
  if { [set ${container}_vpos] > 1.0 - $frac } {set ${container}_vpos [expr {1.0 - $frac}]}
  $container.vs set [set ${container}_vpos] [expr {[set ${container}_vpos] + $frac}]
  place $container.f.scrl -in $container.f -x 0 -y [expr {-$hs * [set ${container}_vpos]}] -relwidth 1.0
}

# scrollable frame constructor
proc sframe {container} {
  frame $container.f
  scrollbar $container.vs -command "scrollyview $container" ;# scrollyview moveto commands
  frame $container.f.scrl
  pack $container.f -expand yes -fill both -side left
  pack $container.vs -expand yes -fill y
  return $container.f.scrl
}
#### /Scrollable frame

## evaluate expression. if expression has errors or does not evaluate return expression as is
proc ev {s} {
  if {![catch {expr $s} res]} {
    return [format %.4g $res]
  } else {
    return $s
  }
}
proc netlist {source_file show netlist_file} {
 global XSCHEM_SHAREDIR flat_netlist hspice_netlist netlist_dir
 global verilog_2001 debug_var OS
 
 simuldir
 set netlist_type [xschem get netlist_type]
 if {$debug_var <= -1} { puts "netlist: source_file=$source_file, netlist_type=$netlist_type" }
 set dest $netlist_dir/$netlist_file
 if {$netlist_type eq {spice}} {
   if { $hspice_netlist == 1 } {
     set simulator {-hspice}
   } else {
     set simulator {}
   }
   if { [sim_is_xyce] } {
     set xyce  {-xyce}
   } else {
     set xyce  {}
   }
   set cmd  ${XSCHEM_SHAREDIR}/spice.awk
   set brk ${XSCHEM_SHAREDIR}/break.awk
   set flatten ${XSCHEM_SHAREDIR}/flatten.awk
   if {$flat_netlist==0} {
     eval exec {awk -f $cmd -- $simulator $xyce $source_file | awk -f $brk > $dest}
   } else {
     eval exec {awk -f $cmd -- $simulator $xyce $source_file | awk -f $flatten | awk -f $brk > $dest}
   }
   if ![string compare $show "show"] {
      textwindow $dest
   }
 } 
 if {$netlist_type eq {vhdl}} {
   set cmd $XSCHEM_SHAREDIR/vhdl.awk
   eval exec {awk -f $cmd $source_file > $dest}
   if ![string compare $show "show"] {
     textwindow $dest
   }
 }
 if {$netlist_type eq {tedax}} {
    set cmd1  $XSCHEM_SHAREDIR/tedax.awk
    set cmd2 $XSCHEM_SHAREDIR/flatten_tedax.awk
    if {[catch {eval exec {awk -f $cmd1 $source_file | awk -f $cmd2 > $dest} } err] } {
     puts stderr "tEDAx errors: $err"
   }
   if ![string compare $show "show"] {
     textwindow $dest
   }
 }
 if {$netlist_type eq {verilog}} {
   set cmd  ${XSCHEM_SHAREDIR}/verilog.awk
   eval exec {awk -f $cmd $source_file > $dest}
   if { $verilog_2001==1 } { 
     set cmd ${XSCHEM_SHAREDIR}/convert_to_verilog2001.awk
     set interm ${dest}[pid]
     eval exec {awk -f $cmd $dest > $interm}
     file rename -force $interm $dest
   }
   if ![string compare $show "show"] {
     textwindow "$dest"
   }
 }
 return {}
}

# 20161121
proc convert_to_pdf {filename dest} {
  global to_pdf OS
  if { [regexp -nocase {\.pdf$} $dest] } {
    set pdffile [file rootname $filename].pdf
    # puts "---> $to_pdf $filename $pdffile"
    set cmd "exec $to_pdf \$filename \$pdffile"
    if {$OS == "Windows"} {
      set cmd "exec $to_pdf \$pdffile \$filename"
    } 
    if { ![catch {eval $cmd} msg] } {
      file rename -force $pdffile $dest
      # ps2pdf succeeded, so remove original .ps file
      if { ![xschem get debug_var] } {
        file delete $filename
      }
    } else {
      puts stderr "problems converting postscript to pdf: $msg"
    }
  } else {
    file rename -force $filename $dest
  }
}

# 20161121
proc convert_to_png {filename dest} {
  global to_png debug_var OS
  # puts "---> $to_png $filename $dest"
  set cmd "exec $to_png \$filename png:\$dest"
    if {$OS == "Windows"} {
      set cmd "exec $to_png \$dest \$filename"
    } 
  if { ![catch {eval $cmd} msg] } {
    # conversion succeeded, so remove original .xpm file
    if { ![xschem get debug_var] } {
      file delete $filename
    }
  } else {
    puts stderr "problems converting xpm to png: $msg"
  }
}

# always specify Shift- modifier for capital letters
# see tk 'man keysyms' for key names
# example format for s, d: Control-Alt-Key-asterisk
#                          Control-Shift-Key-A
#                          Alt-Key-c
#                          ButtonPress-4
#
proc key_binding {  s  d { topwin {} } } { 
  regsub {.*-} $d {} key


  switch $key {
     Insert { set keysym 65379 } 
     Escape { set keysym 65307 } 
     Return { set keysym 65293 } 
     Delete { set keysym 65535 } 
     F1 { set keysym  65470 } 
     F2 { set keysym  65471 } 
     F3 { set keysym  65472 } 
     F4 { set keysym  65473 } 
     F5 { set keysym  65474 } 
     F6 { set keysym  65475 } 
     F7 { set keysym  65476 } 
     F8 { set keysym  65477 } 
     F9 { set keysym  65478 } 
     F10 { set keysym  65479 } 
     F11 { set keysym  65480 } 
     F12 { set keysym  65481 } 
     BackSpace { set keysym 65288 } 
     default { set keysym [scan "$key" %c] }
  }
  set state 0
  # not found any portable way to get modifier constants ...
  if { [regexp {(Mod1|Alt)-} $d] } { set state [expr {$state +8}] }
  if { [regexp Control- $d] } { set state [expr {$state +4}] }
  if { [regexp Shift- $d] } { set state [expr {$state +1}] }
  if { [regexp ButtonPress-1 $d] } { set state [expr {$state +0x100}] }
  if { [regexp ButtonPress-2 $d] } { set state [expr {$state +0x200}] }
  if { [regexp ButtonPress-3 $d] } { set state [expr {$state +0x400}] }
  # puts "$state $key <${s}>"
  if {[regexp ButtonPress- $d]} {
    bind $topwin.drw "<${s}>" "xschem callback %W %T %x %y 0 $key 0 $state"
  } else {
    if {![string compare $d {} ] } {
      # puts  "bind .drw  <${s}> {}"
      bind $topwin.drw "<${s}>" {}
    } else {
      # puts  "bind .drw  <${s}> xschem callback %W %T %x %y $keysym 0 0 $state"
      bind $topwin.drw  "<${s}>" "xschem callback %W %T %x %y $keysym 0 0 $state"
    }
  }

}

proc edit_file {filename} {
 
 global editor
 # since $editor can be an executable with options (gvim -f) I *need* to use eval
 eval execute 0  $editor  $filename
 return {}
}

# ============================================================
#      SIMULATION CONTROL
# ============================================================

# ============================================================
#      SIMCONF 
# ============================================================

## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
## $s : schematic name (opamp)
## $d : netlist directory
##
## Other global vars:
## netlist_dir
## computerfarm
## terminal
## netlist_type can be obtained with [xschem get netlist_type]
proc save_sim_defaults {f} {
  global sim netlist_dir computerfarm terminal
  
  set a [catch {open $f w} fd]
  if { $a } {
    puts "save_sim_defaults: error opening file $f: $fd"
    return
  }
  puts $fd {# set the list of tools known to xschem}
  puts $fd {# Note that no spaces are allowed around commas in array keys}
  puts $fd "set sim(tool_list) {$sim(tool_list)}"
  puts $fd {}
  foreach tool $sim(tool_list) {
    puts $fd "#Specify the number of configured $tool tools."
    puts $fd "set sim($tool,n) $sim($tool,n) ;# number of configured $tool tools"
    puts $fd "# Specify the default $tool tool to use (first=0)"
    puts $fd "set sim($tool,default) $sim($tool,default) ;# default $tool tool to launch"
    puts $fd {}
    for {set i 0} {$i < $sim($tool,n)} { incr i} {
      puts $fd "# specify tool command (cmd), name (name), if tool must run\
                in foreground and if exit status must be reported"
      puts $fd "set sim($tool,$i,cmd) {$sim($tool,$i,cmd)}"
      puts $fd "set sim($tool,$i,name) {$sim($tool,$i,name)}"
      puts $fd "set sim($tool,$i,fg) $sim($tool,$i,fg)"
      puts $fd "set sim($tool,$i,st) $sim($tool,$i,st)"
      puts $fd {}
    }
    puts $fd {}
  }
  close $fd
}

proc load_recent_file {} {
  global USER_CONF_DIR recentfile has_x
  # recent files
  set recentfile {}
  if { [file exists $USER_CONF_DIR/recent_files] } {
    if {[catch { source $USER_CONF_DIR/recent_files } err] } {
      puts "Problems opening recent_files: $err"
      if {[info exists has_x]} {
        tk_messageBox -message  "Problems opening recent_files: $err" \
            -icon warning -parent . -type ok
      }
    }
  }
}

proc update_recent_file {f {topwin {} } } {
  global recentfile  has_x
  # puts "update recent file, f=$f, topwin=$topwin"
  set old $recentfile
  set recentfile {}
  lappend recentfile $f
  foreach i $old {
    if {[abs_sym_path $i] ne [abs_sym_path $f]} {
      lappend recentfile [abs_sym_path $i]
    }
  }
  # tcl8.4 errors if using lreplace past the last element
  if { [llength $recentfile] > 10  } {
    set recentfile [lreplace $recentfile 10 end]
  }
  write_recent_file
  if { [info exists has_x] } {setup_recent_menu 0 $topwin}
  if { [info exists has_x] } {setup_recent_menu 1 $topwin}
}

proc write_recent_file {} {
  global recentfile USER_CONF_DIR

  # puts "write recent file recentfile=$recentfile"
  set a [catch {open $USER_CONF_DIR/recent_files w} fd]
  if { $a } {
    puts "write_recent_file: error opening file $f: $fd"
    return
  }
  puts $fd "set recentfile {$recentfile}"
  close $fd
}

proc setup_recent_menu { {in_new_window 0} { topwin {} } } {
  global recentfile
  # puts "setup recent menu in_new_window=$in_new_window"
  if {$in_new_window} {
    $topwin.menubar.file.menu.recent_new_window delete 0 9
  } else {
    $topwin.menubar.file.menu.recent delete 0 9
  }
  set i 0
  if { [info exists recentfile] } {
    foreach i $recentfile {
      if {$in_new_window} {
        $topwin.menubar.file.menu.recent_new_window add command \
          -command "xschem load_new_window {$i}" \
          -label [file tail $i]
      } else {
        $topwin.menubar.file.menu.recent add command \
          -command "xschem load {$i}" \
          -label [file tail $i]
      }
    }
  }
}

proc sim_is_xyce {} {
  global sim

  set_sim_defaults

  if { [info exists sim(spice,default)] } {
    set idx $sim(spice,default)
    if { [regexp {[xX]yce} $sim(spice,$idx,cmd)] } {
      return 1
    }
  }
  return 0
}

# generates a proper list, trimming multiple separators
proc tolist {s} {
  set s [string trim $s]
  regsub -all {[\t\n ]+} $s { } s
  if { [string is list $s] } {
    return $s
  } else {
    return [split $s]
  }
}

proc set_sim_defaults {{reset {}}} {
  global sim terminal USER_CONF_DIR has_x bespice_listen_port env OS
  if {$reset eq {reset} } { file delete ${USER_CONF_DIR}/simrc }
  if { $reset eq {} } {
    set failure 0
    if { [info exists has_x] && [winfo exists .sim] } {
      foreach tool $sim(tool_list) {
        for {set i 0} {$i < $sim($tool,n)} { incr i} {
          set sim($tool,$i,cmd) [.sim.topf.f.scrl.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
        }
      }
    } 
    if { ![info exists sim] } {
      if { [file exists ${USER_CONF_DIR}/simrc] } {
        # get conf from simrc
        if { [catch {source ${USER_CONF_DIR}/simrc} err]} {
          puts "Problems opening simrc file: $err"
          if {[info exists has_x]} {
            tk_messageBox -message  "Problems opening simrc file: $err" -icon warning \
               -parent [xschem get topwindow] -type ok
          }
          set failure 1
        }
      }
    } 
  }
  if {( $reset eq {reset} ) || ![info exists sim] || $failure} {
    if {[info exists sim]} {unset sim}
    # no simrc, set a reasonable default
    set sim(tool_list) {spice spicewave verilog verilogwave vhdl vhdlwave}
    if {$OS == "Windows"} {
      set_ne sim(spice,0,cmd) {ngspice -i "$N" -a}
    } else {
      set_ne sim(spice,0,cmd) {$terminal -e 'ngspice -i "$N" -a || sh'}
    }
    # can not use set_ne as variables bound to entry widgets always exist if widget exists
    set sim(spice,0,name) {Ngspice}
    set_ne sim(spice,0,fg) 0
    set_ne sim(spice,0,st) 0
    
    set_ne sim(spice,1,cmd) {ngspice -b -r "$n.raw" -o "$n.out" "$N"}
    set sim(spice,1,name) {Ngspice batch}
    set_ne sim(spice,1,fg) 0
    set_ne sim(spice,1,st) 1
    
    set_ne sim(spice,2,cmd) {Xyce "$N" -r "$n.raw"}
    set sim(spice,2,name) {Xyce batch}
    set_ne sim(spice,2,fg) 0
    set_ne sim(spice,2,st) 1
    
    # number of configured spice simulators, and default one
    set_ne sim(spice,n) 3
    set_ne sim(spice,default) 0
    
    ### spice wave view
    set_ne sim(spicewave,0,cmd) {gaw "$n.raw" } 
    set sim(spicewave,0,name) {Gaw viewer}
    set_ne sim(spicewave,0,fg) 0
    set_ne sim(spicewave,0,st) 0
   
    set_ne sim(spicewave,1,cmd) {$terminal -e ngspice}
    set sim(spicewave,1,name) {Ngpice Viewer}
    set_ne sim(spicewave,1,fg) 0
    set_ne sim(spicewave,1,st) 0

    set_ne sim(spicewave,2,cmd) {rawtovcd -v 1.5 "$n.raw" > "$n.vcd" && gtkwave "$n.vcd" "$n.sav" 2>/dev/null} 
    set sim(spicewave,2,name) {Rawtovcd}
    set_ne sim(spicewave,2,fg) 0
    set_ne sim(spicewave,2,st) 0

    set_ne sim(spicewave,3,cmd) {$env(HOME)/analog_flavor_eval/bin/bspwave --socket localhost $bespice_listen_port "$n.raw" } 
    set sim(spicewave,3,name) {Bespice wave}
    set_ne sim(spicewave,3,fg) 0
    set_ne sim(spicewave,3,st) 0
    # number of configured spice wave viewers, and default one
    set_ne sim(spicewave,n) 4
    set_ne sim(spicewave,default) 0
    
    ### verilog
    set_ne sim(verilog,0,cmd) {iverilog -o .verilog_object -g2012 "$N" && vvp .verilog_object}
    set sim(verilog,0,name) {Icarus verilog}
    set_ne sim(verilog,0,fg) 0
    set_ne sim(verilog,0,st) 1
    # number of configured verilog simulators, and default one
    set_ne sim(verilog,n) 1
    set_ne sim(verilog,default) 0
    
    ### verilog wave view
    set_ne sim(verilogwave,0,cmd) {gtkwave dumpfile.vcd "$N.sav" 2>/dev/null}
    set sim(verilogwave,0,name) {Gtkwave}
    set_ne sim(verilogwave,0,fg) 0
    set_ne sim(verilogwave,0,st) 0
    # number of configured verilog wave viewers, and default one
    set_ne sim(verilogwave,n) 1
    set_ne sim(verilogwave,default) 0
    
    ### vhdl
    set_ne sim(vhdl,0,cmd) {ghdl -c --ieee=synopsys -fexplicit "$N" -r "$s" --wave="$n.ghw"}
    set sim(vhdl,0,name) {Ghdl}
    set_ne sim(vhdl,0,fg) 0
    set_ne sim(vhdl,0,st) 1
    # number of configured vhdl simulators, and default one
    set_ne sim(vhdl,n) 1
    set_ne sim(vhdl,default) 0
    
    ### vhdl wave view
    set_ne sim(vhdlwave,0,cmd) {gtkwave "$n.ghw" "$N.sav" 2>/dev/null}
    set sim(vhdlwave,0,name) {Gtkwave}
    set_ne sim(vhdlwave,0,fg) 0
    set_ne sim(vhdlwave,0,st) 0
    # number of configured vhdl wave viewers, and default one
    set_ne sim(vhdlwave,n) 1
    set_ne sim(vhdlwave,default) 0
  }
} 

proc simconf_reset {} {
  global sim

  set answer [tk_messageBox -message  "Warning: delete simulation configuration file and reset to default?" \
            -icon warning -parent .sim  -type okcancel]
  if { $answer eq {ok}} {
    set_sim_defaults reset
    foreach tool $sim(tool_list) {
      for {set i 0} { $i < $sim($tool,n)} {incr i} {
        .sim.topf.f.scrl.center.$tool.r.$i.cmd delete 1.0 end
        .sim.topf.f.scrl.center.$tool.r.$i.cmd insert 1.0 $sim($tool,$i,cmd)
      }
    }
  }
}

proc simconf_saveconf {scrollframe} {
  global sim USER_CONF_DIR
  foreach tool $sim(tool_list) {
    for {set i 0} { $i < $sim($tool,n)} {incr i} {
      set sim($tool,$i,cmd) [${scrollframe}.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
    }
  }
  # destroy .sim
  # xschem set semaphore [expr {[xschem get semaphore] -1}]
  save_sim_defaults ${USER_CONF_DIR}/simrc
  # puts "saving simrc"
}
 
proc simconf {} {
  global sim USER_CONF_DIR simconf_default_geometry

  if {[winfo exists .sim]} {
    destroy .sim 
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  set_sim_defaults
  toplevel .sim -class dialog
  wm title .sim {Simulation Configuration}
  wm geometry .sim 700x340
  frame .sim.topf
  set scrollframe [sframe .sim.topf]
  frame ${scrollframe}.top
  frame ${scrollframe}.center
  frame .sim.bottom
  pack ${scrollframe}.top -fill x 
  pack ${scrollframe}.center -fill both -expand yes
  set bg(0) {#dddddd}
  set bg(1) {#aaaaaa}
  set toggle 0
  foreach tool $sim(tool_list) {
    frame ${scrollframe}.center.$tool
    label ${scrollframe}.center.$tool.l -width 12 -text $tool  -bg $bg($toggle)
    frame ${scrollframe}.center.$tool.r
    pack ${scrollframe}.center.$tool -fill both -expand yes
    pack ${scrollframe}.center.$tool.l -fill y -side left
    pack ${scrollframe}.center.$tool.r -fill both -expand yes
    for {set i 0} { $i < $sim($tool,n)} {incr i} {
      frame ${scrollframe}.center.$tool.r.$i
      pack ${scrollframe}.center.$tool.r.$i -fill x -expand yes
      entry ${scrollframe}.center.$tool.r.$i.lab -textvariable sim($tool,$i,name) -width 15 -bg $bg($toggle)
      radiobutton ${scrollframe}.center.$tool.r.$i.radio -bg $bg($toggle) \
         -variable sim($tool,default) -value $i
      text ${scrollframe}.center.$tool.r.$i.cmd -width 20 -height 3 -wrap none -bg $bg($toggle)
      ${scrollframe}.center.$tool.r.$i.cmd insert 1.0 $sim($tool,$i,cmd)
      checkbutton ${scrollframe}.center.$tool.r.$i.fg -text Fg -variable sim($tool,$i,fg) -bg $bg($toggle)
      checkbutton ${scrollframe}.center.$tool.r.$i.st -text Status -variable sim($tool,$i,st) -bg $bg($toggle)

      pack ${scrollframe}.center.$tool.r.$i.lab -side left -fill y 
      pack ${scrollframe}.center.$tool.r.$i.radio -side left -fill y 
      pack ${scrollframe}.center.$tool.r.$i.cmd -side left -fill x -expand yes
      pack ${scrollframe}.center.$tool.r.$i.fg -side left -fill y 
      pack ${scrollframe}.center.$tool.r.$i.st -side left -fill y 
    }
    incr toggle
    set toggle [expr {$toggle %2}]
  }
  button .sim.bottom.cancel  -text Cancel -command {
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  button .sim.bottom.help  -text Help -command {
    viewdata {In this dialog box you set the commands xschem uses to launch the 
various external tools.
Xschem has 3 main netlisting modes (spice, verilog, vhdl) and for each
netlisting mode some simulators and some viewers can be defined.
The following variables are defined and will get substituted by
XSCHEM before sending commands to the shell:

 - N: complete filename of netlist for current netlisting mode
   (example: /home/schippes/.xschem/simulations/opamp.spice for spice)
   (example: /home/schippes/.xschem/simulations/opamp.v for verilog)
 - n: complete filename of netlist as above but without extension
   (example: /home/schippes/.xschem/simulations/opamp)
 - S: full pathname of schematic being used (example: 
      /home/schippes/.xschem/xschem_library/opamp.sch)
 - s: name of schematic being used (example: opamp)
 - d: simulation directory (example: /home/schippes/.xschem/simulations)
 - terminal: terminal to be used for applications that need to be
   executed in terminal (example: $terminal -e ngspice -i "$N" -a)
If for a given tool there are multiple rows then the radiobutton
tells which one will be called by xschem.
Variables should be used with the usual substitution character $: $n, $N, etc.
Foreground (Fg) checkbutton tells xschem to wait for child process to finish.
Status checkbutton tells xschem to report a status dialog (stdout, stderr,
exit status) when process finishes.
Any changes made in the command or tool name entries will be saved in 
~/.xschem/simrc when 'Save Configuration to file' button is pressed.
If 'Accept and Close' is pressed then the changes are kept in memory and dialog
is closed without writing to a file, if xschem is restarted changes will be lost.
If no ~/.xschem/simrc is present then a minimal default setup is presented.
To reset to default use the corresponding button or just delete the ~/.xschem/simrc
file manually.
    } ro
  }
  button .sim.bottom.ok  -text {Save Configuration to file} -command "simconf_saveconf $scrollframe"
  button .sim.bottom.reset -text {Reset to default} -command {
    simconf_reset
  }
  button .sim.bottom.close -text {Accept and Close} -command {
    set_sim_defaults
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  wm protocol .sim WM_DELETE_WINDOW {
    set_sim_defaults 
    destroy .sim
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  pack .sim.bottom.cancel -side left -anchor w
  pack .sim.bottom.help -side left
  #foreach tool $sim(tool_list) {
  #  button .sim.bottom.add${tool} -text +${tool} -command "
  #    simconf_add $tool
  #    destroy .sim
  #    xschem set semaphore [expr {[xschem get semaphore] -1}]
  #    save_sim_defaults ${USER_CONF_DIR}/simrc
  ##    simconf
  #  "
  #  pack .sim.bottom.add${tool} -side left
  #}
  pack .sim.bottom.ok -side right -anchor e
  pack .sim.bottom.close -side right
  pack .sim.bottom.reset -side right
  pack .sim.topf -fill both -expand yes
  pack .sim.bottom -fill x
  if { [info exists simconf_default_geometry]} {
     wm geometry .sim "${simconf_default_geometry}"
  }
 
  bind .sim.topf.f <Configure> {scrollyview .sim.topf}
  bind .sim <Configure> {
    set simconf_default_geometry [wm geometry .sim]
  }
  bind .sim <ButtonPress-4> { scrollyview .sim.topf scroll -0.2}
  bind .sim <ButtonPress-5> { scrollyview .sim.topf scroll 0.2}
  scrollyview .sim.topf place
  set maxsize [expr {[winfo height ${scrollframe}] + [winfo height .sim.bottom]}]
  wm maxsize .sim 9999 $maxsize
  # tkwait window .sim
}

proc simconf_add {tool} {
  global sim
  set n $sim($tool,n)
  set sim($tool,$n,cmd) {}
  set sim($tool,$n,name) {}
  set sim($tool,$n,fg) 0
  set sim($tool,$n,st) 0
  incr sim($tool,n)
}

proc bespice_getdata {sock} {
  global bespice_server_getdata
  if {[eof $sock] || [catch {gets $sock bespice_server_getdata(line,$sock)}]} {
    close $sock
    puts "Close $bespice_server_getdata(addr,$sock)"
    unset bespice_server_getdata(addr,$sock)
    unset bespice_server_getdata(line,$sock)
    unset bespice_server_getdata(sock)
  } else {
    puts "bespice --> $bespice_server_getdata(line,$sock)"
    set bespice_server_getdata(last) $bespice_server_getdata(line,$sock)
  }
}

proc xschem_getdata {sock} {
  global xschem_server_getdata
  if {[eof $sock] || [catch {gets $sock xschem_server_getdata(line,$sock)}]} {
    close $sock
    puts "Close $xschem_server_getdata(addr,$sock)"
    unset xschem_server_getdata(addr,$sock)
    unset xschem_server_getdata(line,$sock)
    unset xschem_server_getdata(res,$sock)
  } else {
    puts "tcp--> $xschem_server_getdata(line,$sock)"
    # xschem command must be executed at global scope...
    uplevel #0 [list catch $xschem_server_getdata(line,$sock) xschem_server_getdata(res,$sock)]
    puts $sock "$xschem_server_getdata(res,$sock)"
  }
} 

proc bespice_server {sock addr port} {
  global bespice_server_getdata
  if { ![info exists bespice_server_getdata(sock)] } {
    puts "Accept $sock from $addr port $port"
    fconfigure $sock -buffering line
    set bespice_server_getdata(addr,$sock) [list $addr $port]
    set bespice_server_getdata(sock) [list $sock]
    fileevent $sock readable [list bespice_getdata $sock]
  }
}


proc xschem_server {sock addr port} {
  global xschem_server_getdata
  puts "Accept $sock from $addr port $port"
  fconfigure $sock -buffering line
  set xschem_server_getdata(addr,$sock) [list $addr $port]
  fileevent $sock readable [list xschem_getdata $sock]
}

## given a path (x1.x2.m4) descend into x1.x2 and return m4 whether m4 found or not 
proc descend_hierarchy {path {redraw 1}} {
  xschem set no_draw 1
  # return to top level if not already there
  while { [xschem get currsch] } { xschem go_back } 
  # recursively descend into sub-schematics
  while { [regexp {\.} $path] } {
    xschem unselect_all
    set inst $path
    regsub {\..*} $inst {} inst    ;# take 1st path component: xlev1[3].xlev2.m3 -> xlev1[3]
    regsub {[^.]+\.} $path {} path ;# take remaining path: xlev1[3].xlev2.m3 -> xlev2.m3
    xschem search exact 1 name $inst
    # handle vector instances: xlev1[3:0] -> xlev1[3],xlev1[2],xlev1[1],xlev1[0]
    # descend into the right one
    set inst_list [split [lindex [xschem expandlabel [lindex [xschem selected_set] 0 ] ] 0] {,}]
    set instnum [expr {[lsearch -exact  $inst_list $inst] + 1}]
    xschem descend $instnum
  }
  xschem set no_draw 0
  if {$redraw} {xschem redraw}
  return $path
}

## given a hierarchical instname name (x1.xamp.m1) go down in the hierarchy and 
## select the specified instance (m1).
## this search assumes it is given from the top of hierarchy
proc select_inst {fullinst {redraw 1 } } {
  xschem set no_draw 1
  set inst [descend_hierarchy $fullinst 0] 
  set res [xschem select instance $inst]
  # if nothing found return to top
  if {!$res} {
    while { [xschem get currsch] } { xschem go_back } 
  }
  xschem set no_draw 0
  if {$redraw} {xschem redraw}
  if {$res} {return $inst} else { return {} } 
}

proc pin_label {} {
  if { [file exists [abs_sym_path devices/lab_pin.sym]] } {
    return {devices/lab_pin.sym}
  }
  return {lab_pin.sym}
}

## given a hierarchical net name x1.xamp.netname go down in the hierarchy and 
## highlight the specified net.
## this search assumes it is given from the top of hierarchy
proc probe_net {fullnet {redraw 1} } {
  xschem set no_draw 1
  set net [descend_hierarchy $fullnet 0]
  set res [xschem hilight_netname $net]
  if {$res==0  && [regexp {^net[0-9]+$} $net]} {
    set net \#$net
    set res [xschem hilight_netname $net]
  }
  if {!$res} {
    while { [xschem get currsch] } { xschem go_back } 
  }
  xschem set no_draw 0
  if {$redraw} {xschem redraw}
  if {$res} {return $net} else { return {} } 
}

# backannotate newnet to be connected to specified hierarchical instance name and pin.
# places a label close to the instance pin to be re-routed.
# actual reconnect is human assisted! 
proc reroute_inst {fullinst pinattr pinval newnet} {
  if { [regexp {\.} $fullinst] } { set hier 1 } else { set hier 0 } 
  set res [descend_hierarchy $fullinst 0]
  if {$res ne {} } {
    set coord [xschem instance_pin_coord $res $pinattr $pinval]
    if { $coord eq {} } {
      while { [xschem get currsch] } { xschem go_back } 
      return 0
    }
    set pinname [lindex $coord 0]
    set x [expr {[lindex $coord 1] - 10} ]
    set y [expr {[lindex $coord 2] - 10} ]
    set oldnet [xschem instance_net $res $pinname]

    regsub {.*\.} $newnet {} newnet
    if { $oldnet eq $newnet } {
      while { [xschem get currsch] } { xschem go_back } 
      puts "Warning: netlist patch already done? "
      return 0
    }
 
    xschem instance [pin_label] $x $y 0 0 [list name=l1 lab=$newnet]
    xschem hilight_netname $newnet
    xschem select instance $res
    xschem hilight_netname $oldnet
    if {$hier} { xschem save} ;# save so we can process other reroute_inst without beink asked to save.
    xschem redraw
    return 1
  }
  return 0
}

## put $new net labels close to pins on all elements connected to $old
proc reroute_net {old new} {
  xschem push_undo
  xschem set no_undo 1
  xschem unhilight
  probe_net $old
  set old_nopath [regsub {.*\.} $old {}]
  set new_nopath [regsub {.*\.} $new {}]
  set devlist [xschem instances_to_net $old_nopath]
  foreach i $devlist {
    set instname [lindex $i 0]
    set x  [expr {[lindex $i 2] - 10}]
    set y  [expr {[lindex $i 3] - 10}]
    xschem instance [pin_label] $x $y 0 0 [list name=l1 lab=$new_nopath]
    xschem select instance $instname
  }
  xschem hilight_netname $new_nopath
  xschem set no_undo 0
}

proc simulate {{callback {}}} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp)
  ## $S : schematic name full path (/home/schippes/.xschem/xschem_library/opamp.sch)
  ## $d : netlist directory

  global netlist_dir computerfarm terminal sim
  global execute XSCHEM_SHAREDIR has_x OS

  simuldir 
  set_sim_defaults
  set netlist_type [xschem get netlist_type]
  if { [select_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool $netlist_type
    set S [xschem get schname]
    set s [file tail [file rootname $S]]
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }
    if { ![info exists  sim($tool,default)] } {
      if { $has_x} {alert_ "Warning: simulator for $tool is not configured"}
      puts "Warning: simulator for $tool is not configured"
      return
    }
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst $sim($tool,$def,cmd)]
    if {$OS == "Windows"} {
      # $cmd cannot be surrounded by {} as exec will change forward slash to backward slash
      if { $callback ne {} } {
        eval $callback
      }
      #eval exec {cmd /V /C "cd $netlist_dir&&$cmd}
      eval exec $cmd &
    } else {
      set execute(callback) $callback
      $fg $st sh -c "cd $netlist_dir; $cmd"
    }
  }
}

proc gaw_echoline {} {
  global gaw_fd
  gets $gaw_fd line
  if {[eof $gaw_fd]} {
     puts "finishing connection from gaw"
     close $gaw_fd
     unset gaw_fd
  } else {
    # generate a variable event we can vwait for
    set gaw_fd $gaw_fd
  }
  puts "gaw -> $line"
}

proc setup_tcp_gaw {} {
  global gaw_fd gaw_tcp_address netlist_dir has_x
 
  if { [info exists gaw_fd] } { return 1; } 
  simuldir
  set s [file tail [file rootname [xschem get schname 0]]]

  if { ![info exists gaw_fd] && [catch {eval socket $gaw_tcp_address} gaw_fd] } {
    puts "Problems opening socket to gaw on address $gaw_tcp_address"
    unset gaw_fd
    if {[info exists has_x]} {
      tk_messageBox -type ok -title {Tcp socket error} \
       -message [concat "Problems opening socket to gaw on address $gaw_tcp_address. " \
         "Ensure the following line is present uncommented in ~/.gaw/gawrc: up_listenPort = 2020." \
         "If you recently closed gaw the port may be in a TIME_WAIT state for a minute or so ." \
         "Close gaw, Wait a minute or two, then send waves to gaw again."]
    }
    return 0
  }
  chan configure $gaw_fd -blocking 1 -buffering line -encoding binary -translation binary
  fileevent $gaw_fd readable gaw_echoline
  puts $gaw_fd "table_set $s.raw"
  return 1
}

proc gaw_cmd {cmd} {
  global gaw_fd gaw_tcp_address netlist_dir has_x

  simuldir
  if { ![info exists gaw_fd] && [catch {eval socket $gaw_tcp_address} gaw_fd] } {
    puts "Problems opening socket to gaw on address $gaw_tcp_address"
    unset gaw_fd
    if {[info exists has_x]} {
      tk_messageBox -type ok -title {Tcp socket error} \
       -message [concat "Problems opening socket to gaw on address $gaw_tcp_address. " \
         "If you recently closed gaw the port may be in a TIME_WAIT state for a minute or so ." \
         "Close gaw, Wait a minute or two, then send waves to gaw again."]
    }
    return
  }
  chan configure $gaw_fd -blocking 0 -buffering line -encoding binary -translation binary
  puts $gaw_fd "$cmd"
  set n [regexp -all \n $cmd]
  incr n
  puts "gaw command lines: $n"
  fileevent $gaw_fd readable gaw_echoline
  while { $n} { 
    #timeout for abnormal deadlocks 
    set wd [after 10000 set gaw_fd stalled]
    vwait gaw_fd
    if { $gaw_fd ne {stalled} } {
      after cancel $wd
    } else { 
      puts "timeout waiting for gaw response.."
      break
    }
    incr n -1
  }
  close $gaw_fd
  unset gaw_fd
}

proc waves {} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp)
  ## $S : schematic name full path (/home/schippes/.xschem/xschem_library/opamp.sch)
  ## $d : netlist directory

  global netlist_dir computerfarm terminal sim XSCHEM_SHAREDIR has_x 
  global bespice_listen_port env

  simuldir
  set netlist_type [xschem get netlist_type]
  set_sim_defaults
  if { [select_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool ${netlist_type}
    set S [xschem get schname]
    set s [file tail [file rootname $S]]
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }
    set tool ${tool}wave
    if { ![info exists  sim($tool,default)] } {
      if { $has_x} {alert_ "Warning: viewer for $tool is not configured"}
      puts "Warning: viewer for $tool is not configured"
      return
    }
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst $sim($tool,$def,cmd)]
    $fg $st sh -c "cd $netlist_dir; $cmd"
  }
}
# ============================================================




# allow change color (via graph_change_wave_color) of double clicked wave
proc graph_edit_wave {n n_wave} {
  global graph_sel_color graph_selected colors graph_sel_wave
  global graph_schname
  set graph_schname [xschem get schname]
  set_ne graph_sel_color 4
  set graph_selected $n
  set graph_sel_wave $n_wave
  set col  [xschem getprop rect 2 $graph_selected color]
  set node [xschem getprop rect 2 $graph_selected node]
  # add default colors if unspecified in col
  set i 0
  foreach graph_node $node {
    if {[lindex $col $i] eq {}} { lappend col $graph_sel_color}
    incr i
  }
  # remove excess colors
  set col [lrange $col 0 [expr {$i - 1}]]
  set graph_sel_color [lindex $col $graph_sel_wave]
  xschem setprop rect 2 $graph_selected color $col fast
  xschem draw_graph  $graph_selected
  toplevel .graphdialog
  frame .graphdialog.f
  button .graphdialog.ok -text OK -command {destroy .graphdialog}
  button .graphdialog.cancel -text Cancel -command {destroy .graphdialog}
  for {set i 4} {$i < 22} {incr i} {
    radiobutton .graphdialog.f.r$i -value $i -bg [lindex $colors $i] \
         -variable graph_sel_color -command {graph_change_wave_color $graph_sel_wave }
    pack .graphdialog.f.r$i -side left -fill both -expand yes
  }
  grid .graphdialog.f  - -sticky nsew
  grid .graphdialog.ok .graphdialog.cancel -sticky ew
  grid rowconfig .graphdialog 0 -weight 1
  grid column .graphdialog 0 -weight 1
  grid column .graphdialog 1 -weight 1
  tkwait window .graphdialog
  set graph_schname {}
}


# get selected text from a text widget:
#
# .graphdialog.center.right.text1 get sel.first sel.last
#
# see if a selection is present:
# .graphdialog.center.right.text1 tag ranges sel
#
# replace selected text: 
# .graphdialog.center.right.text1 replace sel.first sel.last BUS
#
# programmatically select text:
# .graphdialog.center.right.text1 tag add sel 1.0 {end - 1 chars}
# clear selection
# .graphdialog.center.right.text1 tag remove sel 1.0 end
# get position of cursor:
# .graphdialog.center.right.text1  index insert
# set cursor position: 
# .graphdialog.center.right.text1 mark set insert 2.18


# add nodes from provided list of {node color} .... 
proc graph_add_nodes_from_list {nodelist} {
  global graph_bus graph_selected graph_schname
  set sel {}
  if {$graph_bus} {
    set sep ,
  } else {
    set sep \n
  }

  if { [winfo exists .graphdialog] } {
    set current_node_list [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
    set col  [xschem getprop rect 2 $graph_selected color]
    if {[string length $current_node_list] > 0 && ![regexp "\n$" $current_node_list]} {
      .graphdialog.center.right.text1 insert end \n
    }
    set change_done 0
    set first 0
    foreach {i c} $nodelist {
      if {$sel ne {}} {append sel $sep}
      if {!$first  || !$graph_bus } {
        regsub {\[.*} $i {} busname
        lappend col $c
      }
      append sel $i
      set change_done 1
      set first 1
    }
    if {$change_done && $graph_bus} {
      set sel "[string toupper $busname],${sel}\n"
    } else {
      set sel "${sel}\n"
    }
    if {$change_done} {
      .graphdialog.center.right.text1 insert end $sel
      if { [xschem get schname] eq $graph_schname } {
        set node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
        xschem setprop rect 2 $graph_selected color $col fastundo
        graph_update_nodelist
        regsub -all {\\?(["\\])} $node {\\\1} node_quoted ;#"4vim
        xschem setprop rect 2 $graph_selected node $node_quoted fast
        xschem draw_graph $graph_selected
      }
    }
  } else {
    set graph_bus 0
    set nn {}
    set cc {}
    foreach {n c} $nodelist {
      if { $nn ne {}} {append nn \n}
      if { $cc ne {}} {append cc " "}
      append nn $n
      append cc $c
    }

    set nnn [xschem getprop rect 2 [xschem get graph_lastsel] node]
    set ccc [xschem getprop rect 2 [xschem get graph_lastsel] color]
    if { $nnn ne {}} {append nnn "\n"}
    if { $ccc ne {}} {append ccc " "}
    append nnn $nn
    append ccc $cc
    regsub -all {\\?(["\\])} $nnn {\\\1} node_quoted ;#"4vim
    xschem setprop rect 2 [xschem get graph_lastsel] color $ccc fastundo
    xschem setprop rect 2 [xschem get graph_lastsel] node $node_quoted fast
    xschem draw_graph [xschem get graph_lastsel]
  }
}

# add nodes from left listbox
proc graph_add_nodes {} {
  global graph_bus
  set sel_idx [.graphdialog.center.left.list1 curselection]
  set sel {}
  if {$graph_bus} {
    set sep ,
  } else {
    set sep \n
  }
  set current_node_list [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
  if {[string length $current_node_list] > 0 && ![regexp "\n$" $current_node_list]} {
    .graphdialog.center.right.text1 insert end \n
  }
  set change_done 0
  foreach i $sel_idx {
    set c [.graphdialog.center.left.list1 get $i]
    set c [regsub -all {([][])}  $c {\\\1}]
    if { ![regexp "(^|\[ \t\n\])${c}($|\[ \t\n\])" $current_node_list]} {
      if {$sel ne {}} {append sel $sep}
      append sel [.graphdialog.center.left.list1 get $i]
      set change_done 1
    }
  }
  if {$change_done && $graph_bus} {
    set sel "BUS_NAME,${sel}\n"
  } else {
    set sel "${sel}\n"
  }
  if {$change_done} {
    .graphdialog.center.right.text1 insert {insert lineend + 1 char} $sel
  }
}

proc graph_get_signal_list {siglist pattern } {
  global graph_sort
  set direction {-decreasing}
  if {$graph_sort} {set direction {-increasing}}
  set result {}
  set siglist [join [lsort $direction -dictionary $siglist] \n]
  # just check if pattern is a valid regexp
  set err [catch {regexp $pattern {12345}} res]
  if {$err} {set pattern {}}
  foreach i $siglist {
    regsub {^v\((.*)\)$} $i {\1} i
    if {[regexp $pattern $i] } {
       lappend result $i
    }
  }
  return $result
}

# change color of selected wave in text widget and redraw graph
# OR
# change color attribute of wave given as parameter, redraw graph
proc graph_change_wave_color {{wave {}}} {
  global graph_sel_color graph_selected graph_schname

  if { [xschem get schname] ne $graph_schname } return
  #  get tag the cursor is on:
  if { $wave eq {}} {
    set tag [.graphdialog.center.right.text1 tag names insert]
    if { [regexp {^t} $tag]} {
      set index [string range $tag 1 end]
      set col  [xschem getprop rect 2 $graph_selected color]
      set col [lreplace $col $index $index  $graph_sel_color]
      xschem setprop rect 2 $graph_selected color $col fast
      graph_update_nodelist
      xschem draw_graph $graph_selected
    }
  # wave to change provided as parameter
  } else {
    set col  [xschem getprop rect 2 $graph_selected color]
    set col [lreplace $col $wave $wave  $graph_sel_color]
    xschem setprop rect 2 $graph_selected color $col fast
    xschem draw_graph $graph_selected
  }
}

# tag nodes in text widget with assigned colors 
proc graph_update_nodelist {} {
  global graph_selected colors graph_sel_color graph_schname
  if { [xschem get schname] ne $graph_schname } return
  # delete old tags
  eval .graphdialog.center.right.text1 tag delete [ .graphdialog.center.right.text1 tag names]
  # tagging nodes in text widget:
  set col  [xschem getprop rect 2 $graph_selected color]
  set col [string trim $col " \n"]

  set regx {(?:"[^"]+")|(?:[^\n \t]+)}
  set txt [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]
  set tt {}
  set cc {}
  set start 0
  while {[regexp -indices -start $start $regx $txt idx]} {
    lappend tt [lindex $idx 0]
    set start [expr {[lindex $idx 1] + 1}]
    lappend cc $start
  }

  set n 0
  if { $tt ne {} } {
    foreach t $tt c $cc {
      set col_idx [lindex $col $n]
      # add missing colors
      if {$col_idx eq {}} {
        set col_idx $graph_sel_color
        lappend col $graph_sel_color
      }
      set b [lindex $colors $col_idx]  
      .graphdialog.center.right.text1 tag add t$n "1.0 + $t chars" "1.0 + $c chars"
      .graphdialog.center.right.text1 tag configure t$n -background $b -selectbackground grey40
      incr n
    }
    # remove excess colors
    set col [lrange $col 0 [expr {$n - 1}]]
    ## for debug
    # if { [llength $col] != [llength [tolist [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}]]] } {
    #   puts "PROBLEMS: colors and nodes of different length"
    # }
  } else {
    set col {}
  }
  xschem setprop rect 2 $graph_selected color $col fast
}

proc fill_graph_listbox {} {
  set retval [.graphdialog.top.search get]
  set retval [graph_get_signal_list [xschem raw_query list] $retval]
  .graphdialog.center.left.list1 delete 0 end
  eval .graphdialog.center.left.list1 insert 0 $retval
}

# called from event handlers (OK, KeyRelease, DoubleClick) in graph_edit_properties
proc update_graph_node {node} {
  global graph_selected
  graph_update_nodelist
  regsub -all {\\?(["\\])} $node {\\\1} node_quoted ;#"4vim
  xschem setprop rect 2 $graph_selected node $node_quoted fast
  xschem draw_graph $graph_selected
}

proc graph_edit_properties {n} {
  global graph_bus graph_sort graph_digital graph_selected colors graph_sel_color
  global graph_unlocked graph_schname

  xschem push_undo
  set geom {}
  if { [winfo exists .graphdialog]} {
    set geom [winfo geometry .graphdialog]
  } 
  catch {destroy .graphdialog}
  toplevel .graphdialog -width 1 -height 1
  update idletasks
  if {$geom ne {}} { wm geometry .graphdialog $geom}

  set graph_selected $n
  set graph_schname [xschem get schname]
  set_ne graph_sel_color 4
  set_ne graph_sort 0
  set graph_digital 0
  if {[xschem getprop rect 2 $n digital] == 1} {set graph_digital 1}
  if {[regexp {unlocked} [xschem getprop rect 2 $n flags]]} {
    set graph_unlocked 1
  } else {
    set graph_unlocked 0
  }
  
  frame .graphdialog.top
  # another row of buttons
  frame .graphdialog.top2 
  panedwindow .graphdialog.center -orient horiz
  frame .graphdialog.bottom
  frame .graphdialog.center.left
  frame .graphdialog.center.right
  .graphdialog.center add .graphdialog.center.left .graphdialog.center.right
  pack .graphdialog.top -side top -fill x 
  pack .graphdialog.top2 -side top -fill x 
  pack .graphdialog.center -side top -fill both -expand yes
  pack .graphdialog.bottom -side top -fill x 

  # center-left frame
  label .graphdialog.center.left.lab1 -text {Signal list}
  button .graphdialog.center.left.add -text Add -command {
    graph_add_nodes; graph_update_nodelist
  }
  listbox .graphdialog.center.left.list1 -width 20 -height 10 -selectmode extended \
     -yscrollcommand {.graphdialog.center.left.yscroll set} \
     -xscrollcommand {.graphdialog.center.left.xscroll set}
  scrollbar .graphdialog.center.left.yscroll -command {.graphdialog.center.left.list1 yview}
  scrollbar .graphdialog.center.left.xscroll -orient horiz -command {.graphdialog.center.left.list1 xview}
  grid .graphdialog.center.left.lab1 .graphdialog.center.left.add
  grid .graphdialog.center.left.list1 - .graphdialog.center.left.yscroll -sticky nsew
  grid .graphdialog.center.left.xscroll - -sticky nsew
  grid rowconfig .graphdialog.center.left 0 -weight 0
  grid rowconfig .graphdialog.center.left 1 -weight 1 -minsize 100
  grid columnconfig .graphdialog.center.left 0 -weight 1
  grid columnconfig .graphdialog.center.left 1 -weight 1

  # center right frame
  label .graphdialog.center.right.lab1 -text {Signals in graph}
  checkbutton .graphdialog.center.right.unlocked -text {Unlocked X axis} -variable graph_unlocked
  text .graphdialog.center.right.text1 -wrap none -width 50 -height 10 -bg grey70 -fg black \
     -insertbackground grey40 -exportselection 1 \
     -yscrollcommand {.graphdialog.center.right.yscroll set} \
     -xscrollcommand {.graphdialog.center.right.xscroll set}
  scrollbar .graphdialog.center.right.yscroll -command {.graphdialog.center.right.text1 yview}
  scrollbar .graphdialog.center.right.xscroll -orient horiz -command {.graphdialog.center.right.text1 xview}

  grid .graphdialog.center.right.lab1 .graphdialog.center.right.unlocked
  grid .graphdialog.center.right.text1 - .graphdialog.center.right.yscroll -sticky nsew
  grid .graphdialog.center.right.xscroll - -sticky nsew
  grid rowconfig .graphdialog.center.right 0 -weight 0
  grid rowconfig .graphdialog.center.right 1 -weight 1 -minsize 100
  grid columnconfig .graphdialog.center.right 0 -weight 1
  grid columnconfig .graphdialog.center.right 1 -weight 1

  # bottom frame
  button .graphdialog.bottom.cancel -text Cancel -command {
    destroy .graphdialog
    set graph_selected {}
    set graph_schname {}
  }
  button .graphdialog.bottom.ok -text OK -command {
    if { [xschem get schname] eq $graph_schname } {

      update_graph_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
      xschem setprop rect 2 $graph_selected y1 [.graphdialog.top.min get] fast
      xschem setprop rect 2 $graph_selected y2 [.graphdialog.top.max get] fast

      if {$graph_unlocked} {
        xschem setprop rect 2 $graph_selected flags {graph,unlocked} fast
      } else {
        xschem setprop rect 2 $graph_selected flags {graph} fast
      }
      destroy .graphdialog
      set graph_selected {}
      set graph_schname {}
    } else {
      destroy .graphdialog
      set graph_selected {}
      set graph_schname {}
    }
  }
  button .graphdialog.bottom.apply -text Apply -command {
    if { [xschem get schname] eq $graph_schname } {

      update_graph_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
      xschem setprop rect 2 $graph_selected y1 [.graphdialog.top.min get] fast
      xschem setprop rect 2 $graph_selected y2 [.graphdialog.top.max get] fast
      if {$graph_unlocked} {
        xschem setprop rect 2 $graph_selected flags {graph,unlocked} fast
      } else {
        xschem setprop rect 2 $graph_selected flags {graph} fast
      }
    }
  }

  # top packs
  pack .graphdialog.bottom.ok -side left
  pack .graphdialog.bottom.apply -side left
  pack .graphdialog.bottom.cancel -side left

  for {set i 4} {$i < 22} {incr i} {
    radiobutton .graphdialog.bottom.r$i -value $i -bg [lindex $colors $i] \
      -variable graph_sel_color -command graph_change_wave_color
    pack .graphdialog.bottom.r$i -side left
  }

  # top2 frame
  label .graphdialog.top2.labunitx -text {X units}
  spinbox .graphdialog.top2.unitx -values {p n u m 1 k M G} -width 2 \
   -command {
      xschem setprop rect 2 $graph_selected unitx [.graphdialog.top2.unitx get]
      xschem draw_graph $graph_selected
    }

  label .graphdialog.top2.labunity -text {  Y units}
  spinbox .graphdialog.top2.unity -values {p n u m 1 k M G} -width 2 \
   -command {
      xschem setprop rect 2 $graph_selected unity [.graphdialog.top2.unity get]
      xschem draw_graph $graph_selected
    }

  label .graphdialog.top2.labdivx -text {  X div.}
  entry .graphdialog.top2.divx -width 2
  bind .graphdialog.top2.divx <KeyRelease> {
    xschem setprop rect 2 $graph_selected divx [.graphdialog.top2.divx get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top2.labdivy -text {  Y div.}
  entry .graphdialog.top2.divy -width 2
  bind .graphdialog.top2.divy <KeyRelease> {
    xschem setprop rect 2 $graph_selected divy [.graphdialog.top2.divy get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top2.labsubdivx -text {  X subdiv.}
  entry .graphdialog.top2.subdivx  -width 2
  bind .graphdialog.top2.subdivx <KeyRelease> {
    xschem setprop rect 2 $graph_selected subdivx [.graphdialog.top2.subdivx get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top2.labsubdivy -text {  Y subdiv.}
  entry .graphdialog.top2.subdivy -width 2
  bind .graphdialog.top2.subdivy <KeyRelease> {
    xschem setprop rect 2 $graph_selected subdivy [.graphdialog.top2.subdivy get]
    xschem draw_graph $graph_selected
  }
  
  label .graphdialog.top2.labdset -text {  Dataset}
  entry .graphdialog.top2.dset -width 4
  bind .graphdialog.top2.dset <KeyRelease> {
    xschem setprop rect 2 $graph_selected dataset [.graphdialog.top2.dset get]
    xschem draw_graph $graph_selected
  }
  
  .graphdialog.top2.dset insert 0 [xschem getprop rect 2 $graph_selected dataset]
  set graph_divx [xschem getprop rect 2 $graph_selected divx]
  if {$graph_divx eq {}} { set graph_divx 5}
  .graphdialog.top2.divx insert 0 $graph_divx
  set graph_divy [xschem getprop rect 2 $graph_selected divy]
  if {$graph_divy eq {}} { set graph_divy 5}
  .graphdialog.top2.divy insert 0 $graph_divy
  set graph_subdivx [xschem getprop rect 2 $graph_selected subdivx]
  if {$graph_subdivx eq {}} { set graph_subdivx 1}
  .graphdialog.top2.subdivx insert 0 $graph_subdivx
  set graph_subdivy [xschem getprop rect 2 $graph_selected subdivy]
  if {$graph_subdivy eq {}} { set graph_subdivy 1}
  .graphdialog.top2.subdivy insert 0 $graph_subdivy
  set graph_unitx [xschem getprop rect 2 $graph_selected unitx]
  if {$graph_unitx eq {}} { set graph_unitx 1}
  .graphdialog.top2.unitx set $graph_unitx
  set graph_unity [xschem getprop rect 2 $graph_selected unity]
  if {$graph_unity eq {}} { set graph_unity 1}
  .graphdialog.top2.unity set $graph_unity
  pack .graphdialog.top2.labunitx .graphdialog.top2.unitx \
       .graphdialog.top2.labunity .graphdialog.top2.unity -side left

  pack .graphdialog.top2.labdivx .graphdialog.top2.divx \
       .graphdialog.top2.labdivy .graphdialog.top2.divy \
       .graphdialog.top2.labsubdivx .graphdialog.top2.subdivx \
       .graphdialog.top2.labsubdivy .graphdialog.top2.subdivy \
       .graphdialog.top2.labdset .graphdialog.top2.dset -side left

  # top frame
  label .graphdialog.top.labsearch -text Search:
  entry .graphdialog.top.search -width 10 
  checkbutton .graphdialog.top.bus -text Bus -padx 2 -variable graph_bus
  checkbutton .graphdialog.top.incr -text {Incr. sort} -variable graph_sort -indicatoron 1 \
    -command fill_graph_listbox
  checkbutton .graphdialog.top.dig -text {Digital} -variable graph_digital -indicatoron 1 \
    -command {
       if { [xschem get schname] eq $graph_schname } {
         xschem setprop rect 2 $graph_selected digital $graph_digital fast
         xschem draw_graph $graph_selected
       }
     }

  label .graphdialog.top.labmin -text {  Min Value:}
  entry .graphdialog.top.min -width 5
  bind .graphdialog.top.min <KeyRelease> {
    xschem setprop rect 2 $graph_selected y1 [.graphdialog.top.min get]
    xschem draw_graph $graph_selected
  }

  label .graphdialog.top.labmax -text {  Max Value:}
  entry .graphdialog.top.max -width 5
  bind .graphdialog.top.max <KeyRelease> {
    xschem setprop rect 2 $graph_selected y2 [.graphdialog.top.max get]
    xschem draw_graph $graph_selected
  }

  button .graphdialog.top.clear -text Clear -padx 2 -command {
    .graphdialog.top.search delete 0 end
    fill_graph_listbox 
  }
  pack .graphdialog.top.labsearch .graphdialog.top.search -side left
  pack .graphdialog.top.clear -side left
  pack .graphdialog.top.incr -side left
  pack .graphdialog.top.bus -side left
  pack .graphdialog.top.dig -side left
  pack .graphdialog.top.labmin .graphdialog.top.min .graphdialog.top.labmax .graphdialog.top.max -side left
  .graphdialog.top.min insert 0 [xschem getprop rect 2 $graph_selected y1]
  .graphdialog.top.max insert 0 [xschem getprop rect 2 $graph_selected y2]

  # binding
  bind .graphdialog.top.search <KeyRelease> {
    fill_graph_listbox
  }
  bind .graphdialog.center.left.list1 <Double-Button-1> {
    graph_add_nodes
    if { [xschem get schname] eq $graph_schname } {
      update_graph_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
    }
  }

  bind .graphdialog.center.right.text1 <KeyRelease> {
    if { [xschem get schname] eq $graph_schname } {
      update_graph_node [string trim [.graphdialog.center.right.text1 get 1.0 {end - 1 chars}] " \n"]
    }
  }
  bind .graphdialog <Control-Return> {
    .graphdialog.bottom.ok invoke
  }
  bind .graphdialog <Escape> {
    .graphdialog.bottom.cancel invoke
  }
  wm protocol .graphdialog  WM_DELETE_WINDOW {
    .graphdialog.bottom.cancel invoke
  }
  
  # fill data in left listbox
  eval .graphdialog.center.left.list1 insert 0 [graph_get_signal_list [xschem raw_query list] {}]

  # fill data in right textbox
  set plotted_nodes [xschem getprop rect 2 $n node]
  if {[string length $plotted_nodes] > 0 && [string index $plotted_nodes end] ne "\n"} {append plotted_nodes \n}
  .graphdialog.center.right.text1 insert 1.0 $plotted_nodes
  graph_update_nodelist
  # add stuff in textbox at end of line + 1 char (after newline) 
  # .graphdialog.center.right.text1 insert {insert lineend + 1 char} foo\n
  # tkwait window .graphdialog
}

proc graph_show_measure {{action show}} {
  global measure_id measure_text
 
  set_ne measure_text "y=\nx="
  if { [info exists measure_id] } {
    after cancel $measure_id
    unset measure_id
  }
  destroy .measure
  if {$action eq {stop}} { return }
  set measure_id [after 400 {
    unset measure_id
    toplevel .measure -bg {}
    label .measure.lab -text $measure_text -bg black -fg yellow -justify left
    pack .measure.lab
    wm overrideredirect .measure 1
    wm geometry .measure +[expr {[winfo pointerx .measure] +10}]+[expr {[winfo pointery .measure] -8}]
  }]
}

proc get_shell { curpath } {
 global netlist_dir debug_var
 global  terminal

 simuldir
 execute 0 sh -c "cd $curpath && $terminal"
}

proc edit_netlist {schname } {
 global netlist_dir debug_var
 global editor terminal OS

 simuldir
 set netlist_type [xschem get netlist_type]
 set tmpname [file rootname "$schname"]

 if { [regexp vim $editor] } { set ftype "-c \":set filetype=$netlist_type\"" } else { set ftype {} }
 if { [select_netlist_dir 0] ne "" } {
   # puts "edit_netlist: \"$editor $ftype  ${schname}.v\" \"$netlist_dir\" bg"
   if { $netlist_type=="verilog" } {
     execute 0  sh -c "cd $netlist_dir && $editor $ftype  \"${tmpname}.v\""
   } elseif { $netlist_type=="spice" } {
     if {$OS == "Windows"} {
       set cmd "$editor \"$netlist_dir/${tmpname}.spice\""
       eval exec $cmd &
     } else {
       execute 0  sh -c "cd $netlist_dir && $editor $ftype \"${tmpname}.spice\""
     }
   } elseif { $netlist_type=="tedax" } {
     if {$OS == "Windows"} {
       set cmd "$editor \"$netlist_dir/${tmpname}.tdx\""
       eval exec $cmd &
     } else {
       execute 0 sh -c "cd $netlist_dir && $editor $ftype \"${tmpname}.tdx\""
     }
   } elseif { $netlist_type=="vhdl" } { 
     if {$OS == "Windows"} {
       set cmd "$editor \"$netlist_dir/${tmpname}.vhdl\""
       eval exec $cmd &
     } else {
       execute 0 sh -c "cd $netlist_dir && $editor $ftype \"${tmpname}.vhdl\""
     }
   }
 }
 return {}
}


# 20180926
# global_initdir should be set to:
#   INITIALLOADDIR  for load
#   INITIALINSTDIR  for instance placement
# ext:  .sch or .sym or .sch.sym or .sym.sch
#
proc save_file_dialog { msg ext global_initdir {initialf {}} {overwrt 1} } {
  upvar #0 $global_initdir initdir
  set temp $initdir
  if { $initialf ne {}} {
    set initialdir [file dirname $initialf]
    set initialf [file tail $initialf]
  } else {
    set initialdir $initdir
    set initialf {}
  }
  set initdir $initialdir
  set r [load_file_dialog $msg $ext $global_initdir 0 $overwrt $initialf] 
  set initdir $temp
  return $r
}

proc is_xschem_file {f} {
  set a [catch {open "$f" r} fd]
  set ret 0
  set score 0
  set instances 0
  if {$a} {
    puts stderr "Can not open file $f"
  } else {
    while { [gets $fd line] >=0 } {
      if { [regexp {^[TKGVSE] \{} $line] } { incr score }
      if { [regexp {^[BL] +[0-9]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } {incr score}
      if { [regexp {^N +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } {incr score}
      if { [regexp {^C +\{[^{}]+\} +[-0-9.eE]+ +[-0-9.eE]+ +[0-3]+ +[0-3]+ +\{} $line] } {incr instances; incr score}
      if { [regexp "^v\[ \t\]+\{xschem\[ \t\]+version\[ \t\]*=.*\[ \t\]+file_version\[ \t\]*=" $line] } {
        set ret 1
      }
    } 
    if { $score > 6 }  { set ret 1} ;# Heuristic decision :-)
    if { $ret } {
      if { $instances} {
        set ret SCHEMATIC
      } else { 
        set ret SYMBOL
      }
    }
    close $fd
  }
  # puts "score=$score"
  return $ret
}

proc myload_set_colors1 {} {
  global myload_files1 dircolor
  for {set i 0} { $i< [.dialog.l.paneleft.list index end] } { incr i} {
    set name "[lindex $myload_files1 $i]"
    .dialog.l.paneleft.list itemconfigure $i -foreground black -selectforeground black
    foreach j [array names dircolor] {
      set pattern $j
      set color $dircolor($j)
      if { [regexp $pattern $name] } {
        .dialog.l.paneleft.list itemconfigure $i -foreground $color -selectforeground $color
      }
    }
  }
}

proc myload_set_colors2 {} {
  global myload_index1 myload_files2 dircolor
  set dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
  for {set i 0} { $i< [.dialog.l.paneright.list index end] } { incr i} {
    set name "$dir1/[lindex $myload_files2 $i]"
    if {[ file isdirectory $name]} {
      .dialog.l.paneright.list itemconfigure $i -foreground blue
      foreach j [array names dircolor] {
        set pattern $j 
        set color $dircolor($j)
        if { [regexp $pattern $name] } {
          .dialog.l.paneright.list itemconfigure $i -foreground $color -selectforeground $color
        }
      }

    } else {
      .dialog.l.paneright.list itemconfigure $i -foreground black
    }
  }
}

proc myload_set_home {dir} {
  global pathlist  myload_files1 myload_index1

  set curr_dirname [xschem get current_dirname]
  .dialog.l.paneleft.list selection clear 0 end
  if { $dir eq {.}} { set dir $curr_dirname}
  # puts "set home: dir=$dir, pathlist=$pathlist"
  set pl {}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]} {
      set path_elem $curr_dirname
    }
    lappend pl $path_elem
  }
  set i [lsearch -exact $pl $dir]
  if { $i>=0 } {
    set myload_files1 $pathlist
    update
    myload_set_colors1
    .dialog.l.paneleft.list xview moveto 1
    set myload_index1 $i
    .dialog.l.paneleft.list selection set $myload_index1
  } else {
    set myload_files1 [list $dir]
    update
    myload_set_colors1
    .dialog.l.paneleft.list xview moveto 1
    set myload_index1 0
    .dialog.l.paneleft.list selection set 0
  }
}

proc setglob {dir} {
      global globfilter myload_files2
      set myload_files2 [lsort [glob -nocomplain -directory $dir -tails -type d .* *]]
      if { $globfilter eq {*}} {
        set myload_files2 ${myload_files2}\ [lsort [glob -nocomplain -directory $dir -tails -type {f} .* $globfilter]]
      } else {
        set myload_files2 ${myload_files2}\ [lsort [glob -nocomplain -directory $dir -tails -type {f} $globfilter]]
      }
}

proc load_file_dialog_mkdir {dir} {
  global myload_dir1
  if { $dir ne {} } {
    file mkdir "${myload_dir1}/$dir"
    setglob ${myload_dir1}
    myload_set_colors2
  }
}
proc load_file_dialog_up {dir} {
  global myload_dir1
  bind .dialog.l.paneright.drw <Expose> {}
  .dialog.l.paneright.drw configure -background white
  set d [file dirname $dir]
  if { [file isdirectory $d]} {
    myload_set_home $d
    setglob $d
    myload_set_colors2
    set myload_dir1 $d
  }
}

proc load_file_dialog {{msg {}} {ext {}} {global_initdir {INITIALINSTDIR}} 
     {loadfile {1}} {confirm_overwrt {1}} {initialf {}}} {
  global myload_index1 myload_files2 myload_files1 myload_retval myload_dir1 pathlist OS
  global myload_default_geometry myload_sash_pos myload_yview tcl_version globfilter myload_dirs2
  global save_initialfile

  set save_initialfile $initialf
  set globfilter *
  set myload_retval {} 
  upvar #0 $global_initdir initdir
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class dialog
  wm title .dialog $msg
  set_ne myload_index1 0
  if { ![info exists myload_files1]} {
    set myload_files1 $pathlist
    set myload_index1 0
  }
  set_ne myload_files2 {}
  panedwindow  .dialog.l -orient horizontal
  frame .dialog.l.paneleft
  if {$tcl_version > 8.5} { set just {-justify right}} else {set just {}}
  eval [subst {listbox .dialog.l.paneleft.list -listvariable myload_files1 -width 20 -height 12 $just \
    -yscrollcommand ".dialog.l.paneleft.yscroll set" -selectmode browse \
    -xscrollcommand ".dialog.l.paneleft.xscroll set" -exportselection 0}]
  myload_set_colors1
  scrollbar .dialog.l.paneleft.yscroll -command ".dialog.l.paneleft.list yview" 
  scrollbar .dialog.l.paneleft.xscroll -command ".dialog.l.paneleft.list xview" -orient horiz
  pack  .dialog.l.paneleft.yscroll -side right -fill y
  pack  .dialog.l.paneleft.xscroll -side bottom -fill x
  pack  .dialog.l.paneleft.list -fill both -expand true
  bind .dialog.l.paneleft.list <<ListboxSelect>> { 
    # bind .dialog.l.paneright.drw <Expose> {}
    # .dialog.l.paneright.drw configure -background white
    set myload_sel [.dialog.l.paneleft.list curselection]
    if { $myload_sel ne {} } {
      set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_sel]]
      set myload_index1 $myload_sel
      set globfilter *
      if {$save_initialfile eq {}} {.dialog.buttons_bot.entry delete 0 end}
      setglob $myload_dir1
      myload_set_colors2
    }
  }
  frame .dialog.l.paneright
  frame .dialog.l.paneright.drw -background white -width 200 -height 200
  listbox .dialog.l.paneright.list  -listvariable myload_files2 -width 20 -height 12\
    -yscrollcommand ".dialog.l.paneright.yscroll set" -selectmode browse \
    -xscrollcommand ".dialog.l.paneright.xscroll set" -exportselection 0
  scrollbar .dialog.l.paneright.yscroll -command ".dialog.l.paneright.list yview"
  scrollbar .dialog.l.paneright.xscroll -command ".dialog.l.paneright.list xview" -orient horiz
  pack .dialog.l.paneright.drw -side bottom -anchor s -fill x 
  pack  .dialog.l.paneright.yscroll -side right -fill y
  pack  .dialog.l.paneright.xscroll -side bottom -fill x
  pack  .dialog.l.paneright.list -side bottom  -fill both -expand true

  .dialog.l  add .dialog.l.paneleft -minsize 40
  .dialog.l  add .dialog.l.paneright -minsize 40
  # .dialog.l paneconfigure .dialog.l.paneleft -stretch always
  # .dialog.l paneconfigure .dialog.l.paneright -stretch always
  frame .dialog.buttons 
  frame .dialog.buttons_bot
  button .dialog.buttons_bot.ok -width 5 -text OK -command {
    set myload_retval [.dialog.buttons_bot.entry get]
    destroy .dialog
  } 
  button .dialog.buttons_bot.cancel -width 5 -text Cancel -command {
    set myload_retval {}
    destroy .dialog
  }
  button .dialog.buttons.home -width 5 -text {Home} -command {
    bind .dialog.l.paneright.drw <Expose> {}
    .dialog.l.paneright.drw configure -background white
    set myload_files1 $pathlist
    update
    myload_set_colors1
    .dialog.l.paneleft.list xview moveto 1
    set myload_index1 0
    set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
    setglob $myload_dir1
    myload_set_colors2
    .dialog.l.paneleft.list selection clear 0 end
    .dialog.l.paneright.list selection clear 0 end
    .dialog.l.paneleft.list selection set $myload_index1
  }
  label .dialog.buttons_bot.label  -text {  File/Search:}
  entry .dialog.buttons_bot.entry
  if { $save_initialfile ne {} } { 
    .dialog.buttons_bot.entry insert 0 $save_initialfile
  }
  bind .dialog.buttons_bot.entry <KeyRelease> {
    if {$save_initialfile eq {} } {
      set globfilter  *[.dialog.buttons_bot.entry get]*
      if { $globfilter eq {**} } { set globfilter * }
      setglob $myload_dir1
    }
  }
  radiobutton .dialog.buttons_bot.all -text All -variable globfilter -value {*} \
     -command { setglob $myload_dir1 }
  radiobutton .dialog.buttons_bot.sym -text .sym -variable globfilter -value {*.sym} \
     -command { setglob $myload_dir1 }
  radiobutton .dialog.buttons_bot.sch -text .sch -variable globfilter -value {*.sch} \
     -command { setglob $myload_dir1 }
  button .dialog.buttons.up -width 5 -text Up -command {load_file_dialog_up  $myload_dir1}
  label .dialog.buttons.mkdirlab -text { New dir: } -fg blue
  entry .dialog.buttons.newdir -width 16
  button .dialog.buttons.mkdir -width 5 -text Create -fg blue -command { 
    load_file_dialog_mkdir [.dialog.buttons.newdir get]
  }
  button .dialog.buttons.rmdir -width 5 -text Delete -fg blue -command { 
    if { [.dialog.buttons.newdir get] ne {} } {
      file delete "${myload_dir1}/[.dialog.buttons.newdir get]"
      setglob ${myload_dir1}
      myload_set_colors2
    }
  }
  button .dialog.buttons.pwd -text {Current dir} -command {load_file_dialog_up  [xschem get schname]}
  pack .dialog.buttons.home .dialog.buttons.up .dialog.buttons.pwd -side left
  pack .dialog.buttons.mkdirlab -side left
  pack .dialog.buttons.newdir -expand true -fill x -side left
  pack .dialog.buttons.rmdir .dialog.buttons.mkdir -side right
  pack .dialog.buttons_bot.all .dialog.buttons_bot.sym .dialog.buttons_bot.sch -side left
  pack .dialog.buttons_bot.label -side left
  pack .dialog.buttons_bot.entry -side left -fill x -expand true
  pack .dialog.buttons_bot.cancel .dialog.buttons_bot.ok -side left
  pack .dialog.l -expand true -fill both
  pack .dialog.buttons -side top -fill x
  pack .dialog.buttons_bot -side top -fill x
  if { [info exists myload_default_geometry]} {
     wm geometry .dialog "${myload_default_geometry}"
  }
  myload_set_home $initdir
  bind .dialog <Return> { 
    set myload_retval [.dialog.buttons_bot.entry get]
    if {$myload_retval ne {} } {
      destroy .dialog
    }
  }
  bind .dialog.l.paneright.list <Double-Button-1> {
    set myload_retval [.dialog.buttons_bot.entry get]
    if {$myload_retval ne {}  && ![file isdirectory "$myload_dir1/[.dialog.l.paneright.list get $myload_sel]"]} {
      bind .dialog.l.paneright.drw <Expose> {}
      destroy .dialog
    }
  }
  bind .dialog <Escape> {
    set myload_retval {}
    destroy .dialog
  }

  ### update
  if { [ info exists myload_sash_pos] } {
    eval .dialog.l sash mark 0 [.dialog.l sash coord 0]
    eval .dialog.l sash dragto 0 [subst $myload_sash_pos]
  }
  ### update
  .dialog.l.paneleft.list xview moveto 1
  bind .dialog <Configure> {
    set myload_sash_pos [.dialog.l sash coord 0]
    set myload_default_geometry [wm geometry .dialog]
    .dialog.l.paneleft.list xview moveto 1
    # regsub {\+.*} $myload_default_geometry {} myload_default_geometry
  }

  bind .dialog.l.paneright.yscroll <Motion> {
    set myload_yview [.dialog.l.paneright.list yview]
  }

  xschem preview_window create .dialog.l.paneright.drw {}
  set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
  setglob $myload_dir1
  myload_set_colors2

  bind .dialog.l.paneright.list <ButtonPress> { 
    set myload_yview [.dialog.l.paneright.list yview]
  }
  bind .dialog.l.paneright.list <<ListboxSelect>> {
    set myload_yview [.dialog.l.paneright.list yview] 
    set myload_sel [.dialog.l.paneright.list curselection]
    if { $myload_sel ne {} } {
      set myload_dir1 [abs_sym_path [.dialog.l.paneleft.list get $myload_index1]]
      set myload_dir2 [.dialog.l.paneright.list get $myload_sel]
      if {$myload_dir2 eq {..}} {
        set myload_d [file dirname $myload_dir1]
      } elseif { $myload_dir2 eq {.} } {
        set myload_d  $myload_dir1
      } else {
        if {$OS == "Windows"} {
          if {[regexp {^[A-Za-z]\:/$} $myload_dir1]} {
            set myload_d "$myload_dir1$myload_dir2"
          } else {
            set myload_d "$myload_dir1/$myload_dir2"
          }
        } else {
          if {$myload_dir1 eq "/"} {
            set myload_d "$myload_dir1$myload_dir2"
          } else {
            set myload_d "$myload_dir1/$myload_dir2"
          }
        }
      }
      if { [file isdirectory $myload_d]} {
        bind .dialog.l.paneright.drw <Expose> {}
        .dialog.l.paneright.drw configure -background white
        myload_set_home $myload_d
        setglob $myload_d
        myload_set_colors2
        set myload_dir1 $myload_d
        # .dialog.buttons_bot.entry delete 0 end
      } else {
        set globfilter *
        .dialog.buttons_bot.entry delete 0 end
        .dialog.buttons_bot.entry insert 0 $myload_dir2
         set myload_type [is_xschem_file $myload_dir1/$myload_dir2]
         if { $myload_type ne {0}  } {
	   ### update
           if { [winfo exists .dialog] } {
             .dialog.l.paneright.drw configure -background {}
             xschem preview_window draw .dialog.l.paneright.drw "$myload_dir1/$myload_dir2"
             bind .dialog.l.paneright.drw <Expose> {
               xschem preview_window draw .dialog.l.paneright.drw "$myload_dir1/$myload_dir2"
             }
           }
         } else {
           bind .dialog.l.paneright.drw <Expose> {}
           .dialog.l.paneright.drw configure -background white
         }
         # puts "xschem preview_window draw .dialog.l.paneright.drw \"$myload_dir1/$myload_dir2\""
      }
    }
  }
  if { [ info exists myload_yview]} {
   .dialog.l.paneright.list yview moveto  [lindex $myload_yview 0]
  }
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  xschem preview_window destroy {} {} 
  set initdir "$myload_dir1"
  if { $myload_retval ne {}} {
    if {![file exists "$myload_dir1/$myload_retval"] } {
      return "$myload_dir1/$myload_retval"
    }
    if { $loadfile == 0 } {
      if {[file exists "$myload_dir1/$myload_retval"]} {
        if {$confirm_overwrt == 1 } {
          set answer [tk_messageBox -message  "Overwrite $myload_dir1/${myload_retval}?" \
               -icon warning -parent [xschem get topwindow] -type okcancel]
        } else {
          set answer ok
        }
        if {$answer eq {ok}} {
          return "$myload_dir1/$myload_retval"
        } else { 
          return {}
        }
      }
    }
    set myload_type [is_xschem_file "$myload_dir1/$myload_retval"]
    if { $myload_type eq {0}  } {
      set answer [
        tk_messageBox -message "$myload_dir1/$myload_retval does not seem to be an xschem file...\nContinue?" \
         -icon warning -parent [xschem get topwindow] -type yesno]
      if { $answer eq "no"} {
        set myload_retval {}
        return {}
      } else {
        return "$myload_dir1/$myload_retval"
      }
    } elseif { $myload_type ne {SYMBOL} && ($ext eq {.sym}) } {
      set answer [
        tk_messageBox -message "$myload_dir1/$myload_retval does not seem to be a SYMBOL file...\nContinue?" \
           -icon warning -parent [xschem get topwindow] -type yesno]
      if { $answer eq "no"} {
        set myload_retval {}
        return {}
      } else {
        return "$myload_dir1/$myload_retval"
      }
    } else {
      return "$myload_dir1/$myload_retval"
    }
  } else {
    return {}
  }
}

# get last n path components: example , n=1 --> /aaa/bbb/ccc/ddd.sch -> ccc/ddd.sch
proc get_cell {s n } {
  set slist [file split $s]
  set l [llength $slist]
  if { $n >= $l } {set n [expr {$l - 1}]}
  set p {}
  for {set i [expr {$l-1-$n}]} {$i < $l} { incr i } {
    append p [lindex $slist $i]
    if {$i < $l - 1} {
      append p {/}
    }
  }
  return $p
}

# chop last n path components from s
proc path_head {s n } {
  set slist [file split $s]
  set l [llength $slist]
  if { $n < 0 } { set n 0 }
  set p {}
  for {set i 0} {$i < [expr {$l - $n}]} { incr i } {
    append p [lindex $slist $i]
    if {$i < $l -$n- 1 && ([lindex $slist $i] ne {/})} {
      append p {/}
    }
  }
  return $p
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
  global env retval USER_CONF_DIR
  global filetmp

  set retval [ read_data_nonewline $filetmp ]
  regsub -all {<} $retval {[} retval 
  regsub -all {>} $retval {]} retval 
  set lines [split $retval \n]
  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines { 
    if {$indirect} {
      puts $fd "C \{[rel_sym_path devices/[lindex $i 1].sym]\} 0 [set y [expr {$y-20}]] \
                0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
    } else {
      puts $fd "C \{[rel_sym_path [lindex $i 1].sym]\} 0 [set y [expr {$y-20}]] \
                0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
    }
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}

proc rectorder {x1 y1 x2 y2} {
  if {$x2 < $x1} {set tmp $x1; set x1 $x2; set x2 $tmp}
  if {$y2 < $y1} {set tmp $y1; set y1 $y2; set y2 $tmp}
  return [list $x1 $y1 $x2 $y2]
}

proc order {x1 y1 x2 y2} {
  if {$x2 < $x1} {set tmp $x1; set x1 $x2; set x2 $tmp; set tmp $y1; set y1 $y2; set y2 $tmp
  } elseif {$x2==$x1 && $y2<$y1} {set tmp $y1; set y1 $y2; set y2 $tmp}
  return [list $x1 $y1 $x2 $y2]
}

proc rotation {x0 y0 x y rot flip} {
  set tmp [expr {$flip? 2*$x0-$x : $x}]
  if {$rot==0} {set rx $tmp; set ry $y }
  if {$rot==1} {set rx [expr {$x0 - $y +$y0}]; set ry [expr {$y0+$tmp-$x0}]}
  if {$rot==2} {set rx [expr {2*$x0-$tmp}]; set ry [expr {2*$y0-$y}]}
  if {$rot==3} {set rx [expr {$x0+$y-$y0}]; set ry [expr {$y0-$tmp+$x0}]}
  return [list $rx $ry]
}

proc schpins_to_sympins {} {
  global env USER_CONF_DIR
  set pinhsize 2.5
  set first 1
  xschem copy
  set clipboard [read_data_nonewline $USER_CONF_DIR/.clipboard.sch]
  set lines [split $clipboard \n]
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    set ii [split [regexp -all -inline {\S+} $i]]
    if {[regexp {^C \{.*(i|o|io)pin} $i ]} {
      if {[regexp {ipin} [lindex $ii 1]]} { set dir in }
      if {[regexp {opin} [lindex $ii 1]]} { set dir out }
      if {[regexp {iopin} [lindex $ii 1]]} { set dir inout }
      set rot [lindex $ii 4]
      set flip [lindex $ii 5]
      while {1} {
        if { [regexp {lab=} $i] } {
          regsub {^.*lab=} $i {} lab
          regsub {[\} ].*} $lab {} lab
        } 
        if { [regexp {\}} $i]} { break} 
      }
      set x0 [lindex $ii 2]
      set y0 [lindex $ii 3]
      if {$first} {
        puts $fd "G {$x0 $y0 }"
        set first 0
      }
      set pinx1 [expr {$x0-$pinhsize}]
      set pinx2 [expr {$x0+$pinhsize}]
      set piny1 [expr {$y0-$pinhsize}]
      set piny2 [expr {$y0+$pinhsize}]
      if {![string compare $dir  "out"] || ![string compare $dir "inout"] } {
        set linex1 [expr {$x0-20}]
        set liney1 $y0
        set linex2 $x0
        set liney2 $y0
        set textx0 [expr {$x0-25}] 
        set texty0 [expr {$y0-4}]
        set textflip [expr {!$flip}]
      } else {
        set linex1 [expr {$x0+20}]
        set liney1 $y0
        set linex2 $x0
        set liney2 $y0
        set textx0 [expr {$x0+25}]
        set texty0 [expr {$y0-4}]
        set textflip [expr {$flip}]
      }
      lassign [rotation $x0 $y0 $linex1 $liney1 $rot $flip] linex1 liney1
      lassign [rotation $x0 $y0 $linex2 $liney2 $rot $flip] linex2 liney2
      lassign [order $linex1 $liney1 $linex2 $liney2] linex1 liney1 linex2 liney2
      lassign [rotation $x0 $y0 $textx0 $texty0 $rot $flip] textx0 texty0
      puts $fd "B 5 $pinx1 $piny1 $pinx2 $piny2 \{name=$lab dir=$dir\}"
      puts $fd "L 4 $linex1 $liney1 $linex2 $liney2 \{\}"
      puts $fd "T \{$lab\} $textx0 $texty0 $rot $textflip 0.2 0.2 \{\}"
    }
  }
  close $fd
  xschem paste
}


proc add_lab_no_prefix {} { 
  global env retval USER_CONF_DIR
  global filetmp

  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  set retval [ read_data_nonewline $filetmp ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {$indirect} {
      puts $fd "C \{devices/lab_pin.sym\} 0 [set y [expr {$y+20}]] \
               0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
    } else {
      puts $fd "C \{lab_pin.sym\} 0 [set y [expr {$y+20}]] \
               0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
    }
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}

proc add_lab_prefix {} {
  global env retval USER_CONF_DIR
  global filetmp

  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  set retval [ read_data_nonewline $filetmp ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {$indirect} {
      puts $fd "C \{devices/lab_pin.sym\} 0 [set y [expr {$y+20}]] \
               0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
    } else {
      puts $fd "C \{lab_pin.sym\} 0 [set y [expr {$y+20}]] \
               0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
    }
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}

proc make_symbol {name} {
  global XSCHEM_SHAREDIR symbol_width
  set name [abs_sym_path $name ]
  # puts "make_symbol{}, executing: ${XSCHEM_SHAREDIR}/make_sym.awk $symbol_width ${name}"
  eval exec {awk -f ${XSCHEM_SHAREDIR}/make_sym.awk $symbol_width $name}
  return {}
}

proc make_symbol_lcc {name} {
  global XSCHEM_SHAREDIR
  set name [abs_sym_path $name]
  # puts "make_symbol{}, executing: ${XSCHEM_SHAREDIR}/make_sym_lcc.awk ${name}"
  eval exec {awk -f ${XSCHEM_SHAREDIR}/make_sym_lcc.awk $name}
  return {}
}

# create simulation dir 'simulation/' under current schematic directory
proc simuldir {} {
  global netlist_dir local_netlist_dir
  if { $local_netlist_dir == 1 } {
    set simdir [xschem get current_dirname]/simulation
    file mkdir $simdir
    set netlist_dir $simdir
    return $netlist_dir
  }
  return {}
}

#
# force==0: force creation of $netlist_dir (if not empty)
#           if netlist_dir empty and no dir given prompt user
#           else set netlist_dir to dir
#
# force==1: if no dir given prompt user
#           else set netlist_dir to dir
#
proc select_netlist_dir { force {dir {} }} {
  global netlist_dir env OS

  if { ( $force == 0 )  && ( $netlist_dir ne {} ) } {
    if {![file exist $netlist_dir]} {
      file mkdir $netlist_dir
    }
    regsub {^~/} $netlist_dir ${env(HOME)}/ netlist_dir
    return $netlist_dir
  } 
  if { $dir eq {} } {
    if { $netlist_dir ne {} }  { 
      set initdir $netlist_dir
    } else {
      if {$OS == "Windows"} {
        set initdir  $env(windir)
      } else {
        set initdir  [pwd]
      }
    }
    # 20140409 do not change netlist_dir if user Cancels action
    set new_dir [tk_chooseDirectory -initialdir $initdir \
       -parent [xschem get topwindow] -title {Select netlist DIR} -mustexist false]
  } else {
    set new_dir $dir
  }

  if {$new_dir ne {} } {
    if {![file exist $new_dir]} {
      file mkdir $new_dir
    }
    set netlist_dir $new_dir  
  }
  regsub {^~/} $netlist_dir ${env(HOME)}/ netlist_dir
  return $netlist_dir
}


proc enter_text {textlabel {preserve_disabled disabled}} {
  global retval rcode has_cairo preserve_unchanged_attrs wm_fix
  set rcode {}
  toplevel .dialog -class Dialog
  wm title .dialog {Enter text}

  set X [expr {[winfo pointerx .dialog] - 30}]
  set Y [expr {[winfo pointery .dialog] - 25}]

  # 20100203
  if { $wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "+$X+$Y"
  frame .dialog.f1
  label .dialog.f1.txtlab -text $textlabel
  text .dialog.txt -width 100 -height 4
  .dialog.txt delete 1.0 end
  .dialog.txt insert 1.0 $retval
  checkbutton .dialog.f1.l1 -text "preserve unchanged props" -variable preserve_unchanged_attrs \
     -state $preserve_disabled
  pack .dialog.f1 -side top -fill x ;# -expand yes
  pack .dialog.f1.l1 -side left
  pack .dialog.f1.txtlab -side left -expand yes -fill x

  pack .dialog.txt -side top -fill  both -expand yes
  frame .dialog.edit
    frame .dialog.edit.lab
    frame .dialog.edit.entries
    pack  .dialog.edit.lab -side left 
    pack  .dialog.edit.entries -side left -fill x  -expand yes
    pack .dialog.edit  -side top  -fill x 
      if {$has_cairo } {
        entry .dialog.edit.entries.hsize -relief sunken -textvariable vsize -width 20
      } else {
        entry .dialog.edit.entries.hsize -relief sunken -textvariable hsize -width 20
      }
      entry .dialog.edit.entries.vsize -relief sunken -textvariable vsize -width 20
      entry .dialog.edit.entries.props -relief sunken -textvariable props -width 20 
      pack .dialog.edit.entries.hsize .dialog.edit.entries.vsize  \
       .dialog.edit.entries.props -side top  -fill x -expand yes
      label .dialog.edit.lab.hlab -text "hsize:"
      label .dialog.edit.lab.vlab -text "vsize:"
      label .dialog.edit.lab.proplab -text "props:"
      pack .dialog.edit.lab.hlab .dialog.edit.lab.vlab  \
       .dialog.edit.lab.proplab -side top
  frame .dialog.buttons
  button .dialog.buttons.ok -text "OK" -command  \
  {
   set retval [.dialog.txt get 1.0 {end - 1 chars}]
   if {$has_cairo} { 
     set hsize $vsize
   }
   set rcode {ok}
   destroy .dialog 
  }
  button .dialog.buttons.cancel -text "Cancel" -command  \
  {
   set retval {}
   set rcode {}
   destroy .dialog 
  }
  button .dialog.buttons.b3 -text "Load" -command \
  {
    global INITIALTEXTDIR
    if { ![info exists INITIALTEXTDIR] } { set INITIALTEXTDIR [xschem get current_dirname] }
    set a [tk_getOpenFile -parent .dialog -initialdir $INITIALTEXTDIR ]
    if [string compare $a ""] {
     set INITIALTEXTDIR [file dirname $a]
     read_data_window  .dialog.txt  $a
    }
  }
  button .dialog.buttons.b4 -text "Del" -command \
  {
    .dialog.txt delete 1.0 end
  }
  pack .dialog.buttons.ok  -side left -fill x -expand yes
  pack .dialog.buttons.cancel  -side left -fill x -expand yes
  pack .dialog.buttons.b3  -side left -fill x -expand yes
  pack .dialog.buttons.b4  -side left -fill x -expand yes
  pack .dialog.buttons -side bottom -fill x
  bind .dialog <Escape> {
    if ![string compare $retval [.dialog.txt get 1.0 {end - 1 chars}]] {
      .dialog.buttons.cancel invoke
    }
  }
  bind .dialog <Control-Return> {.dialog.buttons.ok invoke}
  #grab set .dialog
  tkwait window .dialog
  return $retval
}

# evaluate a tcl command from GUI
proc tclcmd {} {
  global tclcmd_txt
  if {[winfo exists .tclcmd]} {
    destroy .tclcmd
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .tclcmd -class dialog
  label .tclcmd.txtlab -text {Enter TCL expression:}
  label .tclcmd.result -text {Result:}
  text .tclcmd.t -width 100 -height 8
  text .tclcmd.r -width 100 -height 6 -yscrollcommand ".tclcmd.yscroll set" 
  scrollbar .tclcmd.yscroll -command  ".tclcmd.r yview"
  .tclcmd.t insert 1.0 $tclcmd_txt

  frame .tclcmd.b
  button .tclcmd.b.close -text Close -command {
    set tclcmd_txt [.tclcmd.t get 1.0 end]
    destroy .tclcmd
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  }
  button .tclcmd.b.ok -text Evaluate -command {
    set tclcmd_txt [.tclcmd.t get 1.0 end]
    set res [eval $tclcmd_txt]
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

proc select_layers {} {
  global dark_colorscheme colors enable_layer
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .sl -class dialog
  if { $dark_colorscheme == 1 } {
    set txt_color black
  } else {
    set txt_color white
  }
  set j 0
  set f 0
  frame .sl.f0
  frame .sl.f1
  pack .sl.f0 .sl.f1 -side top -fill x
  button .sl.f1.ok -text OK -command { destroy .sl}
  pack .sl.f1.ok -side left -expand yes -fill x
  frame .sl.f0.f$f 
  pack .sl.f0.f$f -side left -fill y
  foreach i $colors {
    if { $dark_colorscheme == 1 } {
      set ind_bg white
    } else {
      set ind_bg black
    }
    if {  $j == [xschem get pinlayer] } {
      set laylab [format %2d $j]-PIN
      set layfg $txt_color
    } elseif { $j == [xschem get wirelayer] } {
      set laylab [format %2d $j]-WIRE
      set layfg $txt_color
    } elseif { $j == [xschem get textlayer] } {
      set laylab [format %2d $j]-TEXT
      set layfg $txt_color
    } elseif { $j == [xschem get backlayer] } {
      set laylab [format %2d $j]-BG
      if { $dark_colorscheme == 1 } {
        set layfg white
        set ind_bg black
      } else {
        set layfg black
        set ind_bg white
      }
    } elseif { $j == [xschem get gridlayer] } {
      set laylab [format %2d $j]-GRID
      set layfg $txt_color
    } else {
      set laylab "[format %2d $j]        "
      set layfg $txt_color
    }

    checkbutton .sl.f0.f$f.cb$j -text $laylab  -variable enable_layer($j) -activeforeground $layfg \
       -selectcolor $ind_bg -anchor w -foreground $layfg -background $i -activebackground $i \
       -command { 
           xschem enable_layers
        }
    pack .sl.f0.f$f.cb$j -side top -fill x
    incr j
    if { [expr {$j%10}] == 0 } {
      incr f
      frame .sl.f0.f$f
      pack .sl.f0.f$f -side left  -fill y
    }
  }
  tkwait window .sl
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc color_dim {} {
  global dim_bg dim_value enable_dim_bg
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dim -class dialog
  wm title .dim {Dim colors}
  checkbutton .dim.bg -text {Dim background} -variable enable_dim_bg
  # xschem color_dim <scale value> sets also dim_value variable
  scale .dim.scale -digits 2 -label {Dim factor} -length 256 \
     -showvalue 1 -command {xschem color_dim} -orient horizontal \
     -from -5 -to 5 -resolution 0.1
  button .dim.ok -text OK -command {destroy .dim}
  .dim.scale set $dim_value
  pack .dim.scale
  pack .dim.bg -side left
  pack .dim.ok -side right -anchor e
  tkwait window .dim
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc about {} {
  if [winfo exists .about] { 
    bind .about.link <Button-1> {}
    bind .about.link2 <Button-1> {}
    destroy .about
  }
  toplevel .about -class dialog
  wm title .about {About XSCHEM}
  label .about.xschem -text "XSCHEM V[xschem get version]" -font {Sans 24 bold}
  label .about.descr -text "Schematic editor / netlister for VHDL, Verilog, SPICE, tEDAx"
  button .about.link -text {http://repo.hu/projects/xschem} -font Underline-Font -fg blue -relief flat
  button .about.link2 -text {https://github.com/StefanSchippers/xschem} -font Underline-Font -fg blue -relief flat
  button .about.link3 -text {Online XSCHEM Manual} -font Underline-Font -fg blue -relief flat
  label .about.copyright -text "\n Copyright 1998-2021 Stefan Schippers (stefan.schippers@gmail.com) \n
 This is free software; see the source for copying conditions.  There is NO warranty;
 not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\n"
  button .about.close -text Close -command {destroy .about} -font {Sans 18}
  pack .about.xschem
  pack .about.link
  pack .about.link2
  pack .about.link3
  pack .about.descr
  pack .about.copyright
  pack .about.close
  bind .about.link <Button-1> { execute 0  xdg-open http://repo.hu/projects/xschem}
  bind .about.link2 <Button-1> { execute 0  xdg-open https://github.com/StefanSchippers/xschem}
  bind .about.link3 <Button-1> { execute 0  xdg-open http://repo.hu/projects/xschem/index.html}
}

proc property_search {} {
  global search_value search_found
  global search_exact
  global search_select
  global custom_token OS

  set search_found 0
  while { !$search_found} {
    if { [winfo exists .dialog] } return
    xschem set semaphore [expr {[xschem get semaphore] +1}]
    toplevel .dialog -class Dialog
    wm title .dialog {Search}
    if { ![info exists X] } {
      set X [expr {[winfo pointerx .dialog] - 60}]
      set Y [expr {[winfo pointery .dialog] - 35}]
    }
    wm geometry .dialog "+$X+$Y"
    frame .dialog.custom 
    label .dialog.custom.l -text "Token"
    entry .dialog.custom.e -width 32
    .dialog.custom.e insert 0 $custom_token
    pack .dialog.custom.e .dialog.custom.l -side right
    frame .dialog.val 
    label .dialog.val.l -text "Value"
    entry .dialog.val.e -width 32
    .dialog.val.e insert 0 $search_value
    pack .dialog.val.e .dialog.val.l -side right
    frame .dialog.but
    button .dialog.but.ok -text OK -command {
          set search_value [.dialog.val.e get]
          set custom_token [.dialog.custom.e get]
          if {$debug_var<=-1} { puts stderr "|$custom_token|" }
          if { $search_exact==1 } {
            set search_found [xschem searchmenu exact $search_select $custom_token $search_value]
          } else {
            set search_found [xschem searchmenu regex $search_select $custom_token $search_value]
          }
          destroy .dialog 
    }
    button .dialog.but.cancel -text Cancel -command { set search_found 1; destroy .dialog }
    
    # Window doesn't support regular expression, has to be exact match for now
    if {$OS == "Windows"} {
      set search_exact 1 
      checkbutton .dialog.but.sub -text Exact_search -variable search_exact -state disable
    } else {
      checkbutton .dialog.but.sub -text Exact_search -variable search_exact 
    }
    radiobutton .dialog.but.nosel -text {Highlight} -variable search_select -value 0
    radiobutton .dialog.but.sel -text {Select} -variable search_select -value 1
    # 20171211 added unselect
    radiobutton .dialog.but.unsel -text {Unselect} -variable search_select -value -1
    pack .dialog.but.ok  -anchor w -side left
    pack .dialog.but.sub  -side left
    pack .dialog.but.nosel  -side left
    pack .dialog.but.sel  -side left
    pack .dialog.but.unsel  -side left
    pack .dialog.but.cancel -anchor e
    pack .dialog.custom  -anchor e
    pack .dialog.val  -anchor e
    pack .dialog.but -expand yes -fill x
    focus  .dialog
    bind .dialog <Escape> {.dialog.but.cancel invoke}
    bind .dialog <Return> {.dialog.but.ok invoke}
    bind .dialog <Control-Return> {.dialog.but.ok invoke}
    grab set .dialog
    tkwait window .dialog
    xschem set semaphore [expr {[xschem get semaphore] -1}]
  } 
  return {}
}

#20171029
# allows to call TCL hooks from 'format' strings during netlisting
# example of symbol spice format definition:
# format="@name @pinlist @symname @tcleval(<script>) m=@m"
# NOTE: spaces and quotes in <script> must be escaped
# symname and instname are predefined variables in the <script> context
# they can be used together with TCL xschem command to query instance or symbol 
# attributes.
#
proc tclpropeval {s instname symname} {
  # puts "tclpropeval: $s $instname $symname"
  global env debug_var
  if {$debug_var <=-1} {puts "tclpropeval: $s"}
  regsub {^@tcleval\(} $s {} s
  regsub {\)([ \t\n]*)$} $s {\1} s
  if { [catch {eval $s} res] } {
    if { $debug_var<=-1 } { puts "tclpropeval warning: $res"}
    set res ?\n
  }
  return $res
}

# this hook is called in translate() if whole string is contained in a tcleval(...) construct
proc tclpropeval2 {s} {
  global debug_var env path

  set netlist_type [xschem get netlist_type]
  # puts "tclpropeval2: s=|$s|"
  if {$debug_var <=-1} {puts "tclpropeval2: $s"}
  set path [string range [xschem get sch_path] 1 end]
  if { $netlist_type eq {spice} } {
    # this is necessary if spiceprefix is being used in netlists
    regsub {^([^xX])} $path {x\1} path
    while { [regsub {\.([^xX])} $path {.x\1} path] } {}
  }
  if { [sim_is_xyce]} {
    regsub -all {\.} [string toupper $path] {:} path
  }
  if { $debug_var<=-1 } { puts "---> path=$path" }
  regsub {^tcleval\(} $s {} s
  regsub {\)([ \n\t]*)$} $s {\1} s
  # puts "tclpropeval2: s=|$s|"
  # puts "tclpropeval2: subst $s=|[subst $s]|"
  if { [catch {uplevel #0 "subst \{$s\}"} res] } {
    if { $debug_var<=-1 } { puts "tclpropeval2 warning: $res"}
    set res ?\n
  }
  # puts "tclpropeval2: res=|$res|"
  return $res
}

proc attach_labels_to_inst {} {
  global use_lab_wire use_label_prefix custom_label_prefix rcode do_all_inst rotated_text

  set rcode {}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class Dialog
  wm title .dialog {Add labels to instances}

  # 20100408
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]
  wm geometry .dialog "+$X+$Y"

  frame .dialog.custom 
  label .dialog.custom.l -text "Prefix"
  entry .dialog.custom.e -width 32
  .dialog.custom.e insert 0 $custom_label_prefix
  pack .dialog.custom.e .dialog.custom.l -side right


  frame .dialog.but
  button .dialog.but.ok -text OK -command {
        set custom_label_prefix [.dialog.custom.e get]
        #### put command here
        set rcode yes
        destroy .dialog 
  }
  button .dialog.but.cancel -text Cancel -command { set rcode {}; destroy .dialog }
  checkbutton .dialog.but.wire -text {use wire labels} -variable use_lab_wire
  checkbutton .dialog.but.do_all -text {Do all} -variable do_all_inst
  label .dialog.but.rot -text {Rotated Text}
  entry .dialog.but.rotated -textvariable rotated_text -width 2
  checkbutton .dialog.but.prefix -text {use prefix} -variable use_label_prefix
  pack .dialog.but.ok  -anchor w -side left
  pack .dialog.but.prefix  -side left
  pack .dialog.but.wire  -side left
  pack .dialog.but.do_all  -side left
  pack .dialog.but.rotated  -side left
  pack .dialog.but.rot  -side left
  pack .dialog.but.cancel -anchor e

  pack .dialog.custom  -anchor e
  pack .dialog.but -expand yes -fill x

  focus  .dialog
  bind .dialog <Escape> {.dialog.but.cancel invoke}
  bind .dialog <Return> {.dialog.but.ok invoke}
  bind .dialog <Control-Return> {.dialog.but.ok invoke}
  grab set .dialog
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return {}
}

proc ask_save { {ask {save file?}} {cancel 1}} {
  global rcode wm_fix
  set rcode {}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class Dialog
  wm title .dialog {Ask Save}

  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]
  if { $wm_fix } { tkwait visibility .dialog }

  wm geometry .dialog "+$X+$Y"
  label .dialog.l1  -text $ask
  frame .dialog.f1
  button .dialog.f1.b1 -text {Yes} -command\
  {
   set rcode {yes}
   destroy .dialog
  }
  if {$cancel} {
    button .dialog.f1.b2 -text {Cancel} -command\
    {
     set rcode {}
     destroy .dialog
    }
  }
  button .dialog.f1.b3 -text {No} -command\
  {
   set rcode {no}
   destroy .dialog
  }
  pack .dialog.l1 .dialog.f1 -side top -fill x
  pack .dialog.f1.b1 -side left -fill x -expand yes
  if { $cancel} {
    pack .dialog.f1.b2 -side left -fill x -expand yes
  }
  pack .dialog.f1.b3 -side left -fill x -expand yes
  if {$cancel} {
    bind .dialog <Escape> {.dialog.f1.b2 invoke}
  }
  # needed, otherwise problems when descending with double clixk 23012004
  tkwait visibility .dialog
  grab set .dialog
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $rcode
}


proc edit_vi_prop {txtlabel} {
  global XSCHEM_TMP_DIR retval symbol prev_symbol rcode debug_var editor
  global user_wants_copy_cell
 
  set netlist_type [xschem get netlist_type]
  set user_wants_copy_cell 0
  set rcode {}
  set filename .xschem_edit_file.[pid]
  if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
  set filename $filename.$suffix
  write_data $retval $XSCHEM_TMP_DIR/$filename
  # since $editor can be an executable with options (gvim -f) I *need* to use eval
  eval execute_wait 0 $editor $XSCHEM_TMP_DIR/$filename ;# 20161119
  if {$debug_var<=-1} {puts "edit_vi_prop{}:\n--------\nretval=$retval\n---------\n"}
  if {$debug_var<=-1} {puts "edit_vi_prop{}:\n--------\nsymbol=$symbol\n---------\n"}
  set tmp [read_data $XSCHEM_TMP_DIR/$filename]
  file delete $XSCHEM_TMP_DIR/$filename
  if {$debug_var<=-1} {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
  if [string compare $tmp $retval] {
         set retval $tmp
         if {$debug_var<=-1} {puts "modified"}
         set rcode ok
         return  $rcode
  } else {
         set rcode {}
         return $rcode
  }
}

proc edit_vi_netlist_prop {txtlabel} {
  global XSCHEM_TMP_DIR retval rcode debug_var editor
  global user_wants_copy_cell
 
  set netlist_type [xschem get netlist_type]
  set user_wants_copy_cell 0
  set filename .xschem_edit_file.[pid]
  if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
  set filename $filename.$suffix
  regsub -all {\\?"} $retval {"} retval
  regsub -all {\\?\\} $retval {\\} retval
  write_data $retval $XSCHEM_TMP_DIR/$filename
  if { [regexp vim $editor] } { set ftype "\{-c :set filetype=$netlist_type\}" } else { set ftype {} }
  # since $editor can be an executable with options (gvim -f) I *need* to use eval
  eval execute_wait 0 $editor  $ftype $XSCHEM_TMP_DIR/$filename
  if {$debug_var <= -1}  {puts "edit_vi_prop{}:\n--------\n$retval\n---------\n"}
  set tmp [read_data $XSCHEM_TMP_DIR/$filename]
  file delete $XSCHEM_TMP_DIR/$filename
  if {$debug_var <= -1}  {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
  if [string compare $tmp $retval] {
         set retval $tmp
         regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
         set retval \"${retval}\" 
         if {$debug_var <= -1}  {puts "modified"}
         set rcode ok
         return  $rcode
  } else {
         set rcode {}
         return $rcode
  }
}
proc reset_colors {ask} {
  global colors dark_colors light_colors dark_colorscheme USER_CONF_DIR svg_colors ps_colors
  global light_colors_save dark_colors_save

  if {$ask} {
    set answer [tk_messageBox -message  "Warning: delete 'colors' configuration file?" \
        -icon warning -parent [xschem get topwindow] -type okcancel]
    if {$answer ne {ok}} { return }
  }
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  set light_colors $light_colors_save
  set dark_colors $dark_colors_save
  if { $dark_colorscheme == 1 } {
    set colors $dark_colors
  } else {
    set colors $light_colors
  }
  regsub -all {"} $light_colors  {} ps_colors
  regsub -all {#} $ps_colors  {0x} ps_colors
  regsub -all {"} $colors {} svg_colors
  regsub -all {#} $svg_colors {0x} svg_colors
  file delete ${USER_CONF_DIR}/colors
  xschem build_colors
  xschem redraw
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc change_color {} {
  global colors dark_colors light_colors dark_colorscheme cadlayers USER_CONF_DIR svg_colors ps_colors

  set n [xschem get rectcolor]
  if { $n < 0 || $n >=$cadlayers} return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  if { $dark_colorscheme == 1 } {
    set c $dark_colors
  } else {
    set c $light_colors
  }
  set initial_color [lindex $c $n]
  set value [tk_chooseColor -initialcolor $initial_color]
  if {[string compare $value {}] } {
    set cc [lreplace $c $n $n $value]
    set colors $cc
    if { $dark_colorscheme == 1 } {
      set dark_colors $cc
    } else {
      set light_colors $cc
      regsub -all {"} $cc  {} ps_colors
      regsub -all {#} $ps_colors  {0x} ps_colors
    }
    regsub -all {"} $colors {} svg_colors
    regsub -all {#} $svg_colors {0x} svg_colors

    xschem build_colors
    xschem redraw
    set    savedata "#### THIS FILE IS AUTOMATICALLY GENERATED BY XSCHEM, DO NOT EDIT.\n"
    append savedata "set cadlayers $cadlayers\n"
    append savedata "set light_colors {$light_colors}\n"
    append savedata "set dark_colors {$dark_colors}\n"
    write_data $savedata ${USER_CONF_DIR}/colors
  }
  xschem set semaphore [expr {[xschem get semaphore] -1}]
}

proc edit_prop {txtlabel} {
  global edit_prop_size infowindow_text selected_tok edit_symbol_prop_new_sel edit_prop_pos
  global prev_symbol retval symbol rcode no_change_attrs preserve_unchanged_attrs copy_cell debug_var
  global user_wants_copy_cell editprop_sympath retval_orig old_selected_tok
  set user_wants_copy_cell 0
  set rcode {}
  set retval_orig $retval
  if {$debug_var <= -1}  {puts " edit_prop{}: retval=$retval"}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog  -class Dialog 
  wm title .dialog {Edit Properties}
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]

  # 20160325 change and remember widget size
  bind .dialog <Configure> { 
    # puts [wm geometry .dialog]
    if { $edit_symbol_prop_new_sel != 1 } {
      set edit_prop_size [wm geometry .dialog]
      set edit_prop_pos $edit_prop_size
      regsub {\+.*} $edit_prop_size {} edit_prop_size
      regsub {[^+]*\+} $edit_prop_pos {+} edit_prop_pos
    }
  }
  wm geometry .dialog "${edit_prop_size}+$X+$Y"
  set prev_symbol $symbol
  set editprop_sympath [file dirname [abs_sym_path $symbol]]
  frame .dialog.f4
  label .dialog.f4.l1  -text $txtlabel
  label .dialog.f4.path  -text "Path:"
  entry .dialog.f4.e1  -textvariable editprop_sympath -width 0 -state readonly
  text .dialog.e1   -yscrollcommand ".dialog.yscroll set" -setgrid 1 \
                  -xscrollcommand ".dialog.xscroll set" -wrap none
    .dialog.e1 delete 1.0 end
    .dialog.e1 insert 1.0 $retval
  scrollbar .dialog.yscroll -command  ".dialog.e1 yview"
  scrollbar .dialog.xscroll -command ".dialog.e1 xview" -orient horiz
  frame .dialog.f1
  frame .dialog.f2
  label .dialog.f1.l2 -text "Symbol"
  entry .dialog.f1.e2 -width 30
  .dialog.f1.e2 insert 0 $symbol
  button .dialog.f1.b5 -text "Browse" -command {
    set r [tk_getOpenFile -parent .dialog -initialdir $INITIALINSTDIR ]
    if {$r ne {} } {
      .dialog.f1.e2 delete 0 end
      .dialog.f1.e2 insert 0 $r
    }
    raise .dialog .drw
  }
  button .dialog.f1.b1 -text "OK" -command   {
    set retval [.dialog.e1 get 1.0 {end - 1 chars}]
    if { $selected_tok ne {<ALL>} } {
      regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
      set retval \"${retval}\"
      set retval [xschem subst_tok $retval_orig $selected_tok $retval]
      set selected_tok {<ALL>}
    }
    set symbol [.dialog.f1.e2 get]
    set abssymbol [abs_sym_path $symbol]
    set rcode {ok}
    set user_wants_copy_cell $copy_cell
    set prev_symbol [abs_sym_path $prev_symbol]
    if { ($abssymbol ne $prev_symbol) && $copy_cell } {
      if { ![regexp {^/} $symbol] && ![regexp {^[a-zA-Z]:} $symbol] } {
        set symlist [file split $symbol]
        set symlen [llength $symlist]
        set abssymbol "[path_head $prev_symbol $symlen]/$symbol"
      }
      # puts "$abssymbol   $prev_symbol"
      if { [file exists "[file rootname $prev_symbol].sch"] } {
        if { ! [file exists "[file rootname ${abssymbol}].sch"] } {
          file copy "[file rootname $prev_symbol].sch" "[file rootname $abssymbol].sch"
          # puts "file copy [file rootname $prev_symbol].sch [file rootname $abssymbol].sch"
        }
      }
      if { [file exists "$prev_symbol"] } {
        if { ! [file exists "$abssymbol"] } {
          file copy "$prev_symbol" "$abssymbol"
          # puts "file copy [file rootname $prev_symbol].sym [file rootname $abssymbol].sym"
        }
      }
    }
    #puts "symbol: $symbol , prev_symbol: $prev_symbol"
    set copy_cell 0 ;# 20120919
    destroy .dialog
  }
  button .dialog.f1.b2 -text "Cancel" -command  {
    set rcode {}
    set edit_symbol_prop_new_sel {}
    destroy .dialog
  }
  wm protocol .dialog  WM_DELETE_WINDOW {
    set rcode {}
    set edit_symbol_prop_new_sel {}
    destroy .dialog
  }
  button .dialog.f1.b3 -text "Load" -command {
    global INITIALPROPDIR
    set a [tk_getOpenFile -parent .dialog -initialdir $INITIALPROPDIR ]
    if [string compare $a ""] {
     set INITIALPROPDIR [file dirname $a]
     read_data_window  .dialog.e1  $a
    }
  }
  button .dialog.f1.b4 -text "Del" -command {
    .dialog.e1 delete 1.0 end
  }
  checkbutton .dialog.f2.r1 -text "No change properties" -variable no_change_attrs -state normal
  checkbutton .dialog.f2.r2 -text "Preserve unchanged props" -variable preserve_unchanged_attrs -state normal
  checkbutton .dialog.f2.r3 -text "Copy cell" -variable copy_cell -state normal
  set tok_list "<ALL> [xschem list_tokens $retval 0]"
  set selected_tok {<ALL>}
  set old_selected_tok {<ALL>}
  label .dialog.f2.r4 -text {   Edit Attr:}
  if  { [ info tclversion] > 8.4} {
    ttk::combobox .dialog.f2.r5 -values $tok_list -textvariable selected_tok -width 14
  }
  pack .dialog.f1.l2 .dialog.f1.e2 .dialog.f1.b1 .dialog.f1.b2 .dialog.f1.b3 \
       .dialog.f1.b4 .dialog.f1.b5 -side left -expand 1
  pack .dialog.f4 -side top  -anchor nw
  #pack .dialog.f4.path .dialog.f4.e1 .dialog.f4.l1 -side left -fill x 
  pack .dialog.f4.path -side left
  pack .dialog.f4.e1 -side left 
  pack .dialog.f1 .dialog.f2 -side top -fill x 
  pack .dialog.f2.r1 -side left
  pack .dialog.f2.r2 -side left
  pack .dialog.f2.r3 -side left
  pack .dialog.f2.r4 -side left
  if { [ info tclversion] > 8.4 } { pack .dialog.f2.r5 -side left }
  pack .dialog.yscroll -side right -fill y 
  pack .dialog.xscroll -side bottom -fill x
  pack .dialog.e1  -fill both -expand yes
  bind .dialog <Control-Return> {.dialog.f1.b1 invoke}
  bind .dialog <Escape> {
    if { ![string compare $retval [.dialog.e1 get 1.0 {end - 1 chars}]] && \
         ![string compare $symbol [ .dialog.f1.e2 get]] } {
      .dialog.f1.b2 invoke
    }
  }
  if  { [ info tclversion] > 8.4} {
    bind .dialog.f2.r5 <<ComboboxSelected>> {
      if {$old_selected_tok ne $selected_tok} {
        if { $old_selected_tok eq {<ALL>} } {
          set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
        } else {
          set retval [.dialog.e1 get 1.0 {end - 1 chars}]
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } { 
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.e1 delete 1.0 end
      .dialog.e1 insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
 
    bind .dialog.f2.r5 <KeyRelease> {
      set selected_tok [.dialog.f2.r5 get]
      if { $old_selected_tok eq {<ALL>}} {
        set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
      } else {
        set retval [.dialog.e1 get 1.0 {end - 1 chars}]
        if {$retval ne {}} {
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } {
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.e1 delete 1.0 end
      .dialog.e1 insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
  }
  if {$edit_symbol_prop_new_sel == 1} { 
    wm geometry .dialog $edit_prop_pos
  }
  set edit_symbol_prop_new_sel 0
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
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
  ## 20171103 insert text at cursor position instead of at beginning (insert index tag)
  $w insert insert $t
  close $fid
}

proc write_data {data f} {
  set fid [open $f "w"]
  puts  -nonewline $fid $data
  close $fid
  return {}
}

proc text_line {txtlabel clear {preserve_disabled disabled} } {
  global text_line_default_geometry preserve_unchanged_attrs wm_fix
  global retval rcode debug_var selected_tok retval_orig old_selected_tok
  set retval_orig $retval
  if $clear==1 then {set retval ""}
  if {$debug_var <= -1}  {puts " text_line{}: clear=$clear"}
  if {$debug_var <= -1}  {puts " text_line{}: retval=$retval"}
  set rcode {}
  if { [winfo exists .dialog] } return
  toplevel .dialog  -class Dialog
  wm title .dialog {Text input}
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]

  set tok_list "<ALL> [xschem list_tokens $retval 0]"
  set selected_tok {<ALL>}
  set old_selected_tok {<ALL>}
  bind .dialog <Configure> {
    # puts [wm geometry .dialog]
    set text_line_default_geometry [wm geometry .dialog]
    regsub {\+.*} $text_line_default_geometry {} text_line_default_geometry
  }

  # 20100203
  if { $wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "${text_line_default_geometry}+$X+$Y"

  frame .dialog.f0
  frame .dialog.f1
  label .dialog.f0.l1  -text $txtlabel
  text .dialog.e1 -relief sunken -bd 2 -yscrollcommand ".dialog.yscroll set" -setgrid 1 \
       -xscrollcommand ".dialog.xscroll set" -wrap none -width 90 -height 40
  scrollbar .dialog.yscroll -command  ".dialog.e1 yview"
  scrollbar .dialog.xscroll -command ".dialog.e1 xview" -orient horiz
  .dialog.e1 delete 1.0 end
  .dialog.e1 insert 1.0 $retval
  button .dialog.f1.b1 -text "OK" -command  \
  {
    set retval [.dialog.e1 get 1.0 {end - 1 chars}]
    if { $selected_tok ne {<ALL>} } {
      regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
      set retval \"${retval}\"
      set retval [xschem subst_tok $retval_orig $selected_tok $retval]
      set selected_tok {<ALL>}
    }
    destroy .dialog
    set rcode {ok}
  }
  button .dialog.f1.b2 -text "Cancel" -command  \
  {
    set retval [.dialog.e1 get 1.0 {end - 1 chars}]
    set rcode {}
    destroy .dialog
  }
  button .dialog.f1.b3 -text "Load" -command \
  {
    global INITIALPROPDIR
    set a [tk_getOpenFile -parent .dialog -initialdir $INITIALPROPDIR ]
    if [string compare $a ""] {
     set INITIALPROPDIR [file dirname $a]
     read_data_window  .dialog.e1  $a
    }
  }
  button .dialog.f1.b4 -text "Del" -command \
  {
    .dialog.e1 delete 1.0 end
  }
  label .dialog.f1.r4 -text {   Edit Attr:}
  if  { [ info tclversion] > 8.4} {
    ttk::combobox .dialog.f1.r5 -values $tok_list -textvariable selected_tok -width 14
  }
  checkbutton .dialog.f0.l2 -text "preserve unchanged props" -variable preserve_unchanged_attrs \
     -state $preserve_disabled
  pack .dialog.f0 -fill x
  pack .dialog.f0.l2 -side left
  pack .dialog.f0.l1 -side left -expand yes
  pack .dialog.f1  -fill x
  pack .dialog.f1.b1 -side left -fill x -expand yes
  pack .dialog.f1.b2 -side left -fill x -expand yes
  pack .dialog.f1.b3 -side left -fill x -expand yes
  pack .dialog.f1.b4 -side left -fill x -expand yes
  pack .dialog.f1.r4 -side left
  if  { [ info tclversion] > 8.4} {pack .dialog.f1.r5 -side left}


  pack .dialog.yscroll -side right -fill y 
  pack .dialog.xscroll -side bottom -fill x
  pack .dialog.e1   -expand yes -fill both
  bind .dialog <Escape> {
    if ![string compare $retval [.dialog.e1 get 1.0 {end - 1 chars}]] {
      .dialog.f1.b2 invoke
    }
  }

  if  { [ info tclversion] > 8.4} {
    bind .dialog.f1.r5 <<ComboboxSelected>> {
      if {$old_selected_tok ne $selected_tok} {
        if { $old_selected_tok eq {<ALL>} } {
          set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
        } else {
          set retval [.dialog.e1 get 1.0 {end - 1 chars}]
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } {
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.e1 delete 1.0 end
      .dialog.e1 insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
 
    bind .dialog.f1.r5 <KeyRelease> {
      set selected_tok [.dialog.f1.r5 get]
      if { $old_selected_tok eq {<ALL>}} {
        set retval_orig [.dialog.e1 get 1.0 {end - 1 chars}]
      } else {
        set retval [.dialog.e1 get 1.0 {end - 1 chars}]
        if {$retval ne {}} {
          regsub -all {(["\\])} $retval {\\\1} retval ;#"  editor is confused by the previous quote
          set retval \"${retval}\"
          set retval_orig [xschem subst_tok $retval_orig $old_selected_tok $retval]
        }
      }
      if {$selected_tok eq {<ALL>} } {
        set retval $retval_orig
      } else {
        set retval [xschem get_tok $retval_orig $selected_tok 2]
        # regsub -all {\\?"} $retval {"} retval
      }
      .dialog.e1 delete 1.0 end
      .dialog.e1 insert 1.0 $retval
      set old_selected_tok $selected_tok
    }
  }
  bind .dialog <Control-Return> {.dialog.f1.b1 invoke}
  #tkwait visibility .dialog
  #grab set .dialog
  #focus .dialog.e1
  set rcode {}   
  tkwait window .dialog
  return $rcode
}

proc alert_ {txtlabel {position +200+300} {nowait {0}}} {
  global has_x
  if {![info exists has_x] } {return}
  toplevel .alert -class Dialog
  wm title .alert {Alert}
  set X [expr {[winfo pointerx .alert] - 60}]
  set Y [expr {[winfo pointery .alert] - 60}]
  if { [string compare $position ""] != 0 } {
    wm geometry .alert $position
  } else {
    wm geometry .alert "+$X+$Y"
  }
  label .alert.l1  -text $txtlabel -wraplength 700
  button .alert.b1 -text "OK" -command  \
  {
    destroy .alert
  } 
  pack .alert.l1 -side top -fill x
  pack .alert.b1 -side top -fill x
  tkwait visibility .alert
  grab set .alert
  focus .alert.b1
  bind .alert <Return> { destroy .alert }
  bind .alert <Escape> { destroy .alert }
  bind .alert <Visibility> {
    if { [winfo exists .alert] && [winfo ismapped .alert] && [winfo ismapped .] && [wm stackorder .alert isbelow . ]} {
      if { [winfo exists .drw] } {
        raise .alert .drw
      } else {
        raise .alert
      }
      
    }
  }

  if {!$nowait} {tkwait window .alert}
  return {}
}

proc show_infotext {} {
  global has_x infowindow_text
  if {[info exists has_x]} {
     wm deiconify .infotext
  } else {
    puts stderr $infowindow_text
  }
}

proc infowindow {} {
  global infowindow_text

  set infotxt $infowindow_text
  if { $infowindow_text ne {}} {append infotxt \n}
  set z {.infotext}
  if ![string compare $infotxt ""] { 
    if [winfo exists $z] {
      $z.f1.text delete 1.0 end
    }
  }
  if ![winfo exists $z] {
    toplevel $z
    wm title $z {Info window}
    wm  geometry $z 90x24+0+400
    wm iconname $z {Info window}
    wm withdraw $z
    wm protocol .infotext WM_DELETE_WINDOW "wm withdraw $z; set show_infowindow 0"
    #  button $z.dismiss -text Dismiss -command  "destroy $z"
    frame $z.f1
    frame $z.f2
    text $z.f1.text -relief sunken -bd 2 -yscrollcommand "$z.f1.yscroll set" -setgrid 1 \
         -height 6 -width 50  -xscrollcommand "$z.f1.xscroll set" -wrap none
    scrollbar $z.f1.yscroll -command "$z.f1.text yview" -orient v 
    scrollbar $z.f1.xscroll -command "$z.f1.text xview" -orient h 
    grid $z.f1.text - $z.f1.yscroll -sticky nsew
    grid $z.f1.xscroll - -sticky ew
    # grid $z.dismiss - -
    grid rowconfig $z.f1 0 -weight 1
    grid columnconfig $z.f1 0 -weight 1
    pack $z.f1 -fill both -expand yes
    button $z.f2.dismiss -text Dismiss -command "wm withdraw $z; set show_infowindow 0"
    pack $z.f2.dismiss
    pack $z.f2 -fill x
    bind $z <Escape> "wm withdraw $z; set show_infowindow 0"
  }
  $z.f1.text insert end $infotxt
  $z.f1.text see end
  return {}
}

proc textwindow {filename {ro {}}} {
  global textwindow_wcounter
  global textwindow_w
  # set textwindow_w .win$textwindow_wcounter
  # catch {destroy $textwindow_w}
  set textwindow_wcounter [expr {$textwindow_wcounter+1}]
  set textwindow_w .win$textwindow_wcounter


  global textwindow_filename
  global textwindow_fileid
  set textwindow_filename $filename
  toplevel $textwindow_w
  wm title $textwindow_w $filename
  wm iconname $textwindow_w $filename
 frame $textwindow_w.buttons
  pack $textwindow_w.buttons -side bottom -fill x -pady 2m
  button $textwindow_w.buttons.dismiss -text Dismiss -command "destroy $textwindow_w"
  pack $textwindow_w.buttons.dismiss -side left -expand 1
  if { $ro eq {} } {
    button $textwindow_w.buttons.save -text "Save" -command \
     { 
      set textwindow_fileid [open $textwindow_filename "w"]
      puts -nonewline $textwindow_fileid [$textwindow_w.text get 1.0 {end - 1 chars}]
      close $textwindow_fileid 
      destroy $textwindow_w
     }
    pack $textwindow_w.buttons.save  -side left -expand 1
  }

  text $textwindow_w.text -relief sunken -bd 2 -yscrollcommand "$textwindow_w.yscroll set" -setgrid 1 \
       -xscrollcommand "$textwindow_w.xscroll set" -wrap none -height 30
  scrollbar $textwindow_w.yscroll -command  "$textwindow_w.text yview" 
  scrollbar $textwindow_w.xscroll -command "$textwindow_w.text xview" -orient horiz
  pack $textwindow_w.yscroll -side right -fill y
  pack $textwindow_w.text -expand yes -fill both
  pack $textwindow_w.xscroll -side bottom -fill x
  bind $textwindow_w <Escape> "$textwindow_w.buttons.dismiss invoke"
  set textwindow_fileid [open $filename "r"]

  # 20171103 insert at insertion cursor(insert tag) instead of 0.0
  $textwindow_w.text insert insert [read $textwindow_fileid]
  close $textwindow_fileid
  return {}
}

proc viewdata {data {ro {}}} {
  global viewdata_wcounter  rcode viewdata_filename
  global viewdata_w OS viewdata_fileid env
  # set viewdata_w .view$viewdata_wcounter
  # catch {destroy $viewdata_w}
  set viewdata_wcounter [expr {$viewdata_wcounter+1}]
  set viewdata_w .view$viewdata_wcounter
  set rcode {}
  toplevel $viewdata_w
  wm title $viewdata_w {View data}
  frame $viewdata_w.buttons
  pack $viewdata_w.buttons -side bottom -fill x -pady 2m

  button $viewdata_w.buttons.dismiss -text Dismiss -command  "destroy $viewdata_w" 
  pack $viewdata_w.buttons.dismiss -side left -expand 1

  if { $ro eq {} } {
    button $viewdata_w.buttons.saveas -text {Save As} -command  {
      if {$OS == "Windows"} {
        set viewdata_filename [tk_getSaveFile -initialdir $env(windir) ]
      } else {
        set viewdata_filename [tk_getSaveFile -initialdir [pwd] ]
      }
      if { $viewdata_filename != "" } {
        set viewdata_fileid [open $viewdata_filename "w"]
        puts -nonewline $viewdata_fileid [$viewdata_w.text get 1.0 {end - 1 chars}]
        close $viewdata_fileid
      }
    } 
    pack $viewdata_w.buttons.saveas  -side left -expand 1
  }

  text $viewdata_w.text -relief sunken -bd 2 -yscrollcommand "$viewdata_w.yscroll set" -setgrid 1 \
       -xscrollcommand "$viewdata_w.xscroll set" -wrap none -height 30
  scrollbar $viewdata_w.yscroll -command  "$viewdata_w.text yview" 
  scrollbar $viewdata_w.xscroll -command "$viewdata_w.text xview" -orient horiz
  pack $viewdata_w.yscroll -side right -fill y
  pack $viewdata_w.text -expand yes -fill both
  pack $viewdata_w.xscroll -side bottom -fill x
  # 20171103 insert at insertion cursor(insert tag) instead of 0.0
  $viewdata_w.text insert insert $data
  return $rcode
}

# given an absolute path of a symbol/schematic remove the path prefix
# if file is in a library directory (a $pathlist dir)
proc rel_sym_path {symbol} {
  global pathlist

  set curr_dirname [xschem get current_dirname]
  set name {}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]  && [info exist curr_dirname]} {
      set path_elem $curr_dirname
    }
    set pl [string length $path_elem]
    if { [string equal -length $pl $path_elem $symbol] } {
      set name [string range $symbol [expr {$pl+1}] end]
      break
    }
  }
  if {$name eq {} } {
    # no known lib, so return full path
    set name ${symbol}
  }
  return $name
}

## given a library/symbol return its absolute path
proc abs_sym_path {fname {ext {} } } {
  global pathlist OS

  set  curr_dirname [xschem get current_dirname]
  ## empty: do nothing
  if {$fname eq {} } return {}
  ## add extension for 1.0 file format compatibility
  if { $ext ne {} } { 
    set fname [file rootname $fname]$ext
  }
  if {$OS eq "Windows"} {
    ## absolute path: return as is
    if { [regexp {^[A-Za-z]\:/} $fname ] } {
      return "$fname"
    } 
  } else {
    ## absolute path: return as is
    if { [regexp {^/} $fname] } {
      return "$fname"
    }
  }
  ## replace all runs of multiple / with single / in fname
  regsub -all {/+} $fname {/} fname
  ## replace all '/./' with '/'
  while {[regsub {/\./} $fname {/} fname]} {}
  ## transform  a/b/../c to a/c or a/b/c/.. to a/b
  while {[regsub {([^/]*\.*[^./]+[^/]*)/\.\./?} $fname {} fname] } {}  
  ## remove trailing '/'  or '/.'
  while {[regsub {/\.?$} $fname {} fname]} {}
  ## 'found' set to 1 if fname begins with './' or  '../'
  set found 0 
  ## remove any leading './'
  while {[regsub {^\./} $fname {} fname]} {set found 1}
  ## if previous operation left fname empty set to '.'
  if { $fname eq {} } { set fname . }
  ## if fname is just "." return $curr_dirname
  if {[regexp {^\.$} $fname] } { return "$curr_dirname" }
  set tmpdirname $curr_dirname
  set tmpfname $fname
  ## if tmpfname begins with '../' remove this prefix and remove one path component from tmpdirname
  while { [regsub {^\.\./} $tmpfname {} tmpfname] } {
    set found 1
    set tmpdirname [file dirname $tmpdirname]
  }
  ## if tmpfname reducced to '..' return dirname of tmpdirname
  if { $tmpfname eq {..}} { return "[file dirname $tmpdirname]" }
  ## if given file begins with './' or '../' and dir or file exists relative to curr_dirname 
  ## just return it.
  if {$found } {
    set tmpfname "${tmpdirname}/$tmpfname"
    if { [file exists "$tmpfname"] } { return "$tmpfname" }
    ## if file does not exists but directory does return anyway
    if { [file exists [file dirname "$tmpfname"]] } { return "$tmpfname" }
  }
  ## if fname is present in one of the pathlist paths get the absolute path
  set name {}
  foreach path_elem $pathlist {
    ## in xschem a . in pathlist means the directory of currently loaded  schematic/symbol
    if { ![string compare $path_elem .]  && [info exist curr_dirname]} {
      set path_elem $curr_dirname
    }
    set fullpath "$path_elem/$fname"
    if { [file exists $fullpath] } {
      set name $fullpath
      break
    }
  }
  ## nothing found -> use current schematic directory
  if {$name eq {} } {
    set name "$curr_dirname/$fname"
  }
  return "$name"
}

proc add_ext {fname ext} {
  return [file rootname $fname]$ext
}

proc input_line {txt {cmd {}} {preset {}}  {w 12}} {
  global input_line_cmd input_line_data wm_fix
  set input_line_data {}
  if { [winfo exists .dialog] } return
  xschem set semaphore [expr {[xschem get semaphore] +1}]
  toplevel .dialog -class Dialog
  wm title .dialog {Input number}
  set X [expr {[winfo pointerx .dialog] - 60}]
  set Y [expr {[winfo pointery .dialog] - 35}]
  # 20100203
  if { $wm_fix } { tkwait visibility .dialog }
  wm geometry .dialog "+$X+$Y"
  set input_line_cmd $cmd
  frame .dialog.f1
  label .dialog.f1.l -text $txt
  entry .dialog.f1.e -width $w
  .dialog.f1.e insert 0 $preset
  .dialog.f1.e selection range 0 end
  
  pack .dialog.f1.l .dialog.f1.e -side left
  frame .dialog.f2
  button .dialog.f2.ok -text OK  -command {
    if {$input_line_cmd ne {}} {
      eval [subst -nocommands {$input_line_cmd [.dialog.f1.e get]}]
    }
    set input_line_data [.dialog.f1.e get]
    destroy .dialog
  }
  button .dialog.f2.cancel -text Cancel -command { destroy .dialog }
  pack .dialog.f2.ok  -anchor w -side left
  pack .dialog.f2.cancel -anchor e
  pack .dialog.f1
  pack .dialog.f2 -expand yes -fill x
  bind .dialog <Escape> {.dialog.f2.cancel invoke}
  bind .dialog <Return> {.dialog.f2.ok invoke}
  grab set .dialog
  focus .dialog.f1.e
  tkwait window .dialog
  xschem set semaphore [expr {[xschem get semaphore] -1}]
  return $input_line_data
}

proc launcher {launcher_var {launcher_program {} } } {
  # env, XSCHEM_SHAREDIR and netlist_dir not used directly but useful in paths passed thru launcher_var
  global launcher_default_program env XSCHEM_SHAREDIR netlist_dir
  
  if { ![string compare $launcher_program {}] } { set launcher_program $launcher_default_program}
  eval exec  [subst $launcher_program] {[subst $launcher_var]} &
}

proc reconfigure_layers_button { { topwin {} } } {
   global colors dark_colorscheme
   set c [xschem get rectcolor]
   $topwin.menubar.layers configure -background [lindex $colors $c]
   if { $dark_colorscheme == 1 && $c == 0} {
     $topwin.menubar.layers configure -foreground white
   } else {
     $topwin.menubar.layers configure -foreground black
   }
}

proc reconfigure_layers_menu { {topwin {} } } {
   global colors dark_colorscheme
   set j 0
   foreach i $colors {
     set ind_bg white
     if {  $j == [xschem get backlayer] } {
        if { $dark_colorscheme == 1 } { 
          set layfg white
        } else {
          set layfg black
        }
     } else {
        if { $dark_colorscheme == 1 } { 
          set layfg black
        } else {
          set layfg white
        }
     }
     $topwin.menubar.layers.menu entryconfigure $j -activebackground $i \
        -background $i -foreground $layfg -activeforeground $layfg
     incr j
   }
   reconfigure_layers_button $topwin
}

proc get_file_path {ff} {
  global env OS
  # Absolute path ? return as is.
  #        Linux                Windows
  if { [regexp {^/} $ff] || [regexp {^[a-zA-Z]:} $ff] } { return $ff }
  if {$OS == "Windows"} {
    set mylist [split $env(PATH) \;]
  } else {
    set mylist [split $env(PATH) :]
  }
  foreach i $mylist {
    set ii $i/$ff
    if { [file exists $ii]} {return $ii}
  }
  # nothing found, return $ff as is and hope for the best :-)
  return $ff
}

#
# Balloon help system, from https://wiki.tcl-lang.org/page/balloon+help
#
proc balloon {w help} {
    bind $w <Any-Enter> "after 1000 [list balloon_show %W [list $help]]"
    bind $w <Any-Leave> "destroy %W.balloon"
}

proc balloon_show {w arg} {
    if {[eval winfo containing  [winfo pointerxy .]]!=$w} {return}
    set top $w.balloon
    catch {destroy $top}
    toplevel $top -bd 1 -bg black
    wm overrideredirect $top 1
    if {[string equal [tk windowingsystem] aqua]}  {
        ::tk::unsupported::MacWindowStyle style $top help none
    }   
    pack [message $top.txt -aspect 10000 -bg lightyellow \
        -font fixed -text $arg]
    set wmx [winfo rootx $w]
    set wmy [expr {[winfo rooty $w]+[winfo height $w]}]
    wm geometry $top [winfo reqwidth $top.txt]x[winfo reqheight $top.txt]+$wmx+$wmy
    raise $top
}

proc context_menu { } {
  global retval

  set retval 0
  if {[info tclversion] >= 8.5} {
    set font {Sans 8 bold}
  } else {
    set font fixed
  }
  set selection  [expr {[xschem get lastsel] eq {1}}]
  toplevel .ctxmenu
  wm overrideredirect .ctxmenu 1
  set x [expr {[winfo pointerx .ctxmenu] - 10}]
  set y [expr {[winfo pointery .ctxmenu] - 10}]
  if { !$selection} {
    button .ctxmenu.b9 -text {Open most recent} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuRecent -compound left \
      -font [subst $font] -command {set retval 9; destroy .ctxmenu} 
  }
  button .ctxmenu.b10 -text {Edit attributes} -padx 3 -pady 0 -anchor w -activebackground grey50 \
    -image CtxmenuEdit -compound left \
    -font [subst $font] -command {set retval 10; destroy .ctxmenu}
  button .ctxmenu.b11 -text {Edit attr in editor} -padx 3 -pady 0 -anchor w -activebackground grey50 \
    -image CtxmenuEdit -compound left \
    -font [subst $font] -command {set retval 11; destroy .ctxmenu}
  if {$selection} {
    button .ctxmenu.b12 -text {Descend schematic} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuDown -compound left \
      -font [subst $font] -command {set retval 12; destroy .ctxmenu}
    button .ctxmenu.b13 -text {Descend symbol} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuDownSym -compound left \
      -font [subst $font] -command {set retval 13; destroy .ctxmenu}
    button .ctxmenu.b18 -text {Delete selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuDelete -compound left \
      -font [subst $font] -command {set retval 18; destroy .ctxmenu}
    button .ctxmenu.b7 -text {Cut selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuCut -compound left \
      -font [subst $font] -command {set retval 7; destroy .ctxmenu}
    button .ctxmenu.b15 -text {Copy selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuCopy -compound left \
      -font [subst $font] -command {set retval 15; destroy .ctxmenu}
    button .ctxmenu.b16 -text {Move selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuMove -compound left \
      -font [subst $font] -command {set retval 16; destroy .ctxmenu}
    button .ctxmenu.b17 -text {Duplicate selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuDuplicate -compound left \
      -font [subst $font] -command {set retval 17; destroy .ctxmenu}
  }
  if {!$selection} {
    button .ctxmenu.b14 -text {Go to upper level} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuUp -compound left \
      -font [subst $font] -command {set retval 14; destroy .ctxmenu}
    button .ctxmenu.b1 -text {Insert symbol} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuSymbol -compound left \
      -font [subst $font] -command {set retval 1; destroy .ctxmenu}
    button .ctxmenu.b2 -text {Insert wire} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuWire -compound left \
      -font [subst $font] -command {set retval 2; destroy .ctxmenu}
    button .ctxmenu.b3 -text {Insert line} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuLine -compound left \
      -font [subst $font] -command {set retval 3; destroy .ctxmenu}
    button .ctxmenu.b4 -text {Insert box} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuBox -compound left \
      -font [subst $font] -command {set retval 4; destroy .ctxmenu}
    button .ctxmenu.b5 -text {Insert polygon} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuPoly -compound left \
      -font [subst $font] -command {set retval 5; destroy .ctxmenu}
    button .ctxmenu.b19 -text {Insert arc} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuArc -compound left \
      -font [subst $font] -command {set retval 19; destroy .ctxmenu}
    button .ctxmenu.b20 -text {Insert circle} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuCircle -compound left \
      -font [subst $font] -command {set retval 20; destroy .ctxmenu}
    button .ctxmenu.b6 -text {Insert text} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuText -compound left \
      -font [subst $font] -command {set retval 6; destroy .ctxmenu}
    button .ctxmenu.b8 -text {Paste selection} -padx 3 -pady 0 -anchor w -activebackground grey50 \
      -image CtxmenuPaste -compound left \
      -font [subst $font] -command {set retval 8; destroy .ctxmenu}
  }
  button .ctxmenu.b21 -text {Abort command} -padx 3 -pady 0 -anchor w -activebackground grey50 \
    -image CtxmenuAbort -compound left \
    -font [subst $font] -command {set retval 21; destroy .ctxmenu}

  pack .ctxmenu.b21 -fill x -expand true
  if {!$selection} {
    pack .ctxmenu.b9  -fill x -expand true
  }
  pack .ctxmenu.b10 .ctxmenu.b11 -fill x -expand true
  if {$selection} {
    pack .ctxmenu.b12 .ctxmenu.b13 .ctxmenu.b18 -fill x -expand true
  }
  if {!$selection} {
    pack .ctxmenu.b14 -fill x -expand true
    pack .ctxmenu.b1 .ctxmenu.b2 .ctxmenu.b3 .ctxmenu.b4 .ctxmenu.b5 -fill x -expand true
    pack .ctxmenu.b19 .ctxmenu.b20 .ctxmenu.b6  -fill x -expand true
  }
  if {$selection} {
    pack .ctxmenu.b7 -fill x -expand true
    pack .ctxmenu.b15 .ctxmenu.b16 .ctxmenu.b17 -fill x -expand true
  }
  if {!$selection} {
    pack .ctxmenu.b8 -fill x -expand true
  }
  wm geometry .ctxmenu "+$x+$y"
  update
  # if window has been destroyed (by mouse pointer exiting) do nothing
  if { ![winfo exists .ctxmenu] } { return 0 }

  set wx [winfo width .ctxmenu]
  set wy [winfo height .ctxmenu]
  set sx [winfo screenwidth .]
  set sy [winfo screenheight .]
  if { $y + $wy > $sy } {
    set y [expr {$y - ( $y + $wy - $sy )} ]
  }
  if { $x + $wx > $sx } {
    set x [expr {$x - ( $x + $wx - $sx )} ]
  }
  wm geometry .ctxmenu "+$x+$y";# move away from screen edges
  bind .ctxmenu <Leave> {if { {%W} eq {.ctxmenu} } {destroy .ctxmenu}}
  tkwait window .ctxmenu
  return $retval
}

#
# toolbar: Public variables that we allow to be overridden
# Code contributed by Neil Johnson (github: nejohnson)
#
proc setup_toolbar {} {
  global toolbar_visible toolbar_horiz toolbar_list XSCHEM_SHAREDIR
  set_ne toolbar_visible 0
  set_ne toolbar_horiz   1
  set_ne toolbar_list { 
    FileOpen
    FileSave
    FileReload
    ---
    EditUndo
    EditRedo
    EditCut
    EditCopy
    EditPaste
    EditDelete
    ---
    EditDuplicate
    EditMove
    ---
    EditPushSch
    EditPushSym
    EditPop
    ---
    ViewRedraw
    ViewToggleColors
    ---
    ToolInsertSymbol
    ToolInsertText
    ToolInsertWire
    ToolInsertLine
    ToolInsertRect
    ToolInsertPolygon
    ToolInsertArc
    ToolInsertCircle
    ---
    ToolSearch
    ---
    ToolJoinTrim
    ToolBreak
  }
  #
  # Pull in the toolbar graphics resources
  #
  source $XSCHEM_SHAREDIR/resources.tcl
  #
  # Separation bar counter
  #
}

#
# Toolbar constructor
#
proc toolbar_toolbar { {topwin {} } } {
    frame $topwin.toolbar -relief raised -bd 0 -bg white
}

#
# Create a tool button which may be displayed
#
proc toolbar_create {name cmd { help "" } {topwin {} } } {
    button $topwin.toolbar.b$name -image img$name -relief flat -bd 1 -bg white -fg white \
    -padx 0 -pady 0 -command $cmd
    if { $help == "" } { balloon $topwin.toolbar.b$name $name } else { balloon $topwin.toolbar.b$name $help }
}

#
# Show the toolbar in horizontal or vertical position, parsing the toolbar list and 
# adding any separators as needed.
#
proc toolbar_show { { topwin {} } } {
    global toolbar_horiz toolbar_list toolbar_visible tabbed_interface
    if { ! $toolbar_visible } { return }
    if { $toolbar_horiz } { 
        if {$tabbed_interface} {
          pack $topwin.toolbar -fill x -before $topwin.tabs 
        } else {
          pack $topwin.toolbar -fill x -before $topwin.drw
        }
    } else {
        pack $topwin.toolbar -side left -anchor w -fill y -before $topwin.drw
    }
    set pos "top"
    if { $toolbar_horiz } { set pos "left" }
    set tlist [ winfo children $topwin.toolbar ]
    set toolbar_sepn 0
    foreach b $toolbar_list {
        if { $b == "---" } {
            if { $toolbar_horiz } {
                frame $topwin.toolbar.sep$toolbar_sepn -bg lightgrey -width 2
                pack $topwin.toolbar.sep$toolbar_sepn -side $pos -padx 1 -pady 0 -fill y
            } else {
                frame $topwin.toolbar.sep$toolbar_sepn -bg lightgrey -height 2
                pack $topwin.toolbar.sep$toolbar_sepn -side $pos -padx 0 -pady 1 -fill x
            }
            incr toolbar_sepn
        } else {
            if { [ lsearch -exact $tlist "$topwin.toolbar.b$b" ] != -1 } {
                pack $topwin.toolbar.b$b -side $pos
            } else {
                puts "Error: unknown toolbar item \"$b\""
            }
        }
    }
    set pos "bottom"
    if { $toolbar_horiz } { set pos "right" }
    foreach b { Waves Simulate Netlist } {
        pack $topwin.toolbar.b$b -side $pos
    }
    set $toolbar_visible 1
}

#
# Hide the toolbar, unpack the buttons, and remove any separators
#
proc toolbar_hide { { topwin {} } } {
    global toolbar_visible
    set tlist [ winfo children $topwin.toolbar ]
    foreach b $tlist {
        pack forget $b
        if { [ string match "$topwin.toolbar.sep*" $b ] == 1 } { 
            destroy $b
        }
    }
    pack forget $topwin.toolbar
    set $toolbar_visible 0
}

proc setup_tabbed_interface {} {
  global tabbed_interface

  if { $tabbed_interface } {
    if { ![winfo exists .tabs] } {
      frame .tabs
      button .tabs.x0 -padx 2 -pady 0 -anchor nw -text Main -command "xschem new_schematic switch_tab .drw"
      bind .tabs.x0 <ButtonPress> {swap_tabs %X %Y press}
      bind .tabs.x0 <ButtonRelease> {swap_tabs %X %Y release}
      button .tabs.add -padx 0 -pady 0  -text { + } -command "xschem new_schematic create"
      pack .tabs.x0 .tabs.add -side left
      pack .tabs  -fill x -side top -expand false -side top -before .drw
    }
  } else {
    destroy .tabs
  }
  if {$tabbed_interface} {
    .menubar.file.menu entryconfigure 6 -state disabled
    .menubar.file.menu entryconfigure 7 -state disabled
    set_tab_names 
  } else {
    .menubar.file.menu entryconfigure 6 -state normal
    .menubar.file.menu entryconfigure 7 -state normal
  } 
  # update tabbed window close (X) function
  if {$tabbed_interface} {
    wm protocol . WM_DELETE_WINDOW { 
      if { [xschem get current_win_path] eq {.drw} } {
        xschem exit
      } else { 
        xschem new_schematic destroy [xschem get current_win_path] {}
      }
    }
  # restore non tabbed window close function for main window
  } else {
    wm protocol . WM_DELETE_WINDOW {
      set old  [xschem get current_win_path]
      save_ctx $old
      restore_ctx .drw
      housekeeping_ctx
      xschem new_schematic switch_win .drw
      xschem exit
      # did not exit (user cancel) ... switch back 
      restore_ctx $old
      housekeeping_ctx
      xschem new_schematic switch_win $old
    }
  }
}

proc delete_tab {path} {
  regsub {\.drw$} $path {} path
  destroy .tabs$path
}

# button press on a tab, drag onto another tab, release button --> swap
proc swap_tabs {x y what} {
  if {$what eq {press} } {
    # puts "From: [winfo containing $x $y]"
    set tctx::source_swap_tab [winfo containing $x $y]
  } else {
    # puts "To: [winfo containing $x $y]"
    set tctx::dest_swap_tab [winfo containing $x $y]
    if {[info exists tctx::source_swap_tab] && [info exists tctx::dest_swap_tab]} {
      set cond1 [regexp {\.tabs\.x} $tctx::source_swap_tab]
      set cond2 [regexp {\.tabs\.x} $tctx::dest_swap_tab]
      set cond3 [expr { $tctx::source_swap_tab ne $tctx::dest_swap_tab }]
      if { $cond1 && $cond2  && $cond3} {
        # puts "ok for swapping ctx"
        set tablist [pack slaves .tabs]
        set sourceidx [lsearch -exact $tablist $tctx::source_swap_tab]
        set destidx [lsearch -exact $tablist $tctx::dest_swap_tab]
        incr sourceidx
        incr destidx
        set following_source_tab [lindex $tablist $sourceidx]
        set following_dest_tab [lindex $tablist $destidx]
        # puts " $tablist  --> $following_source_tab   $following_dest_tab"
        pack $tctx::source_swap_tab -side left -before $following_dest_tab
        pack $tctx::dest_swap_tab -side left -before  $following_source_tab
      }
      set tctx::source_swap_tab {}
      set tctx::dest_swap_tab {}
    }
  }
}

proc prev_tab {} {
  global tabbed_interface
  if { !$tabbed_interface} { return}
  set currwin [xschem get current_win_path]
  regsub {\.drw} $currwin {} tabname
  if {$tabname eq {}} { set tabname .x0}
  regsub {\.x} $tabname {} number
  set next_tab $number
  set highest -10000000
  set xcoord [winfo rootx .tabs$tabname]
  for {set i 0} {$i < $tctx::max_new_windows} { incr i} {
    if { $i == $number} { continue}
    if { [winfo exists .tabs.x$i] } {
      set tab_coord  [winfo rootx .tabs.x$i]
      if {$tab_coord < $xcoord && $tab_coord > $highest} {
        set next_tab $i
        set highest $tab_coord
      }
    }
  }
  .tabs.x$next_tab invoke
}

proc next_tab {} {
  global tabbed_interface
  if { !$tabbed_interface} {return}
  set currwin [xschem get current_win_path]
  regsub {\.drw} $currwin {} tabname
  if {$tabname eq {}} { set tabname .x0}
  regsub {\.x} $tabname {} number
  set next_tab $number
  set lowest 10000000
  set xcoord [winfo rootx .tabs$tabname]
  for {set i 0} {$i < $tctx::max_new_windows} { incr i} {
    if { $i == $number} { continue}
    if { [winfo exists .tabs.x$i] } {
      set tab_coord [winfo rootx .tabs.x$i]
      if {$tab_coord > $xcoord && $tab_coord < $lowest} {
        set next_tab $i
        set lowest $tab_coord
      }
    }
  }
  .tabs.x$next_tab invoke
}

proc set_tab_names {{mod {}}} {
  global tabbed_interface has_x

  if {[info exists has_x] && $tabbed_interface } {
    set currwin [xschem get current_win_path]
    regsub {\.drw} $currwin {} tabname
    if {$tabname eq {}} { set tabname .x0}
    .tabs$tabname configure -text [file tail [xschem get schname]]$mod -bg Palegreen
    if {$tabname eq {.x0}} {
      .tabs$tabname configure -fg red
    }
    for { set i 0} { $i < $tctx::max_new_windows} { incr i} {
      if { [winfo exists .tabs.x$i] && ($tabname ne ".x$i")} {
         .tabs.x$i configure -bg $tctx::tab_bg
      }
    }
  }
}

proc raise_dialog {parent window_path } {
  if {[winfo exists .dialog] && [winfo ismapped .dialog] && [winfo ismapped $parent] &&
      [wm stackorder .dialog isbelow $parent ]} {
    raise .dialog $window_path
  }
  if {[winfo exists .graphdialog] && [winfo ismapped .graphdialog] && [winfo ismapped $parent] &&
      [wm stackorder .graphdialog isbelow $parent ]} {
    raise .graphdialog $window_path
  }
}

proc set_old_tk_fonts {} {
  if {[info tclversion] <= 8.4} {
     set myfont {-*-helvetica-*-r-*-*-12-*-*-*-*-*-*-*}
     set mymonofont fixed
     option add *Button*font $myfont startupFile
     option add *Menubutton*font $myfont startupFile
     option add *Menu*font $myfont startupFile
     option add *Listbox*font $myfont startupFile
     option add *Entry*font $mymonofont startupFile
     option add *Text*font $mymonofont startupFile
     option add *Label*font $myfont startupFile
  }
}

proc every {interval script} {
    uplevel #0 $script
    after $interval [list every $interval $script]
}

## tcl context switching global namespace
namespace eval tctx {
  variable tctx
  variable i
  variable global_list
  variable global_array_list
  variable dialog_list
  variable tab_bg
  variable max_new_windows
  variable source_swap_tab
  variable dest_swap_tab
}

## list of dialogs: when open do not perform context switching
## do not include .infotext as it is always open (in withdrawn mode)
set tctx::dialog_list { .ctxmenu .alert .sim .dialog .tclcmd .sl .dim }
 
proc no_open_dialogs {} {
  set res 1
  foreach tctx::i $tctx::dialog_list {
    if { [winfo exists $tctx::i] } { 
      # puts "$tctx::i dialog open"
      set res 0
    }
  }
  return $res
}

## list of globals to save/restore on context switching
## EXCEPTIONS, not to be saved/restored:
## "textwindow_wcounter" should be kept unique as it is the number of open textwindows
## "viewdata_wcounter" should be kept unique as it is the number of open viewdatas
## "measure_id" should be kept unique since we allow only one measure tooltip in graphs
## "tabbed_interface"


set tctx::global_list {
  auto_hilight autotrim_wires bespice_listen_port big_grid_points bus_replacement_char
  cadgrid cadlayers cadsnap cairo_font_name
  change_lw color_ps colors connect_by_kissing constrained_move copy_cell custom_label_prefix custom_token dark_colors
  dark_colorscheme dim_bg dim_value disable_unique_names do_all_inst draw_grid draw_window
  edit_prop_pos edit_prop_size editprop_sympath edit_symbol_prop_new_sel enable_dim_bg enable_stretch 
  en_hilight_conn_inst filetmp
  flat_netlist fullscreen gaw_fd gaw_tcp_address globfilter
  graph_bus graph_digital
  graph_sel_color graph_schname graph_selected graph_sel_wave graph_sort
  graph_unlocked
  hide_empty_graphs hide_symbols hsize hspice_netlist 
  incr_hilight infowindow_text INITIALINSTDIR INITIALLOADDIR INITIALPROPDIR INITIALTEXTDIR
  input_line_cmd input_line_data launcher_default_program light_colors line_width local_netlist_dir
  measure_text
  myload_d myload_default_geometry myload_dir1 myload_dir2 myload_dirs2 myload_files1 myload_files2 myload_index1
  myload_retval myload_sash_pos myload_sel myload_type myload_yview netlist_dir netlist_show
  netlist_type no_change_attrs noprint_libs old_selected_tok
  only_probes path pathlist persistent_command preserve_unchanged_attrs prev_symbol ps_colors rainbow_colors
  rawfile_loaded rcode recentfile replace_key retval retval_orig rotated_text save_initialfile search_exact
  search_found search_select search_value selected_tok show_infowindow show_pin_net_names 
  simconf_default_geometry simconf_vpos 
  spiceprefix split_files svg_colors svg_font_name symbol symbol_width sym_txt tclcmd_txt tclstop
  text_line_default_geometry textwindow_fileid textwindow_filename textwindow_w tmp_bus_char 
  toolbar_horiz toolbar_visible top_subckt transparent_svg undo_type
  use_label_prefix use_lab_wire user_wants_copy_cell verilog_2001
  viewdata_fileid viewdata_filename viewdata_w vsize xschem_libs xschem_listen_port
}

## list of global arrays to save/restore on context switching
## EXCEPTIONS, not to be saved/restored:
## execute
set tctx::global_array_list {
  dircolor sim enable_layer
}

proc delete_ctx {context} {
  global has_x
  if {![info exists $has_x]} {return}
  set tctx::tctx $context
  uplevel #0 {
    # puts "delete_ctx $tctx::tctx"
    array unset $tctx::tctx
    foreach tctx::i $tctx::global_array_list {
      if { [array exists ${tctx::tctx}_$tctx::i] } {
        array unset ${tctx::tctx}_$tctx::i
      }
    }
  }
}

proc restore_ctx {context} {
  global has_x
  if {![info exists has_x]} {return}
  # puts "restoring tcl context $context : semaphore=[xschem get semaphore]"
  set tctx::tctx $context
  array unset ::sim
  uplevel #0 {
    if { [ array exists $tctx::tctx ] } {
      # puts "restore_ctx $tctx::tctx"
      ## Cleanup these vars to avoid side effects from previous ctx
      unset -nocomplain gaw_fd
      foreach tctx::i $tctx::global_list {
        if { [info exists [subst $tctx::tctx]($tctx::i)] } {
          # puts "restoring:  $tctx::i"
          set $tctx::i [set [subst $tctx::tctx]($tctx::i)]
        }
      }
    }
    foreach tctx::i $tctx::global_array_list {
      if { [array exists ${tctx::tctx}_$tctx::i] } {
        array set $tctx::i [array get [subst ${tctx::tctx}_$tctx::i]]
      }
    }
  }
}

proc save_ctx {context} {
  global has_x
  if {![info exists has_x]} {return}
  # puts "saving tcl context $context : semaphore=[xschem get semaphore]"
  set tctx::tctx $context
  uplevel #0 {
    # puts "save_ctx $tctx::tctx"
    foreach tctx::i $tctx::global_list {
      if { [info exists $tctx::i] } {
        # puts "saving:  $tctx::i"
        set [subst $tctx::tctx]($tctx::i) [set $tctx::i]
      }
    }
    foreach tctx::i $tctx::global_array_list {
      if { [array exists $tctx::i] } {
        array set [subst ${tctx::tctx}_$tctx::i] [array get $tctx::i]
      }
    }
  }
}

proc housekeeping_ctx {} {
  global has_x
  if {![info exists $has_x]} {return}
  uplevel #0 {
    # puts housekeeping_ctx
  }
}

proc simulate_button {button_path} {
  if { [set simulate_bg [$button_path cget -bg]] ne {red} } {
    $button_path configure -bg red
    simulate "$button_path configure -bg $simulate_bg"
  }
}

proc set_bindings {topwin} {
global env has_x OS
  ###
  ### Tk event handling
  ###

  # puts "set_binding: topwin=$topwin"
  if {($OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 ) && [info exists has_x]} {
    set parent [winfo toplevel $topwin]
  
    bind $parent <Expose> [list raise_dialog $parent $topwin]
    bind $parent <Visibility> [list raise_dialog $parent $topwin]
    bind $parent <FocusIn> [list raise_dialog $parent $topwin]
    # send non-existent event just to force change schematic window context.
    bind $parent <Enter> "
       if { {$parent} eq {.}} {
         if { {%W} eq {$parent}} {
           # send a fake event just to force context switching in callback()
           xschem callback .drw -55 0 0 0 0 0 0
         }
       } else {
         if { {%W} eq {$parent}} {
           # send a fake event just to force context switching in callback()
           xschem callback $parent.drw -55 0 0 0 0 0 0
         }
       }
    "
    bind $topwin <Leave> "graph_show_measure stop"
    bind $topwin <Expose> "xschem callback %W %T %x %y 0 %w %h %s"
    bind $topwin <Double-Button-1> "xschem callback %W -3 %x %y 0 %b 0 %s"
    bind $topwin <Double-Button-2> "xschem callback %W -3 %x %y 0 %b 0 %s"
    bind $topwin <Double-Button-3> "xschem callback %W -3 %x %y 0 %b 0 %s"
    bind $topwin <Configure> "xschem callback %W %T %x %y 0 %w %h 0"
    bind $topwin <ButtonPress> "xschem callback %W %T %x %y 0 %b 0 %s"
    bind $topwin <ButtonRelease> "xschem callback %W %T %x %y 0 %b 0 %s"
    bind $topwin <KeyPress> "xschem callback %W %T %x %y %N 0 0 %s"
    bind $topwin <KeyRelease> "xschem callback %W %T %x %y %N 0 0 %s"
    bind $topwin <Motion> "focus $topwin; xschem callback %W %T %x %y 0 0 0 %s"
    bind $topwin <Enter> "destroy .ctxmenu; focus $topwin; xschem callback %W %T %x %y 0 0 0 0"
    bind $topwin <Unmap> " wm withdraw .infotext; set show_infowindow 0 "
    bind $topwin  "?" {textwindow "${XSCHEM_SHAREDIR}/xschem.help"}
  
    # on Windows Alt key mask is reported as 131072 (1<<17) so build masks manually with values passed from C code 
    if {$OS == "Windows" } {
      bind $topwin <Alt-KeyPress> {xschem callback %W %T %x %y %N 0 0 [expr {$Mod1Mask}]}
      bind $topwin <Control-Alt-KeyPress> {xschem callback %W %T %x %y %N 0 0 [expr {$ControlMask + $Mod1Mask}]}
      bind $topwin <Shift-Alt-KeyPress> {xschem callback %W %T %x %y %N 0 0 [expr {$ShiftMask + $Mod1Mask}]}
      bind $topwin <MouseWheel> {
        if {%D<0} {
          xschem callback %W 4 %x %y 0 5 0 %s
        } else {
          xschem callback %W 4 %x %y 0 4 0 %s
        }
      }
    }
  }
}

## this function sets up all tk windows and binds X events. It is executed by xinit.c after completing 
## all X initialization. This avoids race conditions.
## In previous flow xschem.tcl was setting up windows and events before X initialization was completed by xinit.c.
## this could lead to crashes on some (may be slow) systems due to Configure/Expose events being delivered
## before xschem being ready to handle them.
proc pack_widgets { { topwin {} } } {
  global env has_x OS tabbed_interface
  if {($OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 ) && [info exists has_x]} {
    pack $topwin.statusbar.2 -side left 
    pack $topwin.statusbar.3 -side left 
    pack $topwin.statusbar.4 -side left 
    pack $topwin.statusbar.5 -side left 
    pack $topwin.statusbar.6 -side left 
    pack $topwin.statusbar.7 -side left 
    pack $topwin.statusbar.8 -side left 
    pack $topwin.statusbar.1 -side left -fill x
    pack $topwin.menubar -anchor n -side top -fill x
    pack $topwin.drw -anchor n -side top -fill both -expand true
    setup_tabbed_interface
    if {$tabbed_interface} {
       pack $topwin.tabs  -fill x -side top -expand false -side top
    }
    toolbar_show $topwin
    pack $topwin.statusbar -after $topwin.drw -anchor sw  -fill x 
    bind $topwin.statusbar.5 <Leave> "set cadgrid \[$topwin.statusbar.5 get\]; xschem set cadgrid \$cadgrid"
    bind $topwin.statusbar.3 <Leave> "set cadsnap \[$topwin.statusbar.3 get\]; xschem set cadsnap \$cadsnap"
  }
}

# if undo_type == disk save undo to disk
# if undo_type == memory keep undo in memory
# In memory undo is faster but in case of crashes nothing can be recovered
# On disk undo is slower but can be used to recover state just before a crash
proc switch_undo {} {
  global undo_type
  if { $undo_type eq {memory} } {
    set res [tk_messageBox -type yesno -parent [xschem get topwindow] \
            -message {Ok to keep undo in memory? Undo will be reset.\
            Memory undo is faster but in case of crashes nothing can be recovered.}]
    if {$res eq {yes} } {
      xschem undo_type $undo_type
    } else {
      set undo_type disk
    }
  } else { ;# disk
    set res [tk_messageBox -type yesno -parent [xschem get topwindow] \
            -message {Ok to save undo on disk? Undo will be reset.\
            Disk undo is slower but in case of crashes previous state can be recovered.}]
    if {$res eq {yes} } {
      xschem undo_type $undo_type
    } else {
      set undo_type memory
    }
  }
}

proc build_widgets { {topwin {} } } {
  global XSCHEM_SHAREDIR tabbed_interface
  global colors recentfile color_ps transparent_svg menu_debug_var enable_stretch
  global netlist_show flat_netlist split_files hspice_netlist tmp_bus_char 
  global draw_grid big_grid_points sym_txt change_lw incr_hilight symbol_width
  global cadgrid draw_window show_pin_net_names toolbar_visible hide_symbols undo_type
  global disable_unique_names persistent_command autotrim_wires en_hilight_conn_inst
  global local_netlist_dir editor netlist_type netlist_dir spiceprefix initial_geometry
  set mbg {}
  set bbg {-highlightthickness 0}
  if { $topwin ne {}} {
    set mbg {-bg gray50}
    set bbg {-bg gray50 -highlightthickness 0}
  }
  eval frame $topwin.menubar -relief raised -bd 2 $mbg
  toolbar_toolbar $topwin
  eval menubutton $topwin.menubar.file -text "File" -menu $topwin.menubar.file.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.file.menu -tearoff 0
  eval menubutton $topwin.menubar.edit -text "Edit" -menu $topwin.menubar.edit.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.edit.menu -tearoff 0
  eval menubutton $topwin.menubar.option -text "Options" -menu $topwin.menubar.option.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.option.menu -tearoff 0
  eval menubutton $topwin.menubar.view -text "View" -menu $topwin.menubar.view.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.view.menu -tearoff 0
  eval menubutton $topwin.menubar.prop -text "Properties" -menu $topwin.menubar.prop.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.prop.menu -tearoff 0
  eval menubutton $topwin.menubar.layers -text "Layers" -menu $topwin.menubar.layers.menu \
   -padx 3 -pady 0 -background [lindex $colors 4]
  menu $topwin.menubar.layers.menu -tearoff 0
  eval menubutton $topwin.menubar.tools -text "Tools" -menu $topwin.menubar.tools.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.tools.menu -tearoff 0
  eval menubutton $topwin.menubar.sym -text "Symbol" -menu $topwin.menubar.sym.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.sym.menu -tearoff 0
  eval menubutton $topwin.menubar.hilight -text "Highlight" -menu $topwin.menubar.hilight.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.hilight.menu -tearoff 0
  eval menubutton $topwin.menubar.simulation -text "Simulation" -menu $topwin.menubar.simulation.menu \
   -padx 3 -pady 0 $mbg
  menu $topwin.menubar.simulation.menu -tearoff 0
  eval menubutton $topwin.menubar.help -text "Help" -menu $topwin.menubar.help.menu \
  -padx 3 -pady 0 $mbg
  menu $topwin.menubar.help.menu -tearoff 0
  $topwin.menubar.help.menu add command -label "Help" -command "textwindow \"${XSCHEM_SHAREDIR}/xschem.help\" ro" \
       -accelerator {?}
  $topwin.menubar.help.menu add command -label "Keys" -command "textwindow \"${XSCHEM_SHAREDIR}/keys.help\" ro"
  $topwin.menubar.help.menu add command -label "About XSCHEM" -command "about"
  
  $topwin.menubar.file.menu add command -label "New Schematic"  -accelerator Ctrl+N\
    -command {
      xschem clear SCHEMATIC
    }
  # toolbar_create FileNew {xschem clear SCHEMATIC} "New Schematic" $topwin
  $topwin.menubar.file.menu add command -label "New Symbol" -accelerator Ctrl+Shift+N \
    -command {
      xschem clear SYMBOL
    }
  # toolbar_create FileNewSym {xschem clear SYMBOL} "New Symbol" $topwin
  $topwin.menubar.file.menu add command -label "New empty Schematic window" -accelerator {Alt+N} \
    -command {
      xschem new_window
    }
  $topwin.menubar.file.menu add command -label "New empty Symbol window" -accelerator {Alt+Shift+N} \
    -command {
      xschem new_symbol_window
    }
  $topwin.menubar.file.menu add command -label "Open" -command "xschem load" -accelerator {Ctrl+O}
  $topwin.menubar.file.menu add cascade -label "Open Recent" -menu $topwin.menubar.file.menu.recent
  $topwin.menubar.file.menu add cascade -label {Open Recent in new window} \
    -menu $topwin.menubar.file.menu.recent_new_window
  menu $topwin.menubar.file.menu.recent_new_window -tearoff 0
  menu $topwin.menubar.file.menu.recent -tearoff 0
  setup_recent_menu 0 $topwin
  setup_recent_menu 1 $topwin
  $topwin.menubar.file.menu add command -label {Open new window} -command "xschem load_new_window"
  if {$tabbed_interface} {
    $topwin.menubar.file.menu entryconfigure 6 -state disabled
    $topwin.menubar.file.menu entryconfigure 7 -state disabled
  }
  toolbar_create FileOpen "xschem load" "Open File" $topwin
  $topwin.menubar.file.menu add command -label "Delete files" -command "xschem delete_files" -accelerator {Shift-D}

  $topwin.menubar.file.menu add command -label "Open Most Recent" \
    -command {xschem load [lindex "$recentfile" 0]} -accelerator {Ctrl+Shift+O}
  $topwin.menubar.file.menu add command -label "Save" -command "xschem save" -accelerator {Ctrl+S}
  toolbar_create FileSave "xschem save" "Save File" $topwin
  $topwin.menubar.file.menu add command -label "Merge" -command "xschem merge" -accelerator {Shift+B}
  # toolbar_create FileMerge "xschem merge" "Merge File" $topwin
  $topwin.menubar.file.menu add command -label "Reload" -accelerator {Alt+S} \
    -command {
     if { [string compare [tk_messageBox -type okcancel -parent [xschem get topwindow] \
             -message {Are you sure you want to reload?}] ok]==0 } {
             xschem reload
        }
    }
  toolbar_create FileReload {
     if { [string compare [tk_messageBox -type okcancel -parent [xschem get topwindow] \
             -message {Are you sure you want to reload?}] ok]==0 } {
             xschem reload
        }
    } "Reload File" $topwin
  $topwin.menubar.file.menu add command -label "Save as" -command "xschem saveas" -accelerator {Ctrl+Shift+S}
  $topwin.menubar.file.menu add command -label "Save as symbol" \
     -command "xschem saveas {} SYMBOL" -accelerator {Ctrl+Alt+S}
  # added svg, png 20171022
  $topwin.menubar.file.menu add command -label "PDF/PS Export" -command "xschem print pdf" -accelerator {*}
  $topwin.menubar.file.menu add command -label "Hierarchical PDF/PS Export" -command "xschem hier_psprint"
  $topwin.menubar.file.menu add command -label "PNG Export" -command "xschem print png" -accelerator {Ctrl+*}
  $topwin.menubar.file.menu add command -label "SVG Export" -command "xschem print svg" -accelerator {Alt+*}
  $topwin.menubar.file.menu add separator
  $topwin.menubar.file.menu add command -label "Exit" -accelerator {Ctrl+Q} -command {
    if {[xschem get current_win_path] eq {.drw} } {
      xschem exit
    } else {
      xschem new_schematic destroy [xschem get current_win_path] {}
    }
  }
  $topwin.menubar.option.menu add checkbutton -label "Color Postscript/SVG" -variable color_ps \
     -command {
        if { $color_ps==1 } {xschem set color_ps 1} else { xschem set color_ps 0}
     }
  $topwin.menubar.option.menu add checkbutton -label "Transparent SVG background" -variable transparent_svg
  $topwin.menubar.option.menu add checkbutton -label "Debug mode" -variable menu_debug_var \
     -command {
        if { $menu_debug_var==1 } {xschem debug 1} else { xschem debug 0}
     }
  $topwin.menubar.option.menu add checkbutton -label "Undo buffer on Disk" -variable undo_type \
     -onvalue disk -offvalue memory -command {switch_undo}
  $topwin.menubar.option.menu add checkbutton -label "Enable stretch" -variable enable_stretch \
     -accelerator Y 
  $topwin.menubar.option.menu add checkbutton -label "Show netlist win" -variable netlist_show \
     -accelerator {Shift+A} 
  $topwin.menubar.option.menu add checkbutton -label "Flat netlist" -variable flat_netlist \
     -accelerator : \
     -command {
        if { $flat_netlist==1 } {xschem set flat_netlist 1} else { xschem set flat_netlist 0} 
     }
  $topwin.menubar.option.menu add checkbutton -label "Split netlist" -variable split_files \
     -accelerator {} 
  $topwin.menubar.option.menu add checkbutton -label "hspice / ngspice netlist" -variable hspice_netlist \
     -accelerator {}
  $topwin.menubar.option.menu add command -label "Replace \[ and \] for buses in SPICE netlist" \
     -command {
       input_line "Enter two characters to replace default bus \[\] delimiters:" "set tmp_bus_char"
       if { [info exists tmp_bus_char] && [string length $tmp_bus_char] >=2} {
         set bus_replacement_char $tmp_bus_char
       } 
     }
  $topwin.menubar.option.menu add checkbutton -label "Verilog 2001 netlist variant" -variable verilog_2001
  $topwin.menubar.option.menu add checkbutton -label "Draw grid" -variable draw_grid \
     -accelerator {%} \
     -command {
       xschem redraw
     }
  $topwin.menubar.option.menu add command -label "Half Snap Threshold" -accelerator G -command {
         xschem set cadsnap [expr {[xschem get cadsnap] / 2.0} ]
       }
  $topwin.menubar.option.menu add command -label "Double Snap Threshold" -accelerator Shift-G -command {
         xschem set cadsnap [expr {[xschem get cadsnap] * 2.0} ]
       }
  $topwin.menubar.option.menu add checkbutton -label "Variable grid point size" -variable big_grid_points \
     -command { xschem redraw }
  $topwin.menubar.option.menu add command -label "Set symbol width" \
       -command {
         input_line "Enter Symbol width ($symbol_width)" "set symbol_width" $symbol_width 
       }
  $topwin.menubar.option.menu add checkbutton -label "Show net names on symbol pins" -variable show_pin_net_names \
     -command {
        xschem show_pin_net_names
        xschem redraw
     }
  $topwin.menubar.option.menu add separator
  $topwin.menubar.option.menu add radiobutton -label "Spice netlist" -variable netlist_type -value spice \
       -accelerator {Shift+V} \
       -command "xschem set netlist_type spice"
  $topwin.menubar.option.menu add radiobutton -label "VHDL netlist" -variable netlist_type -value vhdl \
       -accelerator {Shift+V} \
       -command "xschem set netlist_type vhdl"
  $topwin.menubar.option.menu add radiobutton -label "Verilog netlist" -variable netlist_type -value verilog \
       -accelerator {Shift+V} \
       -command "xschem set netlist_type verilog"
  $topwin.menubar.option.menu add radiobutton -label "tEDAx netlist" -variable netlist_type -value tedax \
       -accelerator {Shift+V} \
       -command "xschem set netlist_type tedax"
  $topwin.menubar.option.menu add radiobutton -label "Symbol global attrs" -variable netlist_type -value symbol \
       -accelerator {Shift+V} \
       -command "xschem set netlist_type symbol"
  $topwin.menubar.edit.menu add command -label "Undo" -command "xschem undo; xschem redraw" -accelerator U
  toolbar_create EditUndo "xschem undo; xschem redraw" "Undo" $topwin
  $topwin.menubar.edit.menu add command -label "Redo" -command "xschem redo; xschem redraw" -accelerator {Shift+U}
  toolbar_create EditRedo "xschem redo; xschem redraw" "Redo" $topwin
  toolbar_create EditCut "xschem cut" "Cut" $topwin
  $topwin.menubar.edit.menu add command -label "Copy" -command "xschem copy" -accelerator Ctrl+C
  toolbar_create EditCopy "xschem copy" "Copy" $topwin
  $topwin.menubar.edit.menu add command -label "Cut" -command "xschem cut"   -accelerator Ctrl+X
  $topwin.menubar.edit.menu add command -label "Paste" -command "xschem paste" -accelerator Ctrl+V
  toolbar_create EditPaste "xschem paste" "Paste" $topwin
  $topwin.menubar.edit.menu add command -label "Delete" -command "xschem delete" -accelerator Del
  toolbar_create EditDelete "xschem delete" "Delete" $topwin
  $topwin.menubar.edit.menu add command -label "Select all" -command "xschem select_all" -accelerator Ctrl+A
  $topwin.menubar.edit.menu add command -label "Edit schematic in new window/tab" \
      -command "xschem schematic_in_new_window" -accelerator Alt+E
  $topwin.menubar.edit.menu add command -label "Edit symbol in new window/tab" \
      -command "xschem symbol_in_new_window" -accelerator Alt+I
  $topwin.menubar.edit.menu add command -label "Duplicate objects" -command "xschem copy_objects" -accelerator C
  toolbar_create EditDuplicate "xschem copy_objects" "Duplicate objects" $topwin
  $topwin.menubar.edit.menu add command -label "Move objects" -command "xschem move_objects" -accelerator M
  toolbar_create EditMove "xschem move_objects" "Move objects" $topwin
  $topwin.menubar.edit.menu add command -label "Flip selected objects" -command "xschem flip" -accelerator {Alt-F}
  $topwin.menubar.edit.menu add command -label "Rotate selected objects" -command "xschem rotate" -accelerator {Alt-R}
  $topwin.menubar.edit.menu add radiobutton -label "Unconstrained move" -variable constrained_move \
     -value 0 -command {xschem set constrained_move 0} 
  $topwin.menubar.edit.menu add radiobutton -label "Constrained Horizontal move" -variable constrained_move \
     -value 1 -accelerator H -command {xschem set constrained_move 1} 
  $topwin.menubar.edit.menu add radiobutton -label "Constrained Vertical move" -variable constrained_move \
     -value 2 -accelerator V -command {xschem set constrained_move 2} 
  $topwin.menubar.edit.menu add checkbutton -label "Add wire when separating pins" -variable connect_by_kissing 
  $topwin.menubar.edit.menu add command -label "Push schematic" -command "xschem descend" -accelerator E
  toolbar_create EditPushSch "xschem descend" "Push schematic" $topwin
  $topwin.menubar.edit.menu add command -label "Push symbol" -command "xschem descend_symbol" -accelerator I
  toolbar_create EditPushSym "xschem descend_symbol" "Push symbol" $topwin
  $topwin.menubar.edit.menu add command -label "Pop" -command "xschem go_back" -accelerator Ctrl+E
  toolbar_create EditPop "xschem go_back" "Pop" $topwin
  eval button $topwin.menubar.waves -text "Waves"  -activebackground red  -takefocus 0 \
   -padx 2 -pady 0 -command waves $bbg
  eval button $topwin.menubar.simulate -text "Simulate"  -activebackground red  -takefocus 0 \
   -padx 2 -pady 0 -command \{simulate_button $topwin.menubar.simulate\} $bbg
  eval button $topwin.menubar.netlist -text "Netlist"  -activebackground red  -takefocus 0 \
   -padx 2 -pady 0 -command \{xschem netlist\} $bbg
  # create  $topwin.menubar.layers.menu
  create_layers_menu $topwin
  $topwin.menubar.view.menu add checkbutton -label "Show ERC Info window" -variable show_infowindow \
    -command {
       if { $show_infowindow != 0 } {wm deiconify .infotext
       } else {wm withdraw .infotext}
     }
  $topwin.menubar.view.menu add command -label "Redraw" -command "xschem redraw" -accelerator Esc
  toolbar_create ViewRedraw "xschem redraw" "Redraw" $topwin
  $topwin.menubar.view.menu add checkbutton -label "Fullscreen" -variable fullscreen \
     -accelerator {Alt+Shift+F} -command "
        xschem fullscreen $topwin
     "
  $topwin.menubar.view.menu add command -label "Zoom Full" -command "xschem zoom_full" -accelerator F
  $topwin.menubar.view.menu add command -label "Zoom In" -command "xschem zoom_in" -accelerator Shift+Z
  # toolbar_create ViewZoomIn "xschem zoom_in" "Zoom In" $topwin
  $topwin.menubar.view.menu add command -label "Zoom Out" -command "xschem zoom_out" -accelerator Ctrl+Z
  # toolbar_create ViewZoomOut "xschem zoom_out" "Zoom Out" $topwin
  $topwin.menubar.view.menu add command -label "Zoom box" -command "xschem zoom_box" -accelerator Z
  # toolbar_create ViewZoomBox "xschem zoom_box" "Zoom Box" $topwin
  $topwin.menubar.view.menu add command -label "Set snap value" \
         -command {
         input_line "Enter snap value ( default: [xschem get cadsnap_default] current: [xschem get cadsnap])" \
         "xschem set cadsnap" [xschem get cadsnap]
       }
  $topwin.menubar.view.menu add command -label "Set grid spacing" \
       -command {
         input_line "Enter grid spacing (float):" "xschem set cadgrid" $cadgrid
       }
  $topwin.menubar.view.menu add checkbutton -label "View only Probes" -variable only_probes \
         -accelerator {5} \
         -command { xschem only_probes }
  $topwin.menubar.view.menu add command -label "Toggle colorscheme"  -accelerator {Shift+O} -command {
          xschem toggle_colorscheme
          xschem build_colors 1
          xschem redraw
       }
   toolbar_create ViewToggleColors {
          xschem toggle_colorscheme
          xschem build_colors 1
          xschem redraw
       } "Toggle Color Scheme" $topwin
  $topwin.menubar.view.menu add command -label "Dim colors"  -accelerator {} -command {
          color_dim
       }
  $topwin.menubar.view.menu add command -label "Visible layers"  -accelerator {} -command {
          select_layers
          xschem redraw
       }
  $topwin.menubar.view.menu add command -label "Change current layer color"  -accelerator {} -command {
          change_color
       }
  $topwin.menubar.view.menu add command -label "Reset all colors to default" \
         -accelerator {} -command {
          reset_colors 1
         }
  $topwin.menubar.view.menu add checkbutton -label "No XCopyArea drawing model" -variable draw_window \
         -accelerator {Ctrl+$} \
         -command {
           if { $draw_window == 1} { xschem set draw_window 1} else { xschem set draw_window 0}
         }
  $topwin.menubar.view.menu add checkbutton -label "Symbol text" -variable sym_txt \
     -accelerator {Ctrl+B} -command { xschem set sym_txt $sym_txt; xschem redraw }
  $topwin.menubar.view.menu add checkbutton -label "Toggle variable line width" -variable change_lw \
     -accelerator {_}
  $topwin.menubar.view.menu add command -label "Set line width" \
       -command {
         input_line "Enter linewidth (float):" "xschem line_width"
       }
  $topwin.menubar.view.menu add checkbutton -label "Show Toolbar" -variable toolbar_visible \
     -command "
        if { \$toolbar_visible } \" toolbar_show $topwin\" else \"toolbar_hide $topwin\"
     "
  $topwin.menubar.view.menu add checkbutton -label "Horizontal Toolbar" -variable toolbar_horiz \
     -command " 
        if { \$toolbar_visible } \" toolbar_hide $topwin; toolbar_show $topwin \"
     "
  $topwin.menubar.view.menu add checkbutton -label "Tabbed interface" -variable tabbed_interface \
    -command setup_tabbed_interface
  $topwin.menubar.prop.menu add command -label "Edit" -command "xschem edit_prop" -accelerator Q
  $topwin.menubar.prop.menu add command -label "Edit with editor" -command "xschem edit_vi_prop" -accelerator Shift+Q
  $topwin.menubar.prop.menu add command -label "View" -command "xschem view_prop" -accelerator Ctrl+Shift+Q
  $topwin.menubar.prop.menu add command -background red -label "Edit file (danger!)" \
     -command "xschem edit_file" -accelerator Alt+Q
  $topwin.menubar.sym.menu add radiobutton -label "Show Symbols" \
     -variable hide_symbols -value 0 \
     -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
  $topwin.menubar.sym.menu add radiobutton -label "Show instance Bounding boxes for subcircuit symbols" \
     -variable hide_symbols -value 1 \
     -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
  $topwin.menubar.sym.menu add radiobutton -label "Show instance Bounding boxes for all symbols" \
     -variable hide_symbols -value 2 \
     -command {xschem set hide_symbols $hide_symbols; xschem redraw} -accelerator Alt+B
  $topwin.menubar.sym.menu add command -label "Make symbol from schematic" -command "xschem make_symbol" -accelerator A
  $topwin.menubar.sym.menu add command -label "Make schematic from symbol" -command "xschem make_sch" -accelerator Ctrl+L
  $topwin.menubar.sym.menu add command -label "Make schematic and symbol from selected components" \
     -command "xschem make_sch_from_sel" -accelerator Ctrl+Shift+H
  $topwin.menubar.sym.menu add command -label "Attach net labels to component instance" \
     -command "xschem attach_labels" -accelerator Shift+H
  $topwin.menubar.sym.menu add command -label "Create symbol pins from selected schematic pins" \
          -command "schpins_to_sympins" -accelerator Alt+H
  $topwin.menubar.sym.menu add command -label "Place symbol pin" \
          -command "xschem add_symbol_pin" -accelerator Alt+P
  $topwin.menubar.sym.menu add command -label "Print list of highlight nets" \
          -command "xschem print_hilight_net 1" -accelerator J
  $topwin.menubar.sym.menu add command -label "Print list of highlight nets, with buses expanded" \
          -command "xschem print_hilight_net 3" -accelerator Alt-Ctrl-J
  $topwin.menubar.sym.menu add command -label "Create labels from highlight nets" \
          -command "xschem print_hilight_net 4" -accelerator Alt-J
  $topwin.menubar.sym.menu add command -label "Create labels from highlight nets with 'i' prefix" \
          -command "xschem print_hilight_net 2" -accelerator Alt-Shift-J
  $topwin.menubar.sym.menu add command -label "Create pins from highlight nets" \
          -command "xschem print_hilight_net 0" -accelerator Ctrl-J
  $topwin.menubar.sym.menu add checkbutton -label "Allow duplicated instance names (refdes)" \
      -variable disable_unique_names
  $topwin.menubar.tools.menu add checkbutton -label "Remember last command" -variable persistent_command
  $topwin.menubar.tools.menu add command -label "Insert symbol" -command "xschem place_symbol" -accelerator {Ins, Shift-I}
  toolbar_create ToolInsertSymbol "xschem place_symbol" "Insert Symbol" $topwin
  $topwin.menubar.tools.menu add command -label "Insert wire label" -command "xschem net_label 1" -accelerator {Alt-L}
  $topwin.menubar.tools.menu add command -label "Insert wire label 2" -command "xschem net_label 0" \
     -accelerator {Alt-Shift-L}
  $topwin.menubar.tools.menu add command -label "Insert text" -command "xschem place_text" -accelerator T
  toolbar_create ToolInsertText "xschem place_text" "Insert Text" $topwin
  $topwin.menubar.tools.menu add command -label "Insert wire" -command "xschem wire" -accelerator W
  toolbar_create ToolInsertWire "xschem wire" "Insert Wire" $topwin
  $topwin.menubar.tools.menu add command -label "Insert snap wire" -command "xschem snap_wire" -accelerator Shift+W
  $topwin.menubar.tools.menu add command -label "Insert line" -command "xschem line" -accelerator L
  toolbar_create ToolInsertLine "xschem line" "Insert Line" $topwin
  $topwin.menubar.tools.menu add command -label "Insert rect" -command "xschem rect" -accelerator R
  toolbar_create ToolInsertRect "xschem rect" "Insert Rectangle" $topwin
  $topwin.menubar.tools.menu add command -label "Insert polygon" -command "xschem polygon" -accelerator Ctrl+W
  toolbar_create ToolInsertPolygon "xschem polygon" "Insert Polygon" $topwin
  $topwin.menubar.tools.menu add command -label "Insert arc" -command "xschem arc" -accelerator Shift+C
  toolbar_create ToolInsertArc "xschem arc" "Insert Arc" $topwin
  $topwin.menubar.tools.menu add command -label "Insert circle" -command "xschem circle" -accelerator Ctrl+Shift+C
  toolbar_create ToolInsertCircle "xschem circle" "Insert Circle" $topwin
  $topwin.menubar.tools.menu add command -label "Insert PNG image" -command "xschem add_png"
  $topwin.menubar.tools.menu add command -label "Search" -accelerator Ctrl+F -command  property_search
  toolbar_create ToolSearch property_search "Search" $topwin
  $topwin.menubar.tools.menu add command -label "Align to Grid" -accelerator Alt+U -command  "xschem align"
  $topwin.menubar.tools.menu add command -label "Execute TCL command" -command  "tclcmd"
  $topwin.menubar.tools.menu add command -label "Join/Trim wires" \
     -command "xschem trim_wires" -accelerator {&}
   toolbar_create ToolJoinTrim "xschem trim_wires" "Join/Trim Wires" $topwin
  $topwin.menubar.tools.menu add command -label "Break wires at selected instance pins" \
     -command "xschem break_wires" -accelerator {!}
   toolbar_create ToolBreak "xschem break_wires" "Break wires at selected\ninstance pin intersections" $topwin
  $topwin.menubar.tools.menu add checkbutton -label "Auto Join/Trim Wires" -variable autotrim_wires \
     -command {
         if {$autotrim_wires == 1} {
           xschem trim_wires
           xschem redraw
         }
     }
  $topwin.menubar.tools.menu add command -label "Select all connected wires/labels/pins" \
     -accelerator {Shift-Right Butt.} \
     -command { xschem connected_nets}
  $topwin.menubar.tools.menu add command -label "Select conn. wires, stop at junctions" \
     -accelerator {Ctrl-Righ Butt.} -command { xschem connected_nets 1 }
  $topwin.menubar.hilight.menu add command \
   -label {Highlight net-pin name mismatches on selected instances} \
   -command "xschem net_pin_mismatch" \
   -accelerator {Shift-X} 
  $topwin.menubar.hilight.menu add command -label {Highlight duplicate instance names} \
     -command "xschem check_unique_names 0"  -accelerator {#} 
  $topwin.menubar.hilight.menu add command -label {Rename duplicate instance names} \
     -command "xschem check_unique_names 1" -accelerator {Ctrl+#}
  $topwin.menubar.hilight.menu add command -label {Propagate Highlight selected net/pins} \
     -command "xschem hilight drill" -accelerator {Ctrl+Shift+K}
  $topwin.menubar.hilight.menu add checkbutton -label "Increment Hilight Color" -variable incr_hilight
  $topwin.menubar.hilight.menu add command -label {Highlight selected net/pins} \
     -command "xschem hilight" -accelerator K
  $topwin.menubar.hilight.menu add command -label {Send selected net/pins to Viewer} \
     -command "xschem send_to_viewer" -accelerator Alt+G
  $topwin.menubar.hilight.menu add command -label {Select hilight nets / pins} \
     -command "xschem select_hilight_net" \
     -accelerator Alt+K
  $topwin.menubar.hilight.menu add command -label {Un-highlight all net/pins} \
     -command "xschem unhilight_all" -accelerator Shift+K
  $topwin.menubar.hilight.menu add command -label {Un-highlight selected net/pins} \
     -command "xschem unhilight" -accelerator Ctrl+K
  # 20160413
  $topwin.menubar.hilight.menu add checkbutton -label {Auto-highlight net/pins} -variable auto_hilight
  $topwin.menubar.hilight.menu add checkbutton -label {Enable highlight connected instances} \
    -variable en_hilight_conn_inst

  $topwin.menubar.simulation.menu add command -label "Set netlist Dir" \
    -command {
          select_netlist_dir 1
    }
  $topwin.menubar.simulation.menu add command -label "Set top level netlist name" \
    -command {
          input_line {Set netlist file name} {xschem set netlist_name} [xschem get netlist_name] 40
    }
  $topwin.menubar.simulation.menu add checkbutton -label "Use 'simulation' dir under current schematic dir" \
    -variable local_netlist_dir \
    -command { if {$local_netlist_dir == 0 } { select_netlist_dir 1 } else { simuldir} }
  $topwin.menubar.simulation.menu add command -label {Configure simulators and tools} -command {simconf}
  $topwin.menubar.simulation.menu add command -label {Utile Stimuli Editor (GUI)} -command {
     simuldir
     inutile stimuli.[file rootname [file tail [xschem get schname]]]
  }
  $topwin.menubar.simulation.menu add command -label {Utile Stimuli Translate} -command {
     simuldir
     inutile_translate  stimuli.[file rootname [file tail [xschem get schname]]]
  }
  $topwin.menubar.simulation.menu add command -label {Shell [simulation path]} -command {
     if { [select_netlist_dir 0] ne "" } {
        get_shell $netlist_dir
     }
   }
  $topwin.menubar.simulation.menu add command -label {Edit Netlist} \
     -command {edit_netlist [file tail [xschem get schname]]}
  $topwin.menubar.simulation.menu add command -label {Send highlighted nets to viewer} \
    -command {xschem create_plot_cmd} -accelerator Shift+J
  $topwin.menubar.simulation.menu add checkbutton -label "Hide graphs if no spice data loaded" \
     -variable hide_empty_graphs -command {xschem redraw}
  $topwin.menubar.simulation.menu add checkbutton -variable rawfile_loaded \
     -label {Load/Unload ngspice .raw file} -command {
     xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
  }
  $topwin.menubar.simulation.menu add command -label {Add waveform graph} -command {xschem add_graph}
  $topwin.menubar.simulation.menu add separator
  $topwin.menubar.simulation.menu add checkbutton -label "LVS netlist: Top level is a .subckt" -variable top_subckt 
  $topwin.menubar.simulation.menu add checkbutton -label "Use 'spiceprefix' attribute" -variable spiceprefix \
         -command {xschem save; xschem reload}

  toolbar_create Netlist { xschem netlist } "Create netlist" $topwin
  toolbar_create Simulate "simulate_button $topwin.menubar.simulate" "Run simulation" $topwin
  toolbar_create Waves { waves } "View results" $topwin

  pack $topwin.menubar.file -side left
  pack $topwin.menubar.edit -side left
  pack $topwin.menubar.option -side left
  pack $topwin.menubar.view -side left
  pack $topwin.menubar.prop -side left
  pack $topwin.menubar.layers -side left
  pack $topwin.menubar.tools -side left
  pack $topwin.menubar.sym -side left
  pack $topwin.menubar.hilight -side left
  pack $topwin.menubar.simulation -side left
  pack $topwin.menubar.help -side right
  pack $topwin.menubar.waves -side right
  pack $topwin.menubar.simulate -side right
  pack $topwin.menubar.netlist -side right
  # used to check status of Simulate button later. This variable is constant, never changed
  frame $topwin.drw -background {} -takefocus 1

  focus $topwin.drw
  if { $topwin == {} } {set rootwin .} else { set rootwin $topwin} 
  wm  title $rootwin "xschem - "
  wm iconname $rootwin "xschem - "
  $rootwin configure  -background {}
  wm  geometry $rootwin $initial_geometry
  #wm maxsize . 1600 1200
  if {$tabbed_interface && $rootwin eq {.}} {
    wm protocol $rootwin WM_DELETE_WINDOW {
      if { [xschem get current_win_path] eq {.drw} } {
        xschem exit
      } else {
        xschem new_schematic destroy [xschem get current_win_path] {}
      }
    }
  } elseif { $rootwin == {.}} {
    wm protocol $rootwin WM_DELETE_WINDOW {
       set old  [xschem get current_win_path]
       save_ctx $old
       restore_ctx .drw
       housekeeping_ctx
       xschem new_schematic switch_win .drw
       xschem exit
       # did not exit ... switch back 
       restore_ctx $old
       housekeeping_ctx
       xschem new_schematic switch_win $old
    }
  } else {
    wm protocol $rootwin WM_DELETE_WINDOW "xschem new_schematic destroy $topwin.drw {}"
  }

  frame $topwin.statusbar  
  label $topwin.statusbar.1   -text "STATUS BAR 1"  
  label $topwin.statusbar.2   -text "SNAP:"
  entry $topwin.statusbar.3 -relief sunken -bg white \
         -width 10 -foreground black -takefocus 0
  label $topwin.statusbar.4   -text "GRID:"
  entry $topwin.statusbar.5 -relief sunken -bg white \
         -width 10 -foreground black -takefocus 0
  label $topwin.statusbar.6   -text "NETLIST MODE:"
  label $topwin.statusbar.7 -relief sunken -bg white \
         -width 8 
  label $topwin.statusbar.8 -activebackground red -text {} 
}

proc set_paths {} {
  global XSCHEM_LIBRARY_PATH env pathlist OS
  set pathlist {}
  if { [info exists XSCHEM_LIBRARY_PATH] } {
    if {$OS == "Windows"} {
      set pathlist_orig [split $XSCHEM_LIBRARY_PATH \;]
    } else {
      set pathlist_orig [split $XSCHEM_LIBRARY_PATH :]
    }
    foreach i $pathlist_orig {
      regsub {^~} $i $env(HOME) i
      if { ![string compare $i .] } {
        lappend pathlist $i
      } elseif { [ regexp {\.\.\/} $i] } {
        lappend pathlist [file normalize $i]
      } elseif { [ file exists $i] } {
        lappend pathlist $i
      }
    }
  }
}

proc print_help_and_exit {} {
  global XSCHEM_SHAREDIR
  if { [xschem get help ]} {
    set fd [open "${XSCHEM_SHAREDIR}/xschem.help" r]
    set helpfile [read $fd]
    puts $helpfile
    close $fd
    exit
  }
}

proc set_missing_colors_to_black {} {
  global enable_layer cadlayers svg_colors ps_colors light_colors dark_colors
  ## pad missing colors with black
  for {set i 0} { $i<$cadlayers } { incr i} {
    set_ne enable_layer($i) 1
    foreach j { svg_colors ps_colors light_colors dark_colors } {
      if { ![string compare [lindex [set $j] $i] {} ] } {
        if { ![string compare $j {ps_colors} ] || ![string compare $j {svg_colors} ]} {
          lappend $j {0x000000}
        } else {
          lappend $j {#000000}
        }
      }
    }
  }
  foreach i {svg_colors ps_colors light_colors dark_colors} {
    if { [llength [set $i]] > $cadlayers} {
       set $i [lrange [set $i] 0 [expr {$cadlayers -1}]]
    }
  }
}

proc set_initial_dirs {} {
  global INITIALLOADDIR INITIALINSTDIR INITIALPROPDIR pathlist
  set INITIALLOADDIR {}
  set INITIALINSTDIR {}
  set INITIALPROPDIR {}
  foreach i $pathlist  {
    if { [file exists $i] } {
      set INITIALLOADDIR $i
      set INITIALINSTDIR $i
      set INITIALPROPDIR $i
      break
    }
  }
}

proc create_layers_menu { {topwin {} } } {
  global dark_colorscheme colors
  if { $dark_colorscheme == 1 } { set txt_color black} else { set txt_color white} 
  set j 0
  foreach i $colors {
    ## 20121121
    if {  $j == [xschem get pinlayer] } { 
      set laylab [format %2d $j]-PIN
      set layfg $txt_color
    } elseif { $j == [xschem get wirelayer] } { 
      set laylab [format %2d $j]-WIRE
      set layfg $txt_color
    } elseif { $j == [xschem get textlayer] } { ;# 20161206
      set laylab [format %2d $j]-TEXT
      set layfg $txt_color
    } elseif { $j == [xschem get backlayer] } { ;# 20161206
      set laylab [format %2d $j]-BG
      if { $dark_colorscheme == 1 } {
        set layfg white
      } else {
        set layfg black
      }
    } elseif { $j == [xschem get gridlayer] } { ;# 20161206
      set laylab [format %2d $j]-GRID
      set layfg $txt_color
    } else {
      set laylab "[format %2d $j]        "
      set layfg $txt_color
    }
    $topwin.menubar.layers.menu add command -label $laylab  -activeforeground $layfg \
       -foreground $layfg -background $i -activebackground $i \
       -command "xschem set rectcolor $j; reconfigure_layers_button $topwin"
    if { [expr {$j%10}] == 0 } { $topwin.menubar.layers.menu entryconfigure $j -columnbreak 1 }
    incr j
  }
}   

proc set_replace_key_binding {} {
  global replace_key
  if {[array exists replace_key]} {
    foreach i [array names replace_key] {
      key_binding "$i" "$replace_key($i)"
    }
  }
}

proc source_user_tcl_files {} {
  global tcl_files
  foreach i $tcl_files {
    uplevel #0 "source $i"
  }
}

proc setup_tcp_xschem {} {
  global xschem_listen_port xschem_server_getdata
  if { [info exists xschem_listen_port] && ($xschem_listen_port ne {}) } { 
    if {[catch {socket -server xschem_server $xschem_listen_port} err]} {
      puts "problems listening to TCP port: $xschem_listen_port"
      puts $err
      return 0
    } else {
      set xschem_server_getdata(server) $err
    }
  }
  return 1
}

proc setup_tcp_bespice {} {
  global bespice_listen_port bespice_server_getdata
  if { [info exists bespice_listen_port] && ($bespice_listen_port ne {}) } { 
    if {[catch {socket -server bespice_server $bespice_listen_port} err]} {
      puts "problems listening to TCP port: $bespice_listen_port"
      puts $err
      return 0
    } else {
      set bespice_server_getdata(server) $err
    }
  }
  return 1
}

### 
###   MAIN PROGRAM
###
set OS [lindex $tcl_platform(os) 0]
set env(LC_ALL) C

# tcl variable XSCHEM_LIBRARY_PATH  should already be set in xschemrc
set_paths
print_help_and_exit

if {$OS == "Windows"} {
  set_ne XSCHEM_TMP_DIR [xschem get temp_dir]
} else {
  set_ne XSCHEM_TMP_DIR {/tmp}
}

# used in C code
set_ne xschem_libs {}
set_ne noprint_libs {}
set_ne debug_var 0
# used to activate debug from menu
set_ne menu_debug_var 0
set textwindow_wcounter 1
set viewdata_wcounter 1
set retval ""
set prev_symbol ""
set symbol ""

# 20100204 flag to enable fix for dialog box positioning,  issues with some wm
set wm_fix 0 

# 20171010
set tclcmd_txt {}

###
### user preferences: set default values
###

if { ![info exists dircolor] } {
  set_ne dircolor(/share/xschem/) red
  set_ne dircolor(/share/doc/xschem/) {#338844}
}

set_ne globfilter {*}
## list of tcl procedures to load at end of xschem.tcl
set_ne tcl_files {}
set_ne netlist_dir "$USER_CONF_DIR/simulations"
# this global exists only for netlist_type radiobuttons, don't use, use [xschem] subcommand to get/set values
# it is also used in xschemrc to set initial netlist type.
set_ne netlist_type spice
set_ne local_netlist_dir 0 ;# if set use <sch_dir>/simulation for netlist and sims
set_ne bus_replacement_char {} ;# use {<>} to replace [] with <> in bussed signals
set_ne hspice_netlist 1
set_ne top_subckt 0
set_ne hide_empty_graphs 0 ;# if set to 1 waveform boxes will be hidden if no raw file loaded
set_ne spiceprefix 1
set_ne verilog_2001 1
set_ne split_files 0
set_ne flat_netlist 0
set_ne netlist_show 0
set_ne color_ps 1
set_ne transparent_svg 0
set_ne only_probes 0  ; # 20110112
set_ne fullscreen 0
set_ne unzoom_nodrift 0
set_ne change_lw 1
set_ne line_width 0
set_ne draw_window 0
set_ne incr_hilight 1
set_ne enable_stretch 0
set_ne constrained_move 0
set_ne connect_by_kissing 0
set_ne draw_grid 1
set_ne big_grid_points 0
set_ne persistent_command 0
set_ne autotrim_wires 0
set_ne disable_unique_names 0
set_ne sym_txt 1
set_ne show_infowindow 0 
set_ne symbol_width 150
set_ne editor {gvim -f}
set_ne rainbow_colors 0
set_ne initial_geometry {900x600}
set_ne edit_symbol_prop_new_sel {}
set_ne launcher_default_program {xdg-open}
set_ne auto_hilight 0
set_ne en_hilight_conn_inst 0
## xpm to png conversion
set_ne to_png {gm convert} 
## ps to pdf conversion
set_ne to_pdf {ps2pdf}

# selected graph user is editing attributes with graph GUI
set_ne graph_bus 0
set_ne graph_selected {}
set_ne graph_schname {}
# user clicked this wave 
set_ne graph_sel_wave {}
# flag to force simulation stop (Esc key pressed) 
set_ne tclstop 0
## undo_type: disk or memory
set_ne undo_type disk

## show tab bar (tabbed interface) 
set_ne tabbed_interface 0
## case insensitive symbol lookup (on case insensitive filesystems only!)
set_ne case_insensitive 0

## remember edit_prop widget size
set_ne edit_prop_size 80x12
set_ne text_line_default_geometry 80x12
set_ne terminal xterm

# set_ne analog_viewer waveview
set_ne computerfarm {} ;# 20151007

# xschem tcp port number (listen to port and execute commands from there if set) 
# set a port number in xschemrc if you want accept remote connections.
set_ne xschem_listen_port {}

# server for bespice waveform connection (listen to port and send commands to bespice if set)
# set a port number in xschemrc if you want xschem to be able to cross-probe to bespice
set_ne bespice_listen_port {}

# hide instance details (show only bbox) 
set_ne hide_symbols 0
# show net names if symbol has attributes like @#n:net_name (where n = pin number or pin name) 
# and net_name=true global attribute set on symbol or instance.
set_ne show_pin_net_names 0
# gaw tcp {host port} 
set_ne gaw_tcp_address {localhost 2020}

## cairo stuff 20171112
set_ne cairo_font_scale 1.0
set_ne nocairo_font_xscale .85
set_ne nocairo_font_yscale .88
set_ne cairo_font_line_spacing 1.0
set_ne cairo_vert_correct 0
set_ne nocairo_vert_correct 0

# Arial, Monospace Sans-Serif; default font to use if unspecified in text elements
set_ne cairo_font_name {Sans-Serif}
set_ne svg_font_name {Sans-Serif}

# has_cairo set by c program if cairo enabled
set has_cairo 0 
set rotated_text {} ;#20171208
set_ne dark_colorscheme 1
set_ne enable_dim_bg 0
set_ne dim_bg 0.0
set_ne dim_value 0.0
##### set colors
if {!$rainbow_colors} {
  set_ne cadlayers 22
  ## 20171113
  set_ne light_colors {
   "#ffffff" "#0044ee" "#aaaaaa" "#222222" "#229900"
   "#bb2200" "#00ccee" "#ff0000" "#888800" "#00aaaa"
   "#880088" "#00ff00" "#0000cc" "#666600" "#557755"
   "#aa2222" "#7ccc40" "#00ffcc" "#ce0097" "#d2d46b"
   "#ef6158" "#fdb200"}
  set_ne dark_colors {
   "#000000" "#00ccee" "#3f3f3f" "#cccccc" "#88dd00" 
   "#bb2200" "#00ccee" "#ff0000" "#ffff00" "#ffffff"
   "#ff00ff" "#00ff00" "#0044dd" "#aaaa00" "#aaccaa"
   "#ff7777" "#bfff81" "#00ffcc" "#ce0097" "#d2d46b" 
   "#ef6158" "#fdb200"}
} else {
  # rainbow colors for bitmapping
  # skip if colors defined in ~/.xschem 20121110
  set_ne cadlayers 35
  #20171113
  set_ne light_colors {
    "#000000" "#00ccee" "#aaaaaa" "#ffffff" "#88dd00"
    "#bb2200" "#0000e0" "#2000e0" "#4000e0" "#6000e0"
    "#8000e0" "#a000e0" "#c000e0" "#e000e0" "#e000c0"
    "#e000a0" "#e00080" "#e00060" "#e00040" "#e00020"
    "#e00000" "#e02000" "#e04000" "#e06000" "#e08000"
    "#e0a000" "#e0c000" "#e0e000" "#e0e020" "#e0e040"
    "#e0e060" "#e0e080" "#e0e0a0" "#e0e0c0" "#e0e0e0"
  }
  set_ne dark_colors {
    "#000000" "#00ccee" "#3f3f3f" "#ffffff" "#88dd00" 
    "#bb2200" "#0000e0" "#2000e0" "#4000e0" "#6000e0"
    "#8000e0" "#a000e0" "#c000e0" "#e000e0" "#e000c0"
    "#e000a0" "#e00080" "#e00060" "#e00040" "#e00020"
    "#e00000" "#e02000" "#e04000" "#e06000" "#e08000"
    "#e0a000" "#e0c000" "#e0e000" "#e0e020" "#e0e040"
    "#e0e060" "#e0e080" "#e0e0a0" "#e0e0c0" "#e0e0e0"
  }
}

# for svg and pdf draw 20121108
regsub -all {"} $dark_colors  {} svg_colors
regsub -all {#} $svg_colors  {0x} svg_colors
regsub -all {"} $light_colors  {} ps_colors
regsub -all {#} $ps_colors  {0x} ps_colors
set_missing_colors_to_black
# read-only vars to store defaults (so we can switch to default colors) 
set dark_colors_save $dark_colors
set light_colors_save $light_colors
set ps_colors_save $ps_colors
set svg_colors_save $svg_colors

set_ne colors $dark_colors
##### end set colors


# 20111005 added missing initialization of globals...
set_ne no_change_attrs 0
set_ne preserve_unchanged_attrs 0
set search_select 0

# 20111106 these vars are overwritten by caller with mktemp file names
if {$OS == "Windows"} {
  set filetmp $env(windir)/.tmp2
} else {
  set filetmp [pwd]/.tmp2
}

set rawfile_loaded 0

# flag bound to a checkbutton in symbol editprop form
# if set cell is copied when renaming it
set_ne copy_cell 0

load_recent_file
# schematic to preload in new windows 20090708
set_ne XSCHEM_START_WINDOW {}

# set INITIALLOADDIR INITIALINSTDIR INITIALPROPDIR as initial locations in load file dialog box
set_initial_dirs

set custom_token {lab}
set search_value {}
set search_exact 0

# 20171005
set custom_label_prefix {}

###
### build Tk widgets
###
if { ( $OS== "Windows" || [string length [lindex [array get env DISPLAY] 1] ] > 0 ) && [info exists has_x]} {
  setup_toolbar
  # for hyperlink in about dialog
  eval  font create Underline-Font [ font actual TkDefaultFont ]
  font configure Underline-Font -underline true -size 24
  . configure -cursor left_ptr
  set_old_tk_fonts ;# for xschem compiled with old tcl-tk libs
  if { [info exists tk_scaling] } {tk scaling $tk_scaling} ;# useful for 4k displays (set bigger widgets)
  set infowindow_text {}
  infowindow

  build_widgets {}

  ##
  ## packing top windows (pack instructions) and event binding (bind instructions) done in proc pack_widgets
  ## executed by xinit.c after finalizing X initialization. This avoid potential race conditions
  ## like Configure or Expose events being generated before xschem being ready to handle them.
  ##

  # allow user to modify key bindings
  set_replace_key_binding

  update
  xschem windowid . ;# set icon for window
} ;# end if {[exists has_x]}

# read custom colors
if { [file exists ${USER_CONF_DIR}/colors] } {
  source ${USER_CONF_DIR}/colors
}

if { [llength $dark_colors] < $cadlayers || [llength $light_colors] < $cadlayers } {
  puts stderr { Warning: wrong number of configured layers in light_colors or dark_colors variables.}
}
if { $dark_colorscheme == 1} { 
  set colors $dark_colors
} else {
  set colors $light_colors
}
regsub -all {"} $light_colors  {} ps_colors
regsub -all {#} $ps_colors  {0x} ps_colors
regsub -all {"} $colors {} svg_colors
regsub -all {#} $svg_colors {0x} svg_colors

if { $show_infowindow } { wm deiconify .infotext } 

# xschem listen and bespice listen
setup_tcp_xschem
setup_tcp_bespice

