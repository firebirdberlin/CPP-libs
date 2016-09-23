/*
 * ./CardiacMesh.hpp
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
 */


#ifndef CARDIACMESH_HPP
#define CARDIACMESH_HPP

#include <assert.h>
#include "mymesh.hpp"

/** \file CardiacMesh.hpp
 *	\brief Declares the CardiacMesh class.
 *
 *	This file declares the CardiacMesh-class by defining Construcotrs and private declarations
 */

class CardiacMesh: public SurfaceMesh{

	/** \class CardiacMesh
	* \brief 	CardiacMesh inherits SurfaceMesh and serves to merge
	* 			intracellular mesh information with the global mesh.
	*
	* This class is derived from the SurfaceMesh class. It adds
	* additional information and processes in order to transform
	* intracellular property-files (such as *.transmural.pos) into a
	* fitting structure for global mesh-files (such as *.pts).
	* The sequence of points, describing a mesh,  can't be changed.
	* Once in a while you would want to fit a smaller set of points
	* into a fitting sequence for a bigger set of points.
	* As *.lon files hold intracellular information but preserve
	* [0, 0, 0]-points for extracellular-points, they hold the
	* necessary information for a transformation.
	*
	* CardiacMesh serves for getting the transmural position and twist
	* directions of any intracellular point
	*/
	public:

		CardiacMesh();

		/** \fn CardiacMesh(const char * const, bool)
		* Constructor that initializes a CardiacMesh object through a given SurfaceMesh.
		*
		 * @param filename: filename of the SurfaceMesh data structure.
		 * @param verbose: If true the verbosity of this object is increased.
		*/
		CardiacMesh(const char * const filename, bool verbose = true)
			: 	SurfaceMesh(filename, verbose), teti2e(NULL),tete2i(NULL), transmural_pos(NULL),
				nr_intra_elements(0), nr_extra_elements(0){

			basename = mystring(filename).file_strip_ext();
		}

		/** \fn CardiacMesh(const string, bool)
		* Constructor that initializes a CardiacMesh object through a given SurfaceMesh.
		*
		* @param filename: filename of the SurfaceMesh data structure
		 * @param verbose: If true the verbosity of this object is increased.
		*/
		CardiacMesh(const string filename, bool verbose = true)
			: 	SurfaceMesh(filename, verbose), teti2e(NULL),tete2i(NULL), transmural_pos(NULL),
				nr_intra_elements(0), nr_extra_elements(0){

			basename = mystring(filename).file_strip_ext();
		}

		virtual ~CardiacMesh(){
			free(teti2e);
			free(tete2i);
			free(transmural_pos);
			teti2e 			= NULL;
			tete2i 			= NULL;
			transmural_pos 	= NULL;
		}

		const mystring & GetBasename() {return basename;}

		const size_t& intra_elements() const;
		const size_t& extra_elements() const;
		const size_t& elem_i2e(const size_t &intra_index) const;
		bool save_lon(const char* fn = NULL);
		void read_lon(const mystring base = mystring());
		void read_transmural_pos(const mystring base = mystring());

		void elem2bath(size_t elemID);

		const double& transmural_position(const size_t element_index) const;

		bool save_mesh(const char *basename,const int region = -1);
		bool save_simple_lon(const char *fn, int region);



	private:
		mystring basename;
		vector<Point> 	 lon;
		size_t 	  		*teti2e;
		size_t 	  		*tete2i;
		double 			*transmural_pos;

		size_t 			 nr_intra_elements;
		size_t 			 nr_extra_elements;
};

#endif /* CARDIACMESH_HPP */
