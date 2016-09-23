//      algorithm.cpp
//
//      Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <iostream>

#include <mylibs/myinifiles.hpp>
#include <mylibs/RandomNumber.hpp>
#include <mylibs/algorithm.hpp>
#include <mylibs/cmdline.hpp>
#include <time.h>

using namespace std;
using namespace mylibs;

size_t NRELEM = 5000000;

int main(int argc, char** argv) {

	cmdline::section("Demo for Random numbers and myalgorithm");

	//! First an inifile has to be created,
	myIniFiles ini(argc, argv);

	ini.register_param("nelem","n", "The total number of random numbers.");
	ini.register_param("verbose", "v", "Print out verbose messages", false);

	{ //! ... then the parameters are registered.
		// Register the parameter of those classes that have to be tested and of those that are needed for testing.
		RandomNumber::Register(ini);
	}
	NRELEM  = ini.read("nelem",NRELEM);
	bool verbose = ini.exists("verbose");

	cout << "Remaining args: "<< ini.param_non_opt_args() << endl;

	if (verbose) cmdline::subsection("Initializing RandomNumber Generator");

	RandomNumber rnd(ini);
	rnd.SetSeed(time(0));
	if (verbose) cmdline::msg(" - Seed : " + toString(rnd.GetSeed()));

	if (verbose) {
		cmdline::subsection("Creating vector with equally distributed random numbers");
		cmdline::msg(" - " + toString(NRELEM) + " elements");
	}
	vector<double> v(NRELEM,0.);
	for(size_t i = 0; i < NRELEM; ++i){
		v[i] = rnd.DistributionEqual();
	}

	cmdline::subsection("Minimum and Maximum with OpenMP");
	clock_t start = clock();
	double minimum = algorithm::min(v);
	double maximum = algorithm::max(v);
	clock_t finish = clock();
	double d = (finish-start);
	TELL(minimum);
	TELL(maximum);
	printf(" - total runtime = %lf msec.\n", d/1000.);

	cmdline::subsection("Minimum and Maximum with OpenMP");

	start = clock();
	minimum = maximum = 0.;
	algorithm::minmax(v, minimum, maximum);
	finish = clock();
	double dm = (finish-start)/1000.;
	TELL(minimum);
	TELL(maximum);
	printf(" - total runtime = %lf msec.\n", dm);

	cmdline::subsection("Minimum and Maximum with STL algorithm");
	start = clock();
	double minimum2 = *std::min_element(v.begin(), v.end());
	double maximum2 = *std::max_element(v.begin(), v.end());
	finish = clock();
	double d2 = (finish-start);
	TELL(minimum2);
	TELL(maximum2);
	printf(" - total runtime = %lf msec.\n", d2/1000.);
	printf(" - ratio = %.3lf %% \n", d/d2 * 100.);


	cmdline::subsection("Moving average");
	cout << v.size() << endl;
	algorithm::moving_average<double>(&(v[0]), v.size(), 1);
	cmdline::ok();

	ini.save();

	return 0;
}

