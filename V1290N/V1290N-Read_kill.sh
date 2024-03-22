#!/bin/sh
#
# Kill running V1290N-Read process
#
#
if [ -f tmp.pid ]
then
   rm tmp.pid
fi
ps ux | awk '/V1290N-Read/ && !/awk/ {print $2}' >tmp.pid
read mypid <tmp.pid
echo 'mypid = ' ${mypid}

kill -9 ${mypid}
 
exit
