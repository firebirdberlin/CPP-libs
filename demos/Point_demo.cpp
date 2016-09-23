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
#include <mylibs/cmdline.hpp>

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

using namespace mylibs;

void info(double r, double phi, double theta){
	cout << " r = " << r << ", phi = " << phi << ", theta = " << theta << endl;
}

void infod(double r, double phi, double theta){
	cout << " r = " << r << ", phi = " << phi << "°, theta = " << theta  << "°"<< endl;
}

int main(int argc, char** argv)
{
	{
		cmdline::section("Point to spherical coordinates");
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,0.);
		Point p3 = Point(1.,1.,0.);
		Point p4 = Point(1.,-1.,-2.);

		double r,phi, theta;
		double phid, thetad;
		p1.info("p1");
		p1.spherical_coordinates(r, phi, theta);
		info(r, phi, theta);
		p1.spherical_coordinates_degrees(r, phid, thetad);
		infod(r, phid, thetad);


		p2.info("p2");
		p2.spherical_coordinates(r, phi, theta);
		info(r, phi, theta);
		p2.spherical_coordinates_degrees(r, phid, thetad);
		infod(r, phid, thetad);


		p3.info("p3");
		p3.spherical_coordinates(r, phi, theta);
		info(r, phi, theta);
		p3.spherical_coordinates_degrees(r, phid, thetad);
		infod(r, phid, thetad);
		// transform the vector to point in x-direction
		p3.rotate(-phi,-theta);
		p3.info("p3_rotated");

		p3.spherical_coordinates_degrees(r, phi, theta);
		infod(r, phid, thetad);


		p4.info("p4");
		p4.spherical_coordinates(r, phi, theta);
		info(r, phi, theta);
		p4.spherical_coordinates_degrees(r, phi, theta);
		infod(r, phid, thetad);
	}

	return 0;
}
