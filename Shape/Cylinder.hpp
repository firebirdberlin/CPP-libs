//		Cylinder.hpp
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


#ifndef CYLINDER_HPP
#define CYLINDER_HPP

/** \file Shape/Cylinder.hpp
 * \brief Declares Cylinder-class.
 *
 * This file declares the Cylinder-class, derived from Shape.
 */

#include "Shape.hpp"

/** \class Cylinder
 *	\brief Derived from Shape, describes cylinder, parrallel to z-axis.
 *
 *	This child of Shape describes a cylinder which symmetrie axis is parallel to
 *	the z-axis. The cylinder is thereby defined by its center, the radius and
 *	the total height.
 */
class Cylinder : public Shape {
	public:
		Cylinder(Point const & center_of_cylinder, double const r, double const h);
		Cylinder(Point const & center_of_cylinder, Point edge);
		Cylinder(Cylinder const & other);
		virtual ~Cylinder();

		virtual void Copy(Cylinder const & other);

		Point	GetCenter() const {return center;};
		inline double	GetRadius() const {return radius;};
		inline double	GetHeight() const {return total_height;};

		virtual void Info() const;

		bool IsFlat() const;

		void MakeCircle();

		virtual void ChangeCoordinateSystem(Point const & orgin,
		                                    Point const & size,
		                                    Point const * const min_values,
		                                    Point const * const max_values,
		                                    bool const discrete = false);

		virtual bool Inside(Point const & p) const;

		/** \fn Shape * Cylinder::New() const
		 * \brief Returns a pointer to a deep copy of this object.
		 * \return Pointer to a Shape, which is indeed a deep copy of this object.
		 */
		virtual Shape * New() const {return (new Cylinder(*this));};

	private:
		Point center;
		double radius;
		double total_height;
};// class Cylinder: public Shape

#endif // CYLINDER_HPP
