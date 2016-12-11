#!/usr/bin/awk -f
# 


/^[ \t]*;/{ next }
{ sub(/;/," ;",$0) }
{ print }


