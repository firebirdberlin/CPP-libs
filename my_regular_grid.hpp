/**      my_regular_grid.h
 *
 *      Copyright 2009 Stefan Fruhner <stefan.fruhner@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 **/


#ifndef REGULAR_GRID_H
#define REGULAR_GRID_H

#include <stdio.h>
#include "point.hpp"


/** \class my_regular_grid
 *
 * \brief Class for working with (4d) regular grids. Stores pixel sizes, the
 * location of the origin number of pixels in each dimension.
 *
 *
 **/
class my_regular_grid{
	protected:
		/* add your private declarations */
		size_t 	dimensions[4];		//!< dimensions in x,y,z and t
		size_t 	slice_size_val;		//!< size of each slice
		size_t 	nr_slices;			//!< nr_slices = number of last valid dimension
		size_t	items_val; 			//!< number of items in the matrix
		short	dimension_val;

		double 	v_pixdim[4];					//!< size of one pixels in um (?)
		double 	v_origin[4];					//!< where (0,0,0,0) resides
		bool 	pixdim_set;
		size_t	dx,dy,dz,dt; //!< distance between two elements in the grid in the array

		void init_dimension(const int dimx=1, const int dimy=1, const int dimz=1,const int dimt=1);
		void set_dimension_data();

	public:
		my_regular_grid(int dimx, int dimy=1, int dimz=1, int dimt=1);

		size_t items() {return items_val;}							//!< get the number of items
		void items(int nr) {items_val = nr;}						//!< set the number of items

		short dim( ) {return dimension_val;}						//!< get the dimension of the matrix (1,2,3,4d)

		size_t dims(const int i){return dimensions[i];};					//!< get the dimsions of direction i
		void dims(const size_t i,const size_t j,const size_t k = 1,const size_t l = 1);
		size_t d(short i);

		size_t slices(){return nr_slices;};					//!< get the number of slices
		void slices(int new_number){nr_slices = new_number;}		//!< set the number of slices (with care)
		int slice_size(){return slice_size_val;}					//!< get the size of each slice
		void slice_size(int new_size) {slice_size_val = new_size;}	//!< set the slice size manually (with care)

		Point origin(){return Point(v_origin[0],v_origin[1],v_origin[2],v_origin[3]);}	//!< get the origin as Point
		double origin(short i) const {return v_origin[i];}									//!< get the origin for each direction
		void   origin(const Point &p) {origin(p.x, p.y, p.z, p.t);}							//!< set the origin from Point
		void   origin(double i, double j, double k=0., double l=0.){					//!< set the origin from set of doubles
			v_origin[0] = i;
			v_origin[1] = j;
			v_origin[2] = k;
			v_origin[3] = l;
			return;
		}
		void  origin(vector<double> v){	//!< set the origin
			size_t c = 0;
			for (c = 0; (c < v.size() and c < 4); ++c) v_origin[c] = v[c];
			for (	  ; c < 4; ++c) v_origin[c] = 0.;
			return;
		}

		Point  pixdim(){return Point(v_pixdim[0],v_pixdim[1],v_pixdim[2],v_pixdim[3]);}	//!< get the pixdim as Point
		double pixdim(short i) const {return v_pixdim[i];}						//!< get the pixel dimensions
		void   pixdim(const Point &p) {pixdim(p.x, p.y, p.z, p.t);} 			//!< set the origin
		void   pixdim(double i, double j, double k=0., double l=0.){			//!< set the pixel dimensions
			v_pixdim[0] = i;
			v_pixdim[1] = j;
			v_pixdim[2] = k;
			v_pixdim[3] = l;
			pixdim_set = true;
			return;
		}

		void  pixdim(vector<double> pd){ //!< set the pixel dimensions
			size_t c = 0;
			for (c = 0; (c < pd.size() and c < 4); ++c) v_pixdim[c] = pd[c];
			for (	  ; c < 4; ++c) v_pixdim[c] = 0.;
			pixdim_set = true;
			return;
		}

		Point coords(size_t x,size_t y, size_t z, size_t t=0);
		Point coords_bottom_right();
		Point coords_top_left();

		size_t CoordsToIndex(Point coords);

		void info();

		bool operator==(my_regular_grid &other);
		bool operator!=(my_regular_grid &other);

		size_t index(size_t i,size_t j, size_t k, size_t l=0);
		void index_1to4(size_t index, size_t &x, size_t &y, size_t&z, size_t &t);

		class Exception_ConversionError :public myexception {
			public:	Exception_ConversionError(std::string id) :
				myexception(id+" Conversion error."){}
		};
		class Exception_InitializationError :public myexception {
			public:	Exception_InitializationError(std::string id) :
				myexception(id+" Operation cannot be continued due to missing initialization."){}
		};
};


#endif //REGULAR_GRID_H
