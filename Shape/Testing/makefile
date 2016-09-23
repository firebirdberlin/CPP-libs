#include ../../make_includes.mk

CPPFILES = $(shell ls *.cpp)
HPPFILES = $(shell ls *.hpp)
OFILES   = $(CPPFILES:%.cpp=%.o)
CC = g++

CFLAGS :=-Wall -Wextra `mylibs-config --cflags` -fopenmp

LNK = `mylibs-config --ldflags`

test: libShape.a
	@echo "====================== RUNNING Test ========================="
	$(CC) $(CFLAGS) test_Shape.cpp ../libShape.a -o test_Shape $(LNK)
	./test_Shape

libShape.a:
	make -C ../

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $(OPT) -c $<

clean:
	@echo "======================== Clean Up ==========================="
	rm -rif *.o *.gcov *.gcno *.gcda test_Shape
