

ARCH=$(shell uname -m)

all: mymath

CFLAGS=`mylibs-config --cldflags` -fopenmp -std=gnu++0x -pedantic -Wextra -Wall

mymath: mymath.cpp
	g++ mymath.cpp -o mymath.$(ARCH) $(CFLAGS) $(OPT)

dcm: dicom_transform.cpp
	g++ dicom_transform.cpp -o dcm.$(ARCH) $(CFLAGS)  $(OPT) `Magick++-config --cppflags` `Magick++-config --libs`

clean:
	rm -f mymath.$(ARCH) dcm.$(ARCH) *.o
