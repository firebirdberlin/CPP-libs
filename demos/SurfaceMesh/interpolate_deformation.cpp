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
	ini.set_info("interpolate_deformation. Interpolate deformation vectors.");
	ini.register_param("meshes","i",  "list of mesh files (.pts and .elem)");
	ini.register_param("source","s",  "<int>: source mesh (index in mesh list)");
	ini.register_param("target","t",  "<int>: target mesh (index in mesh list)");
	ini.check(2);

//!########################### Read parameters #########################
	vector<mystring> meshes;
	int source = 0;
	int target = 0;

	ini.readVector("meshes", meshes);
	source = ini.read("source", source);
	target = ini.read("target", target);

//!########################### Error handling ##########################
	// are there mesh files ?
	if (meshes.size() == 0)
		cmdline::exit("Need a mesh ( with igb file ) or a gipl file to run.");

	// check if all mesh files exist
	for (size_t i = 0; i < meshes.size(); ++i){
		mystring s = meshes[i].file_strip_ext() + ".pts";
		if (not s.file_exists()) cmdline::exit( s+ " does not exist.");
	}

	SurfaceMesh::Deformation *deformation = NULL;
	cmdline::section("Loading deformation meshes");
	try {
		deformation = new SurfaceMesh::Deformation(meshes);
	} catch (myexception &e){
		cmdline::msg("Could not load deformation information !");
		deformation = NULL;
	}

//!######################### Do the magic ##############################

	SurfaceMesh mesh(meshes[source]); // load the source mesh
	vector<Point> vecs;

	// initialize interpolation
	deformation->initInterpolation(source, target);

//	vector<Point> pts;
//	size_t step = mesh.points() / 10000;
//	for (size_t i = 0; i < mesh.points(); i+=step){
//		pts.push_back(mesh.p[i]);
//	}

	deformation->interpolate_deformation_vectors(vecs,mesh.p);
	cmdline::msg("Saving deformation vects");
	ofstream of("vecs.vpts");
	of << vecs.size() << endl;
//	size_t i = 0;
//	for (vector<Point>::iterator it = mesh.p.begin(); it != mesh.p.end() ; it++, i++){
//		of << *it+vecs[i] << endl;
//		of << vecs[i] << endl;
//	}
	for (vector<Point>::iterator it = vecs.begin(); it != vecs.end() ; it++){
		of << *it << endl;
	}
	of.close();

	cmdline::ok();

	return 0;
}
