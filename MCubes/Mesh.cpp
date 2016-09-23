//		./MCubes/Grid.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Arash Azhand <azhand@itp.tu-berlin.de>
//		Copyright 2011 Philipp Gast <avatar81@itp.tu-berlin.de>
//
//		This file is part of Virtual Lab.
//
//		Virtual Lab is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		Virtual Lab is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with Virtual Lab; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.

#include <cstring>
#include <fstream>
//#include <iostream>

#include "Mesh.hpp"

namespace MCubes {

/** \fn Mesh::Mesh(const SurfaceMesh &mesh)
 *	\brief  Intitializes data stucture for finding iso-surfaces in irregulear
 * 			meshes.
 */
Mesh::Mesh(const SurfaceMesh &mesh)
	:	 SurfaceMesh(mesh) {
	TriangleList.clear();
}

/** \fn Mesh::~Mesh()
 *	Destructor, frees the memory the constructor has reserved.
 */
Mesh::~Mesh() {

}

/** \fn void Mesh::RunAlgorithm(const double * const, const double)
 * \brief Compute the isosurface.
 *
 *	This method will compute the isosurface within the given data. The
 *	trigangles are at the end in \a TriangleList.
 *
 * \param data: Pointer to doubles, which contain the values. It is assumed that
 *				within the given 1d-array are the three dimensional data.
 * \param iso: The value that should be used for the isosurface.
 */
void Mesh::RunAlgorithm(const double * const data, const double iso) {

//	if (SurfaceMesh::dim != 3) throw throw myexception("MCubes::Mesh::IsoSurface(): Cannot proceed, the mesh is not 3D !");
	Iso_Value = iso;
	TriangleList.clear();


	size_t i = 0;
	int numtri = 0;
//	#ifdef _OPENMP /** Compute in parallelized manner*/
//	#pragma omp parallel for private(i, numtri)
//	#endif
	for (i = 0; i < elements(); i++) {
		if (f[i].size() != 4) continue;

		numtri = PolygoniseTri(data, f[i].v[0],f[i].v[1], f[i].v[2], f[i].v[3]);
//		if (numtri > 0){
//			cout << numtri << " >> " << TriangleList.size() << endl;
//		}
	}
}

/*
   Polygonise a tetrahedron given its vertices within a cube
   This is an alternative algorithm to polygonise a grid.
   It results in a smoother surface but more triangular facets.

                      + 0
                     /|\
                    / | \
                   /  |  \
                  /   |   \
                 /    |    \
                /     |     \
               +-------------+ 1
              3 \     |     /
                 \    |    /
                  \   |   /
                   \  |  /
                    \ | /
                     \|/
                      + 2

   It's main purpose is still to polygonise a gridded dataset and
   would normally be called 6 times, one for each tetrahedron making
   up the grid cell.
   Given the grid labelling as in PolygniseGrid one would call
      PolygoniseTri(grid,iso,triangles,0,2,3,7);
      PolygoniseTri(grid,iso,triangles,0,2,6,7);
      PolygoniseTri(grid,iso,triangles,0,4,6,7);
      PolygoniseTri(grid,iso,triangles,0,6,1,2);
      PolygoniseTri(grid,iso,triangles,0,6,1,4);
      PolygoniseTri(grid,iso,triangles,5,6,1,4);
*/
size_t Mesh::PolygoniseTri(const double *data,size_t v0,size_t v1,size_t v2,size_t v3)
{
	/*
		Determine which of the 16 cases we have given which vertices
		are above or below the isosurface
	*/
	int triindex = 0;
	size_t numtri = 0;
	Point &p0 = p[v0];
	Point &p1 = p[v1];
	Point &p2 = p[v2];
	Point &p3 = p[v3];
	const double &d0 = data[v0];
	const double &d1 = data[v1];
	const double &d2 = data[v2];
	const double &d3 = data[v3];

	if (d0 < Iso_Value) triindex |= 1;
	if (d1 < Iso_Value) triindex |= 2;
	if (d2 < Iso_Value) triindex |= 4;
	if (d3 < Iso_Value) triindex |= 8;

   /* Form the vertices of the triangles for each case */
   try {
	switch (triindex) {
	case 0x00:
	case 0x0F:
		break;
	case 0x0E:
	case 0x01:{
		Point v1 = VertexInterpolation(p0,p1,d0,d1);
		Point v2 = VertexInterpolation(p0,p2,d0,d2);
		Point v3 = VertexInterpolation(p0,p3,d0,d3);
		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		break;
	}
	case 0x0D:
	case 0x02:{
		Point v1 = VertexInterpolation(p1,p0,d1,d0);
		Point v2 = VertexInterpolation(p1,p3,d1,d3);
		Point v3 = VertexInterpolation(p1,p2,d1,d2);
		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		break;
	}
	case 0x0C:
	case 0x03:{
		Point v1 = VertexInterpolation(p0,p3,d0,d3);
		Point v2 = VertexInterpolation(p0,p2,d0,d2);
		Point v3 = VertexInterpolation(p1,p3,d1,d3);
		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		v1 = VertexInterpolation(p1,p2,d1,d2);
		TriangleList.push_back(Triangle(v3, v1, v2));
		numtri++;
      break;
	}
	case 0x0B:
	case 0x04:{
		Point v1 = VertexInterpolation(p2,p0,d2,d0);
		Point v2 = VertexInterpolation(p2,p1,d2,d1);
		Point v3 = VertexInterpolation(p2,p3,d2,d3);

		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		break;
	}
	case 0x0A:
	case 0x05:{
		Point v1 = VertexInterpolation(p0,p1,d0,d1);
		Point v2 = VertexInterpolation(p2,p3,d2,d3);
		Point v3 = VertexInterpolation(p0,p3,d0,d3);
		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		v3 = VertexInterpolation(p1,p2,d1,d2);
		TriangleList.push_back(Triangle(v1,v3,v2));
		numtri++;
		break;
	}
	case 0x09:
	case 0x06:{
		Point v1 = VertexInterpolation(p0,p1,d0,d1);
		Point v2 = VertexInterpolation(p1,p3,d1,d3);
		Point v3 = VertexInterpolation(p2,p3,d2,d3);
		TriangleList.push_back(Triangle(v1, v3, v2));
		numtri++;
		v2 = VertexInterpolation(p0,p2,d0,d2);
		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		break;
   }
	case 0x07:
	case 0x08:{
		Point v1 = VertexInterpolation(p3,p0,d3,d0);
		Point v2 = VertexInterpolation(p3,p2,d3,d2);
		Point v3 = VertexInterpolation(p3,p1,d3,d1);
		TriangleList.push_back(Triangle(v1, v2, v3));
		numtri++;
		break;
	}
	}
	} catch (Point::Exception_ZeroLength &e) {};
	return(numtri);
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
 * \param v1              : Vertex 1
 * \param v2              : Vertex 2
 * \param v1       : value of vertex 1
 * \param v2       : value of vertex 2
 *
 * \return Point with the coordinates of the cut.
 */
Point Mesh::VertexInterpolation(const Point  &p1, const Point  &p2,
								const double &v1, const double &v2 ) {

	if(((Iso_Value - v1)*(Iso_Value - v1)) < SMALL_NUM)	return p1;
	if(((Iso_Value - v2)*(Iso_Value - v2)) < SMALL_NUM)	return p2;
	if(((v1 - v2)       *(v1 - v2))        < SMALL_NUM)	return p1;

	double diff = (Iso_Value - v1) / (v2 - v1);

	if (diff < SMALL_NUM ) cerr << " ATTENTION "<< diff << endl;

	return p1 + diff*(p2 - p1);
}

/** \fn void Mesh::SaveTriangleList(const char * const) const
 * \brief Saves triangles to a file.
 *
 *	This method will store the triangles within a file with given name and path.
 *
 * \param filename: Reference to a string with name and path of the file in
 *				which the triangles should be stored.
 */
void Mesh::SaveTriangleList(const char *filename) const {
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

	this->Save(filename);
}

void Mesh::Save(const char *filename) const {
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

/** \fn void Mesh::LoadTriangleList(const char * const)
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
void Mesh::LoadTriangleList(const char *filename) {
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
