#!/bin/bash
a=0

while [ $a -lt 100 ]
do
   ruby client.rb "./index.html" & echo $a
#    if [ $a -eq 10 ]
#    then
#       break
#    fi
   a=`expr $a + 1`
done
