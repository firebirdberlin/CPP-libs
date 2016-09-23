//      Facet.hpp
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


#ifndef FACET_HPP
#define FACET_HPP

#include "Plane.hpp"
namespace mylibs {
class Facet: public Plane {
	public:
		Facet(const Point p1,const Point p2, const Point p3) : Plane(p1, p2, p3) {};
		Facet(const Plane &pl) : Plane(pl) {};
		Point centroid(){ return (pa+pb+pc)/3.;}
		bool intersection(mylibs::Line line, Point &intersec);
		void info(const char *name=0);
};
}
#endif /* FACET_HPP */
