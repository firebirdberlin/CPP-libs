/*
 * CardiacMesh.cpp
 *
 * Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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
 */


#include "CardiacMesh.hpp"

using namespace mylibs;
/** \fn CardiacMesh::CardiacMesh()
 * \brief Default constructor, initializes the members with default values.
 */
CardiacMesh::CardiacMesh()
	: SurfaceMesh(), basename(""), teti2e(NULL),tete2i(NULL), transmural_pos(NULL),
	  nr_intra_elements(0), nr_extra_elements(0) {
}

/** \fn const size_t& CardiacMesh::intra_elements()const
 * Get the number of intracellular elements.
 * @return number of intracellular elements.
 **/
const size_t& CardiacMesh::intra_elements()const {
	return nr_intra_elements;
}

/** \fn const size_t& CardiacMesh::extra_elements()const
 *
 * Get the number of extracellular elements.
 * @return number of extracellular elements.
 */
const size_t& CardiacMesh::extra_elements()const {
	return nr_extra_elements;
}

/** \fn const size_t& CardiacMesh::elem_i2e(const size_t &) const
* Get the corresponding position of a point in a small set (such as
* *.transmural.pos) in the big set (such as *.lon).
* @param intra_index : index of the point in the small set
* @return sequence-index in the bigger set.
*/
const size_t& CardiacMesh::elem_i2e(const size_t &intra_index) const{
	if (! teti2e) throw myexception(EXCEPTION_ID+"Please call CardiacMesh::read_lon() before !");
	if (intra_index >= nr_intra_elements) throw myexception(EXCEPTION_ID+"Index out of range !");

	return teti2e[intra_index];
}

/** \fn void CardiacMesh::read_lon(const mystring)
*  - Reads in longitudinal fiber directions from *.lon files.
*  - Computes mapping 'teti2e' and 'tete2i' that maps intracellular element ID to
*    extracellular ones and vice versa.
*  - The number of intracellular and extracellular tets is determined.
*
* \attention :  Fibers are only stored for intracellular elements since
* 				the extracellular region does not have fibers at all !!!
* @param base: filename of the *.lon file
*/
void CardiacMesh::read_lon(const mystring base){

	if (not base.empty()) basename = base.file_strip_ext();
	mystring lonfile = basename.file_strip_ext() + ".lon";

//	if (debug)
		printf(" - Reading %s\n", lonfile.c_str());
	FILE *f = fopen(lonfile.c_str(), "r");
	assert(f != NULL && "Could not open lon file");

	lon.clear();
	lon.assign(elements(), Point());

	int dummy = 0;
	int numread = fscanf(f, "%d\n", &dummy);
	assert((numread == 1) && (dummy == 1) && "lon files which do not start with '1' are not supported.");

	teti2e = (size_t*) calloc(elements(), sizeof(size_t));
	tete2i = (size_t*) calloc(elements(), sizeof(size_t));

	// init
	for (size_t i = 0; i < elements(); i++)	tete2i[i] = (size_t)(-1);

	size_t i = 0;
	size_t e = 0;
	while ((i < elements()) && ( ! feof(f))){
		// read in fiber directions
		numread = fscanf(f, "%lf %lf %lf\n", &(lon[i].x), &(lon[i].y), &(lon[i].z));
		if ( ! (lon[i].x == 0. and lon[i].y == 0. and lon[i].z == 0.)){
			teti2e[i] = e; // mapping i2e
			tete2i[e] = i; // mapping e2i
			i++; 		   // only count intracellular tets
		}
		e++; // count every tet
		assert(numread == 3);
	}
	fclose(f);

	// number of intra and extracellular elements is now known
	nr_intra_elements = i;
	nr_extra_elements = e;
	// resize teti2e to correct size
	teti2e = (size_t*) realloc(teti2e, nr_intra_elements * sizeof(size_t));

	lon.resize(nr_intra_elements);
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
	lon.shrink_to_fit();
#endif

	return;
}

/** \fn void CardiacMesh::read_transmural_pos(const mystring)
* Fills the Array holding the transmural positions of a given point set.
* @param base: filename of the *.transmural.pos file
*/
void CardiacMesh::read_transmural_pos(const mystring base){

	if (not base.empty()) basename = base.file_strip_ext();

	mystring file = basename + "_i.transmural.pos";

//	if (debug)
		printf(" - Reading %s\n", file.c_str());

	FILE *f = fopen(file.c_str(), "r");
	assert(f != NULL && "Could not open file");

	// allocate memory
	transmural_pos = (double*) calloc(nr_intra_elements, sizeof(double));

	int numread = 0;
	for (size_t i = 0; i < nr_intra_elements; i++){
		numread = fscanf(f, "%lf\n", &(transmural_pos[i]));
		assert(numread == 1);
	}

	fclose(f);

	return;
}

void CardiacMesh::elem2bath(size_t elemID){
	if (lon.size() == 0) read_lon();
	if (elemID >= elements()) throw myexception(EXCEPTION_ID+"elemID out of range !");

	// element is already part of the bath
	if (tete2i[elemID] == (size_t)(-1)) return;


	lon[tete2i[elemID]] = Point(); // assign a zero length vector
	return;
}

/** \fn const double& CardiacMesh::transmural_position(const size_t) const
* Getter Method for a transmural position of a single point.
*
* @param elemID_intra: point index @attention: The ID must be an intracellular ID !
* @return transmural position.
*/
const double& CardiacMesh::transmural_position(const size_t elemID_intra) const{
	if (! transmural_pos) throw myexception(EXCEPTION_ID+"Please call CardiacMesh::read_transmural_pos() before !");
	if (elemID_intra >= nr_intra_elements) throw myexception(EXCEPTION_ID+"Index out of range !");

	return transmural_pos[elemID_intra];
}

/** \fn bool CardiacMesh::save_simple_lon(const char *, int)
 * Saves a simple lon-file for every triangular or tetrahedral element
 * in the mesh. Two types of vectors are written: (1.,0.,0.) if the
 * element's attribute is equal to the function parameter 'region'
 * or (0.,0.,0.) otherwise.
 *
 *
 * @param fn 	: filename of the file (including extension .lon)
 * @param region: Attribute for which the vector 1.,0.,0. is written.
 *
 * @return True if saving of the file was successful.
 **/
bool CardiacMesh::save_simple_lon(const char *fn, int region){
	cmdline::msg("  - Writing *.lon file : " + string(fn));
	fstream o(fn, fstream::out);
	if (o.fail() ) throw myexception(EXCEPTION_ID+"File output error (" + string(fn) + ").");
	o << "1" << endl;
	for (size_t i = 0; i < elements(); i++){
		if (f[i].v.size() == 3 or (f[i].v.size() == 4)){//only count triangles or tetrahedra
			if (f[i].attribute == region) o << "1.0 0.0 0.0" << endl;
			else o << "0.0 0.0 0.0" << endl;
		}
	}
	return true;
}

bool CardiacMesh::save_lon(const char* fn){
	if (lon.size() != nr_intra_elements)
		throw myexception(EXCEPTION_ID+"Number of lon items does not \
match number of intracellular elements !");

	ofstream o;
	if (fn) {
		printf(" - Saving longitudinal fiber directions %s\n", fn);
		o.open(fn);
	} else { // if not fn
		char f[1024];
		sprintf(f, "%s.lon", basename.c_str());
		printf(" - Saving longitudinal fiber directions %s\n", f);
		o.open(f);
	}

	if (o.fail() ) throw myexception(EXCEPTION_ID+"File output error (" + string(fn) + ").");
	o << "1" << endl;

	vector<Point> out(elements()); // construct extracellular array
	for (size_t i = 0; i < extra_elements(); i++){ // init with zero length vector
			out[i] = Point();
	}

	for (size_t i = 0; i < nr_intra_elements; i++){		// update intracellular fibers
		const size_t &i2e = elem_i2e(i);
		out[i2e] = lon[i];
	}


	for (size_t i = 0; i < extra_elements(); i++){ // save to file
			o << out[i] << endl;
	}
	return true;
}

/** name: CardiacMesh::save_mesh()
 * (Stefan Fruhner - 14.11.2011 11:51:28 CET)
 * Save the mesh in CARP format. Also a simple lon file is created if the
 * parameter region is > -1
 * @param basename : The name of the mesh to be saved. The extensions .pts
 * 					 and .elem are added automatically.
 * @param region   : The region value that denotes the interior or the mesh.
 * 					 -- only needed for computation of simple *.lon files.
 * @return True on success.
 **/
bool CardiacMesh::save_mesh(const char *basename, const int region){
	SurfaceMesh::save_mesh(basename);

	if (region == -1) return true;
	char base[1024];
	// additionally save a lon file
	sprintf(base,"%s.lon",basename);
	save_simple_lon(base, region);
	return true;
}
