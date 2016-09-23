//		Shape.cpp
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

#ifndef SHAPE_CPP
#define SHAPE_CPP

/** \file Shape/Shape.cpp
 * \brief Definition of Shape_exception methods.
 *
 *	This file defines the methods of the Shape_exception-class.
 */

#include "Shape.hpp"

// #############################################################################
//
// Now the definition of the exeption methods.
//
// #############################################################################

/** \fn Shape_exception::Shape_exception(std::string const &, Shape_exception::type const)
 * \brief Creates object from string and id.
 *
 * \param str: Reference to a string, giving additional information to the
 *				exception.
 * \param id: Id of the exception, is stored and used to add a error message.<br>
 *				Default value is Shape_exception::UNKNOWN
 */
Shape_exception::Shape_exception(std::string const & str, Shape_exception::type const id)
	:	s(str), i(id) {
	SetString();
}

/** \fn Shape_exception::Shape_exception(Shape_exception::type const)
 * \brief Creates object from id.
 *
 * \param id: Id of the exception, is stored and used to add a error message.
 */
Shape_exception::Shape_exception(Shape_exception::type const id)
	:	s(""), i(id) {
	SetString();
}

/** \fn void Shape_exception::SetString()
 * \brief Adding error message.
 *
 *	This method adds an error-message to the string, depending upon the
 *	exception-id.
 */
void Shape_exception::SetString() {
	switch(this->i) {
		case UNKNOWN:
			this->s += " Unspecified exception.\n";
			break;
		case GENERATION_NOT_POSSIBLE_ARGUMENTS:
			this->s += " The generation of the desired object wasn't possbile due to arguments which lack desired properties.\n";
			break;
		case GENERATION_NOT_POSSIBLE_INFO:
			this->s += " The generation of the desired object wasn't possbile due to a lack of information (this could be for example due to linear dependence of given points).\n";
			break;
		default:
			this->s += " Unclarified exception.\n";;
			break;
	}
}

#endif
