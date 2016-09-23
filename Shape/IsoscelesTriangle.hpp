//		IsoscelesTriangle.hpp
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


#ifndef ISOSCELESTRIANGLE_HPP
#define ISOSCELESTRIANGLE_HPP

#include "Prism.hpp"

/** \class IsoscelesTriangle
 *	\brief Class for a IsoscelesTriangle.
 *
 *	This describes a IsoscelesTriangle (a triangle with two sides of equal
 *	length), which symmetry axis is in x-direction.
 * \note This class is derived from Prism (since it is a special prism).
 */
class IsoscelesTriangle : public Prism {
	public:
		IsoscelesTriangle(Point const & onsymline, Point const & sec, double const min, double const max);
		IsoscelesTriangle(Point const & onsymline, Point const & sec);
		IsoscelesTriangle(IsoscelesTriangle const & other);
		virtual ~IsoscelesTriangle();

		//virtual void Info() const; // no own method, because the one of Prism can be used.

		//virtual void ChangeCoordinateSystem(Point const & /* orgin */, // no own method, because the one of Prism can be used.
		//Point const & /* size */,
		//Point const * const /* min_values */ = NULL,
		//Point const * const /* max_values */ = NULL,
		//bool const /* discrete */ = false) {};

		//virtual bool Inside(Point const &) const; // no own method, because the one of Prism can be used.

		virtual Shape * New() const;

	private:
		static PointCurve GenPointCurve(Point const & onsymline, Point const & sec);
		static double GenMin(Point const & onsymline, Point const & sec);
		static double GenMax(Point const & onsymline, Point const & sec);
};

#endif // ISOSCELESTRIANGLE_HPP
