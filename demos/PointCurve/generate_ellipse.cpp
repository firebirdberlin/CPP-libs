//      main.cpp
//
//      Copyright 2011 Sonja Molnos <sonja@sisley>
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
#include <mylibs/myline.hpp>
#include <mylibs/mystring.hpp>

#define DEBUG
#include <mylibs/cmdline.hpp>

using namespace std;
using namespace mylibs::cmdline;

void genCircle(PointCurve &curve, Point &p, float r, int num_segments){
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0
	float y = 0;

	for(int ii = 0; ii < num_segments; ii++){
		curve.push_back(Point(x + p.x, y + p.y, p.z));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}

	curve.push_back(curve.front());
}

void info(PointCurve& c){
	cout << "Arc-length     : " << c.arc_length()     << endl;
	cout << "Mean curvature : " << c.mean_curvature() << endl;

	if (c.circular()) cout << "This curve is closed !" << endl;
}

void gen_stimulus(const size_t id, const Point &p, const double width){
	Point p0 = p - Point(width/2., width/2., width/2.);
	cout << endl
<< "stimulus["<< id <<"].x0       =" <<    p0.x << endl
<< "stimulus["<< id <<"].xd       =" <<    width << endl
<< "stimulus["<< id <<"].y0       =" <<    p0.y << endl
<< "stimulus["<< id <<"].yd       =" <<    width << endl
<< "stimulus["<< id <<"].z0       =" <<     p0.z << endl
<< "stimulus["<< id <<"].zd       =" <<    width << endl
<< "stimulus["<< id <<"].stimtype = 0" << endl
<< "stimulus["<< id <<"].start    = 0.000000"<< endl
<< "stimulus["<< id <<"].duration = 2."<< endl
<< "stimulus["<< id <<"].strength = 40.0"<< endl
<< "stimulus["<< id <<"].stimtype = 0"<< endl;
}

int main(int argc, char **argv){
	section("Kreis");
	PointCurve circle = PointCurve();
	Point center(0.,0.,0.);

	genCircle(circle, center, 10., 256);
	circle.save_node("circle.node", 2);
	info(circle);

	section("Ellipse");					  //a,   b, segment_length
	PointCurve ell = PointCurve::Ellipse(200., 100., 1.);
	ell.save_node("ellipse.node", 2);
	info(ell);

	section("inner Circle");
	PointCurve circ1 = PointCurve::Circle(100., 10.);
	circ1.save_node("circle1.node", 2);
	info(circ1);

	section("outer Circle");
	PointCurve circ2 = PointCurve::Circle(200., 10.);
	circ2.save_node("circle2.node", 2);
	info(circ2);

	//
	section("a double circle");					  //a,   b, segment_length
	{
		PointCurve c1 = PointCurve::Circle(32500., 200.);
		c1.attribute(0);

		PointCurve c2 = PointCurve::Circle(16000., 200.);
		c2.attribute(1);

		c1.extend(c2);
		c1.save_node("double_circle.node", 2);
		info(c1);

		PointCurve c3 = PointCurve::Circle(24250., (size_t) 8);
		c3.translate(Point(0.,0.,65000.));
		c3.save("detectors.vpts", 3);
		c3.save("detectors.node");

		PointCurve c4 = PointCurve::Circle(2.*32500., (size_t) 8);
		c4.translate(Point(0.,0.,65000.));
		c4.save("detectors_outside.vpts", 3);
		c4.save("detectors_outside.node");

		// detector mesh
		PointCurve cm;
		for (double x = -16000.; x <= 16000 ; x+=4000){
			for (double y = -16000.; y <= 16000 ; y+=4000){
				cm.push_back(Point(x,y,65000.));
			}
		}
		cm.save("detector_mesh.vpts");

		// stimulation points
		PointPolar ps0(16000, M_PI_2);
		PointPolar ps1(16000, M_PI_4);
		PointPolar ps2(16000, 3*M_PI_4);

		Point pps0(ps0);
		Point pps1(ps1);
		Point pps2(ps2);

		cout << pps0 << endl;
		cout << pps1 << endl;
		cout << pps2 << endl;
		gen_stimulus(0, pps0, 1000.);
		gen_stimulus(1, pps1, 1000.);
		gen_stimulus(2, pps2, 1000.);
//		Point off(-500, -500, 0);
//		cout << pps0+off << endl;
//		cout << pps1+off << endl;
//		cout << pps2+off << endl;
	}

	//
	section("Ellipse as approximation for a heart");					  //a,   b, segment_length
	{
		const double res = 250.;
		PointCurve c = PointCurve::Ellipse(52500., 30000., res);
		c.attribute(0);
		PointCurve c1 = PointCurve::Circle(32000./2., res);
		c1.translate(Point(-45000./2.,0.));
		c1.attribute(1);

		PointCurve c2 = PointCurve::Circle(32000./2., res);
		c2.translate(Point(45000/2.,0.));
		c2.attribute(2);

		c.extend(c1);
		c.extend(c2);

		c.save_node("elliptic_heart.node", 2);
		info(c);

		PointCurve c3 = PointCurve::Ellipse(38500., 20000.,(size_t) 10);
		c3.translate(Point(0.,0.,65000.));
		c3.save("detectors_ellipse.vpts", 3);
		c3.save("detectors_ellipse.node");
	}
	return 0;
}


