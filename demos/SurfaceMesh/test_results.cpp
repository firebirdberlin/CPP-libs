/**
 * interpolate_deformation.hpp
 *
 * Copyright 2013 Stefan Fruhner <stefan.fruhner@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 **/


#include <iostream>
#include <mylibs/myinifiles.hpp>
#include <mylibs/mymesh.hpp>

using namespace std;
using namespace mylibs;

int main(int argc, char **argv){

	myIniFiles ini(argc, argv);
	ini.set_info("Test if displacement vectors look good.");
	ini.register_param("input","i",  "list of mesh files (.pts and .elem)");
	ini.check(2);

//!########################### Read parameters #########################
	vector<mystring> input;
	ini.readVector("input", input);
//!########################### Error handling ##########################
	// are there mesh files ?
	if (input.size() == 0)
		cmdline::exit("Need a mesh ( with igb file ) or a gipl file to run.");

	// check if all mesh files exist
	for (size_t i = 0; i < input.size(); ++i){
		mystring &s = input[i];
		if (not s.file_exists()) cmdline::exit( s+ " does not exist.");
	}

//!######################### Do the magic ##############################

	vector<Point> results;
	SurfaceMesh::read_pts(results, input[0].c_str());

	mystring fn(input[0]);
	fn.file_new_ext("vecdirs");
	cout << fn << endl;
	vector<Point> displacement;
	SurfaceMesh::read_vecdirs(displacement, fn.c_str());

	if (results.size() != displacement.size()) cmdline::exit("number of items is different! Exiting !");
	for (size_t i = 0; i < results.size(); i++){
		results[i] += displacement[i];
	}

	ofstream of("out.pts");
	of << results.size() << endl;
	for (vector<Point>::iterator it = results.begin(); it != results.end() ; it++){
		of << *it << endl;
	}
	of.close();

	cmdline::ok();

	return 0;
}
