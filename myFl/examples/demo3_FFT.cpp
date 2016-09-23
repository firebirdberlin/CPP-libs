//		./examples/demo3_FFT.cpp
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

double *xx	= 0;
double *yy	= 0;
size_t N	= 0;
size_t cols = 0;

double get_sampling_rate(double *xx, size_t n) {
	return ((double) (n-1)/(xx[n-1] - xx[0]));
}

void play(int f = 1) {
	for (int i = 0; i < cols; i++) {
		double **dd = 0;
		graph1->get_ydata(&dd);
		FastFourierTransformation fft(dd[i], N, false);
		fft.Compute();

		double xx   [(int)N/2];
		double reals[(int)N/2];
		double imags[(int)N/2];

		for (int j = 0; j < N/2; j++) xx[j] = j;
		double srate = get_sampling_rate(xx, N/2);
		fft.sampling_rate(srate);
		cout << "sampling rate ="<< srate << endl;

		int k = fft.GetRealCoeff(reals);
		if (i == 0)	graph2->set_2d_data(xx,reals,k);
		else		graph2->add_2d_data(xx,reals,k);

		k = fft.GetImagCoeff(imags);
		if (i == 0)	graph3->set_2d_data(xx,imags,k);
		else		graph3->add_2d_data(xx,imags,k);
	}

	graph1->xlabel("t / a.u.");
	graph1->ylabel("f(x)");
	graph1->title("signal");
	graph2->xlabel("fourier mode");
	graph2->ylabel("magnitude");
	graph2->title("Real fourier coefficients");
	graph3->xlabel("fourier mode");
	graph3->ylabel("magnitude");
	graph3->title("Imaginary fourier coefficients");

//	graph2->plot_style("impulses");
//	graph3->plot_style("impulses");

	graph1->hide();
	graph2->hide();
	graph3->hide();
	graph1->show();
	graph2->show();
	graph3->show();
}

int main(int argc, char **argv) {

	Fl_Double_Window mainWin(600, 700, "FFT demo");
	graph1 = new Fl_Gnuplot(20,10,560, 200);
	graph2 = new Fl_Gnuplot(20,240,560, 200);
	graph3 = new Fl_Gnuplot(20,460,560, 200);



	char line[1024] = "no contents";
	if (argc <= 1) {
		cerr << "$ FFT [input.dat]" << endl;
		exit(-1);
	}

	ifstream file(argv[1]);
	if (not file.is_open()) {
		cerr << "Could not open file " << argv[1] << endl;
		exit(-1);
	}
	cout << "Reading " << argv[1] <<endl;
	size_t cnt = 0;
	mystring test(line);
	myStringList l;
	while( file.getline(line, 1024) ) {
		// check for comments or empty lines
		test = mystring(line).strip();
		if ((test.size() > 0 and test[0] == '#') or (test.size() == 0)) continue;

		l = test.split(); // get number of columns
		cols = (l.size() > cols) ? l.size() : cols;
		cnt++;
	}


	N = cnt;
	xx = new double[N];
	yy = new double[N];

	file.clear();							// forget we hit the end of file
	file.seekg(0, ios::beg);	 // move to the start of the file

	cnt = 0;

	double dd[cols];

	while( file.getline(line, 1024) ) {
		test = mystring(line).strip();
		if ((test.size() > 0 and test[0] == '#') or (test.size() == 0)) continue;

		l = test.split();
		for (int i = 0; i < cols; i++) dd[i] = 0.;

		if (l.size() != cols) cmdline::exit("Number of columns in the file is not equal in every line.");
		for (int i = 0; i < cols; i++) l[i] >> dd[i];

		graph1->append_2d_data_columnsfv(cnt, cols, dd);

//	sscanf(line, "%lf %lf %*lf", &xx[cnt], &yy[cnt]);
		cnt++;
	}
	N = cnt; // true number of data points
	cout << "N = " << N<< endl;
	file.close();



	mainWin.add_resizable(*graph2);
	mainWin.end();

	play();

	mainWin.show(argc, argv);
	return Fl::run();

	delete graph1;
	delete graph2;
	delete graph3;
}
