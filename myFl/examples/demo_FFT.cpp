//		./examples/demo_FFT.cpp
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

#include <mylibs/myinifiles.hpp>
#include <myFl/Fl_Gnuplot.hpp>
#include <mylibs/FastFourierTransformation.hpp>

using namespace std;
using namespace mylibs;

//#define N 400
//#define N 512

Fl_Value_Slider *N;
Fl_Gnuplot *graph1= 0, *graph2= 0, *graph3 = 0;
Fl_Value_Slider *freq1 = 0,*freq2 = 0,*freq3 = 0,*freq4 = 0;

void play(int f __attribute__((unused)) = 1) {
	const int &n = (int)N->value();
	double *xx = new double[n];
	double *yy = new double[n];

	int f1 = freq1->value();
	int f2 = freq2->value();
	int f3 = freq3->value();
	int f4 = freq4->value();
	for (int i = 0; i < n; i++) {
		xx[i] = i;
		yy[i] =       sin(2.* M_PI/n * i*f1)
		            + sin(2.* M_PI/n * i*f2)
		            + sin(2.* M_PI/n * i*f3)
		            + sin(2.* M_PI/n * i*f4);
//~ 	  yy[i] =  (i < f1) ?  1. : 0.  ;
	}


	graph1->set_2d_data(xx,yy,n,"combination of sin(x)");
	graph1->xlabel("t / a.u.");
	graph1->ylabel("f(x)");
	graph1->title("signal");

	FastFourierTransformation fft(yy, n, false);
	fft.Compute();

	double       *reals = new double[n/2];
	double       *imags = new double[n/2];
	double       *power = new double[n/2];
	double *frequencies = new double[n/2];
	int k = 0;
	k = fft.GetRealCoeff(reals);
	k = fft.GetImagCoeff(imags);
	fft.sampling_rate(n);
	k = fft.GetPowerSpectrum(frequencies, power);

//  graph2->set_2d_data(frequencies,reals,k,"real,");
//  graph2->add_2d_data(frequencies,imags,k,"imaginary parts");
	graph2->set_2d_data(frequencies,power,k,"power spectrum");

	delete[] reals;
	delete[] imags;
	delete[] power;
	delete[] frequencies;

	graph2->xlabel("frequency / 1/T");
	graph2->ylabel("magnitude");
	graph2->title("transform");
//  graph2->plot_style("impulses");
	double *inverse = new double[n];
	fft.CutFrequenciesAbove(25.);
	k = fft.ComputeInverse(inverse);

	graph3->set_2d_data(xx, inverse, k);
	delete[] inverse;

	graph3->xlabel("t / a.u.");
	graph3->ylabel("f(x)");
	graph3->title("signal computed by inverse transformation with LowPassFilter at 20 Hz.");

	graph1->hide();
	graph2->hide();
	graph3->hide();
	graph1->show();
	graph2->show();
	graph3->show();

	delete[] xx;
	delete[] yy;

}

static void freq_cb(Fl_Widget*, void*userdata) {
	Fl_Value_Slider *s __attribute__ ((unused)) = (Fl_Value_Slider*) userdata;
	play();
}

int main(int argc, char **argv) {

	Fl_Double_Window mainWin(600, 700, "FFT demo");
	N      = new Fl_Value_Slider(20, 5, 560, 15);

	N->type(FL_HOR_SLIDER);
	N->tooltip("Sampling rate : N. To demonstrate the effects of zero padding \
choose a value that is not a power of 2.");
	N->when(FL_LEAVE);
	N->callback(freq_cb, freq1);
	N->bounds(1, 1024);
	N->value(512.);
	N->step(1.);

	graph1 = new Fl_Gnuplot(20,20,560, 200);
	graph2 = new Fl_Gnuplot(20,250,560, 200);
	graph3 = new Fl_Gnuplot(20,470,560, 200);

	freq1 = new Fl_Value_Slider(20, 208, 560, 10);
	freq2 = new Fl_Value_Slider(20, 218, 560, 10);
	freq3 = new Fl_Value_Slider(20, 228, 560, 10);
	freq4 = new Fl_Value_Slider(20, 238, 560, 10);

	freq1->type(FL_HOR_SLIDER);
	freq2->type(FL_HOR_SLIDER);
	freq3->type(FL_HOR_SLIDER);
	freq4->type(FL_HOR_SLIDER);

	freq1->tooltip("Set the frequency of the signal 1");
	freq2->tooltip("Set the frequency of the signal 2");
	freq3->tooltip("Set the frequency of the signal 3");
	freq4->tooltip("Set the frequency of the signal 4");

	freq1->when(FL_LEAVE);
	freq2->when(FL_LEAVE);
	freq3->when(FL_LEAVE);
	freq4->when(FL_LEAVE);

	freq1->callback(freq_cb, freq1);
	freq2->callback(freq_cb, freq2);
	freq3->callback(freq_cb, freq3);
	freq4->callback(freq_cb, freq4);
	freq1->bounds(1, N->value()/2);
	freq2->bounds(1, N->value()/2);
	freq3->bounds(1, N->value()/2);
	freq4->bounds(1, N->value()/2);

	freq1->value(1.);
	freq2->value(10.);
	freq3->value(20.);
	freq4->value(30.);

	freq1->step(1);
	freq2->step(1);
	freq3->step(1);
	freq4->step(1);
	mainWin.add_resizable(*graph2);
	mainWin.end();

	play();

	mainWin.show(argc, argv);
	return Fl::run();

	delete graph1;
	delete graph2;
	delete graph3;

	delete N;
	delete freq1;
	delete freq2;
	delete freq3;
	delete freq4;
}
