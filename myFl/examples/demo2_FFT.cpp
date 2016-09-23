//		./examples/demo2_FFT.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//
//		This program is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with this program; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.

#include <math.h>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Value_Slider.H>

#include <iostream>
#include <fstream>

#include <mylibs//myinifiles.hpp>
#include <mylibs//cmdline.hpp>
#include <myFl/Fl_Gnuplot.hpp>
#include <mylibs/FastFourierTransformation.hpp>

using namespace std;
using namespace mylibs;

Fl_Gnuplot *graph1= 0, *graph2= 0, *graph3 = 0;
Fl_Value_Slider *filter;

double *xx = 0;
double *yy = 0;
size_t N = 0;

double get_sampling_rate(double *xx, size_t n) {
	return ((double) n/(xx[n-1] - xx[0]));
}

void play(int f = 1) {

	graph1->set_2d_data(xx,yy,N,"data");
	graph1->xlabel("t / a.u.");
	graph1->ylabel("f(x)");
	graph1->title("signal");

	FastFourierTransformation fft(yy, N, false);
	fft.Compute();

	double       reals[(int)N/2];
	double       imags[(int)N/2];
	double       power[(int)N/2];
	double frequencies[(int)N/2];
	int k = 0;
	k = fft.GetRealCoeff(reals);
	k = fft.GetImagCoeff(imags);

	double srate = get_sampling_rate(xx, N);
	fft.sampling_rate(srate);
	fft.HighPassFilter(1);
	cout << "sampling rate ="<< srate << endl;
	k = fft.GetPowerSpectrumLinear(frequencies, power);

	graph2->set_2d_data(frequencies,power,k,"power spectrum");

	graph2->xlabel("frequency / 1/T");
	graph2->ylabel("magnitude");
	graph2->title("transform");
	graph1->command("set xrange [0:32]");
	graph2->command("set xrange [0:2]");
	graph3->command("set xrange [0:32]");
	double inverse[(int)N];

	//fft.CutFrequenciesAbove(25.);
	fft.LowPassFilter(filter->value());
	k = fft.ComputeInverse(inverse);

	graph3->set_2d_data(xx, inverse, k,NULL, Fl_Gnuplot::PLOT_WITH_IMPULSES);

	graph3->xlabel("t / a.u.");
	graph3->ylabel("f(x)");
	graph3->title("signal computed by inverse transformation with LowPassFilter.");

	graph1->hide();
	graph2->hide();
	graph3->hide();
	graph1->show();
	graph2->show();
	graph3->show();
}

static void freq_cb(Fl_Widget*, void*userdata) {
	Fl_Value_Slider *s = (Fl_Value_Slider*) userdata;
	play();
}

int main(int argc, char **argv) {

	Fl_Double_Window mainWin(600, 700, "FFT demo");

	cout << "Reading tip.dat" << endl;
	char line[1024];
	ifstream file("tip.dat") ;
	size_t cnt = 0;
	while( file.getline(line, 1024) ) cnt++;

	N = cnt;
	cout << "N = " << N<< endl;
	xx = new double[N];
	yy = new double[N];

	file.clear();              // forget we hit the end of file
	file.seekg(0, ios::beg);   // move to the start of the file

	cnt = 0;
	while( file.getline(line, 1024) ) {
		sscanf(line, "%lf %lf %*f", &xx[cnt], &yy[cnt]);
		cnt++;
	}
	file.close();

	graph1 = new Fl_Gnuplot(20,10,560, 200);
	graph2 = new Fl_Gnuplot(20,240,560, 200);
	graph3 = new Fl_Gnuplot(20,460,560, 200);

	filter = new Fl_Value_Slider(20, 670, 560, 15);

	filter->type(FL_HOR_SLIDER);
	filter->tooltip("Filter frequencies below the selected value in Hz.");
	filter->when(FL_LEAVE);
	filter->callback(freq_cb, filter);
	filter->bounds(0, 2.);
	filter->value(.35);
	filter->step(.01);

	mainWin.add_resizable(*graph2);
	mainWin.end();

	play();

	mainWin.show(argc, argv);
	return Fl::run();

	delete graph1;
	delete graph2;
	delete graph3;
}
