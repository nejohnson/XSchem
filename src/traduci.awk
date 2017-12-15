#!/usr/bin/gawk -f



FNR == 1 {
    if (_filename_ != "")
        endfile(_filename_)
    _filename_ = FILENAME
    #print "processing: " FILENAME >"/dev/stderr"
    beginfile(FILENAME)
}

END  { endfile(_filename_) }

###### begin user code ########################
# B 17 6640 -67110 17800 -66620 {}
/^B 17/{
  found=1
  l1 = "L 17 " $3 " " $4 " " $5 " " $4 " {}\n"
  l2 = "L 17 " $5 " " $4 " " $5 " " $6 " {}\n"
  l3 = "L 17 " $5 " " $6 " " $3 " " $6 " {}\n"
  l4 = "L 17 " $3 " " $6 " " $3 " " $4 " {}"
  $0= l1 l2 l3 l4
}
###### end  user code  ########################


{
 __a[__lines++] = $0 
}

function beginfile(f)
{
 __lines=0
 found=0
}

function endfile(f,   i)
{
 if(found)  {
   print "patching: " f >"/dev/stderr"
   for(i=0;i<__lines;i++)
   {
    print __a[i] > f
   }
   close(f)
 }
} 

