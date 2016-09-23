//      myexception.cpp
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


#include "myexception.hpp"

using namespace std;

myexception::myexception(std::string str) throw(){
	this->s =str;
}

myexception::myexception(std::string str, int id) throw(){
	stringstream strout;
	strout << id << " " << str;
	this->s = strout.str();
}

myexception::~myexception() throw(){

}
const char* myexception::what() const throw(){
	return s.c_str();
}
