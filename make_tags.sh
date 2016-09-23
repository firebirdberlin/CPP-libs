#! /bin/bash

# Attention: Since we want to use this script within a makefile, that should
# not exit on failure, we return always 0 (=success)
if [ "`which geany`" == "" ]; then
	echo "geany not found. Exit!";
	exit 0;
fi;

rm -if mylib.cpp.tags
touch mylib.cpp.tags
geany -Pg mylib.cpp.tags *.h *.hpp

mkdir -p ~/.config/geany/tags/
cp mylib.cpp.tags ~/.config/geany/tags/
