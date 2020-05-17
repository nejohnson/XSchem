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

BEGIN{
}

function replace_pattern(old, new)
{
  if($0 ~ old) {
    gsub(old, new)
    found = 1
  }
}


###### end  user code  ########################


{
  replace_pattern("drawing window ID.*$", "drawing window ID ***Removed***")
  replace_pattern("top window ID.*$", "top window ID ***Removed***")
  replace_pattern("created dir.*$", "created dir ***Removed***")
  replace_pattern("undo_dirname.*$", "undo_dirname ***Removed***")
  replace_pattern("framewinID.*$", "framewinID ***Removed***")
  replace_pattern("resetwin.*$", "resetwin ***Removed***")
  replace_pattern("read_xschem_file.*$", "read_xschem_file ***Removed***")
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
   #print "patching: " f >"/dev/stderr"
   for(i=0;i<__lines;i++)
   {
    print __a[i] > f
   }
   close(f)
 }
} 

