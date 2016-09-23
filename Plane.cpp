//      Plane.cpp
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

namespace mylibs {

Plane::Plane(const Point p1,const Point p2, const Point p3):
	pa(p1), pb(p2), pc(p3) {

	n = (pb - pa).cross((pc - pa));
	n.normalize();
	d = - n.x * pa.x - n.y * pa.y - n.z * pa.z;

}

void Plane::info(const char *name){
	cout << "Info for Plane ";
	if ( name ) cout << name;
	cout << endl;

	pa.info("pa");
	pb.info("pb");
	pc.info("pc");
	n.info("n ");
	cout << " d = " << d << endl;
}

/** name: Plane::intersection()
 * (Stefan Fruhner - 26.10.2011 13:11:11 CEST)
 *
 * Determine whether or not the line
 * intersects the plane defined by pa,pb,pc
 * Return true/false and the intersection point p
 *
 * The equation of the line is p = pa + mu (pb - pa)
 * The equation of the plane is a x + b y + c z + d = 0
 *                              n.x x + n.y y + n.z z + d = 0
 *
 * @param line     : The Line wich intersects (or not) the plane
 * @param intersec : The Point, where the intersection coordinates shall be saved
 * @return True if there's an intersection false otherwise.
 **/
bool Plane::intersection(Line line, Point &intersec){
	Point pab = line.p2()-line.p1();
	double denom = n.dot( pab ) ;

	/* Line and plane don't intersect */
	if (fabs(denom) < SMALL_VALUE) return false;
	double mu = - (d + n.dot(line.p1())) / denom;

	switch (line.LineType()){
		case Line::line: // easiest case ma doesn't matter
			break;
		case Line::ray: // ma must be positive for intersection
			if (mu < 0.) return false;
			break;
		case Line::segment: // ma must be in [0.,1.]
			if (mu < 0. or mu > 1.) return false;
			break;
		default: throw myexception_UnknownError();
	}

	intersec = line.p1() + mu * (pab);
	return true;
}

}
