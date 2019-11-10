
proc read_ngspice_raw {arr fp} {
  upvar $arr var
  unset -nocomplain var

  set variables 0
  while {[gets $fp line] >= 0} {
    if {$line eq "Binary:"} break
  
    if {[regexp {^No\. Variables:} $line]} {
      set n_vars [lindex $line end]
    }
    if {[regexp {^No\. Points:} $line]} {
      set n_points [lindex $line end]
    }
    if {$variables} {
      set var([lindex $line 1]) {}
      set idx([lindex $line 0]) [lindex $line 1]
    }
    if {[regexp {^Variables:} $line]} {
      set variables 1
    }
  }
  set bindata [read $fp [expr 8 * $n_vars * $n_points]]
  binary scan $bindata d[expr $n_vars * $n_points] data
  for {set p 0} {$p < $n_points} { incr p} {
    for {set v 0} {$v < $n_vars} { incr v} {
      lappend var($idx($v)) [lindex $data [expr $p * $n_vars + $v]]
      # puts "-->|$idx($v)|$var($idx($v))|"
    }
  }
  set var(n\ vars) $n_vars
  set var(n\ points) $n_points
}

proc get_voltage {arr n } {
  global voltage
  upvar $arr var
  set n "v($n)"
  if { abs($var([string tolower $n])) < 1e-3 } {
    return [format %.4e $var([string tolower $n])]
  } else {
    return [format %.4g $var([string tolower $n])]
  }
  # return DELETE
}

proc get_diff_voltage {arr p m } {
  global voltage
  upvar $arr var
  set p "v($p)"
  set m "v($m)"
  return [format %.4e [expr $var([string tolower $p]) - $var([string tolower $m]) ] ]
  # return DELETE
}

proc get_current {arr n } {
  global current_probe
  upvar $arr var
  set n "i($n)"
  if {abs($var([string tolower $n])) <1e-3} {
    return [format %.4e $var([string tolower $n])]
  } else {
    return [format %.4g $var([string tolower $n])]
  }
  # return DELETE
}




proc annotate {} {
  ### disable screen redraw and undo when looping to speed up performance
  ### but save state on undo stack before doing backannotations.

  set rawfile "[xschem get netlist_dir]/[file rootname [file tail [xschem get schname]]].raw"
  
  set fp [open $rawfile r]
  fconfigure $fp -translation binary
  
  while 1 {
    read_ngspice_raw arr $fp
    if { [info exists arr(n\ points)] && $arr(n\ points) == 1 } {
      set op_point_read 1; break
    }
  }
  close $fp
  if { $op_point_read } {
    xschem push_undo
    xschem set no_undo 1
    xschem set no_draw 1
    set lastinst [xschem get lastinst]
    for { set i 0 } { $i < $lastinst } {incr i } {
      set name [xschem getprop instance $i name]
      set type [xschem getprop instance $i cell::type]
      if { $type == "probe"  || $type == "devices/probe"} {
        set net [xschem instance_net $i p]
        if {[catch {xschem setprop instance $i voltage [get_voltage arr $net] fast} err]} {
          puts "1 error : $err net: $net"
        }
      }
      if { $type == "current_probe"  || $type == "devices/current_probe"} {
        if {[catch {xschem setprop instance $i current [get_current arr $name] fast} err]} {
          puts "2 error : $err"
        }
      }
      if { $type == "differential_probe"  || $type == "devices/differential_probe"} {
        set netp [xschem instance_net $i p]
        set netm [xschem instance_net $i m]
        if {[catch {xschem setprop instance $i voltage [get_diff_voltage arr $netp $netm] fast} err]} {
          puts "3 error : $err"
        }
      }
      # puts "$i $name $type"
    }
  
    # re-enable undo and draw
    xschem set no_undo 0
    xschem set no_draw 0
    xschem redraw
  
    ### xschem setprop instructions have not altered circuit topology so 
    ### in this case a connectivity rebuild is not needed.
    # xschem rebuild_connectivity
    #
    #
  }
}
