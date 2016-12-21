#!/usr/bin/awk -f
#
#  File: make_sym.awk
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
#

BEGIN{
 if( ARGV[1] ~ /^[1-9][0-9]*$/ )  {
   width=ARGV[1]+0
   ARGV[1]=""
 } else {
   width=150
 }
}



FNR == 1 {
    if (_filename_ != "") endfile(_filename_)
    _filename_ = FILENAME
    beginfile(FILENAME)
}
 
END  { endfile(_filename_) }
 
 
function beginfile(f)
{
 sym=name=f
 sub(/^.*\//,"",name)
 sub(/\.sch.*$/,"",name)
 sub(/\.sch.*$/,".sym",sym)
 print "**** symbol-izing: " sym "  ****"
  template="" ; start=0
  while((getline symline <sym) >0) {
   if(symline ~ /^G {/ ) start=1
   if(start) template=template symline "\n"
   if(symline ~ /\} *$/) start=0
  }
  close(sym)
  
   
 size=2.5
 space=20
 lwidth=20
 textdist=5
 labsize=0.2
 titlesize=0.3
 text_voffset=20
 lab_voffset=4
 ip=op=n_pin=0
 if(template=="") {
  printf "%s", "G {type=subcircuit\nformat=\"@name @pinlist @symname\"\n"  >sym
  printf "%s\n", "template=\"name=x1\""  >sym
  printf "%s", "}\n"  >sym
 } 
 else print template >sym
 print "T {@symname}" ,-length(name)/2*titlesize*30, -text_voffset*titlesize,0,0,
       titlesize, titlesize, "{}" >sym
 

}
 

{
 sig_type=""  #20070726    # "std_logic"
 verilog_type= ""  # 20070726   "wire" #09112003
 pin_label=""
 value=""
 generic_type=""
 
 if($0 ~ /^.*lab=/)
 {
  pin_label=$0
  sub(/^.*lab=/,"",pin_label)
  sub(/[ }].*$/,"",pin_label)
 }

 if($0 ~ /^.*verilog_type=/)  #09112003
 {
  verilog_type=$0
  sub(/^.*verilog_type=/,"",verilog_type)
  sub(/[}].*$/,"",verilog_type)
  sub(/ $/,"",verilog_type)
 }
 
 if($0 ~ /^.*sig_type=/)
 {
  sig_type=$0
  sub(/^.*sig_type=/,"",sig_type)
  sub(/[ }].*$/,"",sig_type)
 }
 
 if($0 ~ /^.*generic_type=/)
 {
  generic_type=$0
  sub(/^.*generic_type=/,"",generic_type)
  sub(/[}].*$/,"",generic_type)
  sub(/[a-zA-z0-9]+=.*$/,"",generic_type)	#03062002, allow spaces
  print "------------------------" $0 "-->" generic_type
 }

 if($0 ~ /^.*value=/)
 {
  value=$0
  if(value ~ /value="/) 
  {
   sub(/^.*value="/,"",value)
   value= "\"" substr(value,1, match(value, /[^\\]\"/)  ) "\""
  }
  else
  {
   sub(/^.*value=/,"",value)
   sub(/[ }].*$/,"",value)
  }
 }
# print "lab=" lab " sig_type=" sig_type " type=" type " value=" value
}




#   {
#    sig_type=""
#    for(i=7;i<=NF;i++)
#    {
#     if($i ~ /lab=/)
#     {
#      pin_label=$i
#      sub("lab=","",pin_label)
#      sub("}","",pin_label)
#     }
#     if($i ~ /sig_type=/)
#     {
#      sig_type=$i
#      sub("sig_type=","",sig_type)
#      sub("}","",sig_type)
#     }
#    }
#    if(sig_type=="") sig_type="std_logic"
#    #print $0 "--> " sig_type
#   }

/^C {.*generic_pin}/{
  type_pin[n_pin]=generic_type
  dir_pin[n_pin]="generic"
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection for sorting pins 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  n_pin++
  ip++
}

/^C {.*ipin}/{
  type_pin[n_pin]=sig_type
  verilog_pin[n_pin]=verilog_type
  dir_pin[n_pin]="ipin"
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  n_pin++
  ip++
}

/^C {.*[^i]opin}/{
  type_pin[n_pin]=sig_type
  verilog_pin[n_pin]=verilog_type
  dir_pin[n_pin]="opin"
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  n_pin++
  op++
}

/^C {.*iopin}/{
  type_pin[n_pin]=sig_type
  verilog_pin[n_pin]=verilog_type
  dir_pin[n_pin]="iopin"
  y_pin[n_pin] = $4+0 # y coordinate of pin 20140519
  index_pin[n_pin] = n_pin # one level indirection 20140519
  value_pin[n_pin]=value
  label_pin[n_pin] = pin_label
  n_pin++
  op++
}

function endfile(f) {

 n=ip;if(op>n) n=op
 if(n==0) n=1
 m=(n-1)/2
 y=-m*space
 x=-width
 print "T {@name}",-x-lwidth+5, y-space/2-8-lab_voffset,0,0,labsize, labsize,"{}" >sym
 print "L 4 " (x+lwidth) ,y-space/2,(-x-lwidth) , y-space/2,"{}" >sym
 print "L 4 " (x+lwidth) ,y+n*space-space/2,(-x-lwidth) , y+n*space-space/2,"{}" >sym
 print "L 4 " (x+lwidth) ,y-space/2,(x+lwidth) , y+n*space-space/2,"{}" >sym
 print "L 4 " (-x-lwidth) ,y-space/2,(-x-lwidth) , y+n*space-space/2,"{}" >sym


 hsort_key(index_pin, y_pin, n_pin)   # 20140519

 num_i = num_o = 0 #20140519

 for(ii=0;ii<n_pin;ii++)
 {
  i = index_pin[ii] # 20140519
  
  dir=dir_pin[i]
  value=value_pin[i]

  sig_type=type_pin[i]
  verilog_type=verilog_pin[i]
  vert = verilog_type ? (" verilog_type=" verilog_type) : ""
  vhdt = sig_type ? (" sig_type=" sig_type) : ""

  if(dir=="generic")
  {
   printf "B 3 " (x-size) " " (y+num_i*space-size) " " (x+size) " " (y+num_i*space+size) \
         " {name=" label_pin[i] " generic_type=" sig_type " " >sym
   if(value !="") printf "value=" value "}\n" >sym
   else printf "}\n" >sym
   print "L 4 " x,y+num_i*space,x+lwidth, y+num_i*space,"{}" >sym
   print "T {" label_pin[i] "}",x+lwidth+textdist,y+num_i*space-lab_voffset,0,0,labsize, labsize, "{}" >sym
   num_i++ # 20140519
  }
  if(dir=="ipin")
  {
   printf "B 5 " (x-size) " " (y+num_i*space-size) " " (x+size) " " (y+num_i*space+size) \
         " {name=" label_pin[i] vhdt vert " dir=in " >sym
   if(value !="") printf "value=" value "}\n" >sym
   else printf "}\n" >sym
   print "L 4 " x,y+num_i*space,x+lwidth, y+num_i*space,"{}" >sym
   print "T {" label_pin[i] "}",x+lwidth+textdist,y+num_i*space-lab_voffset,0,0,labsize, labsize, "{}" >sym
   num_i++ # 20140519
  }
  if(dir=="opin")
  {
   printf "B 5 " (-x-size) " " (y+num_o*space-size) " " (-x+size) " " (y+num_o*space+size) \
         " {name=" label_pin[i] vhdt vert " dir=out " >sym
   if(value !="") printf "value=" value "}\n" >sym
   else printf "}\n" >sym
   print "L 4 " (-x),(y+num_o*space),(-x-lwidth), (y+num_o*space),"{}" >sym
   print "T {" label_pin[i] "}",-x-lwidth-textdist,y+num_o*space-lab_voffset,0,1,labsize, labsize, "{}" >sym
   num_o++ # 20140519
  }
  if(dir=="iopin")
  {
   printf "B 5 " (-x-size) " " (y+num_o*space-size) " " (-x+size) " " (y+num_o*space+size) \
         " {name=" label_pin[i] vhdt vert " dir=inout " >sym
   if(value !="") printf "value=" value "}\n" >sym
   else printf "}\n" >sym
   print "L 7 " (-x),(y+num_o*space),(-x-lwidth), (y+num_o*space),"{}" >sym
   print "T {" label_pin[i] "}",-x-lwidth-textdist,y+num_o*space-lab_voffset,0,1,labsize, labsize, "{}" >sym
   num_o++ # 20140519
  }
 }
 close(sym)
}

function hsort_key(array, ra, n,      i, j, c, root, temp)
{
    for (i = 1; i < n; i++)
    {
        c = i
        do
        {
            root = int((c - 1) / 2)
            if (comp(ra[root], ra[c]))   # to create MAX ra array
            {
                temp = ra[root]
                ra[root] = ra[c]
                ra[c] = temp
                temp = array[root]
                array[root] = array[c]
                array[c] = temp
            }
            c = root
        } while (c != 0)
    }
    for (j = n - 1; j >= 0; j--)
    {
        temp = ra[0]
        ra[0] = ra[j]    # swap max element with rightmost leaf element
        ra[j] = temp
        temp = array[0]
        array[0] = array[j]
        array[j] = temp
        root = 0
        do
        {
            c = 2 * root + 1    # left node of root element
            if (comp(ra[c], ra[c + 1]) && c < j-1)
                c++
            if (comp(ra[root], ra[c]) && c<j)    # again rearrange to max ra array
            {
                temp = ra[root]
                ra[root] = ra[c]
                ra[c] = temp
                temp = array[root]
                array[root] = array[c]
                array[c] = temp
            }
            root = c
        } while (c < j)
    }
}

function comp(a,b)
{
  return a<b
}

 
