//		./maps.h
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
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

#ifndef MYMAPS_H
#define MYMAPS_H

#include <iostream>
#include <string>
#include <map>

#include "mystring.hpp"
#include "lists.h"

using namespace std;

/** \class myMap
 * \brief 	Template class for any type of maps. This class is derived from
 * 			map, and has additional features, as a length()-method.
**/
template <class Type1, class Type2>
class myMap: public map<Type1, Type2> {

public:

	size_t length() const {
			return this->size();
		}

	/** \fn myList<Type1> myMap::keys() const
	 * \brief Generates and returns a myList with all keys.
	 */
	myList<Type1> keys() const {
		myList<Type1> list;
		typename map<Type1,Type2>::const_iterator i;

		for ( i = this->begin() ; i != this->end(); i++ )
			list.append(i->first);

		return list;
	}
};
#endif // MYMAPS_H
