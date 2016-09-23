//		./MCubes/Grid.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Arash Azhand <azhand@itp.tu-berlin.de>
//		Copyright 2011 Philipp Gast <avatar81@itp.tu-berlin.de>
//
//		This file is part of mylibs.
//
//		mylibs is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		mylibs is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with mylibs; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.

#include <cstring>
#include <fstream>
//#include <iostream>

#include "Grid.hpp"

namespace MCubes {

/** \fn Grid::Grid()
 * \brief Standardconstructor, uses default values.
 */
Grid::Grid()
	:	my_regular_grid(0, 0, 0), Iso_Value(0.) {
	TriangleList.clear();
	pixdim(Point(1., 1., 1.));
	origin(0., 0., 0.);
	CubeList =NULL;
	Cubes_Set = true;
}

/** \fn Grid::Grid(const Point, size_t const, size_t const, size_t const)
 *	\brief Constructor which takes some arguments to initialize the grid.
 */
Grid::Grid(const Point pixel_dims, size_t const max_x, size_t const max_y, size_t const max_z)
	:	 my_regular_grid(max_x, max_y, max_z), Iso_Value(0.) {
	TriangleList.clear();
	pixdim(pixel_dims);
	origin(0., 0., 0.);
	size_t const k = this->dimensions[0] - 1;
	size_t const l = this->dimensions[1] - 1;
	size_t const m = this->dimensions[2] - 1;
	size_t const length = k * l * m;

	CubeList = new Cube[length];
	Cubes_Set = true;
	ConstructCubeList();
}

/** \fn Grid::~Grid()
 *	Destructor, frees the memory the constructor has reserved.
 */
Grid::~Grid() {
	if(Cubes_Set) {
		delete[] this->CubeList;
		this->CubeList = NULL;
	}
}

void Grid::ConstructCubeList() {
	size_t x, y, z, i = 0;

	for(z=0; z < this->dimensions[2] - 1; z++)
		for(y=0; y < this->dimensions[1] - 1; y++)
			for(x=0; x < this->dimensions[0] - 1; x++) {
				//! for every quad in the grid
				Cube &cube = CubeList[i];
				// First setting the coordinates
				Point pnt[8];
				size_t idx[8];

				pnt[0] = this->coords(x  ,y  ,z  );
				pnt[1] = this->coords(x+1,y  ,z  );
				pnt[2] = this->coords(x+1,y+1,z  );
				pnt[3] = this->coords(x  ,y+1,z  );
				pnt[4] = this->coords(x  ,y  ,z+1);
				pnt[5] = this->coords(x+1,y  ,z+1);
				pnt[6] = this->coords(x+1,y+1,z+1);
				pnt[7] = this->coords(x  ,y+1,z+1);

				idx[0] = this->ijk_index(x  ,y  ,z  );
				idx[1] = this->ijk_index(x+1,y  ,z  );
				idx[2] = this->ijk_index(x+1,y+1,z  );
				idx[3] = this->ijk_index(x  ,y+1,z  );
				idx[4] = this->ijk_index(x  ,y  ,z+1);
				idx[5] = this->ijk_index(x+1,y  ,z+1);
				idx[6] = this->ijk_index(x+1,y+1,z+1);
				idx[7] = this->ijk_index(x  ,y+1,z+1);

				cube.SetVertices(pnt[0], pnt[1], pnt[2], pnt[3], pnt[4], pnt[5], pnt[6], pnt[7]);

				// Now setting the cube indices that is the ijk-indices, which will make later value-settings easier from 1D-data-arrays
				cube.SetCubeIndices(idx[0], idx[1], idx[2], idx[3], idx[4], idx[5], idx[6], idx[7]);
				//CubeList.push_back(cube);
				i++;
			}
}

/** \fn void Grid::SetCubeValues(Cube &, const double * const)
 * \brief Sets the vertex values of a cube.
 *
 *	Sets the values at the edges of the cube.
 *
 * \param cube: Reference to the Cube for whom to set the vertex values.
 * \param data: Pointer to a double array where the data can be found, that
 *				should be used to set the values.
 */
void Grid::SetCubeValues(Cube &cube, const double * const data) {
	size_t k;

	for(k = 0; k < 8; k++) {
		cube.SetVertexValue(k, data);
	}
}

/** \fn void Grid::RunAlgorithm(const double * const, const double)
 * \brief Compute the isosurface.
 *
 *	This method will compute the isosurface within the given data. The
 *	trigangles are at the end in \a TriangleList.
 *
 * \param data: Pointer to doubles, which contain the values. It is assumed that
 *				within the given 1d-array are the three dimensional data.
 * \param iso: The value that should be used for the isosurface.
 */
void Grid::RunAlgorithm(const double * const data, const double iso) {
	Iso_Value = iso;
	TriangleList.clear();

	size_t length = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
	size_t i = 0;


#ifdef _OPENMP /** Compute in parallelized manner*/
	#pragma omp parallel for private(i)
#endif
	for (i = 0; i < length; i++) {
		Cube &cube = CubeList[i];
		SetCubeValues(cube, data);
	}

	//#ifdef _OPENMP /** Compute in parallelized manner*/
	//#pragma omp parallel for private(i)
	//#endif
	for (i = 0; i < length; i++) {
		Cube &cube = CubeList[i];
		Polygonise(cube);
	}
}

/** \fn void Grid::Polygonise(Cube &)
 * \brief Get the triangle for a cube.
 *
 * \param cube: The cube for which to determine the triangle at which the
 *				surface intersects.
 */
void Grid::Polygonise(Cube &cube) {
	int cubeindex;
	Point vertexList[12] = {Point()};

	/*
	  Determine the index into the edge table which
	  tells us which vertices are inside of the surface
	*/
	cubeindex = 0;
	if (cube.GetVertexValue(0) < Iso_Value)		cubeindex |= 1;
	if (cube.GetVertexValue(1) < Iso_Value)		cubeindex |= 2;
	if (cube.GetVertexValue(2) < Iso_Value)		cubeindex |= 4;
	if (cube.GetVertexValue(3) < Iso_Value)		cubeindex |= 8;
	if (cube.GetVertexValue(4) < Iso_Value)		cubeindex |= 16;
	if (cube.GetVertexValue(5) < Iso_Value)		cubeindex |= 32;
	if (cube.GetVertexValue(6) < Iso_Value)		cubeindex |= 64;
	if (cube.GetVertexValue(7) < Iso_Value)		cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edgeTable[cubeindex] == 0)	return;

	/* Find the vertices where the surface intersects the cube */
	if (edgeTable[cubeindex] &    1) vertexList[ 0] = VertexInterpolation(cube.GetVertex(0), cube.GetVertex(1), cube.GetVertexValue(0), cube.GetVertexValue(1));

	if (edgeTable[cubeindex] &    2) vertexList[ 1] = VertexInterpolation(cube.GetVertex(1), cube.GetVertex(2), cube.GetVertexValue(1), cube.GetVertexValue(2));

	if (edgeTable[cubeindex] &    4) vertexList[ 2] = VertexInterpolation(cube.GetVertex(2), cube.GetVertex(3), cube.GetVertexValue(2), cube.GetVertexValue(3));

	if (edgeTable[cubeindex] &    8) vertexList[ 3] = VertexInterpolation(cube.GetVertex(3), cube.GetVertex(0), cube.GetVertexValue(3), cube.GetVertexValue(0));

	if (edgeTable[cubeindex] &   16) vertexList[ 4] = VertexInterpolation(cube.GetVertex(4), cube.GetVertex(5), cube.GetVertexValue(4), cube.GetVertexValue(5));

	if (edgeTable[cubeindex] &   32) vertexList[ 5] = VertexInterpolation(cube.GetVertex(5), cube.GetVertex(6), cube.GetVertexValue(5), cube.GetVertexValue(6));

	if (edgeTable[cubeindex] &   64) vertexList[ 6] = VertexInterpolation(cube.GetVertex(6), cube.GetVertex(7), cube.GetVertexValue(6), cube.GetVertexValue(7));

	if (edgeTable[cubeindex] &  128) vertexList[ 7] = VertexInterpolation(cube.GetVertex(7), cube.GetVertex(4), cube.GetVertexValue(7), cube.GetVertexValue(4));

	if (edgeTable[cubeindex] &  256) vertexList[ 8] = VertexInterpolation(cube.GetVertex(0), cube.GetVertex(4), cube.GetVertexValue(0), cube.GetVertexValue(4));

	if (edgeTable[cubeindex] &  512) vertexList[ 9] = VertexInterpolation(cube.GetVertex(1), cube.GetVertex(5), cube.GetVertexValue(1), cube.GetVertexValue(5));

	if (edgeTable[cubeindex] & 1024) vertexList[10] = VertexInterpolation(cube.GetVertex(2), cube.GetVertex(6), cube.GetVertexValue(2), cube.GetVertexValue(6));

	if (edgeTable[cubeindex] & 2048) vertexList[11] = VertexInterpolation(cube.GetVertex(3), cube.GetVertex(7), cube.GetVertexValue(3), cube.GetVertexValue(7));


	/* Create the triangle */
	for(int i = 0; triTable[cubeindex][i] != -1; i+=3) {
		try {
			Triangle T(	vertexList[triTable[cubeindex][i  ]],
			            vertexList[triTable[cubeindex][i+1]],
			            vertexList[triTable[cubeindex][i+2]] );
			//T.info();
			TriangleList.push_back(T);
		} catch(Point::Exception_ZeroLength &e) {
			// There is no need to make a specific handling, ignoring is ok.
		}
	}
}

/** \fn Point Mesh::VertexInterpolation(const Point &, const Point &, const double &, const double &)
 * 	Linearly interpolate the position where an isosurface cuts
 * 	an edge between two vertices, each with their own scalar value
 * If the desired minimum-edge length is violated then the function
 * decides whether to return the left or the right point.
 *
 * \attention
 * The Grid doesn't care about well defined triangles.
 * Possibly some angles may become very small or what is equivalent :
 * some edges become very short. If the user wants to use the
 * triangulations for finite element simulations for example it is
 * appropriate to define a minmimal distance (as percentage of the
 * length of one segment) that is allowed between to grid points.
 * If the interpolated point p is closer to one of the
 * edges p1 or p2, then the point p1 or p2 itself is returned to
 * ensure, that the distance becomes to short. On the other hand
 * this method gains a loss of accuracy.
 *
 * \param p1              : Vertex 1
 * \param p2              : Vertex 2
 * \param v1       : value of vertex 1
 * \param v2       : value of vertex 2
 * \return A point that conatins the position of the cut.
 */
Point Grid::VertexInterpolation(const Point  &p1, const Point  &p2,
								const double &v1, const double &v2 ) {

	if(((Iso_Value - v1)*(Iso_Value - v1)) < SMALL_NUM)	return p1;
	if(((Iso_Value - v2)*(Iso_Value - v2)) < SMALL_NUM)	return p2;
	if(((v1 - v2)       *(v1 - v2))        < SMALL_NUM)	return p1;

	double diff = (Iso_Value - v1) / (v2 - v1);

	return p1 + diff*(p2 - p1);
}

/** \fn size_t Grid::ijk_index(size_t const, size_t const, size_t const)
 * Converts 3d index set into 1d index and assumes the 1d array is
 * sorted such, that x varies fastest, then y and finally z.
 * @param i : index x
 * @param j : index y
 * @param k : index z
   @return	: 1d index */
size_t Grid::ijk_index(size_t const i, size_t const j, size_t const k) {
	size_t ret = 0;
	ret = i + j * (this->dimensions[0]) + k * (this->dimensions[0] * this->dimensions[1]);
	return ret;
}

/** \fn void Grid::SaveTriangleList(const char * const) const
 * \brief Saves triangles to a file.
 *
 *	This method will store the triangles within a file with given name and path.
 *
 * \param filename: Reference to a string with name and path of the file in
 *				which the triangles should be stored.
 */
void Grid::SaveTriangleList(const char *filename) const {
	std::ofstream schreiben;

	//! If no filename is given, we use 'Test.dat' as standard name for the file.
	if(! filename)	schreiben.open("Tri_List_out.dat");
	else			schreiben.open(filename);


	if(not schreiben.bad()) {
		//! First some header information.
		schreiben << "## v1.x v1.y v1.z v2.x v2.y v2.z v3.x v3.y v3.z \n";

		//cout<<"Header is written."<<nr_elements<<" "<<nr_variables<<"\n";

		//! Now we write all values for one point in one line and do this for every point.
		for (list<Triangle>::const_iterator it = TriangleList.begin(); it != TriangleList.end(); it++) {
			const Triangle &t = *it;
			schreiben << t.v1.x << " ";
			schreiben << t.v1.y << " ";
			schreiben << t.v1.z << " ";
			schreiben << t.v2.x << " ";
			schreiben << t.v2.y << " ";
			schreiben << t.v2.z << " ";
			schreiben << t.v3.x << " ";
			schreiben << t.v3.y << " ";
			schreiben << t.v3.z << " ";
			schreiben << "\n";
		}
	}
	schreiben.close();
}

void Grid::Save(const char *filename) const {
	mystring fn;
	if(filename) fn = mystring(filename).file_base();
	else   		 fn = "Tri_List_out";
	SurfaceMesh out;
	for (list<Triangle>::const_iterator it = TriangleList.begin(); it != TriangleList.end(); it++) {
		const Triangle &t = *it;
		size_t i1 = out.append_point_uniquely(t.v1);
		size_t i2 = out.append_point_uniquely(t.v2);
		size_t i3 = out.append_point_uniquely(t.v3);


		out.append_face(Face(i1,i2,i3));
	}
//	out.SmoothSurfaceLaplacianHC(1, .3, 0.);
//	out.SmoothSurfaceLaplacian(1);

	out.save_mesh(fn.c_str());
}

/** \fn void Grid::LoadTriangleList(const char * const)
 * \brief Loads triangles from a file.
 *
 *	This method will load triangles from a file with given name (and path).
 *	The result is at the end in TriangleList.<br>
 *	Previous triangles in TriangleList are cleared.<br>
 *	This method will return without doing something in the case the filename is
 *	empty, the file couldn't be opened the file is empty or an error ouccoured.
 *
 * \param filename: A reference to a string that contains name and path of the
 *				file that should be load.
 */
void Grid::LoadTriangleList(const char *filename) {
	if (strlen(filename) == 0) return;

	FILE *f = NULL;
	f = fopen(filename, "r");//!< Try to open the file.

	double vals[9];
	for(size_t i = 0; i < 0; i++) vals[i] = 0.0;

	if (! f) { //! Error and return if the file could not be opened.
		cerr << "Could not open " <<filename<< endl;
		return;
	}

	char test[1024] = "#";//!< Buffer variable.

	/* Read a line from the file. If a NULL-pointer was returned by fgets either
	 * the end of file is reached or an error occoured.
	 */
	if(NULL == fgets(test,1024, f)) {
		if(0 != ferror(f)) {
			// Here one could handle the error.
			return;
		}

		if(0 != feof(f)) {
			// Here one could handle the end-of-line.
			return;
		}

		// Something unexpected has happened, so return.
		return;
	}

	TriangleList.clear();

	while (fgets(test,1024, f)) {
		if (test[0] == '#') continue;//!< Skip comment and parameter lines
		sscanf(test, "%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", &vals[0], &vals[1], &vals[2],
		       &vals[3], &vals[4], &vals[5],
		       &vals[6], &vals[7], &vals[8]);//!< Read a line with data.

		Triangle T( Point(vals[0], vals[1], vals[2]),
		            Point(vals[3], vals[4], vals[5]),
		            Point(vals[6], vals[7], vals[8]) );
		TriangleList.push_back(T);
	}
}

}
