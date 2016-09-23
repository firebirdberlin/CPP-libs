//		./MCubes/Cube.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Arash Azhand <azhand@itp.tu-berlin.de>
//		Copyright 2011 Philipp Gast <avatar81@itp.tu-berlin.de>
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
#include "Cube.hpp"

namespace MCubes {

/** name Cube::Cube
 *	Default Constructor, that need the number of variables and is setting the 8 vertex points,
 * 	vertex values of 0.0 and cube indices of 0. It is no memory reserved.
 */
Cube::Cube() {
	// initialize the single vertices
	for(size_t i = 0; i < 8; i++) {
		vertices[i] 	 = Point();
		vertex_values[i] = 0.0;
		cube_index[i] 	 = 0;
	}
}

/** name Cube::Cube
 *	Copy Constructor which is just copying a given Cube, here given by a reference to the Cube object
 * 	called other.
 *
 * \param other:	Cube object.
 */
Cube::Cube(const Cube &other) {
	for(size_t i = 0; i < 8; i++) {
		vertices[i] = other.vertices[i];
		vertex_values[i] = other.vertex_values[i];
		cube_index[i] = other.cube_index[i];
	}
}

/** name: Cube::~Cube
 *	@attention there is no memeory reserved thus no memory has to be freed up.
 */
Cube::~Cube() {
	//delete[] vertices;
	//delete[] vertex_values;
}

/** name: Cube::SetVertexCoordinates
 *	Sets the coordinates of a given vertex (index size_t i) to given coordinate array (double *coords).
 * 	Throws a Cube_exception when the i exceeds the overall number of vertices, which is of course 8.
 * @attention	the index here is the internal index from 0 to 7 and not the index that is relating to
 * 				the spatial coordinates of the vertex.
 *
 * \param i: 		size_t	->	The given index for the cube vertex (0 to 7).
 * \param coords: 	Point	->	The passed coordinate array.
 * \return void
 */
void Cube::SetVertexCoordinates(size_t i, const Point coords) {
	if(i >= 8)	throw Cube_exception("the given index i exceeds the overall number of vertices for a cube (which is 8)!!!");

	this->vertices[i] = Point(coords);
}

/** name: Cube::SetVertices
 *	Sets the single vertex coordinates by given set of 7 points, here given by reference to 7 points.
 * \param p0, p1, p2, p3, p4, p5, p6, p7:	7 given points.
 * \return void
 */
void Cube::SetVertices(Point &p0, Point &p1, Point &p2, Point &p3, Point &p4, Point &p5, Point &p6, Point &p7) {
	*(this->vertices + 0) = p0;
	*(this->vertices + 1) = p1;
	*(this->vertices + 2) = p2;
	*(this->vertices + 3) = p3;
	*(this->vertices + 4) = p4;
	*(this->vertices + 5) = p5;
	*(this->vertices + 6) = p6;
	*(this->vertices + 7) = p7;
}

/** name: Cube::SetCubeIndices
 *	Sets the single vertex indices by given set of 8 size_t values, here given by reference to 8 size_t values.
 *	It is meant the indices that relate to the spatial coordinate position of each vertex.
 * \param idx_0, idx_1, idx_2, idx_3, idx_4, idx_5, idx_6, idx_7:	8 given size_t values.
 * \return void
 */
void Cube::SetCubeIndices(size_t &idx_0, size_t &idx_1, size_t &idx_2, size_t &idx_3, size_t &idx_4, size_t &idx_5, size_t &idx_6, size_t &idx_7) {
	this->cube_index[0] = idx_0;
	this->cube_index[1] = idx_1;
	this->cube_index[2] = idx_2;
	this->cube_index[3] = idx_3;
	this->cube_index[4] = idx_4;
	this->cube_index[5] = idx_5;
	this->cube_index[6] = idx_6;
	this->cube_index[7] = idx_7;
}

/** name: Cube::SetVertexValue
 *	Sets value at the given index (size_t i) in the vertex_values array equal to the passed value (double *value).
 * Throws a Cube_exception when the i exceeds the overall number of vertices, which is of course 8.
 * @attention	the index here is the internal index from 0 to 7 and not the index that is relating to
 * 				the spatial coordinates of the vertex.
 *
 * \param i: The given index for the cube vertex value number.
 * \param data: Array, where to find the value.
 * \return void
 */
void Cube::SetVertexValue(size_t i, const double *data) {
	//! If i exeeds the cube vertex number, throw an exeption.
	if(i >= 8)	throw Cube_exception("The given index i exceeds the overall amount of cube vertices (which is 8)!!!");

	*(this->vertex_values + i) = *(data + this->cube_index[i]);
}

}
