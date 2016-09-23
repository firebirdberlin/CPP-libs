//		./Analysis/Testing/test_RandomNumber.cpp
//
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
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
#include <iostream>
#include <mylibs/myinifiles.hpp>
#include <mylibs/RandomNumber.hpp>

using namespace std;

int main(int argc, char** argv) {

	//! First an inifile has to be created,
	myIniFiles ini("RandomNumber.ini", argc, argv);

	{ //! ... then the parameters are registered.
		// Register the parameter of those classes that have to be tested and of those that are needed for testing.
		RandomNumber::Register(ini);
	}

	cout<<"Testprogram for the random number generation class.\n";
	RandomNumber rnd_nr_generator(ini);

	char again('y');
	cout<<"Seed of the random number generator: "<<rnd_nr_generator.GetSeed()<<"\n";
	/** The test of the random number generator runs, until the user enters a
	 *	char other than 'y'. For each distribution are the parameters and five
	 *	to ten values printed.
	 */
	while('y' == again) {
		cout<<"Equal\n";
		for(size_t i = 0; i < 5; ++i) cout<<"\t"<<rnd_nr_generator.DistributionEqual();
		cout<<"\n";

		cout<<"Normal (sigma = "<<rnd_nr_generator.GetSigma()<<", mu = "<<rnd_nr_generator.GetMu()<<")\n";
		for(size_t i = 0; i < 5; ++i) cout<<"\t"<<rnd_nr_generator.DistributionNormal();
		cout<<"\n";

		cout<<"Laplace (sigma = "<<rnd_nr_generator.GetSigma()<<")\n";
		for(size_t i = 0; i < 5; ++i) cout<<"\t"<<rnd_nr_generator.DistributionLaplace();
		cout<<"\n";

		cout<<"Flat (range_min = "<<rnd_nr_generator.GetRangeMin()<<", range_max = "<<rnd_nr_generator.GetRangeMax()<<")\n";
		for(size_t i = 0; i < 5; ++i) cout<<"\t"<<rnd_nr_generator.DistributionFlat();
		cout<<"\n";

		cout<<"Poisson (mu = "<<rnd_nr_generator.GetMu()<<")\n";
		for(size_t i = 0; i < 10; ++i) cout<<"\t"<<rnd_nr_generator.DistributionPoisson();
		cout<<"\n";

		cout<<"Bernoulli (mu = "<<rnd_nr_generator.GetProbability()<<")\n";
		for(size_t i = 0; i < 10; ++i) cout<<"\t"<<rnd_nr_generator.DistributionBernoulli();
		cout<<"\n";

		cout<<"Another set of random variables (y/n)?\n";
		cin >>again;
	}

	return 0;
}

