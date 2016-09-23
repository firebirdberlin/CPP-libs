// myio.hpp
//
// Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.


#ifndef MYIO_HPP
#define MYIO_HPP

#include <vector>
#include <list>

#include "myexception.hpp"
#include "mystring.hpp"
#include "cmdline.hpp"
// for mkdir
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// for cp
#include <fcntl.h>
#include <unistd.h>

namespace mylibs{
namespace IO{
	// recursive mkdir
	bool mkdir(mystring dir, mode_t mode = 0740);
	bool rm(const char *filename);
	int cp(const char *from, const char *to);
	int cp(const mystring &from, const mystring& to);

	size_t file_size(const char *filename);
	bool file_exists(const char *filename, int mode=0);
	bool is_executable(const char *cmd);
	const string get_config_dir(const char *name_of_program);
}
}

#endif /* MYIO_HPP */
