## Attention This library should be completely independent of the Simulation
## 			 project so that it can be included in different programs.

CFLAGS = `mylibs-config --cflags`
LNK = `mylibs-config --ldflags`

SDL_CONFIG= $(shell sdl-config --cflags --libs)

CPPFILES = $(shell ls *.cpp)
HPPFILES = $(shell ls *.hpp)
OFILES   = $(CPPFILES:%.cpp=%.o)
CC = g++

prefix =$(shell cat prefix.log)
prefix_re = $(shell echo ${prefix} | sed 's/\//\\\//g')

test: libMCubes.a
	@echo "====================== RUNNING Test ========================="
	$(CC) $(CFLAGS) $(OPT) test_MCubes.cpp -lGLU -lGL -lmylib ../libMCubes.a -o test_MCubes $(SDL_CONFIG) $(LNK)
	./test_MCubes

libMCubes.a:
	make -C ../


%.o: %.cpp $(HPPFILES)
	$(CC) $(CFLAGS) $(OPT) -c $<

clean:
	@echo "======================== Clean Up ==========================="
	rm -rif *.o doxygen.log test_MCubes
	rm -rif doc latex
