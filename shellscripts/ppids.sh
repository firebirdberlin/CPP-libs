#! /bin/bash

# list all parent pids of a process

if [[ $1 = *[[:digit:]]* ]]; then
 PID=$1
else
 PID=`pidof -s $1` # single shot
fi

#PID=$1

ps -p $PID -o ppid | sed 1d | sed 1d;

IS_CONDOR=0
while [ $PID -gt 1 ]; do
	cmd=`ps -p $PID -o cmd  | sed 1d;`
	PID=`ps -p $PID -o ppid | sed 1d;`

	echo "$PID: $cmd"
done;

if [ "$cmd" == "/usr/local/sbin/condor_master" ]; then
 IS_CONDOR=1
fi;

if [[ $IS_CONDOR -eq 1 ]]; then
	echo "condor process : $IS_CONDOR";
fi
