#!/usr/bin/gawk -f
BEGIN{
  debug = 1
  symbolx1 = 0
  symbolx2 = 700 # recalculated when number of top/bottom pins is known
  symboly1 = 0
  symboly2 = 700 # recalculated when number of left/right pins is known

  pinspacing = 40
  pinbusspacing = 20
  firstpinyoffset = 60 # recalculated when top pin max label length is known
  firstpinxoffset = 40
  lastpinyoffset = 60 # recalculated when bottom pin max label length is known
  lastpinxoffset = 40
  pintextoffset = 11
  pinboxhalfsize = 2.5
  pinlinelength = 40
  pintextsize = 0.4
  pintextcenteroffset = 24 * pintextsize
  pintextheight = 56.6 * pintextsize
  pincharspacing = 35 * pintextsize
  pinnumbertextsize = 0.25
  pinnumbercenteroffset = 16
  pinnumberoffset = 12
  labeltextsize = 0.4
  labelspacing = 40 # vertical spacing of label strings
  labelcharspacing = 30 * labeltextsize
  labeloffset = 20 # extra space to put around labels to avoid collision with pin text labels
  labelcenteroffset = 8
  negradius = 5
  triggerxsize = 8
  triggerysize = 8
  grid = 20
  start_pin=0
  nlines = 0
  bus = 0
  pos = 0 ## default if no [position] field specified
  horizontal = 1 # default orient. for top/bottom pins, can be changed with --vertical
}

/^--/{
  if(/^--auto_pinnumber/) { enable_autopinnumber = 1 }
  if(/^--hide_pinnumber/) { hide_pinnumber = 1 }
  if(/^--vmode/) { horizontal = 0 } # force horizontal top/bottom labels
  next
}

/^[ \t]*#/{
  next
}

/^[ \t]*\[labels\]/ {
  start_labels = 1
  next
}

start_labels == 1 {
  if(/^\[/) start_labels = 0
  else if(/^! /) {
    if(attributes) attributes = attributes "\n"
    attributes = attributes substr($0, 3)
    dbg("attributes=" attributes)
    next
  } else if(NF > 0){
    labn = label["n"]++
    label[labn] = $0
    if( length($0) > label["maxlength"] ) label["maxlength"] = length($0)
    dbg("label: " $0)
    next
  }
}

/^[ \t]*\[left\]/   { pos = 0 }
/^[ \t]*\[top\]/    { pos = 1 }
/^[ \t]*\[right\]/  { pos = 2 }
/^[ \t]*\[bottom\]/ { pos = 3 }
/^[ \t]*\[[^][]*\][ \t]*$/ {
  start_labels = 0
  posseq[seqidx++] = pos
  pin[pos, "n"] = 0;
  start_pin = 1;
  coord = 0;
  next
}

/^\./{ 
  if(/^\.bus/) bus = 1
  next
}

## typical pin line is as follows
## 26   i!>   CLK
start_pin {

  if(enable_autopinnumber) $0 = autopinnumber++ " " $0
  n =  pin[pos, "n"]
  if(NF == 2) { $0 = $1 " io " $2 } ## backward compatibility with 'djboxsym' format
  if(NF == 3) {
    trig = 0
    neg = 0
    dir = "inout" ## default if (mandatory) direction forgotten
    if($2 ~ />/) trig = 1
    if($2 ~ /!/) neg = 1
    if($2 ~ /io/) dir = "inout"
    else if($2 ~ /i/) dir = "in"
    else if($2 ~ /o/) dir = "out"
    else if($2 ~ /p/) dir = "inout" # xschem has no special attrs for power so set direction as inout
    pinnumber = $1
    pinname1 = pinname = $3 
    if(pinname ~ /^\\_.*\\_$/) {
      pin[pos, n, "overbar"] = 1
      sub(/^\\/, "", pinname)
      sub(/\\_$/, "_", pinname)
      sub(/^\\_/, "", pinname1)
      sub(/\\_$/, "", pinname1)
      dbg("pinname: " pinname " pinname1: " pinname1)
    }
    pin[pos, n, "pinname"] = pinname
    pin[pos, n, "pinnumber"] = pinnumber
    pin[pos, n, "dir"] = dir
    pin[pos, n, "neg"] = neg
    pin[pos, n, "trigger"] = trig
    if( (pos == 1 || pos == 3 ) && horizontal) pin[pos, n, "bus"] = 0
    else pin[pos, n, "bus"] = bus
    if( length(pinname1) > pin[pos, "maxlength"] ) pin[pos, "maxlength"] = length(pinname)
    pin[pos, "n"]++
  } else if(NF == 0) {
    if(pin[pos, n, "bus"] && n > 1) {
      dbg("last bus bit: " pin[pos, n-1, "pinname"])
      ## empty line after list of bus pins is not a spacer, it ends bus spacing.
      ## remove bus flag on last bus pin so next pin will be spaced normally
      pin[pos, n-1,"bus"] = 0
      coord -= pinbusspacing
    } else {
      pin[pos, n, "spacer"] = 1
      dbg("spacer1")
      pin[pos, "n"]++
    }
    bus = 0
  }
 
  if((pos == 1 || pos == 3) && horizontal) {
    if(pin[pos, n, "spacer"] != 1) {
      pin[pos, "maxcoord"] = coord
      dbg("horiz top/bot pin: " pin[pos, n, "pinname"]" coord: " coord)
      coord += round(length(pin[pos, n, "pinname"]) * pincharspacing + pintextoffset)
    } else {
      pin[pos, "maxcoord"] = coord
      coord += pinspacing
      dbg("spacer2: coord: " coord)
    }
  } else {
    pin[pos, "maxcoord"] = coord
    coord += (bus) ? pinbusspacing : pinspacing
  }
  dbg("pos: " pos " maxcoord: " pin[pos, "maxcoord"])
}

## GENERATOR
END{
  header()
  attrs(attributes)
  
  if(horizontal !=1) {
    firstpinyoffset = round(pincharspacing * pin[1, "maxlength"] + pintextoffset)
    lastpinyoffset = round(pincharspacing * pin[3, "maxlength"] + pintextoffset)
  }
  dbg("pin[3, maxlength]: " pin[3, "maxlength"] " pin[1, maxlength]: " pin[1, "maxlength"])
  dbg("firstpinyoffset: " firstpinyoffset " lastpinyoffset: " lastpinyoffset)
  if(horizontal) {
    firstpinxoffset = max( round(length(pin[1, 0, "pinname"]) * pincharspacing / 2 + pintextoffset), \
                           round(length(pin[3, 0, "pinname"]) * pincharspacing / 2 + pintextoffset))
    lp1 = pin[1, "n"] -1
    lp3 = pin[3, "n"] -1
    lastpinxoffset = max( round(length(pin[1, lp1, "pinname"]) * pincharspacing / 2 + pintextoffset), \
                          round(length(pin[3, lp3, "pinname"]) * pincharspacing / 2 + pintextoffset))
    dbg("firstpinxoffset: " firstpinxoffset)
    dbg("lastpinxoffset: " lastpinxoffset)
  }
  symboly2 = max(pin[0,"maxcoord"], pin[2, "maxcoord"]) + firstpinyoffset + lastpinyoffset
  dbg("pin[3, maxcoord]: " pin[3, "maxcoord"])
  dbg("pin[1, maxcoord]: " pin[1, "maxcoord"])
  dbg("pin[0, maxcoord]: " pin[0, "maxcoord"])
  dbg("pin[2, maxcoord]: " pin[2, "maxcoord"])
  topbotpinsize = max(pin[1,"maxcoord"], pin[3, "maxcoord"]) + firstpinxoffset + lastpinxoffset
  labsize = ( (labelcharspacing * label["maxlength"] + labeloffset)/2 \
             + max(pincharspacing * pin[0, "maxlength"], pincharspacing * pin[2, "maxlength"])) * 2
  dbg("labsize: " labsize)
  dbg("topbotpinsize: " topbotpinsize)
  symbolx2 = round( max(topbotpinsize, labsize) / 2) * 2 ## round to double grid so half size is grid-aligned when centering
  dbg("symbolx2: " symbolx2)
  ## center symbol after size calculations are done
  symbolx1 -= round(symbolx2/2)
  symbolx2 += symbolx1
  symboly1 -= round(symboly2/2)
  symboly2 += symboly1

  dbg("topbotpinsize: " topbotpinsize)
  for(i = 0; i < 4; i++) {
    p = posseq[i]
    if(p == 0 ) { 
      x = symbolx1 - pinlinelength
      y = symboly1 + firstpinyoffset
    }
    else if(p == 1 ) { 
      x = symbolx1 + firstpinxoffset
      y = symboly1 - pinlinelength
    }
    else if(p == 2 ) { 
      x = symbolx2 + pinlinelength
      y = symboly1 + firstpinyoffset
    }
    else if(p == 3 ) { 
      x = symbolx1 + firstpinxoffset
      y = symboly2 + pinlinelength
    }
    for(n = 0; n < pin[p,"n"]; n++) {
      if(pin[p, n, "spacer"] != 1) {
        if(pin[p, n, "bus"]) pd = pinbusspacing
        else pd = pinspacing
        pinbox(p, n, x, y)
        neg = pin[p, n, "neg"]
        trig = pin[p, n, "trigger"]
        pinname = pin[p, n, "pinname"]
        if(pin[p, n, "overbar"] == 1) {
          sub(/^_/,"", pinname)
          sub(/_$/,"", pinname)
        }
        if(p == 0) {
          line(x, y, x + (neg ? pinlinelength - 2 * negradius : pinlinelength), y, 4, "")
          text(pinname, x + pinlinelength + pintextoffset, y - pintextcenteroffset, 0, 0, pintextsize, "")
          if(pin[p, n, "overbar"] == 1) {
            obx = x + pinlinelength + pintextoffset
            oby = y - pintextcenteroffset
            obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
            line(obx, oby, obx + obl, oby, 3, "")
          }
          if(!hide_pinnumber)text(pin[p, n, "pinnumber"], x + pinlinelength - pinnumberoffset, \
               y - pinnumbercenteroffset, 0, 1, pinnumbertextsize, "")
          if(neg) circle(x + pinlinelength - negradius, y, negradius, 4, "")
        } else if(p == 1) {
          line(x, y, x, y + (neg ? pinlinelength - 2 * negradius : pinlinelength) , 4, "")
          if(horizontal) {
            halflabwidth = length(pinname) * pincharspacing / 2
            text(pinname, x - halflabwidth, y + pinlinelength + pintextoffset, 0, 0, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x - halflabwidth
              oby = y + pinlinelength + pintextoffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby, obx + obl, oby, 3, "")
            }
          } else {
            text(pinname, x + pintextcenteroffset, y + pinlinelength + pintextoffset, 1, 0, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x + pintextcenteroffset - pintextheight
              oby = y + pinlinelength + pintextoffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby + obl, obx, oby, 3, "")
            }
          }
          if(!hide_pinnumber)text(pin[p, n, "pinnumber"], x - pinnumbercenteroffset, \
               y + pinlinelength - pinnumberoffset, 3, 0, pinnumbertextsize, "")
          if(neg) circle(x, y + pinlinelength - negradius, negradius, 4, "")
        } else if(p == 2) {
          line(x - (neg ? pinlinelength -2 * negradius :pinlinelength), y, x, y, 4, "")
          text(pinname, x - pinlinelength - pintextoffset, y - pintextcenteroffset, 0, 1, pintextsize, "")
          if(pin[p, n, "overbar"] == 1) {
            obx = x - pinlinelength - pintextoffset
            oby = y - pintextcenteroffset
            obl = (length(pin[p, n, "pinname"]) - 2) * pincharspacing
            line(obx - obl, oby, obx, oby, 3, "")
          }
          if(!hide_pinnumber)text(pin[p, n, "pinnumber"], x - pinlinelength + pinnumberoffset, \
               y - pinnumbercenteroffset, 0, 0, pinnumbertextsize, "")
          if(neg) circle(x - pinlinelength + negradius, y, negradius, 4, "")
        } else if(p == 3) {
          line(x, y - (neg ? pinlinelength -2 * negradius :pinlinelength), x, y, 4, "")
          if(horizontal) {
            halflabwidth = length(pinname) * pincharspacing / 2
            text(pinname, x - halflabwidth, y - pinlinelength - pintextoffset, 2, 1, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x - halflabwidth
              oby = y - pinlinelength - pintextoffset - pintextheight
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby, obx + obl, oby, 3, "")
            }
          } else {
            text(pinname, x + pintextcenteroffset, y - pinlinelength - pintextoffset, 1, 1, pintextsize, "")
            if(pin[p, n, "overbar"] == 1) {
              obx = x + pintextcenteroffset - pintextheight
              oby = y - pinlinelength - pintextoffset
              obl = (length(pin[p, n, "pinname"]) - 2 ) * pincharspacing
              line(obx, oby, obx, oby - obl, 3, "")
            }
          }
          if(!hide_pinnumber)text(pin[p, n, "pinnumber"], x - pinnumbercenteroffset, \
               y - pinlinelength + pinnumberoffset, 3, 1, pinnumbertextsize, "")
          if(neg) circle(x, y - pinlinelength + negradius, negradius, 4, "")
        }
        if(trig) trigger(x, y, 4, p)
      }

      pin[p, lastcoord] = (p == 0 || p == 2) ? y : x
      if(p == 0 || p == 2) y += pd
      else {
        if(horizontal) {
          if(pin[p, n, "spacer"] != 1) {
            if(n + 1 < pin[p,"n"]) {
              x += round( (length(pin[p, n, "pinname"]) + length(pin[p, n + 1, "pinname"]) ) * pincharspacing / 2  + pintextoffset)
            }
          } else x += pd
        } else x += pd
      }
    } # for(n)
    dbg("direction " p " last coord=" pin[p, lastcoord])
  } # for(p)
  box(symbolx1, symboly1, symbolx2, symboly2, 4, "")

  for(l = 0; l < label["n"]; l++) {
    dbg("label: " l " : " label[l])
    labx = (symbolx1 + symbolx2) / 2 - length(label[l]) * labelcharspacing /2 
    laby = (symboly1 + symboly2) / 2 + ( l - label["n"] / 2 ) * labelspacing + labelcenteroffset
    text(label[l], labx, laby, 0, 0, labeltextsize, "")
  }
   
}  

function dbg(s)
{
  if(debug) print s > "/dev/stderr"
}

function pinbox(p, n, x, y, arr)
{
  print "B 5", x-pinboxhalfsize, y-pinboxhalfsize, x+pinboxhalfsize, y+pinboxhalfsize, \
    "{name=" esc(pin[p, n, "pinname"]), \
     "dir=" pin[p, n, "dir"], \
     "pinnumber=" esc(pin[p, n, "pinnumber"]) "}"
}

function trigger(x, y, layer, pos)
{
  if(pos == 0) {
    line(x + pinlinelength, y - triggerysize, x + triggerxsize + pinlinelength, y, layer, "")
    line(x + pinlinelength , y + triggerysize, x + pinlinelength + triggerxsize, y, layer, "")
  } else if(pos == 2) {
    line(x - pinlinelength - triggerxsize, y, x - pinlinelength, y - triggerysize, layer, "")
    line(x - pinlinelength - triggerxsize, y , x - pinlinelength , y + triggerysize, layer, "")
  } else if(pos == 1) {
    line(x - triggerysize, y + pinlinelength, x, y + pinlinelength + triggerxsize, layer, "")
    line(x , y + pinlinelength + triggerxsize, x + triggerysize, y + pinlinelength, layer, "")
  } else if(pos == 3) {
    line(x - triggerysize, y - pinlinelength, x, y - pinlinelength - triggerxsize, layer, "")
    line(x , y - pinlinelength - triggerxsize, x + triggerysize, y - pinlinelength, layer, "")
  }
}

function circle(x, y, r, layer, props)
{
  print "A", layer, x, y, r, 0, 360, "{" esc(props) "}"
}

function box(x1, y1, x2, y2, layer, props)
{
  ## closed polygon
  print "P", layer, 5, x1, y1, x2, y1, x2, y2, x1, y2, x1, y1, "{" esc(props) "}"
}

function line(x1, y1, x2, y2, layer, props)
{
  print "L", layer, x1, y1, x2, y2, "{" esc(props) "}"
}

function text(t, x, y, rot, flip, size, props)
{
  print "T {" esc(t) "}", x, y, rot, flip, size, size, "{" esc(props) "}"
}

function attrs(a)
{
  print "G {" esc(a) "}"
}

function header()
{
  print "v {xschem version=2.9.2  file_version=1.1}"
}

function round(n)
{
  return n==0 ? 0 : (n > 0) ? (int( (n-0.001)/grid) +1) * grid : (int( (n+0.001)/grid) -1) * grid
}

function max(a, b) { return (a > b) ? a : b }

function esc(s)
{
  gsub(/\\/, "\\\\", s)
  gsub(/{/, "\\{", s)
  gsub(/}/, "\\}", s)
  return s
}
