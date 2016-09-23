
ARCH=$(shell uname -m)

OPT=-DDEBUG -g

LIB:=-L../lib/ -lmylib
INC:=-I..

MYLIBS_FOUND = $(shell which mylibs-config)
ifneq ($(MYLIBS_FOUND), )
	LIB = `mylibs-config --ldflags`
	INC = `mylibs-config --cflags` -fopenmp
endif

GSL_FOUND = $(shell which gsl-config)
ifneq ($(GSL_FOUND), )
	LIB += `gsl-config --libs` -DHAVE_GSL
endif

all: myIniFiles Plane RandomNumber myalgorithm Line_demo gen_line point lists xydata gipl gipldo

%:	libmylib.a %.cpp
	g++ $(OPT) $@.cpp -o $@ -Wall $(INC) $(LIB)

myIniFiles: myInifiles_demo.cpp
	g++ $(OPT) myInifiles_demo.cpp -o myInifiles_demo -Wall $(INC) $(LIB)
	g++ $(OPT) myinifiles.cpp -o myinifiles -Wall $(INC) $(LIB)
	g++ $(OPT) myIniFiles_test.cpp -o myIniFiles_test -Wall $(INC) $(LIB)


Plane:	Plane.cpp
	g++ $(OPT) Plane.cpp -o Plane -Wall $(INC) $(LIB)

RandomNumber: test_RandomNumber.cpp
	g++ $(OPT) test_RandomNumber.cpp -o RandomNumber -Wall $(INC) $(LIB)

myalgorithm: test_myalgorithm.cpp
	g++ $(OPT) test_myalgorithm.cpp -o test_myalgorithm -Wall $(INC) $(LIB) -fopenmp
	
point: libmylib.a Point_demo.cpp
	g++ $(OPT) Point_demo.cpp -o Point_demo -Wall $(INC) $(LIB)

lists: lists_demo.cpp
	g++ $(OPT) lists_demo.cpp -o lists_demo -Wall $(INC) $(LIB)
	./lists_demo

xydata: XYData_demo.cpp
	g++ $(OPT) XYData_demo.cpp -o XYData_demo -Wall $(INC) $(LIB)
	./XYData_demo

gipl: gipl_demo.cpp
	g++ $(OPT) gipl_demo.cpp -o gipl_demo -Wall $(INC) $(LIB)
	./gipl_demo
	g++ $(OPT) gipl_sphere.cpp -o gipl_sphere -Wall $(INC) $(LIB)

gipldo:gipldo.cpp ../gipl.cpp
	g++ $(OPT) gipldo.cpp -lmylib -o gipldo.$(ARCH) -fopenmp $(INC) $(LIB)

libmylib.a:
	cd .. && make

clean:
	rm -vf *.o myInifiles_demo myIniFiles_test RandomNumber lists_demo Point_demo gipl_demo gipl_sphere gipldo.$(ARCH)
