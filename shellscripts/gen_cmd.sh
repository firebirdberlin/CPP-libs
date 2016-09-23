#! /bin/bash

######################### S e t t i n g s ##############################

JOB_CMD_TEMPLATE="$MYLOCAL/share/job.single.cmd.template"

JOB_MAX_NUM_NODES=47
JOB_MAX_NUM_PROCS=8

########################################################################

usage(){
	echo -e "usage: $0 [parfile.par]\n\
\n\
	parameters:\n\
		-n : # nodes (max. ${JOB_MAX_NUM_NODES})\n\
		-p : # processors / node (max ${JOB_MAX_NUM_PROCS})\n\
		-t : walltime\n\
		-a : auto submit the job\n\
		-N : No auto submission\n";
		exit -1;
}

# center text
## tput cols : gives the number of columns of the terminal
center(){
    [[ -t 1 ]] && wid=$(tput cols) || wid=80
    printf "%$(( (wid + ${#1}) / 2 ))s\n" "$1"
}

hline(){
	[[ -t 1 ]] && wid=$(tput cols) || wid=80
	tmp="for i in {1..$wid}; do	echo -n '-';	done;";
	eval $tmp;
	echo;
}

if [ $# -lt 1 ];then
	echo "usage: $0 [file containing the command]"
	exit -1;
fi

# give some cluster stats
if [ `hostname` == "e00073" ]; then
	showbf;
fi

while getopts ":n:p:t:aN" opt; do
  case $opt in
    a)
      echo "-a : auto job submission" >&2
      AUTOSUBMIT="True"
#      shift $((OPTIND-1)); OPTIND=1;
      shift 1; OPTIND=1;
      ;;
    N)
      echo "-N : no auto job submission" >&2
      AUTOSUBMIT="False"
#      shift $((OPTIND-1)); OPTIND=1;
      shift 1; OPTIND=1;
      ;;
    n)
      echo "-n : # nodes : $OPTARG" >&2
      NUM_NODES=$OPTARG
      shift $((OPTIND-1)); OPTIND=1;
      ;;
    p)
      echo "-p : # processors / node : $OPTARG" >&2
      NUM_PROCS=$OPTARG
      shift $((OPTIND-1)); OPTIND=1;
      ;;
    t)
      echo "-t : walltime : $OPTARG" >&2
      WALLTIME=$OPTARG
      shift $((OPTIND-1)); OPTIND=1;
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      usage;
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done


echo

if [ -z $NUM_NODES ]; then
	echo -n "How many nodes               (default '1') ? ";
	read NUM_NODES;
	if [ -z $NUM_NODES ]; then NUM_NODES=1; fi
fi

if [ $NUM_NODES -gt ${JOB_MAX_NUM_NODES} ]; then
	echo "[WW] Attention NUM_NODES(${NUM_NODES}) exceeds allowed maximum (${JOB_MAX_NUM_NODES})."
	NUM_PROCS=${JOB_MAX_NUM_PROCS};
fi

if [ -z $NUM_PROCS ]; then
	echo -n "How many processors per node (default '8') ? ";
	read NUM_PROCS;

	if [ -z $NUM_PROCS ]; then NUM_PROCS=8; fi
fi

if [ $NUM_PROCS -gt ${JOB_MAX_NUM_PROCS} ]; then
	echo "[WW] Attention NUM_PROCS(${NUM_PROCS}) exceeds allowed maximum (${JOB_MAX_NUM_PROCS})."
	NUM_PROCS=${JOB_MAX_NUM_PROCS};
fi

if [ -z $WALLTIME ]; then
	echo -n "Walltime [HH:MM:SS]   (default '10:00:00')  ? ";
	read WALLTIME;

	if [ -z $WALLTIME ]; then WALLTIME="10:00:00"; fi
fi

echo;

for i in $@; do
	FILE=$i
	if [ ! -f $i ]; then
		echo "Error: $i is no file !";
		continue;
	fi

	# gen folder name for output
	CMD_FILE="job_`basename $i .cmd`.cmd"


	# current working dir escaped for sed
	CWD="${PWD//\//\\/}"

	# finally create the command
	cmd="\
	sed 's/%CWD%/${CWD}/g' $JOB_CMD_TEMPLATE \
	| sed 's/%NUM_NODES%/${NUM_NODES}/g'\
	| sed 's/%NUM_PROCS%/${NUM_PROCS}/g'\
	| sed 's/%WALLTIME%/${WALLTIME}/g'\
	"
#	echo $cmd

	hline
	center $CMD_FILE
	hline
	eval $cmd | tee $CMD_FILE
	echo "echo \" \$ `cat $FILE` \"" | tee --append $CMD_FILE

	OLDIFS=$IFS;
	IFS=$'\n'
	for i in `cat $FILE`; do
		echo "$i | tee ${CMD_FILE}.\${PBS_JOBID}.tout" | tee --append $CMD_FILE
	done
	IFS=$OLDIFS
	#echo "`cat $FILE` | tee ${CMD_FILE}.\${PBS_JOBID}.tout" | tee --append $CMD_FILE

	echo "notify.sh \"Job \${PBS_JOBID} has finished.\"" | tee --append $CMD_FILE

	if [ "$AUTOSUBMIT" == "False" ]; then continue; fi;

	if [ "$AUTOSUBMIT" == "True" ]; then
			res=`msub $CMD_FILE`
			echo "\$ checkjob $res"
			checkjob $res
	else
		echo -n "$ msub $CMD_FILE ? (y/n) "
		read msub
		if [ $msub == "y" ]; then
			res=`msub $CMD_FILE`
			echo "\$ checkjob $res"
			checkjob $res
		fi
	fi
done
