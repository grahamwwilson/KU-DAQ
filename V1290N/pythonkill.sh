#!/bin/sh
#
# Kill running python program 
#
# a bit dangerous ...
#
#
if [ -f tmp.pid ]
then
   rm tmp.pid
fi
ps ux | awk '/python/ && !/awk/ {print $2}' >tmp.pid
read mypid <tmp.pid
echo 'mypid = ' ${mypid}

kill -9 ${mypid}
 
exit
