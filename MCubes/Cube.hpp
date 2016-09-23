/*
 *		./MCubes/Cube.hpp
 *
 *		Copyright 2009 Arash Azhand <azhand@itp.tu-berlin.de>
 *
 *		This program is free software; you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation; either version 2 of the License, or
 *		(at your option) any later version.
 *
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details.
 *
 *		You should have received a copy of the GNU General Public License
 *		along with this program; if not, write to the Free Software
 *		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *		MA 02110-1301, USA.
 */

#ifndef _CUBE_HPP
#define _CUBE_HPP

/** \file MCubes/Cube.hpp
 *	\brief Declares a Cube object and the Cube_exception object.
 *
 *	This file declares Cube object Cube_exception object.
 *
 * \section Introduction
 *
 * The cube object is defined by its 8 vertices, here given by an array of 8 points. Furthermore at each
 * vertex there is defined value (type double), represented by a double array of length 8. Finally each
 * vertex holds an index of type size_t, giving its poition in an abstract 1D array. This index will allow
 * to assign the correct vertex value for each vertex of the cube from a 1D-data-array.
 *
 */

#include <mylibs/point.hpp>
#include "Triangle.hpp"

namespace MCubes {

/** \class Cube_exeption
 *	\brief A class derived from exeption, for describing and handling Vertex-errors.
 *	Overrides the what-method, to be able to pass information to the user.
 */
class Cube_exception : public exception {
	public:
		Cube_exception(string str) throw() {
			this->s =str;
		}
		Cube_exception() {
			this->s = string();
		}
		virtual ~Cube_exception() throw() {}
		virtual const char* what() const throw() {return s.c_str();};
	private:
		string s;
};

/** class: Cube
 */
class Cube {
	public:
		// ******************************************
		// *------ Constructors & Destructors ------*
		// ******************************************
		Cube();
		Cube(const Cube &other);
		~Cube();

		// ***************************************
		// ----- Object manipulating methods ----*
		// ***************************************
		void SetVertexCoordinates(size_t i, const Point coords);
		void SetVertexValue(size_t i, const double *data);
		void SetVertices(Point &p0, Point &p1, Point &p2, Point &p3, Point &p4, Point &p5, Point &p6, Point &p7);
		void SetCubeIndices(size_t &idx_0, size_t &idx_1, size_t &idx_2, size_t &idx_3, size_t &idx_4, size_t &idx_5, size_t &idx_6, size_t &idx_7);

		// ***************************************
		// ----- Object Getting methods ---------*
		// ***************************************

		/**
		 * name: Cube::GetVertex
		 * This returns the vertex i as Point and throws a Cube_exeption when the i exceeds the overall
		 * number of vertices, which is of course 8.
		 * \param i: 				size_t
		 * \return vertices[i]: 	Point
		 */
		inline Point& GetVertex(size_t i) {
			if(i >= 8)	throw Cube_exception("the given index i exceeds the overall number of vertices for a cube (which is 8)!!!");

			return this->vertices[i];
		}

		/**
		 * name: Cube::GetVertexValue
		 * This returns the value of the vertex i as double and throws a Cube_exeption when the i exceeds the overall
		 * number of vertices, which is of course 8.
		 * \param i: 				size_t
		 * \return vertexvalues[i]:	double
		 */
		inline double& GetVertexValue(size_t i) {
			if(i >= 8)	throw Cube_exception("the given index i exceeds the overall number of vertices for a cube (which is 8)!!!");

			return this->vertex_values[i];
		}

		/**
		 * name: Cube::GetCubeIndex
		 * This returns the index of the vertex i in the spatial configuration and throws a Cube_exeption when the i exceeds the overall
		 * number of vertices, which is of course 8. From this index one is able to find ot at which coordinate point in the chosen space
		 * the given vertex is located, by using the transformation functions of the space class.
		 * \param i: 				size_t
		 * \return vertexvalues[i]:	double
		 */
		size_t & GetCubeIndex(size_t i) {
			if(i >= 8)	throw Cube_exception("the given index i exceeds the overall number of vertices for a cube (which is 8)!!!");

			return this->cube_index[i];
		}



	protected:
		// ******************************************
		// *---------------Variables ---------------*
		// ******************************************
		Point vertices[8];
		double vertex_values[8];
		//! \attention	this variable cube_index[8] is important for making the value-setting easy,
		//!				when an abstract 1D-data-array is given.
		size_t cube_index[8];

	private:
};

}

#endif
