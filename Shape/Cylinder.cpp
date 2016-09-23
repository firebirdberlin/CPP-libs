//		Cylinder.cpp
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

#ifndef CYLINDER_CPP
#define CYLINDER_CPP

/** \file Shape/Cylinder.cpp
 * \brief Defines the methods of Cylinder-class.
 *
 * This file defines the methods (and constructor/destructor) of the Cylinder-class.
 */

#include "Cylinder.hpp"

/** name: Cylinder::Cylinder
 *	Constructor, initializes the base-class (via default constructor) and the
 *	members of this class (using the parameters).
 * \param center_of_cylinder: The center of the cylinder that should be created.
 * \param r: The radius for the cylinder.
 * \param h: The total height of the cylinder.
 */
Cylinder::Cylinder(Point const & center_of_cylinder, double const r, double const h)
	: Shape(Shape::CYLINDER), center(center_of_cylinder), radius(r), total_height(h) {
	// Nothing to do until now.
}

/** name: Cylinder::Cylinder
 *	Constructor, initializes the base-class (via default constructor) and the
 *	members of this class (using the parameters) The center is directly given,
 *	total_height and radius are computed via the second point, which has to lie
 *	on one edge of the cylinder.
 * \param center_of_cylinder: The center of the cylinder that should be created.
 * \param edge: A reference to a point which lies on one egde of the cylinder.
 *				Is used to determine the radius and the total_height.
 */
Cylinder::Cylinder(Point const & center_of_cylinder, Point edge)
	: Shape(Shape::CYLINDER), center(center_of_cylinder), radius(1), total_height(1) {
	Point temp(edge - center_of_cylinder);
	radius = sqrt(temp.x*temp.x + temp.y*temp.y);
	total_height = 2*temp.z;
	if(0 > total_height) total_height *= -1.0;//!< Make sure, the height is greater than zero.
}

/** name: Cylinder::Cylinder
 *	Copyconstructor, generetes this object as a copy of another. Therefore the
 *	Copy-method is used.
 * \param other: Reference to another Cylinder-object, which should be copied.
 */
Cylinder::Cylinder(Cylinder const & other)
	:	Shape(Shape::CYLINDER), center(), radius(1), total_height(1) {
	this->Copy(other);
}

/** name: Cylinder::~Cylinder
 *	Destructor.
 */
Cylinder::~Cylinder() {
	// Nothing to do until now.
}

/** name: Cylinder::Copy
 *	\brief Makes a deep copy.
 *
 *	This method makes this instance to a deep copy of the given one.
 * \param other: Reference to another Cylinder-object, which should be copied.
 * \return void
 */
void Cylinder::Copy(Cylinder const & other) {
	center = other.GetCenter();
	radius = other.GetRadius();
	total_height = other.GetHeight();
}

void Cylinder::Info() const {
	std::cout<<"Printing info about Cylinder:\n";
	std::cout<<"\tcenter "<<center<<"\n";
	std::cout<<"\tradius "<<radius<<"\n";
	std::cout<<"\ttotal height "<<total_height<<"\n";
}

/** \fn bool Cylinder::IsFlat() const
 *	Tests whether the cylinder is only a circle by checking total_height.
 * \return True if the cylinder has a total_height of zero, false otherwise.
 */
bool Cylinder::IsFlat() const {
	if(0.0 == total_height)	return true;
	else					return false;
}

/** \fn void Cylinder::MakeCircle()
 *	\brief Flattens the cylinder.
 *
 *	This method makes this cylinder to a circle by setting the z-values of the
 *	center and the total_height to zero.
 */
void Cylinder::MakeCircle() {
	total_height = 0;
	center.z = 0;
}

/** name: Cylinder::ChangeCoordinateSystem
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
 * \attention To make sure your realy get the same cylinder in the new
 *			coordinate system, the sizes in x- and y-direction have to be equal,
 *			that means size.x = size.y.
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
void Cylinder::ChangeCoordinateSystem(Point const & size,
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
		total_height *= size.z;
	}

	//! Second step, change the orgin.
	center = center + orgin;

	//! If the coordinate system is discrete, cast the coordinates to integers.
	if(true == discrete) {
		for(size_t i = 0; i < 3; ++i) {
			center[i] = int(center[i]);
		}

		radius = int(radius);
		total_height = int(total_height);
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

/** \fn bool Cylinder::Inside(Point const &) const
 *	Test whether a given Point is within or at the border of this cylinder.
 *
 * \note To catch numerical errors, e.g. due to rounding, it is tested if the
 *				distance in the xy-plane is lower than the radius or if the
 *				difference is smaller than a threshold.
 *
 * \param p: Reference to the point for which should be tested if it is within
 *				the cylinder.
 * \return True if the point is within the cylinder or at the border (distance
 *				within the xy-plane is lower or equal to the radius and the
 *				absolute value of the z-difference is lower than half the
 *				total_height), fals otherwise.
 */
bool Cylinder::Inside(Point const & p) const {
	if(		( 0.00000001 > (((center.x - p.x)*(center.x - p.x) + (center.y - p.y)*(center.y - p.y)) - (radius*radius)))
	        and	(abs(center.z - p.z) <= 0.5*total_height)) {
		return true;
	} else {
		return false;
	}
}

#endif // CYLINDER_CPP
