//		IsoscelesTriangle.cpp
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


#ifndef ISOSCELESTRIANGLE_CPP
#define ISOSCELESTRIANGLE_CPP

#include "IsoscelesTriangle.hpp"

/** \fn IsoscelesTriangle::IsoscelesTriangle(Point const &, Point const &, double const, double const)
 * \brief Construct the object with extra given height.
 *
 *	This constructor will use the given points and the mininum and maximum
 *	values to create the object.
 *
 * \param onsymline: Reference to the point which is on the symmetryline of the
 *				IsoscelesTriangle.
 * \param sec: Another edgepoint of the IsoscelesTriangle.
 * \param min, max: The minimum/maximum z-value of the IsoscelesTriangle,
 *				respectiveley.
 */
IsoscelesTriangle::IsoscelesTriangle(Point const & onsymline, Point const & sec, double const min, double const max)
	:	Prism(GenPointCurve(onsymline, sec), min, max, ISOSCELESTRIANGLE) {
}

/** \fn IsoscelesTriangle::IsoscelesTriangle(Point const &, Point const &)
 * \brief Construct the object with extracted from the points.
 *
 *	This constructor will use the given points to create the object. The minimum/
 *	maximum z-values are thereby extracted from the points.
 *
 * \param onsymline: Reference to the point which is on the symmetryline of the
 *				IsoscelesTriangle. The z-value of the point is used for
 *				determining the height of the prism.
 * \param sec: Another edgepoint of the IsoscelesTriangle. The z-value of the
 *				point is used for determining the height of the prism.
 */
IsoscelesTriangle::IsoscelesTriangle(Point const & onsymline, Point const & sec)
	:	Prism(GenPointCurve(onsymline, sec), GenMin(onsymline, sec), GenMax(onsymline, sec), ISOSCELESTRIANGLE) {

}

/** \fn IsoscelesTriangle::IsoscelesTriangle(IsoscelesTriangle const &)
 * \brief Copyconstruct.
 *
 *	This method will construct this object as deep copy of another one.
 *
 * \param other: Reference to the IsoscelesTriangle that should be copied.
 */
IsoscelesTriangle::IsoscelesTriangle(IsoscelesTriangle const & other)
	:	Prism(other) {
}

/** \fn IsoscelesTriangle::~IsoscelesTriangle()
 * \brief Destructor, has nothing to do until now.
 */
IsoscelesTriangle::~IsoscelesTriangle() {

}

/** \fn Shape * IsoscelesTriangle::New() const
 * \brief Returns a pointer to a deep copy of this object.
 * \return Pointer to a Shape, which is indeed a deep copy of this object.
 */
Shape * IsoscelesTriangle::New() const {
	return (new IsoscelesTriangle(*this));
}

/** \fn PointCurve IsoscelesTriangle::GenPointCurve(Point const &, Point const &)
 * \brief Generate a PointCurve that corresponds to the base of the IsoscelesTriangle-Prism.
 *
 *	This method will add the two given Point's a PointCurve. The third edgepoint
 *	of the triangle is also computed and then added to the PointCurve. Those is
 *	then returned.
 *
 * \param onsymline: Reference to the Point which is on the symmetryline of the
 *				IsoscelesTriangle.
 * \param sec: Another edgepoint of the IsoscelesTriangle.
 * \return PointCurve with three Points forming a IsoscelesTriangle.
 */
PointCurve IsoscelesTriangle::GenPointCurve(Point const & onsymline, Point const & sec) {
	PointCurve tmp;
	Point s(sec), t(sec);
	s.z = onsymline.z;
	t.z = onsymline.z;
	t.y = onsymline.y + (onsymline.y - sec.y);

	tmp.push_back(onsymline);
	tmp.push_back(s);
	tmp.push_back(t);

	return tmp;
}

/** \fn PointCurve IsoscelesTriangle::GenMin(Point const &, Point const &)
 * \brief Get minimum z-value.
 *
 *	This method will check the z-values of the given Point's for the minimum and
 *	return them.
 *
 * \param onsymline, sec: References to two edgepoints, from which to compute
 *				the minimum z-value.
 * \return The minimum of the z-values of the two given Points.
 */
double IsoscelesTriangle::GenMin(Point const & onsymline, Point const & sec) {
	if(onsymline.z < sec.z) return onsymline.z;
	else 					return sec.z;
}

/** \fn PointCurve IsoscelesTriangle::GenMax(Point const &, Point const &)
 * \brief Get maximum z-value.
 *
 *	This method will check the z-values of the given Point's for the maximum and
 *	return them.
 *
 * \param onsymline, sec: References to two edgepoints, from which to compute
 *				the maximum z-value.
 * \return The maximum of the z-values of the two given Points.
 */
double IsoscelesTriangle::GenMax(Point const & onsymline, Point const & sec) {
	if(onsymline.z > sec.z) return onsymline.z;
	else 					return sec.z;
}

#endif // ISOSCELESTRIANGLE_CPP
