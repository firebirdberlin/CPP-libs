#! /bin/bash

for i in $@; do
	filename=${i#./} # strip leading ./
	#echo $filename
	extension="${filename##*.}"
	#echo $extension
	ff="${filename%.$extension}"
	#echo $ff
	if [[ "$extension" == "pdf" ]]; then continue; fi;
	echo "inkscape $i --export-pdf=$ff.pdf"
	inkscape $i --export-pdf=$ff.pdf
	pdfcrop $ff.pdf $ff.pdf
done;
