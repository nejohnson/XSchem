#!/usr/bin/awk -f
# script to rescale MOS W, L in a sch file



FNR == 1 {
    if (_filename_ != "")
        endfile(_filename_)
    _filename_ = FILENAME
    #print "processing: " FILENAME >"/dev/stderr"
    beginfile(FILENAME)
}

END  { endfile(_filename_) }

###### begin user code ########################

{
  for(i=1;i<=NF;i++)
  {
    if($i ~ /[wWlL][a-zA-Z_]*=[0-9.]+[uU]$/) {
      found=1
      split($i, arr, "=")
      sub(/[uU]$/, "", arr[2])
      $i= arr[1] "=" 2*arr[2] "u"
      
    }
  }
}

###### end  user code  ########################


{
 a[lines++] = $0
}

function beginfile(f)
{
 lines=0
 found=0
}

function endfile(f)
{
 if(found)  {
   print "patching: " f >"/dev/stderr"
   for(i=0;i<lines;i++)
   {
    print a[i] > f
   }
   close(f)
 }
}
