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

/^ *customfont *= *set_text_custom_font.*/{
  aa=$0
  sub(/[^ ].*/,"",aa)
  found=1
  $0 = aa "#ifdef HAS_CAIRO\n" $0 "\n" aa "#endif"
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

