//		./demos/Line_demo.cpp
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
/*
 *      test_lists.cpp
 * 		Test program for lists.
 * 		g++ test_lists.cpp -o test_lists.cpp -Wall
 */


#include <iostream>
#include <mylibs/point.hpp>
#include <mylibs/lists.h>
#include <mylibs/myline.hpp>
#include <mylibs/cmdline.hpp>
#include <mylibs/xydata.hpp>

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

using namespace mylibs;

int main(int argc, char** argv){

	cmdline::section("XYData Test 1");
	cmdline::subsection("Creating XYData");
	double xval[20];
	double yval[20];
	size_t len = 20;

	for (int i = 0; i < len ; i++){
		xval[i] = i;
		yval[i] = sin( double(i)/len * 2. * M_PI) -.2;
		cout << xval[i] << " " << yval[i] << endl;
	}

	XYData func(xval, yval, len);

	cmdline::subsection("y = 0.5");
	set<double> res = func.x(0.5);
	for (set<double>::iterator it = res.begin(); it != res.end() ; it++){
		cout << "f("<<*it << ") == 0.5 " << endl;
	}

	cmdline::subsection("y = 10.5");
	res = func.x(10.5);
	for (set<double>::iterator it = res.begin(); it != res.end() ; it++){
		cout << "f("<<*it << ") == 10.5 " << endl;
	}

	cmdline::subsection("f(x = 112.7)");
	try{
		double x0 = func.y(112.7);
		cout << "f(112.7) == " << x0 << endl;
		x0 = func.y(12.7);
		cout << "f(12.7) == " << x0 << endl;
	} catch (XYData::Exception_Range &e) {
		cerr << "Range error." << endl;
	}
	cmdline::subsection("f(x = 12.7)");
	try{
		double x0 = func.y(12.7);
		cout << "f(12.7) == " << x0 << endl;
	} catch (XYData::Exception_Range &e) {
		cerr << "Range error." << endl;
	}

	return 0;
}
