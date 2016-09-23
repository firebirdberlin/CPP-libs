//		./lists.h
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

#ifndef MYLISTS_H
#define MYLISTS_H

#include <iostream>
#include <string>
#include <list>
#include <exception>
#include <string>
#include <sstream>

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

#define INDEXERROR(itm, len) 	{char error[100];\
								sprintf(error, "Index error: List index %d out of range (%lu items).", (itm), (unsigned long)(len));\
								throw string(error);}

using namespace std;

/** \class myList
 *  \brief 	Template class for any type of lists. This class is derived from
 * 			list, and has additional features, as a length()-method, and a
 * 			field operator, which can be used to iterate to a certain item.
 */
template <class Type>
class myList: public list<Type> {
public:
	myList() : last_iter(NULL), last_index(0), iterator_set(false) {};	//!< constructor
	myList(list<Type> s) {*this = s;}									//!< constructor
	myList(const size_t n) : list<Type>(n),last_iter(NULL), last_index(0), iterator_set(false) {}	//!< constructor

	list<Type> operator=(const list<Type> a) {return list<Type>::operator=(a);}
	Type& operator[](const int itm);									//!< field operator
	typename myList<Type>::iterator index_2_iter(const int index);
	void append(const Type item) {this->push_back(item);}
	void extend(myList<Type> second);
	void erase(const int index);
	void erase(typename list<Type>::iterator it) {
		list<Type>::erase(it);};
	inline size_t length() const {return this->size();}
	void prnt(const char *c = 0) const;
	void info(const char *c = 0) const {return prnt(c);};
	Type* toArray();

	Type& next();
	Type& prev();
	void pop() {this->pop_front();};							//!< pop = pop_front()
	void push( const Type &x ) {this->push_front(x);};			//!< push = push_front()
	Type take_front();
	Type take_back();
	bool contains(const Type & item) const;

	Type& at(size_t index);

private:
	typename list<Type>::iterator last_iter;	//!< last iterator used in field op.
	int last_index;								//!< index that belongs to last iterator
	bool iterator_set;

	void set_index(typename list<Type>::iterator &iter, const int index) {
		last_iter = iter;
		last_index = index;
		iterator_set = true;
	}
public: // Exceptions
	class Exception: public std::exception {
		public:
			Exception(std::string str) throw(){
				this->s =str;
			}

			Exception(std::string str, int id) throw(){
				stringstream strout;
				strout << id << " " << str;
				this->s = strout.str();
			}

			virtual ~Exception() throw(){

			}
			virtual const char* what() const throw(){
				return s.c_str();
			}

	private:
		std::string s;
	};

	class Exception_IndexError :private Exception {
		public:	Exception_IndexError(int given, int size) :	Exception("Index out of range "){
			stringstream ss;
			ss << "Exception : Index out of range (" << given << "/" << size<< ")" << endl;
			cerr << ss.str() << endl;
		}
	};

	class Exception_ListEmpty :private Exception {
		public:	Exception_ListEmpty() :	Exception("Operation cannot succeed. List is empty !"){
		}
	};

	class Exception_ItemNotFound :private Exception {
		public:	Exception_ItemNotFound() :	Exception("Unknown error : List index in range, but item not found !?"){
		}
	};

	class Exception_IterError :private Exception {
		public:	Exception_IterError() :	Exception("prev() and next() cannot be called when no starting item was chosen, with list[0]."){
		}
	};

};

/** name operator[]
	 * Field operator for lists. Searches for an item with the index itm and
	 * returns objects.
	 * Can handle negative indices. Iterations start from the beginning or the
	 * end of a list, depending if the index was larger or smaller than length()/2.
	 *
	 * \param itm: index of the item which is searched for.
	 * \return Item that has the index itm.
	 */
template <class Type>
Type & myList<Type>::operator[](const int itm){
	return *index_2_iter(itm);
}

/** name index_2_iter
 * Searches for an item with the index given as parameter and
 * returns its iterator.
 * Can handle negative indices. Iterations start from the beginning or the
 * end of a list, depending if the index was larger or smaller than length()/2.
 *
 * \param index: index of the item which is searched for.
 * \return Iterator for the element with the given index.
 */
template <class Type>
typename myList<Type>::iterator myList<Type>::index_2_iter(const int index){
	typename myList<Type>::iterator iter;

	if (index < 0 ){ // items can be called with negativ numbers, which means `from the end of a list`
		if ( (size_t)(-index ) > length() ) throw Exception_IndexError(index, length());//INDEXERROR(index, length());
		return index_2_iter(length()+index);
	}
	else { // index >= 0
		if ( (size_t)index > length()-1 ) throw Exception_IndexError(index, length());//INDEXERROR(index, length());

		if ( (size_t)index <= length()/2){ 	// start from the beginning of the list
			int cnt = 0;
			for (iter = this->begin(); iter != this->end(); iter++, cnt++){ // iterate through the list
				if ( cnt == index ) {set_index(iter, index); return iter;}
				//cnt++;							 // and count items
			}
		}
		else { 					// start from the ending of the list
			int cnt = length();
			for (iter = this->end(); iter != this->begin(); iter--){ // iterate through the list
				if ( cnt == index ) {set_index(iter, index); return iter;}
				cnt--;
			}
		}
	}

	if (length() == 0) throw Exception_ListEmpty();

	throw Exception_ItemNotFound();
}

/**
 * name: prnt()
 * Prints out lists to the command line.
 */
template <class Type>
void myList<Type>::prnt(const char *c) const {

	if (length() == 0) throw Exception_ListEmpty();
	typename list<Type>::const_iterator iter;

	if (c) cout << c <<" = ";
	cout << "[";
	for (iter = this->begin(); iter != this->end(); iter++){
		cout << *iter << ", ";
	}
	cout << "\b\b]" << endl;
}

/**
 * name: extend()
 * Extends a list with another list : every element in the second list is
 * appended to the list.
 * @param second : List which contains all elements to be appended.
 *
 * \todo Should be changed so it can take a constant list.
 */
template <class Type>
void myList<Type>::extend(myList<Type> second){
	for (size_t i = 0; i < second.size(); i++){
		this->append(second[i]);
	}
}

/**
 * name: erase()
 * Erases an element by index.
 * \note Erase can also be called with iterators due to implementation in base
 *		class.
 * \param index : Index of the element to be erased.
 */
template <class Type>
void myList<Type>::erase(const int index){
		list<Type>::erase(index_2_iter(index));
}

/**
 * name: myList::toArray
 * Converts a list to an array of Type.
 * \note The array is created via new, the calling instance is responsible for
 *		freeing the space.
 * @return Pointer to array of items. (Type depends on the type of the list.)
 */
template <class Type>
Type* myList<Type>::toArray(){
	Type *array = new Type[this->length()];
	typename list<Type>::const_iterator iter;
	size_t i = 0;
	for (iter = this->begin(); iter != this->end(); iter++) {
		array[i++] = *iter;
	}
	return array;
}

/** next
 * \brief Iterate to the next item in the list. To use this function a starting
 *  element has to be set with list[itm].
 *
 *	This method can be used to set the internal iterator and index to the next
 *	element and returning it. Therefore these have to be already been set.<br>
 *	The increment of iterator and index is only done if the end of the list
 *	isn't already reached.
 *
 * \return Reference to the item.*/
template <class Type>
Type& myList<Type>::next(){
	if (not iterator_set) throw Exception_IterError();
	if ( last_iter != this->end() ) {last_iter++; last_index++;}
	return *last_iter;
}

/** prev
 * \brief Iterate to the previous item in the list. To use this function a starting
 *  element has to be set with list[itm].
 *
 *	This method can be used to set the internal iterator and index to the
 *	previous element and returning it. Therefore these have to be already been
 *	set.<br>
 *	The decrement of iterator and index is only done if the beginning of the
 *	list isn't already reached.
 *
 * \return Reference to the item.*/
template <class Type>
Type& myList<Type>::prev(){
	if (not iterator_set) throw Exception_IterError();
	if (last_iter != this->begin() ){last_iter--; last_index--;}
	return *last_iter;
}

/**myList<Type>::take_front()
 * Removes the first item from the list and returns its value
 * \return Value of type "Type"
 * */
template <class Type>
Type myList<Type>::take_front(){
	if (this->size() == 0 ) throw Exception("Exception: Cannot take_front(): list is empty");
	Type value = myList<Type>::front();
	myList<Type>::pop_front();		// remove the top item
	return value;					// return it's value
}

/**myList<Type>::take_back()
 * Removes the last item from the list and returns its value
 * \return Value of type "Type"
 * */
template <class Type>
Type myList<Type>::take_back(){
	Type value = myList<Type>::back();
	myList<Type>::pop_back();		// remove the top item
	return value;					// return it's value
}

/** name: myList::contains
 * Checks if an item is found in the list
 *
 * \note This method can only work properly if 'Type' has a proper operator for
 *		testing equality.
 *
 * \param item: item of Type of the list
 * \return True if an equal item is found, false otherwise.
 */
template <class Type>
bool myList<Type>::contains(const Type & item) const {
	typename list<Type>::const_iterator iter;
	for (iter = this->begin(); iter != this->end(); iter++)
		if (item==*iter) return true;
	return false;
}

template <class Type>
/**
 * name: myList<Type>::at()
 * Gives a reference to an item stored at index 'index'
 * @param index : The index we're interested in.
 * @return Reference to the corresponding item.
 */
Type& myList<Type>::at(size_t index){
	typename list<Type>::iterator it = this->begin();
	std::advance(it, index);

	return *it;
}

#undef print
#undef INDEXERROR
#endif
