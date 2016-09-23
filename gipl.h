/*
 *	  gipl.h
 *
 *	  Copyright 2008 Stefan Fruhner <fruhner@e82135>
 *
 *	  This program is free software; you can redistribute it and/or modify
 *	  it under the terms of the GNU General Public License as published by
 *	  the Free Software Foundation; either version 2 of the License, or
 *	  (at your option) any later version.
 *
 *	  This program is distributed in the hope that it will be useful,
 *	  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	  GNU General Public License for more details.
 *
 *	  You should have received a copy of the GNU General Public License
 *	  along with this program; if not, write to the Free Software
 *	  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *	  MA 02110-1301, USA.
 */


#ifndef GIPL_H
#define GIPL_H

#include <stdio.h>
#include <stdlib.h>

#include <typeinfo>
#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include "point.hpp"
#include "my_regular_grid.hpp"
#include "cmdline.hpp"
#include <error.h>

using namespace std;

#ifndef uint
typedef unsigned int uint;
#endif
#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef ushort
typedef unsigned short ushort;
#endif

#define GIPL_MAGIC_NUMBER1 (0xefffe9b0)
#define GIPL_MAGIC_NUMBER2 (0x2ae389b8)

/*  IMAGE TYPE DEFINITIONS  */
#define GIPL_NONE			  0
#define GIPL_BINARY			  1
#define GIPL_CHAR			  7
#define GIPL_U_CHAR			  8
#define GIPL_SHORT			 15
#define GIPL_U_SHORT		 16
#define GIPL_U_INT			 31
#define GIPL_INT			 32
#define GIPL_FLOAT			 64
#define GIPL_DOUBLE			 65
#define GIPL_C_SHORT		144
#define GIPL_C_INT			160
#define GIPL_C_DOUBLE		193
#define GIPL_SURFACE		200
#define GIPL_POLYGON		201
#define GIPL_USER_DEFINED 	255

/*  ORIENTATION DEFINITIONS (flag1)  */
#define UNDEFINED 0
#define UNDEFINED_PROJECTION 1
#define AP_PROJECTION 2
#define LATERAL_PROJECTION 3
#define OBLIQUE_PROJECTION 4
#define UNDEFINED_TOMO 8
#define AXIAL 9
#define CORONAL 10
#define SAGITTAL 11
#define OBLIQUE_TOMO 12

enum GIPL_exceptions {NO_GIPL_HEADER_FAILURE, NO_GIPL_TYPE_FAILURE,
						GIPL_FILE_OPEN_FAILURE, GIPL_NO_TYPEID, GIPL_TYPE_ERROR,
						GIPL_USERDEF_ERROR};

class GIPL_exception : public exception {

	GIPL_exceptions id;
	public:
		GIPL_exception(GIPL_exceptions i)
						: id(i) {}
		virtual const char* what() const throw(){
			switch (id){
				case NO_GIPL_HEADER_FAILURE:
					return "GIPL: No GIPL header present"; break;
				case NO_GIPL_TYPE_FAILURE:
					return "GIPL: Image type must not be GIPL_NONE"; break;
				case GIPL_FILE_OPEN_FAILURE:
					return "GIPL: File could not be opened."; break;
				case GIPL_NO_TYPEID:
					return "GIPL: Cannot return any typeid."; break;
				case GIPL_TYPE_ERROR:
					return "GIPL: Cannot handle data type."; break;
				case GIPL_USERDEF_ERROR:
					return "GIPL: For image type GIPL_USERDEF (255) not possible."; break;
				default:
					return "GIPL: Unclarified exception."; break;
			}
		};
};

/** \struct giplheader
 *
 * 	\brief Saves everything, that is contained in the header of a GIPL file.
 */

typedef struct giplheader {
	ushort	dims[4]; 				//!< size of each dimension:
									//!< (size in [x,y,z,t])
	ushort	image_type;				//!< type of data stored
	float	pixdim[4];				//!< size of one pixel in um
	char	infoline[80];			//!< some comment
	float	matrix[20];				//!< no	idea what this is for
	char	flag1, flag2;			//!< unused ?
	double	min,max;				//!< color range (min, max)
	double	origin[4];				//!< where (0,0,0,0) resides
									//!< (origin^T + (i,j,k,l)^T * pixdim^T = coordinates^T)
	float	pixval_offset, pixval_cal;//!< possible offsets
	float	userdef1,userdef2;		//!< user defined values
	uint	magic_number;			//!< the magic number
	map<short,string> TypeDesc;
	bool image_type_set;
} GIPL_Header;

/**
 * \class GIPL
 * \brief Reads an writes GIPL headers. Also does some idex computations to
 * convert indices into coordinates and vice versa. Very similar to
 * my_regular_grid.
 **/
class GIPL{
	private:
		string filename;

		GIPL_Header header;
		int		header_size_val;

		ushort	dimension_val;		// 1, 2, 3 or 4d Data ?
		//int	items_val;			// nr of items in the matrix
		//uint 	nr_slices;			// nr_slices = number of last valid dimension
		//int 	slice_size_val;		// size of each slice

		void	set_standard_header();
		bool	read_header(string fn);

		void 	set_dimension_data();

		ushort	mk_unsigned_short(uchar *ptr);
		uint 	mk_unsigned_int(uchar *ptr);
		float	mk_float(uchar *ptr);
		double	mk_double(uchar *ptr);
		void 	mk_wr_unsigned_short(ushort value, uchar *ptr);
		void	mk_wr_unsigned_int(uint value, uchar *ptr);
		void	mk_wr_float(float value, uchar *ptr);
		void	mk_wr_double(double value, uchar *ptr);

	public:
		GIPL();
		GIPL(string fn);
		GIPL(my_regular_grid &grid);

		my_regular_grid grid(){
			my_regular_grid g(dims(0), dims(1), dims(2), dims(3));
			g.pixdim(pixdim());
			g.origin(origin());
			return g;
		}

		void dims(uint i, uint j,uint k = 1,uint l = 1);			// set the size of dimensions
		void dims(int *N);											// set the size of dimensions with an array of int
		uint dims(int i){return header.dims[i];}					// get the size of dimension #i

		Point  origin(){return Point(header.origin[0],header.origin[1],header.origin[2],header.origin[3]);}	//!< get the origin as Point
		void   origin(double i, double j=0.,double k=0., double l=0.);		// set the origin
		void   origin(Point p);												// set the origin
		double origin(int i){return header.origin[i];}						// get the origin

		Point  pixdim(){return Point(header.pixdim[0],header.pixdim[1],header.pixdim[2],header.pixdim[3]);}	//!< get the pixdim as Point
		void   pixdim(double i, double j=0.,double k=0., double l=0.);		// set the pixel dimensions
		void   pixdim(Point p);												// set the origin
		double pixdim(int i){return header.pixdim[i];}						// get the pixel dimensions for (x,y,z,t) as (0,1,2,3)

		Point coords(uint x,uint y, uint z, uint t=0){						// compute image coordinates for a set of
																			// indices and return a point
			return Point(	origin(0) + x * pixdim(0),\
							origin(1) + y * pixdim(1),\
							origin(2) + z * pixdim(2),\
							origin(3) + t * pixdim(3));
		}

		Point coords_bottom_right(){										// Point at the bottom right corner of the image
			return coords(dims(0)-1,dims(1)-1,dims(2)-1,dims(3)-1);
		}

		Point coords_top_left(){											// Point at the top left corner of the image
			return coords(0,0,0,0);
		}

		void dimension(ushort dim){dimension_val = dim;};		// set the dimension
		ushort dimension(){return dimension_val;};				// get the dimension [0,1,2,3,4]

		short image_type(){return header.image_type;} 			//get the image_type

		void image_type(short type){							//set the image_type
			header.image_type = type;
			if (type == GIPL_USER_DEFINED) header.magic_number = GIPL_MAGIC_NUMBER2;
		}

		uint index(uint i=0, uint j=0, uint k=0,uint l=0);

		uint items(){
			return header.dims[0]*header.dims[1]*header.dims[2]*header.dims[3];
		}

		void print_header();
		FILE* save_header(string fn);

		int header_size(){return header_size_val;};			// get the size of the header in bytes
		void min(double val){header.min = val;}
		void max(double val){header.max = val;}


//		//set user defined values
//		void userdef1(float value){
//			if (header.image_type != GIPL_USER_DEFINED)	header.userdef1 = value;
//			else throw(GIPL_USERDEF_ERROR);
//		}
//		void userdef2(float value){header.userdef2 = value;}
//		//get user defined values
//		float userdef1(){return header.userdef1;}
//		float userdef2(){return header.userdef2;}


		/** name: GIPL::size_of_item
		 * Save the size of one data item for GIPL_USER_DEFINED,
		 * \attention The value is stored in header.userdef1. This is not
		 * standard GIPL !
		 *
		 * @param size : size as returned by sizeof()
		 **/
		void size_of_item(size_t size){header.userdef1 = (float) size;}

		/** name: GIPL::size_of_item
		 * Return the size of one item, value of userdef1 is used for this
		 * \return size
		 **/
		size_t size_of_item();

		/** name: GIPL::num_data_arrays
		 * If one wants to store different data using GIPL, one can append
		 * other data to the gipl files. These data must be of the same type
		 * and of equal length.
		 * \attention The value is stored in header.userdef2. This is not
		 * standard GIPL ! There is also no possibility to save want kind of data
		 *
		 * @param num : size as returned by sizeof()
		 **/
		void num_data_arrays(size_t num){header.userdef2 = (float) num;	}
		size_t num_data_arrays(){return (size_t)header.userdef2;}

		void set_minmax(double min, double max){header.min = min; header.max= max;}
		void infoline(const char *msg){memcpy(header.infoline, msg, 80);}
		void info_types(){
			map<short,string>::iterator it;
			for ( it= header.TypeDesc.begin() ; it != header.TypeDesc.end(); it++ )
				std::cout << (*it).first << " => " << (*it).second << std::endl;
		}

		// set the image type by given typeid
		template <class T>
		void set_image_type(T t);
};

// In C++ function templates have to be implemented in the same file
// where they are declared
// So here follow all templates

/** name: GIPL::set_image_type()
 *
 * Sets the images type according to the type of the parameter given
 * @param t : some variable thta has the type to be set
 **/
template <class T>
void GIPL::set_image_type(T t){
	if (typeid(t) == typeid(bool))	image_type(GIPL_BINARY);
	else
	if (typeid(t)== typeid(char))	image_type(GIPL_CHAR);
	else
	if (typeid(t)== typeid(uchar))	image_type(GIPL_U_CHAR);
	else
	if (typeid(t)== typeid(short))	image_type(GIPL_SHORT);
	else
	if (typeid(t)== typeid(ushort))	image_type(GIPL_U_SHORT);
	else
	if (typeid(t)== typeid(uint))	image_type(GIPL_U_INT);
	else
	if (typeid(t)== typeid(int))	image_type(GIPL_INT);
	else
	if (typeid(t)== typeid(float))	image_type(GIPL_FLOAT);
	else
	if (typeid(t)== typeid(double))	image_type(GIPL_DOUBLE);
	else
	throw(GIPL_NO_TYPEID);
}

#endif /* GIPL_H */
