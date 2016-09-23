### name xvecgz.sh
##  - Extracts *.vec.gz files and saves all sensor traces as single data file.
##  - $1 should be a *.vec.gz file
##  - needs ncat that is able to cat every n'th line in a text file.

function run(){
	echo "$ $1";
	eval $1;
	res=$?;
	if [[ $res -ne 0 ]]; then
		echo "Error: return code was $res;"
		exit $res;
	fi
}

function die(){
	echo "$1";
	exit;
}

# simple approach
#extension=${1##*.}

# special approach
case "$1" in
    *.vec.gz)  extension="vec.gz" ;;
    *.vec)     extension="vec" ;;
    *)         die "$0 [file.vec.gz]";;
esac

if [ "$extension" == "vec.gz" ] && [ -f $1 ]; then
	run "gunzip $1";
fi
echo "ext: $extension"

unzipped=`basename $1 .gz`
base=`basename $unzipped .vec`

number_sensors=`cat $unzipped | sed -n 's/^.*contains \([0-9]*\) different.*/\1/p'`
number_commentlines=`grep "^#" $unzipped | wc -l`

max_id=`calc -p ${number_sensors}-1`

number_commentlines=`calc -p ${number_commentlines}+1`;

echo "number_sensors      = $number_sensors";
echo "number_commentlines = $number_commentlines";
run "mkdir ${base}"
if [[ -f $unzipped ]]; then
	for j in `seq 0 $max_id`; do
		run "ncat `calc -p ${number_commentlines}+$j` ${number_sensors} ${unzipped} > ${base}/${j}.dat;";
	done;
fi

if [ "$extension" == "vec.gz" ]; then
	run "gzip ${unzipped}";
	run "gzip -l $1";
fi
