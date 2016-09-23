//		Prism.hpp
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


#ifndef PRISM_HPP
#define PRISM_HPP

#include <mylibs/myline.hpp>

#include "Shape.hpp"

class Prism : public Shape {
	public:
		Prism(PointCurve const & basepoints, double const min, double const max);

		Prism(Prism const & other);

		virtual void Info() const;

		virtual bool Inside(Point const &) const;

		virtual void ChangeCoordinateSystem(Point const & orgin,
		                                    Point const & size,
		                                    Point const * const min_values,
		                                    Point const * const max_values,
		                                    bool const discrete = false);

		inline double				GetZMin()		const {return zmin;};
		inline double				GetZMax()		const {return zmax;};
		PointCurve	const &	GetBaseline()	const {return baseline;};

		/** \fn Shape * Prism::New() const
		 * \brief Returns a pointer to a deep copy of this object.
		 * \return Pointer to a Shape, which is indeed a deep copy of this object.
		 */
		virtual Shape * New() const {return (new Prism(*this));};

	protected:
		Prism(PointCurve const & basepoints, double const min, double const max, type const t);

	private:
		PointCurve	baseline;//!< Pointcurve, which describes the shape within the xy-plane.
		double		zmin;//!< The z-slice where the prism starts.
		double		zmax;//!< The z-slice where the prism ends.
};

#endif // PRISM_HPP
