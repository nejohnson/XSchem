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



{
  replace_pattern("prepared_hash_components", "prepared_hash_instances")
  replace_pattern("hash_components", "hash_instances")
  replace_pattern("componentdelete", "instdelete")
  replace_pattern("componentinsert", "instinsert")
  replace_pattern("componententry", "instentry")
  replace_pattern("componenttable", "insttable")
  replace_pattern("del_component_table", "del_inst_table")
  replace_pattern("hash_component", "hash_inst")
}


function replace_pattern(old, new)
{
  if($0 ~ old) {
    found=1
    gsub(old, new)
  }
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

