//      Plane.cpp
//
//      Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
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
#include <vector>
#include <mylibs/myinifiles.hpp>
#include <mylibs/Plane.hpp>
#include <mylibs/mymesh.hpp>
#include <mylibs/cmdline.hpp>

using namespace mylibs;

int main(int argc, char** argv)
{
	myIniFiles ini("Plane.ini",argc, argv);
	// Registration of all parameters
	ini.register_param("meshname", "m", "The basename of the mesh you want to load, e.g. *.pts or *.tetras");

	ini.check();

	// Reading of all values (not neccessary at the beginning of the program)
	mystring meshname = ini.read("meshname", mystring());

	// Error checking
	if ( meshname.empty() ) cmdline::exit("Please tell me a mesh to load. ( -m <name of the mesh>)");

	if ( not meshname.file_exists() and
		 not meshname.file_new_ext(".pts").file_exists())
			cmdline::exit("Mesh '"+meshname+"' does not exist.");

	ini.gen_ini_template("Plane.ini.template");

	cmdline::section("Simple intersection tests");
	cmdline::msg(" - intersection with Plane");

	Point a(0.,0.,0.);
	Point b(0.,2.,0.);
	Point c(2.,0.,0.);
	Plane pl(a,b,c);
	Point d(10, 10,-1);
	Point e(10, 10, 1);
	Line l(d, e, Line::line);

	pl.info();
	l.info("l1");
	Point inter;
	if (pl.intersection(l, inter) ) {
		cmdline::okmsg("Intersection found at ");	inter.info();
	};
	cmdline::msg("\n - Intersection of l1 with Facet");
	Facet ft = pl;
	ft.info();

	if (not ft.intersection(l, inter) ) {
		cmdline::okmsg("No intersection found.");
	} else {
		cmdline::warning("Intersection found at");	inter.info();
	}

	cmdline::msg("\n - Intersection with one of the bounding points of the facet");
	d.set(0.,2.,-1.); e.set(0.,2., 1.);
	Line l3(d, e, Line::line);
	l3.info("l3");
	if (not ft.intersection(l3, inter) ) {
		cmdline::warning("No intersection found.");
	} else {
		cmdline::okmsg("Intersection found at");
		inter.info();
	}

	cmdline::msg("\n - Intersection with one edge of the facet");
	d.set(1.,1.,-1.); e.set(1.,1., 1.);
	Line l4(d, e, Line::line);
	l4.info("l4");
	if (not ft.intersection(l4, inter) ) {
		cmdline::warning("No intersection found.");
	} else {
		cmdline::okmsg("Intersection found at"); inter.info();
	}

	cmdline::msg(" - Line in Plane pl");
	Point f(1, 1, 0);
	Point g(2, 2, 0);
	Line l2(f, g, Line::line);
	l2.info("in plane");
	if (not pl.intersection(l2, inter) ) {
		cmdline::okmsg("No intersection found.");
	} else {
		cmdline::warning("Intersection found at");
		inter.info();
	}

	cmdline::section("Intersections with surface");
	cmdline::msg(" - Loading mesh '" + meshname + "'");

	SurfaceMesh mesh(meshname);
	mesh.info(meshname.c_str());

	Point min, max, com;
	mesh.bounding_box(min, max);
	com = mesh.center_of_mass();

	cmdline::msg(" - Searching intersections with " );
	Line line(com, max);
//	Line line(min, max, Line::line);
	line.info();

	cout << "Intersections:" << endl;
	list<Facet> Facets;
	for (size_t i = 0; i < mesh.faces(); i++){
		Facet f = mesh.facet(i);
		Point intersec;
		if ( f.intersection(line, intersec) ) {
			intersec.info();
		}
	}

//	ini.writeVector("l1", l1);
//	ini.save();

	return 0;
}
