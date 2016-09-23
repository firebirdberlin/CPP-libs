
include ../../SWITCHES.mk

LIBS = `mylibs-config --ldflags` -lm -fopenmp
CFLAGS = -Wall `mylibs-config --cflags`

main: main.o
	g++ $(CFLAGS) $(DFLAGS) main.o $(LIBS) -o main

ellipse: generate_ellipse.o
	g++ $(CFLAGS) $(DFLAGS) generate_ellipse.o $(LIBS) -o generate_ellipse

%.o: %.cpp
	g++ $(CFLAGS)  $(DFLAGS) -c $<

mylib:
	make -C ../../

clean:
	rm -f *.o main

.PHONY: mylib
