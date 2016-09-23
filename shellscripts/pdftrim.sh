#! /bin/bash

## Trims images using ImageMagicks in the style as we do it using
## \includegraphics
##
## \includegraphics[width=.47\columnwidth ,
##					clip=true ,trim=70pt 140pt 80pt 100pt
##					]{img/axial0017.pdf}


function run(){
	echo "\$ $1"
	eval $1
}

function cal(){
	RES=`calc -p $1`
	echo ${RES}
}

function die(){
	echo $1;
	exit -1;
}

EXT=${1#*.} 	# schneidet Anfang 'dateiname.' weg
BASE=${1%.$EXT} # schneidet Endung '.dat' weg

if [[ $# -le 1 ]]; then
	die "usage: $0 <file>.pdf L [B [R [T]]]"
fi;

if [[ "$EXT" != "pdf" ]]; then
	die "Error : File extension must be pdf !";
fi;

in=$1
out=${BASE}_trimmed.${EXT}

shift 1;



	# crop
L=0 # left
B=0 # bottom
R=0 # right
T=0 # top

if [[ $# -ge 1 ]]; then
	L=$1; shift 1;
fi;

if [[ $# -ge 1 ]]; then
	B=$1; shift 1;
fi;

if [[ $# -ge 1 ]]; then
	R=$1; shift 1;
fi;

if [[ $# -ge 1 ]]; then
	T=$1; shift 1;
fi;

# get width and height of the original image
W=`pdfinfo $in | sed -n 's/Page size:[ ]*\([0-9]*\) x [0-9]*.*/\1/p'`
H=`pdfinfo $in | sed -n 's/Page size:[ ]*[0-9]* x \([0-9]*\).*/\1/p'`

RB=`cal "$W-$R"`
TB=`cal "$H-$T"`

# trim the image
run "pdfcrop --bbox '$L $B $RB $TB' $in $out"
