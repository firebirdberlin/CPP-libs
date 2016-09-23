## Attention This library should be completely independent of the Simulation
## 			 project so that it can be included in different programs.

HPPFILES = $(shell ls *.hpp)
CC = g++

prefix =$(shell cat prefix.log)
prefix_re = $(shell echo ${prefix} | sed 's/\//\\\//g')

ifdef USE_GCOV
	#ifeq (echo "$CFLAGS" | grep "-fprofile-arcs -ftest-coverage",)
		CFLAGS += -fprofile-arcs -ftest-coverage
	#endif
endif

test: libSymbolic.a
	@echo "====================== RUNNING Test ========================="
	$(CC) $(CFLAGS) $(OPT) test_Symbolic.cpp ../libSymbolic.a -o test_Symbolic
	./test_Symbolic

libSymbolic.a:
	make -C ../

%.o: %.cpp $(HPPFILES)
	$(CC) $(CFLAGS) $(OPT) -c $<

clean:
	@echo "======================== Clean Up ==========================="
	rm -rif *.o test_Symbolic
