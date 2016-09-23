

ARCH=$(shell uname -m)

all: exists

exists: exists.cpp
	g++ $(OPT) exists.cpp -o exists.$(ARCH) `mylibs-config --cldflags` -fopenmp

clean:
	rm -f exists.$(ARCH) *.o
