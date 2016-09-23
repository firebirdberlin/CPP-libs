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
using namespace mylibs;
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

void sinus(PointCurve &curve, int num_segments, int n){

	for(int ii = 0; ii < num_segments; ii++){

		float x = 2*M_PI * (float)ii/num_segments * n;
		float y = sinf(x);
		curve.push_back(Point(x, y));

	}
}

void info(PointCurve& c){
	cout << "Arc-length     : " << c.arc_length()     << endl;
	cout << "Mean curvature : " << c.mean_curvature() << endl;

	if (c.circular()) cout << "This curve is closed !" << endl;
}

void save_curvature(const char * fn, PointCurve& c){
	cout << "Saving "<< fn << endl;
	FILE *f = 0;
	int cnt = 0;
	f = fopen(fn,"w");
	for (PointCurve::const_iterator it = c.begin(); it != c.end(); it++, cnt++){
		fprintf(f,"%04d %lf %lf \n",cnt, c.arc_length(it), c.curvature(it));
	}
	fclose(f);

	mystring fn2(fn);
	mystring ext = fn2.file_ext();
	fn2 = fn2.file_base() + "_method2." + fn2.file_ext();

	f = fopen(fn2.c_str(),"w");
	double *cu = c.curvature(10);
	for (size_t i = 0; i < c.size(); i++){
		fprintf(f,"%04lu %lf \n",i, cu[i]);
	}

	delete cu;
 	fclose(f);
 }

int main(int argc, char **argv){
	const float ratio = .2;
	section("Einlesen der Spirale");

	vector<PointCurve> v;
	PointCurve::load_multiple_curves("Test.node", v);


	if (v.size() == 0) cmdline::exit("No curves found !");

	if (v.size() == 0) {
		cerr << "no curves read!" << endl;
		exit(EXIT_FAILURE);
	}

	subsection("Verarbeiten");
	subsection("Einzelkurven speichern.");
	for (size_t i = 0; i < v.size(); i++){
		char fn[128] = "hallo";
		printf("out_%03lu.node", i);
		sprintf(fn, "out_%03lu.node", i);
		v[i].save_node(fn);
	}

	section("Splines, Krümmung und Bogenlänge");
	subsection("Spirale");

	double stepsize = v[0].arc_length()/(double)v[0].size();

	msg(" > Splines");
	// spline of the input curve
	PointCurve f_x = v[0].compute_spline(ratio,stepsize,0);
	save_curvature("curvature_spiral_inter.txt", f_x);
	f_x.save_node("Test_interp.node", 2);

	// first derivative
	PointCurve f_1x = v[0].compute_spline(ratio,stepsize, 1);
	f_1x.save_node("Test_interp_deriv2.node", 2);

	// second derivative
	PointCurve f_2x = v[0].compute_spline(ratio,stepsize, 2);
	f_2x.save_node("Test_interp_deriv2.node", 2);

	msg(" > Krümmung");
	save_curvature("curvature_spiral.txt", v[0]);
	info(v[0]);


	subsection("Kreis");
	PointCurve circle = PointCurve();
	Point center(0.,0.,0.);

	genCircle(circle, center, 10., 256);
	circle.save_node("circle.node", 2);

	PointCurve cspline = circle.compute_spline(ratio, 0.2, 0);
	cspline.save_node("circle_spline.node");

	PointCurve cspline1 = circle.compute_spline(ratio, 0.2, 1);
	cspline1.save_node("circle_spline_deriv1.node");

	PointCurve cspline2 = circle.compute_spline(ratio, 0.2, 2);
	cspline2.save_node("circle_spline_deriv2.node");


	save_curvature("curvature_circle.txt", circle);
 	info(circle);


	subsection("Sinus");
	PointCurve sinus2 = PointCurve();
	sinus(sinus2, 2560, 5);
	sinus2.save_node("sinus.node", 2);
	save_curvature("curvature_sine.txt", sinus2);

 	info(sinus2);
 		section("Berechnung Geschwindigkeit");
 			vector<PointCurve> v2,v3;

	PointCurve::load_multiple_curves("Isolines_1.node", v2);
	PointCurve::load_multiple_curves("Isolines_2.node", v3);
	if (v2.size() == 0 or v3.size() == 0) cmdline::exit("Konnte nix finden !");
	FILE*f=fopen("speed.txt","w");
 	for(PointCurve::const_iterator it=v2[0].begin();it!=v2[0].end();it++){
//		v2[0].speed_normal(it,v3[0],0.002);
		fprintf(f,"%lf %lf\n",v2[0].arc_length(it),v2[0].speed_normal(it,v3[0],0.002).abs());
	}
	return 0;
}


