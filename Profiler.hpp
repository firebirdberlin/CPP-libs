// Profiler.hpp
//
// Copyright 2013 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <cstdio>
#include <iomanip>
#include <cmath>
#include <string.h>
#include <omp.h>


class Profiler
{
	public:
		Profiler(size_t total_items=1);
		void reset(size_t total_items=1);
		void info();
		void finalize();

	private:
		int total;
		int progress;
		int tstart;
		int tend;
		/* add your private declarations */
};

#endif /* PROFILER_HPP */
