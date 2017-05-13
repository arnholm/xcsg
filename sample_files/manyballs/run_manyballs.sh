#!/bin/bash
rm *.stl
for i in {1..16..1}
  do 
     start=`date +%s`
     xcsg --stl "manyballs_$i.xcsg" > "manyballs_$i.txt"
     end=`date +%s`
     runtime=$((end-start))
     echo "completed manyballs_$i.xcsg using " $runtime " sec".
  done
  
