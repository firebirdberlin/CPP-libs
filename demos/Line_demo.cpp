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

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

using namespace mylibs;

bool find_intersection(Line &l1, Line &l2){

	l1.prnt("l1"); cmdline::newline();
	l2.prnt("l2");

	Point intersection;
	bool result = l1.intersection(l2, intersection);
	switch (result){
	case true:
		cout << "intersection : "; intersection.prnt();
		break;
	default:
		cout << "No intersection found !" << endl;

	}
	return result;

}

void test_inside(PointCurve &pc, Point &p){
	p.info();

	if (not pc.inside(p))
		cmdline::warning("Point shouldn't be outside !");
	else
		cmdline::okmsg("Point is inside !");
}

void test_outside(PointCurve &pc, Point &p){
	p.info();

	if (not pc.inside(p))
		cmdline::okmsg("Point is outside !");
	else
		cmdline::warning("Point shouldn't be inside !");
}

void test_perp_foot(Line &l1, Point &pt,bool has_foot = true ){
	l1.info("line ");
	cout << "Point = "; pt.info();

	Point foot(0.,0.,0.);
	if (has_foot){
		if (l1.perpendicular_foot_of(pt, foot)) {
			cmdline::okmsg("Foot found :"); foot.info();
		} else cmdline::warning("Foot NOT found.");
	}
	else {
		if (not l1.perpendicular_foot_of(pt, foot)) {
			cmdline::okmsg("Foot NOT found");
		} else {
			cmdline::warning("Foot found : ");  foot.info();
		}
	}
}

int main(int argc, char** argv)
{
	{
		cmdline::section("Intersection 1");
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,0.);
		Point p3 = Point(1.,1.,0.);
		Point p4 = Point(2.,0.,0.);
		Line l1 = Line(p1,p2,Line::line);
		Line l2 = Line(p3,p4,Line::segment);
		if (find_intersection(l1,l2)) cmdline::okmsg("Test passed");
	}

	{
		cmdline::section("Intersection 2");
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(1.2,.3,0.);
		Point p3 = Point(1.,1.,0.);
		Point p4 = Point(2.,0.,0.);
		Line l1 = Line(p1,p2,Line::segment);
		Line l2 = Line(p3,p4,Line::segment);
		if (not find_intersection(l1,l2)) cmdline::okmsg("Test passed");
	}

	{
		cmdline::section("Intersection 3");
		Point p1 = Point(1.,0.,1.);
		Point p2 = Point(2.,1.,1.);
		Point p3 = Point(1.,1.,1.1);
		Point p4 = Point(2.,0.,1.1);
		Line l1 = Line(p1,p2,Line::line);
		Line l2 = Line(p3,p4,Line::line);
		if (not find_intersection(l1,l2)) cmdline::okmsg("Test passed");
	}

	{
		cmdline::section("Intersection 4");
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,1.);
		Point p3 = Point(1.,1.,1.);
		Point p4 = Point(2.,0.,0.);
		Line l1 = Line(p1,p2,Line::line);
		Line l2 = Line(p3,p4,Line::line);
		if (find_intersection(l1,l2)) cmdline::okmsg("Test passed");
	}

	{
		cmdline::section("Intersection 5");
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,1.);
		Point p3 = Point(1.,0.,0.);
		Point p4 = Point(2.,0.,0.);
		Line l1 = Line(p1,p2,Line::line);
		Line l2 = Line(p3,p4,Line::line);
		if (find_intersection(l1,l2)) cmdline::okmsg("Test passed");
	}

	{
		cmdline::section("Testing PointCurve");
		PointCurve pc;
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(5.,0.,0.);
		Point p3 = Point(5.,4.,0.);
		Point p4 = Point(1.,4.,0.);

		pc.push(p1);
		pc.push(p2);
		pc.push(p3);
		pc.push(p4);
		pc.push(p1);

		if (pc.circular()) cout << "PointCurve is circular." << endl;
		cout << "Arc-length : " << pc.arc_length() << endl;
		for (size_t i = 0; i < pc.size(); i++){
			cout << "Arc-length until element "<< i << ": " << pc.compute_arc_length(i) << endl;
		}

		pc.info();

		Point outside 	= Point(0.,0.,0.); // There different posibilities to construct a point
		Point test		= Point(2.5,1.,0.);// but this one creates an instance and then copies the point,
		Point test2(6,1.,0.);			   // whereas copieing is not needed here.
		Point test3(3.,1.,0.);
		Point test4(3.,0.,0.);


		cmdline::subsection("Testing some points using function inside(Point &, Point &)");

		test.info();
		if (pc.inside(test, outside))
			cmdline::okmsg("Point is inside !");

		outside.info();
		if (not pc.inside(outside, outside))
			cmdline::okmsg("Point is outside !");

		test2.info();
		if (not pc.inside(test2, outside))
			cmdline::okmsg("Point is outside !");

		test3.info();
		if (not pc.inside(test3, outside))
			cmdline::warning("Point is outside !");
		else
			cmdline::okmsg("Point is inside !");

		test4.info();
		if (not pc.inside(test4, outside))
			cmdline::warning("Point is outside !");
		else
			cmdline::okmsg("Point is inside !");


		p2.info();
		if (not pc.inside(p2, outside))
			cmdline::warning("Point is outside !");
		else
			cmdline::okmsg("Point is inside !");

		cmdline::subsection("Testing some points, using function inside(Point &)");
		test_inside(pc, test);
		test_outside(pc, outside);
		test_outside(pc, test2);

		test_inside(pc, p1);
		test_inside(pc, p2);
		test_inside(pc, p3);

		test_inside(pc,test3 );

	}

	cmdline::section("Testing perpendicular_foot_of()");
	{
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(3.,0.,0.);

		Line l1 = Line(p1,p2,Line::line);
		Point pt = Point(2.,1.,0.);

		test_perp_foot(l1, pt);

	}

	cmdline::subsection(" Test #2 ");
	{
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,0.);

		Line l1 = Line(p1,p2,Line::segment);
		Point pt  = Point(2.5, 0.5, 0.);
		Point pt2 = Point(1.5, 0.5, 0.);
		Point pt3 = Point(2.5, 10.5, 0.);

		test_perp_foot(l1, pt);
		test_perp_foot(l1, pt2);
		test_perp_foot(l1, pt3, false);
	}

	cmdline::subsection(" Test #3 ");
	{
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,2.);

		Line l1 = Line(p1,p2,Line::line);
		Point pt  = Point(2.5, 0.5, 0.);
		Point pt2 = Point(1.5, 0.5, 4.);

		test_perp_foot(l1, pt);
		test_perp_foot(l1, pt2);
	}

	cmdline::section("Testing Line::is_on_line()");
	{
		Point p1 = Point(1.,0.,0.);
		Point p2 = Point(2.,1.,0.);
		Point p3 = Point(-1.572, 2., 0.);
		Point p4 = Point(3.8796, 4.67657,0.);
		Line l1 = Line(p1,p2,Line::line);
		Line l2 = Line(p1,p2,Line::segment);
		Line l3 = Line(p3,p4,Line::line);

		cmdline::subsection("Intersection");
		Point intersec = Point();
		if (l1.intersection(l3, intersec)) intersec.info();
		double m = 0.;

		if (l1.is_on_line(intersec, m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::warning("Pt. is NOT on line.");
		if (l3.is_on_line(intersec, m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::warning("Pt. is NOT on line.");

		Point pt  = Point(1.5, 0.5, 0.);
		Point pt2 = Point(2.5, 0.5, 0.);
		Point pt3 = Point(2.5, 1.5, 0.);

		cmdline::subsection("Testing is_on_line()");

		l1.info("l1");
		if (l1.is_on_line(pt , m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::warning("Pt. is NOT on line.");

		if (l1.is_on_line(pt2 , m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::okmsg("Pt. is NOT on line.");

		if (l1.is_on_line(pt3 , m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::warning("Pt. is NOT on line.");

		cout << endl;
		l2.info("l2");
		if (l2.is_on_line(pt , m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::warning("Pt. is NOT on line.");

		if (l2.is_on_line(pt2 , m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::okmsg("Pt. is NOT on line.");

		if (l2.is_on_line(pt3 , m)) cout << "Pt. is on line : m = " <<  m << endl;
		else cmdline::okmsg("Pt. is NOT on line.");

	}

	cmdline::section("Line and normal vectors");
	{
		Point pa = Point(5,10,0);
		Point pb = Point(6, 7,0);

		Line l = Line(pa, pb);
		Point nl = l.normal_lhs();
		Point nr = l.normal_rhs();

		l.info();
		nl.info();
		nr.info();
	}

	cmdline::section("Computing a point in a certain distance ... ");
	{
		Point pa = Point(12.5, -59.38, 118.5);
		Point pb = Point(1.565, -92.185, -70.54);

		Line l = Line(pa, pb, Line::segment);
		l.info( "l_{ab}");
		cout << "length = "<<l.length() << endl;
		Point pt = l.point_in_dist(129.);

		pa.info("                          pa ");
		pb.info("                          pb ");
		pt.info(" - Point in 12.9 cm distance ");

	}
	return 0;
}
