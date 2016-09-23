//		Shape.hpp
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


#ifndef SHAPE_HPP
#define SHAPE_HPP

/** \file Shape/Shape.hpp
 * \brief Decares Shape-object.
 *
 * This file declares the Shape-object.
 */

#include <sstream>

#include <mylibs/point.hpp>

/** \class Shape
 *	\brief Basis-class for different shapes.
 *
 * This class serves as a basis class from which other shapes (e.g. circles,
 * cubes) could be derived.
 */
class Shape {
	public:
		enum type {SHAPE, CUBE, CYLINDER, PRISM, SPHERE, ISOSCELESTRIANGLE};

		Shape(type t = SHAPE) : shape(t) {};
		Shape(Shape const & other) : shape(other.GetType()) {};

		virtual ~Shape() {};

		inline type GetType() const {return shape;};
		virtual void Info() const {std::cout<<"No further information about Shape.\n";};


		virtual void ChangeCoordinateSystem(Point const & /* orgin */,
		                                    Point const & /* size */,
		                                    Point const * const /* min_values */ = NULL,
		                                    Point const * const /* max_values */ = NULL,
		                                    bool const /* discrete */ = false) {};// Parameternames commented out to avoid unused values warnings.

		//void SetOrgin(Point const & pos);

		/** \fn bool Inside(Point const &) const
		 * \brief Check if the Point is inside of this Shape.
		 */
		virtual bool Inside(Point const &) const {return false;};

		/** \fn Shape * Shape::New() const
		 * \brief Returns a pointer to a deep copy of this object.
		 * \return Pointer to a Shape, which is indeed a deep copy of this object.
		 */
		virtual Shape * New() const {return (new Shape(*this));};


	private:
		type shape;// What kind of shape this is.
		//Point orgin;
};

/** \class Shape_exception
 * \brief Exception class for Shape-objects.
 *
 *	This is the exception class for the Shape-objects. It overrides the what()-
 *	method to pass information about the exception to the user.
 */
class Shape_exception : public std::exception {
	public:
		/** \enum type
		 * \brief Type of the exception.
		 */
		enum type {UNKNOWN, GENERATION_NOT_POSSIBLE_ARGUMENTS, GENERATION_NOT_POSSIBLE_INFO};

		Shape_exception(std::string const & str, type const id = UNKNOWN);
		Shape_exception(type const id);

		~Shape_exception() throw() {};

		type GetType() const {return i;};
		std::string const & GetString() const {return s;};

		char const * what() const throw() {return this->s.c_str();};
	protected:
	private:
		std::string s;
		type i;//!< Id is stored to enable specific handling.

		void SetString();
};

#endif // SHAPE_HPP
