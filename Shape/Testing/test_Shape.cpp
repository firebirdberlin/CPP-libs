//		./Shape/Testing/test_Shape.cpp
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

#include <mylibs/myline.hpp>

#include "../Shape.hpp"
#include "../Cube.hpp"
#include "../Cylinder.hpp"
#include "../Prism.hpp"
#include "../Sphere.hpp"

using namespace std;

int main() {
	Point one_corner(-20.1, -51.4, 15.1), sec_corner(60.9, 10.2, 10.0);
	Point center(0, 0, 8);
	Point test1(-20.1, 0, 11), test2(-20.1, 1, 0);
	Point k1(9.2, 7.7, 0.7), k2(10.0, 8.6, -2.7), k3(-3.0, 4.1, -8.9), k4(-6.6, 4.4, -7.7);
	Point prism_p1(0.0, 0.0, 0.0), prism_p2(-20.0, -4.0, 0.0), prism_p3(-22.0, 0.0, 0.0),
	      prism_p4(-7.5, 3.5, 0.0), prism_p5(0.0, 4, 0.0);
	PointCurve prism_curve;
	prism_curve.push_back(prism_p1);
	prism_curve.push_back(prism_p2);
	prism_curve.push_back(prism_p3);
	prism_curve.push_back(prism_p4);
	prism_curve.push_back(prism_p5);
	prism_curve.push_back(prism_p1);

	Cube cube(one_corner, sec_corner);
	Cylinder cylinder(center, one_corner);
	Sphere sphere(center, one_corner);
	Sphere sphere_4p(k1, k2, k3, k4);
	Prism prism(prism_curve, 12.1, 9.4);

	cout<<"at start:\n";
	cube.Info();
	cylinder.Info();
	sphere.Info();
	sphere_4p.Info();
	prism.Info();

	cout<<" test if "<<test1<<" is within cube: "		<<	cube.Inside(test1)		<<"\n";
	cout<<" test if "<<test1<<" is within cylinder: "	<<	cylinder.Inside(test1)	<<"\n";
	cout<<" test if "<<test2<<" is within cylinder: "	<<	cylinder.Inside(test2)	<<"\n";
	cout<<" test if "<<test1<<" is within sphere: "		<<	sphere.Inside(test1)	<<"\n";
	cout<<" test if "<<test1<<" is within prism:  "		<<	prism.Inside(test1)		<<"\n";

	cout<<"Making cube a rectangle.\n";
	cube.MakeRectangle();
	cube.Info();
	cout<<" test if "<<test1<<" is within rectangle: "<<cube.Inside(test1)<<"\n";
	cout<<" test if "<<test2<<" is within rectangle: "<<cube.Inside(test2)<<"\n";

	return 0;
}
