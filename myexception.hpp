//      myexception.h
//
//      Copyright 2010 Stefan Fruhner <stefan@kandinsky>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#ifndef MYEXCEPTION_HPP
#define MYEXCEPTION_HPP

#include <exception>
#include <string>
#include <sstream>
#include "mystring.hpp"

#ifndef EXCEPTION_ID
#define EXCEPTION_ID (toString(__FILE__,0) +":"+ toString(__FUNCTION__,0) + "(..):" + toString(__LINE__,0) + ": ")
#endif

class myexception : public std::exception {
	public:
		myexception(std::string str) throw();
		myexception(std::string str, int id) throw();

		virtual ~myexception() throw();
		virtual const char* what() const throw();

	private:
		std::string s;
};

class myexception_UnknownError : public myexception {
	public:
		myexception_UnknownError(std::string id = std::string()) :
			myexception(id+"Error unknown, but severe enough to be thrown."){}
};

#endif /* MYEXCEPTION_HPP */
