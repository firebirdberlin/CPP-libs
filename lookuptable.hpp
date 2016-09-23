//		./lookuptable.h
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



#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <iostream>
#include "myexception.hpp"

using namespace std;

/** \class LookUpTable
 * \brief Provides a lookuptable for classes.
 *
 *	This class implements a lookupttable written as template, so it can be used
 *	for different classes.<br>
 *
 * \section sec_requirements requirements to the template-class
 *	The template class must have at least the following properties, or this
 *	class can't work properly:
 *	- A pulic method evaluate(float, float, float), whereby it is assumed, that
 *		the parameters are value, minimum and maximum, respectivley.
 *	- A default-constructor (one which takes no parameters).
 */
template<class Type>
class LookUpTable{
	public:

		LookUpTable(int const item_count, float const mini,float const maxi);
		~LookUpTable();

		Type &lookup(float const val);

		Type& operator()(float const val);

		void bounds(float lower, float upper);
		void min(float const val){min_i = val; diffm1 = (float)items/range(); step =  range()/ items;}
		void max(float const val){max_i = val; diffm1 = (float)items/range(); step =  range()/ items;}

		float min() const {return min_i;}
		float max() const {return max_i;}
		float range() const {return max_i-min_i;}
		size_t size() const {return (size_t) items;}

		void set_scale(float mi, float ma);

		float value_at(size_t index);
		Type &operator[](size_t const index);

		void info(const string name = string()) const;

	private:
		float min_i;//! minimum value
		float max_i;//! maximum value.
		float step;
		float diffm1; //!< Inverse stepsize (= items / (max-min)).
		int items;//!< number of items
		Type *list;//!< Pointer for the array with the objects.


	public: //Exceptions
		class Exception_IndexError :public myexception {
		public:	Exception_IndexError(std::string id) :	myexception(id+"Index out of range."){}
		};
};

/** name: LookUpTable::LookUpTable
 * \brief Creates the object, taking #items, minimum and maximum.
 *
 *	This is the constructor of this class. It takes three parameters,
 *	giving the number of values that should be computed, and minimum and
 *	maximum of the range. These parameters are used to initialize the
 *	corresponding parameter.<br>
 *	The members \a diffm1 and \a step are set within the constructor.
 *	Also the list with the template-objects is created and for each is
 *	the evaluate-method called.
 */
template <class Type>
LookUpTable<Type>::LookUpTable(int const item_count, float const mini,float const maxi)
	:	min_i(mini),
		max_i(maxi),
		step(1.0),
		diffm1(1.0),
		items(item_count),
		list(NULL) {
	diffm1 = (float)items/range();
	step =  range()/ (items-1.);
	list = new Type[items]; // create array with 'items' items

	for (int i=0; i<items;i++)	// insert values
		list[i].evaluate(min_i + i * step, min_i, max_i); // evaluate item to the corresponding value
}

/** name: LookUpTable::~LookUpTable
 * \brief Deletes the list.
 *
 *	This destructor deletes the list with the template objects and sets
 *	the pointer to zero.
 */
template <class Type>
LookUpTable<Type>::~LookUpTable(){
		delete[] list; // free memory
	list = NULL;
}

/** name: LookUpTable::lookup
 * \brief Get element corresponding to a value.
 *
 *	Makes the lookup, e.g. returns the object that belongs to the given
 *	value. Therefore the index that corresponds to this value is
 *	computed and the listitem is returned. If the index exceeds the
 *	limits (0/items-1) it is set to the minimum/maximum value.
 *
 * \param val: Value for which the corresponding object should be
 *				returned.
 * \return Copy of the object which corresponds to the given value.
 */
template <class Type>
Type& LookUpTable<Type>::lookup(float const val){

	int index = (int) ((val - min_i) * diffm1); //! compute the index

	if (index < 0)	index = 0;				//! val < min : minimum value is returned
	if (index >= items ) index = items-1;   //! val > max : maximum value is returned

	return list[index];
}

/** name: LookUpTable::operator()
 * \brief Get element corresponding to a value.
 *
 *	Makes the lookup, e.g. returns a reference to the object that
 *	belongs to the given value. Therefore the index that corresponds to
 *	this value is computed and the listitem is returned. If the index
 *	exceeds the limits (0/items-1) it is set to the minimum/maximum value.
 *
 * \param val: Value for which the corresponding object should be
 *				returned.
 * \return Reference to the object which corresponds to the given value.
 */
template <class Type>
Type& LookUpTable<Type>::operator()(float const val){

	int index = (int) ((val - min_i) * diffm1); //! compute the index

	if (index < 0)	index = 0;				//! val < min : minimum value is returned
	if (index >= items ) index = items-1;   //! val > max : maximum value is returned
	return list[index];
}

/** name: LookUpTable::bounds
 * \brief Adjust the bounds
 *
 *	Changes the bounds to the given values and re-computes the
 * 	members that depend on the bounds.
 *
 * \param lower:
 * \param upper: New values for minimum and maximum, respectivly.
 */
template <class Type>
void LookUpTable<Type>::bounds(float lower, float upper){
	min_i = lower;
	max_i = upper;
	diffm1 = (float)items/range();
	step = range()/ items;
}

/** set_scale()
 * A scaling to all values may be applied to the stored data.
 * Therefore, the values for the minimum and maximum need to be set.
 * \param mi: minimum
 * \param ma: maximum
 **/
template <class Type>
void LookUpTable<Type>::set_scale(const float mi,const float ma){
	min_i = mi;
	max_i = ma;

	// for the lookup the tables need to be recomputed
	diffm1 = (float)items/range();
	step =  range()/ (items-1.);

	for (int i=0; i<items;i++)	// insert values
		list[i].evaluate(min_i + i * step, min_i, max_i); // evaluate item to the corresponding value
}

/** name: LookUpTable::value_at
 * \brief Compute the value for a given index
 * \param index: Index for which to compute the value.
 */
template <class Type>
float LookUpTable<Type>::value_at(size_t index){ // compute the value at the x-axis for a given index
	if (index < (size_t) items)
		return step * index + min_i + step*0.5;

	throw Exception_IndexError(EXCEPTION_ID);
}

/** name: LookUpTable::operator[]
 * \brief Access the template-objects via index.
 *
 * \param index: Index of the template object that should be returned.
 * \return Reference to the template-object with the given index.
 */
template <class Type>
Type& LookUpTable<Type>::operator[](size_t const index){
	return list[index];
}

/** name: LookUpTable::info
 * \brief Print status of the object.
 *
 *	This method gives the status of the object, by printing the values
 *	of all the members to the standard-output.
 */
template <class Type>
void LookUpTable<Type>::info(const string name) const {
	if (name.size() == 0)
		cout <<"\n  Info for Lookuptable" << endl;
	else
		cout <<"\n  Info for " << name << endl;
		cout <<"    min   \t:\t" << min_i  << endl;
		cout <<"    max   \t:\t" << max_i  << endl;
		cout <<"    step  \t:\t" << step   << endl;
		cout <<"    diff  \t:\t" << diffm1 << endl;
		cout <<"    items \t:\t" << items  << endl;
}

#endif
