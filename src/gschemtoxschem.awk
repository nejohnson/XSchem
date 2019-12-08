#!/usr/bin/awk -f

BEGIN{
  pinseq = 0
  halfpinsize=20
  print "G {type=symbol}"
  print "V {}"
  print "S {}"
  print "E {}"
}

/^L/{
  x1 = $2
  y1 = $3
  x2 = $4
  y2 = $5
  print "L 4", order(x1, y1, x2, y2), "{}"
}

/^T/{
  xt = $2
  yt = $3
  size = $5 / 5
  angle = $8
  align = $9
  nlines = $10
  text = ""
  value = 0
  for(i = 1; i<= nlines; i++) {
    getline
    if(length($0) > value) value = length($0)
    if(text != "") text = text "\n"
    text = text $0
  }
  flip = 0
  if(align == 6 || align == 7 || align == 8) flip = 1
  if(align == 0 || align == 3 || align == 6) {
    yt += size*50
  }
  if(align == 1 || align == 4 || align == 7) {
    yt += size*25
  }
  if(align == 3 || align == 4 || align == 5) {
    xt -= size*17*value
  }
  print "T {" text "} " xt " " (-yt) " " int(angle/90) " " flip " " size " " size " {}"
} 
   
/^B/{
  x1 = $2
  y1 = $3
  x2 = x1 + $4
  y2 = y1 + $5
  print "L 4", x1, -y1, x2, -y1, "{}"
  print "L 4", x2, -y2, x2, -y1, "{}"
  print "L 4", x1, -y2, x2, -y2, "{}"
  print "L 4", x1, -y2, x1, -y1, "{}"
}
/^V/{ #circle
  print "A 4", $2, -$3, $4, 0, 360, "{}"
}
# P 900 100 750 100 1 0 0
/^P/{
  print "L 4 " order($2, $3, $4, $5) " {}"
  if($8 == 0) {
    pin = "B 5 " $2-halfpinsize " " (-$3-halfpinsize) " " $2+halfpinsize " " (-$3+halfpinsize) 
  } else {
    pin = "B 5 " $4-halfpinsize " " (-$5-halfpinsize) " " $4+halfpinsize " " (-$5+halfpinsize) 
  }
  process_attributes()
  ++pinseq
}

function order(x1, y1, x2, y2,       tmp) 
{
  y1 = -y1
  y2 = -y2
  if(x2<x1) { 
    tmp = x1; x1 = x2; x2 = tmp; tmp = y1; y1 = y2; y2 = tmp
  } else if(x2 == x1 && y2 < y1) {
    tmp = y1; y1 = y2; y2 = tmp
  }
  return x1 " " y1 " " x2 " " y2
}

function rectorder(x1, y1, x2, y2,       tmp) 
{
  y1 = -y1
  y2 = -y2
  if(x2 < x1) { tmp=x1;x1=x2;x2=tmp}
  if(y2 < y1) { tmp=y1;y1=y2;y2=tmp}
  return x1 " " y1 " " x2 " " y2
}



function process_attributes(                i)
{
  getline
  if($1 == "{") {
    getline
  } else {
    return
  }
  
  text = ""
  while(1) {
    if($1 == "T") {
      xt = $2
      yt = $3
      color = $4
      size = $5/5
      visible = $6
      show = $7
      angle = $8
      align = $9
      nlines = $10
      # print "nlines=" nlines > "/dev/stderr"
      for(i=1; i<= nlines; i++) {
        getline
        if($1 ~ /pinnumber/ || $1 ~ /pinlabel/ || $1 ~/pintype/) {
          if(text != "") text = text "\n"
          sub(/pinlabel/, "name", $0)
          sub(/pintype/, "dir", $0)
          text = text $0
          value = attr = $0
          sub(/=.*/, "", attr);
          sub(/.*=/,"", value)
          attr = "@#" pinseq ":" attr  
          flip =0
          if(align == 6 || align == 7 || align == 8) flip = 1 
          if(align == 0 || align == 3 || align == 6) {
            yt += size*50
          }
          if(align == 1 || align == 4 || align == 7) {
            yt += size*25
          }
          if(align == 3 || align == 4 || align == 5) {
            xt -= size*17*length(value)
          }
          if(attr ~ /pinnumber/ || attr ~ /name/)  {
            print "T {" attr "} " xt " " (-yt) " " int(angle/90) " " flip " " size " " size " {}"
          }
        }
      }
    }
    getline
    if($1 == "}") {
      printf pin " {" text "}\n"
      return
    }
  }

}
