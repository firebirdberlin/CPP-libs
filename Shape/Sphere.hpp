//		Sphere.hpp
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


#ifndef SPHERE_HPP
#define SPHERE_HPP

/** \file Shape/Sphere.hpp
 * \brief Declares Sphere-class, derived from Shape.
 *
 * This file declares the Sphere-class, which is derived from Shape.
 */

#include "Shape.hpp"

/** \class Sphere
 *	\brief From Shape derived, describing a Sphere.
 *
 *	This class is derived from Shape and describes a Sphere via center and
 *	radius.
 */
class Sphere: public Shape {
	public:
		Sphere(Point const & c, double const r);
		Sphere(Point const & c, Point const & on_sphere);
		Sphere(Point const & p1, Point const & p2, Point const & p3, Point const & p4);
		Sphere(Sphere const & other);
		virtual ~Sphere();

		virtual void Copy(Sphere const & other);

		Point	GetCenter() const {return center;};
		inline double	GetRadius() const {return radius;};

		virtual void Info() const;

		virtual void ChangeCoordinateSystem(Point const & size,
		                                    Point const & orgin,
		                                    Point const * const min_values,
		                                    Point const * const max_values,
		                                    bool const discrete = false);

		virtual bool Inside(Point const & p) const;

		/** \fn Shape * Sphere::New() const
		 * \brief Returns a pointer to a deep copy of this object.
		 * \return Pointer to a Shape, which is indeed a deep copy of this object.
		 */
		virtual Shape * New() const {return (new Sphere(*this));};

	private:
		Point center;
		double radius;
};// class Sphere: public Shape

#endif // SPHERE_HPP
