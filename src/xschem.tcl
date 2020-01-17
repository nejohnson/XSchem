#
#  File: xschem.tcl
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
  global execute_id execute_pipe execute_data execute_cmd execute_wait_flag simulate_oldbg
  global execute_status execute_callback
  append execute_data($id) [read $execute_pipe($id) 1024]
  if {[eof $execute_pipe($id)]} {
      fileevent $execute_pipe($id) readable ""
      # setting pipe to blocking before closing allows to see if pipeline failed
      fconfigure $execute_pipe($id) -blocking 1
      set status 0
      if {[catch {close $execute_pipe($id)} err options]} {
        set details [dict get $options -errorcode]
        if {[lindex $details 0] eq "CHILDSTATUS"} {
            set status [lindex $details 2]
            if { $execute_status($id) } {
              viewdata "Failed: $execute_cmd($id)\nstderr:\n$err\ndata:\n$execute_data($id)" ro
            }
        } else {
          if { $execute_status($id) } {
            viewdata "Completed: $execute_cmd($id)\nstderr:\n$err\ndata:\n$execute_data($id)" ro
          }
        }
      } 
      if { $status == 0 } {
        if { $execute_status($id) } {
          viewdata "Completed: $execute_cmd($id)\ndata:\n$execute_data($id)" ro
        }
      }
      if { [info exists execute_callback($id)] } { eval $execute_callback($id); unset execute_callback($id) } 
      unset execute_pipe($id)
      unset execute_data($id)
      unset execute_status($id)
      unset execute_cmd($id)
  }
}


proc execute_wait {status args} {
  global execute_pipe execute_wait_flag
  xschem set semaphore [expr [xschem get semaphore] +1]
  set execute_wait_flag 1
  set id [eval execute $status $args]
  vwait execute_pipe($id)
  xschem set semaphore [expr [xschem get semaphore] -1]
  unset execute_wait_flag
  return $id
}

# equivalent to the 'exec' tcl function but keeps the event loop
# responding, so widgets get updated properly
# while waiting for process to end.
proc execute {status args} {
  global execute_id execute_status
  global execute_data
  global execute_cmd
  global execute_pipe
  if {![info exists execute_id]} {
      set execute_id 0
  } else {
      incr execute_id
  }
  set pipe [open "|$args" r]
  set execute_status($execute_id) $status
  set execute_pipe($execute_id) $pipe
  set execute_cmd($execute_id) $args
  set execute_data($execute_id) ""
  set id $execute_id
  fconfigure $pipe -blocking 0
  fileevent $pipe readable "execute_fileevent $id"
  return $execute_id
}

proc netlist {source_file show netlist_file} {
 global XSCHEM_SHAREDIR flat_netlist hspice_netlist netlist_dir
 global verilog_2001

 if [regexp {\.spice} $netlist_file ] {
   if { $hspice_netlist == 1 } {
     set hspice {-hspice}
   } else {
     set hspice {}
   }
   if {$flat_netlist==0} then {
     eval exec {awk -f ${XSCHEM_SHAREDIR}/spice.awk -- $hspice $netlist_dir/$source_file | \
          awk -f ${XSCHEM_SHAREDIR}/break.awk > $netlist_dir/$netlist_file}
   } else {
     eval exec {awk -f "${XSCHEM_SHAREDIR}/spice.awk -- $hspice $netlist_dir/$source_file | \
          awk -f ${XSCHEM_SHAREDIR}/flatten.awk | awk -f ${XSCHEM_SHAREDIR}/break.awk > $netlist_dir/$netlist_file}
   }
   if ![string compare $show "show"] {
      textwindow $netlist_dir/$netlist_file
   }
 } 
 if [regexp {\.vhdl} $netlist_file ] {
   eval exec {awk -f $XSCHEM_SHAREDIR/vhdl.awk $netlist_dir/$source_file > $netlist_dir/$netlist_file}
   if ![string compare $show "show"] {
     textwindow $netlist_dir/$netlist_file
   }
 }
 if [regexp {\.tdx$} $netlist_file ] {
   eval exec {awk -f $XSCHEM_SHAREDIR/tedax.awk $netlist_dir/$source_file \
              > $netlist_dir/$netlist_file}
   if ![string compare $show "show"] {
     textwindow $netlist_dir/$netlist_file
   }
 }
 if [regexp {\.v$} $netlist_file ] {
   eval exec {awk -f ${XSCHEM_SHAREDIR}/verilog.awk $netlist_dir/$source_file \
              > $netlist_dir/$netlist_file}

   # 20140409
   if { $verilog_2001==1 } { 
     eval exec {awk -f ${XSCHEM_SHAREDIR}/convert_to_verilog2001.awk $netlist_dir/$netlist_file > $netlist_dir/${netlist_file}vv}
     eval exec {mv $netlist_dir/${netlist_file}vv $netlist_dir/$netlist_file}
   }
   if ![string compare $show "show"] {
     textwindow "$netlist_dir/$netlist_file"
   }
 }

 if ![string compare $netlist_file "netlist"] {
         textwindow "$netlist_dir/netlist"
 }
 return {}
}

# 20161121
proc convert_to_pdf {filename dest} {
  global a3page
  if { $a3page == 1 } { set paper a3 } else { set paper a4 }
  if { ![catch "exec ps2pdf -sPAPERSIZE=$paper $filename" msg] } {
    # ps2pdf succeeded, so remove original .ps file
    file rename -force [file rootname $filename].pdf $dest
    if { ![xschem get debug_var] } {
      file delete $filename
    }
  }
}

# 20161121
proc convert_to_png {filename dest} {
  global to_png tcl_debug
  # puts "---> $to_png $filename $destfile"
  if { ![catch "exec $to_png $filename png:$dest" msg] } {
    # conversion succeeded, so remove original .xpm file
    if { ![xschem get debug_var] } {
      file delete $filename
    }
  }
}

#20171024
proc key_binding {  s  d } { 
# always specify Shift- modifier for capital letters
# see tk 'man keysyms' for key names
# example format for s, d: Control-Alt-Key-asterisk
#                          Control-Shift-Key-A
#                          Alt-Key-c
#                          ButtonPress-4
#
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
  if { [regexp {(Mod1|Alt)-} $d] } { set state [expr $state +8] }
  if { [regexp Control- $d] } { set state [expr $state +4] }
  if { [regexp Shift- $d] } { set state [expr $state +1] }
  if { [regexp ButtonPress-1 $d] } { set state [expr $state +0x100] }
  if { [regexp ButtonPress-2 $d] } { set state [expr $state +0x200] }
  if { [regexp ButtonPress-3 $d] } { set state [expr $state +0x400] }
  # puts "$state $key <${s}>"
  if {[regexp ButtonPress- $d]} {
    bind .drw "<${s}>" "xschem callback %T %x %y 0 $key 0 $state"
  } else {
    if {![string compare $d {} ] } {
      bind .drw "<${s}>" {}
    } else {
      bind .drw  "<${s}>" "xschem callback %T %x %y $keysym 0 0 $state"
    }
  }

}

proc edit_file {filename} {
 
 global editor
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
## netlist_type
## computerfarm
## terminal

proc save_sim_defaults {f} {
  global sim 
  
  set a [catch "open $f w" fd]
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
      puts $fd "# specify tool command (cmd), name (name), and if tool must run in foreground"
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

proc set_sim_defaults {} {
  ### spice 
  global sim terminal USER_CONF_DIR

  if { [winfo exists .sim] } {
    foreach tool $sim(tool_list) {
      for {set i 0} {$i < $sim($tool,n)} { incr i} {
        set sim($tool,$i,cmd) [.sim.topf.f.scrl.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
      }
    }
    return
  }  else {
    if { [info exists sim] } {unset sim}
    set sim(tool_list) {spice spicewave verilog verilogwave vhdl vhdlwave}
    set_ne sim(spice,0,cmd) {$terminal -e 'ngspice -i "$N" -a || sh'}
    set_ne sim(spice,0,name) {Ngspice}
    set_ne sim(spice,0,fg) 0
    set_ne sim(spice,0,st) 0
    
    set_ne sim(spice,1,cmd) {ngspice -b -r "$n.raw" -o "$n.out" "$N"}
    set_ne sim(spice,1,name) {Ngspice batch}
    set_ne sim(spice,1,fg) 0
    set_ne sim(spice,1,st) 1
    
    # number of configured spice simulators, and default one
    set_ne sim(spice,n) 2
    set_ne sim(spice,default) 0
    
    ### spice wave view
    set_ne sim(spicewave,0,cmd) {gaw "$n.raw" } 
    set_ne sim(spicewave,0,name) {Gaw viewer}
    set_ne sim(spicewave,0,fg) 0
    set_ne sim(spicewave,0,st) 0
   
    set_ne sim(spicewave,1,cmd) {echo load "$n.raw" > .spiceinit
  $terminal -e ngspice
  rm .spiceinit} 
    set_ne sim(spicewave,1,name) {Ngpice Viewer}
    set_ne sim(spicewave,1,fg) 0
    set_ne sim(spicewave,1,st) 0

    set_ne sim(spicewave,2,cmd) {rawtovcd "$n.raw" > "$n.vcd" && gtkwave "$n.vcd" "$n.sav" 2>/dev/null} 
    set_ne sim(spicewave,2,name) {Rawtovcd}
    set_ne sim(spicewave,2,fg) 0
    set_ne sim(spicewave,2,st) 0
    # number of configured spice wave viewers, and default one
    set_ne sim(spicewave,n) 3
    set_ne sim(spicewave,default) 0
    
    ### verilog
    set_ne sim(verilog,0,cmd) {iverilog -o .verilog_object -g2012 "$N" && vvp .verilog_object}
    set_ne sim(verilog,0,name) {Icarus verilog}
    set_ne sim(verilog,0,fg) 0
    set_ne sim(verilog,0,st) 1
    # number of configured verilog simulators, and default one
    set_ne sim(verilog,n) 1
    set_ne sim(verilog,default) 0
    
    ### verilog wave view
    set_ne sim(verilogwave,0,cmd) {gtkwave dumpfile.vcd "$N.sav" 2>/dev/null}
    set_ne sim(verilogwave,0,name) {Gtkwave}
    set_ne sim(verilogwave,0,fg) 0
    set_ne sim(verilogwave,0,st) 0
    # number of configured verilog wave viewers, and default one
    set_ne sim(verilogwave,n) 1
    set_ne sim(verilogwave,default) 0
    
    ### vhdl
    set_ne sim(vhdl,0,cmd) {ghdl -c --ieee=synopsys -fexplicit "$N" -r "$s" --wave="$n.ghw"}
    set_ne sim(vhdl,0,name) {Ghdl}
    set_ne sim(vhdl,0,fg) 0
    set_ne sim(vhdl,0,st) 1
    # number of configured vhdl simulators, and default one
    set_ne sim(vhdl,n) 1
    set_ne sim(vhdl,default) 0
    
    ### vhdl wave view
    set_ne sim(vhdlwave,0,cmd) {gtkwave "$n.ghw" "$N.sav" 2>/dev/null}
    set_ne sim(vhdlwave,0,name) {Gtkwave}
    set_ne sim(vhdlwave,0,fg) 0
    set_ne sim(vhdlwave,0,st) 0
    # number of configured vhdl wave viewers, and default one
    set_ne sim(vhdlwave,n) 1
    set_ne sim(vhdlwave,default) 0
  }


  if { [file exists ${USER_CONF_DIR}/simrc] } {
    unset sim
    source ${USER_CONF_DIR}/simrc
    #puts "sourcing simrc"
  }
} 

proc simconf_yview { args } {
  global simconf_vpos
  # puts "simconf_yview: $args"
  set_ne simconf_vpos 0
  if {[lindex $args 0] eq {place}} {
    place .sim.topf.f.scrl -in .sim.topf.f -x 0 -y 0 -relwidth 1
    update
  } 
  set ht [winfo height .sim.topf.f]
  set hs [winfo height .sim.topf.f.scrl]
  # puts "ht=$ht hs=$hs"
  set frac [expr {double($ht)/$hs}]
  if { [lindex $args 0] eq {scroll}} {
    set simconf_vpos [expr $simconf_vpos + [lindex $args 1] *(1.0/$frac)/5]
  } elseif { [lindex $args 0] eq {moveto}} {
    set simconf_vpos [lindex $args 1]
  }
  if { $simconf_vpos < 0.0 } { set simconf_vpos 0.0}
  if { $simconf_vpos > 1.0-$frac } { set simconf_vpos [expr 1.0 - $frac]}
  .sim.topf.vs set $simconf_vpos [expr $simconf_vpos + $frac]
  place .sim.topf.f.scrl -in .sim.topf.f -x 0 -y [expr -$hs * $simconf_vpos] -relwidth 1
}

proc simconf {} {
  global sim USER_CONF_DIR simconf_default_geometry

  catch { destroy .sim } 
  set_sim_defaults
  toplevel .sim -class dialog
  wm geometry .sim 700x340
  frame .sim.topf
  frame .sim.topf.f
  frame .sim.topf.f.scrl 
  scrollbar .sim.topf.vs -command {simconf_yview}
  pack .sim.topf.f -fill both -expand yes -side left
  pack .sim.topf.vs -fill y -expand yes
  frame .sim.topf.f.scrl.top
  frame .sim.topf.f.scrl.center
  frame .sim.bottom
  pack .sim.topf.f.scrl.top -fill x 
  pack .sim.topf.f.scrl.center -fill both -expand yes
  set bg(0) {#44eeee}
  set bg(1) {#44aaaa}
  set toggle 0
  foreach tool $sim(tool_list) {
    frame .sim.topf.f.scrl.center.$tool
    label .sim.topf.f.scrl.center.$tool.l -width 12 -text $tool  -bg $bg($toggle)
    frame .sim.topf.f.scrl.center.$tool.r
    pack .sim.topf.f.scrl.center.$tool -fill both -expand yes
    pack .sim.topf.f.scrl.center.$tool.l -fill y -side left
    pack .sim.topf.f.scrl.center.$tool.r -fill both -expand yes
    for {set i 0} { $i < $sim($tool,n)} {incr i} {
      frame .sim.topf.f.scrl.center.$tool.r.$i
      pack .sim.topf.f.scrl.center.$tool.r.$i -fill x -expand yes
      entry .sim.topf.f.scrl.center.$tool.r.$i.lab -textvariable sim($tool,$i,name) -width 15 -bg $bg($toggle)
      radiobutton .sim.topf.f.scrl.center.$tool.r.$i.radio -bg $bg($toggle) \
         -variable sim($tool,default) -value $i
      text .sim.topf.f.scrl.center.$tool.r.$i.cmd -width 20 -height 3 -wrap none -bg $bg($toggle)
      .sim.topf.f.scrl.center.$tool.r.$i.cmd insert 1.0 $sim($tool,$i,cmd)
      checkbutton .sim.topf.f.scrl.center.$tool.r.$i.fg -text Fg -variable sim($tool,$i,fg) -bg $bg($toggle)
      checkbutton .sim.topf.f.scrl.center.$tool.r.$i.st -text Status -variable sim($tool,$i,st) -bg $bg($toggle)

      pack .sim.topf.f.scrl.center.$tool.r.$i.lab -side left -fill y 
      pack .sim.topf.f.scrl.center.$tool.r.$i.radio -side left -fill y 
      pack .sim.topf.f.scrl.center.$tool.r.$i.cmd -side left -fill x -expand yes
      pack .sim.topf.f.scrl.center.$tool.r.$i.fg -side left -fill y 
      pack .sim.topf.f.scrl.center.$tool.r.$i.st -side left -fill y 
    }
    incr toggle
    set toggle [expr {$toggle %2}]
  }
  button .sim.bottom.cancel  -text Cancel -command {destroy .sim}
  button .sim.bottom.help  -text Help -command {
    set h {The following variables are defined and will get substituted by
XSCHEM before sending commands to the shell:

 - N: complete filename of netlist for current netlisting mode
   (example: /home/schippes/.xschem/simulations/opamp.spice for spice)
   (example: /home/schippes/.xschem/simulations/opamp.v for verilog)
 - n: complete filename of netlist as above but without extension
   (example: /home/schippes/.xschem/simulations/opamp)
 - s: name of schematic being used (example: opamp)
 - d: simulation directory (example: /home/schippes/.xschem/simulations)
 - terminal: terminal to be used for applications that need to be
   executed in terminal (example: $terminal -e ngspice -i "$N" -a)
If for a given tool there are multiple rows then the radiobutton
tells which one will be called by xschem.
Variables should be used with the usual substitution character $: $n, $N, etc.
Foreground checkbutton tells xschem to wait for child process to finish.
Status checkbutton tells xschem to report a status dialog (stdout, stderr,
exit status) when process finishes.
This is useful for tasks that execute quickly and for which xschem may display
the result / error reporting.
Any changes made in the command or tool name entries will be saved in 
~/.xschem/simrc when 'Save Configuration' button is pressed.
If no ~/.xschem/simrc is present then a bare minumum skeleton setup is presented.
To reset to default just delete the ~/.xschem/simrc file manually.
    }
    viewdata $h ro
  }
  button .sim.bottom.ok  -text {Save Configuration} -command {
    foreach tool $sim(tool_list) {
      for {set i 0} { $i < $sim($tool,n)} {incr i} {
        set sim($tool,$i,cmd) [.sim.topf.f.scrl.center.$tool.r.$i.cmd get 1.0 {end - 1 chars}]
      }
    }
    # destroy .sim
    save_sim_defaults ${USER_CONF_DIR}/simrc
    # puts "saving simrc"
  }
  button .sim.bottom.close -text Close -command {
    destroy .sim
  }
  pack .sim.bottom.cancel -side left -anchor w
  pack .sim.bottom.help -side left
  #foreach tool $sim(tool_list) {
  #  button .sim.bottom.add${tool} -text +${tool} -command "
  #    simconf_add $tool
  #    destroy .sim
  #    save_sim_defaults ${USER_CONF_DIR}/simrc
  ##    simconf
  #  "
  #  pack .sim.bottom.add${tool} -side left
  #}
  pack .sim.bottom.ok -side right -anchor e
  pack .sim.bottom.close -side right
  pack .sim.topf -fill both -expand yes
  pack .sim.bottom -fill x
  if { [info exists simconf_default_geometry]} {
     wm geometry .sim "${simconf_default_geometry}"
  }
 
  bind .sim.topf.f <Configure> {simconf_yview}
  bind .sim <Configure> {
    set simconf_default_geometry [wm geometry .sim]
  }
  bind .sim <ButtonPress-4> { simconf_yview scroll -0.2}
  bind .sim <ButtonPress-5> { simconf_yview scroll 0.2}
  simconf_yview place
  set maxsize [expr [winfo height .sim.topf.f.scrl] + [winfo height .sim.bottom]]
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

proc simulate {{callback {}}} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp)
  ## $d : netlist directory

  global netlist_dir netlist_type computerfarm terminal current_dirname sim
  global execute_callback
  set_sim_defaults
  
  if { [select_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool $netlist_type
    set s [file tail [file rootname [xschem get schname]]]
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst -nocommands $sim($tool,$def,cmd)]
   
    set id [$fg $st sh -c "cd $netlist_dir; $cmd"]
    set execute_callback($id) $callback
  }
}


proc waves {} { 
  ## $N : netlist file full path (/home/schippes/simulations/opamp.spice) 
  ## $n : netlist file full path with extension chopped (/home/schippes/simulations/opamp)
  ## $s : schematic name (opamp)
  ## $d : netlist directory

  global netlist_dir netlist_type computerfarm terminal current_dirname sim

  set_sim_defaults
  
  if { [select_netlist_dir 0] ne {}} {
    set d ${netlist_dir}
    set tool ${netlist_type}
    set s [file tail [file rootname [xschem get schname]]]
    set n ${netlist_dir}/${s}
    if {$tool eq {verilog}} {
      set N ${n}.v
    } else {
      set N ${n}.${tool}
    }

    set tool ${tool}wave
    set def $sim($tool,default)
    set fg  $sim($tool,$def,fg)
    set st  $sim($tool,$def,st)
    if {$fg} {
      set fg {execute_wait}
    } else {
      set fg {execute}
    }
    set cmd [subst -nocommands $sim($tool,$def,cmd)]
   
    $fg $st sh -c "cd $netlist_dir; $cmd"
  }
}
# ============================================================

proc utile_translate {schname} { 
  global netlist_dir netlist_type tcl_debug XSCHEM_SHAREDIR
  global utile_gui_path utile_cmd_path
  set tmpname [file rootname "$schname"]
  eval exec {sh -c "cd \"$netlist_dir\"; XSCHEM_SHAREDIR=\"$XSCHEM_SHAREDIR\" \"$utile_cmd_path\" stimuli.$tmpname"}
}

proc utile_gui {schname} { 
  global netlist_dir netlist_type tcl_debug XSCHEM_SHAREDIR
  global utile_gui_path utile_cmd_path
  set tmpname [file rootname "$schname"]
  eval exec {sh -c "cd \"$netlist_dir\"; XSCHEM_SHAREDIR=\"$XSCHEM_SHAREDIR\" \"$utile_gui_path\" stimuli.$tmpname"} &
}

proc utile_edit {schname} { 
  global netlist_dir netlist_type tcl_debug editor XSCHEM_SHAREDIR
  global utile_gui_path utile_cmd_path 
  set tmpname [file rootname "$schname"]
  eval exec {sh -c "cd \"$netlist_dir\"; $editor stimuli.$tmpname ; cd \"$netlist_dir\"; XSCHEM_SHAREDIR=\"$XSCHEM_SHAREDIR\" \"$utile_cmd_path\" stimuli.$tmpname"} &
}

proc get_shell { curpath } {
 global netlist_dir netlist_type tcl_debug
 global  terminal

 execute 0 sh -c "cd $curpath; $terminal"
}

proc edit_netlist {schname } {
 global netlist_dir netlist_type tcl_debug
 global editor terminal
 set tmpname [file rootname "$schname"]

 if { [regexp vim $editor] } { set ftype "-c \":set filetype=$netlist_type\"" } else { set ftype {} }
 if { [select_netlist_dir 0] ne "" } {
   # puts "edit_netlist: \"$editor $ftype  ${schname}.v\" \"$netlist_dir\" bg"
   if { $netlist_type=="verilog" } {
     execute 0  sh -c "cd $netlist_dir; $editor $ftype  \"${tmpname}.v\""
   } elseif { $netlist_type=="spice" } {
     execute 0  sh -c "cd $netlist_dir; $editor $ftype \"${tmpname}.spice\""
   } elseif { $netlist_type=="tedax" } {
     execute 0 sh -c "cd $netlist_dir; $editor $ftype \"${tmpname}.tdx\""
   } elseif { $netlist_type=="vhdl" } { 
     execute 0 sh -c "cd $netlist_dir; $editor $ftype \"${tmpname}.vhdl\""
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
proc save_file_dialog { msg ext global_initdir {initialfile {}} {overwrt 1} } {
  global tcl_version
  upvar #0 $global_initdir initdir
  if { $initialfile ne {}} {
    set initialdir [file dirname $initialfile]
    set initialfile [file tail $initialfile]
  } else {
    set initialdir $initdir
    set initialfile {}
  }
  set types(.sym) { {{All Files} * } {{Symbol files} {.sym}} }
  set types(.sch) { {{All Files} * } {{Schematic files} {.sch}} }
  set types(.sch.sym) { {{All Files} * } {{Schematic files} {.sch}} {{Symbol files} {.sym}} }
  set types(.sym.sch) { {{All Files} * } {{Symbol files} {.sym}} {{Schematic files} {.sch}} }
  if {$tcl_version > 8.5} {
    set r [tk_getSaveFile -title $msg -initialfile $initialfile -filetypes $types($ext) -initialdir $initialdir -confirmoverwrite $overwrt]
  } else {
    set r [tk_getSaveFile -title $msg -initialfile $initialfile -filetypes $types($ext) -initialdir $initialdir]
  }
  set dir [file dirname $r]
  # 20181011 no change initdir if operation cancelled by user
  if { $r ne {} } { set initdir $dir }
  return $r
}

proc is_xschem_file {f} {
  set fd [open $f r]
  set a [catch "open $f r" fd]
  set ret 0
  set score 0
  if {$a} {
    puts stderr "Can not open file $f"
  } else {
    while { [gets $fd line] >=0 } {
      if { [regexp {^[TGVSE] \{} $line] } { incr score }
      if { [regexp {^[BL] +[0-9]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } { incr score }
      if { [regexp {^N +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +[-0-9.eE]+ +\{} $line] } { incr score }
      if { [regexp {^C +\{[^{}]+\} +[-0-9.eE]+ +[-0-9.eE]+ +[0-3]+ +[0-3]+ +\{} $line] } { incr score }
      if { [regexp "^v\[ \t\]+\{xschem\[ \t\]+version\[ \t\]*=.*\[ \t\]+file_version\[ \t\]*=" $line] } {
        set ret 1
      }
    } 
    if { $score > 6 }  { set ret 1} ;# Heuristic decision :-)
    close $fd
  }
  # puts "score=$score"
  return $ret
}

proc list_dirs {pathlist } {
  global list_dirs_selected_dir INITIALINSTDIR
  toplevel .list -class dialog
  wm title .list {Select Library:}
  wm protocol .list WM_DELETE_WINDOW { set list_dirs_selected_dir {} } 
  set X [expr {[winfo pointerx .list] - 30}]
  set Y [expr {[winfo pointery .list] - 25}]
  if { $::wm_fix } { tkwait visibility .list }
  wm geometry .list "+$X+$Y"

  set x 0
  set dir {}
  label .list.title \
      -text "Choose path to start from. You can navigate anywhere\n with the file selector from there \n" \
      -background {#77dddd}
  pack .list.title -fill x -side top
  foreach elem $pathlist {
    frame .list.${x}
    label .list.${x}.l -text [expr {$x+1}] -width 4
    button .list.${x}.b -text $elem -command "set list_dirs_selected_dir $elem"
    pack .list.${x}.l -side left
    pack .list.${x}.b -side left -fill x -expand yes
    pack .list.${x} -side top -fill x 
    incr x
  }
  frame .list.${x}
  label .list.${x}.l -text [expr {$x+1}] -width 4
  button .list.${x}.b -text {Last used dir} -command "set list_dirs_selected_dir $INITIALINSTDIR"
  pack .list.${x}.l -side left
  pack .list.${x}.b -side left -fill x -expand yes
  pack .list.${x} -side top -fill x 
  frame .list.but
  button .list.but.cancel -text Cancel -command {set list_dirs_selected_dir {} }
  
  pack .list.but.cancel -side bottom
  pack .list.but -fill x -side top
  vwait list_dirs_selected_dir
  destroy .list
  return $list_dirs_selected_dir
}

proc myload_set_colors {} {
  global myload_index1 myload_files2
  #### update
  set dir1 [abs_sym_path [.myload.l.paneleft.list get $myload_index1]]
  for {set i 0} { $i< [.myload.l.paneright.list index end] } { incr i} {
    if {[ file isdirectory "$dir1/[lindex $myload_files2 $i]"]} {
      .myload.l.paneright.list itemconfigure $i -foreground blue
    } else {
      .myload.l.paneright.list itemconfigure $i -foreground black
    }
  }
}
proc myload_set_home {dir} {
  global pathlist  myload_files1 myload_index1 current_dirname

  if { $dir eq {.}} { set dir $current_dirname}
  # puts "set home: dir=$dir, pathlist=$pathlist"
  set pl {}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]  && [info exist current_dirname]} {
      set path_elem $current_dirname
    }
    lappend pl $path_elem
  }
  set i [lsearch -exact $pl $dir]
  if { $i>=0 } {
    set myload_files1 $pathlist
    update
    .myload.l.paneleft.list xview moveto 1
    set myload_index1 $i
    .myload.l.paneleft.list selection set $myload_index1
  } else {
    set myload_files1 [list $dir]
    update
    .myload.l.paneleft.list xview moveto 1
    set myload_index1 0
    .myload.l.paneleft.list selection set 0
  }
}

proc load_file_dialog {{msg {}} {ext {}} {global_initdir {INITIALINSTDIR}}} {
  global myload_index1 myload_files2 myload_files1 myload_retval myload_dir1 pathlist
  global myload_default_geometry myload_sash_pos myload_yview tcl_version
  upvar #0 $global_initdir initdir
  toplevel .myload -class dialog
  wm title .myload $msg
  set_ne myload_index1 0
  if { ![info exists myload_files1]} {
    set myload_files1 $pathlist
    set myload_index1 0
  }
  set_ne myload_files2 {}

  # return value
  set myload_retval {} 

  # set files [lsort [glob -directory . -tails \{.*,*\}]]
  
  panedwindow  .myload.l -orient horizontal

  frame .myload.l.paneleft
  if {$tcl_version > 8.5} { set just {-justify right}} else {set just {}}
  eval [subst {listbox .myload.l.paneleft.list -listvariable myload_files1 -width 20 -height 18 $just \
    -yscrollcommand ".myload.l.paneleft.yscroll set" -selectmode browse \
    -xscrollcommand ".myload.l.paneleft.xscroll set"}]
  scrollbar .myload.l.paneleft.yscroll -command ".myload.l.paneleft.list yview" 
  scrollbar .myload.l.paneleft.xscroll -command ".myload.l.paneleft.list xview" -orient horiz
  pack  .myload.l.paneleft.yscroll -side right -fill y
  pack  .myload.l.paneleft.xscroll -side bottom -fill x
  pack  .myload.l.paneleft.list -fill both -expand true
  bind .myload.l.paneleft.list <<ListboxSelect>> { 
    # bind .myload.l.paneright.pre <Expose> {}
    # .myload.l.paneright.pre configure -background white
    set sel [.myload.l.paneleft.list curselection]
    if { $sel ne {} } {
      set myload_dir1 [abs_sym_path [.myload.l.paneleft.list get $sel]]
      set myload_index1 $sel
      set myload_files2 [lsort [glob -directory $myload_dir1 -tails \{.*,*\}]]
      myload_set_colors
    }
  }

  frame .myload.l.paneright
  frame .myload.l.paneright.pre -background white -width 200 -height 200
  listbox .myload.l.paneright.list  -listvariable myload_files2 -width 20 -height 18\
    -yscrollcommand ".myload.l.paneright.yscroll set" -selectmode browse \
    -xscrollcommand ".myload.l.paneright.xscroll set"
  scrollbar .myload.l.paneright.yscroll -command ".myload.l.paneright.list yview"
  scrollbar .myload.l.paneright.xscroll -command ".myload.l.paneright.list xview" -orient horiz
  pack .myload.l.paneright.pre -side bottom -anchor s -fill x 
  pack  .myload.l.paneright.yscroll -side right -fill y
  pack  .myload.l.paneright.xscroll -side bottom -fill x
  pack  .myload.l.paneright.list -side bottom  -fill both -expand true

  .myload.l  add .myload.l.paneleft -minsize 40
  .myload.l  add .myload.l.paneright -minsize 40
  # .myload.l paneconfigure .myload.l.paneleft -stretch always
  # .myload.l paneconfigure .myload.l.paneright -stretch always
  frame .myload.buttons 
  button .myload.buttons.ok -text OK -command { set myload_retval [.myload.buttons.entry get]; destroy .myload} 
  button .myload.buttons.cancel -text Cancel -command {set myload_retval {}; destroy .myload}
  button .myload.buttons.home -text Home -command {
    bind .myload.l.paneright.pre <Expose> {}
    .myload.l.paneright.pre configure -background white
    set myload_files1 $pathlist
    update
    .myload.l.paneleft.list xview moveto 1
    set myload_index1 0
    set myload_dir1 [abs_sym_path [.myload.l.paneleft.list get $myload_index1]]
    set myload_files2 [lsort [glob -directory $myload_dir1 -tails \{.*,*\}]]
    myload_set_colors
    .myload.buttons.entry delete 0 end
    .myload.l.paneleft.list selection set $myload_index1
  }
  label .myload.buttons.label  -text {File:}
  entry .myload.buttons.entry
  button .myload.buttons.up -text UP -command {
    bind .myload.l.paneright.pre <Expose> {}
    .myload.l.paneright.pre configure -background white
    set d [file dirname $myload_dir1]
    if { [file isdirectory $d]} {
      myload_set_home $d
      set myload_files2 [lsort [glob -directory $d -tails \{.*,*\}]]
      myload_set_colors
      set myload_dir1 $d
      .myload.buttons.entry delete 0 end
    }
  }
  pack .myload.buttons.ok .myload.buttons.up .myload.buttons.cancel \
       .myload.buttons.home .myload.buttons.label -side left
  pack .myload.buttons.entry -side left -fill x -expand true
  pack .myload.l -expand true -fill both
  pack .myload.buttons -side top -fill x
  if { [info exists myload_default_geometry]} {
     wm geometry .myload "${myload_default_geometry}"
  }
  myload_set_home $initdir
  bind .myload <Return> { 
    set myload_retval [.myload.buttons.entry get]
    if {$myload_retval ne {} } {
      destroy .myload
    }
  }
  bind .myload.l.paneright.list <Double-Button-1> {
    set myload_retval [.myload.buttons.entry get]
    if {$myload_retval ne {} } {
      destroy .myload
    }
  }
  bind .myload <Escape> { set myload_retval {}; destroy .myload}

  update
  if { [ info exists myload_sash_pos] } {
    eval .myload.l sash mark 0 [.myload.l sash coord 0]
    eval .myload.l sash dragto 0 [subst $myload_sash_pos]
  }
  update
  if { [ info exists myload_yview]} {
   .myload.l.paneright.list yview moveto  [lindex $myload_yview 0]
  }
  .myload.l.paneleft.list xview moveto 1
  bind .myload <Configure> {
    set myload_sash_pos [.myload.l sash coord 0]
    set myload_default_geometry [wm geometry .myload]
    .myload.l.paneleft.list xview moveto 1
    # regsub {\+.*} $myload_default_geometry {} myload_default_geometry
  }

  bind .myload.l.paneright.yscroll <Motion> {
    set myload_yview [.myload.l.paneright.list yview]
  }

  xschem preview_window create .myload.l.paneright.pre {}
  set myload_dir1 [abs_sym_path [.myload.l.paneleft.list get $myload_index1]]
  set myload_files2 [lsort [glob -directory $myload_dir1 -tails \{.*,*\}]]
  myload_set_colors

  bind .myload.l.paneright.list <ButtonPress> { 
    set myload_yview [.myload.l.paneright.list yview]
  }
  bind .myload.l.paneright.list <<ListboxSelect>> {
    set myload_yview [.myload.l.paneright.list yview] 
    set sel [.myload.l.paneright.list curselection]
    if { $sel ne {} } {
      set myload_dir1 [abs_sym_path [.myload.l.paneleft.list get $myload_index1]]
      set dir2 [.myload.l.paneright.list get $sel]
      if {$dir2 eq {..}} {
        set d [file dirname $myload_dir1]
      } elseif { $dir2 eq {.} } {
        set d  $myload_dir1
      } else {
        set d "$myload_dir1/$dir2"
      }
      if { [file isdirectory $d]} {
        bind .myload.l.paneright.pre <Expose> {}
        .myload.l.paneright.pre configure -background white
        myload_set_home $d
        set myload_files2 [lsort [glob -directory $d -tails \{.*,*\}]]
        myload_set_colors
        set myload_dir1 $d
        .myload.buttons.entry delete 0 end
      } else {
        .myload.buttons.entry delete 0 end
        .myload.buttons.entry insert 0 $dir2
         if { [is_xschem_file $myload_dir1/$dir2] } {
           .myload.l.paneright.pre configure -background {}
	   update
           xschem preview_window draw .myload.l.paneright.pre "$myload_dir1/$dir2"
           bind .myload.l.paneright.pre <Expose> {xschem preview_window draw .myload.l.paneright.pre "$myload_dir1/$dir2"}
         } else {
           bind .myload.l.paneright.pre <Expose> {}
           .myload.l.paneright.pre configure -background white
         }
         # puts "xschem preview_window draw .myload.l.paneright.pre \"$myload_dir1/$dir2\""
      }
    }
  }
  tkwait window .myload
  xschem preview_window destroy {} {} 
  set initdir "$myload_dir1"
  if { $myload_retval ne {} } {
    return "$myload_dir1/$myload_retval"
  } else {
    return {}
  }
}


# used in scheduler.c  20121111
# get last 2 path components: example /aaa/bbb/ccc/ddd.sch -> ccc/ddd
# optionally with extension if present and $ext==1
proc get_cell {s {ext 1} } {
  set slist [file split $s]
  if { [llength $slist] >1 } {
    if {$ext} {
      return [lindex $slist end-1]/[lindex $slist end]
    } else {
      return [lindex $slist end-1]/[file rootname [lindex $slist end]]
    }
  } else {
    if {$ext} {
      return [lindex $slist end]
    } else {
      return [file rootname [lindex $slist end]]
    }
  }
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
  global filetmp1 filetmp2

  set retval [ read_data_nonewline $filetmp2 ]
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
      puts $fd "C \{[rel_sym_path devices/[lindex $i 1].sym]\} 0 [set y [expr $y-20]]  0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
    } else {
      puts $fd "C \{[rel_sym_path [lindex $i 1].sym]\} 0 [set y [expr $y-20]]  0 0 \{ name=p[incr pcnt] lab=[lindex $i 0] \}"
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
  xschem copy
  set clipboard [read_data_nonewline $USER_CONF_DIR/.clipboard.sch]
  set lines [split $clipboard \n]
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {[regexp {^C \{.*(i|o|io)pin} $i ]} {
      if {[regexp {ipin} [lindex $i 1]]} { set dir in }
      if {[regexp {opin} [lindex $i 1]]} { set dir out }
      if {[regexp {iopin} [lindex $i 1]]} { set dir inout }
      set rot [lindex $i 4]
      set flip [lindex $i 5]
      regsub {^.*lab=} $i {} lab
      regsub {[\} ].*} $lab {} lab
      set x0 [lindex $i 2]
      set y0 [lindex $i 3]
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


# 20120913
proc add_lab_no_prefix {} { 
  global env retval USER_CONF_DIR
  global filetmp1 filetmp2

  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  set retval [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {$indirect} {
      puts $fd "C \{devices/lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
    } else {
      puts $fd "C \{lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=wire lab=[lindex $i 0] \}"
    }
  }
  close $fd
  xschem merge $USER_CONF_DIR/.clipboard.sch
}


proc add_lab_prefix {} {
  global env retval USER_CONF_DIR
  global filetmp1 filetmp2

  if { [file exists [abs_sym_path devices/ipin.sym]] } {
    set indirect 1
  } else {
    set indirect 0
  }
  set retval [ read_data_nonewline $filetmp2 ]
  regsub -all {<} $retval {[} retval
  regsub -all {>} $retval {]} retval
  set lines [split $retval \n]
  # viewdata $retval
  set pcnt 0
  set y 0
  set fd [open $USER_CONF_DIR/.clipboard.sch "w"]
  foreach i $lines {
    if {$indirect} {
      puts $fd "C \{devices/lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
    } else {
      puts $fd "C \{lab_pin.sym\} 0 [set y [expr $y+20]]  0 0 \{ name=p[incr pcnt] verilog_type=reg lab=i[lindex $i 0] \}"
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

proc select_netlist_dir { force {dir {} }} {
   global netlist_dir env

   if { ( $force == 0 )  && ( $netlist_dir ne {} ) } {
     if {![file exist $netlist_dir]} {
       file mkdir $netlist_dir
     }
     regsub {^~/} $netlist_dir ${env(HOME)}/ netlist_dir
     xschem set_netlist_dir $netlist_dir
     return $netlist_dir
   } 
   if { $dir eq {} } {
     if { $netlist_dir ne {} }  { 
       set initdir $netlist_dir
     } else {
       set initdir  $env(PWD) 
     }
     # 20140409 do not change netlist_dir if user Cancels action
     set new_dir [tk_chooseDirectory -initialdir $initdir -parent . -title {Select netlist DIR} -mustexist false]
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
   xschem set_netlist_dir $netlist_dir
   return $netlist_dir
}

proc enter_text {textlabel} {
   global txt rcode has_cairo
   set rcode {}
   toplevel .t -class Dialog
   wm title .t {Enter text}

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
       if {$has_cairo } {
         entry .t.edit.entries.hsize -relief sunken -textvariable vsize -width 20
       } else {
         entry .t.edit.entries.hsize -relief sunken -textvariable hsize -width 20
       }
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
    if {$has_cairo} { 
      set hsize $vsize
    }
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
   bind .t <Escape> {
     if ![string compare $txt [.t.txt get 1.0 {end - 1 chars}]] {
       .t.buttons.cancel invoke
     }
   }
   bind .t <Control-Return> {.t.buttons.ok invoke}
   #grab set .t
   tkwait window .t
   return $txt
}

# evaluate a tcl command from GUI
proc tclcmd {} {
  global tclcmd_txt
  catch {destroy .tclcmd}
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
         set ind_bg black
       } else {
         set layfg black
         set ind_bg white
       }
     } elseif { $j == [xschem get gridlayer] } { ;# 20161206
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
     if { [expr $j%10] == 0 } {
       incr f
       frame .sl.f0.f$f
       pack .sl.f0.f$f -side left  -fill y
     }
   }
   tkwait window .sl
}

proc color_dim {} {
  toplevel .dim -class dialog
  wm title .dim {Dim colors}
  scale .dim.scale -digits 2 -label {Dim factor} -length 256 \
     -showvalue 1 -command {xschem color_dim} -orient horizontal \
     -from -5 -to 5 -resolution 0.1
  button .dim.ok -text OK -command {destroy .dim}
  .dim.scale set [xschem get dim]
  pack .dim.scale
  pack .dim.ok
}
proc about {} {

  if [winfo exists .about] { 
    bind .about.link <Button-1> {}
    bind .about.link2 <Button-1> {}
    destroy .about
  }
  toplevel .about -class dialog
  wm title .about {About XSCHEM}
  label .about.xschem -text "[xschem get version]" -font {Sans 24 bold}
  label .about.descr -text "Schematic editor / netlister for VHDL, Verilog, SPICE, tEDAx"
  button .about.link -text "http://repo.hu/projects/xschem" -font Underline-Font -fg blue -relief flat
  button .about.link2 -text "http://repo.hu/projects/coraleda" -font Underline-Font -fg blue -relief flat
  label .about.copyright -text "\n Copyright 1998-2019 Stefan Schippers (stefan.schippers@gmail.com) \n
 This is free software; see the source for copying conditions.  There is NO warranty;
 not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\n"
  button .about.close -text Close -command {destroy .about} -font {Sans 18}
  pack .about.xschem
  pack .about.link
  pack .about.link2
  pack .about.descr
  pack .about.copyright
  pack .about.close
  bind .about.link <Button-1> { execute 0  xdg-open http://repo.hu/projects/xschem}
  bind .about.link2 <Button-1> { execute 0  xdg-open http://repo.hu/projects/coraleda}
}

proc property_search {} {
  global search_value
  global search_exact
  global search_select
  global custom_token

  toplevel .lw -class Dialog
  wm title .lw {Search}
  bind .lw <Visibility> {
    if { [wm stackorder .lw isbelow . ]} {
      raise .lw .drw 
    }
  }
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
        if { $search_exact==1 } { xschem search exact $search_select $token $search_value
        } else { xschem search regex $search_select $token $search_value }
        destroy .lw 
  }
  button .lw.but.cancel -text Cancel -command { destroy .lw }
  checkbutton .lw.but.sub -text Exact_search -variable search_exact
  radiobutton .lw.but.nosel -text {Highlight} -variable search_select -value 0
  radiobutton .lw.but.sel -text {Select} -variable search_select -value 1
  # 20171211 added unselect
  radiobutton .lw.but.unsel -text {Unselect} -variable search_select -value -1
  pack .lw.but.ok  -anchor w -side left
  pack .lw.but.sub  -side left
  pack .lw.but.nosel  -side left
  pack .lw.but.sel  -side left
  pack .lw.but.unsel  -side left
  pack .lw.but.cancel -anchor e
  pack .lw.custom  -anchor e
  pack .lw.val  -anchor e
  pack .lw.but -expand yes -fill x
  focus  .lw
  bind .lw <Escape> {.lw.but.cancel invoke}
  bind .lw <Return> {.lw.but.ok invoke}
  bind .lw <Control-Return> {.lw.but.ok invoke}
  grab set .lw
  tkwait window .lw
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
  # puts ">>>> $s $instname $symname"
  regsub {^@tcleval\(} $s {} s
  regsub {\)$} $s {} s
  return [eval $s]
}

#20171005
proc attach_labels_to_inst {} {
  global use_lab_wire use_label_prefix custom_label_prefix rcode do_all_inst rotated_text

  toplevel .label -class Dialog
  set rcode {}
  wm title .label {Add labels to instances}
  bind .label <Visibility> {
    if {[wm stackorder .label isbelow . ]} {
      raise .label .drw 
    }
  }

  # 20100408
  set X [expr [winfo pointerx .label] - 60]
  set Y [expr [winfo pointery .label] - 35]
  wm geometry .label "+$X+$Y"

  frame .label.custom 
  label .label.custom.l -text "Prefix"
  entry .label.custom.e -width 32
  .label.custom.e insert 0 $custom_label_prefix
  pack .label.custom.e .label.custom.l -side right


  frame .label.but
  button .label.but.ok -text OK -command {
        set custom_label_prefix [.label.custom.e get]
        set token $custom_token
        #### put command here
        set rcode yes
        destroy .label 
  }
  button .label.but.cancel -text Cancel -command { set rcode {}; destroy .label }
  checkbutton .label.but.wire -text {use wire labels} -variable use_lab_wire
  checkbutton .label.but.do_all -text {Do all} -variable do_all_inst
  label .label.but.rot -text {Rotated Text}
  entry .label.but.rotated -textvariable rotated_text -width 2
  checkbutton .label.but.prefix -text {use prefix} -variable use_label_prefix
  pack .label.but.ok  -anchor w -side left
  pack .label.but.prefix  -side left
  pack .label.but.wire  -side left
  pack .label.but.do_all  -side left
  pack .label.but.rotated  -side left
  pack .label.but.rot  -side left
  pack .label.but.cancel -anchor e

  pack .label.custom  -anchor e
  pack .label.but -expand yes -fill x

  focus  .label
  bind .label <Escape> {.label.but.cancel invoke}
  bind .label <Return> {.label.but.ok invoke}
  bind .label <Control-Return> {.label.but.ok invoke}
  grab set .label
  tkwait window .label
  return {}
}

proc ask_save { {ask {save file?}} } {
   global rcode
   toplevel .ent2 -class Dialog
   wm title .ent2 {Ask Save}

   bind .ent2 <Visibility> {
     if {[wm stackorder .ent2 isbelow . ]} {
       raise .ent2 .drw 
     }
   }
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
   bind .ent2 <Escape> {.ent2.f1.b2 invoke}
   # needed, otherwise problems when descending with double clixk 23012004
   tkwait visibility .ent2
   grab set .ent2
   tkwait window .ent2
   return $rcode
}


proc edit_vi_prop {txtlabel} {
 global XSCHEM_TMP_DIR retval symbol prev_symbol rcode tcl_debug netlist_type editor

 # 20150914
 global user_wants_copy_cell
 set user_wants_copy_cell 0

 set rcode {}
 set filename .xschem_edit_file.[pid]
 if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
 set filename $filename.$suffix

 write_data $retval $XSCHEM_TMP_DIR/$filename
 eval execute_wait 0 $editor $XSCHEM_TMP_DIR/$filename ;# 20161119

 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\nretval=$retval\n---------\n"}
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\nsymbol=$symbol\n---------\n"}
 set tmp [read_data $XSCHEM_TMP_DIR/$filename]
 file delete $XSCHEM_TMP_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
 if [string compare $tmp $retval] {
        set retval $tmp
        if $tcl_debug<=-1 then {puts "modified"}
        set rcode ok
        return  $rcode
 } else {
        set rcode {}
        return $rcode
 }
}

proc edit_vi_netlist_prop {txtlabel} {
 global XSCHEM_TMP_DIR retval rcode tcl_debug netlist_type editor

 # 20150914
 global user_wants_copy_cell
 set user_wants_copy_cell 0

 set filename .xschem_edit_file.[pid]
 if ![string compare $netlist_type "vhdl"] { set suffix vhd } else { set suffix v }
 set filename $filename.$suffix
 
 regsub -all {\\?"} $retval {"} retval
 write_data $retval $XSCHEM_TMP_DIR/$filename
 if { [regexp vim $editor] } { set ftype "\{-c :set filetype=$netlist_type\}" } else { set ftype {} }
 eval execute_wait 0 $editor  $ftype $XSCHEM_TMP_DIR/$filename

 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$retval\n---------\n"}

 set tmp [read_data $XSCHEM_TMP_DIR/$filename] ;# 20161204 dont remove newline even if it is last char
 # set tmp [read_data_nonewline $XSCHEM_TMP_DIR/$filename]
 file delete $XSCHEM_TMP_DIR/$filename
 if $tcl_debug<=-1 then {puts "edit_vi_prop{}:\n--------\n$tmp\n---------\n"}
 if [string compare $tmp $retval] {
        set retval $tmp

        regsub -all {\\?"} $retval {\\"} retval
        set retval "\"${retval}\"" 
        if $tcl_debug<=-1 then {puts "modified"}
        set rcode ok
        return  $rcode
 } else {
        set rcode {}
        return $rcode
 }
}

proc change_color {} {
  global colors dark_colors light_colors dark_colorscheme cadlayers ctrl_down USER_CONF_DIR


  set n [xschem get rectcolor]
  if { $n < 0 || $n >=$cadlayers} return
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
    }
    xschem change_colors
    set savedata "#### THIS FILE IS AUTOMATICALLY GENERATED BY XSCHEM, DO NOT EDIT.\n"
    set savedata "$savedata set cadlayers $cadlayers\n"
    set savedata "$savedata set light_colors {$light_colors}\n"
    set savedata "$savedata set dark_colors {$dark_colors}\n"
    write_data $savedata ${USER_CONF_DIR}/colors
  }
}

proc edit_prop {txtlabel} {
   global edit_prop_default_geometry infowindow_text
   global prev_symbol retval symbol rcode no_change_attrs preserve_unchanged_attrs copy_cell tcl_debug editprop_semaphore
   global user_wants_copy_cell editprop_sympath
   set user_wants_copy_cell 0
   set rcode {}
   set editprop_semaphore 1
   if $tcl_debug<=-1 then {puts " edit_prop{}: retval=$retval"}
   toplevel .ent2  -class Dialog 
   wm title .ent2 {Edit Properties}
   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent2 }

   wm geometry .ent2 "${edit_prop_default_geometry}+$X+$Y"

   bind .ent2 <Visibility> { if { [wm stackorder .ent2 isbelow . ] } {raise .ent2 .drw}  }

   # 20160325 change and remember widget size
   bind .ent2 <Configure> { 
     # puts [wm geometry .ent2]
     set geom [wm geometry .ent2]
     regsub {\+.*} $geom {} geom
     set edit_prop_default_geometry $geom
   }

   set prev_symbol $symbol
   set editprop_sympath [file dirname [abs_sym_path $symbol]]
   frame .ent2.f4
   label .ent2.f4.l1  -text $txtlabel
   label .ent2.f4.path  -text "Path:"
   entry .ent2.f4.e1  -textvariable editprop_sympath -width 0 -state readonly
   text .ent2.e1   -yscrollcommand ".ent2.yscroll set" -setgrid 1 \
                   -xscrollcommand ".ent2.xscroll set" -wrap none
     .ent2.e1 delete 1.0 end
     .ent2.e1 insert 1.0 $retval

   scrollbar .ent2.yscroll -command  ".ent2.e1 yview"
   scrollbar .ent2.xscroll -command ".ent2.e1 xview" -orient horiz
   frame .ent2.f1
   frame .ent2.f2
   label .ent2.f1.l2 -text "Symbol"
   entry .ent2.f1.e2 -width 30
   .ent2.f1.e2 insert 0 $symbol
   button .ent2.f1.b5 -text "BROWSE" -command {
     bind .ent2 <Visibility> {}
     set r [save_file_dialog  {New symbol} .sym INITIALINSTDIR {} 0]
     if {$r ne {} } {
       .ent2.f1.e2 delete 0 end
       .ent2.f1.e2 insert 0 $r
     }
     raise .ent2 .drw
     bind .ent2 <Visibility> { if {[wm stackorder .ent2 isbelow . ]} {raise .ent2 .drw}  }
   }

   button .ent2.f1.b1 -text "OK" -command   {
     set retval [.ent2.e1 get 1.0 {end - 1 chars}] 
     set abssymbol [abs_sym_path [ .ent2.f1.e2 get]]
     set symbol [.ent2.f1.e2 get]
     set rcode {ok}
     set editprop_semaphore 0
     set user_wants_copy_cell $copy_cell
     set prev_symbol [abs_sym_path $prev_symbol]
     if { ($abssymbol ne $prev_symbol) && $copy_cell } {
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
       ## 20190326
     }
     #puts "symbol: $symbol , prev_symbol: $prev_symbol"
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
   checkbutton .ent2.f2.r1 -text "no change properties" -variable no_change_attrs -state normal
   checkbutton .ent2.f2.r2 -text "preserve unchanged props" -variable preserve_unchanged_attrs -state normal
   checkbutton .ent2.f2.r3 -text "copy cell" -variable copy_cell -state normal

   pack .ent2.f1.l2 .ent2.f1.e2 .ent2.f1.b1 .ent2.f1.b2 .ent2.f1.b3 .ent2.f1.b4 .ent2.f1.b5 -side left -expand 1
   pack .ent2.f4 -side top  -anchor nw
   #pack .ent2.f4.path .ent2.f4.e1 .ent2.f4.l1 -side left -fill x 
   pack .ent2.f4.path -side left
   pack .ent2.f4.e1 -side left 
   pack .ent2.f1 .ent2.f2 -side top -fill x 
   pack .ent2.f2.r1 -side left
   pack .ent2.f2.r2 -side left
   pack .ent2.f2.r3 -side left
   pack .ent2.yscroll -side right -fill y 
   pack .ent2.xscroll -side bottom -fill x
   pack .ent2.e1  -fill both -expand yes
   bind .ent2 <Control-Return> {.ent2.f1.b1 invoke}
   bind .ent2 <Escape> {
     if { ![string compare $retval [.ent2.e1 get 1.0 {end - 1 chars}]] && \
          ![string compare $symbol [ .ent2.f1.e2 get]] } {
       .ent2.f1.b2 invoke
     }
   }
   #tkwait visibility .ent2
   #grab set .ent2
   #focus .ent2.e1
   #tkwait window .ent2
   while {1} {
     tkwait  variable editprop_semaphore
     if { $editprop_semaphore == 2 } {
       set retval [.ent2.e1 get 1.0 {end - 1 chars}] 
       set symbol [ .ent2.f1.e2 get]
       xschem update_symbol ok 
       set editprop_semaphore 1
       xschem fill_symbol_editprop_form
       set editprop_sympath [file dirname [abs_sym_path $symbol]]
    
       # 20160423 no more setected stuff--> close
       if {$editprop_semaphore==0 } {
         break
       }
       # 20110325 update symbol variable after clicking another element to avoid 
       #          modified flag to be set even when nothing changed
       ## set symbol [ .ent2.f1.e2 get]

       .ent2.e1 delete 1.0 end
       .ent2.e1 insert 1.0 $retval
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
   global text_line_default_geometry preserve_unchanged_attrs
   global retval rcode tcl_debug
   if $clear==1 then {set retval ""}
   if $tcl_debug<=-1 then {puts " text_line{}: clear=$clear"}
   if $tcl_debug<=-1 then {puts " text_line{}: retval=$retval"}
   toplevel .ent2  -class Dialog
   wm title .ent2 {Text input}
   set X [expr [winfo pointerx .ent2] - 60]
   set Y [expr [winfo pointery .ent2] - 35]

   bind .ent2 <Visibility> { 
     if {[wm stackorder .ent2 isbelow . ]} {
       raise .ent2 .drw
     } 
   }
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

   frame .ent2.f0
   frame .ent2.f1
   label .ent2.f0.l1  -text $txtlabel

   text .ent2.e1 -relief sunken -bd 2 -yscrollcommand ".ent2.yscroll set" -setgrid 1 \
        -xscrollcommand ".ent2.xscroll set" -wrap none -width 90 -height 40
   scrollbar .ent2.yscroll -command  ".ent2.e1 yview"
   scrollbar .ent2.xscroll -command ".ent2.e1 xview" -orient horiz
   .ent2.e1 delete 1.0 end
   .ent2.e1 insert 1.0 $retval
   button .ent2.f1.b1 -text "OK" -command  \
   {
     set retval [.ent2.e1 get 1.0 {end - 1 chars}] 
     destroy .ent2
     set rcode {ok}
   }
   button .ent2.f1.b2 -text "Cancel" -command  \
   {
     set retval [.ent2.e1 get 1.0 {end - 1 chars}]
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
   checkbutton .ent2.f0.l2 -text "preserve unchanged props" -variable preserve_unchanged_attrs -state $preserve_disabled
   pack .ent2.f0 -fill x
   pack .ent2.f0.l2 -side left
   pack .ent2.f0.l1 -side left -expand yes
   pack .ent2.f1  -fill x
   pack .ent2.f1.b1 -side left -fill x -expand yes
   pack .ent2.f1.b2 -side left -fill x -expand yes
   pack .ent2.f1.b3 -side left -fill x -expand yes
   pack .ent2.f1.b4 -side left -fill x -expand yes
 

   pack .ent2.yscroll -side right -fill y 
   pack .ent2.xscroll -side bottom -fill x
   pack .ent2.e1   -expand yes -fill both
   bind .ent2 <Escape> {
     if ![string compare $retval [.ent2.e1 get 1.0 {end - 1 chars}]] {
       .ent2.f1.b2 invoke
     }
   }
   bind .ent2 <Control-Return> {.ent2.f1.b1 invoke}
   #tkwait visibility .ent2
   #grab set .ent2
   #focus .ent2.e1

   # 20100208
   set rcode {}   

   tkwait window .ent2
   return $rcode
}

proc alert_ {txtlabel {position +200+300}} {
   toplevel .ent3 -class Dialog
   wm title .ent3 {Alert}
   set X [expr [winfo pointerx .ent3] - 60]
   set Y [expr [winfo pointery .ent3] - 60]

   # 20100203
   if { $::wm_fix } { tkwait visibility .ent3 }
   bind .ent3 <Visibility> { 
     if { [wm stackorder .ent3 isbelow . ] } {raise .ent3 .drw}
   }
   if { [string compare $position ""] != 0 } {
     wm geometry .ent3 $position
   } else {
     wm geometry .ent3 "+$X+$Y"
   }
   
   label .ent3.l1  -text $txtlabel -wraplength 700
   button .ent3.b1 -text "OK" -command  \
   {
     destroy .ent3
   } 
   pack .ent3.l1 -side top -fill x
   pack .ent3.b1 -side top -fill x
   grab set .ent3
   focus .ent3.b1
   bind .ent3 <Return> { destroy .ent3 }
   bind .ent3 <Escape> { destroy .ent3 }

   tkwait window .ent3  
   return {}
}


proc infowindow {} {
 global infowindow_text

 set infotxt $infowindow_text
 if { $infowindow_text ne {}} {append infotxt \n}
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
  bind $z <Escape> {wm withdraw .infotext; set show_infowindow 0}
  return {}
}
proc textwindow {filename {ro {}}} {
   global textwindow_wcounter
   global textwindow_w
   set textwindow_w .win$textwindow_wcounter
   # catch {destroy $textwindow_w}
   set textwindow_wcounter [expr $textwindow_wcounter+1]
   set textwindow_w .win$textwindow_wcounter


   global fff
   global fileid
   set fff $filename
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
       set fileid [open $fff "w"]
       puts -nonewline $fileid [$textwindow_w.text get 1.0 {end - 1 chars}]
       close $fileid 
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
   set fileid [open $filename "r"]

   # 20171103 insert at insertion cursor(insert tag) instead of 0.0
   $textwindow_w.text insert insert [read $fileid]
   close $fileid
   return {}
}


proc viewdata {data {ro {}}} {
   global viewdata_wcounter  rcode
   global viewdata_w
   set viewdata_w .win$viewdata_wcounter
   # catch {destroy $viewdata_w}
   set viewdata_wcounter [expr $viewdata_wcounter+1]
   set rcode {}
   set viewdata_w .win$viewdata_wcounter
   toplevel $viewdata_w
   wm title $viewdata_w {Wiew data}
   frame $viewdata_w.buttons
   pack $viewdata_w.buttons -side bottom -fill x -pady 2m

   button $viewdata_w.buttons.dismiss -text Dismiss -command  "destroy $viewdata_w" 
   pack $viewdata_w.buttons.dismiss -side left -expand 1

   if { $ro eq {} } {
     button $viewdata_w.buttons.saveas -text {Save As} -command  {
       set fff [tk_getSaveFile -initialdir $env(PWD) ]
       if { $fff != "" } {
         set fileid [open $fff "w"]
         puts -nonewline $fileid [$viewdata_w.text get 1.0 {end - 1 chars}]
         close $fileid
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
  global pathlist current_dirname

  set lib_cell [get_cell $symbol]
  set cell [file tail $symbol]
  set name {}
  foreach path_elem $pathlist {
    if { ![string compare $path_elem .]  && [info exist current_dirname]} {
      set path_elem $current_dirname
    }
    # libname/symname[.ext] and libname in $path_elem 
    # --> libname/symname
    if { [file exists [file dirname "${path_elem}/${lib_cell}"]] && 
       (![string compare $symbol $lib_cell ]) } {
      set name ${lib_cell} ;# was lib_cell
    # /.../path/.../libname/cellname[.ext] and libname in $path_elem 
    # --> libname/cellname
    } elseif { (![string compare $symbol  "${path_elem}/${lib_cell}" ]) 
             && [file exists [file dirname "${path_elem}/${lib_cell}"]] } {
      set name ${lib_cell} ;# was lib_cell
    } elseif { (![string compare $symbol "${path_elem}/${cell}" ]) 
             && [file exists "${path_elem}/${cell}"] } {
      set name ${cell}
    } 
    if {$name ne {} } { break} 
  }
  if { ![string compare $name {} ] } {
    # no known lib, so return full path
    set name ${symbol}
  }
  return $name
}


# given a library/symbol return its absolute path
proc abs_sym_path {fname {ext {} } } {
  global pathlist current_dirname

  # add extension for 1.0 file format compatibility
  if { $ext ne {} } { 
    set fname [file rootname $fname]$ext
  }
  # transform ./file_or_path to file_or_path
  if { [regexp {^\.\./} $fname ] } {
    set fname [file normalize $fname]
  } elseif {[regexp {^\./} $fname ] } {
    regsub {^\./} $fname {} fname
  }
  set lib_cell [get_cell $fname]
  if {$fname eq {} } return {}
  set name {}
  # fname is of type libname/cellname[.ext] but not ./cellname[.ext] or
  # ../cellname[.ext] and has a slash, so no cellname[.ext] 
  # no ./cell.sym
  if {![string compare $fname $lib_cell ]} {
    foreach path_elem $pathlist {
      if { ![string compare $path_elem .]  && [info exist current_dirname]} {
        set path_elem $current_dirname
      }
      # libname/cellname and libname is in pathlist
      # --> $pathlist/libname/cellname
      # cellname and $pathlist/cellname exists
      # --> $pathlist/cellname
      if { [regexp {/} $fname] && [file exists "${path_elem}/${fname}"] } {
        set name  "$path_elem/$lib_cell"
        break
      }
      if { [file exists "${path_elem}/${fname}"] &&
        ![regexp {/} $fname] 
      } {
        set name  "$path_elem/$lib_cell"
        break
      }
    }
    # if no abs path, no existing items elsewhere, 
    # set name relative to $current_dirname
    if { ![string compare $name {}] } {
      set name "$current_dirname/$fname"
    }
  }
  if { ![string compare $name {}] } {
     set name $fname
  }
  regsub {/\.$} $name {} name
  return $name
}

proc add_ext {fname ext} {
#  if {![string match "*$ext" $fname]} {
#    set fname ${fname}$ext
#  }
#  return $fname
  return [file rootname $fname]$ext
}
 

proc input_number {txt cmd} {
          global xx
          toplevel .lw -class Dialog
          wm title .lw {Input number}
          set X [expr [winfo pointerx .lw] - 60]
          set Y [expr [winfo pointery .lw] - 35]
          # 20100203
          if { $::wm_fix } { tkwait visibility .lw }
          wm geometry .lw "+$X+$Y"

          bind .lw <Visibility> {
            if { [wm stackorder .lw isbelow . ]} {
              raise .lw .drw 
            }
          } 

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
          bind .lw <Escape> {.lw.f2.cancel invoke}
          grab set .lw
          focus .lw
          tkwait window .lw
}

## 20161102
proc launcher {} {
  global launcher_var launcher_default_program launcher_program env
  
  ## puts ">>> $launcher_program $launcher_var "
  # 20170413
  if { ![string compare $launcher_program {}] } { set launcher_program $launcher_default_program}

  eval exec  [subst $launcher_program] {[subst $launcher_var]} &
}

proc reconfigure_layers_button {} {
   global colors dark_colorscheme
   set c [xschem get rectcolor]
   .menubar.layers configure -background [lindex $colors $c]
   if { $dark_colorscheme == 1 && $c == 0} {
     .menubar.layers configure -foreground white
   } else {
     .menubar.layers configure -foreground black
   }
}

proc reconfigure_layers_menu {} {
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
     .menubar.layers.menu entryconfigure $j -activebackground $i \
        -background $i -foreground $layfg -activeforeground $layfg
     incr j
   }
   reconfigure_layers_button
}

proc get_file_path {ff} {
  global env
  if { [regexp {/} $ff] } { return $ff } 
  set pathlist [split $env(PATH) :]
  foreach i $pathlist {
    set ii $i/$ff
    if { [file exists $ii]} {return $ii}
  }
  return $ff
}

### 
###   MAIN PROGRAM
###


# tcl variable XSCHEM_LIBRARY_PATH  should already be set in xschemrc
set pathlist {}
if { [info exists XSCHEM_LIBRARY_PATH] } {
  set pathlist_orig [split $XSCHEM_LIBRARY_PATH :]
  foreach i $pathlist_orig {
    regsub {^~} $i $env(HOME) i
    if { ![string compare $i .] } {
      lappend pathlist $i
    } elseif { [ file exists $i] } {
      lappend pathlist [file normalize ${i}]
    }
  }
}

if { [xschem get help ]} {
  set fd [open "${XSCHEM_SHAREDIR}/xschem.help" r]
  set helpfile [read $fd]
  puts $helpfile
  close $fd
  exit
}

set_ne XSCHEM_TMP_DIR {/tmp}

# used in C code
set_ne xschem_libs {}
set_ne tcl_debug 0
# used to activate debug from menu
set_ne menu_tcl_debug 0
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

## list of tcl procedures to load at end of xschem.tcl
set_ne tcl_files {}
set_ne netlist_dir "$USER_CONF_DIR/simulations"
set_ne bus_replacement_char {} ;# use {<>} to replace [] with <> in bussed signals
set_ne hspice_netlist 0
set_ne verilog_2001 1
set_ne split_files 0
set_ne flat_netlist 0
set_ne netlist_type vhdl
set_ne netlist_show 0
set_ne color_ps 0
set_ne only_probes 0  ; # 20110112
set_ne a3page 0
set_ne fullscreen 0
set_ne unzoom_nodrift 1
set_ne change_lw 0
set_ne draw_window 0
set_ne line_width 0
set_ne incr_hilight 1
set_ne enable_stretch 0
set_ne horizontal_move 0 ; # 20171023
set_ne vertical_move 0 ; # 20171023
set_ne draw_grid 1
set_ne snap 10
set_ne grid 20
set_ne persistent_command 0
set_ne disable_unique_names 1
set_ne sym_txt 1
set_ne show_infowindow 0 
set_ne symbol_width 150
set_ne editprop_semaphore 0
set_ne editor {gvim -f}
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

# set_ne analog_viewer waveview
set_ne computerfarm {} ;# 20151007

## utile
set_ne utile_gui_path "${XSCHEM_SHAREDIR}/utile/utile3"
set_ne utile_cmd_path "${XSCHEM_SHAREDIR}/utile/utile"

## cairo stuff 20171112
set_ne cairo_font_scale 1.0
set_ne nocairo_font_xscale .85
set_ne nocairo_font_yscale .88
set_ne cairo_font_line_spacing 1.0
set_ne cairo_vert_correct 0
set_ne nocairo_vert_correct 0

# Arial, Monospace
set_ne cairo_font_name {Arial}
# has_cairo set by c program if cairo enabled
set has_cairo 0 
set rotated_text {} ;#20171208
set_ne dark_colorscheme 1
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
   "#ff00ff" "#00ff00" "#0000cc" "#aaaa00" "#aaccaa"
   "#ff7777" "#bfff81" "#00ffcc" "#ce0097" "#d2d46b" 
   "#ef6158" "#fdb200"}

  set_ne ps_colors {
    0x000000 0x0000ee 0x7f7f7f 0x000000 0x338800 
    0xbb2200 0x0000ee 0xff0000 0xffff00 0x000000 
    0xff00ff 0x00ff00 0x0000cc 0xaaaa00 0xaaccaa 
    0xff7777 0xbfff81 0x00ffcc 0xce0097 0xd2d46b 
    0xef6158 0xfdb200}
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
  # same colors as above ...
  set_ne dark_colors {
    "#000000" "#00ccee" "#3f3f3f" "#ffffff" "#88dd00" 
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

## pad missing colors with black
for {set i 0} { $i<$cadlayers } { incr i} {
  set_ne enable_layer($i) 1
  foreach j { ps_colors light_colors dark_colors } {
    if { ![string compare [lindex [set $j] $i] {} ] } {
      if { ![string compare $j {ps_colors} ] } {
        lappend $j {0x000000}
      } else {
        lappend $j {#000000}
      }
    }
  }
}

set_ne colors $dark_colors
##### end set colors


# 20111005 added missing initialization of globals...
set_ne no_change_attrs 0
set_ne preserve_unchanged_attrs 0
set search_select 0

# 20111106 these vars are overwritten by caller with mktemp file names
set filetmp1 $env(PWD)/.tmp1
set filetmp2 $env(PWD)/.tmp2
# /20111106

# flag bound to a checkbutton in symbol editprop form
# if set cell is copied when renaming it
set_ne copy_cell 0


# for svg draw 20121108
regsub -all {\"} $colors  {} svg_colors
regsub -all {#} $svg_colors  {0x} svg_colors


# schematic to preload in new windows 20090708
set_ne XSCHEM_START_WINDOW {}

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

set txt ""
set custom_token {lab}
set search_value {}
set search_exact 0


# 20171005
set custom_label_prefix {}

# 20171112 cairo stuff
xschem set cairo_font_scale $cairo_font_scale
xschem set nocairo_font_xscale $nocairo_font_xscale
xschem set nocairo_font_yscale $nocairo_font_yscale
xschem set cairo_font_line_spacing $cairo_font_line_spacing
xschem set cairo_vert_correct $cairo_vert_correct
xschem set nocairo_vert_correct $nocairo_vert_correct
xschem set persistent_command $persistent_command
xschem set disable_unique_names $disable_unique_names
# font name can not be set here as we need to wait for X-initialization 
# to complete. Done in xinit.c

###
### build Tk widgets
###
if { [string length   [lindex [array get env DISPLAY] 1] ] > 0 
     && ![info exists no_x]} {

# for hyperlink in about dialog
eval  font create Underline-Font [ font actual TkDefaultFont ]
font configure Underline-Font -underline true -size 24


   . configure -cursor left_ptr
#   option add *Button*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Menubutton*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Menu*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Listbox*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Entry*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   #option add *Text*font "-*-courier-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#   option add *Label*font "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile

   if { [info exists tk_scaling] } {tk scaling $tk_scaling}
   set infowindow_text {}
   infowindow
   #proc unknown  {comm args} { puts "unknown command-> \<$comm\> $args" }
   frame .menubar -relief raised -bd 2 
   
   menubutton .menubar.file -text "File" -menu .menubar.file.menu
   menu .menubar.file.menu -tearoff 0
   menubutton .menubar.edit -text "Edit" -menu .menubar.edit.menu
   menu .menubar.edit.menu -tearoff 0
   menubutton .menubar.option -text "Options" -menu .menubar.option.menu
   menu .menubar.option.menu -tearoff 0
   menubutton .menubar.zoom -text "View" -menu .menubar.zoom.menu
   menu .menubar.zoom.menu -tearoff 0
   menubutton .menubar.prop -text "Properties" -menu .menubar.prop.menu
   menu .menubar.prop.menu -tearoff 0
   menubutton .menubar.layers -text "Layers" -menu .menubar.layers.menu \
    -background [lindex $colors 4]
   menu .menubar.layers.menu -tearoff 0
   menubutton .menubar.tools -text "Tools" -menu .menubar.tools.menu
   menu .menubar.tools.menu -tearoff 0
   menubutton .menubar.sym -text "Symbol" -menu .menubar.sym.menu
   menu .menubar.sym.menu -tearoff 0
   menubutton .menubar.hilight -text "Highlight" -menu .menubar.hilight.menu
   menu .menubar.hilight.menu -tearoff 0
   menubutton .menubar.simulation -text "Simulation" -menu .menubar.simulation.menu
   menu .menubar.simulation.menu -tearoff 0
   menubutton .menubar.help -text "Help" -menu .menubar.help.menu
   menu .menubar.help.menu -tearoff 0
   .menubar.help.menu add command -label "Help" -command "textwindow \"${XSCHEM_SHAREDIR}/xschem.help\" ro" \
        -accelerator {?}
   .menubar.help.menu add command -label "Keys" -command "textwindow \"${XSCHEM_SHAREDIR}/keys.help\" ro"
   .menubar.help.menu add command -label "About XSCHEM" -command "about"
   
   .menubar.file.menu add command -label "New Schematic"  -accelerator Ctrl+N\
     -command {
       xschem clear SCHEMATIC
     }
   .menubar.file.menu add command -label "New Symbol" -accelerator Ctrl+Shift+N \
     -command {
       xschem clear SYMBOL
     }
   .menubar.file.menu add command -label "Open" -command "xschem load" -accelerator {Ctrl+O}
   .menubar.file.menu add command -label "Save" -command "xschem save" -accelerator {Ctrl+S}
   .menubar.file.menu add command -label "Merge" -command "xschem merge" -accelerator B
   .menubar.file.menu add command -label "Reload" -accelerator {Alt+S} \
     -command {
      if { [string compare [tk_messageBox -type okcancel -message {sure wanna reload?}] ok]==0 } {
              xschem reload
         }
     }
   .menubar.file.menu add command -label "Save as" -command "xschem saveas" -accelerator {Ctrl+Shift+S}
   .menubar.file.menu add command -label "Save as symbol" \
      -command "xschem set current_type SYMBOL; xschem saveas" -accelerator {Ctrl+Alt+S}
   # added svg, png 20171022
   .menubar.file.menu add command -label "PDF Export" -command "xschem print pdf" -accelerator {*}
   .menubar.file.menu add command -label "PNG Export" -command "xschem print png" -accelerator {Ctrl+*}
   .menubar.file.menu add command -label "SVG Export" -command "xschem print svg" -accelerator {Alt+*}
   .menubar.file.menu add separator
   .menubar.file.menu add command -label "Exit" -command {xschem exit} -accelerator {Ctrl+Q}
   
   .menubar.option.menu add checkbutton -label "Show info win" -variable show_infowindow \
     -command {
        if { $show_infowindow != 0 } {wm deiconify .infotext
        } else {wm withdraw .infotext}
      }
   .menubar.option.menu add checkbutton -label "Color postscript" -variable color_ps \
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
   .menubar.option.menu add checkbutton -label "Enable stretch" -variable enable_stretch \
      -accelerator Y \
      -command {
         if { $enable_stretch==1 } {xschem set enable_stretch 1} else { xschem set enable_stretch 0} 
      }
   .menubar.option.menu add checkbutton -label "Show netlist win" -variable netlist_show \
      -accelerator {Shift+A} \
      -command {
         if { $netlist_show==1 } {xschem set netlist_show 1} else { xschem set netlist_show 0} 
      }
   .menubar.option.menu add checkbutton -label "Flat netlist" -variable flat_netlist \
      -accelerator : \
      -command {
         if { $flat_netlist==1 } {xschem set flat_netlist 1} else { xschem set flat_netlist 0} 
      }
   .menubar.option.menu add checkbutton -label "Split netlist" -variable split_files \
      -accelerator {} \
      -command {
         if { $split_files==1 } {xschem set split_files 1} else { xschem set split_files 0} 
      }
   .menubar.option.menu add checkbutton -label "hspice / ngspice netlist" -variable hspice_netlist \
      -accelerator {} \
      -command {
         if { $hspice_netlist==1 } {xschem set hspice_netlist 1} else { xschem set hspice_netlist 0} 
      }
   .menubar.option.menu add checkbutton -label "Verilog 2001 netlist variant" -variable verilog_2001 \
   
   .menubar.option.menu add checkbutton -label "Draw grid" -variable draw_grid \
      -accelerator {%} \
      -command {
        if { $draw_grid == 1} { xschem set draw_grid 1; xschem redraw} else { xschem set draw_grid 0; xschem redraw}
      }
   .menubar.option.menu add checkbutton -label "Symbol text" -variable sym_txt \
      -accelerator {Ctrl+B} \
      -command {
        if { $sym_txt == 1} { xschem set sym_txt 1; xschem redraw} else { xschem set sym_txt 0; xschem redraw}
      }
   .menubar.option.menu add checkbutton -label "Toggle variable line width" -variable change_lw \
      -accelerator {_} \
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
   .menubar.option.menu add checkbutton -label "Allow duplicated instance names (refdes)" \
       -variable disable_unique_names -command {
          xschem set disable_unique_names $disable_unique_names
       }

   .menubar.option.menu add separator
   .menubar.option.menu add radiobutton -label "Spice netlist" -variable netlist_type -value spice \
        -accelerator {Shift+V} \
        -command "xschem netlist_type spice"
   .menubar.option.menu add radiobutton -label "VHDL netlist" -variable netlist_type -value vhdl \
        -accelerator {Shift+V} \
        -command "xschem netlist_type vhdl"
   .menubar.option.menu add radiobutton -label "Verilog netlist" -variable netlist_type -value verilog \
        -accelerator {Shift+V} \
        -command "xschem netlist_type verilog"
   .menubar.option.menu add radiobutton -label "tEDAx netlist" -variable netlist_type -value tedax \
        -accelerator {Shift+V} \
        -command "xschem netlist_type tedax"
   .menubar.edit.menu add command -label "Undo" -command "xschem undo; xschem redraw" -accelerator U
   .menubar.edit.menu add command -label "Redo" -command "xschem redo; xschem redraw" -accelerator {Shift+U}
   .menubar.edit.menu add command -label "Copy" -command "xschem copy" -accelerator Ctrl+C
   .menubar.edit.menu add command -label "Cut" -command "xschem cut"   -accelerator Ctrl+X
   .menubar.edit.menu add command -label "Paste" -command "xschem paste" -accelerator Ctrl+V
   .menubar.edit.menu add command -label "Delete" -command "xschem delete" -accelerator Del
   .menubar.edit.menu add command -label "Select all" -command "xschem select_all" -accelerator Ctrl+A
   .menubar.edit.menu add command -label "Edit selected schematic in new window" \
       -command "xschem schematic_in_new_window" -accelerator Alt+E
   .menubar.edit.menu add command -label "Edit selected symbol in new window" \
       -command "xschem symbol_in_new_window" -accelerator Alt+I
   .menubar.edit.menu add command -label "Duplicate objects" -command "xschem copy_objects" -accelerator C
   .menubar.edit.menu add command -label "Move objects" -command "xschem move_objects" -accelerator M
   .menubar.edit.menu add checkbutton -label "Constrained Horizontal move" -variable horizontal_move \
      -command "xschem set horizontal_move" -accelerator H
   .menubar.edit.menu add checkbutton -label "Constrained Vertical move" -variable vertical_move \
      -command "xschem set vertical_move" -accelerator V
   # added collapse/join/break wires menu command  (& key) 20171022
   .menubar.edit.menu add command -label "Push schematic" -command "xschem descend" -accelerator E
   .menubar.edit.menu add command -label "Push symbol" -command "xschem descend_symbol" -accelerator I
   .menubar.edit.menu add command -label "Pop" -command "xschem go_back" -accelerator Ctrl+E
   button .menubar.waves -text "Waves"  -activebackground red  -takefocus 0\
     -command {
       waves
      }
   button .menubar.simulate -text "Simulate"  -activebackground red  -takefocus 0\
     -command {
       if { ![info exists simulate_oldbg] } {
         set simulate_oldbg [.menubar.simulate cget -bg]
         .menubar.simulate configure -bg red
         simulate {.menubar.simulate configure -bg $::simulate_oldbg; unset ::simulate_oldbg}
       }
      }
   button .menubar.netlist -text "Netlist"  -activebackground red  -takefocus 0\
     -command {
       xschem netlist
      }
   
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

     .menubar.layers.menu add command -label $laylab  -activeforeground $layfg \
        -foreground $layfg -background $i -activebackground $i \
        -command "xschem set rectcolor $j; reconfigure_layers_button"
     if { [expr $j%10] == 0 } { .menubar.layers.menu entryconfigure $j -columnbreak 1 }
     incr j
     
   }
   .menubar.zoom.menu add command -label "Redraw" -command "xschem redraw" -accelerator Esc
   .menubar.zoom.menu add checkbutton -label "Fullscreen" -variable fullscreen \
      -accelerator {Alt+Shift+F} -command {
         xschem fullscreen
      }
   .menubar.zoom.menu add command -label "Zoom Full" -command "xschem zoom_full" -accelerator F
   .menubar.zoom.menu add command -label "Zoom In" -command "xschem zoom_in" -accelerator Shift+Z
   .menubar.zoom.menu add command -label "Zoom Out" -command "xschem zoom_out" -accelerator Ctrl+Z
   .menubar.zoom.menu add command -label "Zoom box" -command "xschem zoom_box" -accelerator Z
   .menubar.zoom.menu add command -label "Half Snap Threshold" -accelerator G -command {
          xschem set cadsnap [expr [xschem get cadsnap] / 2.0 ]
        }
   .menubar.zoom.menu add command -label "Double Snap Threshold" -accelerator Shift-G -command {
          xschem set cadsnap [expr [xschem get cadsnap] * 2.0 ]
        }
   .menubar.zoom.menu add command -label "Set snap value" \
          -command {
          input_number "Enter snap value ( default: [xschem get cadsnap_default] current: [xschem get cadsnap])" \
          "xschem set cadsnap"
        }
   .menubar.zoom.menu add command -label "Set grid spacing" \
        -command {
          input_number "Enter grid spacing (float):" "xschem set cadgrid"
        }
   .menubar.zoom.menu add checkbutton -label "View only Probes" -variable only_probes \
          -accelerator {5} \
          -command { xschem only_probes }
   .menubar.zoom.menu add command -label "Toggle colorscheme"  -accelerator {Shift+O} -command {
           xschem toggle_colorscheme
           xschem change_colors
        }
   .menubar.zoom.menu add command -label "Dim colors"  -accelerator {} -command {
           color_dim
           xschem color_dim
        }
   .menubar.zoom.menu add command -label "Symbol visible layers"  -accelerator {} -command {
           select_layers
           xschem redraw
        }
   .menubar.zoom.menu add command -label "Change Current Layer color"  -accelerator {} -command {
           change_color
        }
   .menubar.zoom.menu add checkbutton -label "No XCopyArea drawing model" -variable draw_window \
          -accelerator {Ctrl+$} \
          -command {
           if { $draw_window == 1} { xschem set draw_window 1} else { xschem set draw_window 0}
        }
   .menubar.prop.menu add command -label "Edit" -command "xschem edit_prop" -accelerator Q
   .menubar.prop.menu add command -label "Edit with editor" -command "xschem edit_vi_prop" -accelerator Shift+Q
   .menubar.prop.menu add command -label "View" -command "xschem view_prop" -accelerator Ctrl+Q
   .menubar.prop.menu add command -background red -label "Edit file (danger!)" -command "xschem edit_file" -accelerator Alt+Q

   .menubar.sym.menu add command -label "Make symbol from schematic" -command "xschem make_symbol" -accelerator A
   .menubar.sym.menu add command -label "Make schematic from symbol" -command "xschem make_sch" -accelerator Ctrl+L
   .menubar.sym.menu add command -label "Attach pins to component instance" -command "xschem attach_pins" -accelerator Shift+H
   .menubar.sym.menu add command -label "Create Symbol pins from selected schematic pins" \
           -command "schpins_to_sympins" -accelerator Alt+H

   .menubar.tools.menu add checkbutton -label "Remember last command" -variable persistent_command \
      -accelerator {} \
      -command {
        if { $persistent_command == 1} { xschem set persistent_command 1} else { xschem set persistent_command 0}
      }
   .menubar.tools.menu add command -label "Insert symbol" -command "xschem place_symbol" -accelerator Ins
   .menubar.tools.menu add command -label "Insert text" -command "xschem place_text" -accelerator T
   .menubar.tools.menu add command -label "Insert wire" -command "xschem wire" -accelerator W
   .menubar.tools.menu add command -label "Insert snap wire" -command "xschem snap_wire" -accelerator Shift+W
   .menubar.tools.menu add command -label "Insert line" -command "xschem line" -accelerator L
   .menubar.tools.menu add command -label "Insert rect" -command "xschem rect" -accelerator R
   .menubar.tools.menu add command -label "Insert polygon" -command "xschem polygon" -accelerator Ctrl+W
   .menubar.tools.menu add command -label "Insert arc" -command "xschem arc" -accelerator Shift+C
   .menubar.tools.menu add command -label "Insert circle" -command "xschem circle" -accelerator Ctrl+Shift+C
   .menubar.tools.menu add command -label "Search" -accelerator Ctrl+F -command  property_search
   .menubar.tools.menu add command -label "Align to Grid" -accelerator Alt+U -command  "xschem align"
   .menubar.tools.menu add command -label "Execute TCL command" -command  "tclcmd"
   .menubar.tools.menu add command -label "Join/Trim wires" \
      -command "xschem collapse_wires" -accelerator {&}
   .menubar.tools.menu add command -label "Break wires" \
      -command "xschem break_wires" -accelerator {!}

   .menubar.hilight.menu add command -label {Highlight duplicate instance names} -command "xschem check_unique_names 0" -accelerator {#} 
   .menubar.hilight.menu add command -label {Rename duplicate instance names} -command "xschem check_unique_names 1" -accelerator {Ctrl+#}
   .menubar.hilight.menu add command -label {Highlight selected net/pins} -command "xschem hilight" -accelerator K
   .menubar.hilight.menu add command -label {Select connected nets / pins} -command "xschem select_connected_nets" -accelerator Alt+K
   .menubar.hilight.menu add command -label {Un-highlight all net/pins} \
        -command "xschem clear_hilights" -accelerator Shift+K
   .menubar.hilight.menu add command -label {Un-highlight selected net/pins} \
        -command "xschem unhilight" -accelerator Ctrl+K
   # 20160413
   .menubar.hilight.menu add checkbutton -label {Auto-highlight net/pins} -variable auto_hilight \
      -command {
        if { $auto_hilight == 1} {
          xschem set auto_hilight 1
        } else {
          xschem set auto_hilight 0
        }
      }

   .menubar.simulation.menu add command -label "Set netlist Dir" \
     -command {
           select_netlist_dir 1
     }
   .menubar.simulation.menu add command -label {Configure simulators and tools} -command {simconf}
   .menubar.simulation.menu add command -label {Utile Stimuli Editor (GUI)} -command {utile_gui [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label "Utile Stimuli Editor ([lindex $editor 0])" -command {utile_edit [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label {Utile Stimuli Translate} -command {utile_translate [file tail [xschem get schname]]}
   .menubar.simulation.menu add command -label {Shell [simulation path]} \
      -command {
         if { [select_netlist_dir 0] ne "" } {
           get_shell $netlist_dir
         }
       }
   .menubar.simulation.menu add command -label {Edit Netlist} -command {edit_netlist [file tail [xschem get schname]]}

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

   frame .drw -background {} -takefocus 1

   wm  title . "xschem - "
   wm iconname . "xschem - "
   . configure  -background {}
   wm  geometry . $initial_geometry
   #wm maxsize . 1600 1200
   wm protocol . WM_DELETE_WINDOW {xschem exit}
   focus .drw

   frame .statusbar  
   label .statusbar.1   -text "STATUS BAR 1"  
   label .statusbar.2   -text "SNAP:"
   entry .statusbar.3 -textvariable snap -relief sunken -bg white \
          -width 10 -foreground black 
   label .statusbar.4   -text "GRID:"
   entry .statusbar.5 -textvariable grid -relief sunken -bg white \
          -width 10 -foreground black 
   label .statusbar.6   -text "NETLIST MODE:"
   entry .statusbar.7 -textvariable netlist_type -relief sunken -bg white \
          -width 10 -state disabled -disabledforeground black 
   pack .statusbar.2 -side left 
   pack .statusbar.3 -side left 
   pack .statusbar.4 -side left 
   pack .statusbar.5 -side left 
   pack .statusbar.6 -side left 
   pack .statusbar.7 -side left 
   pack .statusbar.1 -side left -fill x
   pack .drw -anchor n -side top -fill both -expand true
   pack .menubar -anchor n -side top -fill x  -before .drw
   pack .statusbar -after .drw -anchor sw  -fill x 
   bind .statusbar.5 <Leave> { xschem set cadgrid $grid; focus .drw}
   bind .statusbar.3 <Leave> { xschem set cadsnap $snap; focus .drw}
###
### Tk event handling
###
### bind .drv <event> {xschem callback <type> <x> <y> <keysym> <button of w> <h> <state>}
###
   bind .drw <Double-Button-1> {xschem callback -3 %x %y 0 %b 0 %s}
   bind .drw <Double-Button-2> {xschem callback -3 %x %y 0 %b 0 %s}
   bind .drw <Double-Button-3> {xschem callback -3 %x %y 0 %b 0 %s}
   bind .drw <Expose> {xschem callback %T %x %y 0 %w %h %s}
   bind .drw <Configure> {xschem windowid; xschem callback %T %x %y 0 %w %h 0}
   bind .drw <ButtonPress> {xschem callback %T %x %y 0 %b 0 %s}
   bind .drw <ButtonRelease> {xschem callback %T %x %y 0 %b 0 %s}
   bind .drw <KeyPress> {xschem callback %T %x %y %N 0 0 %s}
   bind .drw <KeyRelease> {xschem callback %T %x %y %N 0 0 %s} ;# 20161118
   bind .drw <Motion> {xschem callback %T %x %y 0 0 0 %s}
   bind .drw  <Enter> {xschem callback %T %x %y 0 0 0 0 }
   bind .drw <Leave> {}
   bind .drw <Unmap> {
    wm withdraw .infotext
    set show_infowindow 0
   }
   bind .drw  "?" { textwindow "${XSCHEM_SHAREDIR}/xschem.help" }

   if {[array exists replace_key]} {
     foreach i [array names replace_key] {
       key_binding "$i" "$replace_key($i)"
     }
   }
}


foreach i $tcl_files {
  source $i
}
