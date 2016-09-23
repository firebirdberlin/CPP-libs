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
#include <fstream>

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

using namespace std;
using namespace mylibs;

double step = 10.E3; // 1 cm, when units are in um
const size_t start = 4;
const size_t end   = 104;
//const size_t start = 13;
//const size_t end   = 14;
int main(int argc, char** argv){
	
	Point p1 = Point( 0.,0., -6644.637207);
	Point p2 = Point( +101968.367188, -69100.210938, -6644.637207);
	Line  ll = Line(p1,p2,Line::line); // left
	
	Point p3 = Point( -101968.367188, -69100.210938, -6644.637207);
	Line  lr = Line(p1,p3,Line::line); // right

	// print number of items
	ofstream op("points.pts", ios::trunc);
	op << (end-start) *2 << endl;
	for (size_t i = start; i < end; i++){
		Point P1 = ll.point_in_dist(i*step);
		Point P2 = lr.point_in_dist(i*step);
		op << scientific << setprecision(15) << setw(22) << P1 << endl;
		op << scientific << setprecision(15) << setw(22) << P2 << endl;
	}
	op.close();

	ofstream os("points.selection", ios::trunc);
	os << (end-start) *2 << " 3 2 0" << endl;
	size_t cnt = 0;
	for (size_t i = start; i < end; i++, cnt++){
		Point P1 = ll.point_in_dist(i*step);
		Point P2 = lr.point_in_dist(i*step);
		os 	<< (i -start) * 2 << " " 
			<< scientific << setprecision(15) << setw(22) << P1 << " "
			<< cnt   << " 2" << endl
			<< (i -start) * 2 + 1 << " " 
			<< scientific << setprecision(15) << setw(22) << P2 << " "
			<< cnt << " 2" << endl;
//		cout << " " << fixed << setprecision(2) << setw(13)
//			 << (P2-p1).abs() << " " 
//			 << (P2-P1).abs() << endl;
	}
	op.close();
	
	ofstream of("distances.dat", ios::trunc);
	of << "# distance from center\tdistance p2-p1" << endl;
//	of << (end-start) << endl;
	for (size_t i = start; i < end; i++){
		Point P1 = ll.point_in_dist(i*step);
		Point P2 = lr.point_in_dist(i*step);
			
//		of << " " << fixed << setprecision(2) << setw(13)
		of << scientific << setprecision(15) << setw(22)
			 << (P2-p1).abs() << "\t" 
			 << (P2-P1).abs() << endl;
	}
	of.close();
	return 0;
}
