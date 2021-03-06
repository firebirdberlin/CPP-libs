//      Plane.hpp
//
//      Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#ifndef PLANE_HPP
#define PLANE_HPP

#include "line.hpp"
#include "point.hpp"
namespace mylibs {
class Plane{
	friend class Facet;
	public:
		Plane(const Point pa,const Point pb, const Point pc);

		bool intersection(mylibs::Line line, Point &intersec);
		const Point normal(){return n;}
		void info(const char *name=NULL);
	protected:
		Point pa, pb, pc;	//!< 3 Points defining the plane
		Point n;			//!< face normal of the plane
		double d;
};
}
#endif /* PLANE_HPP */
