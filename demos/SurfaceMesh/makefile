
#INC = $(HOME)/CPP-includes/gipl.cpp -I $(HOME)/CPP-includes/ -lz -fopenmp
INC 	= `mylibs-config --cflags` `myImg-config --cflags` `gsl-config --cflags`
LDFLAGS =  `mylibs-config --ldflags` `myImg-config --ldflags` `gsl-config --libs` -lz -fopenmp
#lmcubes

ARCH = $(shell uname -m)
PWD  = $(shell pwd)
WARN =  -Wall -Wextra

# -Werror -Wfatal-errors

ifndef OPT
	OPT  := -O3
endif

OPT += -fopenmp

CC=g++
CC=colorgcc

#-lmyMath -fopenmp
SRC = interpolate_deformation.cpp
EXE = interpolate_deformation


all: $(SRC)
	$(CC) $(WARN) $(OPT) -o $(EXE).$(ARCH) $(SRC) $(INC) $(LDFLAGS) $(OPT)

test: test_results.cpp
	$(CC) $(WARN) $(OPT) -o test.$(ARCH)  test_results.cpp $(INC) $(LDFLAGS) $(OPT)

clean:
	rm -if *.o $(EXE).$(ARCH)

install: all
	mkdir -p ~/local_$(ARCH)
	cd ~/local_$(ARCH)/bin &&  ln -sf $(PWD)/$(EXE).$(ARCH)

run: all
	./interpolate_deformation.x86_64 -f test.ini --source 7 --target 0
