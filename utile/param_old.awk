#!/bin/sh
# 

mawk '

/^\.*param/{
 param[$2]=$3
}

{
 for(nf=2;nf<=NF;nf++)
 {
  if($i ~ /^;/) break
  if($nf in param) $nf=param[$nf]
 }  
}   

{ print }

' $@
