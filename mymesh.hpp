
//		./mymesh.hpp
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
#ifndef MYMESH_H
#define MYMESH_H

/**
  * \page mylibs
  * \section sec_mymesh Mesh and Face
  * \subsection sec_desc Description
  *
  * This file defines classes which are used by triangulate. These are dealing
  * with the FEM mesh, faces and so on ...
  */

#include <iostream>
#include <limits.h>
#include <vector>
#include <deque>
#include <list>
#include <limits>
#include <set>
#include <map>
#include <omp.h>
#include <queue>
#include "gipl.h"
#include "mymatrix.hpp"
#include "mystring.hpp"
#include "point.hpp" //!< a class for points
#include "lists.h"
#include "maps.h"
#include "myline.hpp"
#include "NeighbourSearch.hpp"
#include "Facet.hpp"
#include "cmdline.hpp"
#include "BoundingBox.hpp"
#include "units.hpp"
#include "Profiler.hpp"
#include <time.h>

#ifndef print
#ifdef DEBUG
	#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#else
	#define print(x) (cout << x << endl)
#endif
#endif


//--------------------------- C l a s s e s ------------------------------------
namespace mymesh{
	mystring get_valid_line(fstream &f);
}
/**
 *  \class Face
 *	\brief Class for dealing with faces in finite element meshes. Saves indices
 * 	of point, attributes and a boundary value.
 **/
class Face {
	public:
		vector<int> v;		// indices of points at the edges
		int attribute;		// a attribute for a face , can be a region
		int bnd;			// boundary marker

		Face(){
			bnd = 0;
			attribute = 0;
			}

		Face(int a, int b, int c, int att = 0){
			v.push_back(a);
			v.push_back(b);
			v.push_back(c);
			this->attribute = att;
		}

		size_t size(){return this->v.size();}
		int a(){return v[0];}
		int b(){return v[1];}
		int c(){return v[2];}
		int operator[](size_t index){return this->v[index];}

		void info(){
			cout << "[ ";
			for (size_t i = 0; i < v.size(); i++){
				cout.width(6);
				cout << v[i] << " ";
			}
			cout << "]"<<endl;
		}

};

typedef vector<double>	Attributes;
typedef myList<int> 	neighbours;
typedef set<int>		nbset;
/** \class SurfaceMesh
 * Class for triangular meshes.
 */
class SurfaceMesh{

	private:
		enum ElemType {hybrid, triangles, tets};

	public:
		class Region {
			public:
				Point pos;
				int attribute;
				double constraint;

				Region(double x, double y, double z, int attribute, double constraint){
						this->pos.x = x;
						this->pos.y = y;
						this->pos.z = z;
						this->attribute = attribute;
						this->constraint = constraint;
					}
				Region(const Point p, int attribute = 0, double constraint = 0.){
						this->pos = p;
						this->attribute = attribute;
						this->constraint = constraint;
					}
		};

		class Deformation {

			friend class SurfaceMesh;
			public:
				Deformation(vector<mystring> meshes);
				~Deformation();
				SurfaceMesh& bwd(size_t i){
					if (i < deform_bwd.size()) return *deform_bwd[i];
					else throw myexception("Index out of bounds");
				}
				SurfaceMesh& fwd(size_t i){
					if (i < deform_fwd.size()) return *deform_fwd[i];
					else throw myexception("Index out of bounds");
				}

				bool inside(const Point &pt, const size_t &index);

				void initInterpolation(const size_t source_index=0, const size_t target_index=0);
				void interpolate_deformation_vectors_bwd(vector<Point> &vectors, const vector<Point> &pts);
				void interpolate_deformation_vectors_fwd(vector<Point> &vectors, const vector<Point> &pts);
				void interpolate_deformation_vectors(vector<Point> &vectors, const vector<Point> &pts);

			private:
				vector<SurfaceMesh*> deform_bwd;
				vector<SurfaceMesh*> deform_fwd;
				size_t source;
				size_t target;
				size_t diff_fwd;
				size_t diff_bwd;

		};

	public:
		vector<Point> p;	//!< save all point coordinates
		vector<Face>  f;	//!< all faces/ elements
		vector<myLine>l;	//!< all boundary lines
		list<int> vtx; 	//! vertex list = index list of extracellular vertices

	private:
		vector< Attributes > *p_attributes; //!< attributes for points
		size_t nr_attributes;

		map<uint,int> 						pointmap;	//!< cf. append_point(Point, idx) red black tree for unified appending
		map<Point,size_t, PointCompare> point_rbtree;	//!< red black tree for storing Points with an (integer) attribute
		myList<Curve> 						 borders;	//!< list of border segments
		myList<int> 						 regions;	//!< collects all face attributes
		vector<neighbours> 				 tetlist;	//!< lists all tets each point is part of
		vector<neighbours> 				  facenb;	//!< lists all the faces neighbouring each face
		vector<nbset>						      nb;	//!< neigbour list
		size_t 							     dim;	//!< dimension of the mesh 1D-4D
		bool 								   debug;
		ElemType							elemtype;	//!< knows which type of mesh
		mylibs::NeighbourSearch::NSearch	*pSearch;	//!< Pointer for searching algoritm

	public:
		myList<Region> region_def; 	//!< holds all region definitions

		SurfaceMesh():
					p_attributes(0), nr_attributes(0), nb(0), dim(3),
					debug(true), elemtype(hybrid),
					pSearch(NULL) {
			clear(); 						// initialize
		}

		SurfaceMesh(const char * const filename, bool verbose = true)
			:	p_attributes(0),
				nr_attributes(0),
				nb(0),
				dim(3),
				debug(verbose),
				elemtype(hybrid),
				pSearch(NULL) {

			clear(); 						// initialize
			read_mesh(filename);
		}

		SurfaceMesh(const string filename, bool verbose = true)
			:	p_attributes(NULL),
				nr_attributes(0),
				nb(0),
				dim(3),
				debug(verbose),
				elemtype(hybrid),
				pSearch(NULL){

			clear(); 						// initialize
			read_mesh(filename.c_str());
		}

		virtual ~SurfaceMesh(){clear();} 			// destructor

		size_t points() const {return p.size();};				// # of points
		size_t faces()  const __attribute_deprecated__ {return f.size();};				// # of faces (tetras)
		size_t elements() const {return f.size();};				// # of elements (tetras/ triangles)
		size_t lines()  const {return l.size();};				// # of lines

		size_t attributes() const {return nr_attributes;} // the # of attributes (double)
		void attributes(size_t nr){
			delete p_attributes;
			p_attributes = 0;
			vector<double> tmp;
			p_attributes = new vector<Attributes>(points(), tmp);

			for (size_t i = 0; i < points(); i++)
				p_attributes->at(i).assign(nr, 0.);
			nr_attributes = nr;
		}
		double& attribute(size_t point, size_t att){ // get one particular attribute
			if (nr_attributes > 0)
				return p_attributes->at(point)[att];
			else
				throw myexception("No (double valued) attributes are set.");
		}
		void set_attribute(size_t point, size_t att, double value){ // set one particular attribute
			if (nr_attributes > 0)
				p_attributes->at(point)[att] = value;
			else
				throw myexception("No (double valued) attributes are set.");
		}

		size_t dimension() const {return dim;}

		int  append_point(Point &point, uint idx);

//		template <class T>
//		int  append_point(Point &point, map<T> &);
		size_t append_point_uniquely(const Point &point);
		void append_point(const Point &point);

		Point * points2array() const;

		bool append_face(const Face &face);
		bool append_line(const myLine &line);
		bool append_region(const Region reg);
		void set_region_at_vtxID(const int vertexID, const int regionID,  const int only_region  = -1);
		void exchange_face_attribute(const int from, const int to);

		void bounding_box(Point &min, Point &max) const;
		mylibs::BoundingBox bounding_box() const;
		mylibs::BoundingBox bounding_box(const list<int> &pts) const;

		bool read_mesh(const char *fn);
		bool read_pts(const char *fn);
		bool read_tetras(const char *fn, int nr_edges = 4);
		void read_vtx(const char *fn);
		static bool read_pts(list<Point> &plist, const char *fn);
		static bool read_pts(vector<Point> &pvec, const char *fn);
		static bool read_vecdirs(vector<Point> &pvec, const char *fn);
		static size_t read_vec(vector<Point> &vec_ctrs, vector<PointSpherical> &vec_dirs, const char *fn);
		static size_t read_vec(vector<Point> &vec_ctrs, vector<Point> &vec_dirs, const char *fn);

		bool save_pts(const char *fn);
		bool save_pts(mystring fn);
		bool save_tetras(const char *fn);
		bool save_tetras(mystring fn);
		bool save_tri(const char *fn);
		bool save_elem(const char *fn);

		//! \todo Move SurfaceMesh::save_simple_lon to CardiacMesh::save_simple_lon()
		bool save_simple_lon(const char *fn, int region) __attribute_deprecated__;
		bool save_mesh(const char *basename);

		bool save_off(const char *fn, int region=-1);
		bool save_smesh(const char *fn, int region=-1);
		bool save_smesh(mystring fn, int region=-1);
		void save_vect(const char*fn, vector<Point> &vect);

		const Point& point(size_t idx);
		const nbset& neighbors(size_t idx);
		const Face& face(size_t idx) const;
		const mylibs::Facet facet(size_t idx) const;


		size_t index_of_point(Point p);
		const myList<int>& scan_for_regions();
		map<int,double> region_statistics(mystring source_unit=mystring("um"), mystring output_unit=mystring("um"));
		void clear();
		size_t determine_dimension();

		LineMap find_border_segments(int region=-1);
	private:
		map< list<int> , pair<int,int> > compute_surface_i(const set<int> &region);
	public:
		SurfaceMesh compute_surface(int region=-1);
		SurfaceMesh compute_surface(const set<int> &region);
		myList<PointCurve> find_border();

		list<int> find_border_nodes(int region=-1) __attribute_deprecated__;
		list<int> boundary_nodes(int region=-1);

		void compute_borders2D(int force_region=-1);
		void pseudo3d(double height, double resolution, int force_region=-1);

		Point centroid(size_t ele_idx); //!< compute centroid of a face, tetrahedron
		Point* centroids();
		Point center_of_mass() const;

		void smooth_borders2D();
		void SmoothSurfaceLaplacian(size_t num_iterations);
		void SmoothSurfaceLaplacianHC(size_t num_iterations, double alpha, double beta);
		void info(const char* s = NULL) const;

		void compute_min_max(Point &min, Point &max) __attribute_deprecated__;
		template <class T> T interpolate(const Point &pt, const T *values);
		template <class T> const T* interpolate(const vector<Point> &pts, const T *values);

		void interpolate_deformation(vector<Point> &def, vector<Point> &nodes, Profiler &prof);

		template <class T>
		mymatrix<T>* convert_to_mymatrix(	const T *values,
											const int nr_frames,
											vector<double> &resolution,
											int max_iterations=1000,
											SurfaceMesh::Deformation *deformation=NULL,
											const size_t mesh_idx = 0,
											const size_t reference_idx = 0);
		template <class T>
		mymatrix<T>* convert_to_mymatrix(	const T *values,
											const int nr_frames,
											my_regular_grid &grid,
											int max_iterations=1000,
											SurfaceMesh::Deformation *deformation=NULL,
											const size_t mesh_idx = 0,
											const size_t reference_idx = 0);

		double Volume(int facet);
		static double Volume(vector<Point> points);
		static double Volume(vector<Point*> points);
		bool inside(const Point &pt, const int &facet, vector<double> &D, double tol=1.E-5);
		bool inside(const Point &pt, vector<double> &D, double tol=1.E-5);
		size_t find_element(const Point &pt, vector<double> &D, double tol=1.E-5);
		size_t find_closest_element(const Point &pt, vector<double> &D, double tol=1.E-5);
		size_t find_closest_element(const Point &pt);
		size_t find_closest_node(Point &pt);

		list<Point> intersection(mylibs::Line line) const __attribute__ ((deprecated));
		list<Point> boundary_intersections(mylibs::Line line) const;

		vector<double> barycentric_coordinates(const Point &pt, const int &facet);
		void fprintf_barycentric_coordinates(FILE *f, vector<double> D) const;

		void pclear();
		void clear_rb(){ // clear temporarily used red black trees
			point_rbtree.clear();
			pointmap.clear();
		}

		class Exception_InterpolationFailure :public myexception {
		public:	Exception_InterpolationFailure(std::string id) :	myexception(id+"Interpolation failed."){}
		};
		class Exception_IOFailure :public myexception {
		public:	Exception_IOFailure(std::string id) :	myexception(id+"IO failure."){}
		};

	private:
		bool read_node(const char *fn);
		bool read_ele (const char *fn);
		bool read_poly(const char *fn);
		void compute_tetlist();
		void compute_face_neighbour_list();
		void compute_neighbour_list();

};

/** \fn template <class T> T SurfaceMesh::interpolate(const Point &, const T *)
 *
 * Searches the appropriate element for a given point pt and computes
 * the barycentric interpolation.
 * \todo check this function
 * @param pt : 		Point , for which the interpolation is done.
 * @param values :  array of all possible values at the nodes,
 * 					which must be of equal length as array of points
 */
template <class T>
T SurfaceMesh::interpolate(const Point &pt, const T *values){

	vector<double> D; // to store barycentric coords
	for (size_t i = 0; i < this->elements(); i++){ // run through all faces
		if (this->inside(pt, i, D)){		 // find surrounding face
			Face &t = this->f[i];
			/** barycentric interpolation
			  * Assume the points A,B,C,D are given with some function
			  * values \f$ f(A), f(B), f(C) \f$ and \f$ f(D) \f$ .
			  * A point p can be constucted from the barycentric
			  * coordinates \f$ D_1 \f$ - \f$ D_4 \f$ as:
			  * \f$ p = D_1 A + D_2 B + D_3 C + D_4 D \f$ .
			  * Then the barycentric_interpolation does the following:
			  * \f$ f(p) = D_1 f_A + D_2 f_B + D_3 f_C + D_4 f_D \f$
			  */
			T interp = T();
			for (int k = 0; k < (int) t.size(); k++){
				T val = values[t[k]] * D[k+1];
				interp += val ;
				//interp += values[t[k]] * D[k+1];
			}
			return interp;
		}
	}
	stringstream ss;
	ss << " ATTENTION : No interpolation done for "
	   << "pt = "<< pt << " ."<< endl;
	throw SurfaceMesh::Exception_InterpolationFailure(ss.str());
}

template <class T>
/** \fn template <class T> const T* SurfaceMesh::interpolate(const vector<Point> &, const T *)
 *
 * Compute an interpolation for a set of points.
 *
 * @param pts	  : Vector of points to be interplated
 * @param values : Values that are used for interpolation. The length
 * 					of this array must equal the number of nodes in the
 * 					mesh (this->p.size()).
 * @return		 : An array of template type T is returned.
 * 					\attention The user has to free the memory using delete[].
 */
const T* SurfaceMesh::interpolate(const vector<Point> &pts, const T * values){
	T *vals = new T[pts.size()];
	size_t cnt = 0;
	for (vector<Point>::iterator it = pts.begin(); it != pts.end(); it++){
		vals[cnt] = interpolate(*it, values);
		cnt++;
	}

	return vals;

}

template <class T>
/** \fn template <class T> mymatrix<T>* SurfaceMesh::convert_to_mymatrix(const T *,
 * 																			const int,
																			vector<double> &,
																			int,
																			SurfaceMesh::Deformation *,
																			const size_t, size_t)
 *
 * Projects the SurfaceMesh data onto a regular grid and
 * interpolates function values known for all vertices.
 * For the sake of speed the interpolation method is as follows:
 *  - First for every point in the regular mesh, the closest neighbour is searched.
 *  - Then all tetrahedra containing this neighour are checked if they surround
 *    the grid point.
 *  - If this was not successful, then in the tetrahedra belonging to the next
 *    neighbouring points is, searched and so on. To speed up the variable
 *    max_iterations can be set to stop the algorithm after somer iterations.
 *    \attention Choosing this value to small, fastens the algorithm, but there
 *               are false negatives, for which no surrounding tetrahedron is
 *               found.
 *  - Then a barycentric interpolation is done
 *
 * \param values 	:	function values ordered by x then y then z
 * \param nr_frames	:	each frame is thought to be a complete set of
 * 						function values.Therefore the number of frames is the
 * 						number of known values per vertex (can be some time
 * 						evolution for example)
 * \param resolution: the resolution of the regular grid.
 * \param max_iterations : number of iterations (see above)
 * \param deformation    : Array of deformation vectors, one per node
 * \param mesh_idx       : Target index for the interpolation.
 * \param reference_idx  : Source index for the interpolation.
 * \return *mymatrix
 */
mymatrix<T>* SurfaceMesh::convert_to_mymatrix(	const T *values,
												const int nr_frames,
												vector<double> &resolution,
												int max_iterations,
												SurfaceMesh::Deformation *deformation,
												const  size_t mesh_idx,
												size_t reference_idx){
	if (resolution.size() != 4)
		throw myexception("Please note that the resolution is a vector containing\
 four values for the x,y,z and 't' - direction");

	uint dimx=1,dimy=1,dimz=1;

	// get maximum size of the surrounding block
	Point min, max;

	this->compute_min_max(min, max);

	dimx = (uint) ((max.x - min.x) / resolution[0]) + 1;
	dimy = (uint) ((max.y - min.y) / resolution[1]) + 1;
	dimz = (uint) ((max.z - min.z) / resolution[2]) + 1;

//	mymatrix<T> *matrix = new mymatrix<T>(dimx, dimy, dimz, nr_frames);
//	matrix->pixdim(resolution[0], resolution[1], resolution[2], resolution[3]);
//	matrix->origin(min.x, min.y, min.z, 0.);

	my_regular_grid regular_grid(dimx, dimy, dimz, nr_frames);
	regular_grid.pixdim(resolution[0], resolution[1], resolution[2], resolution[3]);
	regular_grid.origin(min.x, min.y, min.z, 0.);

	return convert_to_mymatrix(values, nr_frames, regular_grid, max_iterations, deformation, mesh_idx, reference_idx);
}

template <class T>
/** \fn template <class T> mymatrix<T>* SurfaceMesh::convert_to_mymatrix(const T *,
												const int, my_regular_grid &,
												int, SurfaceMesh::Deformation *,
												size_t mesh_idx, size_t)
 *
 * Projects the SurfaceMesh data onto a regular grid and
 * interpolates function values known for all vertices.
 * For the sake of speed the interpolation method is as follows:
 *  - First for every point in the regular mesh, the closest neighbour is searched.
 *  - Then all tetrahedra containing this neighour are checked if they surround
 *    the grid point.
 *  - If this was not successful, then in the tetrahedra belonging to the next
 *    neighbouring points is, searched and so on. To speed up the variable
 *    max_iterations can be set to stop the algorithm after somer iterations.
 *    \attention Choosing this value to small, fastens the algorithm, but there
 *               are false negatives, for which no surrounding tetrahedron is
 *               found.
 *  - Then a barycentric interpolation is done
 *
 * \param values 	:	function values ordered by x then y then z, one value per node
 * \param nr_frames	:	each frame is thought to be a complete set of
 * 						function values.Therefore the number of frames is the
 * 						number of known values per vertex (can be some time
 * 						evolution for example)
 * \param grid  	:	regular grid (dimensions, pixdim, origin).
 * 						\attention : Value of 4th dimension (e.g. time)
 * 									 is ignored.
 * \param max_iterations : number of iterations (see above)
 * \param deformation	 : list of deformation vectors, one vector per node
 * \param mesh_idx       : Determines the target for the interpolation.
 * \param reference_idx  : Determines the source for the interpolation.
 * \return mymatrix<Type>
 */
mymatrix<T>* SurfaceMesh::convert_to_mymatrix(	const T *values,
												const int nr_frames,
												my_regular_grid &grid,
												int max_iterations,
												SurfaceMesh::Deformation *deformation,
												size_t mesh_idx,
												size_t reference_idx){
	uint dimx = grid.dims(0);
	uint dimy = grid.dims(1);
	uint dimz = grid.dims(2);

	mymatrix<T> *matrix = new mymatrix<T>(dimx, dimy, dimz, nr_frames);
	matrix->pixdim(grid.pixdim());
	matrix->origin(grid.origin());

	vector<Point> p2;
	p2.push_back(Point( matrix->coords_bottom_right()));
	p2.push_back(Point( matrix->coords_top_left()));

	this->compute_neighbour_list();
	this->compute_tetlist();

//	static int c = 0;
//	c++;
//	FILE *f1, *f2, *f3;
//	char fn1[256], fn2[256], fn3[256];
//	sprintf(fn1, "%02d_before.dat", c);
//	f1 = fopen(fn1, "w");

//	sprintf(fn2, "%02d_after.dat", c);
//	f2 = fopen(fn2, "w");

//	sprintf(fn3, "%02d_outside.dat", c);
//	f3 = fopen(fn3, "w");

//	try {
	mylibs::NeighbourSearch::NSearch Search(p,p2);
	// now compute interpolation of the data vector
	size_t sz = dimx * dimy * dimz;
	size_t wstep = sz / 200; // limit the number of updates

	// apply deformation vectors ---------------------------------------
	vector<Point> def;
	vector<Point> pts; // points to be moved along deformation vectors
	if (deformation and (reference_idx != mesh_idx)){ // only if deformation is applicable
		cout << EXCEPTION_ID << "Computing deformation vectors "
			 << mesh_idx << ">>" << reference_idx << endl;
		// append all the points in the regular mesh to the list of
		// points to be deformed
		for (size_t idx = 0; idx < sz; idx++){
			size_t ix = 0,iy = 0,iz = 0, t_dummy = 0;
			matrix->index_1to4(idx,ix,iy,iz,t_dummy); // compute x,y,z indices
			Point pt = matrix->coords(ix,iy,iz);	 // and physical coordinates
			pts.push_back(pt);
		}

		deformation->initInterpolation(reference_idx, mesh_idx);
		deformation->interpolate_deformation_vectors(def,pts);
	}
	// -----------------------------------------------------------------
	#pragma omp parallel for schedule(dynamic, 1000)
	for (size_t idx = 0; idx < sz; idx++){     // for each pixel in the matrix
		Point pt;
		size_t ix = 0,iy = 0,iz = 0, t_dummy = 0; // compute x,y,z indices
		matrix->index_1to4(idx,ix,iy,iz,t_dummy);
		pt = matrix->coords(ix,iy,iz);	 // and physical coordinates

		/** If a deformation vectors are provided, then we move pt according
		 * to these vectors.
		 * Before translating pt lies within the reference mesh.
		 * After translation it is referred to the target mesh.
		 */
		if (deformation and def.size() == sz){

			// skip points that lie outside the reference mesh
			if (not deformation->inside(pt, reference_idx)) continue;
//			pt.info("before = ");
			pt += def[idx];
//			pt.info("after  = ");

			// skip points that are translated to the outside of the considered mesh.
//			if (not deformation->inside(pt, mesh_idx)) continue;
		}

		bool found = false;
		int iter = 0;
		Point *nb = Search.find_neighbour(pt);

		if  ( nb != NULL ) {
			ptrdiff_t ptrdiff =  nb - &(this->p[0]); // difference to the beginning of the array
			queue<int> qu;
			queue<int> qu_points;
			vector<bool> ok(elements(), false);
			vector<bool> ok_points(points(), false);
			size_t ok_points_count = 1;
			qu_points.push((int) ptrdiff); // push first point in the queue
			ok_points[(int) ptrdiff] = true;

			while (qu_points.size() > 0 and ok_points_count < points() and iter < max_iterations){
				iter++;
				int ptidx = qu_points.front();
				// push tets belonging to the point in the queue of faces
				for (size_t i = 0; i < tetlist[ptidx].size(); i++){
					int id = tetlist[ptidx][i];
					if (not ok[id]){
						qu.push(id);
						ok[id] = true; // push only once
					}
				}

				while (qu.size() > 0){
					int face_idx = qu.front();
					vector<double> D; // to store barycentric coords

					if ( this->inside(pt, face_idx, D) ){
						Face &t = this->f[face_idx];
						/** barycentric interpolation
						  * Assume the points A,B,C,D are given with some function
						  * values \f$ f(A), f(B), f(C) \f$ and \f$ f(D) \f$ .
						  * A point p can be constucted from the barycentric
						  * coordinates \f$ D_1 \f$ - \f$ D_4 \f$ as:
						  * \f$ p = D_1 A + D_2 B + D_3 C + D_4 D \f$ .
						  * Then the barycentric_interpolation does the following:
						  * \f$ f(p) = D_1 f_A + D_2 f_B + D_3 f_C + D_4 f_D \f$
						  */

						for (int f = 0; f < nr_frames; f++){
							uint idx = matrix->index(ix,iy,iz,f);
							T interp = 0.;
							for (int k = 0; k < (int) t.size(); k++) 			// For each node t of the element
								interp += D[k+1] * values[points()*f + t[k]]; 	// ... compute weight
							matrix->operator[](idx) = interp;
						}

						found = true;
						break;
					}
					qu.pop(); // remove the first item
				}

				if (not found){ // push neighbours in the queue of points
					// todo rewrite as for over iterators
					for(nbset::iterator it = this->nb[ptidx].begin();
										it != this->nb[ptidx].end();
										++it){
						if ( not ok_points[ *it ]){
							qu_points.push(*it);
							ok_points[ *it ] = true;
							ok_points_count++;
						}
					}
				} else break;
				qu_points.pop(); // remove the first item
			}

		}
		else throw myexception("Error: Could not find a neighbour.");


		if ( ( omp_get_thread_num() == 0 ) and
						( idx % wstep == 0 )){
			printf("\r %4.4lf %%  (%d threads)    ",
				(double)idx/sz * 100, omp_get_num_threads());
			fflush(stdout);
		}

	}
	cout << endl; // for status

//	} // end of try
//	catch(string e){
//			cout << e << endl;
//		}

//	fclose(f1);
//	fclose(f2);
//	fclose(f3);

	// clear the tetlist again
	for (size_t i = 0; i < this->tetlist.size(); i++)	tetlist[i].clear();
	this->tetlist.clear();
	this->nb.clear();
	return matrix;
}



#undef print
#endif // __MYMESH_H
