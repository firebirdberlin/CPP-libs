//		Cube.hpp
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


#ifndef CUBE_HPP
#define CUBE_HPP

/** \file Shape/Cube.hpp
 * \brief Declares Cube-class, derived from Shape.
 *
 * This file declares the Cube-class, which is derived from Shape.
 */

#include "Shape.hpp"

/** \class Cube
 *	\brief Derived from Shape, describes a cuboid, parallel to the coordinate axis.
 *
 *	This class is a child of Shape and describes a cube which edges are parallel
 *	to the coordinate axis. The cube is thereby defined through two opposite
 *	corners.
 */
class Cube: public Shape {
	public:
		Cube(Point const & edge, Point const & opposite_edge);
		Cube(Cube const & other);
		virtual ~Cube();

		virtual void Copy(Cube const & other);

		Point GetCorner() const {return corner;};
		Point GetOppositeCorner() const {return opposite_corner;};

		virtual void Info() const;

		virtual bool IsFlat() const;

		void MakeRectangle(const double zvalue = 0.);
		void OrderCornersPoints();

		virtual void ChangeCoordinateSystem(Point const & size,
		                                    Point const & orgin,
		                                    Point const * const min_values = NULL,
		                                    Point const * const max_values = NULL,
		                                    bool const discrete = false);

		virtual bool Inside(Point const & p) const;

		/** \fn Shape * Cube::New() const
		 * \brief Returns a pointer to a deep copy of this object.
		 * \return Pointer to a Shape, which is indeed a deep copy of this object.
		 */
		virtual Shape * New() const {return (new Cube(*this));};

	private:
		Point corner;
		Point opposite_corner;
};// class Cube: public Shape

#endif // CUBE_HPP
