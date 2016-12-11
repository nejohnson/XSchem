#!/usr/bin/awk -f



FNR == 1 {
    if (_filename_ != "")
        endfile(_filename_)
    _filename_ = FILENAME
    #print "processing: " FILENAME >"/dev/stderr"
    beginfile(FILENAME)
}

END  { endfile(_filename_) }

###### begin user code ########################

/\<snprintf\>/{
  found=1
  gsub(/\<snprintf\>/,"my_snprintf")
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

