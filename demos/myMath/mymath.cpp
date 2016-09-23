//      exists.cpp
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
#include <mylibs/mymath.hpp>
#include <mylibs/cmdline.hpp>

using namespace myMath;

int main(int argc, char **argv){


	// in gcc 4.6 here the c++ version is printed
	cout << __cplusplus << endl;

	Vector p1({-0.1,0.,0.});
	p1.info("p1");

	double v[9] = { 3,1,5,
					 3,3,1,
					 4,6,4
					}; 
	Matrix m(3, 3, v);
	m.info("m");
	m.transpose();
	m.info("Transposed");
	
	Vector r = m * p1;
	r.info("m.p1");

//	Matrix M = m + m;
//	M.info();

	Matrix min = m.Minor(0,1);
	
	
	m.invert();
	m.info("m^(-1)");

	return 0;
}

