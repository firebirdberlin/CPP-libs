//		./MCubes/Grid.hpp
//
//		Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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

#ifndef _MESH_HPP
#define _MESH_HPP

/** \file MCubes/Grid.hpp
 *	\brief Declares a Grid object and the Grid_exception object.
 *
 *	This file declares Grid object and the Grid_exception object.
 */

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <omp.h>

#include "Triangle.hpp"
#include <mylibs/mymesh.hpp>

#define SMALL_NUM  1.e-8	 // anything that avoids division overflow

namespace MCubes {

/** \class Grid
 *
 * \section Introduction
 *
 * The Grid class is the main class for performing the Marching Cubes algorithm. It inherits from
 * my_regular_grid to have the basic properties of a regular grid, such as the origin (as Point)
 * and pixel dimensions (also as Point). By this the spatial dimensions are set automatically in
 * the regular grid class and can be used directliy here.
 *
 * \section Methods list
 *
 * Constructur and destructor:
 * Grid();
 * Grid(const Point pixel_dims, size_t max_x, size_t max_y, size_t max_z);
 * ~Grid();
 *
 * Calculation methods:
 * void ConstructCubeList();
 * void SetCubeValues(Cube &cube, const double *data);
 * void RunAlgorithm(const double *data, const double iso);
 * void Polygonise(Cube &cube);
 * void VertexInterpolation(Point &v,const Point &v1,const Point &v2, const double &value_one,const double &value_two);
 * void SaveTriangleList(const string* filename);
 * void LoadTriangleList(const char* filename);
 * size_t ijk_index(size_t i, size_t j, size_t k);
 */
class Mesh : public SurfaceMesh {
	public:
		list<Triangle> TriangleList;

		// ******************************************
		// *------ Constructors & Destructors ------*
		// ******************************************
		Mesh(const SurfaceMesh &mesh);
		~Mesh();

		// ******************************************
		// *--------- Calculation methods ----------*
		// ******************************************
	private:
		size_t PolygoniseTri(const double *data, size_t v0,size_t v1,size_t v2,size_t v3);
		Point VertexInterpolation(const Point &p1,const Point &p2, const double &v1,const double &v2);
	public:
		void RunAlgorithm(const double *data, const double iso);
		void SaveTriangleList(const char *filename = NULL) const __attribute_deprecated__;
		void Save(const char *filename = NULL) const;
		void LoadTriangleList(const char *filename);

		// ***************************************
		// ----- Object Getting methods ---------*
		// ***************************************

	protected:
		// ******************************************
		// *---------------Variables ---------------*
		// ******************************************
		double Iso_Value;
};

}
#endif
