CFLAGS = `myFl-config --cflags` -fopenmp -Wall -pedantic -Wextra -std=c++11
# -pedantic-errors
LIBS = `myFl-config --ldflags`

#DEPS = ../Fl_Gnuplot.o

FFT: demo_FFT.cpp demo2_FFT.cpp demo3_FFT.cpp
	#cd .. && make Fl_Gnuplot.o OPT=$(OPT)
	g++ $(OPT)  demo_FFT.cpp  $(DEPS) $(CFLAGS) $(LIBS) -o demo_FFT
	g++ $(OPT)  demo2_FFT.cpp  $(DEPS) $(CFLAGS)  $(LIBS) -o demo2_FFT
	g++ $(OPT)  demo3_FFT.cpp  $(DEPS) $(CFLAGS)  $(LIBS) -o FFT

signal_analyzer: signal_analyzer.cpp
	#cd .. && make Fl_Gnuplot.o OPT=$(OPT)
	g++ $(OPT) -fopenmp signal_analyzer.cpp  $(DEPS) $(CFLAGS) $(LIBS) -o signal_analyzer

clean:
	rm -f demo*_FFT FFT *.o
