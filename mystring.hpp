//		./mystring.hpp
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

// Komfort-String-Funktionen

#ifndef MYSTRING_HPP
#define MYSTRING_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <typeinfo>
#include <iomanip>
#include <limits.h>
#include <string.h>
#include "lists.h"
#include <vector>
#include <cctype>
/** \file mystring.hpp
 *	\brief Declares mystring + exception and toString.
 *
 *	This file declares the mystring-class (and the code for exception
 * 	handling) and defines the methods which use templates. The
 *	toString-function is also defined here.
 */

using namespace std;

enum mystring_exceptions {CONVERSION_FAILURE};

class mystring_exception : public exception {

	mystring_exceptions id;

	public:
		mystring_exception(mystring_exceptions i) : id(i) {}
		virtual const char* what() const throw(){
			switch (id){
				case CONVERSION_FAILURE:
					return "mystring: Conversion Failure."; break;
				default:
					return "mystring: Unclarified Exception."; break;
			}
		};

};

class mystring;

typedef myList<mystring> myStringList;//!< Defines a list of mystrings

/** \class mystring
 * \brief 	Inherits string, but has more methods that make life with strings
 * 			easier.
 *
 *	This class is derived from the standard string. It has additional methods
 *	for handling of path, filenames and extensions, for string-operations (e.g.
 *	slicing, striping and splitting) and for string-conversions (via << and >>).
 */
class mystring : public string {
public:
	// constructors are not inherited and must be defined
	mystring(const char *c) : string(c){} // create mystrings from c-strings.
	mystring(const string s) : string(s){}  // create mystrings from cpp-strings
	mystring(){*this = string();}

	// assignment operators are not inherited
	string operator=(const string a){
		return string::operator=(a);
	}

	string operator=(const char * const a){
		return string::operator=(a);
	}

	bool startswith(const string a) const{
		return (find(a) == 0);
	}

	bool endswith(const string a) const {
		if (a.length() > length()) return false;
		return (rfind(a) == length() - a.length());
	}

	bool startswith(const char a) const {
		return (find(a) == 0);
	}

	bool endswith(const char a) const {
		return (rfind(a) == length() - 1);
	}

	bool contains(const char a) const {
		return (find(a) != string::npos);
	}

	bool contains(const char * const a) const{
		return (find(a) != string::npos);
	}

	bool contains(const mystring a) const{
		return (find(a) != string::npos);
	}

	string slice(int left, int right) const;

	mystring operator()(const int left=0, const int right = INT_MAX) const{
		return slice(left, right);
	}

	bool is_comment(const char commentchar);
	bool isdigit() const;
	bool isalpha() const;
	bool isalnum() const;

	mystring replace(const char * const from, const char * const to);

	mystring fill(const unsigned int count, const char fillchar=' ' , const bool sign = false);
	mystring rfill(const unsigned int count, const char fillchar=' ', const bool sign = false);
	mystring lfill(const unsigned int count, const char fillchar=' ', const bool sign = false);

	bool	 file_is_absolute() const;
	bool	 file_absolute() const {return file_is_absolute();};
	mystring file_base(bool strip_path=false) const;
	mystring file_ext(const int max_length = -1) const;
	mystring file_new_ext(const char* new_ext);
	mystring file_new_ext(const char* new_ext) const;
	mystring file_strip_ext() const;
	mystring file_strip_path() const {return path_strip();};
	mystring file_replace_ext(const char* old_ext,const char* new_ext);
	mystring file_lower_ext();
	mystring file_path() const;
	bool file_exists() const;

	mystring path_join(const mystring itm, const char path_delimiter='/') const;
	mystring path_join(const string itm, const char path_delimiter='/') const;
	mystring path_join(const char * const itm, const char path_delimiter='/') const;
	mystring path_strip() const;

	mystring &strip();
	mystring &strip(const char * const c);
	mystring &lstrip();
	mystring &lstrip(const char sc);
	mystring &rstrip();
	mystring &rstrip(const char sc);

	mystring& lower();
	mystring& upper();

	myStringList split(const char * const c) const;
	myStringList split() const;
	myStringList split(const char c) const;

	vector<mystring> splitv(const char * const c) const;
	vector<mystring> splitv() const;
	vector<mystring> splitv(const char c) const;

	template <class T>
	void extract(vector<T> &res);

	template <class T>
	void extract(T &res);

	template <class T>
	vector<T> extract();

	double ToDouble() const {
		return atof(c_str());
	}

	int ToInt() const {
		return atoi(c_str());
	}


	/** mystring::operator>>(TYPE &value)
	  *  Operator >> to use conversion from (this) mystring into any Type.
	  * \param value: the value to be extracted
	  * \return Reference to value
	  */
	template<class Type>
	Type& operator>>(Type &value) const{
		stringstream ss;
		ss << *this;
		if ((ss >> value).fail()) throw mystring_exception(CONVERSION_FAILURE);
		return value;
	}

	mystring& operator>>(mystring &value) const{
//		value = this->strip();
		value = *this;
		return value;
	}

	template<class Type>
	mystring& operator<<(Type &value) __attribute__ ((deprecated)); //! \todo Remove when no longer utilized.

	template<class Type>
	mystring& operator<<(const Type &value);

	mystring operator<<(const string a){
		return string::operator=(a);
	}
};

/** name: toString
 *	Converts Types to a mystring. Floating point numbers are converted with a
 *	fixed number of digits after the floating point.
 * \param wert: The value that should be converted to a mystring.
 * \param width: How many digits after the floating point should be printed
 *				(into the mystring) for floating point numbers.<br>
 *				Default value is 6.<br>
 *				Not used for types other than float and double.
 * \return The value as a mystring.
 */
template <typename Typ>
mystring toString(const Typ wert, const int width=6) {
	ostringstream strout; // Unser Ausgabe-Stream
	mystring str;		 // Ein String-Objekt

	if (typeid(wert) == typeid( (float) 0.1)
	 || (typeid(wert) == typeid( (double) 0.1))){
//		cout << "Zahl: " << ::std::fixed << wert << endl;
		strout.precision(width);
		strout << ::std::fixed << wert;
	}
	else
	if(	   (typeid(wert) == typeid( (int)    1))
		|| (typeid(wert) == typeid( (size_t) 1))){
//		strout.fill('0');
		strout.width(width);
		strout << right << wert;
	}
	else strout << wert;			// Zahl auf Ausgabe-Stream ausgeben

	str = strout.str();		// Streaminhalt an String-Variable zuweisen
	return str;				// String zurückgeben
}

/** mystring::operator<<(TYPE &value)
 *  Operator << to use conversion from any Type into mystring.
 * \param value: the value to be converted
 * \return Reference to *this
 */
template<class Type>
mystring& mystring::operator<<(Type &value) {
	stringstream ss;
	if ((ss << value).fail()) throw mystring_exception(CONVERSION_FAILURE);
	ss >> *this;
	return *this;
}

/** mystring::operator<<(TYPE &value)
 *  Operator << to use conversion from any Type into mystring.
 * \param value: the value to be converted
 * \return Reference to *this
 */
template<class Type>
mystring& mystring::operator<<(const Type &value){
	stringstream ss;
	if ((ss << value).fail()) throw mystring_exception(CONVERSION_FAILURE);
	ss >> *this;
	return *this;
}

/** \fn void mystring::extract(vector<T> &)
 * (Stefan Fruhner - 03.02.2012 10:10:24 CET)
 *	Extracts items of the template type T from a C-string, ignoring all chars
 * 	which are of incorrect type.
 *
 *	Example how to use this method:
 * \verbatim
vector<double> v;
mystring test("{1, 2 , 3},crap{45.5,6,7,8}");
test.extract(v);
\endverbatim
 *	This would result in 1 2 3 45.5 6 7 8 as contents of v.
 *
 * @param res : Reference to a vector (e.g of the type double) where to store
 *				the found values.
 **/
template <class T>
void mystring::extract(vector<T> &res) {
	stringstream s(*this);
	res.clear();
	T val;
	char dummy;
	while (not s.eof()){
		s >> val;
		if ( s.fail() ){
			s.clear();
			s.get(dummy);
			continue;
		}
		res.push_back(val);
	}
}

/** \fn void mystring::extract(T &)
 * (Stefan Fruhner - 03.02.2012 10:10:24 CET)
 *	Extracts the first item of the template type T from a C-string,
 * 	ignoring all chars which are of incorrect type.
 *
 *	Example how to use this method:
 * \verbatim
vector v;
mystring test("{1, 2 , 3},crap{45.5,6,7,8}");
test.extract(v);
\endverbatim
 *	This would result in 1 as content of v.
 *
 * @param res : Reference to a variable (e.g of the type double) where to store
 *				the first valid value.
 **/
template <class T>
void mystring::extract(T &res) {
	stringstream s(*this);
	T val;
	char dummy;
	while (not s.eof()){
		s >> val;
		if ( s.fail() ){
			s.clear();
			s.get(dummy);
			continue;
		}
		res = val;
		return;
	}
}

template <class T>
vector<T> mystring::extract(){
	vector<T> data;
	extract(data);
	return data;
}


#endif
