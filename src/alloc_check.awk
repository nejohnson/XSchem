#!/bin/sh
mawk '

# my_calloc(): allocating 136154256 ,  41600 bytes
# my_strdup(): duplicated 134973960 string library/w9el/chiptop.sch
# my_malloc(): allocating 138401376 , 256 bytes
# my_realloc(): reallocating 0 --> 138307456 to 256 bytes
# my_free():  freeing 138401904


{
 print
}
/^my_realloc\(\):/{
  if($3 !=$5) {
   delete allocated[$3]
   delete allocator[$3]
   allocated[$5]++
   allocator[$5]=allocator[$5] " realloc"
  }
}

/^my_strdup\(\):/{ 
 allocated[$3]++
 allocator[$3]=allocator[$3] " strdup"
 string[$3] = $NF
}
/^my_malloc\(\):/{ 
 allocated[$3]++
 allocator[$3]=allocator[$3] " malloc"
}
/^my_calloc\(\):/{ 
 allocated[$3]++
 allocator[$3]=allocator[$3] " calloc"
}


/my_free\(\):/{
 if( !($3 in allocated) ) {
   print ">>>>>>>>>>>>>> freeing unknown pointer: " $3
 }
 else {
  delete allocated[$3]
  delete allocator[$3]
 }
  
}

END{
 for(i in allocated) 
   print ">>>>>>>>>>>>>> unfreed pointer: ", i, "allocated " allocated[i] " times by  " allocator[i] ": " string[i]
}
' $@
