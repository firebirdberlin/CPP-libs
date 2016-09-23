#! /bin/bash

# how do we reach the remote machine ? 
REMOTE="cluster"
	
if [ $# -lt 1 ]; then
	echo -e "\n\
	Syncs a list of files with a remote machine assuming \n\
	the directory structure is the same on the target machine.\n\
\n\
	usage : sync.sh [files]\n\
\n\
	note  : * The target machine is addressed as $REMOTE. 
 "
	exit -1;
fi

# current dir
SRC=${PWD}
# strip home directory from current dir
FLD=~${PWD#$HOME}
# target dir
TGT=${REMOTE}:~${PWD#$HOME}

# speak to the user
echo "source : $SRC"
echo "target : $TGT"
echo "files  : $@"

# strip trailing slashes
FILES=
for i in $@; do
	i=${i%\/}
	FILES+="$i "
done;

# create the target folder if needed
ssh ${REMOTE} "if [[ ! -d ${FLD} ]];then mkdir -p ${FLD}; fi;"

# start syncing 
echo " $ rsync -rv $@ ${TGT}/"
rsync --progress -rv ${FILES} ${TGT}/

