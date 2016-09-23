#! /bin/bash

for i in $@; do
	if [[ ! -f $i ]]; then continue; fi;
	gnuplot $i;
done;

for k in *.eps; do
	b=`basename $k .eps`
	epstopdf $k;
	rm $k
	pdfcrop ${b}.pdf ${b}.pdf
done
