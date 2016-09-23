//		Cube.cpp
//
//		Copyright 2011 Rico Buchholz <buchholz@klee>
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

#ifndef CUBE_CPP
#define CUBE_CPP

/** \file Shape/Cube.hpp
 * \brief Defines the methods of Cube-class.
 *
 * This file defines the methods of the Cube-class, along with it constructors
 * and destructors.
 */

#include "Cube.hpp"

/** name: Cube::Cube
 *	Constructor, generates a cube from two opposite points.
 * \param edge: A corner of the cube that should be genereated.
 * \param opposite_edge: The corner opposite to 'edge' of the cube.
 */
Cube::Cube(Point const & edge, Point const & opposite_edge)
	: Shape(Shape::CUBE), corner(edge), opposite_corner(opposite_edge) {
	OrderCornersPoints();
}

/** name: Cube::Cube
 *	Copyconstructor, generates this object as a copy of another. Therefore the
 *	Copy-method is used.
 * \param other: Reference to another Cube-object, which should be copied.
 */
Cube::Cube(Cube const & other)
	: Shape(Shape::CUBE), corner(), opposite_corner() {
	Copy(other);
}

/** name: Cube::~Cube
 *	Destructor.
 */
Cube::~Cube() {

}

/** name: Cube::Copy
 *	\brief Makes a deep copy.
 *
 *	This method makes this instance to a deep copy of the given one.
 * \param other: Reference to another Cube-object, which should be copied.
 * \return void
 */
void Cube::Copy(Cube const & other) {
	corner = other.GetCorner();
	opposite_corner = other.GetOppositeCorner();
}

void Cube::Info() const {
	std::cout<<"Info about Cube:\n";
	std::cout<<"\tfirst  corner"<<corner<<"\n";
	std::cout<<"\tsecond corner"<<opposite_corner<<"\n";
}

/** name: Cube::IsFlat
 *	Tests whether the cylinder is only a rectangle by checking if theire are
 *	coordinates of corner and opposite_corner equal.
 * \return True if there is a pair of equal coordinates, false otherwise.
 */
bool Cube::IsFlat() const {
	if(corner.x == opposite_corner.x)		return true;
	else if(corner.y == opposite_corner.y)	return true;
	else if(corner.z == opposite_corner.z)	return true;
	else									return false;
}

/** \fn void Cube::MakeRectangle(const double)
 *	\brief Flattens the cube by reducing the z-coordinate to a constant value.
 *
 *	This method makes this cube to a rectangle by setting the z-values of the
 *	corners to a given constant.
 * \param zvalue: The constant to which to set the z-values of the edgepoints.
 */
void Cube::MakeRectangle(const double zvalue) {
	corner.z = zvalue;
	opposite_corner.z = zvalue;
}

/** \fn void Cube::OrderCornersPoints()
 *	\brief Orders coordinates of the corners.
 *
 *	\image html Doc_Images/OrderEdgePointsScheme.png
 *	This method orders the coordinates (including time) of the corners, so that
 *	first point (corner) has the minimum values and the second (opposite_corner)
 *	the maximum values. So for all possible pairs of opposite edge points (like
 *	the blue and the green, in either order in the picture), at the end 'corner'
 *	will be the yellow point and 'opposite_corner' the red one.<br>
 *	At all, the described cube remains the same, but the Cube-object is changed.
 * \return void
 */
void Cube::OrderCornersPoints() {
	size_t i;
	double temp;
	for(i = 0; i < 4; ++i) {
		if(corner[i] > opposite_corner[i]) {
			temp				= corner[i];
			corner[i]			= opposite_corner[i];
			opposite_corner[i]	= temp;
		}
	}
}

/** name: Cube::ChangeCoordinateSystem
 *	\brief Change the cuboid to another coordinate system.
 *
 *	This method changes the coordinates of the corners. At the end the same
 *	cuboid is described, but in a different coordinate system.
 *	A few examples
 *	- To change from a coordinate system with
 *			\f$ (x,y,z) \in [-maxx,maxx]x[-maxy,maxy]x[-maxz,maxz] \f$ to a
 *			realtive coordinates system \f$ (x,y,z) \in [0,1]x[0,1]x[0,1] \f$
 *			set size=(1/(2maxx), 1/(2maxy), 1/(2maxz)), origin=(0.0, 0.5, 0.5),
 *			 min_values=(0, 0, 0), max_values=(1, 1, 1) and discrete=false.
 *
 * \param size: Point, containing the size of one cell (for each direction) of
 *				the old coordinate system in the units of the new one.
 * \param orgin: Point, containing the position of the orgin of the old
 *				coordinate system in the new coordinate system.
 * \param min_values: Pointer to a point, containing the lower limits for the
 *				coordinate-values. If NULL is passed (default value) is assumed,
 *				that there are no lower limits.
 * \param max_values: Pointer to a point, containing the upper limits for the
 *				coordinate-values. If NULL is passed (default value) is assumed,
 *				that there are no upper limits.
 * \param discrete: If false (default value), nothing special is done. If true
 *				it is assumed that the new coordinate system is discret and
 *				therefore the coordinates are casted to integers.
 */
void Cube::ChangeCoordinateSystem(Point const & size,
                                  Point const & orgin,
                                  Point const * const min_values,
                                  Point const * const max_values,
                                  bool const discrete) {
	{
		//! At first the units have to be changed.
		corner.x *=  size.x;
		corner.y *=  size.y;
		corner.z *=  size.z;

		opposite_corner.x *=  size.x;
		opposite_corner.y *=  size.y;
		opposite_corner.z *=  size.z;
	}

	{
		//! Second step, change the orgin.
		corner = corner + orgin;
		opposite_corner = opposite_corner + orgin;
	}

	//! If the coordinate system is discrete, cast the coordinates to integers.
	if(true == discrete) {
		corner.x = int(corner.x);
		opposite_corner.x = int(opposite_corner.x);
		corner.y = int(corner.y);
		opposite_corner.y = int(opposite_corner.y);
		corner.z = int(corner.z);
		opposite_corner.z = int(opposite_corner.z);
	}

	//! At least make sure that the coordinate values fit in the range [min,max] (if their where given).
	if(NULL != min_values) {
		if(corner.x < min_values->x) corner.x = min_values->x;
		if(corner.y < min_values->y) corner.y = min_values->y;
		if(corner.z < min_values->z) corner.z = min_values->z;

		if(opposite_corner.x < min_values->x) opposite_corner.x = min_values->x;
		if(opposite_corner.y < min_values->y) opposite_corner.y = min_values->y;
		if(opposite_corner.z < min_values->z) opposite_corner.z = min_values->z;
	}

	if(NULL != max_values) {
		if(corner.x > max_values->x) corner.x = max_values->x;
		if(corner.y > max_values->y) corner.y = max_values->y;
		if(corner.z > max_values->z) corner.z = max_values->z;

		if(opposite_corner.x > max_values->x) opposite_corner.x = max_values->x;
		if(opposite_corner.y > max_values->y) opposite_corner.y = max_values->y;
		if(opposite_corner.z > max_values->z) opposite_corner.z = max_values->z;
	}

}

/** \fn bool Cube::Inside(Point const &) const
 *	Test whether a given Point is within or at the border of this cube.
 * \param p: Reference to the point for which should be tested if it is within
 *				the cube.
 * \return True if the point is within the cube or at the border (each is
 *				greater or equal to those of corner and lower or equal to those
 *				of opposite_corner), false otherwise.
 */
bool Cube::Inside(Point const & p) const {
	if(			(corner.x <= p.x) 			and (corner.y <= p.y) 			and (corner.z <= p.z)
	            and	(opposite_corner.x >= p.x) 	and (opposite_corner.y >= p.y) 	and (opposite_corner.z >= p.z)) {
		return true;
	}

	return false;
}

#endif // CUBE_CPP
