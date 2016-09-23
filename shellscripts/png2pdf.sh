#! /bin/bash

for i in $@; do
	filename=${i#./} # strip leading ./
	#echo $filename
	extension="${filename##*.}"
	#echo $extension
	ff="${filename%.$extension}"
	#echo $ff
	if [[ "$extension" == "pdf" ]]; then continue; fi;
	echo "$ convert -density 300 $i $ff.pdf;"
	convert -density 300 $i $ff.pdf;
done;
