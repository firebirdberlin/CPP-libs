//		Sphere.cpp
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

#ifndef SPHERE_CPP
#define SPHERE_CPP

/** \file Shape/Sphere.cpp
 * \brief Defines methods of Sphere-object.
 *
 * This file defines the methods of the Sphere-objects (the constructor and
 * destructor too).
 */

#include "Sphere.hpp"

/** name: Sphere::Sphere
 *	Constructor, creates the Sphere from the center and the radius.
 * \param c: The center of the sphere, given as referenze to a Point.
 * \param r: The radius of the sphere.
 */
Sphere::Sphere(Point const & c, double const r)
	: Shape(Shape::SPHERE), center(c), radius(r) {
}

/** name: Sphere::Sphere
 *	Constructor, creates the Sphere from the center and a Point on the sphere.
 *	The radius is calculated as the absolute of the difference-vector from the
 *	center to the point on the sphere.
 * \param c: The center of the sphere, given as referenze to a Point.
 * \param on_sphere: Referenze to a Point located at the sphere.
 */
Sphere::Sphere(Point const & c, Point const & on_sphere)
	: Shape(Shape::SPHERE), center(c), radius(1.0) {
	radius = (center - on_sphere).abs();
}

/** \fn Sphere::Sphere(Point const &, Point const &, Point const &, Point const &)
 * \brief Generate a sphere from four points.
 *
 *	This constructor creates a Sphere from four nonplanar Points.<br>
 *	The calculation of the center and the radius is thereby done by the
 *	following scheme:
 *	Equation of a sphere is
 * \f[
 *		r^2 = x^2 - 2 \cdot x \cdot x_m + x_m^2 + y^2 - 2 \cdot y \cdot y_m + y_m^2 + z^2 - 2 \cdot z \cdot z_m + z_m^2.
 * \f]
 *	Getting now all terms with subscript 'm' on the left and the rest on the
 *	right results in
 * \f[
 *		x_m^2 + y_m^2 + z_m^2 - r^2 - 2 \cdot x_m \cdot x - 2 \cdot y_m \cdot y - 2 \cdot z_m \cdot z = - (x^2 + y^2 + z^2).
 * \f]
 *	With \f$ A:= x_m^2 + y_m^2 + z_m^2 - r_2 \f$, \f$ B:= -2·x_m \f$,
 *	\f$ C:= -2·y_m \f$ and \f$ D:= -2·z_m \f$ on get
 * \f[
 *		A + B \cdot x + C \cdot y + D \cdot z = -(x^2 + y^2 + z^2)
 * \f]
 *	Ther are now four independent equations of this kind (because the points
 *	are nonplanar), which result in solving the linear equation system
 * \f[
 *		A + B \cdot x_1 + C \cdot y_1 + D \cdot z_1 = -(x_1^2 + y1^2 + z_1^2) \\
 *		A + B \cdot x_2 + C \cdot y_2 + D \cdot z_2 = -(x_2^2 + y2^2 + z_2^2) \\
 *		A + B \cdot x_3 + C \cdot y_3 + D \cdot z_3 = -(x_3^2 + y3^2 + z_3^2) \\
 *		A + B \cdot x_4 + C \cdot y_4 + D \cdot z_4 = -(x_4^2 + y4^2 + z_4^2).
 * \f]
 *	With solution
 *	\f[
 * A = \frac{	- x_4 y_3 z_2 r_1^2 + x_3 y_4 z_2 r_1^2 + x_4 y_2 z_3 r_1^2
 *				- x_2 y_4 z_3 r_1^2 - x_3 y_2 z_4 r_1^2 + x_2 y_3 z_4 r_1^2
 *				+ r_2^2 x_4 y_3 z_1 - r_2^2 x_3 y_4 z_1 - r_2^2 x_4 y_1 z_3
 *				+ r_2^2 x_1 y_4 z_3 + r_2^2 x_3 y_1 z_4 - r_2^2 x_1 y_3 z_4
 *				+ r_4^2 (  x_3 y_2 z_1 - x_2 y_3 z_1 - x_3 y_1 z_2 + x_1 y_3 z_2 + x_2 y_1 z_3 - x_1 y_2 z_3)
 *				+ r_3^2 (- x_4 y_2 z_1 + x_2 y_4 z_1 + x_4 y_1 z_2 - x_1 y_4 z_2 - x_2 y_1 z_4 + x_1 y_2 z_4)}
 *		{	s} \\
 * B = \frac{	  y_3 z_2 r_1^2 - y_4 z_2 r_1^2 - y_2 z_3 r_1^2 + y_4 z_3 r_1^2
 *				+ y_2 z_4 r_1^2 - y_3 z_4 r_1^2 - r_2^2 y_3 z_1 + r_2^2 y_4 z_1
 *				+ r_2^2 y_1 z_3 - r_2^2 y_4 z_3 - r_2^2 y_1 z_4 + r_2^2 y_3 z_4
 *				+ r_4^2 (- y_2 z_1 + y_3 z_1 + y_1 z_2 - y_3 z_2 - y_1 z_3 + y_2 z_3)
 *				+ r_3^2 (- y_4 z_1 - y_1 z_2 + y_4 z_2 + y_2 (z_1 - z_4) + y_1 z_4)}
 *		{	s} \\
 * C = \frac{	- x_3 z_2 r_1^2 + x_4 z_2 r_1^2 + x_2 z_3 r_1^2 - x_4 z_3 r_1^2
 *				- x_2 z_4 r_1^2 + x_3 z_4 r_1^2 + r_2^2 x_3 z_1 - r_2^2 x_4 z_1
 *				- r_2^2 x_1 z_3 + r_2^2 x_4 z_3 + r_2^2 x_1 z_4 - r_2^2 x_3 z_4
 *				+ r_4^2 (x_2 z_1 - x_3 z_1 - x_1 z_2 + x_3 z_2 + x_1 z_3 - x_2 z_3)
 *				+ r_3^2 (x_4 z_1 + x_1 z_2 - x_4 z_2 - x_1 z_4 + x_2 (z_4 - z_1))}
 *		{	s} \\
 * D = \frac{	  x_3 y_2 r_1^2 - x_4 y_2 r_1^2 - x_2 y_3 r_1^2 + x_4 y_3 r_1^2
 *				+ x_2 y_4 r_1^2 - x_3 y_4 r_1^2 - r_2^2 x_3 y_1 + r_2^2 x_4 y_1
 *				+ r_2^2 x_1 y_3 - r_2^2 x_4 y_3 - r_2^2 x_1 y_4 + r_2^2 x_3 y_4
 *				+ r_4^2 (- x_2 y_1 + x_3 y_1 + x_1 y_2 - x_3 y_2 - x_1 y_3 + x_2 y_3)
 *				+ r_3^2 (- x_4 y_1 - x_1 y_2 + x_4 y_2 + x_2 (y_1 - y_4) + x_1 y_4)}
 *		{	s} \\
 * s = x_2 y_3 z_1 - x_2 y_4 z_1 - x_1 y_3 z_2 + x_1 y_4 z_2 - x_2 y_1 z_3 + x_1 y_2 z_3 - x_1 y_4 z_3 + x_2 y_4 z_3
 *			+ x_4 (y_2 z_1 - y_3 z_1 - y_1 z_2 + y_3 z_2 + y_1 z_3 - y_2 z_3)
 *			+ x_2 y_1 z_4 - x_1 y_2 z_4 + x_1 y_3 z_4 - x_2 y_3 z_4
 *			+ x_3 (y_4 z_1 + y_1 z_2 - y_4 z_2 - y_1 z_4 + y_2 (z_4 - z_1))
 * \f]
 *	The center is now \f$ (x_m, y_m, z_m) = - 1/2(B, C, D) \f$ and \f$ r^2 = x_m^2 + y_m^2 + z_m^2 - A\f$.
 *
 * \param p1, p2, p3, p4: References to four Points, which are on the shell of
 *				the sphere and are not within a plane.
 */
Sphere::Sphere(Point const & p1, Point const & p2, Point const & p3, Point const & p4)
	:	Shape(Shape::SPHERE), center(), radius(1.0) {
	double a, b, c, d;
	double r1(p1.norm()), r2(p2.norm()), r3(p3.norm()), r4(p4.norm());
	double scaling, temp;

	temp = p2.x*p3.y*p1.z - p2.x*p4.y*p1.z - p1.x*p3.y*p2.z + p1.x*p4.y*p2.z - p2.x*p1.y*p3.z + p1.x*p2.y*p3.z - p1.x*p4.y*p3.z + p2.x*p4.y*p3.z + p4.x*(p2.y*p1.z - p3.y*p1.z - p1.y*p2.z + p3.y*p2.z + p1.y*p3.z - p2.y*p3.z) + p2.x*p1.y*p4.z - p1.x*p2.y*p4.z + p1.x*p3.y*p4.z - p2.x*p3.y*p4.z + p3.x*(p4.y*p1.z + p1.y*p2.z - p4.y*p2.z - p1.y*p4.z + p2.y*(p4.z - p1.z));
	if(0.0 == temp) {
		std::stringstream err;
		err <<__FILE__<<":"<<__FUNCTION__<<"():"<<__LINE__<<":\n";
		err <<"\tCan't generate sphere because 's' is infinit (1/s = "<<temp<<").\n";
		err <<"\t Points given: \n";
		err <<"\t\t"<<p1<<"\n\t\t"<<p2<<"\n\t\t"<<p3<<"\n\t\t"<<p4<<"\n";
		throw Shape_exception(err.str(), Shape_exception::GENERATION_NOT_POSSIBLE_INFO);
	}
	scaling = 1.0/temp;
	a = - p4.x*p3.y*p2.z*r1 + p3.x*p4.y*p2.z*r1 + p4.x*p2.y*p3.z*r1 - p2.x*p4.y*p3.z*r1 - p3.x*p2.y*p4.z*r1 + p2.x*p3.y*p4.z*r1 + r2*p4.x*p3.y*p1.z - r2*p3.x*p4.y*p1.z - r2*p4.x*p1.y*p3.z + r2*p1.x*p4.y*p3.z + r2*p3.x*p1.y*p4.z - r2*p1.x*p3.y*p4.z + r4*(p3.x*p2.y*p1.z - p2.x*p3.y*p1.z - p3.x*p1.y*p2.z + p1.x*p3.y*p2.z + p2.x*p1.y*p3.z - p1.x*p2.y*p3.z) + r3*(-p4.x*p2.y*p1.z + p2.x*p4.y*p1.z + p4.x*p1.y*p2.z - p1.x*p4.y*p2.z - p2.x*p1.y*p4.z + p1.x*p2.y*p4.z);
	a *= scaling;
	b =   p3.y*p2.z*r1 - p4.y*p2.z*r1 - p2.y*p3.z*r1 + p4.y*p3.z*r1 + p2.y*p4.z*r1 - p3.y*p4.z*r1 - r2*p3.y*p1.z + r2*p4.y*p1.z + r2*p1.y*p3.z - r2*p4.y*p3.z - r2*p1.y*p4.z + r2*p3.y*p4.z + r4*(-p2.y*p1.z + p3.y*p1.z + p1.y*p2.z - p3.y*p2.z - p1.y*p3.z + p2.y*p3.z) + r3*(-p4.y*p1.z - p1.y*p2.z + p4.y*p2.z + p2.y*(p1.z - p4.z) + p1.y*p4.z);
	b *= scaling;
	c = - p3.x*p2.z*r1 + p4.x*p2.z*r1 + p2.x*p3.z*r1 - p4.x*p3.z*r1 - p2.x*p4.z*r1 + p3.x*p4.z*r1 + r2*p3.x*p1.z - r2*p4.x*p1.z - r2*p1.x*p3.z + r2*p4.x*p3.z + r2*p1.x*p4.z - r2*p3.x*p4.z + r4*( p2.x*p1.z - p3.x*p1.z - p1.x*p2.z + p3.x*p2.z + p1.x*p3.z - p2.x*p3.z) + r3*( p4.x*p1.z + p1.x*p2.z - p4.x*p2.z - p1.x*p4.z + p2.x*(p4.z - p1.z));
	c *= scaling;
	d =   p3.x*p2.y*r1 - p4.x*p2.y*r1 - p2.x*p3.y*r1 + p4.x*p3.y*r1 + p2.x*p4.y*r1 - p3.x*p4.y*r1 - r2*p3.x*p1.y + r2*p4.x*p1.y + r2*p1.x*p3.y - r2*p4.x*p3.y - r2*p1.x*p4.y + r2*p3.x*p4.y + r4*(-p2.x*p1.y + p3.x*p1.y + p1.x*p2.y - p3.x*p2.y - p1.x*p3.y + p2.x*p3.y) + r3*(-p4.x*p1.y - p1.x*p2.y + p4.x*p2.y + p2.x*(p1.y - p4.y) + p1.x*p4.y);
	d *= scaling;
	center.set(-0.5*b, -0.5*c, -0.5*d);
	radius = sqrt(center.x*center.x + center.y*center.y + center.z*center.z - a);
}

/** name: Sphere::Sphere
 *	Copyconstructor, generetes this object as a copy of another. Therefore the
 *	Copy-method is used.
 * \param other: Reference to another Sphere-object, which should be copied.
 */
Sphere::Sphere(Sphere const & other)
	:	Shape(Shape::SPHERE), center(), radius(1.0) {
	this->Copy(other);
}

/** name: Sphere::~Sphere
 *	Destructor.
 */
Sphere::~Sphere() {

}

/** name: Sphere::Copy
 *	\brief Makes a deep copy.
 *
 *	This method makes this instance to a deep copy of the given one.
 * \param other: Reference to another Sphere-object, which should be copied.
 * \return void
 */
void Sphere::Copy(Sphere const & other) {
	center = other.GetCenter();
	radius = other.GetRadius();
}

void Sphere::Info() const {
	std::cout<<"Printing info about Sphere:\n";
	std::cout<<"\tcenter"<<center<<"\n";
	std::cout<<"\tradius"<<radius<<"\n";
}

/** name: Sphere::ChangeCoordinateSystem
 *	\brief Change the sphere to another coordinate system.
 *
 *	This method changes the coordinates of the center and the radius. At the end
 *	the same sphere is described, but in a different coordinate system.
 *	A few examples
 *	- To change from a coordinate system with
 *			\f$ (x,y,z) \in [-maxx,maxx]x[-maxy,maxy]x[-maxz,maxz] \f$ to a
 *			realtive coordinates system \f$ (x,y,z) \in [0,1]x[0,1]x[0,1] \f$
 *			set size=(1/(2maxx), 1/(2maxy), 1/(2maxz)), origin=(0.0, 0.5, 0.5),
 *			 min_values=(0, 0, 0), max_values=(1, 1, 1) and discrete=false.
 *
 * \attention To make sure your realy get the same sphere in the new coordinate
 *			system, the sizes has to be equal, that means size.x = size.y = size.z.
 *
 * \param size: Point, containing the size of one cell (for each direction) of
 *				the old coordinate system in the units of the new one. For the
 *				radius is size.x used.
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
void Sphere::ChangeCoordinateSystem(Point const & size,
                                    Point const & orgin,
                                    Point const * const min_values,
                                    Point const * const max_values,
                                    bool const discrete) {
	{
		//! At first the units have to be changed.
		center.x *=  size.x;
		center.y *=  size.y;
		center.z *=  size.z;

		radius *= size.x;
	}

	//! Second step, change the orgin.
	center = center + orgin;

	//! If the coordinate system is discrete, cast the coordinates to integers.
	if(true == discrete) {
		for(size_t i = 0; i < 3; ++i) {
			center[i] = int(center[i]);
		}

		radius = int(radius);
	}

	//! At least make sure that the coordinate values fit in the range [min,max] (if their where given).
	if(NULL != min_values) {
		if(center.x < min_values->x) center.x = min_values->x;
		if(center.y < min_values->y) center.y = min_values->y;
		if(center.z < min_values->z) center.z = min_values->z;
	}

	if(NULL != max_values) {
		if(center.x > max_values->x) center.x = max_values->x;
		if(center.y > max_values->y) center.y = max_values->y;
		if(center.z > max_values->z) center.z = max_values->z;
	}

}

/** \fn bool Sphere::Inside(Point const &) const
 *	Test whether a given Point is within or at the border of this sphere.
 * \param p: Reference to the point for which should be tested if it is within
 *				the sphere.
 * \return True if the point is within the sphere or at the border (distance to
 *				the center is equal or lower than the radius), false otherwise.
 */
bool Sphere::Inside(Point const & p) const {
	if((p - this->center).abs() <= this->radius) {
		return true;
	} else {
		return false;
	}
}

#endif // SPHERE_CPP
