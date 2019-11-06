#!/usr/bin/gawk -f
BEGIN{
  debug = 0
  symbolx1 = 0
  symbolx2 = 700 # recalculated when number of top/bottom pins is known
  symboly1 = 0
  symboly2 = 700 # recalculated when number of left/right pins is known

  pinspacing = 40
  pinbusspacing = 20
  firstpinyoffset = 40 # recalculated when top pin max label length is known
  firstpinxoffset = 40
  lastpinyoffset = 40 # recalculated when bottom pin max label length is known
  lastpinxoffset = 40
  pintextoffset = 15
  pinboxhalfsize = 2.5
  pinlinelength = 40
  pintextsize = 0.3
  pintextcenteroffset = 6
  pincharspacing = 40 * pintextsize
  pinnumbertextsize = 0.25
  pinnumbercenteroffset = 16
  pinnumberoffset = 12
  labeltextsize = 0.4
  labelspacing = 40 # vertical spacing of label strings
  labelcharspacing = 30 * labeltextsize
  labeloffset = 20 # extra space to put around labels to avoid collision with pin text labels
  negradius = 5
  triggerxsize = 8
  triggerysize = 8
  grid = 20
  start_pin=0
  nlines = 0
  bus = 0
  pos = 0 ## default if no [position] field specified
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
  n =  pin[pos, "n"]
  if(NF == 2) { $0 = $1 " b " $2 } ## backward compatibility with 'djboxsym' format
  if(NF == 3) {
    trig = 0
    neg = 0
    dir = "inout" ## default if (mandatory) direction forgotten
    if($2 ~ />/) trig = 1
    if($2 ~ /!/) neg = 1
    if($2 ~ /i/) dir = "in"
    if($2 ~ /o/) dir = "out"
    if($2 ~ /b/) dir = "inout"
    pinnumber = $1
    pinname = $3
    pin[pos, n, "pinname"] = pinname
    pin[pos, n, "pinnumber"] = pinnumber
    pin[pos, n, "dir"] = dir
    pin[pos, n, "neg"] = neg
    pin[pos, n, "trigger"] = trig
    pin[pos, n, "bus"] = bus
    pin[pos, n, "coord"] = coord
    dbg("pin " pin[pos, n, "pinname"] " coord=" pin[pos, n, "coord"])
    if( length(pinname) > pin[pos, "maxlength"] ) pin[pos, "maxlength"] = length(pinname)
    pin[pos, "n"]++
  } else if(NF == 0) {
    if(bus && n > 1) {
      dbg("last bus bit: " pin[pos, n-1, "pinname"])
      ## empty line after list of bus pins is not a spacer, it ends bus spacing.
      ## remove bus flag on last bus pin so next pin will be spaced normally
      pin[pos, n-1,"bus"] = 0
      coord -= pinbusspacing
    } else {
      pin[pos, n, "spacer"] = 1
      pin[pos, "n"]++
    }
    bus = 0
  }
  pin[pos, "maxcoord"] = coord
  coord += (bus) ? pinbusspacing : pinspacing
}

## GENERATOR
END{
  header()
  attrs(attributes)
  firstpinyoffset = round(pincharspacing * pin[1, "maxlength"])
  lastpinyoffset = round(pincharspacing * pin[3, "maxlength"])
  symboly2 = max(pin[0,"maxcoord"], pin[2, "maxcoord"]) + firstpinyoffset + lastpinyoffset
  topbotpinsize = round(max(pin[1,"maxcoord"], pin[3, "maxcoord"]) + firstpinxoffset + lastpinxoffset)
  labsize = round( ( (labelcharspacing * label["maxlength"] + labeloffset)/2 \
             + max(pincharspacing * pin[0, "maxlength"], pincharspacing * pin[2, "maxlength"])) * 2 )
  symbolx2 = max(topbotpinsize, labsize)
  ## center symbol after size calculations are done
  symbolx1 -= round(symbolx2/2)
  symbolx2 += symbolx1
  symboly1 -= round(symboly2/2)
  symboly2 += symboly1

  dbg("labsize: " labsize)
  dbg("topbotpinsize: " topbotpinsize)
  for(p = 0; p < 4; p++) {
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
        if(p == 0) {
          line(x, y, x + (neg ? pinlinelength - 2 * negradius : pinlinelength), y, 4, "")
          text(pin[p, n, "pinname"], x + pinlinelength + pintextoffset, y - pintextcenteroffset, 0, 0, pintextsize, "")
          text(pin[p, n, "pinnumber"], x + pinlinelength - pinnumberoffset, y - pinnumbercenteroffset, 0, 1, pinnumbertextsize, "")
          if(neg) circle(x + pinlinelength - negradius, y, negradius, 4, "")
        } else if(p == 1) {
          line(x, y, x, y + (neg ? pinlinelength - 2 * negradius : pinlinelength) , 4, "")
          text(pin[p, n, "pinname"], x + pintextcenteroffset, y + pinlinelength + pintextoffset, 1, 0, pintextsize, "")
          text(pin[p, n, "pinnumber"], x - pinnumbercenteroffset, y + pinlinelength - pinnumberoffset, 3, 0, pinnumbertextsize, "")
          if(neg) circle(x, y + pinlinelength - negradius, negradius, 4, "")
        } else if(p == 2) {
          line(x - (neg ? pinlinelength -2 * negradius :pinlinelength), y, x, y, 4, "")
          text(pin[p, n, "pinname"], x - pinlinelength - pintextoffset, y - pintextcenteroffset, 0, 1, pintextsize, "")
          text(pin[p, n, "pinnumber"], x - pinlinelength + pinnumberoffset , y - pinnumbercenteroffset, 0, 0, pinnumbertextsize, "")
          if(neg) circle(x - pinlinelength + negradius, y, negradius, 4, "")
        } else if(p == 3) {
          line(x, y - (neg ? pinlinelength -2 * negradius :pinlinelength), x, y, 4, "")
          text(pin[p, n, "pinname"], x + pintextcenteroffset, y - pinlinelength - pintextoffset, 1, 1, pintextsize, "")
          text(pin[p, n, "pinnumber"], x - pinnumbercenteroffset, y - pinlinelength + pinnumberoffset, 3, 1, pinnumbertextsize, "")
          if(neg) circle(x, y - pinlinelength + negradius, negradius, 4, "")
        }
        if(trig) trigger(x, y, 4, p)
      }

      pin[p, lastcoord] = (p == 0 || p == 2) ? y : x
      if(p == 0 || p == 2) y += pd
      else x += pd
    }
    dbg("direction " p " last coord=" pin[p, lastcoord])
  }
  box(symbolx1, symboly1, symbolx2, symboly2, 4, "")

  for(l = 0; l < label["n"]; l++) {
    dbg("label: " l " : " label[l])
    labx = (symbolx1 + symbolx2) / 2 - length(label[l]) * labelcharspacing /2 
    laby = (symboly1 + symboly2) / 2 + l * labelspacing - label["n"] * labelspacing / 2
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
