

ARCH=$(shell uname -m)

all: front_width mean_value linleastsquares run

front_width: front_width.cpp
	g++ front_width.cpp -o front_width.$(ARCH) `mylibs-config --cldflags` -fopenmp

mean_value: mean_value.cpp
	g++ mean_value.cpp -o mean_value.$(ARCH) `mylibs-config --cldflags` -fopenmp

linleastsquares: linleastsquares.cpp
	g++ linleastsquares.cpp -o linleastsquares.$(ARCH) `mylibs-config --cldflags` -fopenmp

run: front_width.$(ARCH)
	./front_width.$(ARCH) -i func.dat

clean:
	rm -f front_width.$(ARCH) *.o
