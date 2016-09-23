//		Prism.cpp
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

#ifndef PRISM_CPP
#define PRISM_CPP

#include "Prism.hpp"

/** \fn Prism::Prism(PointCurve const &, double const, double const)
 * \brief Constructs a Prism from a PointCurve and min/max z-coordinates.
 * \param basepoints: Reference to a PointCurve which give the baseline of the
 *				Prism (the polygon at the bottom/top).
 * \param min, max: The minimum and maximum value of the z-coordinate. If min >
 *				max then these values will be changed.
 */
Prism::Prism(PointCurve const & basepoints, double const min, double const max)
	:	Shape(Shape::PRISM), baseline(basepoints), zmin(min), zmax(max) {
	if(3 > baseline.size()) {
		std::stringstream err;
		err<<__FILE__<<":"<<__FUNCTION__<<"():"<<__LINE__<<":\n";
		err<<" The given PointCurve hasn't enough points for a prism. Needed are at least three, length is "<<baseline.size()<<"\n";
	}
	if(false == baseline.isflatxy()) {
		std::stringstream err;
		err<<__FILE__<<":"<<__FUNCTION__<<"():"<<__LINE__<<":\n";
		err<<" The given PointCurve hasn't the property isflatxy(). Tilted prisms are not possible.\n";
		throw Shape_exception(err.str(), Shape_exception::GENERATION_NOT_POSSIBLE_ARGUMENTS);
	}

	if(zmin > zmax) {
		double const temp(zmin);
		zmin = zmax;
		zmax = temp;
	}

	if(not baseline.circular())		baseline.append(baseline.front());
}

/** \fn Prism::Prism(Prism const &)
 * \brief Copyconstructor
 * \param other: Reference to the Prism that should be copied.
 */
Prism::Prism(Prism const & other)
	:	Shape(Shape::PRISM), baseline(other.GetBaseline()),
	    zmin(other.GetZMin()), zmax(other.GetZMax()) {
}

/** \fn Prism::Prism(PointCurve const &, double const, double const, type const)
 * \brief Constructor for derived classes.
 *
 *	This constructor can be used by the classes derived from this one, to set
 *	the correct type.
 *
 * \param basepoints: Reference to a PointCurve which give the baseline of the
 *				Prism (the polygon at the bottom/top).
 * \param min, max: The minimum and maximum value of the z-coordinate. If min >
 *				max then these values will be changed.
 * \param t: The type of the shape.
 */
Prism::Prism(PointCurve const & basepoints, double const min, double const max, type const t)
	:	Shape(t), baseline(basepoints), zmin(min), zmax(max) {
	if(3 > baseline.size()) {
		std::stringstream err;
		err<<__FILE__<<":"<<__FUNCTION__<<"():"<<__LINE__<<":\n";
		err<<" The given PointCurve hasn't enough points for a prism. Needed are at least three, length is "<<baseline.size()<<"\n";
	}
	if(false == baseline.isflatxy()) {
		std::stringstream err;
		err<<__FILE__<<":"<<__FUNCTION__<<"():"<<__LINE__<<":\n";
		err<<" The given PointCurve hasn't the property isflatxy(). Tilted prisms are not possible.\n";
		throw Shape_exception(err.str(), Shape_exception::GENERATION_NOT_POSSIBLE_ARGUMENTS);
	}

	if(zmin > zmax) {
		double const temp(zmin);
		zmin = zmax;
		zmax = temp;
	}

	if(not baseline.circular())		baseline.append(baseline.front());
}

/** \fn void Prism::Info() const
 * \brief Writes stats of this object ot standard output.
 */
void Prism::Info() const {
	std::cout<<"Printing info about Prism:\n";
	std::cout<<"\tbaseline\n";
	for(PointCurve::const_iterator it = baseline.begin(); it != baseline.end(); ++it) {
		std::cout<<"         "<<(*it)<<"\n";
	}

	std::cout<<"\tzmin     "<<zmin<<"\n";
	std::cout<<"\tzmax     "<<zmax<<"\n";
}

/** \fn bool Prism::Inside(Point const &) const
 * \brief Check whether the given point is inside the prism.
 *
 *	This function can be used to check whether a point lies inside of the prism
 *	or not. The point is considered to be inside if its projection onto the the
 *	top- or base-plane is inside the baseline and the z-coordinate is within
 *	zmin and zmax.
 *
 * \todo2 PointCurve::Inside(const Point &) has to become const, to get rid of
 *		the need for copying the baseline.
 *
 * \param p: Reference to the point, for which should be checked if it is inside
 *				the prism or not.
 * \return True if the point is inside the prism, false otherwise.
 */
bool Prism::Inside(Point const & p) const {
	Point const xy(p.x, p.y, baseline.front().z);
	PointCurve temp(this->baseline);

	return (temp.inside(xy) and ((zmin <= p.z) and (zmax >= p.z)));
}

/** \fn void Prism::ChangeCoordinateSystem(Point const &, Point const &, Point const * const, Point const * const, bool const)
 *	\brief Change the cuboid to another coordinate system.
 *
 *	This method changes the coordinates of the prism. At the end the same prism
 *	is described, but in a different coordinate system.<br>
 *	A few examples
 *	- To change from a coordinate system with
 *			\f$ (x,y,z) \in [-maxx,maxx]x[-maxy,maxy]x[-maxz,maxz] \f$ to a
 *			realtive coordinates system \f$ (x,y,z) \in [0,1]x[0,1]x[0,1] \f$
 *			set size=(1/(2maxx), 1/(2maxy), 1/(2maxz)), origin=(0.0, 0.5, 0.5),
 *			 min_values=(0, 0, 0), max_values=(1, 1, 1) and discrete=false.
 *
 * \param size: Point, containing the size of one cell (for each direction) of
 *				the old coordinate system in the units of the new one.
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
void Prism::ChangeCoordinateSystem(Point const & size,
                                   Point const & origin,
                                   Point const * const min_values,
                                   Point const * const max_values,
                                   bool const discrete) {
	{
		//! At first the units have to be changed.
		for(PointCurve::iterator it(baseline.begin()); it != baseline.end(); ++it) {
			(*it).x *= size.x;
			(*it).y *= size.y;
			(*it).z *= size.z;
		}

		zmin *= size.z;
		zmax *= size.z;
	}

	{
		//! Second step, change the orgin.
		for(PointCurve::iterator it(baseline.begin()); it != baseline.end(); ++it) {
			(*it) += origin;
		}

		zmin += origin.z;
		zmax += origin.z;
	}

	//! If the coordinate system is discrete, cast the coordinates to integers.
	if(true == discrete) {
		for(PointCurve::iterator it(baseline.begin()); it != baseline.end(); ++it) {
			(*it).x = size_t((*it).x);
			(*it).y = size_t((*it).y);
			(*it).z = size_t((*it).z);
		}

		zmin = size_t(zmin);
		zmax = size_t(zmax);
	}

	//! At least make sure that the coordinate values fit in the range [min,max] (if their where given).
	if(NULL != min_values) {
		for(PointCurve::iterator it(baseline.begin()); it != baseline.end(); ++it) {
			if((*it).x < min_values->x) (*it).x = min_values->x;
			if((*it).y < min_values->y) (*it).y = min_values->y;
			if((*it).z < min_values->z) (*it).z = min_values->z;
		}

		if(zmin < min_values->z) zmin = min_values->z;
		if(zmax < min_values->z) zmax = min_values->z;
	}

	if(NULL != max_values) {
		for(PointCurve::iterator it(baseline.begin()); it != baseline.end(); ++it) {
			if((*it).x > max_values->x) (*it).x = max_values->x;
			if((*it).y > max_values->y) (*it).y = max_values->y;
			if((*it).z > max_values->z) (*it).z = max_values->z;
		}

		if(zmin > max_values->z) zmin = max_values->z;
		if(zmax > max_values->z) zmax = max_values->z;
	}
}

#endif // PRISM_CPP
