//      Facet.cpp
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


#include "Plane.hpp"
#include "Facet.hpp"

namespace mylibs {
/** name: Facet::intersection()
 * (Stefan Fruhner - 26.10.2011 13:11:11 CEST)
 *
 * Determines, if the line intersects the plane that contains the facet.
 * Then it is checked of the intersection point is bounded by pa, pb and pc.
 * The method used here relies on the fact that the sum of the internal angles
 * of a point on the interior of a triangle is 2pi, points outside the
 * triangular facet will have lower angle sums.
 * \note This method was adapted from Paul Bourke, who presents a lot of useful
 * geometric algorithms on his webpage.
 *
 * For this example compare  http://paulbourke.net/geometry/linefacet/ .
 *
 * @param line     : The Line wich intersects (or not) the plane
 * @param intersec : The Point, where the intersection coordinates shall be saved
 * @return True if there's an intersection false otherwise.
 **/
bool Facet::intersection(mylibs::Line line, Point &intersec){

	// check if there's an intersection with the plane
	if (Plane::intersection(line, intersec)) {
		// if yes
		   /* Determine whether or not the intersection point is bounded by pa,pb,pc */
		try {
			Point pa1 = pa - intersec;
			pa1.normalize();

			Point pa2 = pb - intersec;
			pa2.normalize();

			Point pa3 = pc - intersec;
			pa3.normalize();

			double a1 = pa1.dot(pa2);
			double a2 = pa2.dot(pa3);
			double a3 = pa3.dot(pa1);
			double total = (acos(a1) + acos(a2) + acos(a3));
			if (fabs(total - 2.*M_PI) > SMALL_VALUE)  return(false);

			return(true);
		} catch (Point::Exception_ZeroLength &e) {
			// in case this error occurs this means the intersection point must
			// be an edge point of the facet.
			return true;
		}

	}

	return false;
}


void Facet::info(const char *name){
	string na;
	if (name) na += name;
	na += " (Facet)";
	Plane::info(na.c_str());
}

}
