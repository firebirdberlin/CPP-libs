//		./cmdline.hpp
//
//		Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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

#ifndef __CMDLINE_HPP__
#define __CMDLINE_HPP__
// Serves functions for formatted output

#include <cstdlib>
#include <queue>
#include <iostream>
#include <ios>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include "lists.h"
#include "mystring.hpp"


using namespace std;

// print the pair variable name = value to stdout
#ifndef TELL
//#define DYNVAR(x) ##x;
#define TELL(x) cout << " - " << #x <<" = " << x << endl;
#endif

//#ifndef print
//#ifdef DEBUG
//	#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
//#else
//	#define print(x) (cout << x << endl)
//#endif
//#endif

namespace mylibs{
/**
 * \namespace cmdline
 * Functions to print out formatted output on the bash and to execute
 * bash commands.
 */
namespace cmdline {

int columns();
void newline();
void section(const char *heading=NULL);
void section(const string heading);
void subsection(const char *heading=NULL);
void subsection(const string heading);
void line(const char ch='-');

bool ok(int spaces=-1);
bool no(int spaces=-1);
void msg(const char *msg, ostream &s = std::cout);
void msg(const string mesg, ostream &s = std::cout);

void emsg(const char *mesg);
void emsg(const string mesg);

void warning(const char *mesg);
void warning(const string mesg);
void okmsg(const char *mesg);
void okmsg(const string mesg);
void exit(const char* message);
void exit(const string message);

void run_command(const string cmd);
void run_command(const char *cmd, const char *working_dir=NULL);


template<class T>
/** \fn template<class T> void info(vector<T>, const char *)
 * \brief Print values of a std::vector to standard output.
 *
 * This function will print the content of a std::vector to the standard output.
 * The values will in brackets '[]' and separated by ' '.
 * It is also possible to pass a name that then apears with a '=' before the
 * list of the values.
 *
 * \note The '<<'-operator needs to be defined for the template class. Otherwise
 *		this function can not be used.
 *
 * \param v: Vector which sould be printed to standard output.
 * \param name: C-string, that contains the name that should be used for the
 *				output vector.
 */
void info(vector<T> v, const char *name=NULL){
	if (v.size() == 0) return; // do nothing if vector is empty
	if (name) cout << name << " = ";
	cout << "[ ";
	typename vector<T>::iterator i = v.begin();
	for (i = v.begin(); i != v.end(); ++i){
		if (i != v.begin() ) cout << ", ";
		cout << *i;
	}
	cout << " ]" << endl;
}

} // end of namespace cmdline
} // end of namespace mylibs
#endif
