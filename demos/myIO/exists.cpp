//      exists.cpp
//
//      Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#include <iostream>
#include <mylibs/myio.hpp>
#include <mylibs/cmdline.hpp>

using namespace mylibs;
using namespace mylibs::cmdline;

int main(int argc, char **argv){

	cout << "Config dir of geany = ";
	std::string geanyconf = IO::get_config_dir("geany");
	cout << "'" <<geanyconf << "'" <<endl;

	cout << "Does muhplot work ?" << endl;
	if (IO::is_executable("muhplot")) 	ok();
	else 								no();

	cout << "Does gnuplot work ?" << endl;
	if (IO::is_executable("gnuplot")) 	ok();
	else 								no();

	return 0;
}

