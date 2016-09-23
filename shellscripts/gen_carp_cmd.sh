#! /bin/bash

######################### S e t t i n g s ##############################

CARP_CMD="carp.petsc.mpi"

PARFILE_PREFIX="parameters_"
PARFILE_SUFFIX=".par"

JOB_CMD_TEMPLATE="$MYLOCAL/share/job.cmd.template"

JOB_MAX_NUM_NODES=47
JOB_MAX_NUM_PROCS=8

########################################################################

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


function check(){
	if [[ $? -ne  0 ]] ; then
		echo "Error code was $?";
		exit -1;
	fi;
}

function run(){
	echo "$ $1";
	eval $1;
	check
}

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

#process remaining args
# the remaining args should only be *.par files
#echo $# $@
if [ $# -lt 1 ];then
	usage;
fi

if [ -z $NUM_NODES ]; then
	echo -n "How many nodes              (default '1') ? ";
	read NUM_NODES;
	if [ -z $NUM_NODES ]; then NUM_NODES=1; fi
fi

if [ $NUM_NODES -gt ${JOB_MAX_NUM_NODES} ]; then
	echo "[WW] Attention NUM_NODES(${NUM_NODES}) exceeds allowed maximum (${JOB_MAX_NUM_NODES})."
	NUM_PROCS=${JOB_MAX_NUM_PROCS};
fi

if [ -z $NUM_PROCS ]; then
	echo -n "How many processors per node (default '6') ? ";
	read NUM_PROCS;
	if [ -z $NUM_PROCS ]; then NUM_PROCS=6; fi
fi

if [ $NUM_PROCS -gt ${JOB_MAX_NUM_PROCS} ]; then
	echo "[WW] Attention NUM_PROCS(${NUM_PROCS}) exceeds allowed maximum (${JOB_MAX_NUM_PROCS})."
	NUM_PROCS=${JOB_MAX_NUM_PROCS};
fi

if [ -z $WALLTIME ]; then
	echo -n "Walltime [HH:MM:SS]   (default '04:00:00')  ? ";
	read WALLTIME;
	if [ -z $WALLTIME ]; then WALLTIME="04:00:00"; fi
fi;

echo;

for i in $@; do
	if [ ! -f $i ]; then
		echo "Error: $i is no file !";
		continue;
	fi

	# gen folder name for output
	cmd="echo $i | sed 's/${PARFILE_PREFIX}//' | sed 's/${PARFILE_SUFFIX}//'"
	OUT_DIR=`eval $cmd`
	CMD_FILE="job_${OUT_DIR}.cmd"

	# current working dir escaped for sed
	CWD="${PWD//\//\\/}"

	# finally create the command
	cmd="\
	sed 's/%CWD%/${CWD}/g' $JOB_CMD_TEMPLATE \
	| sed 's/%CMD%/${CARP_CMD} +F $i -simID ${OUT_DIR}/g'\
	| sed 's/%NUM_NODES%/${NUM_NODES}/g'\
	| sed 's/%NUM_PROCS%/${NUM_PROCS}/g'\
	| sed 's/%WALLTIME%/${WALLTIME}/g'\
	"
	#echo $cmd

	hline
	center $CMD_FILE
	hline
	# print the cmd file and write it
	eval $cmd | tee $CMD_FILE
	hline

	cmd="msub ${CMD_FILE}"
	if [ -z $AUTOSUBMIT ]; then
		echo -n "Run $ ${cmd} ? (y/n) ";
		read STARTCMD;
		if [[ "$STARTCMD" == "y" ]]; then run "$cmd"; fi
	elif [[ "$AUTOSUBMIT" == "True" ]]; then
		run "$cmd";
	elif [[ "$AUTOSUBMIT" == "False" ]]; then
		echo -e "For auto submission run\n $ $cmd \n";
	fi;

done
