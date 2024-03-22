#!/bin/sh
#
# Launch V1290N program
#
#
run=$1
neventmax=$2
pulser_freq=$3
gate_freq=$4
mydate=$5
mytime=$6
comments=$7
echo 'Starting V1290N-Read with parameters of ' ${run} ${neventmax} ${pulser_freq}
echo 'Mydate = ' ${mydate}
echo 'Mytime = ' ${mytime}
echo 'Comments = ' ${comments}
exedir=${HOME}/work/KU-DAQ/V1290N
outdird=${HOME}/My_Data_Taking/MultiHitTDC_DataTaking/${mydate}
outdirdr=${HOME}/My_Data_Taking/MultiHitTDC_DataTaking/${mydate}/Run${run}

echo 'Testing ' ${outdird}
if [ -d ${outdird} ]
then
   echo ${outdird} 'directory already exists'
else
   echo ${outdird} 'does not exist'
   mkdir ${outdird}
fi

echo 'Testing ' ${outdirdr}
if [ -d ${outdirdr} ]
then
   echo ${outdirdr} 'directory already exists'
else
   echo ${outdirdr} 'does not exist'
   mkdir ${outdirdr}
fi

cd ${outdirdr}
pwd

echo ${comments} > comments.out
date
${exedir}/start_time.py >start.out
${exedir}/V1290N-Read ${neventmax} ${pulser_freq} ${gate_freq} >V1290N-Read.out
${exedir}/stop_time.py >stop.out

echo 'Data taking run completed '
date

# Tidy up by killing the python process 
cd ${exedir}
${exedir}/pythonkill.sh
exit
