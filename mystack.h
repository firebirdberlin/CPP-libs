//		./mystack.h
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

#ifndef MYSTACK_H
#define MYSTACK_H

#include <stack>

using namespace std;

/** \class myStack
 *  \brief 	Template class for any type of stacks. This class is derived from
 * 			stack but works a bit different.
 */
template <class Type>
class myStack: public stack<Type> {
private:

public:
	myStack(){}; 	//!< constructor
	void append(Type item){	push(item);}
	size_t length(){return this->size();}

	/** takeout :
	 * Returns the first element and removes it
	 */
	Type takeout(){
			Type value = 0;

			value = stack<Type>::top();
//			typename Type itm = 0;
//			itm = this->top();	// get the top item
//			typename Type value = itm;
			stack<Type>::pop();		// remove the top item
			return value;			// return it's value
		}
};

#endif //MYSTACK
