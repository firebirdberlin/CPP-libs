//      front_width.cpp
//
//      Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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
#include <mylibs/xydata.hpp>
#include <mylibs/myinifiles.hpp>
#include <math.h>

int main(int argc, char **argv)
{
	myIniFiles ini(argc, argv);

	ini.set_info("\
Analyses xy-datasets and prints out least squares fit");

	ini.register_param("input" , "i", "input data");
	ini.register_flag("nohead", "n", "Omit the header, only data output");
//	ini.register_param("output", "o", "output data");

	mystring input  = ini.read("input", mystring());
	bool header = ( not ini.exists("nohead") );
//	mystring output = ini.read("output", mystring());

	ini.check();

	XYData func(input.c_str());

	double c0 = 0.;
	double c1 = 0.;
	XYData::LinLSQ lsf = func.LinearLeastSquaresFit();

	if (header) {
		cout << "### Linear Least Squares Fit of data in "<< input << endl
			 << "## c0  = " << lsf.c0    << endl
			 << "## c1  = " << lsf.c1    << endl
			 << "## R^2 = " << lsf.rsq() << endl << endl;
		cout << "#x\tfit(x)\tstddev"<< endl;
	}

	double step = (func.xmax() - func.xmin())/10.;
	for (double x = func.xmin(); x < func.xmax(); x+= step){
		pair<double, double> y = lsf.y(x);
		cout << x << "\t" << y.first << "\t" << y.second << endl;
	}


	return 0;
}

